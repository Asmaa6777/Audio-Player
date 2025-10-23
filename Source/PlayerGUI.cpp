/*
  ==============================================================================

    AudioGUI.cpp
    Created: 23 Oct 2025 6:23:40pm
    Author:  Family

  ==============================================================================
*/

#include "PlayerGUI.h"

PlayerGUI::PlayerGUI()
{
    // Configure buttons
    for (auto* button : { &loadButton, &playButton, &stopButton, &restartButton, &loopButton, &muteButton })
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

    // Set initial states
    setLoopState(false);
    setPlaybackState(false);
    setMuteState(false);  // NEW: Initialize mute state
}

PlayerGUI::~PlayerGUI()
{
}

void PlayerGUI::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);

    // Draw a subtle border around the component
    g.setColour(juce::Colours::lightgrey);
    g.drawRect(getLocalBounds(), 1);
}

void PlayerGUI::resized()
{
    auto area = getLocalBounds();
    int buttonHeight = 30;
    int margin = 10;

    // Top row - transport buttons
    auto topRow = area.removeFromTop(buttonHeight + margin).reduced(margin);
    int buttonWidth = (topRow.getWidth() - (6 * margin)) / 6;  // CHANGED: 6 buttons now

    loadButton.setBounds(topRow.removeFromLeft(buttonWidth));
    topRow.removeFromLeft(margin);
    playButton.setBounds(topRow.removeFromLeft(buttonWidth));
    topRow.removeFromLeft(margin);
    stopButton.setBounds(topRow.removeFromLeft(buttonWidth));
    topRow.removeFromLeft(margin);
    restartButton.setBounds(topRow.removeFromLeft(buttonWidth));
    topRow.removeFromLeft(margin);
    loopButton.setBounds(topRow.removeFromLeft(buttonWidth));
    topRow.removeFromLeft(margin);
    muteButton.setBounds(topRow.removeFromLeft(buttonWidth));  // NEW: Mute button position

    // Bottom row - volume control
    auto bottomRow = area.reduced(margin);
    auto labelArea = bottomRow.removeFromLeft(60);
    volumeLabel.setBounds(labelArea);
    volumeSlider.setBounds(bottomRow);
}

void PlayerGUI::buttonClicked(juce::Button* button)
{
    if (listener == nullptr)
        return;

    if (button == &loadButton)
    {
        listener->loadButtonClicked();
    }
    else if (button == &playButton)
    {
        listener->playButtonClicked();
    }
    else if (button == &stopButton)
    {
        listener->stopButtonClicked();
    }
    else if (button == &restartButton)
    {
        listener->restartButtonClicked();
    }
    else if (button == &loopButton)
    {
        listener->loopButtonClicked();
    }
    else if (button == &muteButton)  // NEW: Mute button handler
    {
        listener->muteButtonClicked();
    }
}

void PlayerGUI::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &volumeSlider && listener != nullptr)
    {
        listener->volumeChanged(static_cast<float>(slider->getValue()));
    }
}

void PlayerGUI::setListener(Listener* newListener)
{
    listener = newListener;
}

void PlayerGUI::setLoopState(bool isLooping)
{
    if (isLooping)
    {
        loopButton.setButtonText("Loop: ON");
        loopButton.setColour(juce::TextButton::buttonColourId, juce::Colours::hotpink);
    }
    else
    {
        loopButton.setButtonText("Loop: OFF");
        loopButton.setColour(juce::TextButton::buttonColourId, juce::Colours::lightblue);
    }
}

void PlayerGUI::setPlaybackState(bool playing)
{
    isPlaying = playing;

    if (isPlaying)
    {
        playButton.setButtonText("Pause");
        playButton.setColour(juce::TextButton::buttonColourId, juce::Colours::hotpink);
    }
    else
    {
        playButton.setButtonText("Play");
        playButton.setColour(juce::TextButton::buttonColourId, juce::Colours::lightblue);
    }
}

void PlayerGUI::setVolumeLevel(float volume)
{
    volumeSlider.setValue(volume, juce::dontSendNotification);
}

// NEW: Mute state setter
void PlayerGUI::setMuteState(bool isMuted)
{
    this->isMuted = isMuted;

    if (isMuted)
    {
        muteButton.setButtonText("Mute: ON");
        muteButton.setColour(juce::TextButton::buttonColourId, juce::Colours::hotpink);
    }
    else
    {
        muteButton.setButtonText("Mute: OFF");
        muteButton.setColour(juce::TextButton::buttonColourId, juce::Colours::lightblue);
    }
}
