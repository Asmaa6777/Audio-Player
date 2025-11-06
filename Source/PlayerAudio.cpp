#include "PlayerAudio.h"

PlayerAudio::PlayerAudio()
{
    formatManager.registerBasicFormats();
}

PlayerAudio::~PlayerAudio()
{
    transportSource.setSource(nullptr);
}

void PlayerAudio::loadFile(const juce::File& audioFile)
{
    if (auto* reader = formatManager.createReaderFor(audioFile))
    {
        transportSource.stop();
        transportSource.setSource(nullptr);
        readerSource.reset();
        readerSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);
        readerSource->setLooping(isLooping);
        transportSource.setSource(readerSource.get(), 0, nullptr, reader->sampleRate);
        currentFile = audioFile;

        // Reset slice state when new file is loaded
        sliceReady = false;
        audioSlice.setSize(0, 0);

        // Clear markers when new file is loaded
        clearAllMarkers();
    }
}

void PlayerAudio::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void PlayerAudio::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    transportSource.getNextAudioBlock(bufferToFill);
    checkSegmentLooping();
}

void PlayerAudio::releaseResources()
{
    transportSource.releaseResources();
}

void PlayerAudio::play()
{
    if (!transportSource.isPlaying())
        transportSource.start();
}

void PlayerAudio::stop()
{
    if (transportSource.isPlaying())
        transportSource.stop();
}

void PlayerAudio::restart()
{
    transportSource.setPosition(0.0);
    transportSource.start();
}

bool PlayerAudio::toggleLooping()
{
    isLooping = !isLooping;
    if (readerSource)
        readerSource->setLooping(isLooping);
    return isLooping;
}

bool PlayerAudio::isPlaying() const
{
    return transportSource.isPlaying();
}

void PlayerAudio::setVolume(float newVolume)
{
    currentVolume = juce::jlimit(0.0f, 1.0f, newVolume);
    transportSource.setGain(currentVolume);
}

float PlayerAudio::getVolume() const
{
    return currentVolume;
}

void PlayerAudio::backward(double seconds)
{
    auto newPosition = juce::jmax(0.0, transportSource.getCurrentPosition() - seconds);
    transportSource.setPosition(newPosition);
}

void PlayerAudio::forward(double seconds)
{
    if (readerSource != nullptr)
    {
        auto totalLength = readerSource->getTotalLength() / readerSource->getAudioFormatReader()->sampleRate;
        auto newPosition = juce::jmin(totalLength, transportSource.getCurrentPosition() + seconds);
        transportSource.setPosition(newPosition);
    }
}

void PlayerAudio::goToEnd()
{
    if (readerSource != nullptr)
    {
        double totalLength = transportSource.getLengthInSeconds();
        transportSource.setPosition(juce::jmax(0.0, totalLength - 0.1));
    }
}

void PlayerAudio::SaveState(juce::PropertiesFile& props, const juce::String& keyPrefix)
{
    props.setValue(keyPrefix + "_lastFile", currentFile.getFullPathName());
    props.setValue(keyPrefix + "_lastPosition", transportSource.getCurrentPosition());
    props.saveIfNeeded();
}

void PlayerAudio::RestoreState(juce::PropertiesFile& props, const juce::String& keyPrefix)
{
    juce::String lastFile = props.getValue(keyPrefix + "_lastFile", "");
    if (lastFile.isNotEmpty())
    {
        juce::File fileToLoad(lastFile);
        if (fileToLoad.existsAsFile())
        {
            loadFile(fileToLoad);
            double lastPosition = props.getDoubleValue(keyPrefix + "_lastPosition", 0.0);
            transportSource.setPosition(lastPosition);
        }
    }
}

// A-B Segment Looping
void PlayerAudio::setMarkerA()
{
    markerA = transportSource.getCurrentPosition();
    if (markerB > 0 && markerA > markerB)
    {
        std::swap(markerA, markerB);
    }
}

void PlayerAudio::setMarkerB()
{
    markerB = transportSource.getCurrentPosition();
    if (markerA > 0 && markerB < markerA)
    {
        std::swap(markerA, markerB);
    }
}

void PlayerAudio::clearMarkers()
{
    markerA = -1.0;
    markerB = -1.0;
    segmentLooping = false;
}

void PlayerAudio::setSegmentLooping(bool shouldLoop)
{
    segmentLooping = shouldLoop && hasMarkers();
}

void PlayerAudio::checkSegmentLooping()
{
    if (segmentLooping && hasMarkers() && transportSource.isPlaying())
    {
        double currentPos = transportSource.getCurrentPosition();
        if (currentPos >= markerB)
        {
            transportSource.setPosition(markerA);
        }
    }
}

// Audio Slicing Implementation
bool PlayerAudio::createSliceFromMarkers()
{
    if (!hasMarkers() || readerSource == nullptr)
        return false;

    sliceStart = markerA;
    sliceEnd = markerB;

    auto* reader = readerSource->getAudioFormatReader();
    if (reader == nullptr)
        return false;

    // Calculate slice parameters
    int sampleRate = reader->sampleRate;
    int startSample = (int)(sliceStart * sampleRate);
    int numSamples = (int)((sliceEnd - sliceStart) * sampleRate);

    if (numSamples <= 0)
        return false;

    // Create audio buffer for the slice
    audioSlice.setSize(reader->numChannels, numSamples);

    // Read the audio data into the buffer
    if (!reader->read(&audioSlice, 0, numSamples, startSample, true, true))
        return false;

    sliceReady = true;
    return true;
}

bool PlayerAudio::saveSliceToFile(const juce::File& outputFile)
{
    if (!sliceReady || audioSlice.getNumSamples() == 0)
        return false;

    // Create WAV writer
    juce::WavAudioFormat wavFormat;
    std::unique_ptr<juce::AudioFormatWriter> writer;

    auto outputStream = outputFile.createOutputStream();
    if (outputStream == nullptr)
        return false;

    auto* reader = readerSource->getAudioFormatReader();
    if (reader == nullptr)
        return false;

    writer.reset(wavFormat.createWriterFor(outputStream.release(),
        reader->sampleRate,
        audioSlice.getNumChannels(),
        16,
        juce::StringPairArray(),
        0));

    if (writer != nullptr)
    {
        bool success = writer->writeFromAudioSampleBuffer(audioSlice, 0, audioSlice.getNumSamples());
        return success;
    }

    return false;
}

bool PlayerAudio::hasValidSlice() const
{
    return sliceReady && audioSlice.getNumSamples() > 0;
}

juce::String PlayerAudio::getSliceInfo() const
{
    if (!sliceReady)
        return "No slice created";

    double duration = sliceEnd - sliceStart;
    return juce::String::formatted("Slice: %.1fs - %.1fs (%.1fs)",
        sliceStart, sliceEnd, duration);
}

// Track Markers Implementation
void PlayerAudio::addMarker(double time, const juce::String& name) {
    Marker newMarker;
    newMarker.time = time;

    if (name.isEmpty()) {
        newMarker.name = "Marker " + juce::String(markers.size() + 1);
    }
    else {
        newMarker.name = name;
    }

    markers.add(newMarker);
    std::sort(markers.begin(), markers.end());
    sendChangeMessage(); // Notify GUI to update
}

void PlayerAudio::removeMarker(int index) {
    if (index >= 0 && index < markers.size()) {
        markers.remove(index);
        sendChangeMessage(); // Notify GUI to update
    }
}

void PlayerAudio::jumpToMarker(int index) {
    if (index >= 0 && index < markers.size()) {
        transportSource.setPosition(markers[index].time);
    }
}

void PlayerAudio::clearAllMarkers() {
    markers.clear();
    sendChangeMessage(); // Notify GUI to update
}

juce::String PlayerAudio::getMarkerInfo(int index) const {
    if (index >= 0 && index < markers.size()) {
        int minutes = static_cast<int>(markers[index].time) / 60;
        int seconds = static_cast<int>(markers[index].time) % 60;
        juce::String timeString = juce::String::formatted("%02d:%02d", minutes, seconds);
        return markers[index].name + " (" + timeString + ")";
    }
    return "";
}