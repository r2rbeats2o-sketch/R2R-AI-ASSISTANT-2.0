#include "PluginProcessor.h"
#include "PluginEditor.h"

R2RAIAssistantAudioProcessor::R2RAIAssistantAudioProcessor()
    : AudioProcessor(BusesProperties()
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      parameters(*this, nullptr, "PARAMETERS", {})
{
}

void R2RAIAssistantAudioProcessor::prepareToPlay(double sampleRate, int)
{
    currentSampleRate = sampleRate > 0.0 ? sampleRate : 44100.0;
}

bool R2RAIAssistantAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    return layouts.getMainInputChannelSet() == layouts.getMainOutputChannelSet()
        && (layouts.getMainOutputChannelSet() == juce::AudioChannelSet::mono()
            || layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo());
}

void R2RAIAssistantAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    juce::ScopedNoDenormals noDenormals;
    const int channels = getTotalNumInputChannels();
    for (int ch = getTotalNumOutputChannels(); ch < buffer.getNumChannels(); ++ch)
        buffer.clear(ch, 0, buffer.getNumSamples());

    const int root = pendingRoot.exchange(-1);
    if (root >= 0)
    {
        const int quality = pendingQuality.load();
        const int third = quality == 1 ? 3 : (quality == 2 ? 4 : 3);
        const int fifth = quality == 3 ? 6 : 7;
        const int notes[] = { root, root + third, root + fifth };
        const int noteOffSample = juce::jlimit(1, juce::jmax(1, buffer.getNumSamples() - 1),
                                               (int) (currentSampleRate * 0.35));
        for (int note : notes)
        {
            const int bounded = juce::jlimit(0, 127, note);
            midi.addEvent(juce::MidiMessage::noteOn(1, bounded, (juce::uint8) 100), 0);
            midi.addEvent(juce::MidiMessage::noteOff(1, bounded), noteOffSample);
        }
    }
    juce::ignoreUnused(channels);
}

void R2RAIAssistantAudioProcessor::requestChord(int root, int quality) noexcept
{
    pendingQuality.store(quality);
    pendingRoot.store(juce::jlimit(0, 127, root));
}

juce::AudioProcessorEditor* R2RAIAssistantAudioProcessor::createEditor()
{
    return new R2RAIAssistantAudioProcessorEditor(*this);
}

void R2RAIAssistantAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    juce::MemoryOutputStream stream(destData, true);
    stream.writeInt(1);
}

void R2RAIAssistantAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    juce::ignoreUnused(data, sizeInBytes);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new R2RAIAssistantAudioProcessor();
}
