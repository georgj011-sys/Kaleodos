#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Params.h"
#include "Presets.h"
#include <cstring>

KaleidoProcessor::KaleidoProcessor()
    : AudioProcessor (BusesProperties().withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "PARAMS", Params::createLayout())
{
    noteStack.ensureStorageAllocated (128);
    physicalHeld.ensureStorageAllocated (128);
    arpNotes.ensureStorageAllocated (128);
}

void KaleidoProcessor::prepareToPlay (double sr, int samplesPerBlock)
{
    sampleRate = sr;
    for (auto& voice : voices)
        voice.prepare (sr);

    fx.prepare (sr, samplesPerBlock);

    noteStack.clearQuick();
    physicalHeld.clearQuick();
    arpNotes.clearQuick();
    pendingCount = 0;
    arpRunning = false;
    lastNote = -1;
    clock = 0.0;
}

bool KaleidoProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}

void KaleidoProcessor::readParams (VoiceParams& vp, FxParams& fp) const
{
    for (int i = 0; i < 3; ++i)
    {
        auto& o = vp.osc[i];
        o.type   = vi (Params::osc (i, "type"));
        o.shape  = v (Params::osc (i, "shape"));
        o.color  = v (Params::osc (i, "color"));
        o.octave = v (Params::osc (i, "octave"));
        o.semi   = v (Params::osc (i, "semi"));
        o.fine   = v (Params::osc (i, "fine"));
        o.level  = v (Params::osc (i, "level"));
        o.pan    = v (Params::osc (i, "pan"));
    }

    vp.xfm        = v ("xfm");
    vp.ring       = v ("ring");
    vp.filterType = vi ("filter_type");
    vp.cutoff     = v ("cutoff");
    vp.reso       = v ("reso");
    vp.envAmt     = v ("fenv_amt");
    vp.keytrack   = v ("keytrack");
    vp.drive      = v ("filter_drive");
    vp.ampEnv     = { v ("amp_attack"), v ("amp_decay"), v ("amp_sustain"), v ("amp_release") };
    vp.fltEnv     = { v ("flt_attack"), v ("flt_decay"), v ("flt_sustain"), v ("flt_release") };
    vp.velSens    = v ("vel_sens");
    vp.modAttack  = v ("menv_attack");
    vp.modDecay   = v ("menv_decay");
    vp.modLoop    = vi ("menv_mode") == 1;
    vp.lfoWave[0] = vi ("lfo1_wave");
    vp.lfoRate[0] = v ("lfo1_rate");
    vp.lfoWave[1] = vi ("lfo2_wave");
    vp.lfoRate[1] = v ("lfo2_rate");

    for (int i = 0; i < 4; ++i)
    {
        vp.slots[i].src = vi (Params::slot (i, "src"));
        vp.slots[i].dst = vi (Params::slot (i, "dst"));
        vp.slots[i].amt = v (Params::slot (i, "amt"));
    }

    vp.gravity    = v ("gravity");
    vp.entropy    = v ("entropy");
    vp.drift      = v ("drift");
    vp.orbitRate  = v ("orbit_rate");
    vp.orbitDepth = v ("orbit_depth");
    vp.spread     = v ("spread");
    vp.glide      = v ("glide");

    fp.distType      = vi ("dist_type");
    fp.distAmt       = v ("dist_amt");
    fp.tapeWow       = v ("tape_wow");
    fp.tapeFlutter   = v ("tape_flutter");
    fp.tapeAge       = v ("tape_age");
    fp.chorusRate    = v ("chorus_rate");
    fp.chorusDepth   = v ("chorus_depth");
    fp.chorusMix     = v ("chorus_mix");
    fp.phaserRate    = v ("phaser_rate");
    fp.phaserDepth   = v ("phaser_depth");
    fp.phaserMix     = v ("phaser_mix");
    fp.delaySync     = vi ("delay_sync");
    fp.delayTime     = v ("delay_time");
    fp.delayFb       = v ("delay_fb");
    fp.delayTone     = v ("delay_tone");
    fp.delayMix      = v ("delay_mix");
    fp.cloudSize     = v ("cloud_size");
    fp.cloudDensity  = v ("cloud_density");
    fp.cloudSpray    = v ("cloud_spray");
    fp.cloudMix      = v ("cloud_mix");
    fp.reverbSize    = v ("reverb_size");
    fp.reverbShimmer = v ("reverb_shimmer");
    fp.reverbMix     = v ("reverb_mix");
    fp.gateRate      = vi ("gate_rate");
    fp.gatePattern   = vi ("gate_pattern");
    fp.gateDepth     = v ("gate_depth");
    fp.panRate       = v ("pan_rate");
    fp.panDepth      = v ("pan_depth");
    fp.haasMs        = v ("haas");
    fp.width         = v ("width");
    fp.master        = v ("master");
}

void KaleidoProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    juce::ScopedNoDenormals noDenormals;
    const int numSamples = buffer.getNumSamples();

    buffer.clear();
    keyboardState.processNextMidiBuffer (midi, 0, numSamples, true);

    VoiceParams vp;
    FxParams fp;
    readParams (vp, fp);

    if (auto* ph = getPlayHead())
    {
        if (auto pos = ph->getPosition())
        {
            if (auto b = pos->getBpm())
                bpm = juce::jlimit (20.0, 400.0, *b);

            if (pos->getIsPlaying())
            {
                if (auto q = pos->getPpqPosition())
                {
                    fp.ppq = *q;
                    fp.hasPpq = true;
                }
            }
        }
    }
    fp.bpm = bpm;

    mode = juce::jlimit (0, 2, vi ("mode"));
    if (mode != lastMode)
    {
        if (lastMode != -1)
        {
            engineReleaseAll();
            for (size_t i = 1; i < voices.size(); ++i)
                voices[i].kill();
        }
        lastMode = mode;
    }

    blockStart = clock;
    arpOn = vi ("arp_on") == 1;
    latch = vi ("arp_latch") == 1;

    if (arpOn != arpWasOn)
    {
        stopArp();
        engineReleaseAll();

        if (arpOn)
        {
            arpNotes = physicalHeld;
            if (! arpNotes.isEmpty())
            {
                arpRunning = true;
                arpStep = arpNoteIndex = walkPos = 0;
                arpGrid = arpNext = blockStart;
            }
        }
        arpWasOn = arpOn;
    }

    if (arpOn && ! latch && physicalHeld.isEmpty() && ! arpNotes.isEmpty())
    {
        arpNotes.clearQuick();
        stopArp();
    }

    if (buffer.getNumChannels() < 2)
    {
        clock += numSamples;
        return;
    }

    outL = buffer.getWritePointer (0);
    outR = buffer.getWritePointer (1);

    int cur = 0;
    for (const auto metadata : midi)
    {
        const int t = juce::jlimit (0, numSamples, metadata.samplePosition);
        advanceTo (cur, t, vp);
        handleMidi (metadata.getMessage(), blockStart + t, vp);
    }
    advanceTo (cur, numSamples, vp);

    clock += numSamples;
    fx.process (buffer, fp);
}

void KaleidoProcessor::advanceTo (int& cur, int target, const VoiceParams& vp)
{
    const double absTarget = blockStart + target;

    for (int guard = 0; guard < 4096; ++guard)
    {
        double next = 1.0e300;
        int which = -2;

        if (arpRunning)
        {
            next = arpNext;
            which = -1;
        }

        for (int k = 0; k < pendingCount; ++k)
        {
            if (pending[(size_t) k].time < next)
            {
                next = pending[(size_t) k].time;
                which = k;
            }
        }

        if (which == -2 || next >= absTarget)
            break;

        const int off = juce::jlimit (cur, target, (int) std::floor (next - blockStart));
        renderVoices (cur, off - cur, vp);
        cur = off;

        if (which == -1)
        {
            doArpStep (arpNext, vp);
        }
        else
        {
            const Pending ev = pending[(size_t) which];
            pending[(size_t) which] = pending[(size_t) (--pendingCount)];

            if (ev.on) engineNoteOn (ev.note, ev.vel, vp);
            else       engineNoteOff (ev.note);
        }
    }

    renderVoices (cur, target - cur, vp);
    cur = target;
}

void KaleidoProcessor::renderVoices (int start, int num, const VoiceParams& vp)
{
    if (num <= 0)
        return;

    const float bendSemis = bend * (float) vi ("bend");
    const int count = mode == Poly ? (int) voices.size() : 1;

    for (int i = 0; i < count; ++i)
        voices[(size_t) i].render (outL, outR, start, num, vp, bendSemis, modWheel);
}

void KaleidoProcessor::handleMidi (const juce::MidiMessage& msg, double absTime, const VoiceParams& vp)
{
    if (msg.isNoteOn())
    {
        const int n = msg.getNoteNumber();

        if (arpOn)
        {
            if (latch && physicalHeld.isEmpty())
                arpNotes.clearQuick();

            physicalHeld.addIfNotAlreadyThere (n);
            if (! arpNotes.contains (n) && arpNotes.size() < Arp::maxHeld)
                arpNotes.add (n);

            if (! arpRunning && ! arpNotes.isEmpty())
            {
                arpRunning = true;
                arpStep = arpNoteIndex = walkPos = 0;
                arpGrid = arpNext = absTime;
            }
        }
        else
        {
            physicalHeld.addIfNotAlreadyThere (n);
            engineNoteOn (n, msg.getFloatVelocity(), vp);
        }
    }
    else if (msg.isNoteOff())
    {
        const int n = msg.getNoteNumber();
        physicalHeld.removeAllInstancesOf (n);

        if (arpOn)
        {
            if (! latch)
            {
                arpNotes.removeAllInstancesOf (n);
                if (arpNotes.isEmpty())
                    stopArp();
            }
        }
        else
        {
            engineNoteOff (n);
        }
    }
    else if (msg.isPitchWheel())
    {
        bend = ((float) msg.getPitchWheelValue() - 8192.0f) / 8192.0f;
    }
    else if (msg.isController() && msg.getControllerNumber() == 1)
    {
        modWheel = (float) msg.getControllerValue() / 127.0f;
    }
    else if (msg.isAllNotesOff() || msg.isAllSoundOff())
    {
        physicalHeld.clearQuick();
        arpNotes.clearQuick();
        stopArp();
        engineReleaseAll();
    }
}

//==============================================================================
void KaleidoProcessor::doArpStep (double time, const VoiceParams& vp)
{
    if (arpNotes.isEmpty())
    {
        stopArp();
        return;
    }

    Arp::NoteList sorted, played;
    for (int i = 0; i < arpNotes.size() && i < Arp::maxHeld; ++i)
    {
        played.add (arpNotes[i]);
        sorted.add (arpNotes[i]);
    }
    std::sort (sorted.n, sorted.n + sorted.count);

    const double stepLen = Params::arpRateBeats (vi ("arp_rate")) * 60.0 / bpm * sampleRate;
    const char* pattern = Params::rhythmPattern (vi ("arp_rhythm"));
    const int len = juce::jmax (1, (int) std::strlen (pattern));
    const int idx = arpStep % len;
    const char c = pattern[idx];
    arpDisplayStep.store (idx);

    if (c != '.')
    {
        Arp::NoteList result;
        const bool ok = Arp::pick (vi ("arp_mode"), sorted, played, vi ("arp_octaves"), arpNoteIndex, walkPos,
                                   [this] (int m) { return arpRng.nextInt (juce::jmax (1, m)); }, result);
        if (ok)
        {
            ++arpNoteIndex;

            const float vel = c == 'X' ? 1.0f : 0.78f;
            const int shift = arpRng.nextFloat() < v ("arp_jump") ? (arpRng.nextBool() ? 12 : -12) : 0;

            int count = 1;
            if (arpRng.nextFloat() < v ("arp_ratchet"))
                count = arpRng.nextFloat() < 0.35f ? 3 : 2;

            const double sub = stepLen / count;
            const double gateLen = juce::jmax (0.002 * sampleRate, (double) v ("arp_gate") * sub);

            for (int r = 0; r < count; ++r)
            {
                const double tOn = time + r * sub;
                for (int j = 0; j < result.count; ++j)
                {
                    const int note = juce::jlimit (0, 127, result.n[j] + shift);
                    if (r == 0) engineNoteOn (note, vel, vp);
                    else        schedule (tOn, note, vel, true);
                    schedule (tOn + gateLen, note, 0.0f, false);
                }
            }
        }
    }

    ++arpStep;
    arpGrid += stepLen;
    arpNext = arpGrid + ((arpStep % 2 == 1) ? (double) v ("arp_swing") * 0.33 * stepLen : 0.0);
}

void KaleidoProcessor::schedule (double time, int note, float vel, bool on)
{
    if (pendingCount < (int) pending.size())
        pending[(size_t) (pendingCount++)] = { time, note, vel, on };
}

void KaleidoProcessor::stopArp()
{
    arpRunning = false;

    for (int k = 0; k < pendingCount; ++k)
        if (! pending[(size_t) k].on)
            engineNoteOff (pending[(size_t) k].note);

    pendingCount = 0;
    arpDisplayStep.store (-1);
}

//==============================================================================
void KaleidoProcessor::engineNoteOn (int note, float velocity, const VoiceParams& vp)
{
    static constexpr float spreadSeq[8] = { -1.0f, 1.0f, -0.5f, 0.5f, -0.75f, 0.75f, -0.25f, 0.25f };

    noteStack.removeAllInstancesOf (note);
    noteStack.add (note);

    if (mode == Poly)
    {
        KaleidoVoice* target = nullptr;

        for (auto& voice : voices)
            if (voice.isActive() && voice.getNote() == note)
                target = &voice;

        if (target == nullptr)
            for (auto& voice : voices)
                if (! voice.isActive()) { target = &voice; break; }

        if (target == nullptr)
        {
            for (auto& voice : voices)
            {
                if (target == nullptr) { target = &voice; continue; }
                const bool better = (! voice.isGateOn() && target->isGateOn())
                                 || (voice.isGateOn() == target->isGateOn() && voice.age < target->age);
                if (better)
                    target = &voice;
            }
        }

        const float from = (vp.glide > 0.0005f && lastNote >= 0) ? (float) lastNote : (float) note;
        target->start (note, velocity, from, vp, ++seed, spreadSeq[(spreadCounter++) & 7]);
        target->age = ++ageCounter;
    }
    else
    {
        auto& voice = voices[0];

        if (mode == Legato && noteStack.size() > 1 && voice.isActive() && voice.isGateOn())
        {
            voice.glideTo (note);
        }
        else
        {
            float from = (float) note;
            if (mode == Mono)
                from = voice.isActive() ? voice.getPitch() : (lastNote >= 0 ? (float) lastNote : (float) note);
            voice.start (note, velocity, from, vp, ++seed, 0.0f);
        }
    }

    lastNote = note;
}

void KaleidoProcessor::engineNoteOff (int note)
{
    const bool wasTop = ! noteStack.isEmpty() && noteStack.getLast() == note;
    noteStack.removeAllInstancesOf (note);

    if (mode == Poly)
    {
        for (auto& voice : voices)
            if (voice.isGateOn() && voice.getNote() == note)
                voice.release();
        return;
    }

    auto& voice = voices[0];

    if (noteStack.isEmpty())
    {
        if (voice.isGateOn())
            voice.release();
    }
    else if (wasTop && voice.isGateOn())
    {
        const int previous = noteStack.getLast();
        voice.glideTo (previous);
        lastNote = previous;
    }
}

void KaleidoProcessor::engineReleaseAll()
{
    for (auto& voice : voices)
        if (voice.isGateOn())
            voice.release();

    noteStack.clearQuick();
}

//==============================================================================
juce::AudioProcessorEditor* KaleidoProcessor::createEditor()
{
    return new KaleidoEditor (*this);
}

int KaleidoProcessor::getNumPrograms()
{
    return (int) getFactoryPresets().size();
}

const juce::String KaleidoProcessor::getProgramName (int index)
{
    const auto& presets = getFactoryPresets();
    if (index >= 0 && index < (int) presets.size())
        return presets[(size_t) index].name;
    return {};
}

void KaleidoProcessor::setCurrentProgram (int index)
{
    const auto& presets = getFactoryPresets();
    if (index < 0 || index >= (int) presets.size())
        return;

    currentProgram = index;
    const auto& preset = presets[(size_t) index];

    for (auto* rawParam : getParameters())
    {
        if (auto* p = dynamic_cast<juce::RangedAudioParameter*> (rawParam))
        {
            float normalised = p->getDefaultValue();

            for (const auto& kv : preset.values)
            {
                if (kv.first == p->paramID)
                {
                    normalised = p->convertTo0to1 (kv.second);
                    break;
                }
            }

            p->setValueNotifyingHost (normalised);
        }
    }
}

void KaleidoProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    state.setProperty ("program", currentProgram, nullptr);

    if (auto xml = state.createXml())
        copyXmlToBinary (*xml, destData);
}

void KaleidoProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml (getXmlFromBinary (data, sizeInBytes));

    if (xml != nullptr && xml->hasTagName (apvts.state.getType()))
    {
        auto tree = juce::ValueTree::fromXml (*xml);
        currentProgram = (int) tree.getProperty ("program", 0);
        apvts.replaceState (tree);
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new KaleidoProcessor();
}
