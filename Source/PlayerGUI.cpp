#include "PlayerGUI.h"

PlayerGUI::PlayerGUI()
{
    loadButtonIcons();
    auto baseColour = juce::Colour(0xFF2B2B5E);     // deep space blue
    auto accentColour = juce::Colour(0xFF6A5ACD);   // violet
    auto activeColour = juce::Colour(0xFF9370DB);   // brighter purple
    auto borderColour = juce::Colour(0xFF483D8B);   // slate
    auto textColour = juce::Colour(0xFFB0AFFF);     // soft white-blue
    auto sliderThumb = juce::Colour(0xFFAD8CFF);    // glowing lavender

    // Load and Stop buttons (text buttons)
    for (auto* button : { &loadButton, &stopButton })
    {
        addAndMakeVisible(button);
        button->addListener(this);
        button->setColour(juce::TextButton::buttonColourId, accentColour);
        button->setColour(juce::TextButton::buttonOnColourId, activeColour);
        button->setColour(juce::TextButton::textColourOffId, textColour);
        button->setColour(juce::TextButton::textColourOnId, juce::Colours::white);
    }

    // Icon buttons
    for (auto* button : { &muteButton, &restartButton, &backwardButton, &playButton,
                         &forwardButton, &goToEndButton, &loopButton })
    {
        addAndMakeVisible(button);
        button->addListener(this);
        button->setColour(juce::DrawableButton::backgroundColourId, accentColour);
        button->setColour(juce::DrawableButton::backgroundOnColourId, activeColour);
        button->setAlpha(0.95f);
    }

    // A-B Segment Looping buttons - consistent purple styling
    for (auto* button : { &markerAButton, &markerBButton, &clearMarkersButton, &segmentLoopButton })
    {
        addAndMakeVisible(button);
        button->addListener(this);
        button->setColour(juce::TextButton::buttonColourId, accentColour);
        button->setColour(juce::TextButton::buttonOnColourId, activeColour);
        button->setColour(juce::TextButton::textColourOffId, textColour);
        button->setColour(juce::TextButton::textColourOnId, juce::Colours::white);
    }

    // Slicing buttons - same styling as A-B buttons
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
    saveSliceButton.setEnabled(false); // Initially disabled until slice is created

    // Slice info label
    sliceInfoLabel.setJustificationType(juce::Justification::centred);
    sliceInfoLabel.setText("Set A-B markers and click Create Slice", juce::dontSendNotification);
    sliceInfoLabel.setColour(juce::Label::textColourId, textColour);
    addAndMakeVisible(sliceInfoLabel);

    // Volume slider
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

    // Position slider
    positionSlider.setRange(0.0, 1.0, 0.0001);
    positionSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    positionSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    positionSlider.addListener(this);
    positionSlider.setColour(juce::Slider::thumbColourId, sliderThumb);
    positionSlider.setColour(juce::Slider::trackColourId, accentColour);
    positionSlider.setColour(juce::Slider::backgroundColourId, juce::Colour(0xFFF0E6FF));
    addAndMakeVisible(positionSlider);

    // Time labels
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
}

PlayerGUI::~PlayerGUI() = default;

void PlayerGUI::resized()
{
    auto area = getLocalBounds();
    int margin = 10;

    // Main transport buttons row (top)
    auto topRow = area.removeFromTop(80); // 80px height for main buttons
    topRow.reduce(margin, margin);

    int buttonWidth = 70;
    int buttonHeight = 70;
    int spacing = 10;

    // Calculate dynamic button positions for main transport buttons
    int totalButtonWidth = (buttonWidth + spacing) * 9 - spacing; // 9 main buttons
    int startX = (topRow.getWidth() - totalButtonWidth) / 2; // Center the buttons

    int x = startX;
    int y = topRow.getY();

    loadButton.setBounds(x, y, buttonWidth, buttonHeight);       x += buttonWidth + spacing;
    muteButton.setBounds(x, y, buttonWidth, buttonHeight);       x += buttonWidth + spacing;
    restartButton.setBounds(x, y, buttonWidth, buttonHeight);    x += buttonWidth + spacing;
    backwardButton.setBounds(x, y, buttonWidth, buttonHeight);   x += buttonWidth + spacing;
    playButton.setBounds(x, y, buttonWidth, buttonHeight);       x += buttonWidth + spacing;
    forwardButton.setBounds(x, y, buttonWidth, buttonHeight);    x += buttonWidth + spacing;
    goToEndButton.setBounds(x, y, buttonWidth, buttonHeight);    x += buttonWidth + spacing;
    loopButton.setBounds(x, y, buttonWidth, buttonHeight);       x += buttonWidth + spacing;
    stopButton.setBounds(x, y, buttonWidth, buttonHeight);

    // A-B Segment Looping buttons row (middle, centered)
    auto middleRow = area.removeFromTop(40); // 40px height for A-B buttons
    middleRow.reduce(margin, 0);

    int abButtonWidth = 70;
    int abButtonHeight = 30;
    int abSpacing = 15;

    // Calculate dynamic positions for A-B buttons
    int totalABWidth = (abButtonWidth + abSpacing) * 4 - abSpacing; // 4 A-B buttons
    int abStartX = (middleRow.getWidth() - totalABWidth) / 2; // Center the A-B buttons

    int abX = abStartX;
    int abY = middleRow.getY() + 5; // Slight vertical padding

    markerAButton.setBounds(abX, abY, abButtonWidth, abButtonHeight);        abX += abButtonWidth + abSpacing;
    markerBButton.setBounds(abX, abY, abButtonWidth, abButtonHeight);        abX += abButtonWidth + abSpacing;
    clearMarkersButton.setBounds(abX, abY, abButtonWidth, abButtonHeight);   abX += abButtonWidth + abSpacing;
    segmentLoopButton.setBounds(abX, abY, abButtonWidth, abButtonHeight);

    // Slicing buttons row (below A-B buttons)
    auto sliceRow = area.removeFromTop(40); // 40px height for slicing buttons
    sliceRow.reduce(margin, 0);

    int sliceButtonWidth = 80;
    int sliceButtonHeight = 30;
    int sliceSpacing = 15;

    // Calculate dynamic positions for slicing buttons
    int totalSliceWidth = (sliceButtonWidth + sliceSpacing) * 2 - sliceSpacing; // 2 slicing buttons
    int sliceStartX = (sliceRow.getWidth() - totalSliceWidth) / 2; // Center the slicing buttons

    int sliceX = sliceStartX;
    int sliceY = sliceRow.getY() + 5; // Slight vertical padding

    sliceButton.setBounds(sliceX, sliceY, sliceButtonWidth, sliceButtonHeight);
    sliceX += sliceButtonWidth + sliceSpacing;
    saveSliceButton.setBounds(sliceX, sliceY, sliceButtonWidth, sliceButtonHeight);

    // Slice info label (below slicing buttons)
    auto infoRow = area.removeFromTop(25);
    sliceInfoLabel.setBounds(infoRow);

    // Bottom controls (position slider and volume)
    auto bottomArea = area.reduced(margin);

    // Position slider and time labels
    int timeLabelWidth = 80;
    int sliderHeight = 25;
    int timeRowHeight = 30;

    auto timeRow = bottomArea.removeFromTop(timeRowHeight);
    currentTimeLabel.setBounds(timeRow.removeFromLeft(timeLabelWidth));
    totalTimeLabel.setBounds(timeRow.removeFromRight(timeLabelWidth));
    positionSlider.setBounds(timeRow);

    // Volume controls
    auto volumeRow = bottomArea.removeFromTop(40);
    volumeLabel.setBounds(volumeRow.removeFromLeft(80));
    volumeSlider.setBounds(volumeRow);
}

void PlayerGUI::buttonClicked(juce::Button* button)
{
    if (!listener) return;

    if (button == &loadButton)     listener->loadButtonClicked();
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
}

void PlayerGUI::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &volumeSlider && listener)
        listener->volumeChanged((float)slider->getValue());
    else if (slider == &positionSlider && listener)
    {
        double cur = positionSlider.getValue() * lastTotalSeconds;
        currentTimeLabel.setText(juce::String::formatted("%d:%02d", (int)(cur / 60.0), (int)((int)cur % 60)), juce::dontSendNotification);

        double newSeconds = positionSlider.getValue() * lastTotalSeconds;
        listener->positionSliderMoved(newSeconds);
    }
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
    // Colors are already set in constructor, toggle state handles the visual change
}

void PlayerGUI::setMarkerBState(bool isSet)
{
    markerBButton.setToggleState(isSet, juce::dontSendNotification);
    // Colors are already set in constructor, toggle state handles the visual change
}

void PlayerGUI::setSegmentLoopState(bool isActive)
{
    segmentLoopButton.setToggleState(isActive, juce::dontSendNotification);
    // Colors are already set in constructor, toggle state handles the visual change
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