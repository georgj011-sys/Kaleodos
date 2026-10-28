#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

namespace Params
{
    using juce::StringArray;

    inline const StringArray& oscTypes()
    {
        static const StringArray s { "Saw", "Pulse", "Fold", "Sync", "FM", "Formant", "Noise",
                                     "Vector", "Chip", "Swarm", "Phase dist", "Harmonic", "String" };
        return s;
    }

    inline const char* shapeName (int type)
    {
        static const char* n[] = { "Detune", "Width", "Fold", "Sync", "Index", "Formant", "Rate",
                                   "X", "Crunch", "Spread", "Warp", "Tilt", "Bright" };
        return n[juce::jlimit (0, 12, type)];
    }

    inline const char* colorName (int type)
    {
        static const char* n[] = { "Stack", "PWM", "Bias", "Blend", "Ratio", "Focus", "Smooth",
                                   "Y", "Morph", "Density", "Reso", "Hollow", "Sustain" };
        return n[juce::jlimit (0, 12, type)];
    }

    inline const StringArray& filterTypes()
    {
        static const StringArray s { "Low pass 12", "Low pass 24", "Band pass", "High pass", "Notch", "Comb", "Formant" };
        return s;
    }

    inline const StringArray& lfoWaves()
    {
        static const StringArray s { "Sine", "Triangle", "Ramp up", "Ramp down", "Square", "Sample & hold", "Drift", "Stairs" };
        return s;
    }

    inline const StringArray& modSources()
    {
        static const StringArray s { "None", "LFO 1", "LFO 2", "Mod envelope", "Velocity", "Mod wheel", "Note random", "Key position" };
        return s;
    }

    inline const StringArray& modDests()
    {
        static const StringArray s { "Pitch", "Osc 1 shape", "Osc 2 shape", "Osc 3 shape", "All colors", "Cutoff",
                                     "Resonance", "Pan", "Volume", "Cross FM", "Ring mod", "LFO 1 rate" };
        return s;
    }

    inline const StringArray& arpModes()
    {
        static const StringArray s { "Up", "Down", "Up-down", "As played", "Random", "Converge", "Diverge",
                                     "Pinky", "Thumb", "Octave bounce", "Random walk", "Chord" };
        return s;
    }

    inline const StringArray& arpRates()
    {
        static const StringArray s { "1/4", "1/8", "1/8 T", "1/16", "1/16 T", "1/32" };
        return s;
    }

    inline double arpRateBeats (int i)
    {
        static const double b[] = { 1.0, 0.5, 1.0 / 3.0, 0.25, 1.0 / 6.0, 0.125 };
        return b[juce::jlimit (0, 5, i)];
    }

    // X = accented hit, x = hit, . = rest
    inline const StringArray& rhythmNames()
    {
        static const StringArray s { "Straight", "Tresillo", "Cinquillo", "Euclid 5/16", "Euclid 7/16",
                                     "Gallop", "Offbeat", "Bossa", "Stutter", "Sparse" };
        return s;
    }

    inline const char* rhythmPattern (int i)
    {
        static const char* p[] = { "Xxxx", "X..x..x.", "X.xx.xx.", "X..x..x..x..x...", "X..x.x.x..x.x.x.",
                                   "X.xx", ".x", "X..x..x...x..x..", "Xxx.xx.x", "X...x.x...x....." };
        return p[juce::jlimit (0, 9, i)];
    }

    inline const StringArray& driveTypes()
    {
        static const StringArray s { "Soft", "Hard", "Fold", "Crush", "Rectify" };
        return s;
    }

    inline const StringArray& delaySyncs()
    {
        static const StringArray s { "Free", "1/4", "1/8", "1/8 dotted", "1/16", "1/4 T" };
        return s;
    }

    inline double delaySyncBeats (int i)
    {
        static const double b[] = { 0.0, 1.0, 0.5, 0.75, 0.25, 2.0 / 3.0 };
        return b[juce::jlimit (0, 5, i)];
    }

    inline const StringArray& gateRates()
    {
        static const StringArray s { "1/8", "1/16", "1/32", "1/8 T", "1/16 T" };
        return s;
    }

    inline double gateRateBeats (int i)
    {
        static const double b[] = { 0.5, 0.25, 0.125, 1.0 / 3.0, 1.0 / 6.0 };
        return b[juce::jlimit (0, 4, i)];
    }

    inline const StringArray& gatePatternNames()
    {
        static const StringArray s { "Pulse", "Trance", "Offbeat", "Stutter", "Broken", "Triplet" };
        return s;
    }

    inline const char* gatePattern (int i)
    {
        static const char* p[] = { "xxxxxxxxxxxxxxxx", "xx.xx.x.xx.xx.x.", ".x.x.x.x.x.x.x.x",
                                   "xxxx....xx..x...", "x.xx..x.x..xx.x.", "x..x..x..x..x..x" };
        return p[juce::jlimit (0, 5, i)];
    }

    inline juce::String osc (int index, const char* name)   { return "o" + juce::String (index + 1) + "_" + name; }
    inline juce::String slot (int index, const char* name)  { return "m" + juce::String (index + 1) + "_" + name; }

    inline juce::AudioProcessorValueTreeState::ParameterLayout createLayout()
    {
        using namespace juce;
        AudioProcessorValueTreeState::ParameterLayout layout;

        auto skewed = [] (float lo, float hi, float centre)
        {
            NormalisableRange<float> r (lo, hi);
            r.setSkewForCentre (centre);
            return r;
        };

        auto addFloat = [&layout] (const String& id, const String& name,
                                   NormalisableRange<float> range, float def, const String& label = {})
        {
            layout.add (std::make_unique<AudioParameterFloat> (ParameterID { id, 1 }, name, range, def,
                                                               AudioParameterFloatAttributes().withLabel (label)));
        };

        auto addInt = [&layout] (const String& id, const String& name, int lo, int hi, int def)
        {
            layout.add (std::make_unique<AudioParameterInt> (ParameterID { id, 1 }, name, lo, hi, def));
        };

        auto addChoice = [&layout] (const String& id, const String& name, const StringArray& items, int def)
        {
            layout.add (std::make_unique<AudioParameterChoice> (ParameterID { id, 1 }, name, items, def));
        };

        const NormalisableRange<float> unit (0.0f, 1.0f);
        const NormalisableRange<float> bipolar (-1.0f, 1.0f);
        const auto timeRange = skewed (0.001f, 12.0f, 0.6f);
        const auto rateRange = skewed (0.02f, 30.0f, 2.0f);
        const StringArray onOff { "Off", "On" };

        // Oscillators
        const int   defType[3]  = { 0, 1, 11 };
        const float defLevel[3] = { 0.8f, 0.0f, 0.0f };

        for (int i = 0; i < 3; ++i)
        {
            const String n = "Osc " + String (i + 1) + " ";
            addChoice (osc (i, "type"), n + "Model", oscTypes(), defType[i]);
            addFloat (osc (i, "shape"), n + "Shape", unit, 0.25f);
            addFloat (osc (i, "color"), n + "Color", unit, 0.4f);
            addInt (osc (i, "octave"), n + "Octave", -3, 3, 0);
            addInt (osc (i, "semi"), n + "Semitones", -12, 12, 0);
            addFloat (osc (i, "fine"), n + "Fine", NormalisableRange<float> (-100.0f, 100.0f), 0.0f, "ct");
            addFloat (osc (i, "level"), n + "Level", unit, defLevel[i]);
            addFloat (osc (i, "pan"), n + "Pan", bipolar, 0.0f);
        }

        addFloat ("xfm", "Cross FM (3 to 1)", unit, 0.0f);
        addFloat ("ring", "Ring Mod (1 x 2)", unit, 0.0f);

        // Filter
        addChoice ("filter_type", "Filter Type", filterTypes(), 1);
        addFloat ("cutoff", "Cutoff", skewed (20.0f, 20000.0f, 1000.0f), 4000.0f, "Hz");
        addFloat ("reso", "Resonance", unit, 0.2f);
        addFloat ("fenv_amt", "Filter Env Amount", bipolar, 0.3f);
        addFloat ("keytrack", "Key Tracking", unit, 0.5f);
        addFloat ("filter_drive", "Filter Drive", unit, 0.1f);

        // Envelopes
        addFloat ("amp_attack", "Amp Attack", timeRange, 0.005f, "s");
        addFloat ("amp_decay", "Amp Decay", timeRange, 0.4f, "s");
        addFloat ("amp_sustain", "Amp Sustain", unit, 0.8f);
        addFloat ("amp_release", "Amp Release", timeRange, 0.35f, "s");
        addFloat ("vel_sens", "Velocity Sensitivity", unit, 0.5f);
        addFloat ("flt_attack", "Filter Attack", timeRange, 0.005f, "s");
        addFloat ("flt_decay", "Filter Decay", timeRange, 0.5f, "s");
        addFloat ("flt_sustain", "Filter Sustain", unit, 0.4f);
        addFloat ("flt_release", "Filter Release", timeRange, 0.4f, "s");
        addFloat ("menv_attack", "Mod Env Attack", timeRange, 0.01f, "s");
        addFloat ("menv_decay", "Mod Env Decay", timeRange, 0.5f, "s");
        addChoice ("menv_mode", "Mod Env Mode", StringArray { "One shot", "Loop" }, 0);

        // LFOs
        addChoice ("lfo1_wave", "LFO 1 Wave", lfoWaves(), 0);
        addFloat ("lfo1_rate", "LFO 1 Rate", rateRange, 5.0f, "Hz");
        addChoice ("lfo2_wave", "LFO 2 Wave", lfoWaves(), 1);
        addFloat ("lfo2_rate", "LFO 2 Rate", rateRange, 0.3f, "Hz");

        // Mod matrix
        for (int i = 0; i < 4; ++i)
        {
            const String n = "Mod " + String (i + 1) + " ";
            addChoice (slot (i, "src"), n + "Source", modSources(), 0);
            addChoice (slot (i, "dst"), n + "Destination", modDests(), 0);
            addFloat (slot (i, "amt"), n + "Amount", bipolar, 0.0f);
        }

        // Strange physics
        addFloat ("gravity", "Gravity", bipolar, 0.0f);
        addFloat ("entropy", "Entropy", unit, 0.0f);
        addFloat ("drift", "Analog Drift", unit, 0.1f);
        addFloat ("orbit_rate", "Orbit Rate", skewed (0.02f, 10.0f, 0.8f), 0.3f, "Hz");
        addFloat ("orbit_depth", "Orbit Depth", unit, 0.0f);
        addFloat ("spread", "Voice Spread", unit, 0.3f);

        // Voice
        addChoice ("mode", "Voice Mode", StringArray { "Mono", "Legato", "Poly" }, 2);
        addFloat ("glide", "Glide", skewed (0.0f, 2.0f, 0.2f), 0.0f, "s");
        addInt ("bend", "Bend Range", 1, 24, 2);

        // Arpeggiator
        addChoice ("arp_on", "Arp On", onOff, 0);
        addChoice ("arp_mode", "Arp Mode", arpModes(), 0);
        addChoice ("arp_rate", "Arp Rate", arpRates(), 3);
        addChoice ("arp_rhythm", "Arp Rhythm", rhythmNames(), 0);
        addInt ("arp_octaves", "Arp Octaves", 1, 4, 1);
        addFloat ("arp_gate", "Arp Gate", NormalisableRange<float> (0.05f, 1.5f), 0.6f);
        addFloat ("arp_swing", "Arp Swing", unit, 0.0f);
        addFloat ("arp_ratchet", "Arp Ratchet", unit, 0.0f);
        addFloat ("arp_jump", "Arp Octave Jump", unit, 0.0f);
        addChoice ("arp_latch", "Arp Latch", onOff, 0);

        // Effects
        addChoice ("dist_type", "Drive Type", driveTypes(), 0);
        addFloat ("dist_amt", "Drive Amount", unit, 0.0f);
        addFloat ("tape_wow", "Tape Wow", unit, 0.0f);
        addFloat ("tape_flutter", "Tape Flutter", unit, 0.0f);
        addFloat ("tape_age", "Tape Age", unit, 0.0f);
        addFloat ("chorus_rate", "Chorus Rate", skewed (0.05f, 8.0f, 1.0f), 0.5f, "Hz");
        addFloat ("chorus_depth", "Chorus Depth", unit, 0.5f);
        addFloat ("chorus_mix", "Chorus Mix", unit, 0.0f);
        addFloat ("phaser_rate", "Phaser Rate", skewed (0.02f, 10.0f, 1.0f), 0.3f, "Hz");
        addFloat ("phaser_depth", "Phaser Depth", unit, 0.7f);
        addFloat ("phaser_mix", "Phaser Mix", unit, 0.0f);
        addChoice ("delay_sync", "Delay Sync", delaySyncs(), 0);
        addFloat ("delay_time", "Delay Time", skewed (0.02f, 1.5f, 0.35f), 0.3f, "s");
        addFloat ("delay_fb", "Delay Feedback", NormalisableRange<float> (0.0f, 0.95f), 0.35f);
        addFloat ("delay_tone", "Delay Tone", unit, 0.5f);
        addFloat ("delay_mix", "Delay Mix", unit, 0.0f);
        addFloat ("cloud_size", "Cloud Grain Size", unit, 0.4f);
        addFloat ("cloud_density", "Cloud Density", unit, 0.4f);
        addFloat ("cloud_spray", "Cloud Spray", unit, 0.3f);
        addFloat ("cloud_mix", "Cloud Mix", unit, 0.0f);
        addFloat ("reverb_size", "Reverb Size", unit, 0.5f);
        addFloat ("reverb_shimmer", "Reverb Shimmer", unit, 0.0f);
        addFloat ("reverb_mix", "Reverb Mix", unit, 0.15f);
        addChoice ("gate_rate", "Gate Rate", gateRates(), 1);
        addChoice ("gate_pattern", "Gate Pattern", gatePatternNames(), 1);
        addFloat ("gate_depth", "Gate Depth", unit, 0.0f);
        addFloat ("pan_rate", "Auto-Pan Rate", skewed (0.05f, 15.0f, 1.5f), 1.0f, "Hz");
        addFloat ("pan_depth", "Auto-Pan Depth", unit, 0.0f);
        addFloat ("haas", "Haas Delay", NormalisableRange<float> (0.0f, 30.0f), 0.0f, "ms");
        addFloat ("width", "Stereo Width", NormalisableRange<float> (0.0f, 2.0f), 1.0f);
        addFloat ("master", "Master", unit, 0.7f);

        return layout;
    }
}
