#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include <array>
#include "PluginProcessor.h"

namespace Theme
{
    inline const juce::Colour bg        { 0xff111215 };
    inline const juce::Colour panel     { 0xff1b1d21 };
    inline const juce::Colour panelEdge { 0xff2e3138 };
    inline const juce::Colour text      { 0xffe8e4da };
    inline const juce::Colour textDim   { 0xff8b8880 };
    inline const juce::Colour metal1    { 0xff383b41 };
    inline const juce::Colour metal2    { 0xff232529 };

    // Spectrum-coded groups: each page area gets its own hue
    inline const juce::Colour osc     { 0xffff7a45 };
    inline const juce::Colour filter  { 0xfff5c84c };
    inline const juce::Colour env     { 0xff5aa9ff };
    inline const juce::Colour voice   { 0xffc792ea };
    inline const juce::Colour lfo     { 0xff9d8cff };
    inline const juce::Colour matrix  { 0xff6ee0a0 };
    inline const juce::Colour strange { 0xffff6b9a };
    inline const juce::Colour arp     { 0xff4fd6e8 };
    inline const juce::Colour colourFx{ 0xffff5d5d };
    inline const juce::Colour timeFx  { 0xff3fd8c4 };
    inline const juce::Colour spaceFx { 0xff7fb6ff };
}

class KaleidoLookAndFeel : public juce::LookAndFeel_V4
{
public:
    KaleidoLookAndFeel();

    void drawRotarySlider (juce::Graphics&, int x, int y, int width, int height, float sliderPos,
                           float rotaryStartAngle, float rotaryEndAngle, juce::Slider&) override;

    juce::Font getComboBoxFont (juce::ComboBox&) override { return juce::Font (juce::FontOptions (13.0f)); }
    juce::Font getPopupMenuFont() override               { return juce::Font (juce::FontOptions (14.0f)); }
};

class LabeledKnob : public juce::Component
{
public:
    LabeledKnob (juce::AudioProcessorValueTreeState& state, const juce::String& paramID,
                 const juce::String& name, juce::Colour accent);
    void resized() override;
    void setLabelText (const juce::String& t) { label.setText (t, juce::dontSendNotification); }

private:
    juce::Slider slider;
    juce::Label label;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment;
};

class ParamCombo : public juce::Component
{
public:
    ParamCombo (juce::AudioProcessorValueTreeState& state, const juce::String& paramID, const juce::String& name);
    void resized() override;

private:
    juce::ComboBox box;
    juce::Label label;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> attachment;
};

class OscTop : public juce::Component, private juce::Timer
{
public:
    OscTop (juce::AudioProcessorValueTreeState& state, int oscIndex, juce::Colour accent);
    void setModKnobs (LabeledKnob* shapeKnob, LabeledKnob* colorKnob);
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;

    std::atomic<float>* typeParam;
    std::atomic<float>* shapeParam;
    std::atomic<float>* colorParam;
    juce::Colour accent;
    juce::ComboBox box;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> attachment;
    LabeledKnob* shapeKnob = nullptr;
    LabeledKnob* colorKnob = nullptr;
    std::array<float, 256> wave {};
    int lastType = -1;
    float lastShape = -1.0f, lastColor = -1.0f;
    juce::Rectangle<int> scopeArea;
};

class ArpDisplay : public juce::Component, private juce::Timer
{
public:
    ArpDisplay (KaleidoProcessor& p);
    void paint (juce::Graphics&) override;

private:
    void timerCallback() override;

    KaleidoProcessor& proc;
    std::atomic<float>* rhythmParam;
    std::atomic<float>* onParam;
    std::atomic<float>* modeParam;
    int lastStep = -2, lastRhythm = -1, lastOn = -1, lastMode = -1;
};

class Section : public juce::Component
{
public:
    Section (juce::String title, int columns, juce::Colour accent);

    LabeledKnob* addKnob (juce::AudioProcessorValueTreeState& state, const juce::String& id, const juce::String& name);
    void addCombo (juce::AudioProcessorValueTreeState& state, const juce::String& id, const juce::String& name);
    void setTop (std::unique_ptr<juce::Component> component, int height);

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    juce::String title;
    int columns;
    juce::Colour accent;
    std::unique_ptr<juce::Component> top;
    int topHeight = 0;
    std::vector<std::unique_ptr<juce::Component>> items;
};

class KaleidoEditor : public juce::AudioProcessorEditor
{
public:
    explicit KaleidoEditor (KaleidoProcessor&);
    ~KaleidoEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    enum Page { SynthPage = 0, ModPage, ArpPage, FxPage, numPages };

    Section& makeSection (int page, const juce::String& title, int columns, juce::Colour accent);
    void selectPreset (int index);
    void showPage (int page);
    void layoutRow (juce::Rectangle<int> row, std::initializer_list<std::pair<Section*, float>> parts);

    static constexpr int cheekWidth = 22;
    static constexpr int headerHeight = 56;
    static constexpr int tabHeight = 34;

    KaleidoLookAndFeel lnf;
    KaleidoProcessor& proc;

    juce::ComboBox presetBox;
    juce::TextButton prevButton { "<" }, nextButton { ">" };
    std::array<juce::TextButton, numPages> tabs;
    std::array<juce::Component, numPages> pages;
    int currentPage = SynthPage;

    std::vector<std::unique_ptr<Section>> sections;
    std::array<Section*, 3> oscSec {};
    Section *filterSec = nullptr, *fenvSec = nullptr, *ampSec = nullptr, *mixSec = nullptr, *voiceSec = nullptr;
    Section *lfo1Sec = nullptr, *lfo2Sec = nullptr, *menvSec = nullptr, *strangeSec = nullptr;
    std::array<Section*, 4> slotSec {};
    Section* arpSec = nullptr;
    Section *driveSec = nullptr, *tapeSec = nullptr, *chorusSec = nullptr, *phaserSec = nullptr;
    Section *delaySec = nullptr, *cloudSec = nullptr, *reverbSec = nullptr;
    Section *gateSec = nullptr, *stereoSec = nullptr, *outSec = nullptr;

    juce::MidiKeyboardComponent keyboard;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (KaleidoEditor)
};
