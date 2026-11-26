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
      generateButton("GENERATE CHORD"), askButton("GET SUGGESTION")
{
    setSize(900, 600);
    setResizable(true, true);
    setResizeLimits(760, 500, 1400, 900);

    brandLabel.setText("R2R", juce::dontSendNotification);
    brandLabel.setFont(juce::Font(juce::FontOptions(28.0f, juce::Font::bold)));
    brandLabel.setColour(juce::Label::textColourId, cyan);
    addAndMakeVisible(brandLabel);

    versionLabel.setText("AI ASSISTANT 2.0  /  MUSIC WORKSTATION", juce::dontSendNotification);
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

    statusLabel.setText("ENGINE READY  •  LOCAL TOOLS ACTIVE", juce::dontSendNotification);
    statusLabel.setColour(juce::Label::textColourId, cyan);
    statusLabel.setFont(juce::Font(juce::FontOptions(11.0f)));
    addAndMakeVisible(statusLabel);

    assistantText.setText("Your production co-pilot. Ask for arrangement ideas, sound-design direction, music theory help, or a mixing checklist. Remote AI chat is not connected in this starter build.", juce::dontSendNotification);
    assistantText.setColour(juce::Label::textColourId, muted);
    assistantText.setJustificationType(juce::Justification::topLeft);
    assistantText.setMinimum水平Scale(1.0f);
    addAndMakeVisible(assistantText);

    promptBox.setMultiLine(true);
    promptBox.setReturnKeyStartsNewLine(true);
    promptBox.setTextToShowWhenEmpty("Describe your track, genre, BPM, or production question...", muted);
    promptBox.setColour(juce::TextEditor::backgroundColourId, panel);
    promptBox.setColour(juce::TextEditor::textColourId, juce::Colours::white);
    promptBox.setColour(juce::TextEditor::outlineColourId, cyan.withAlpha(0.4f));
    addAndMakeVisible(promptBox);

    askButton.setColour(juce::TextButton::buttonColourId, cyan);
    askButton.setColour(juce::TextButton::textColourOffId, bg);
    askButton.onClick = [this]
    {
        const auto q = promptBox.getText().trim();
        assistantText.setText(q.isEmpty()
            ? "Type a production question first."
            : "LOCAL ASSISTANT NOTE\n\nTry defining the target genre, BPM, key, energy, and reference mood. Then build the arrangement in 8- or 16-bar sections.\n\nYour prompt: " + q
              + "\n\nThis is an offline starter response; connect a trusted AI provider to enable remote answers.",
            juce::dontSendNotification);
    };
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
    setPage(0);
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
    promptBox.setVisible(assistant);
    askButton.setVisible(assistant);
    rootBox.setVisible(chords);
    qualityBox.setVisible(chords);
    chordLabel.setVisible(chords);
    generateButton.setVisible(chords);
    intensitySlider.setVisible(page == 3);
    if (page == 2)
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
    g.drawText("R2R VISION  /  AUDIO INTELLIGENCE SUITE", 28, getHeight() - 32, getWidth() - 56, 16, juce::Justification::centredLeft);
}

void R2RAIAssistantAudioProcessorEditor::resized()
{
    const int w = getWidth();
    brandLabel.setBounds(22, 14, 76, 48);
    versionLabel.setBounds(104, 22, 400, 24);
    const int navY = 88, navW = (w - 72) / 4;
    navAssistant.setBounds(24, navY, navW, 42);
    navChords.setBounds(32 + navW, navY, navW, 42);
    navMidi.setBounds(40 + navW * 2, navY, navW, 42);
    navMix.setBounds(48 + navW * 3, navY, navW, 42);
    pageTitle.setBounds(38, 166, w - 76, 38);
    assistantText.setBounds(40, 218, w - 80, 105);
    promptBox.setBounds(40, 340, w - 80, 100);
    askButton.setBounds(w - 220, 454, 180, 42);
    rootBox.setBounds(42, 242, 180, 42);
    qualityBox.setBounds(242, 242, 180, 42);
    chordLabel.setBounds(42, 312, w - 84, 40);
    generateButton.setBounds(42, 372, 230, 48);
    intensitySlider.setBounds(42, 360, w - 84, 100);
    statusLabel.setBounds(40, getHeight() - 66, w - 80, 20);
}
