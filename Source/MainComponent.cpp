#include "MainComponent.h"
#include <juce_data_structures/juce_data_structures.h>
#include <random>

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

    startTimerHz(30);

    // Set up single player GUI
    addAndMakeVisible(playerGUI);
    playerGUI.setListener(this);
    playerGUI.getMarkersList().setModel(this);

    // Listen for marker changes from both players
    player1.addChangeListener(this);
    player2.addChangeListener(this);

    // Set up audio mixing for both players
    mixerAudioSource.addInputSource(&player1, false);
    mixerAudioSource.addInputSource(&player2, false);

    setAudioChannels(0, 2);
    setSize(1000, 800);
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

    // === Title Bar ===
    auto titleArea = getLocalBounds().removeFromTop(40).reduced(20, 5);
    g.setColour(juce::Colours::white.withAlpha(0.9f));
    g.setFont(20.0f);
    g.drawText("Dual Track Mixer - Two Waveforms, One Player", titleArea, juce::Justification::centred);

    // === Status Bar with Playlist Info ===
    auto statusArea = getLocalBounds().removeFromTop(25).reduced(20, 0);

    // Show playlist info if loaded
    if (playlistLoaded)
    {
        g.setColour(juce::Colours::limegreen.withAlpha(0.8f));
        g.setFont(11.0f);
        juce::String playlistInfo = "Playlist: " + juce::String(currentPlaylistIndex + 1) +
            "/" + juce::String(playlist.size());
        g.drawText(playlistInfo, statusArea.removeFromRight(100), juce::Justification::centredRight);
    }

    // Track status
    g.setColour(juce::Colours::white.withAlpha(0.6f));
    g.setFont(12.0f);
    juce::String statusText = "Track 1: ";
    if (player1.getLengthInSeconds() > 0)
        statusText += player1.getMetadata().title;
    else
        statusText += "No File";

    statusText += " | Track 2: ";
    if (player2.getLengthInSeconds() > 0)
        statusText += player2.getMetadata().title;
    else
        statusText += "No File";

    g.drawText(statusText, statusArea, juce::Justification::centredLeft);

    // === Dual Waveform Display ===
    auto waveformArea = getLocalBounds().removeFromTop(240).reduced(20, 10);

    // Draw background for waveform area
    g.setColour(juce::Colours::black.withAlpha(0.3f));
    g.fillRoundedRectangle(waveformArea.toFloat(), 8.0f);
    g.setColour(juce::Colours::white.withAlpha(0.2f));
    g.drawRoundedRectangle(waveformArea.toFloat(), 8.0f, 1.0f);

    // Split into two waveform areas
    auto waveform1Area = waveformArea.removeFromTop(waveformArea.getHeight() / 2).reduced(5, 5);
    auto waveform2Area = waveformArea.reduced(5, 5);

    // Draw Track 1 Waveform
    if (showWaveform1 && thumbnail1.getTotalLength() > 0.0)
    {
        // Background for track 1 waveform
        g.setColour(juce::Colours::black.withAlpha(0.5f));
        g.fillRoundedRectangle(waveform1Area.toFloat(), 4.0f);

        // Waveform gradient for track 1
        juce::ColourGradient waveform1Gradient(
            juce::Colours::cyan.withAlpha(0.8f), (float)waveform1Area.getX(), (float)waveform1Area.getCentreY(),
            juce::Colours::blue.withAlpha(0.8f), (float)waveform1Area.getRight(), (float)waveform1Area.getCentreY(),
            false);
        g.setGradientFill(waveform1Gradient);
        thumbnail1.drawChannel(g, waveform1Area, 0.0, thumbnail1.getTotalLength(), 0, 0.85f);

        // Draw markers for track 1
        auto& markers1 = player1.getMarkers();
        double totalLength1 = player1.getLengthInSeconds();
        for (const auto& marker : markers1)
        {
            float xPos = (float)(marker.time / totalLength1) * waveform1Area.getWidth() + waveform1Area.getX();
            g.setColour(juce::Colours::yellow);
            g.drawLine(xPos, (float)waveform1Area.getY(), xPos, (float)waveform1Area.getBottom(), 2.0f);

            g.setColour(juce::Colours::orange);
            g.fillEllipse(xPos - 3, waveform1Area.getBottom() - 8, 6, 6);
        }

        // Draw current playback cursor for track 1
        if (player1.isPlaying())
        {
            double position = player1.getCurrentPosition();
            double proportion = (totalLength1 > 0.0) ? (position / totalLength1) : 0.0;
            float x = (float)waveform1Area.getX() + (float)(proportion * waveform1Area.getWidth());
            g.setColour(juce::Colours::white);
            g.drawLine(x, (float)waveform1Area.getY(), x, (float)waveform1Area.getBottom(), 2.0f);
        }

        // Track 1 label
        g.setColour(juce::Colours::white.withAlpha(0.8f));
        g.setFont(12.0f);
        g.drawText("Track 1: " + player1.getMetadata().title, waveform1Area, juce::Justification::topLeft);
    }
    else
    {
        g.setColour(juce::Colours::white.withAlpha(0.4f));
        g.setFont(14.0f);
        g.drawText("Load Track 1", waveform1Area, juce::Justification::centred);
    }

    // Draw Track 2 Waveform
    if (showWaveform2 && thumbnail2.getTotalLength() > 0.0)
    {
        // Background for track 2 waveform
        g.setColour(juce::Colours::black.withAlpha(0.5f));
        g.fillRoundedRectangle(waveform2Area.toFloat(), 4.0f);

        // Waveform gradient for track 2
        juce::ColourGradient waveform2Gradient(
            juce::Colours::green.withAlpha(0.8f), (float)waveform2Area.getX(), (float)waveform2Area.getCentreY(),
            juce::Colours::lime.withAlpha(0.8f), (float)waveform2Area.getRight(), (float)waveform2Area.getCentreY(),
            false);
        g.setGradientFill(waveform2Gradient);
        thumbnail2.drawChannel(g, waveform2Area, 0.0, thumbnail2.getTotalLength(), 0, 0.85f);

        // Draw markers for track 2
        auto& markers2 = player2.getMarkers();
        double totalLength2 = player2.getLengthInSeconds();
        for (const auto& marker : markers2)
        {
            float xPos = (float)(marker.time / totalLength2) * waveform2Area.getWidth() + waveform2Area.getX();
            g.setColour(juce::Colours::yellow);
            g.drawLine(xPos, (float)waveform2Area.getY(), xPos, (float)waveform2Area.getBottom(), 2.0f);

            g.setColour(juce::Colours::orange);
            g.fillEllipse(xPos - 3, waveform2Area.getBottom() - 8, 6, 6);
        }

        // Draw current playback cursor for track 2
        if (player2.isPlaying())
        {
            double position = player2.getCurrentPosition();
            double proportion = (totalLength2 > 0.0) ? (position / totalLength2) : 0.0;
            float x = (float)waveform2Area.getX() + (float)(proportion * waveform2Area.getWidth());
            g.setColour(juce::Colours::white);
            g.drawLine(x, (float)waveform2Area.getY(), x, (float)waveform2Area.getBottom(), 2.0f);
        }

        // Track 2 label
        g.setColour(juce::Colours::white.withAlpha(0.8f));
        g.setFont(12.0f);
        g.drawText("Track 2: " + player2.getMetadata().title, waveform2Area, juce::Justification::topLeft);
    }
    else
    {
        g.setColour(juce::Colours::white.withAlpha(0.4f));
        g.setFont(14.0f);
        g.drawText("Load Track 2", waveform2Area, juce::Justification::centred);
    }

    // === Mixing Information ===
    auto mixInfoArea = getLocalBounds().removeFromTop(30).reduced(20, 5);
    g.setColour(juce::Colours::white.withAlpha(0.7f));
    g.setFont(12.0f);

    juce::String mixInfo = "Mixer: ";
    if (player1.isPlaying() && player2.isPlaying())
        mixInfo += "Both Tracks Active | ";
    else if (player1.isPlaying())
        mixInfo += "Track 1 Only | ";
    else if (player2.isPlaying())
        mixInfo += "Track 2 Only | ";
    else
        mixInfo += "Stopped | ";

    mixInfo += "Volumes: T1=" + juce::String(player1.getVolume(), 2) +
        " T2=" + juce::String(player2.getVolume(), 2);

    g.drawText(mixInfo, mixInfoArea, juce::Justification::centred);

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

    // === Instructions ===
    auto instructionsArea = getLocalBounds().removeFromBottom(20).reduced(20, 0);
    g.setColour(juce::Colours::white.withAlpha(0.5f));
    g.setFont(11.0f);
    g.drawText("Use playlist for Track 1 | Load individual files for Track 2 | Both tracks mix together",
        instructionsArea, juce::Justification::centred);
}

void MainComponent::resized()
{
    auto area = getLocalBounds();

    // Reserve space for all UI elements in order from top to bottom
    area.removeFromTop(40);    // Title bar
    area.removeFromTop(25);    // Status/playlist info area
    area.removeFromTop(240);   // Dual waveform display
    area.removeFromTop(30);    // Mixing information
    area.removeFromBottom(20); // Instructions at bottom

    // The remaining area is for the single player GUI
    playerGUI.setBounds(area.reduced(10, 5));
}

// GUI Button Actions
void MainComponent::loadButtonClicked()
{
    fileChooser = std::make_unique<juce::FileChooser>(
        "Select an audio file for Track " + juce::String(currentLoadingTrack) + "...",
        juce::File{},
        "*.wav;*.mp3;*.aiff;*.flac;*.aif");

    auto chooserFlags = juce::FileBrowserComponent::openMode |
        juce::FileBrowserComponent::canSelectFiles;

    fileChooser->launchAsync(chooserFlags, [this](const juce::FileChooser& chooser)
        {
            auto file = chooser.getResult();
            if (file.existsAsFile())
            {
                if (currentLoadingTrack == 1)
                {
                    player1.loadFile(file);
                    playerGUI.setPlaybackState(false);
                    playerGUI.setLoopState(player1.isLoopingEnabled());
                    playerGUI.setMuteState(false);

                    // Load waveform for track 1
                    thumbnail1.setSource(new juce::FileInputSource(file));
                    showWaveform1 = true;

                    // Switch to loading track 2 next time
                    currentLoadingTrack = 2;
                }
                else
                {
                    player2.loadFile(file);
                    // Don't reset playback state for track 2
                    thumbnail2.setSource(new juce::FileInputSource(file));
                    showWaveform2 = true;

                    // Switch back to loading track 1 next time
                    currentLoadingTrack = 1;
                }

                // Update metadata display
                updateMetadataDisplay();
                repaint();
            }
        });
}

void MainComponent::loadSecondTrackButtonClicked()
{
    currentLoadingTrack = 2;
    loadButtonClicked();
}

void MainComponent::playButtonClicked()
{
    // Start both players if they have files loaded
    if (player1.getLengthInSeconds() > 0)
    {
        if (player1.isPlaying())
            player1.stop();
        else
            player1.play();
    }

    if (player2.getLengthInSeconds() > 0)
    {
        if (player2.isPlaying())
            player2.stop();
        else
            player2.play();
    }

    // Update GUI state based on whether any player is playing
    bool anyPlaying = player1.isPlaying() || player2.isPlaying();
    playerGUI.setPlaybackState(anyPlaying);
}

void MainComponent::stopButtonClicked()
{
    player1.stop();
    player2.stop();
    playerGUI.setPlaybackState(false);
}

void MainComponent::restartButtonClicked()
{
    player1.restart();
    player2.restart();
    playerGUI.setPlaybackState(true);
}

void MainComponent::loopButtonClicked()
{
    // Toggle looping for both players
    bool loop1 = player1.toggleLooping();
    bool loop2 = player2.toggleLooping();

    // Update GUI with track 1's loop state
    playerGUI.setLoopState(loop1);
}

void MainComponent::muteButtonClicked()
{
    toggleMute();
}

void MainComponent::volumeChanged(float newVolume)
{
    if (!isMuted)
    {
        // Set volume for both players
        player1.setVolume(newVolume);
        player2.setVolume(newVolume);
        previousVolume = newVolume;
    }
}

void MainComponent::toggleMute()
{
    isMuted = !isMuted;

    if (isMuted)
    {
        previousVolume = player1.getVolume();
        player1.setVolume(0.0f);
        player2.setVolume(0.0f);
    }
    else
    {
        player1.setVolume(previousVolume);
        player2.setVolume(previousVolume);
    }
    playerGUI.setMuteState(isMuted);
}

void MainComponent::forwardButtonClicked()
{
    player1.forward(10.0);
    player2.forward(10.0);
}

void MainComponent::backwardButtonClicked()
{
    player1.backward(10.0);
    player2.backward(10.0);
}

void MainComponent::goToEndButtonClicked()
{
    player1.goToEnd();
    player2.goToEnd();
}

void MainComponent::positionSliderMoved(double newSeconds)
{
    // Set position for both players
    player1.setPosition(newSeconds);
    player2.setPosition(newSeconds);
}

void MainComponent::timerCallback()
{
    repaint();

    // Update position display based on the longer track
    double total1 = player1.getLengthInSeconds();
    double current1 = player1.getCurrentPosition();
    double total2 = player2.getLengthInSeconds();
    double current2 = player2.getCurrentPosition();

    // Use the longer track for position display
    double displayTotal = juce::jmax(total1, total2);
    double displayCurrent = juce::jmax(current1, current2);

    if (!std::isfinite(displayTotal) || displayTotal <= 0.0) displayTotal = 0.0;

    playerGUI.updatePositionDisplay(displayCurrent, displayTotal);

    // Update playback state
    bool anyPlaying = player1.isPlaying() || player2.isPlaying();
    playerGUI.setPlaybackState(anyPlaying);

    // Update marker states (use track 1 for markers)
    playerGUI.setMarkerAState(player1.getMarkerA() >= 0);
    playerGUI.setMarkerBState(player1.getMarkerB() >= 0);
    playerGUI.setSegmentLoopState(player1.isSegmentLooping());
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
            thumbnail1.setSource(new juce::FileInputSource(lastFile));
            showWaveform1 = true;
        }
        else
        {
            showWaveform1 = false;
        }
    }
}

void MainComponent::markerAButtonClicked()
{
    player1.setMarkerA();
    playerGUI.setMarkerAState(player1.getMarkerA() >= 0);
    updateSliceState();
}

void MainComponent::markerBButtonClicked()
{
    player1.setMarkerB();
    playerGUI.setMarkerBState(player1.getMarkerB() >= 0);
    updateSliceState();
}

void MainComponent::clearMarkersButtonClicked()
{
    player1.clearMarkers();
    playerGUI.setMarkerAState(false);
    playerGUI.setMarkerBState(false);
    playerGUI.setSegmentLoopState(false);
    updateSliceState();
}

void MainComponent::segmentLoopButtonClicked()
{
    bool newState = !player1.isSegmentLooping();
    player1.setSegmentLooping(newState);
    playerGUI.setSegmentLoopState(newState);

    if (newState)
    {
        player1.toggleLooping();
        playerGUI.setLoopState(false);
    }
}

// Slicing Functionality
void MainComponent::sliceButtonClicked()
{
    bool success = player1.createSliceFromMarkers();

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
    bool hasValidSlice = player1.hasValidSlice();

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

                bool success = player1.saveSliceToFile(file);

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
    playerGUI.setSliceState(player1.hasValidSlice());
}

// New: Update metadata display
void MainComponent::updateMetadataDisplay()
{
    auto metadata1 = player1.getMetadata();
    auto metadata2 = player2.getMetadata();

    juce::String metadataText;

    if (metadata1.title.isNotEmpty())
    {
        metadataText = "T1: " + metadata1.title;
        if (metadata2.title.isNotEmpty())
        {
            metadataText += " | T2: " + metadata2.title;
        }
    }
    else if (metadata2.title.isNotEmpty())
    {
        metadataText = "T2: " + metadata2.title;
    }
    else
    {
        metadataText = "No tracks loaded";
    }

    playerGUI.setMetadataDisplay(metadataText);
}

// Track Markers Functionality
void MainComponent::addMarkerButtonClicked()
{
    double currentTime = player1.getCurrentPosition();
    player1.addMarker(currentTime);
}

void MainComponent::deleteMarkerButtonClicked()
{
    int selectedRow = playerGUI.getMarkersList().getSelectedRow();
    if (selectedRow >= 0)
    {
        player1.removeMarker(selectedRow);
        playerGUI.getMarkersList().updateContent();
        playerGUI.getMarkersList().repaint();
    }
}

void MainComponent::jumpToMarker(int index)
{
    player1.jumpToMarker(index);
}

// ListBoxModel implementation for markers
int MainComponent::getNumRows()
{
    return player1.getMarkers().size();
}

void MainComponent::paintListBoxItem(int rowNumber, juce::Graphics& g,
    int width, int height, bool rowIsSelected)
{
    juce::Array<PlayerAudio::Marker> markers = player1.getMarkers();

    if (rowNumber < markers.size())
    {
        auto textColour = juce::Colour(0xFFB0AFFF);
        auto highlightColour = juce::Colour(0xFF9370DB);

        if (rowIsSelected)
        {
            g.fillAll(highlightColour);
            g.setColour(juce::Colours::white);
        }
        else
        {
            g.setColour(textColour);
        }

        g.setFont(14.0f);

        // Format time as MM:SS
        int minutes = static_cast<int>(markers[rowNumber].time) / 60;
        int seconds = static_cast<int>(markers[rowNumber].time) % 60;
        juce::String timeString = juce::String::formatted("%02d:%02d", minutes, seconds);

        g.drawText(markers[rowNumber].name + " - " + timeString,
            8, 0, width - 8, height,
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
        // Update markers list
        playerGUI.getMarkersList().updateContent();
        playerGUI.getMarkersList().repaint();

        // Update metadata
        updateMetadataDisplay();
    }
    else if (source == &player2)
    {
        // Update metadata for track 2
        updateMetadataDisplay();
    }
}

// Playlist functionality
void MainComponent::loadPlaylistButtonClicked()
{
    fileChooser = std::make_unique<juce::FileChooser>(
        "Select playlist file...",
        juce::File{},
        "*.m3u;*.m3u8;*.txt");

    auto chooserFlags = juce::FileBrowserComponent::openMode |
        juce::FileBrowserComponent::canSelectFiles;

    fileChooser->launchAsync(chooserFlags, [this](const juce::FileChooser& chooser)
        {
            auto file = chooser.getResult();
            if (file.existsAsFile())
            {
                loadPlaylist(file);
            }
        });
}

void MainComponent::prevTrackButtonClicked()
{
    playPreviousInPlaylist();
}

void MainComponent::nextTrackButtonClicked()
{
    playNextInPlaylist();
}

void MainComponent::playlistBoxChanged(int selectedId)
{
    if (selectedId > 0) // 0 means no selection
    {
        playTrack(selectedId - 1); // Convert to 0-based index
    }
}

void MainComponent::loadPlaylist(const juce::File& playlistFile)
{
    playlist.clear();

    juce::FileInputStream inputStream(playlistFile);
    if (!inputStream.openedOk())
        return;

    juce::StringArray lines;
    while (!inputStream.isExhausted())
    {
        auto line = inputStream.readNextLine();
        if (line.isNotEmpty() && !line.startsWithChar('#'))
        {
            juce::File trackFile;
            if (line.startsWithChar('/') || line.containsChar(':'))
            {
                // Absolute path
                trackFile = juce::File(line);
            }
            else
            {
                // Relative to playlist file
                trackFile = playlistFile.getSiblingFile(line);
            }

            if (trackFile.existsAsFile() &&
                (trackFile.hasFileExtension("wav") || trackFile.hasFileExtension("mp3") ||
                    trackFile.hasFileExtension("aiff") || trackFile.hasFileExtension("flac") ||
                    trackFile.hasFileExtension("aif")))
            {
                playlist.add(trackFile);
            }
        }
    }

    if (!playlist.isEmpty())
    {
        playlistLoaded = true;
        currentPlaylistIndex = 0;
        updatePlaylistDisplay();
        playTrack(0);
    }
}

void MainComponent::playNextInPlaylist()
{
    if (playlistLoaded && !playlist.isEmpty())
    {
        currentPlaylistIndex = (currentPlaylistIndex + 1) % playlist.size();
        playTrack(currentPlaylistIndex);
    }
}

void MainComponent::playPreviousInPlaylist()
{
    if (playlistLoaded && !playlist.isEmpty())
    {
        currentPlaylistIndex = (currentPlaylistIndex - 1 + playlist.size()) % playlist.size();
        playTrack(currentPlaylistIndex);
    }
}

void MainComponent::playTrack(int index)
{
    if (index >= 0 && index < playlist.size())
    {
        currentPlaylistIndex = index;

        // Load the track into track 1 (main track)
        player1.loadFile(playlist[index]);
        playerGUI.setPlaybackState(false);

        // Load waveform for track 1
        thumbnail1.setSource(new juce::FileInputSource(playlist[index]));
        showWaveform1 = true;

        updatePlaylistDisplay();
        repaint();
    }
}

void MainComponent::updatePlaylistDisplay()
{
    // Update the playlist box
    playerGUI.getPlaylistBox().clear();

    for (int i = 0; i < playlist.size(); ++i)
    {
        auto trackName = playlist[i].getFileNameWithoutExtension();
        playerGUI.getPlaylistBox().addItem(trackName, i + 1); // ID is index + 1
    }

    if (currentPlaylistIndex >= 0 && currentPlaylistIndex < playlist.size())
    {
        playerGUI.getPlaylistBox().setSelectedId(currentPlaylistIndex + 1);
    }
}