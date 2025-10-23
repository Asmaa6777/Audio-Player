#pragma once

#include <JuceHeader.h>

class PlayerGUI : public juce::Component,
    public juce::Button::Listener,
    public juce::Slider::Listener
{
public:
    // Listener interface for communication with parent

    class Listener
    {
    public:
        virtual ~Listener() = default;

        virtual void loadButtonClicked() = 0;
        virtual void playButtonClicked() = 0;
        virtual void stopButtonClicked() = 0;
        virtual void restartButtonClicked() = 0;
        virtual void loopButtonClicked() = 0;
        virtual void muteButtonClicked() = 0;  // NEW: Mute button
        virtual void volumeChanged(float newVolume) = 0;
    };

    PlayerGUI();
    ~PlayerGUI() override;

    // Component overrides
    void paint(juce::Graphics& g) override;
    void resized() override;

    // Listener overrides
    void buttonClicked(juce::Button* button) override;
    void sliderValueChanged(juce::Slider* slider) override;

    // Public API methods
    void setListener(Listener* newListener);
    void setLoopState(bool isLooping);
    void setPlaybackState(bool isPlaying);
    void setVolumeLevel(float volume);
    void setMuteState(bool isMuted);  // NEW: Mute state

private:
    // GUI Elements
    juce::TextButton loadButton{ "Load File" };
    juce::TextButton playButton{ "Play" };
    juce::TextButton stopButton{ "Stop" };
    juce::TextButton restartButton{ "Restart" };
    juce::TextButton loopButton{ "Loop: OFF" };
    juce::TextButton muteButton{ "Mute: OFF" };  // NEW: Mute button
    juce::Slider volumeSlider;
    juce::Label volumeLabel{ "Volume" };

    // Listener for callbacks
    Listener* listener = nullptr;

    // State
    bool isPlaying = false;
    bool isMuted = false;  // NEW: Mute state

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerGUI)
};