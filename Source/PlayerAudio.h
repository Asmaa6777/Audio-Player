#pragma once
#include <JuceHeader.h>
class PlayerAudio
{
public:
    PlayerAudio();
    ~PlayerAudio();
    void loadFile(const juce::File& audioFile);
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill);
    void releaseResources();
    void play();
    void stop();
    void restart();
    bool toggleLooping();
    bool isPlaying() const;
    void backward(double seconds);
    void forward(double seconds);
    void setVolume(float newVolume);
    float getVolume() const;
    void goToEnd();
private:
    juce::AudioFormatManager formatManager;
    juce::AudioTransportSource transportSource;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    bool isLooping = false;
    float currentVolume = 1.0f;
};