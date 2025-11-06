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

    // Set up both player GUIs
    addAndMakeVisible(playerGUI1);
    addAndMakeVisible(playerGUI2);
    playerGUI1.setListener(this);
    playerGUI2.setListener(this);

    // Set the MainComponent as the model for both markers lists
    playerGUI1.getMarkersList().setModel(this);
    playerGUI2.getMarkersList().setModel(this);

    // Listen for marker changes from both players
    player1.addChangeListener(this);
    player2.addChangeListener(this);

    // Set up audio mixing for multiple players
    mixerAudioSource.addInputSource(&player1, false);
    mixerAudioSource.addInputSource(&player2, false);

    setAudioChannels(0, 2);
    setSize(1000, 800); // Increased size for two players
}

MainComponent::~MainComponent()
{
    SaveState();
    shutdownAudio();
}

void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    player1.prepareToPlay(samplesPerBlockExpected, sampleRate);
    player2.prepareToPlay(samplesPerBlockExpected, sampleRate);
    mixerAudioSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    mixerAudioSource.getNextAudioBlock(bufferToFill);
}

void MainComponent::releaseResources()
{
    player1.releaseResources();
    player2.releaseResources();
    mixerAudioSource.releaseResources();
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
        auto& markers = player1.getMarkers(); // Using player1 markers for now
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
        double position = player1.getCurrentPosition(); // Using player1 position for now
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

    // === Player Labels ===
    g.setColour(juce::Colours::white.withAlpha(0.8f));
    g.setFont(16.0f);
    g.drawText("Player 1", playerGUI1.getBounds().getX(), playerGUI1.getBounds().getY() - 25,
        playerGUI1.getWidth(), 20, juce::Justification::centred);
    g.drawText("Player 2", playerGUI2.getBounds().getX(), playerGUI2.getBounds().getY() - 25,
        playerGUI2.getWidth(), 20, juce::Justification::centred);
}

void MainComponent::resized()
{
    auto area = getLocalBounds().withTrimmedTop(110).withTrimmedBottom(10);

    // Split area for two players
    auto player1Area = area.removeFromLeft(area.getWidth() / 2);
    auto player2Area = area;

    playerGUI1.setBounds(player1Area.reduced(5, 0));
    playerGUI2.setBounds(player2Area.reduced(5, 0));
}

// GUI Button Actions - Updated for multiple players
void MainComponent::loadButtonClicked()
{
    // Determine which player triggered the load
    activePlayer = 1; // Default to player 1

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
                if (activePlayer == 1)
                {
                    player1.loadFile(file);
                    playerGUI1.setPlaybackState(false);
                    playerGUI1.setLoopState(player1.isLoopingEnabled());
                    playerGUI1.setMuteState(isMuted);
                }
                else
                {
                    player2.loadFile(file);
                    playerGUI2.setPlaybackState(false);
                    playerGUI2.setLoopState(player2.isLoopingEnabled());
                    playerGUI2.setMuteState(isMuted);
                }

                // Load waveform (using player1 for now)
                thumbnail.setSource(new juce::FileInputSource(file));
                showWaveform = true;

                // Update metadata display
                updateMetadataDisplay();

                repaint();
            }
        });
}

void MainComponent::playButtonClicked()
{
    if (activePlayer == 1)
    {
        if (player1.isPlaying())
            player1.stop();
        else
            player1.play();
        playerGUI1.setPlaybackState(player1.isPlaying());
    }
    else
    {
        if (player2.isPlaying())
            player2.stop();
        else
            player2.play();
        playerGUI2.setPlaybackState(player2.isPlaying());
    }
}

void MainComponent::stopButtonClicked()
{
    if (activePlayer == 1)
    {
        player1.stop();
        playerGUI1.setPlaybackState(false);
    }
    else
    {
        player2.stop();
        playerGUI2.setPlaybackState(false);
    }
}

void MainComponent::restartButtonClicked()
{
    if (activePlayer == 1)
    {
        player1.restart();
        playerGUI1.setPlaybackState(true);
    }
    else
    {
        player2.restart();
        playerGUI2.setPlaybackState(true);
    }
}

void MainComponent::loopButtonClicked()
{
    if (activePlayer == 1)
    {
        bool loop = player1.toggleLooping();
        playerGUI1.setLoopState(loop);
    }
    else
    {
        bool loop = player2.toggleLooping();
        playerGUI2.setLoopState(loop);
    }
}

void MainComponent::muteButtonClicked()
{
    toggleMute();
}

void MainComponent::volumeChanged(float newVolume)
{
    if (!isMuted)
    {
        if (activePlayer == 1)
        {
            player1.setVolume(newVolume);
        }
        else
        {
            player2.setVolume(newVolume);
        }
        previousVolume = newVolume;
    }
}

void MainComponent::toggleMute()
{
    isMuted = !isMuted;

    if (activePlayer == 1)
    {
        if (isMuted)
        {
            previousVolume = player1.getVolume();
            player1.setVolume(0.0f);
        }
        else
        {
            player1.setVolume(previousVolume);
        }
        playerGUI1.setMuteState(isMuted);
    }
    else
    {
        if (isMuted)
        {
            previousVolume = player2.getVolume();
            player2.setVolume(0.0f);
        }
        else
        {
            player2.setVolume(previousVolume);
        }
        playerGUI2.setMuteState(isMuted);
    }
}

void MainComponent::forwardButtonClicked()
{
    if (activePlayer == 1)
        player1.forward(10.0);
    else
        player2.forward(10.0);
}

void MainComponent::backwardButtonClicked()
{
    if (activePlayer == 1)
        player1.backward(10.0);
    else
        player2.backward(10.0);
}

void MainComponent::goToEndButtonClicked()
{
    if (activePlayer == 1)
        player1.goToEnd();
    else
        player2.goToEnd();
}

void MainComponent::positionSliderMoved(double newSeconds)
{
    if (activePlayer == 1)
    {
        player1.setPosition(newSeconds);
        playerGUI1.setPlaybackState(player1.isPlaying());
    }
    else
    {
        player2.setPosition(newSeconds);
        playerGUI2.setPlaybackState(player2.isPlaying());
    }
}

void MainComponent::timerCallback()
{
    repaint();

    // Update both players
    double total1 = player1.getLengthInSeconds();
    double current1 = player1.getCurrentPosition();
    double total2 = player2.getLengthInSeconds();
    double current2 = player2.getCurrentPosition();

    if (!std::isfinite(total1) || total1 <= 0.0) total1 = 0.0;
    if (!std::isfinite(total2) || total2 <= 0.0) total2 = 0.0;

    playerGUI1.updatePositionDisplay(current1, total1);
    playerGUI2.updatePositionDisplay(current2, total2);

    playerGUI1.setPlaybackState(player1.isPlaying());
    playerGUI2.setPlaybackState(player2.isPlaying());

    playerGUI1.setMarkerAState(player1.getMarkerA() >= 0);
    playerGUI1.setMarkerBState(player1.getMarkerB() >= 0);
    playerGUI1.setSegmentLoopState(player1.isSegmentLooping());

    playerGUI2.setMarkerAState(player2.getMarkerA() >= 0);
    playerGUI2.setMarkerBState(player2.getMarkerB() >= 0);
    playerGUI2.setSegmentLoopState(player2.isSegmentLooping());
}

void MainComponent::SaveState()
{
    if (propertiesFile)
    {
        player1.SaveState(*propertiesFile, "player1");
        player2.SaveState(*propertiesFile, "player2");
    }
}

void MainComponent::RestoreState()
{
    if (propertiesFile)
    {
        player1.RestoreState(*propertiesFile, "player1");
        player2.RestoreState(*propertiesFile, "player2");

        juce::String lastFilePath = propertiesFile->getValue("player1_lastFile");
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
    if (activePlayer == 1)
    {
        player1.setMarkerA();
        playerGUI1.setMarkerAState(player1.getMarkerA() >= 0);
        updateSliceState();
    }
    else
    {
        player2.setMarkerA();
        playerGUI2.setMarkerAState(player2.getMarkerA() >= 0);
        updateSliceState();
    }
}

void MainComponent::markerBButtonClicked()
{
    if (activePlayer == 1)
    {
        player1.setMarkerB();
        playerGUI1.setMarkerBState(player1.getMarkerB() >= 0);
        updateSliceState();
    }
    else
    {
        player2.setMarkerB();
        playerGUI2.setMarkerBState(player2.getMarkerB() >= 0);
        updateSliceState();
    }
}

void MainComponent::clearMarkersButtonClicked()
{
    if (activePlayer == 1)
    {
        player1.clearMarkers();
        playerGUI1.setMarkerAState(false);
        playerGUI1.setMarkerBState(false);
        playerGUI1.setSegmentLoopState(false);
        updateSliceState();
    }
    else
    {
        player2.clearMarkers();
        playerGUI2.setMarkerAState(false);
        playerGUI2.setMarkerBState(false);
        playerGUI2.setSegmentLoopState(false);
        updateSliceState();
    }
}

void MainComponent::segmentLoopButtonClicked()
{
    if (activePlayer == 1)
    {
        bool newState = !player1.isSegmentLooping();
        player1.setSegmentLooping(newState);
        playerGUI1.setSegmentLoopState(newState);

        if (newState)
        {
            player1.toggleLooping();
            playerGUI1.setLoopState(false);
        }
    }
    else
    {
        bool newState = !player2.isSegmentLooping();
        player2.setSegmentLooping(newState);
        playerGUI2.setSegmentLoopState(newState);

        if (newState)
        {
            player2.toggleLooping();
            playerGUI2.setLoopState(false);
        }
    }
}

// Slicing Functionality
void MainComponent::sliceButtonClicked()
{
    bool success = false;
    if (activePlayer == 1)
    {
        success = player1.createSliceFromMarkers();
    }
    else
    {
        success = player2.createSliceFromMarkers();
    }

    if (success)
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
    bool hasValidSlice = false;
    if (activePlayer == 1)
    {
        hasValidSlice = player1.hasValidSlice();
    }
    else
    {
        hasValidSlice = player2.hasValidSlice();
    }

    if (!hasValidSlice)
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

                bool success = false;
                if (activePlayer == 1)
                {
                    success = player1.saveSliceToFile(file);
                }
                else
                {
                    success = player2.saveSliceToFile(file);
                }

                if (success)
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
    if (activePlayer == 1)
    {
        playerGUI1.setSliceState(player1.hasValidSlice());
    }
    else
    {
        playerGUI2.setSliceState(player2.hasValidSlice());
    }
}

// New: Update metadata display
void MainComponent::updateMetadataDisplay()
{
    if (activePlayer == 1)
    {
        auto metadata = player1.getMetadata();
        juce::String metadataText;

        if (metadata.artist.isNotEmpty() && metadata.title.isNotEmpty())
        {
            metadataText = metadata.artist + " - " + metadata.title;
            if (metadata.album.isNotEmpty())
                metadataText += " (" + metadata.album + ")";
        }
        else
        {
            metadataText = metadata.title;
        }

        if (metadata.duration > 0)
        {
            int minutes = static_cast<int>(metadata.duration) / 60;
            int seconds = static_cast<int>(metadata.duration) % 60;
            metadataText += juce::String::formatted(" [%d:%02d]", minutes, seconds);
        }

        playerGUI1.setMetadataDisplay(metadataText);
    }
    else
    {
        auto metadata = player2.getMetadata();
        juce::String metadataText;

        if (metadata.artist.isNotEmpty() && metadata.title.isNotEmpty())
        {
            metadataText = metadata.artist + " - " + metadata.title;
            if (metadata.album.isNotEmpty())
                metadataText += " (" + metadata.album + ")";
        }
        else
        {
            metadataText = metadata.title;
        }

        if (metadata.duration > 0)
        {
            int minutes = static_cast<int>(metadata.duration) / 60;
            int seconds = static_cast<int>(metadata.duration) % 60;
            metadataText += juce::String::formatted(" [%d:%02d]", minutes, seconds);
        }

        playerGUI2.setMetadataDisplay(metadataText);
    }
}

// Track Markers Functionality
void MainComponent::addMarkerButtonClicked()
{
    double currentTime = 0.0;
    if (activePlayer == 1)
    {
        currentTime = player1.getCurrentPosition();
        player1.addMarker(currentTime);
    }
    else
    {
        currentTime = player2.getCurrentPosition();
        player2.addMarker(currentTime);
    }
}

void MainComponent::deleteMarkerButtonClicked()
{
    if (activePlayer == 1)
    {
        int selectedRow = playerGUI1.getMarkersList().getSelectedRow();
        if (selectedRow >= 0)
        {
            player1.removeMarker(selectedRow);
        }
    }
    else
    {
        int selectedRow = playerGUI2.getMarkersList().getSelectedRow();
        if (selectedRow >= 0)
        {
            player2.removeMarker(selectedRow);
        }
    }
}

void MainComponent::jumpToMarker(int index)
{
    if (activePlayer == 1)
    {
        player1.jumpToMarker(index);
    }
    else
    {
        player2.jumpToMarker(index);
    }
}

// ListBoxModel implementation for markers
int MainComponent::getNumRows()
{
    if (activePlayer == 1)
    {
        return player1.getMarkers().size();
    }
    else
    {
        return player2.getMarkers().size();
    }
}

void MainComponent::paintListBoxItem(int rowNumber, juce::Graphics& g,
    int width, int height, bool rowIsSelected)
{
    juce::Array<PlayerAudio::Marker> markers;
    if (activePlayer == 1)
    {
        markers = player1.getMarkers();
    }
    else
    {
        markers = player2.getMarkers();
    }

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
    if (source == &player1)
    {
        // Update the markers list when markers change in player1
        playerGUI1.getMarkersList().updateContent();
        playerGUI1.getMarkersList().repaint();
    }
    else if (source == &player2)
    {
        // Update the markers list when markers change in player2
        playerGUI2.getMarkersList().updateContent();
        playerGUI2.getMarkersList().repaint();
    }
}