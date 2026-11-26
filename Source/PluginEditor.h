#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class R2RAIAssistantAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit R2RAIAssistantAudioProcessorEditor(R2RAIAssistantAudioProcessor&);
    ~R2RAIAssistantAudioProcessorEditor() override = default;
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    class NavButton final : public juce::TextButton
    {
    public:
        explicit NavButton(const juce::String& text) : juce::TextButton(text) {}
    };

    void setPage(int page);
    void refreshChordLabel();

    R2RAIAssistantAudioProcessor& processor;
    juce::Label brandLabel, versionLabel, pageTitle, statusLabel, chordLabel, assistantText;
    juce::TextButton navAssistant, navChords, navMidi, navMix, generateButton, askButton;
    juce::ComboBox rootBox, qualityBox;
    juce::TextEditor promptBox;
    juce::Slider intensitySlider;
    int currentPage = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(R2RAIAssistantAudioProcessorEditor)
};
