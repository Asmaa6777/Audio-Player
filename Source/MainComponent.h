#pragma once

#include <JuceHeader.h>
#include "PlayerAudio.h"
#include "PlayerGUI.h"

class MainComponent : public juce::AudioAppComponent,
    public PlayerGUI::Listener,
    public juce::Timer
{
public:
    MainComponent();
    ~MainComponent() override;

    // ========== Component pure virtual functions ==========
    void paint(juce::Graphics& g) override;
    void resized() override;

    // ========== AudioAppComponent pure virtual functions ==========
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    // ========== PlayerGUI::Listener callbacks - ALL REQUIRED ==========
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
    void positionSliderMoved(double newSeconds) override;
    void markerAButtonClicked() override;
    void markerBButtonClicked() override;
    void clearMarkersButtonClicked() override;
    void segmentLoopButtonClicked() override;
    void sliceButtonClicked() override;
    void saveSliceButtonClicked() override; 

    // ========== Timer pure virtual function ==========
    void timerCallback() override;

    // ========== Other methods ==========
    void SaveState();
    void RestoreState();

private:
    void toggleMute();
    void updateSliceState();

    PlayerAudio player;
    PlayerGUI playerGUI;
    std::unique_ptr<juce::FileChooser> fileChooser;
    std::unique_ptr<juce::PropertiesFile> propertiesFile;

    bool isMuted = false;
    float previousVolume = 1.0f;

    juce::AudioFormatManager formatManager;
    juce::AudioThumbnailCache thumbnailCache{ 5 };
    juce::AudioThumbnail thumbnail{ 512, formatManager, thumbnailCache };

    bool showWaveform = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};