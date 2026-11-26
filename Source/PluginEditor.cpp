#include "PluginEditor.h"

namespace
{
    const juce::Colour bg { 0xff090c12 };
    const juce::Colour panel { 0xff111722 };
    const juce::Colour cyan { 0xff00e5ff };
    const juce::Colour red { 0xffff3158 };
    const juce::Colour muted { 0xff8b9aaa };
}

R2RAIAssistantAudioProcessorEditor::R2RAIAssistantAudioProcessorEditor(R2RAIAssistantAudioProcessor& p)
    : AudioProcessorEditor(&p), processor(p),
      navAssistant("AI ASSISTANT"), navChords("CHORD LAB"), navMidi("MIDI TOOLS"), navMix("MIX GUIDE"),
      generateButton("GENERATE CHORD"), askButton("ASK GEMMA"),
      saveKeyButton("SAVE KEY"), clearKeyButton("CLEAR")
{
    setSize(980, 740);
    setResizable(true, true);
    setResizeLimits(900, 680, 1500, 1000);

    brandLabel.setText("R2R", juce::dontSendNotification);
    brandLabel.setFont(juce::Font(juce::FontOptions(28.0f, juce::Font::bold)));
    brandLabel.setColour(juce::Label::textColourId, cyan);
    addAndMakeVisible(brandLabel);

    versionLabel.setText("AI ASSISTANT 2.0  /  GEMMA LOCAL + GEMINI ONLINE", juce::dontSendNotification);
    versionLabel.setColour(juce::Label::textColourId, muted);
    versionLabel.setFont(juce::Font(juce::FontOptions(11.0f)));
    addAndMakeVisible(versionLabel);

    auto styleNav = [this](juce::TextButton& b, juce::Colour c)
    {
        b.setColour(juce::TextButton::buttonColourId, panel);
        b.setColour(juce::TextButton::buttonOnColourId, c.withAlpha(0.22f));
        b.setColour(juce::TextButton::textColourOffId, muted);
        b.setColour(juce::TextButton::textColourOnId, c);
        b.setClickingTogglesState(true);
        addAndMakeVisible(b);
    };
    styleNav(navAssistant, cyan); styleNav(navChords, cyan);
    styleNav(navMidi, cyan); styleNav(navMix, cyan);

    pageTitle.setFont(juce::Font(juce::FontOptions(23.0f, juce::Font::bold)));
    pageTitle.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(pageTitle);

    statusLabel.setText("GEMMA LOCAL  •  START OLLAMA TO CONNECT", juce::dontSendNotification);
    statusLabel.setColour(juce::Label::textColourId, cyan);
    statusLabel.setFont(juce::Font(juce::FontOptions(11.0f)));
    addAndMakeVisible(statusLabel);

    providerLabel.setText("AI ENGINE", juce::dontSendNotification);
    providerLabel.setColour(juce::Label::textColourId, cyan);
    providerLabel.setFont(juce::Font(juce::FontOptions(10.0f, juce::Font::bold)));
    addAndMakeVisible(providerLabel);

    providerBox.addItem("GEMMA LOCAL  /  OFFLINE", 1);
    providerBox.addItem("GEMINI ONLINE  /  API", 2);
    providerBox.setSelectedId(1, juce::dontSendNotification);
    providerBox.setColour(juce::ComboBox::backgroundColourId, panel);
    providerBox.setColour(juce::ComboBox::textColourId, juce::Colours::white);
    providerBox.setColour(juce::ComboBox::outlineColourId, cyan.withAlpha(0.5f));
    addAndMakeVisible(providerBox);
    providerBox.onChange = [this] { updateProviderControls(); };

    assistantText.setText("R2R MUSIC PRODUCTION CO-PILOT\nGemma runs locally on this PC through Ollama. No Gemini API key or internet connection is needed after the model is downloaded.", juce::dontSendNotification);
    assistantText.setColour(juce::Label::textColourId, muted);
    assistantText.setJustificationType(juce::Justification::topLeft);
    assistantText.setMinimumHorizontalScale(1.0f);
    addAndMakeVisible(assistantText);

    apiKeyLabel.setText("GEMINI API KEY  •  STORED IN WINDOWS CREDENTIAL MANAGER", juce::dontSendNotification);
    apiKeyLabel.setColour(juce::Label::textColourId, cyan);
    apiKeyLabel.setFont(juce::Font(juce::FontOptions(10.0f, juce::Font::bold)));
    addAndMakeVisible(apiKeyLabel);

    styleTextEditor(apiKeyBox);
    apiKeyBox.setPasswordCharacter(0x2022);
    apiKeyBox.setText(gemini.loadApiKey(), false);
    apiKeyBox.setTextToShowWhenEmpty("Paste Gemini API key here...", muted);
    addAndMakeVisible(apiKeyBox);

    localModelLabel.setText("LOCAL MODEL  •  DEFAULT: gemma4:e2b", juce::dontSendNotification);
    localModelLabel.setColour(juce::Label::textColourId, cyan);
    localModelLabel.setFont(juce::Font(juce::FontOptions(10.0f, juce::Font::bold)));
    addAndMakeVisible(localModelLabel);
    styleTextEditor(localModelBox);
    localModelBox.setText("gemma4:e2b", false);
    localModelBox.setTextToShowWhenEmpty("Ollama model name, e.g. gemma4:e2b", muted);
    addAndMakeVisible(localModelBox);

    auto styleSmallButton = [](juce::TextButton& b, juce::Colour colour)
    {
        b.setColour(juce::TextButton::buttonColourId, colour);
        b.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    };
    styleSmallButton(saveKeyButton, cyan);
    saveKeyButton.setColour(juce::TextButton::textColourOffId, bg);
    saveKeyButton.onClick = [this]
    {
        if (gemini.saveApiKey(apiKeyBox.getText()))
            statusLabel.setText("GEMINI KEY SAVED SECURELY  •  READY TO CONNECT", juce::dontSendNotification);
        else
            statusLabel.setText("KEY SAVE FAILED  •  CHECK THE KEY AND WINDOWS ACCOUNT", juce::dontSendNotification);
    };
    addAndMakeVisible(saveKeyButton);

    styleSmallButton(clearKeyButton, red);
    clearKeyButton.onClick = [this]
    {
        gemini.clearApiKey();
        apiKeyBox.clear();
        statusLabel.setText("SAVED GEMINI KEY CLEARED", juce::dontSendNotification);
    };
    addAndMakeVisible(clearKeyButton);

    styleTextEditor(promptBox);
    promptBox.setMultiLine(true);
    promptBox.setReturnKeyStartsNewLine(true);
    promptBox.setTextToShowWhenEmpty("Ask about a 16-bar Bengali melodic techno arrangement at 128 BPM...", muted);
    addAndMakeVisible(promptBox);

    askButton.setColour(juce::TextButton::buttonColourId, cyan);
    askButton.setColour(juce::TextButton::textColourOffId, bg);
    askButton.onClick = [this] { sendGeminiPrompt(); };
    addAndMakeVisible(askButton);

    rootBox.addItemList({"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"}, 1);
    rootBox.setSelectedId(1);
    qualityBox.addItemList({"Minor", "Major", "Diminished", "Power"}, 1);
    qualityBox.setSelectedId(1);
    for (auto* c : { (juce::Component*) &rootBox, (juce::Component*) &qualityBox })
    {
        c->setColour(juce::ComboBox::backgroundColourId, panel);
        c->setColour(juce::ComboBox::textColourId, juce::Colours::white);
        c->setColour(juce::ComboBox::outlineColourId, cyan.withAlpha(0.5f));
        addAndMakeVisible(c);
    }

    chordLabel.setColour(juce::Label::textColourId, cyan);
    chordLabel.setFont(juce::Font(juce::FontOptions(19.0f, juce::Font::bold)));
    addAndMakeVisible(chordLabel);
    generateButton.setColour(juce::TextButton::buttonColourId, red);
    generateButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    generateButton.onClick = [this]
    {
        const int root = 48 + rootBox.getSelectedId() - 1;
        processor.requestChord(root, qualityBox.getSelectedId() - 1);
        refreshChordLabel();
        statusLabel.setText("MIDI CHORD QUEUED  •  PLAY / RECORD IN YOUR DAW", juce::dontSendNotification);
    };
    addAndMakeVisible(generateButton);

    intensitySlider.setRange(0.0, 100.0, 1.0);
    intensitySlider.setValue(70.0);
    intensitySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 22);
    intensitySlider.setColour(juce::Slider::trackColourId, cyan);
    intensitySlider.setColour(juce::Slider::thumbColourId, red);
    addAndMakeVisible(intensitySlider);

    navAssistant.onClick = [this] { setPage(0); };
    navChords.onClick = [this] { setPage(1); };
    navMidi.onClick = [this] { setPage(2); };
    navMix.onClick = [this] { setPage(3); };
    updateProviderControls();
    setPage(0);
}

void R2RAIAssistantAudioProcessorEditor::styleTextEditor(juce::TextEditor& editor)
{
    editor.setColour(juce::TextEditor::backgroundColourId, panel);
    editor.setColour(juce::TextEditor::textColourId, juce::Colours::white);
    editor.setColour(juce::TextEditor::outlineColourId, cyan.withAlpha(0.4f));
    editor.setColour(juce::TextEditor::focusedOutlineColourId, cyan);
}

void R2RAIAssistantAudioProcessorEditor::updateProviderControls()
{
    const bool useGemini = providerBox.getSelectedId() == 2;
    const bool assistant = currentPage == 0;
    apiKeyLabel.setVisible(assistant && useGemini);
    apiKeyBox.setVisible(assistant && useGemini);
    saveKeyButton.setVisible(assistant && useGemini);
    clearKeyButton.setVisible(assistant && useGemini);
    localModelLabel.setVisible(assistant && !useGemini);
    localModelBox.setVisible(assistant && !useGemini);
    askButton.setButtonText(useGemini ? "ASK GEMINI" : "ASK GEMMA");
    if (assistant)
        statusLabel.setText(useGemini ? "GEMINI ONLINE  •  API KEY REQUIRED" : "GEMMA LOCAL  •  OLLAMA MUST BE RUNNING", juce::dontSendNotification);
    resized();
}

void R2RAIAssistantAudioProcessorEditor::sendGeminiPrompt()
{
    const auto prompt = promptBox.getText().trim();
    const bool useGemini = providerBox.getSelectedId() == 2;
    const auto key = apiKeyBox.getText().trim().isNotEmpty() ? apiKeyBox.getText().trim() : gemini.loadApiKey();
    const auto model = localModelBox.getText().trim();

    if (prompt.isEmpty())
    {
        assistantText.setText("Type a music-production question first.", juce::dontSendNotification);
        return;
    }
    if (useGemini && key.isEmpty())
    {
        statusLabel.setText("API KEY REQUIRED  •  SWITCH TO GEMMA LOCAL OR ADD A KEY", juce::dontSendNotification);
        return;
    }
    if (!useGemini && model.isEmpty())
    {
        statusLabel.setText("MODEL NAME REQUIRED  •  ENTER AN INSTALLED OLLAMA MODEL", juce::dontSendNotification);
        return;
    }

    assistantText.setText(useGemini ? "GEMINI IS THINKING...\nSending request over HTTPS."
                                    : "GEMMA IS THINKING...\nRunning locally through Ollama on this PC.",
                          juce::dontSendNotification);
    statusLabel.setText(useGemini ? "CONNECTING TO GEMINI..." : "CONNECTING TO LOCAL OLLAMA...");
    askButton.setEnabled(false);

    juce::Component::SafePointer<R2RAIAssistantAudioProcessorEditor> safeThis(this);
    const auto callback = [safeThis, useGemini](const juce::String& answer)
    {
        if (safeThis == nullptr)
            return;
        safeThis->assistantText.setText(answer, juce::dontSendNotification);
        const bool failed = answer.startsWith("Gemini API error") || answer.startsWith("Gemma Local error")
                         || answer.startsWith("Network error") || answer.startsWith("Connection error");
        safeThis->statusLabel.setText(failed ? "AI REQUEST FAILED  •  SEE MESSAGE ABOVE"
                                             : (useGemini ? "GEMINI ONLINE  •  RESPONSE RECEIVED"
                                                          : "GEMMA LOCAL  •  RESPONSE RECEIVED"),
                                      juce::dontSendNotification);
        safeThis->askButton.setEnabled(true);
    };

    const bool started = useGemini ? gemini.ask(prompt, key, callback)
                                   : gemini.askLocal(prompt, model, callback);
    if (!started)
    {
        askButton.setEnabled(true);
        statusLabel.setText("REQUEST BUSY  •  WAIT FOR THE CURRENT RESPONSE", juce::dontSendNotification);
    }
}

void R2RAIAssistantAudioProcessorEditor::refreshChordLabel()
{
    const juce::String names[] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
    const int root = juce::jlimit(1, 12, rootBox.getSelectedId()) - 1;
    const juce::String quality = qualityBox.getText();
    chordLabel.setText(names[root] + " " + quality + " triad  •  MIDI output", juce::dontSendNotification);
}

void R2RAIAssistantAudioProcessorEditor::setPage(int page)
{
    currentPage = page;
    navAssistant.setToggleState(page == 0, juce::dontSendNotification);
    navChords.setToggleState(page == 1, juce::dontSendNotification);
    navMidi.setToggleState(page == 2, juce::dontSendNotification);
    navMix.setToggleState(page == 3, juce::dontSendNotification);

    const bool assistant = page == 0;
    const bool chords = page == 1;
    pageTitle.setText(assistant ? "AI PRODUCTION ASSISTANT" : chords ? "CHORD & HARMONY LAB" : page == 2 ? "MIDI WORKSPACE" : "MIXING & MASTERING GUIDE", juce::dontSendNotification);
    assistantText.setVisible(assistant || page == 2 || page == 3);
    providerLabel.setVisible(assistant);
    providerBox.setVisible(assistant);
    promptBox.setVisible(assistant);
    askButton.setVisible(assistant);
    rootBox.setVisible(chords);
    qualityBox.setVisible(chords);
    chordLabel.setVisible(chords);
    generateButton.setVisible(chords);
    intensitySlider.setVisible(page == 3);
    updateProviderControls();

    if (page == 0)
        assistantText.setText("R2R MUSIC PRODUCTION CO-PILOT\nGemma Local runs through Ollama on this PC. Once the model is downloaded, prompts run locally without a Gemini API key.", juce::dontSendNotification);
    else if (page == 2)
        assistantText.setText("MIDI WORKSPACE\n\nIncoming MIDI passes through. Use CHORD LAB to generate a triad into your DAW. Record the MIDI output on an instrument track routed from this plugin.", juce::dontSendNotification);
    else if (page == 3)
        assistantText.setText("MIX CHECKLIST\n\n1. Balance levels before adding processing.\n2. Check kick and bass interaction.\n3. Use reference tracks at matched loudness.\n4. Check mono compatibility and headroom.\n5. Leave final loudness decisions until the arrangement is balanced.", juce::dontSendNotification);
    resized();
}

void R2RAIAssistantAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(bg);
    g.setColour(panel);
    g.fillRoundedRectangle(16.0f, 82.0f, (float) getWidth() - 32.0f, 54.0f, 10.0f);
    g.setColour(cyan.withAlpha(0.65f));
    g.fillRect(16.0f, 82.0f, 3.0f, 54.0f);
    g.setColour(panel);
    g.fillRoundedRectangle(16.0f, 151.0f, (float) getWidth() - 32.0f, (float) getHeight() - 205.0f, 12.0f);
    g.setColour(red.withAlpha(0.8f));
    g.fillRect(16.0f, 151.0f, 3.0f, 70.0f);
    g.setColour(muted);
    g.setFont(juce::Font(juce::FontOptions(10.0f)));
    g.drawText("R2R VISION  /  LOCAL GEMMA + GEMINI MUSIC INTELLIGENCE", 28, getHeight() - 32, getWidth() - 56, 16, juce::Justification::centredLeft);
}

void R2RAIAssistantAudioProcessorEditor::resized()
{
    const int w = getWidth();
    brandLabel.setBounds(22, 14, 76, 48);
    versionLabel.setBounds(104, 22, 650, 24);
    const int navY = 88, navW = (w - 72) / 4;
    navAssistant.setBounds(24, navY, navW, 42);
    navChords.setBounds(32 + navW, navY, navW, 42);
    navMidi.setBounds(40 + navW * 2, navY, navW, 42);
    navMix.setBounds(48 + navW * 3, navY, navW, 42);
    pageTitle.setBounds(38, 166, w - 76, 38);
    providerLabel.setBounds(40, 210, 100, 20);
    providerBox.setBounds(150, 207, 300, 30);
    assistantText.setBounds(40, 248, w - 80, 158);
    apiKeyLabel.setBounds(40, 414, w - 80, 20);
    apiKeyBox.setBounds(40, 438, w - 300, 38);
    saveKeyButton.setBounds(w - 244, 438, 120, 38);
    clearKeyButton.setBounds(w - 112, 438, 72, 38);
    localModelLabel.setBounds(40, 414, w - 80, 20);
    localModelBox.setBounds(40, 438, w - 80, 38);
    promptBox.setBounds(40, 490, w - 80, 108);
    askButton.setBounds(w - 220, 612, 180, 42);
    rootBox.setBounds(42, 242, 180, 42);
    qualityBox.setBounds(242, 242, 180, 42);
    chordLabel.setBounds(42, 312, w - 84, 40);
    generateButton.setBounds(42, 372, 230, 48);
    intensitySlider.setBounds(42, 360, w - 84, 100);
    statusLabel.setBounds(40, getHeight() - 66, w - 80, 20);
}
