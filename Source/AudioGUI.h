#pragma once
#include <JuceHeader.h>
#include <vector>

class PlayerAudio : public juce::AudioSource
{
public:
    struct Marker {
        double timeInSeconds;
        juce::String label;
    };

    PlayerAudio();
    ~PlayerAudio() override;

    void loadFile(const juce::File& audioFile);
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    void play();
    void stop();
    void restart();
    bool toggleLooping();
    bool isPlaying() const;
    bool isLoopingEnabled() const { return isLooping; }

    void backward(double seconds);
    void forward(double seconds);
    void setVolume(float newVolume);
    float getVolume() const;
    void goToEnd();
    double getCurrentPosition() const;

    // Marker management
    void addMarker(double time, const juce::String& label);
    void removeMarker(int index);
    void clearMarkers();
    const std::vector<Marker>& getMarkers() const;
    void jumpToMarker(int index);

private:
    juce::AudioFormatManager formatManager;
    juce::AudioTransportSource transportSource;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;

    std::vector<Marker> markers;
    bool isLooping = false;
    float currentVolume = 1.0f;
};
