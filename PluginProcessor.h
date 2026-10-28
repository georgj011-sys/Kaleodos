#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include <array>
#include <atomic>
#include "Voice.h"
#include "FX.h"
#include "Arp.h"

class KaleidoProcessor : public juce::AudioProcessor
{
public:
    KaleidoProcessor();
    ~KaleidoProcessor() override = default;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    using AudioProcessor::processBlock;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "Kaleido"; }
    bool acceptsMidi() const override  { return true; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 8.0; }

    int getNumPrograms() override;
    int getCurrentProgram() override { return currentProgram; }
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState apvts;
    juce::MidiKeyboardState keyboardState;
    std::atomic<int> arpDisplayStep { -1 };

private:
    enum Mode { Mono = 0, Legato, Poly };

    struct Pending
    {
        double time = 0.0;
        int note = 0;
        float vel = 0.0f;
        bool on = false;
    };

    void readParams (VoiceParams& vp, FxParams& fp) const;
    void advanceTo (int& cur, int target, const VoiceParams& vp);
    void renderVoices (int start, int num, const VoiceParams& vp);
    void handleMidi (const juce::MidiMessage& msg, double absTime, const VoiceParams& vp);

    void engineNoteOn (int note, float velocity, const VoiceParams& vp);
    void engineNoteOff (int note);
    void engineReleaseAll();

    void doArpStep (double time, const VoiceParams& vp);
    void schedule (double time, int note, float vel, bool on);
    void stopArp();

    float v (juce::StringRef id) const { return apvts.getRawParameterValue (id)->load(); }
    int vi (juce::StringRef id) const  { return (int) std::lround (v (id)); }

    std::array<KaleidoVoice, 8> voices;
    KaleidoFX fx;
    double sampleRate = 44100.0;

    // Block-local pointers
    float* outL = nullptr;
    float* outR = nullptr;
    double blockStart = 0.0;
    double clock = 0.0;
    double bpm = 120.0;

    // Engine state
    juce::Array<int> noteStack;
    int lastNote = -1;
    int mode = Poly, lastMode = -1;
    float bend = 0.0f, modWheel = 0.0f;
    juce::uint32 seed = 1;
    juce::uint64 ageCounter = 0;
    int spreadCounter = 0;

    // Arpeggiator state
    bool arpOn = false, arpWasOn = false, arpRunning = false, latch = false;
    juce::Array<int> physicalHeld, arpNotes;
    double arpGrid = 0.0, arpNext = 0.0;
    int arpStep = 0, arpNoteIndex = 0, walkPos = 0;
    std::array<Pending, 256> pending;
    int pendingCount = 0;
    juce::Random arpRng;

    int currentProgram = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (KaleidoProcessor)
};
