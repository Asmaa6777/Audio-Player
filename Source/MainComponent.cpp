#include "MainComponent.h"

MainComponent::MainComponent()
{
    formatManager.registerBasicFormats();

    // Set up the GUI
    playerGUI.setListener(this);
    addAndMakeVisible(playerGUI);

    setSize(600, 200);  // CHANGED: Slightly wider for the extra button
    setAudioChannels(0, 2);
}

MainComponent::~MainComponent()
{
    shutdownAudio();
}

void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    transportSource.getNextAudioBlock(bufferToFill);
}

void MainComponent::releaseResources()
{
    transportSource.releaseResources();
}

void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);
}

void MainComponent::resized()
{
    playerGUI.setBounds(getLocalBounds());
}

// PlayerGUI::Listener implementations
void MainComponent::loadButtonClicked()
{
    fileChooser = std::make_unique<juce::FileChooser>(
        "Select an audio file...",
        juce::File{},
        "*.wav;*.mp3;*.aiff");

    fileChooser->launchAsync(
        juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
        [this](const juce::FileChooser& fc)
        {
            auto file = fc.getResult();
            if (file.existsAsFile())
            {
                loadFile(file);
            }
        });
}

void MainComponent::playButtonClicked()
{
    if (transportSource.isPlaying())
    {
        transportSource.stop();
        playerGUI.setPlaybackState(false);
    }
    else
    {
        transportSource.start();
        playerGUI.setPlaybackState(true);
    }
}

void MainComponent::stopButtonClicked()
{
    transportSource.stop();
    transportSource.setPosition(0.0);
    playerGUI.setPlaybackState(false);
}

void MainComponent::restartButtonClicked()
{
    transportSource.setPosition(0.0);
    if (!transportSource.isPlaying())
    {
        transportSource.start();
        playerGUI.setPlaybackState(true);
    }
}

void MainComponent::loopButtonClicked()
{
    if (readerSource != nullptr)
    {
        bool shouldLoop = !readerSource->isLooping();
        readerSource->setLooping(shouldLoop);
        playerGUI.setLoopState(shouldLoop);
    }
}

// NEW: Mute button handler
void MainComponent::muteButtonClicked()
{
    toggleMute();
}

void MainComponent::volumeChanged(float newVolume)
{
    if (!isMuted)  // Only update actual volume if not muted
    {
        transportSource.setGain(newVolume);
        previousVolume = newVolume;  // Store the volume for unmute
    }
    playerGUI.setVolumeLevel(newVolume);
}

void MainComponent::loadFile(const juce::File& file)
{
    if (auto* reader = formatManager.createReaderFor(file))
    {
        // Stop and clear current source
        transportSource.stop();
        transportSource.setSource(nullptr);
        readerSource.reset();

        // Create new reader source
        readerSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);

        // Set looping based on current GUI state
        readerSource->setLooping(false);
        playerGUI.setLoopState(false);

        // Reset mute state when loading new file
        if (isMuted)
        {
            toggleMute();  // Unmute if currently muted
        }

        // Attach to transport
        transportSource.setSource(readerSource.get(),
            0,
            nullptr,
            reader->sampleRate);

        playerGUI.setPlaybackState(false);
    }
}

// NEW: Mute toggle function
void MainComponent::toggleMute()
{
    isMuted = !isMuted;

    if (isMuted)
    {
        // Store current volume and set to 0
        previousVolume = transportSource.getGain();
        transportSource.setGain(0.0f);
        playerGUI.setVolumeLevel(0.0f);  // Update slider to show 0
    }
    else
    {
        // Restore previous volume
        transportSource.setGain(previousVolume);
        playerGUI.setVolumeLevel(previousVolume);  // Update slider to show restored volume
    }


    playerGUI.setMuteState(isMuted);
}