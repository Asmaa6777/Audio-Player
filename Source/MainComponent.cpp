#include "MainComponent.h"
MainComponent::MainComponent()
{
    addAndMakeVisible(playerGUI);
    playerGUI.setListener(this);
    
    setAudioChannels(0, 2);
   
    setSize(800, 250);
}
MainComponent::~MainComponent()
{
    shutdownAudio();
} 
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
 
void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);
}
void MainComponent::resized()
{
    playerGUI.setBounds(getLocalBounds());
}
 
void MainComponent::loadButtonClicked()
{
    fileChooser = std::make_unique<juce::FileChooser>(
        "Select an audio file...", juce::File{}, "*.wav;*.mp3;*.aiff");
    fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
        [this](const juce::FileChooser& fc)
        {
            auto file = fc.getResult();
            if (file.existsAsFile())
                player.loadFile(file);
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