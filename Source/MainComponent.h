#pragma once

#include <JuceHeader.h>
#include "PlayerGUI.h"


class MainComponent : public juce::AudioAppComponent,
    public PlayerGUI::Listener
{
public:
    MainComponent();
    ~MainComponent() override;


    // Audio
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    // GUI
    void paint(juce::Graphics& g) override;
    void resized() override;

    // PlayerGUI::Listener implementations
    void loadButtonClicked() override;
    void playButtonClicked() override;
    void stopButtonClicked() override;
    void restartButtonClicked() override;
    void loopButtonClicked() override;
    void muteButtonClicked() override;  // NEW: Mute button handler
    void volumeChanged(float newVolume) override;

private:
    // Audio
    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource transportSource;

    // GUI
    PlayerGUI playerGUI;

    // File handling
    std::unique_ptr<juce::FileChooser> fileChooser;

    // Mute functionality
    bool isMuted = false;  // NEW: Mute state
    float previousVolume = 0.5f;  // NEW: Store volume before mute

    void loadFile(const juce::File& file);
    void toggleMute();  // NEW: Mute toggle function


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};