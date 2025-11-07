#include "PlayerGUI.h"

PlayerGUI::PlayerGUI()
{
    loadButtonIcons();
    auto baseColour = juce::Colour(0xFF2B2B5E);
    auto accentColour = juce::Colour(0xFF6A5ACD);
    auto activeColour = juce::Colour(0xFF9370DB);
    auto borderColour = juce::Colour(0xFF483D8B);
    auto textColour = juce::Colour(0xFFB0AFFF);
    auto sliderThumb = juce::Colour(0xFFAD8CFF);

    metadataLabel.setJustificationType(juce::Justification::centred);
    metadataLabel.setColour(juce::Label::textColourId, textColour);
    metadataLabel.setText("No file loaded", juce::dontSendNotification);
    addAndMakeVisible(metadataLabel);

    for (auto* button : { &loadButton, &loadSecondTrackButton, &stopButton })
    {
        addAndMakeVisible(button);
        button->addListener(this);
        button->setColour(juce::TextButton::buttonColourId, accentColour);
        button->setColour(juce::TextButton::buttonOnColourId, activeColour);
        button->setColour(juce::TextButton::textColourOffId, textColour);
        button->setColour(juce::TextButton::textColourOnId, juce::Colours::white);
    }

    for (auto* button : { &muteButton, &restartButton, &backwardButton, &playButton,
                         &forwardButton, &goToEndButton, &loopButton })
    {
        addAndMakeVisible(button);
        button->addListener(this);
        button->setColour(juce::DrawableButton::backgroundColourId, accentColour);
        button->setColour(juce::DrawableButton::backgroundOnColourId, activeColour);
        button->setAlpha(0.95f);
    }

    for (auto* button : { &markerAButton, &markerBButton, &clearMarkersButton, &segmentLoopButton })
    {
        addAndMakeVisible(button);
        button->addListener(this);
        button->setColour(juce::TextButton::buttonColourId, accentColour);
        button->setColour(juce::TextButton::buttonOnColourId, activeColour);
        button->setColour(juce::TextButton::textColourOffId, textColour);
        button->setColour(juce::TextButton::textColourOnId, juce::Colours::white);
    }

    for (auto* button : { &sliceButton, &saveSliceButton })
    {
        addAndMakeVisible(button);
        button->addListener(this);
        button->setColour(juce::TextButton::buttonColourId, accentColour);
        button->setColour(juce::TextButton::buttonOnColourId, activeColour);
        button->setColour(juce::TextButton::textColourOffId, textColour);
        button->setColour(juce::TextButton::textColourOnId, juce::Colours::white);
    }

    sliceButton.setButtonText("Create Slice");
    saveSliceButton.setButtonText("Save Slice");
    saveSliceButton.setEnabled(false);

    sliceInfoLabel.setJustificationType(juce::Justification::centred);
    sliceInfoLabel.setText("Set A-B markers and click Create Slice", juce::dontSendNotification);
    sliceInfoLabel.setColour(juce::Label::textColourId, textColour);
    addAndMakeVisible(sliceInfoLabel);

    addAndMakeVisible(addMarkerButton);
    addMarkerButton.addListener(this);
    addMarkerButton.setColour(juce::TextButton::buttonColourId, accentColour);
    addMarkerButton.setColour(juce::TextButton::buttonOnColourId, activeColour);
    addMarkerButton.setColour(juce::TextButton::textColourOffId, textColour);
    addMarkerButton.setColour(juce::TextButton::textColourOnId, juce::Colours::white);
    addMarkerButton.setButtonText("Add Marker");

    addAndMakeVisible(deleteMarkerButton);
    deleteMarkerButton.addListener(this);
    deleteMarkerButton.setColour(juce::TextButton::buttonColourId, accentColour);
    deleteMarkerButton.setColour(juce::TextButton::buttonOnColourId, activeColour);
    deleteMarkerButton.setColour(juce::TextButton::textColourOffId, textColour);
    deleteMarkerButton.setColour(juce::TextButton::textColourOnId, juce::Colours::white);
    deleteMarkerButton.setButtonText("Delete Marker");

    addAndMakeVisible(markersList);
    markersList.setRowHeight(25);
    markersList.setColour(juce::ListBox::backgroundColourId, baseColour.withAlpha(0.7f));
    markersList.setColour(juce::ListBox::outlineColourId, borderColour);

    volumeSlider.setRange(0.0, 1.0, 0.01);
    volumeSlider.setValue(0.5);
    volumeSlider.addListener(this);
    volumeSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    volumeSlider.setColour(juce::Slider::thumbColourId, sliderThumb);
    volumeSlider.setColour(juce::Slider::trackColourId, accentColour);
    volumeSlider.setColour(juce::Slider::backgroundColourId, juce::Colour(0xFFF0E6FF));
    volumeSlider.setColour(juce::Slider::textBoxTextColourId, textColour);
    volumeSlider.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(0xFFF8F6FF));
    volumeSlider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colour(0xFFD8BFD8));
    addAndMakeVisible(volumeSlider);

    volumeLabel.setJustificationType(juce::Justification::centredLeft);
    volumeLabel.setText("Volume", juce::dontSendNotification);
    volumeLabel.setColour(juce::Label::textColourId, textColour);
    addAndMakeVisible(volumeLabel);

    speedSlider.setRange(0.5, 2.0, 0.01);
    speedSlider.setValue(1.0);
    speedSlider.addListener(this);
    speedSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    speedSlider.setColour(juce::Slider::thumbColourId, sliderThumb);
    speedSlider.setColour(juce::Slider::trackColourId, accentColour);
    speedSlider.setColour(juce::Slider::backgroundColourId, juce::Colour(0xFFF0E6FF));
    speedSlider.setColour(juce::Slider::textBoxTextColourId, textColour);
    speedSlider.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(0xFFF8F6FF));
    speedSlider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colour(0xFFD8BFD8));
    addAndMakeVisible(speedSlider);

    speedLabel.setJustificationType(juce::Justification::centredLeft);
    speedLabel.setText("Speed", juce::dontSendNotification);
    speedLabel.setColour(juce::Label::textColourId, textColour);
    addAndMakeVisible(speedLabel);

    positionSlider.setRange(0.0, 1.0, 0.0001);
    positionSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    positionSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    positionSlider.addListener(this);
    positionSlider.setColour(juce::Slider::thumbColourId, sliderThumb);
    positionSlider.setColour(juce::Slider::trackColourId, accentColour);
    positionSlider.setColour(juce::Slider::backgroundColourId, juce::Colour(0xFFF0E6FF));
    addAndMakeVisible(positionSlider);

    currentTimeLabel.setText("0:00", juce::dontSendNotification);
    totalTimeLabel.setText("0:00", juce::dontSendNotification);
    currentTimeLabel.setJustificationType(juce::Justification::centred);
    totalTimeLabel.setJustificationType(juce::Justification::centred);
    currentTimeLabel.setColour(juce::Label::textColourId, textColour);
    totalTimeLabel.setColour(juce::Label::textColourId, textColour);
    addAndMakeVisible(currentTimeLabel);
    addAndMakeVisible(totalTimeLabel);

    setLoopState(false);
    setPlaybackState(false);
    setMuteState(false);

    addAndMakeVisible(loadPlaylistButton);
    loadPlaylistButton.addListener(this);
    loadPlaylistButton.setColour(juce::TextButton::buttonColourId, accentColour);
    loadPlaylistButton.setColour(juce::TextButton::buttonOnColourId, activeColour);
    loadPlaylistButton.setColour(juce::TextButton::textColourOffId, textColour);

    addAndMakeVisible(prevTrackButton);
    prevTrackButton.addListener(this);
    prevTrackButton.setColour(juce::TextButton::buttonColourId, accentColour);
    prevTrackButton.setColour(juce::TextButton::buttonOnColourId, activeColour);
    prevTrackButton.setColour(juce::TextButton::textColourOffId, textColour);

    addAndMakeVisible(nextTrackButton);
    nextTrackButton.addListener(this);
    nextTrackButton.setColour(juce::TextButton::buttonColourId, accentColour);
    nextTrackButton.setColour(juce::TextButton::buttonOnColourId, activeColour);
    nextTrackButton.setColour(juce::TextButton::textColourOffId, textColour);

    addAndMakeVisible(playlistBox);
    playlistBox.addListener(this);
    playlistBox.setColour(juce::ComboBox::backgroundColourId, baseColour);
    playlistBox.setColour(juce::ComboBox::textColourId, textColour);
    playlistBox.setColour(juce::ComboBox::arrowColourId, textColour);

    playlistLabel.setText("Playlist:", juce::dontSendNotification);
    playlistLabel.setColour(juce::Label::textColourId, textColour);
    addAndMakeVisible(playlistLabel);
}

PlayerGUI::~PlayerGUI() = default;

void PlayerGUI::resized()
{
    auto area = getLocalBounds();
    int margin = 8;

    auto metadataRow = area.removeFromTop(25);
    metadataLabel.setBounds(metadataRow.reduced(5, 2));

    auto playlistRow = area.removeFromTop(30);
    playlistRow.reduce(margin, 0);

    int playlistButtonWidth = 90;
    int playlistComboWidth = 150;

    loadPlaylistButton.setBounds(playlistRow.removeFromLeft(playlistButtonWidth).reduced(2));
    prevTrackButton.setBounds(playlistRow.removeFromLeft(40).reduced(2));
    nextTrackButton.setBounds(playlistRow.removeFromLeft(40).reduced(2));

    playlistLabel.setBounds(playlistRow.removeFromLeft(60).reduced(2));
    playlistBox.setBounds(playlistRow.reduced(2));

    auto topRow = area.removeFromTop(70);

    int buttonWidth = 65;
    int buttonHeight = 65;
    int spacing = 8;

    int totalButtonWidth = (buttonWidth + spacing) * 9 - spacing;
    int startX = (topRow.getWidth() - totalButtonWidth) / 2;

    int x = startX;
    int y = topRow.getY() + 2;

    loadButton.setBounds(x, y, buttonWidth, buttonHeight);       x += buttonWidth + spacing;
    loadSecondTrackButton.setBounds(x, y, buttonWidth, buttonHeight); x += buttonWidth + spacing;
    muteButton.setBounds(x, y, buttonWidth, buttonHeight);       x += buttonWidth + spacing;
    restartButton.setBounds(x, y, buttonWidth, buttonHeight);    x += buttonWidth + spacing;
    backwardButton.setBounds(x, y, buttonWidth, buttonHeight);   x += buttonWidth + spacing;
    playButton.setBounds(x, y, buttonWidth, buttonHeight);       x += buttonWidth + spacing;
    forwardButton.setBounds(x, y, buttonWidth, buttonHeight);    x += buttonWidth + spacing;
    goToEndButton.setBounds(x, y, buttonWidth, buttonHeight);    x += buttonWidth + spacing;
    loopButton.setBounds(x, y, buttonWidth, buttonHeight);       x += buttonWidth + spacing;
    stopButton.setBounds(x, y, buttonWidth, buttonHeight);

    auto abRow = area.removeFromTop(35);
    abRow.reduce(margin, 0);

    int abButtonWidth = 65;
    int abButtonHeight = 28;
    int abSpacing = 10;

    int totalABWidth = (abButtonWidth + abSpacing) * 4 - abSpacing;
    int abStartX = (abRow.getWidth() - totalABWidth) / 2;

    int abX = abStartX;
    int abY = abRow.getY() + 3;

    markerAButton.setBounds(abX, abY, abButtonWidth, abButtonHeight);        abX += abButtonWidth + abSpacing;
    markerBButton.setBounds(abX, abY, abButtonWidth, abButtonHeight);        abX += abButtonWidth + abSpacing;
    clearMarkersButton.setBounds(abX, abY, abButtonWidth, abButtonHeight);   abX += abButtonWidth + abSpacing;
    segmentLoopButton.setBounds(abX, abY, abButtonWidth, abButtonHeight);

    auto sliceRow = area.removeFromTop(35);
    sliceRow.reduce(margin, 0);

    int sliceButtonWidth = 90;
    int sliceButtonHeight = 28;
    int sliceSpacing = 12;

    int totalSliceWidth = (sliceButtonWidth + sliceSpacing) * 2 - sliceSpacing;
    int sliceStartX = (sliceRow.getWidth() - totalSliceWidth) / 2;

    int sliceX = sliceStartX;
    int sliceY = sliceRow.getY() + 3;

    sliceButton.setBounds(sliceX, sliceY, sliceButtonWidth, sliceButtonHeight);
    sliceX += sliceButtonWidth + sliceSpacing;
    saveSliceButton.setBounds(sliceX, sliceY, sliceButtonWidth, sliceButtonHeight);

    auto infoRow = area.removeFromTop(22);
    sliceInfoLabel.setBounds(infoRow.reduced(5, 2));

    auto markersSection = area.removeFromTop(100);
    markersSection.reduce(margin, 4);

    auto markerButtonsRow = markersSection.removeFromTop(28);
    int markerButtonWidth = 100;
    addMarkerButton.setBounds(markerButtonsRow.removeFromLeft(markerButtonWidth).reduced(2));
    deleteMarkerButton.setBounds(markerButtonsRow.removeFromLeft(markerButtonWidth).reduced(2));

    markersList.setBounds(markersSection.reduced(2));

    auto positionArea = area.removeFromTop(45);
    positionArea.reduce(margin, 0);

    int timeLabelWidth = 70;
    auto timeLabelArea = positionArea.removeFromTop(20);
    currentTimeLabel.setBounds(timeLabelArea.removeFromLeft(timeLabelWidth));
    totalTimeLabel.setBounds(timeLabelArea.removeFromRight(timeLabelWidth));

    positionSlider.setBounds(positionArea.reduced(0, 2));

    auto volumeArea = area.removeFromTop(35);
    volumeArea.reduce(margin, 0);
    volumeLabel.setBounds(volumeArea.removeFromLeft(70));
    volumeSlider.setBounds(volumeArea);

    auto speedArea = area.removeFromTop(35);
    speedArea.reduce(margin, 0);
    speedLabel.setBounds(speedArea.removeFromLeft(70));
    speedSlider.setBounds(speedArea);
}

void PlayerGUI::buttonClicked(juce::Button* button)
{
    if (!listener) return;

    if (button == &loadButton)     listener->loadButtonClicked();
    else if (button == &loadSecondTrackButton) listener->loadSecondTrackButtonClicked();
    else if (button == &playButton)     listener->playButtonClicked();
    else if (button == &stopButton)     listener->stopButtonClicked();
    else if (button == &restartButton)  listener->restartButtonClicked();
    else if (button == &loopButton)     listener->loopButtonClicked();
    else if (button == &muteButton)     listener->muteButtonClicked();
    else if (button == &forwardButton)  listener->forwardButtonClicked();
    else if (button == &backwardButton) listener->backwardButtonClicked();
    else if (button == &goToEndButton)  listener->goToEndButtonClicked();
    else if (button == &markerAButton)  listener->markerAButtonClicked();
    else if (button == &markerBButton)  listener->markerBButtonClicked();
    else if (button == &clearMarkersButton) listener->clearMarkersButtonClicked();
    else if (button == &segmentLoopButton) listener->segmentLoopButtonClicked();
    else if (button == &sliceButton)     listener->sliceButtonClicked();
    else if (button == &saveSliceButton) listener->saveSliceButtonClicked();
    else if (button == &addMarkerButton) listener->addMarkerButtonClicked();
    else if (button == &deleteMarkerButton) listener->deleteMarkerButtonClicked();
    else if (button == &loadPlaylistButton) listener->loadPlaylistButtonClicked();
    else if (button == &prevTrackButton) listener->prevTrackButtonClicked();
    else if (button == &nextTrackButton) listener->nextTrackButtonClicked();
}

void PlayerGUI::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &volumeSlider && listener)
        listener->volumeChanged((float)slider->getValue());
    else if (slider == &speedSlider && listener)
        listener->speedChanged((float)slider->getValue());
    else if (slider == &positionSlider && listener)
    {
        double cur = positionSlider.getValue() * lastTotalSeconds;
        currentTimeLabel.setText(juce::String::formatted("%d:%02d", (int)(cur / 60.0), (int)((int)cur % 60)), juce::dontSendNotification);

        double newSeconds = positionSlider.getValue() * lastTotalSeconds;
        listener->positionSliderMoved(newSeconds);
    }
}

void PlayerGUI::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    markersList.updateContent();
    markersList.repaint();
}

void PlayerGUI::setListener(Listener* newListener)
{
    listener = newListener;
}

void PlayerGUI::loadButtonIcons()
{
    auto playImg = juce::Drawable::createFromImageData(BinaryData::play_jpg, BinaryData::play_jpgSize);
    auto pauseImg = juce::Drawable::createFromImageData(BinaryData::pause_jpg, BinaryData::pause_jpgSize);
    playButton.setImages(playImg.get(), nullptr, nullptr, nullptr, pauseImg.get());

    auto muteImg = juce::Drawable::createFromImageData(BinaryData::mute_jpg, BinaryData::mute_jpgSize);
    auto unmuteImg = juce::Drawable::createFromImageData(BinaryData::unmute_jpg, BinaryData::unmute_jpgSize);
    muteButton.setImages(muteImg.get(), nullptr, nullptr, nullptr, unmuteImg.get());

    auto loopImg = juce::Drawable::createFromImageData(BinaryData::loop_jpg, BinaryData::loop_jpgSize);
    loopButton.setImages(loopImg.get());

    auto restartImg = juce::Drawable::createFromImageData(BinaryData::restart_jpg, BinaryData::restart_jpgSize);
    restartButton.setImages(restartImg.get());

    auto back10Img = juce::Drawable::createFromImageData(BinaryData::back10_jpg, BinaryData::back10_jpgSize);
    backwardButton.setImages(back10Img.get());

    auto forward10Img = juce::Drawable::createFromImageData(BinaryData::forward10_jpg, BinaryData::forward10_jpgSize);
    forwardButton.setImages(forward10Img.get());

    auto endImg = juce::Drawable::createFromImageData(BinaryData::gotoend_jpg, BinaryData::gotoend_jpgSize);
    goToEndButton.setImages(endImg.get());
}

void PlayerGUI::setLoopState(bool isLoopingNow)
{
    isLooping = isLoopingNow;
    loopButton.setToggleState(isLooping, juce::dontSendNotification);

    auto accentColour = juce::Colour(0xFF6A5ACD);
    auto activeColour = juce::Colour(0xFF9370DB);

    loopButton.setColour(juce::DrawableButton::backgroundColourId, isLooping ? activeColour : accentColour);
    loopButton.setColour(juce::DrawableButton::backgroundOnColourId, isLooping ? activeColour : accentColour);
}

void PlayerGUI::setPlaybackState(bool playing)
{
    isPlaying = playing;
    playButton.setToggleState(playing, juce::dontSendNotification);
}

void PlayerGUI::setVolumeLevel(float volume)
{
    volumeSlider.setValue(volume, juce::dontSendNotification);
}

void PlayerGUI::setMuteState(bool isMutedNow)
{
    isMuted = isMutedNow;
    muteButton.setToggleState(isMuted, juce::dontSendNotification);
}

void PlayerGUI::updatePositionDisplay(double currentSeconds, double totalSeconds)
{
    lastTotalSeconds = totalSeconds;

    if (totalSeconds > 0.0)
        positionSlider.setValue(currentSeconds / totalSeconds, juce::dontSendNotification);
    else
        positionSlider.setValue(0.0, juce::dontSendNotification);

    auto formatTime = [](double s) -> juce::String {
        if (s <= 0.0 || !std::isfinite(s)) return "0:00";
        int total = (int)std::round(s);
        int mins = total / 60;
        int secs = total % 60;
        return juce::String::formatted("%d:%02d", mins, secs);
        };

    currentTimeLabel.setText(formatTime(currentSeconds), juce::dontSendNotification);
    totalTimeLabel.setText(formatTime(totalSeconds), juce::dontSendNotification);
}

void PlayerGUI::setMarkerAState(bool isSet)
{
    markerAButton.setToggleState(isSet, juce::dontSendNotification);
}

void PlayerGUI::setMarkerBState(bool isSet)
{
    markerBButton.setToggleState(isSet, juce::dontSendNotification);
}

void PlayerGUI::setSegmentLoopState(bool isActive)
{
    segmentLoopButton.setToggleState(isActive, juce::dontSendNotification);
}

void PlayerGUI::setSliceState(bool hasSlice)
{
    saveSliceButton.setEnabled(hasSlice);
    if (hasSlice)
    {
        sliceInfoLabel.setText("Slice Ready! Click Save Slice to export", juce::dontSendNotification);
    }
    else
    {
        sliceInfoLabel.setText("Set A-B markers and click Create Slice", juce::dontSendNotification);
    }
}

void PlayerGUI::setMetadataDisplay(const juce::String& metadataText)
{
    metadataLabel.setText(metadataText, juce::dontSendNotification);
}

void PlayerGUI::comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged)
{
    if (comboBoxThatHasChanged == &playlistBox && listener)
    {
        listener->playlistBoxChanged(playlistBox.getSelectedId());
    }
}