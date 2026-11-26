#pragma once
#include <JuceHeader.h>
#include <functional>

class GeminiClient final : private juce::Thread
{
public:
    using Reply = std::function<void(const juce::String&)>;

    GeminiClient();
    ~GeminiClient() override;

    bool saveApiKey(const juce::String& key);
    juce::String loadApiKey() const;
    void clearApiKey();
    bool ask(const juce::String& prompt, const juce::String& apiKey, Reply reply);

private:
    void run() override;
    juce::String pendingPrompt;
    juce::String pendingKey;
    Reply pendingReply;
    juce::CriticalSection requestLock;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GeminiClient)
};
