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

    // Track Markers buttons - FIXED: Added missing addAndMakeVisible calls
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

    // Markers list
    addAndMakeVisible(markersList);
    markersList.setModel(this);
    markersList.setRowHeight(25);
    markersList.setColour(juce::ListBox::backgroundColourId, baseColour.withAlpha(0.7f));
    markersList.setColour(juce::ListBox::outlineColourId, borderColour);

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
    int margin = 8; // Reduced margin for more space

    // === 1. Main Transport Buttons (Top Row) ===
    auto topRow = area.removeFromTop(70); // Reduced from 80
    topRow.reduce(margin, 5);

    int buttonWidth = 65; // Slightly smaller buttons
    int buttonHeight = 65;
    int spacing = 8;

    // Calculate centered positions
    int totalButtonWidth = (buttonWidth + spacing) * 9 - spacing;
    int startX = (topRow.getWidth() - totalButtonWidth) / 2;

    int x = startX;
    int y = topRow.getY() + 2; // Slight vertical adjustment

    loadButton.setBounds(x, y, buttonWidth, buttonHeight);       x += buttonWidth + spacing;
    muteButton.setBounds(x, y, buttonWidth, buttonHeight);       x += buttonWidth + spacing;
    restartButton.setBounds(x, y, buttonWidth, buttonHeight);    x += buttonWidth + spacing;
    backwardButton.setBounds(x, y, buttonWidth, buttonHeight);   x += buttonWidth + spacing;
    playButton.setBounds(x, y, buttonWidth, buttonHeight);       x += buttonWidth + spacing;
    forwardButton.setBounds(x, y, buttonWidth, buttonHeight);    x += buttonWidth + spacing;
    goToEndButton.setBounds(x, y, buttonWidth, buttonHeight);    x += buttonWidth + spacing;
    loopButton.setBounds(x, y, buttonWidth, buttonHeight);       x += buttonWidth + spacing;
    stopButton.setBounds(x, y, buttonWidth, buttonHeight);

    // === 2. A-B Segment Looping Row ===
    auto abRow = area.removeFromTop(35); // Reduced from 40
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

    // === 3. Slicing Row ===
    auto sliceRow = area.removeFromTop(35); // Reduced from 40
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

    // === 4. Slice Info Label ===
    auto infoRow = area.removeFromTop(22); // Reduced from 25
    sliceInfoLabel.setBounds(infoRow.reduced(5, 2));

    // === 5. Track Markers Section ===
    auto markersSection = area.removeFromTop(100); // Reduced from 120
    markersSection.reduce(margin, 4);

    // Marker buttons row
    auto markerButtonsRow = markersSection.removeFromTop(28);
    int markerButtonWidth = 100;
    addMarkerButton.setBounds(markerButtonsRow.removeFromLeft(markerButtonWidth).reduced(2));
    deleteMarkerButton.setBounds(markerButtonsRow.removeFromLeft(markerButtonWidth).reduced(2));

    // Markers list - takes remaining space in markers section
    markersList.setBounds(markersSection.reduced(2));

    // === 6. Position Slider & Time Labels ===
    auto positionArea = area.removeFromTop(45); // Combined area for position
    positionArea.reduce(margin, 0);

    // Time labels
    int timeLabelWidth = 70;
    auto timeLabelArea = positionArea.removeFromTop(20);
    currentTimeLabel.setBounds(timeLabelArea.removeFromLeft(timeLabelWidth));
    totalTimeLabel.setBounds(timeLabelArea.removeFromRight(timeLabelWidth));

    // Position slider
    positionSlider.setBounds(positionArea.reduced(0, 2));

    // === 7. Volume Controls ===
    auto volumeArea = area.removeFromTop(35);
    volumeArea.reduce(margin, 0);
    volumeLabel.setBounds(volumeArea.removeFromLeft(70));
    volumeSlider.setBounds(volumeArea);

    // DEBUG: Check if we have any space left (should be very little)
    if (area.getHeight() > 10) {
        DBG("Warning: " << area.getHeight() << " pixels unused in layout");
    }
}

// ... REST OF YOUR METHODS REMAIN THE SAME ...
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
    else if (button == &addMarkerButton) listener->addMarkerButtonClicked();
    else if (button == &deleteMarkerButton) listener->deleteMarkerButtonClicked();
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

void PlayerGUI::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    // Update the markers list when markers change
    markersList.updateContent();
    markersList.repaint();
}

void PlayerGUI::setListener(Listener* newListener)
{
    listener = newListener;
}

void PlayerGUI::loadButtonIcons()
{
    // Your existing icon loading code...
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

// ListBoxModel implementation
int PlayerGUI::getNumRows()
{
    // This will be implemented in MainComponent
    return 0;
}

void PlayerGUI::paintListBoxItem(int rowNumber, juce::Graphics& g,
    int width, int height, bool rowIsSelected)
{
    // This will be implemented in MainComponent
}

void PlayerGUI::listBoxItemClicked(int row, const juce::MouseEvent& event)
{
    if (listener)
        listener->jumpToMarker(row);
}