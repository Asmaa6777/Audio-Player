#include "PlayerGUI.h"

PlayerGUI::PlayerGUI()
{
    
    for (auto* button : { &loadButton, &playButton, &stopButton, &restartButton, &loopButton, &muteButton, &forwardButton, &backwardButton, &goToEndButton })
    {
        button->addListener(this);
        addAndMakeVisible(button);
        button->setColour(juce::TextButton::buttonColourId, juce::Colours::purple);
    }

    
    volumeSlider.setRange(0.0, 1.0, 0.01);
    volumeSlider.setValue(0.5);
    volumeSlider.addListener(this);
    volumeSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    addAndMakeVisible(volumeSlider);
    volumeSlider.setColour(juce::Slider::thumbColourId, juce::Colours::purple);
    volumeSlider.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colours::purple);
    volumeLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(volumeLabel);

    setLoopState(false);
    setPlaybackState(false);
    setMuteState(false);
}

PlayerGUI::~PlayerGUI() = default;

void PlayerGUI::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::silver);
    g.setColour(juce::Colours::purple);
    g.drawRect(getLocalBounds(), 2);
}

void PlayerGUI::resized()
{
    int y = 20;
    int buttonWidth = 70;
    int buttonHeight = 20;

    loadButton.setBounds(20, y, 80, buttonHeight);
    playButton.setBounds(110, y, buttonWidth, buttonHeight);
    stopButton.setBounds(190, y, buttonWidth, buttonHeight);
    restartButton.setBounds(270, y, buttonWidth, buttonHeight);
    loopButton.setBounds(350, y, buttonWidth, buttonHeight);
    muteButton.setBounds(430, y, buttonWidth, buttonHeight);
    backwardButton.setBounds(510, y, buttonWidth, buttonHeight);
    forwardButton.setBounds(590, y, buttonWidth, buttonHeight);
    goToEndButton.setBounds(670, y, buttonWidth, buttonHeight);

    volumeSlider.setBounds(20, 60, getWidth() - 40, 30);
}

void PlayerGUI::buttonClicked(juce::Button* button)
{
    if (!listener) return;

    if (button == &loadButton) listener->loadButtonClicked();
    else if (button == &playButton) listener->playButtonClicked();
    else if (button == &stopButton) listener->stopButtonClicked();
    else if (button == &restartButton) listener->restartButtonClicked();
    else if (button == &loopButton) listener->loopButtonClicked();
    else if (button == &muteButton) listener->muteButtonClicked();
    else if (button == &forwardButton) listener->forwardButtonClicked();
    else if (button == &backwardButton) listener->backwardButtonClicked();
    else if (button == &goToEndButton) listener->goToEndButtonClicked();
}

void PlayerGUI::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &volumeSlider && listener)
        listener->volumeChanged((float)slider->getValue());
}

void PlayerGUI::setListener(Listener* newListener) { listener = newListener; }

void PlayerGUI::setLoopState(bool isLooping)
{
    loopButton.setButtonText(isLooping ? "Loop: ON" : "Loop: OFF");
    loopButton.setColour(juce::TextButton::buttonColourId,
        isLooping ? juce::Colours::hotpink : juce::Colours::purple);
}

void PlayerGUI::setPlaybackState(bool playing)
{
    isPlaying = playing;
    playButton.setButtonText(playing ? "Pause" : "Play");
    playButton.setColour(juce::TextButton::buttonColourId,
        playing ? juce::Colours::hotpink : juce::Colours::purple);
}

void PlayerGUI::setVolumeLevel(float volume)
{
    volumeSlider.setValue(volume, juce::dontSendNotification);
}

void PlayerGUI::setMuteState(bool isMutedNow)
{
    isMuted = isMutedNow;
    muteButton.setButtonText(isMuted ? "Mute: ON" : "Mute: OFF");
    muteButton.setColour(juce::TextButton::buttonColourId,
        isMuted ? juce::Colours::hotpink : juce::Colours::purple);
}