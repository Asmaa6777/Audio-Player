#pragma once
#include <JuceHeader.h>
#include "PlayerAudio.h"
#include "PlayerGUI.h"
class MainComponent : public juce::AudioAppComponent,
    public PlayerGUI::Listener
{
public:
    MainComponent();
    ~MainComponent() override;
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;
    void paint(juce::Graphics&) override;
    void resized() override;
    // GUI button callbacks
    void loadButtonClicked() override;
    void playButtonClicked() override;
    void stopButtonClicked() override;
    void restartButtonClicked() override;
    void loopButtonClicked() override;
    void muteButtonClicked() override;
    void volumeChanged(float newVolume) override;
    void forwardButtonClicked() override;
    void backwardButtonClicked() override;
    void goToEndButtonClicked() override;
private:
    void toggleMute();
    PlayerAudio player;
    PlayerGUI playerGUI;
    std::unique_ptr<juce::FileChooser> fileChooser;
    bool isMuted = false;
    float previousVolume = 1.0f;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};