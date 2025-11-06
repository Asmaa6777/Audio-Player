#include <JuceHeader.h>
#include "MainComponent.h"

class SimpleAudioPlayer : public juce::JUCEApplication
{
public:
    const juce::String getApplicationName() override { return "Advanced Audio Player"; }
    const juce::String getApplicationVersion() override { return "2.0"; }
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
                juce::Colours::darkgrey,
                DocumentWindow::allButtons)
        {
            setUsingNativeTitleBar(true);
            setContentOwned(new MainComponent(), true);

            // Set a more reasonable minimum size for two players
            setResizable(true, true);
            setResizeLimits(800, 600, 1600, 1200);

            centreWithSize(1000, 800);
            setVisible(true);
        }

        void closeButtonPressed() override
        {
            juce::JUCEApplication::getInstance()->systemRequestedQuit();
        }

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindow)
    };

    std::unique_ptr<MainWindow> mainWindow;
};

START_JUCE_APPLICATION(SimpleAudioPlayer)