#pragma once
#include <JuceHeader.h>
#include "PlayerAudio.h"
#include "PlayerGUI.h"

class MainComponent : public juce::AudioAppComponent,
    public PlayerGUI::Listener,
    public juce::ChangeListener,
    public juce::Timer,
    public juce::ListBoxModel
{
public:
    MainComponent();
    ~MainComponent() override;

    // Audio callbacks
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    // GUI
    void paint(juce::Graphics&) override;
    void resized() override;

    // PlayerGUI::Listener implementations
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

    // New marker functions
    void addMarkerButtonClicked() override;
    void deleteMarkerButtonClicked() override;
    void jumpToMarker(int index) override;

    void timerCallback() override;

    // ListBoxModel implementation for markers
    int getNumRows() override;
    void paintListBoxItem(int rowNumber, juce::Graphics& g,
        int width, int height, bool rowIsSelected) override;
    void listBoxItemClicked(int row, const juce::MouseEvent& event) override;

    // ChangeListener for marker updates
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;

private:
    // New: Two players for multiple players feature
    PlayerAudio player1;
    PlayerAudio player2;
    PlayerGUI playerGUI1;
    PlayerGUI playerGUI2;

    juce::AudioFormatManager formatManager;
    juce::AudioThumbnailCache thumbnailCache{ 1 };
    juce::AudioThumbnail thumbnail{ 512, formatManager, thumbnailCache };
    bool showWaveform = false;

    std::unique_ptr<juce::FileChooser> fileChooser;
    std::unique_ptr<juce::PropertiesFile> propertiesFile;

    bool isMuted = false;
    float previousVolume = 0.5f;

    // New: Track which player is active
    int activePlayer = 1;

    void toggleMute();
    void SaveState();
    void RestoreState();
    void updateSliceState();
    void updateMetadataDisplay();

    // New: Audio mixing for multiple players
    juce::MixerAudioSource mixerAudioSource;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};