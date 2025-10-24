#include "PlayerGUI.h"

PlayerGUI::PlayerGUI()
{
    // Add all buttons
    for (auto* button : { &loadButton, &playButton, &stopButton, &restartButton, &loopButton, &muteButton, &forwardButton, &backwardButton })
    {
        button->addListener(this);
        addAndMakeVisible(button);
    }

    // Configure volume slider
    volumeSlider.setRange(0.0, 1.0, 0.01);
    volumeSlider.setValue(0.5);
    volumeSlider.addListener(this);
    volumeSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    addAndMakeVisible(volumeSlider);

    // Configure volume label
    volumeLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(volumeLabel);

    setLoopState(false);
    setPlaybackState(false);
    setMuteState(false);
}

PlayerGUI::~PlayerGUI() = default;

void PlayerGUI::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);
    g.setColour(juce::Colours::lightgrey);
    g.drawRect(getLocalBounds(), 1);
}

void PlayerGUI::resized()
{
    auto area = getLocalBounds();
    int buttonHeight = 30;
    int margin = 8;
    auto topRow = area.removeFromTop(buttonHeight + margin).reduced(margin);

    // Adjust button count to 8 (added forward/backward)
    int buttonWidth = (topRow.getWidth() - (7 * margin)) / 8;

    loadButton.setBounds(topRow.removeFromLeft(buttonWidth)); topRow.removeFromLeft(margin);
    playButton.setBounds(topRow.removeFromLeft(buttonWidth)); topRow.removeFromLeft(margin);
    stopButton.setBounds(topRow.removeFromLeft(buttonWidth)); topRow.removeFromLeft(margin);
    restartButton.setBounds(topRow.removeFromLeft(buttonWidth)); topRow.removeFromLeft(margin);
    loopButton.setBounds(topRow.removeFromLeft(buttonWidth)); topRow.removeFromLeft(margin);
    muteButton.setBounds(topRow.removeFromLeft(buttonWidth)); topRow.removeFromLeft(margin);
    backwardButton.setBounds(topRow.removeFromLeft(buttonWidth)); topRow.removeFromLeft(margin);
    forwardButton.setBounds(topRow.removeFromLeft(buttonWidth));

    auto bottomRow = area.reduced(margin);
    auto labelArea = bottomRow.removeFromLeft(60);
    volumeLabel.setBounds(labelArea);
    volumeSlider.setBounds(bottomRow);
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
        isLooping ? juce::Colours::hotpink : juce::Colours::lightblue);
}

void PlayerGUI::setPlaybackState(bool playing)
{
    isPlaying = playing;
    playButton.setButtonText(playing ? "Pause" : "Play");
    playButton.setColour(juce::TextButton::buttonColourId,
        playing ? juce::Colours::hotpink : juce::Colours::lightblue);
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
        isMuted ? juce::Colours::hotpink : juce::Colours::lightblue);
}
