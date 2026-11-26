#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "GeminiClient.h"

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
    void updateProviderControls();
    void refreshChordLabel();
    void sendGeminiPrompt();
    void styleTextEditor(juce::TextEditor& editor);

    R2RAIAssistantAudioProcessor& processor;
    GeminiClient gemini;
    juce::Label brandLabel, versionLabel, pageTitle, statusLabel, chordLabel, assistantText;
    juce::Label apiKeyLabel, providerLabel, localModelLabel;
    juce::TextButton navAssistant, navChords, navMidi, navMix, generateButton, askButton, saveKeyButton, clearKeyButton;
    juce::ComboBox rootBox, qualityBox, providerBox;
    juce::TextEditor promptBox, apiKeyBox, localModelBox;
    juce::Slider intensitySlider;
    int currentPage = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(R2RAIAssistantAudioProcessorEditor)
};
