#pragma once
#include <JuceHeader.h>

class PlayerAudio
    : public juce::AudioSource,
    public juce::ChangeBroadcaster
{
public:
    PlayerAudio();
    ~PlayerAudio();

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
    void setSpeed(float newSpeed);
    float getSpeed() const { return currentSpeed; }
    void goToEnd();
    double getCurrentPosition() const { return transportSource.getCurrentPosition(); }
    double getLengthInSeconds() const { return transportSource.getLengthInSeconds(); }
    void setPosition(double seconds) { transportSource.setPosition(seconds); }

    void SaveState(juce::PropertiesFile& props, const juce::String& keyPrefix);
    void RestoreState(juce::PropertiesFile& props, const juce::String& keyPrefix);

    void setMarkerA();
    void setMarkerB();
    void clearMarkers();
    bool isSegmentLooping() const { return segmentLooping; }
    void setSegmentLooping(bool shouldLoop);
    double getMarkerA() const { return markerA; }
    double getMarkerB() const { return markerB; }
    bool hasMarkers() const { return markerA >= 0 && markerB > markerA; }
    void checkSegmentLooping();

    bool createSliceFromMarkers();
    bool saveSliceToFile(const juce::File& outputFile);
    bool hasValidSlice() const;
    juce::String getSliceInfo() const;

    struct Marker {
        double time;
        juce::String name;
        bool operator<(const Marker& other) const {
            return time < other.time;
        }
    };

    void addMarker(double time, const juce::String& name = "");
    void removeMarker(int index);
    void jumpToMarker(int index);
    void clearAllMarkers();
    const juce::Array<Marker>& getMarkers() const { return markers; }
    juce::String getMarkerInfo(int index) const;

    struct Metadata {
        juce::String title;
        juce::String artist;
        juce::String album;
        juce::String year;
        juce::String filename;
        double duration;

        Metadata() : title(""), artist(""), album(""), year(""), filename(""), duration(0.0) {}
    };

    Metadata getMetadata() const { return metadata; }

private:
    juce::AudioFormatManager formatManager;
    juce::AudioTransportSource transportSource;
    juce::ResamplingAudioSource resampleSource{ &transportSource, false, 2 };
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;

    bool isLooping = false;
    float currentVolume = 1.0f;
    float currentSpeed = 1.0f;

    double markerA = -1.0;
    double markerB = -1.0;
    bool segmentLooping = false;

    juce::File currentFile;


    juce::AudioBuffer<float> audioSlice;
    bool sliceReady = false;
    double sliceStart = 0.0;
    double sliceEnd = 0.0;

    juce::Array<Marker> markers;

    Metadata metadata;

    void extractMetadata(juce::AudioFormatReader* reader, const juce::File& audioFile);
    bool isValidAudioFile(const juce::File& file) const;
};