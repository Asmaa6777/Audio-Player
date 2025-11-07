#pragma once
#include <JuceHeader.h>

class PlayerGUI : public juce::Component,
    public juce::Button::Listener,
    public juce::Slider::Listener,
    public juce::ChangeListener,
    public juce::ComboBox::Listener
{
public:
    class Listener
    {
    public:
        virtual ~Listener() = default;
        virtual void loadButtonClicked() = 0;
        virtual void loadSecondTrackButtonClicked() = 0;
        virtual void playButtonClicked() = 0;
        virtual void stopButtonClicked() = 0;
        virtual void restartButtonClicked() = 0;
        virtual void loopButtonClicked() = 0;
        virtual void muteButtonClicked() = 0;
        virtual void volumeChanged(float newVolume) = 0;
        virtual void speedChanged(float newSpeed) = 0;
        virtual void forwardButtonClicked() = 0;
        virtual void backwardButtonClicked() = 0;
        virtual void goToEndButtonClicked() = 0;
        virtual void positionSliderMoved(double newSeconds) = 0;
        virtual void markerAButtonClicked() = 0;
        virtual void markerBButtonClicked() = 0;
        virtual void clearMarkersButtonClicked() = 0;
        virtual void segmentLoopButtonClicked() = 0;
        virtual void sliceButtonClicked() = 0;
        virtual void saveSliceButtonClicked() = 0;
        virtual void addMarkerButtonClicked() = 0;
        virtual void deleteMarkerButtonClicked() = 0;
        virtual void jumpToMarker(int index) = 0;

        virtual void loadPlaylistButtonClicked() = 0;
        virtual void prevTrackButtonClicked() = 0;
        virtual void nextTrackButtonClicked() = 0;
        virtual void playlistBoxChanged(int selectedId) = 0;
    };

    PlayerGUI();
    ~PlayerGUI() override;

    void setSliceState(bool hasSlice);

    void resized() override;

    void buttonClicked(juce::Button* button) override;
    void sliderValueChanged(juce::Slider* slider) override;
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;
    void comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) override;

    void setListener(Listener* newListener);

    void setLoopState(bool isLooping);
    void setPlaybackState(bool isPlaying);
    void setVolumeLevel(float volume);
    void setMuteState(bool isMuted);
    void updatePositionDisplay(double currentSeconds, double totalSeconds);
    void setMarkerAState(bool isSet);
    void setMarkerBState(bool isSet);
    void setSegmentLoopState(bool isActive);

    juce::ListBox& getMarkersList() { return markersList; }

    void setMetadataDisplay(const juce::String& metadataText);

    void loadButtonIcons();
    juce::ComboBox& getPlaylistBox() { return playlistBox; }

private:
    juce::DrawableButton muteButton{ "Mute",     juce::DrawableButton::ImageFitted };
    juce::DrawableButton restartButton{ "Restart",  juce::DrawableButton::ImageFitted };
    juce::DrawableButton backwardButton{ "Back10",   juce::DrawableButton::ImageFitted };
    juce::DrawableButton playButton{ "Play",     juce::DrawableButton::ImageFitted };
    juce::DrawableButton forwardButton{ "Forward10",juce::DrawableButton::ImageFitted };
    juce::DrawableButton goToEndButton{ "GoToEnd",  juce::DrawableButton::ImageFitted };
    juce::DrawableButton loopButton{ "Loop",     juce::DrawableButton::ImageFitted };

    juce::TextButton loadButton{ "Load Track 1" };
    juce::TextButton loadSecondTrackButton{ "Load Track 2" };
    juce::TextButton stopButton{ "Stop" };

    juce::Slider volumeSlider;
    juce::Label  volumeLabel{ "Volume" };
    juce::Slider speedSlider;
    juce::Label  speedLabel{ "Speed" };

    juce::Slider positionSlider;
    juce::Label currentTimeLabel;
    juce::Label totalTimeLabel;
    double lastTotalSeconds = 0.0;
    bool isPositionDragging = false;

    Listener* listener = nullptr;
    bool isPlaying = false;
    bool isMuted = false;
    bool isLooping = false;

    juce::TextButton markerAButton{ "set A" };
    juce::TextButton markerBButton{ "set B" };
    juce::TextButton clearMarkersButton{ "Clear" };
    juce::TextButton segmentLoopButton{ "A-B Loop" };

    juce::TextButton sliceButton{ "Create A-B Slice" };
    juce::TextButton saveSliceButton{ "Save Slice" };
    juce::Label sliceInfoLabel;

    juce::TextButton addMarkerButton{ "Add Marker" };
    juce::TextButton deleteMarkerButton{ "Delete Marker" };
    juce::ListBox markersList;

    juce::Label metadataLabel;


    juce::TextButton loadPlaylistButton{ "Load Playlist" };
    juce::TextButton prevTrackButton{ "Prev Track" };
    juce::TextButton nextTrackButton{ "Next Track" };
    juce::ComboBox playlistBox;
    juce::Label playlistLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerGUI)
};