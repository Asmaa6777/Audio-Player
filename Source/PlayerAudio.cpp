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
        // Safely stop playback and clear previous source
        transportSource.stop();
        transportSource.setSource(nullptr);
        readerSource.reset();
        readerSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);
        readerSource->setLooping(isLooping);
        transportSource.setSource(readerSource.get(), 0, nullptr, reader->sampleRate);
        currentFile = audioFile; 
    }
}
void PlayerAudio::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}
void PlayerAudio::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    transportSource.getNextAudioBlock(bufferToFill);
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
    currentVolume = juce::jlimit(0.0f, 1.0f, newVolume);  // clamp for safety
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
