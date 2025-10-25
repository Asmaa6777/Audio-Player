#include "PlayerGUI.h"

PlayerGUI::PlayerGUI()
{
    loadButtonIcons();

    // === 🎨 Lavender Haze Color Palette ===
    auto baseColour = juce::Colour(0xFFD8CFFC); // pastel lavender
    auto accentColour = juce::Colour(0xFFB5A8F0); // soft lilac
    auto activeColour = juce::Colour(0xFF9A8CFF); // glowing lavender
    auto borderColour = juce::Colour(0xFF7D6AE0); // deep violet
    auto textColour = juce::Colour(0xFF3D2C6B); // indigo text
    auto sliderThumb = juce::Colour(0xFF8A7CE6); // medium orchid

    // === Text buttons (Load + Stop) ===
    for (auto* button : { &loadButton, &stopButton })
    {
        addAndMakeVisible(button);
        button->addListener(this);
        button->setColour(juce::TextButton::buttonColourId, accentColour);
        button->setColour(juce::TextButton::buttonOnColourId, activeColour);
        button->setColour(juce::TextButton::textColourOffId, textColour);
        button->setColour(juce::TextButton::textColourOnId, juce::Colours::white);
    }

    // === Image buttons ===
    for (auto* button : { &muteButton, &restartButton, &backwardButton, &playButton,
                          &forwardButton, &goToEndButton, &loopButton })
    {
        addAndMakeVisible(button);
        button->addListener(this);
        button->setColour(juce::DrawableButton::backgroundColourId, accentColour);
        button->setColour(juce::DrawableButton::backgroundOnColourId, activeColour);
        button->setAlpha(0.95f);
    }

    // === Volume slider ===
    volumeSlider.setRange(0.0, 1.0, 0.01);
    volumeSlider.setValue(0.5);
    volumeSlider.addListener(this);
    volumeSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    volumeSlider.setColour(juce::Slider::thumbColourId, sliderThumb);
    volumeSlider.setColour(juce::Slider::backgroundColourId, juce::Colour(0xFFE7E3FF)); // misty violet
    volumeSlider.setColour(juce::Slider::textBoxTextColourId, textColour);
    volumeSlider.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(0xFFF5F3FF));
    addAndMakeVisible(volumeSlider);

    // === Volume label ===
    volumeLabel.setJustificationType(juce::Justification::centredLeft);
    volumeLabel.setText("Volume", juce::dontSendNotification);
    volumeLabel.setColour(juce::Label::textColourId, textColour);
    addAndMakeVisible(volumeLabel);

    setLoopState(false);
    setPlaybackState(false);
    setMuteState(false);
}

PlayerGUI::~PlayerGUI() = default;

// === 🌙 Background + border ===
void PlayerGUI::paint(juce::Graphics& g)
{
    juce::ColourGradient gradient(
        juce::Colour(0xFFF6F4FF), 0, 0,          // top — soft white-lavender
        juce::Colour(0xFFDAD4FF), 0, (float)getHeight(), // bottom — faint violet haze
        false
    );
    g.setGradientFill(gradient);
    g.fillAll();

    g.setColour(juce::Colour(0xFF9A8CFF)); // medium lavender border
    g.drawRoundedRectangle(getLocalBounds().toFloat().reduced(1.0f), 12.0f, 3.0f);
}

void PlayerGUI::resized()
{
    const int y = 50;
    const int buttonWidth = 70;
    const int buttonHeight = 70;
    const int spacing = 10;

    int x = 20;

    // Layout: Load | Mute | Restart | <<10 | [PLAY] | >>10 | GoToEnd | Loop | Stop
    loadButton.setBounds(x, y, buttonWidth, buttonHeight);       x += buttonWidth + spacing;
    muteButton.setBounds(x, y, buttonWidth, buttonHeight);       x += buttonWidth + spacing;
    restartButton.setBounds(x, y, buttonWidth, buttonHeight);    x += buttonWidth + spacing;
    backwardButton.setBounds(x, y, buttonWidth, buttonHeight);   x += buttonWidth + spacing;
    playButton.setBounds(x, y, buttonWidth, buttonHeight);       x += buttonWidth + spacing;
    forwardButton.setBounds(x, y, buttonWidth, buttonHeight);    x += buttonWidth + spacing;
    goToEndButton.setBounds(x, y, buttonWidth, buttonHeight);    x += buttonWidth + spacing;
    loopButton.setBounds(x, y, buttonWidth, buttonHeight);       x += buttonWidth + spacing;
    stopButton.setBounds(x, y, buttonWidth, buttonHeight);

    volumeLabel.setBounds(20, 135, 100, 20);
    volumeSlider.setBounds(20, 160, getWidth() - 40, 30);
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
}

void PlayerGUI::setListener(Listener* newListener)
{
    listener = newListener;
}

void PlayerGUI::loadButtonIcons()
{
    // === 🎵 Image loading ===
    auto playImg = juce::Drawable::createFromImageData(BinaryData::play_jpg, BinaryData::play_jpgSize);
    auto pauseImg = juce::Drawable::createFromImageData(BinaryData::pause_jpg, BinaryData::pause_jpgSize);
    playButton.setImages(playImg.get(), nullptr, nullptr, nullptr, pauseImg.get());

    // 🔊 Correct order: "mute" = unmuted (default), "unmute" = muted (toggled)
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

    auto base = juce::Colour(0xFFB5A8F0); // soft lilac
    auto active = juce::Colour(0xFF9A8CFF); // bright lavender
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
