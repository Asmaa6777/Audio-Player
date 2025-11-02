#include "MainComponent.h"
#include <juce_data_structures/juce_data_structures.h>

// ========================
// Constructor
// ========================

MainComponent::MainComponent()
{
    // Create and configure the PropertiesFile for saving app state
    juce::PropertiesFile::Options options;
    options.applicationName = "MyAudioPlayer";
    options.filenameSuffix = ".settings";
    options.osxLibrarySubFolder = "Application Support"; // for macOS, ignored on Windows/Linux
    options.folderName = "MyAudioPlayerData";
    options.storageFormat = juce::PropertiesFile::storeAsXML;

    propertiesFile = std::make_unique<juce::PropertiesFile>(options);

    // Restore last session if available
    RestoreState();

    startTimerHz(10);
    addAndMakeVisible(playerGUI);
    playerGUI.setListener(this);

    setAudioChannels(0, 2);
    setSize(800, 250);
}

// ========================
// Destructor
// ========================
MainComponent::~MainComponent()
{
    // Save state before shutting down audio
    SaveState();

    shutdownAudio();
}

// ========================
// Audio setup
// ========================
void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    player.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    player.getNextAudioBlock(bufferToFill);
}

void MainComponent::releaseResources()
{
    player.releaseResources();
}

// ========================
// GUI
// ========================
void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xFFF5F0FF)); // very light lavender
}

void MainComponent::resized()
{
    playerGUI.setBounds(getLocalBounds());
}

// ========================
// GUI Button Actions
// ========================
void MainComponent::loadButtonClicked()
{
    fileChooser = std::make_unique<juce::FileChooser>(
        "Select an audio file...",
        juce::File{},
        "*.wav;*.mp3;*.aiff");

    fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
        [this](const juce::FileChooser& chooser)
        {
            auto file = chooser.getResult();
            if (file.existsAsFile())
            {
                player.loadFile(file);
                playerGUI.setPlaybackState(false);
                playerGUI.setLoopState(player.isLoopingEnabled());
                playerGUI.setMuteState(isMuted);
            }
        });
}

void MainComponent::playButtonClicked()
{
    if (player.isPlaying())
        player.stop();
    else
        player.play();

    playerGUI.setPlaybackState(player.isPlaying());
}

void MainComponent::stopButtonClicked()
{
    player.stop();
    playerGUI.setPlaybackState(false);
}

void MainComponent::restartButtonClicked()
{
    player.restart();
    playerGUI.setPlaybackState(true);
}

void MainComponent::loopButtonClicked()
{
    bool loop = player.toggleLooping();
    playerGUI.setLoopState(loop);
}

void MainComponent::muteButtonClicked()
{
    toggleMute();
}

void MainComponent::volumeChanged(float newVolume)
{
    if (!isMuted)
    {
        player.setVolume(newVolume);
        previousVolume = newVolume;
    }
}

void MainComponent::toggleMute()
{
    isMuted = !isMuted;

    if (isMuted)
    {
        previousVolume = player.getVolume();
        player.setVolume(0.0f);
    }
    else
    {
        player.setVolume(previousVolume);
    }

    playerGUI.setMuteState(isMuted);
}

void MainComponent::forwardButtonClicked()
{
    player.forward(10.0);
}

void MainComponent::backwardButtonClicked()
{
    player.backward(10.0);
}

void MainComponent::goToEndButtonClicked()
{
    player.goToEnd();
}

void MainComponent::positionSliderMoved(double newSeconds)
{
    player.setPosition(newSeconds);
    playerGUI.setPlaybackState(player.isPlaying());
}

// ========================
// Timer
// ========================
void MainComponent::timerCallback()
{
    double total = player.getLengthInSeconds();
    double current = player.getCurrentPosition();

    if (!std::isfinite(total) || total <= 0.0)
        total = 0.0;

    playerGUI.updatePositionDisplay(current, total);
    playerGUI.setPlaybackState(player.isPlaying());
}

// ========================
// State Saving / Loading
// ========================
void MainComponent::SaveState()
{
    if (propertiesFile)
        player.SaveState(*propertiesFile, "player");
}

void MainComponent::RestoreState()
{
    if (propertiesFile)
        player.RestoreState(*propertiesFile, "player");
}
