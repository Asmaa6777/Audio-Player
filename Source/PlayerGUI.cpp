#include "PlayerGUI.h"

PlayerGUI::PlayerGUI()
{
    loadButtonIcons(); 
    auto baseColour = juce::Colour(0xFFE6E6FA); // lavender
    auto accentColour = juce::Colour(0xFFB19CD9); // light purple
    auto activeColour = juce::Colour(0xFF9370DB); // medium purple
    auto borderColour = juce::Colour(0xFF6A5ACD); // slate blue
    auto textColour = juce::Colour(0xFF191970); // midnight blue (navy)
    auto sliderThumb = juce::Colour(0xFF483D8B); // dark slate blue
     
    for (auto* button : { &loadButton, &stopButton })
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

    
    volumeSlider.setRange(0.0, 1.0, 0.01);
    volumeSlider.setValue(0.5);
    volumeSlider.addListener(this);
    volumeSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    volumeSlider.setColour(juce::Slider::thumbColourId, sliderThumb);
    volumeSlider.setColour(juce::Slider::trackColourId, accentColour);
    volumeSlider.setColour(juce::Slider::backgroundColourId, juce::Colour(0xFFF0E6FF)); // very light lavender
    volumeSlider.setColour(juce::Slider::textBoxTextColourId, textColour);
    volumeSlider.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(0xFFF8F6FF));
    volumeSlider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colour(0xFFD8BFD8));
    addAndMakeVisible(volumeSlider);
 
    volumeLabel.setJustificationType(juce::Justification::centredLeft);
    volumeLabel.setText("Volume", juce::dontSendNotification);
    volumeLabel.setColour(juce::Label::textColourId, textColour);
    addAndMakeVisible(volumeLabel);

 
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
}

PlayerGUI::~PlayerGUI() = default;

void PlayerGUI::paint(juce::Graphics& g)
{
    // Lavender gradient background
    juce::ColourGradient gradient(
        juce::Colour(0xFFF5F0FF), 0, 0,          // top — very light lavender
        juce::Colour(0xFFE6E6FA), 0, (float)getHeight(), // bottom — lavender
        false
    );
    g.setGradientFill(gradient);
    g.fillAll();

    // Navy blue border
    g.setColour(juce::Colour(0xFF191970));
    g.drawRoundedRectangle(getLocalBounds().toFloat().reduced(1.0f), 12.0f, 2.5f);
}

void PlayerGUI::resized()
{
    const int y = 50;
    const int buttonWidth = 70;
    const int buttonHeight = 70;
    const int spacing = 10;

    int x = 20;

    loadButton.setBounds(x, y, buttonWidth, buttonHeight);       x += buttonWidth + spacing;
    muteButton.setBounds(x, y, buttonWidth, buttonHeight);       x += buttonWidth + spacing;
    restartButton.setBounds(x, y, buttonWidth, buttonHeight);    x += buttonWidth + spacing;
    backwardButton.setBounds(x, y, buttonWidth, buttonHeight);   x += buttonWidth + spacing;
    playButton.setBounds(x, y, buttonWidth, buttonHeight);       x += buttonWidth + spacing;
    forwardButton.setBounds(x, y, buttonWidth, buttonHeight);    x += buttonWidth + spacing;
    goToEndButton.setBounds(x, y, buttonWidth, buttonHeight);    x += buttonWidth + spacing;
    loopButton.setBounds(x, y, buttonWidth, buttonHeight);       x += buttonWidth + spacing;
    stopButton.setBounds(x, y, buttonWidth, buttonHeight);

    int labelWidth = 100;
    int labelHeight = 20;
    int sliderHeight = 30;

    currentTimeLabel.setBounds(20, 135, labelWidth, labelHeight);
    totalTimeLabel.setBounds(getWidth() - 20 - labelWidth, 135, labelWidth, labelHeight);

    positionSlider.setBounds(20 + labelWidth + 5, 135 - 5, getWidth() - (labelWidth * 2 + 50), sliderHeight);

    volumeLabel.setBounds(20, 135 + 45, 100, 20);
    volumeSlider.setBounds(20, 135 + 70, getWidth() - 40, 30);
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

    auto base = juce::Colour(0xFFB19CD9); // light purple
    auto active = juce::Colour(0xFF9370DB); // medium purple  
    loopButton.setColour(juce::DrawableButton::backgroundColourId, isLooping ? active : base);
    loopButton.setColour(juce::DrawableButton::backgroundOnColourId, isLooping ? active : base);
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