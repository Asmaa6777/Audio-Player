#pragma once

#include <JuceHeader.h>


class PlayerGUI : public juce::Component,
    public juce::Button::Listener,
    public juce::Slider::Listener
{
public:
    class Listener
    {
    public:
        virtual ~Listener() = default;
        virtual void loadButtonClicked() = 0;
        virtual void playButtonClicked() = 0;
        virtual void stopButtonClicked() = 0;
        virtual void restartButtonClicked() = 0;
        virtual void loopButtonClicked() = 0;
        virtual void muteButtonClicked() = 0;
        virtual void volumeChanged(float newVolume) = 0;
        virtual void forwardButtonClicked() = 0;
        virtual void backwardButtonClicked() = 0;
    };

    PlayerGUI();
    ~PlayerGUI() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    void buttonClicked(juce::Button* button) override;
    void sliderValueChanged(juce::Slider* slider) override;

    void setListener(Listener* newListener);
    void setLoopState(bool isLooping);
    void setPlaybackState(bool isPlaying);
    void setVolumeLevel(float volume);
    void setMuteState(bool isMuted);

private:
    juce::TextButton loadButton{ "Load File" };
    juce::TextButton playButton{ "Play" };
    juce::TextButton stopButton{ "Stop" };
    juce::TextButton restartButton{ "Restart" };
    juce::TextButton loopButton{ "Lopp off" };
    juce::TextButton muteButton{ "Mute off" };
    juce::TextButton forwardButton{ ">> 10s" };
    juce::TextButton backwardButton{ "<< 10s" };

    juce::Slider volumeSlider;
    juce::Label volumeLabel{ "Volume" };

    Listener* listener = nullptr;

    bool isPlaying = false;
    bool isMuted = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerGUI)
};
