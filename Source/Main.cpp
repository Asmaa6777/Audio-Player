#include <JuceHeader.h>
#include "MainComponent.h"

class SimpleAudioPlayer : public juce::JUCEApplication
{
public:
    const juce::String getApplicationName() override { return "Simple Audio Player"; }
    const juce::String getApplicationVersion() override { return "1.0"; }
    bool moreThanOneInstanceAllowed() override { return true; }

    void initialise(const juce::String& commandLine) override
    {
        mainWindow = std::make_unique<MainWindow>(getApplicationName());

        // Optional: Handle command line file opening
        if (commandLine.isNotEmpty())
        {
            juce::File file(commandLine.unquoted());
            if (file.existsAsFile())
            {
                // You could add logic here to auto-load the file
            }
        }
    }

    void shutdown() override
    {
        mainWindow = nullptr;
    }

    void systemRequestedQuit() override
    {
        quit();
    }

    void anotherInstanceStarted(const juce::String& commandLine) override
    {
        // Bring existing window to front when another instance is launched
        if (mainWindow != nullptr)
            mainWindow->toFront(true);
    }

private:
    class MainWindow : public juce::DocumentWindow
    {
    public:
        MainWindow(juce::String name)
            : DocumentWindow(name,
                juce::Colours::darkgrey,  // More neutral default color
                DocumentWindow::allButtons)
        {
            setUsingNativeTitleBar(true);
            setContentOwned(new MainComponent(), true);

            // Set a more reasonable minimum size
            setResizable(true, true);
            setResizeLimits(600, 400, 1200, 800);

            centreWithSize(800, 400);
            setVisible(true);
        }

        void closeButtonPressed() override
        {
            juce::JUCEApplication::getInstance()->systemRequestedQuit();
        }

        // Optional: Handle window resizing more gracefully
        void resized() override
        {
            DocumentWindow::resized();
            // Any additional window-level resizing logic can go here
        }

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindow)
    };

    std::unique_ptr<MainWindow> mainWindow;
};

START_JUCE_APPLICATION(SimpleAudioPlayer)