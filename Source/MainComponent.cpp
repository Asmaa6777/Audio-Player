#include "MainComponent.h"
#include <juce_data_structures/juce_data_structures.h>

MainComponent::MainComponent()
{
    juce::PropertiesFile::Options options;
    options.applicationName = "MyAudioPlayer";
    options.filenameSuffix = ".settings";
    options.osxLibrarySubFolder = "Application Support";
    options.folderName = "MyAudioPlayerData";
    options.storageFormat = juce::PropertiesFile::storeAsXML;

    propertiesFile = std::make_unique<juce::PropertiesFile>(options);

    formatManager.registerBasicFormats();

    RestoreState();

    startTimerHz(10);

    addAndMakeVisible(playerGUI);
    playerGUI.setListener(this);

    // Set the MainComponent as the model for the markers list
    playerGUI.getMarkersList().setModel(this);

    // Listen for marker changes from PlayerAudio
    player.addChangeListener(this);

    setAudioChannels(0, 2);
    setSize(800, 600); // Increased height to accommodate markers section
}

MainComponent::~MainComponent()
{
    SaveState();
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
    // === Galaxy Background ===
    juce::ColourGradient bgGradient(
        juce::Colour::fromRGB(5, 5, 15), 0, 0,
        juce::Colour::fromRGB(20, 10, 35), 0, (float)getHeight(),
        false);
    g.setGradientFill(bgGradient);
    g.fillAll();

    // === Hundreds of Small Stars ===
    g.setColour(juce::Colours::white);
    for (int i = 0; i < 300; ++i)
    {
        float x = juce::Random::getSystemRandom().nextFloat() * getWidth();
        float y = juce::Random::getSystemRandom().nextFloat() * getHeight();
        float brightness = juce::Random::getSystemRandom().nextFloat() * 0.9f + 0.1f;
        float size = juce::Random::getSystemRandom().nextFloat() * 1.5f + 0.5f;

        juce::Colour starColour = juce::Colours::white.withBrightness(brightness);
        g.setColour(starColour);
        g.fillEllipse(x, y, size, size);
    }

    // === Add Galactic Nebula Effects ===
    g.setColour(juce::Colour::fromRGBA(80, 60, 150, 30));
    g.fillEllipse(getWidth() * 0.3f, getHeight() * 0.2f, 250, 150);

    g.setColour(juce::Colour::fromRGBA(120, 80, 180, 25));
    g.fillEllipse(getWidth() * 0.6f, getHeight() * 0.6f, 300, 200);

    // === Waveform Area ===
    auto waveformArea = getLocalBounds().removeFromTop(70).reduced(20, 8);

    if (showWaveform && thumbnail.getTotalLength() > 0.0)
    {
        // Subtle border
        g.setColour(juce::Colours::white.withAlpha(0.2f));
        g.drawRoundedRectangle(waveformArea.toFloat(), 4.0f, 1.0f);

        // Draw waveform
        juce::ColourGradient waveformGradient(
            juce::Colours::cyan.withAlpha(0.8f), (float)waveformArea.getX(), (float)waveformArea.getCentreY(),
            juce::Colours::magenta.withAlpha(0.8f), (float)waveformArea.getRight(), (float)waveformArea.getCentreY(),
            false);
        g.setGradientFill(waveformGradient);

        auto waveformDrawArea = waveformArea.reduced(4, 10);
        thumbnail.drawChannel(g, waveformDrawArea, 0.0, thumbnail.getTotalLength(), 0, 0.85f);

        // Draw markers on the waveform
        auto& markers = player.getMarkers();
        double totalLength = thumbnail.getTotalLength();

        for (const auto& marker : markers)
        {
            float xPos = (float)(marker.time / totalLength) * waveformArea.getWidth() + waveformArea.getX();

            // Draw marker line
            g.setColour(juce::Colours::yellow);
            g.drawLine(xPos, (float)waveformArea.getY(), xPos, (float)waveformArea.getBottom(), 2.0f);

            // Draw marker dot
            g.setColour(juce::Colours::orange);
            g.fillEllipse(xPos - 3, waveformArea.getBottom() - 8, 6, 6);
        }

        // Draw current playback cursor (after markers so it's on top)
        double position = player.getCurrentPosition();
        double proportion = (totalLength > 0.0) ? (position / totalLength) : 0.0;
        float x = (float)waveformArea.getX() + (float)(proportion * waveformArea.getWidth());

        g.setColour(juce::Colours::white);
        g.drawLine(x, (float)waveformArea.getY(), x, (float)waveformArea.getBottom(), 2.0f);
    }
    else
    {
        // No file loaded
        g.setColour(juce::Colours::white.withAlpha(0.4f));
        g.setFont(14.0f);
        g.drawText("Load audio file to see waveform", waveformArea, juce::Justification::centred);
    }

    // === Tiny Cosmic Dust ===
    g.setColour(juce::Colours::white.withAlpha(0.15f));
    for (int i = 0; i < 400; ++i)
    {
        float x = juce::Random::getSystemRandom().nextFloat() * getWidth();
        float y = juce::Random::getSystemRandom().nextFloat() * getHeight();
        g.fillEllipse(x, y, 0.3f, 0.3f);
    }

    // === Galactic Border ===
    juce::ColourGradient borderGradient(
        juce::Colours::indigo.withAlpha(0.7f), 0, 0,
        juce::Colour::fromRGB(100, 60, 150).withAlpha(0.7f), 0, (float)getHeight(),
        false);
    g.setGradientFill(borderGradient);
    g.drawRoundedRectangle(getLocalBounds().toFloat().reduced(1.0f), 8.0f, 1.5f);
}

void MainComponent::resized()
{
    // PlayerGUI takes the middle area (below waveform, above bottom)
    playerGUI.setBounds(getLocalBounds().withTrimmedTop(110).withTrimmedBottom(10));
}

// GUI Button Actions
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

                // Load waveform
                thumbnail.setSource(new juce::FileInputSource(file));
                showWaveform = true;

                // Reset slice state when new file is loaded
                updateSliceState();

                repaint();
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

void MainComponent::timerCallback()
{
    repaint();

    double total = player.getLengthInSeconds();
    double current = player.getCurrentPosition();

    if (!std::isfinite(total) || total <= 0.0)
        total = 0.0;

    playerGUI.updatePositionDisplay(current, total);
    playerGUI.setPlaybackState(player.isPlaying());

    playerGUI.setMarkerAState(player.getMarkerA() >= 0);
    playerGUI.setMarkerBState(player.getMarkerB() >= 0);
    playerGUI.setSegmentLoopState(player.isSegmentLooping());
}

void MainComponent::SaveState()
{
    if (propertiesFile)
        player.SaveState(*propertiesFile, "player");
}

void MainComponent::RestoreState()
{
    if (propertiesFile)
    {
        player.RestoreState(*propertiesFile, "player");

        juce::String lastFilePath = propertiesFile->getValue("player_lastFile");
        juce::File lastFile(lastFilePath);

        if (lastFile.existsAsFile())
        {
            thumbnail.setSource(new juce::FileInputSource(lastFile));
            showWaveform = true;
        }
        else
        {
            showWaveform = false;
        }
    }
}

void MainComponent::markerAButtonClicked()
{
    player.setMarkerA();
    playerGUI.setMarkerAState(player.getMarkerA() >= 0);
    updateSliceState();
}

void MainComponent::markerBButtonClicked()
{
    player.setMarkerB();
    playerGUI.setMarkerBState(player.getMarkerB() >= 0);
    updateSliceState();
}

void MainComponent::clearMarkersButtonClicked()
{
    player.clearMarkers();
    playerGUI.setMarkerAState(false);
    playerGUI.setMarkerBState(false);
    playerGUI.setSegmentLoopState(false);
    updateSliceState();
}

void MainComponent::segmentLoopButtonClicked()
{
    bool newState = !player.isSegmentLooping();
    player.setSegmentLooping(newState);
    playerGUI.setSegmentLoopState(newState);

    if (newState)
    {
        player.toggleLooping();
        playerGUI.setLoopState(false);
    }
}

// Slicing Functionality
void MainComponent::sliceButtonClicked()
{
    if (player.createSliceFromMarkers())
    {
        updateSliceState();
        juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::InfoIcon,
            "Slice Created",
            "Audio slice created successfully! Use the existing A-B loop to preview, then click Save Slice to export.");
    }
    else
    {
        juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon,
            "Slice Failed",
            "Could not create slice. Make sure you have:\n"
            "1. Loaded an audio file\n"
            "2. Set both A and B markers\n"
            "3. The A-B segment has valid duration");
    }
}

void MainComponent::saveSliceButtonClicked()
{
    if (!player.hasValidSlice())
    {
        juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon,
            "No Slice",
            "No slice available to save. Please create a slice first.");
        return;
    }

    fileChooser = std::make_unique<juce::FileChooser>(
        "Save audio slice as WAV file...",
        juce::File::getSpecialLocation(juce::File::userDocumentsDirectory),
        "*.wav");

    fileChooser->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::canSelectFiles,
        [this](const juce::FileChooser& chooser)
        {
            auto file = chooser.getResult();
            if (file != juce::File{})
            {
                if (!file.hasFileExtension(".wav"))
                {
                    file = file.withFileExtension(".wav");
                }

                if (player.saveSliceToFile(file))
                {
                    juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::InfoIcon,
                        "Slice Saved",
                        "Audio slice saved successfully to:\n" + file.getFullPathName());
                }
                else
                {
                    juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon,
                        "Save Failed",
                        "Failed to save audio slice. Please check file permissions and try again.");
                }
            }
        });
}

void MainComponent::updateSliceState()
{
    playerGUI.setSliceState(player.hasValidSlice());
}

// Track Markers Functionality
void MainComponent::addMarkerButtonClicked()
{
    double currentTime = player.getCurrentPosition();
    player.addMarker(currentTime);
}

void MainComponent::deleteMarkerButtonClicked()
{
    int selectedRow = playerGUI.getMarkersList().getSelectedRow();
    if (selectedRow >= 0)
    {
        player.removeMarker(selectedRow);
    }
}

void MainComponent::jumpToMarker(int index)
{
    player.jumpToMarker(index);
}

// ListBoxModel implementation for markers
int MainComponent::getNumRows()
{
    return player.getMarkers().size();
}

void MainComponent::paintListBoxItem(int rowNumber, juce::Graphics& g,
    int width, int height, bool rowIsSelected)
{
    auto& markers = player.getMarkers();

    if (rowNumber < markers.size())
    {
        if (rowIsSelected)
            g.fillAll(juce::Colour(0xFF9370DB)); // Use your active colour

        g.setColour(juce::Colour(0xFFB0AFFF)); // Use your text colour

        // Format time as MM:SS
        int minutes = static_cast<int>(markers[rowNumber].time) / 60;
        int seconds = static_cast<int>(markers[rowNumber].time) % 60;
        juce::String timeString = juce::String::formatted("%02d:%02d", minutes, seconds);

        g.drawText(markers[rowNumber].name + " - " + timeString,
            5, 0, width - 5, height,
            juce::Justification::centredLeft);
    }
}

void MainComponent::listBoxItemClicked(int row, const juce::MouseEvent& event)
{
    jumpToMarker(row);
}

void MainComponent::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    if (source == &player)
    {
        // Update the markers list when markers change
        playerGUI.getMarkersList().updateContent();
        playerGUI.getMarkersList().repaint();
    }
}