/*
  ==============================================================================

   This file is part of the JUCE framework.
   Copyright (c) Raw Material Software Limited

   JUCE is an open source framework subject to commercial or open source
   licensing.

   By downloading, installing, or using the JUCE framework, or combining the
   JUCE framework with any other source code, object code, content or any other
   copyrightable work, you agree to the terms of the JUCE End User Licence
   Agreement, and all incorporated terms including the JUCE Privacy Policy and
   the JUCE Website Terms of Service, as applicable, which will bind you. If you
   do not agree to the terms of these agreements, we will not license the JUCE
   framework to you, and you must discontinue the installation or download
   process and cease use of the JUCE framework.

   JUCE End User Licence Agreement: https://juce.com/legal/juce-9-licence/
   JUCE Privacy Policy: https://juce.com/juce-privacy-policy
   JUCE Website Terms of Service: https://juce.com/juce-website-terms-of-service/

   Or:

   You may also use this code under the terms of the AGPLv3:
   https://www.gnu.org/licenses/agpl-3.0.en.html

   THE JUCE FRAMEWORK IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL
   WARRANTIES, WHETHER EXPRESSED OR IMPLIED, INCLUDING WARRANTY OF
   MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE, ARE DISCLAIMED.

  ==============================================================================
*/

namespace juce
{

DialogWindow::DialogWindow (const String& name, Colour colour,
                            const bool escapeCloses, const bool onDesktop,
                            const float scale)
    : DocumentWindow (name, colour, DocumentWindow::closeButton, onDesktop),
      desktopScale (scale),
      escapeKeyTriggersCloseButton (escapeCloses)
{
}

DialogWindow::~DialogWindow() = default;

bool DialogWindow::escapeKeyPressed()
{
    if (escapeKeyTriggersCloseButton)
    {
        setVisible (false);
        return true;
    }

    return false;
}

bool DialogWindow::keyPressed (const KeyPress& key)
{
    if (key == KeyPress::escapeKey && escapeKeyPressed())
        return true;

    if (isDirectionalFocusNavigationEnabled() && key.isKeyCode (KeyPress::menuKey))
    {
        escapeKeyPressed();
        return true;
    }

    return DocumentWindow::keyPressed (key);
}

void DialogWindow::resized()
{
    DocumentWindow::resized();

    if (escapeKeyTriggersCloseButton)
    {
        if (auto* close = getCloseButton())
        {
            const KeyPress esc (KeyPress::escapeKey, 0, 0);

            if (! close->isRegisteredForShortcut (esc))
                close->addShortcut (esc);
        }
    }
}

//==============================================================================
class DefaultDialogWindow final : public DialogWindow
{
public:
    DefaultDialogWindow (LaunchOptions& options)
        : DialogWindow (options.dialogTitle, options.dialogBackgroundColour,
                        options.escapeKeyTriggersCloseButton, true,
                        options.componentToCentreAround != nullptr
                            ? Component::getApproximateScaleFactorForComponent (options.componentToCentreAround)
                            : 1.0f)
    {
        auto* modeSource = options.componentToCentreAround != nullptr ? options.componentToCentreAround
                                                                      : options.content.get();
        const auto usesDirectionalNavigation = modeSource != nullptr
                                             && modeSource->isDirectionalFocusNavigationEnabled();

        if (options.content.willDeleteObject())
            setContentOwned (options.content.release(), true);
        else
            setContentNonOwned (options.content.release(), true);

        if (modeSource != nullptr)
            setFocusNavigationMode (usesDirectionalNavigation ? FocusNavigationMode::directional
                                                              : FocusNavigationMode::disabled);

        centreAroundComponent (options.componentToCentreAround, getWidth(), getHeight());
        setResizable (options.resizable, options.useBottomRightCornerResizer);

        setUsingNativeTitleBar (options.useNativeTitleBar);
        setAlwaysOnTop (WindowUtils::areThereAnyAlwaysOnTopWindows());
    }

    void closeButtonPressed() override
    {
        setVisible (false);
    }

private:
    JUCE_DECLARE_NON_COPYABLE (DefaultDialogWindow)
};

DialogWindow::LaunchOptions::LaunchOptions() noexcept {}

DialogWindow* DialogWindow::LaunchOptions::create()
{
    jassert (content != nullptr); // You need to provide some kind of content for the dialog!

    return new DefaultDialogWindow (*this);
}

DialogWindow* DialogWindow::LaunchOptions::launchAsync()
{
    auto* d = create();
    d->enterModalState (true, nullptr, true);
    return d;
}

#if JUCE_MODAL_LOOPS_PERMITTED
int DialogWindow::LaunchOptions::runModal()
{
    return launchAsync()->runModalLoop();
}
#endif

//==============================================================================
void DialogWindow::showDialog (const String& dialogTitle,
                               Component* const contentComponent,
                               Component* const componentToCentreAround,
                               Colour backgroundColour,
                               const bool escapeKeyTriggersCloseButton,
                               const bool resizable,
                               const bool useBottomRightCornerResizer)
{
    LaunchOptions o;
    o.dialogTitle = dialogTitle;
    o.content.setNonOwned (contentComponent);
    o.componentToCentreAround = componentToCentreAround;
    o.dialogBackgroundColour = backgroundColour;
    o.escapeKeyTriggersCloseButton = escapeKeyTriggersCloseButton;
    o.useNativeTitleBar = false;
    o.resizable = resizable;
    o.useBottomRightCornerResizer = useBottomRightCornerResizer;

    o.launchAsync();
}

#if JUCE_MODAL_LOOPS_PERMITTED
int DialogWindow::showModalDialog (const String& dialogTitle,
                                   Component* const contentComponent,
                                   Component* const componentToCentreAround,
                                   Colour backgroundColour,
                                   const bool escapeKeyTriggersCloseButton,
                                   const bool resizable,
                                   const bool useBottomRightCornerResizer)
{
    LaunchOptions o;
    o.dialogTitle = dialogTitle;
    o.content.setNonOwned (contentComponent);
    o.componentToCentreAround = componentToCentreAround;
    o.dialogBackgroundColour = backgroundColour;
    o.escapeKeyTriggersCloseButton = escapeKeyTriggersCloseButton;
    o.useNativeTitleBar = false;
    o.resizable = resizable;
    o.useBottomRightCornerResizer = useBottomRightCornerResizer;

    return o.runModal();
}
#endif

//==============================================================================
std::unique_ptr<AccessibilityHandler> DialogWindow::createAccessibilityHandler()
{
    return std::make_unique<AccessibilityHandler> (*this, AccessibilityRole::dialogWindow);
}

#if JUCE_UNIT_TESTS

struct DialogWindowFocusNavigationTests final : UnitTest
{
    DialogWindowFocusNavigationTests()
        : UnitTest ("DialogWindow focus navigation", UnitTestCategories::gui)
    {}

    struct TestDialog final : DialogWindow
    {
        explicit TestDialog (bool escapeCloses)
            : DialogWindow ("Dialog", Colours::black, escapeCloses, false)
        {}

        void closeButtonPressed() override {}
        bool sendKeyPress (const KeyPress& key)    { return keyPressed (key); }
    };

    void runTest() override
    {
        ScopedJuceInitialiser_GUI libraryInitialiser;

        beginTest ("Directional Menu closes or remains contained by the dialog");
        {
            TestDialog dismissible (true);
            dismissible.setFocusNavigationMode (FocusNavigationMode::directional);
            expect (dismissible.sendKeyPress (KeyPress (KeyPress::menuKey)));

            TestDialog nonDismissible (false);
            nonDismissible.setFocusNavigationMode (FocusNavigationMode::directional);
            expect (nonDismissible.sendKeyPress (KeyPress (KeyPress::menuKey)));
        }

        beginTest ("Disabled directional navigation preserves the existing Menu result");
        {
            TestDialog dialog (false);
            dialog.setFocusNavigationMode (FocusNavigationMode::disabled);
            expect (! dialog.sendKeyPress (KeyPress (KeyPress::menuKey)));
        }

        beginTest ("LaunchOptions carries explicit directional mode into the new window");
        {
            Component content;
            content.setSize (100, 80);
            content.setFocusNavigationMode (FocusNavigationMode::directional);
            DialogWindow::LaunchOptions options;
            options.content.setNonOwned (&content);
            std::unique_ptr<DialogWindow> dialog (options.create());
            expect (dialog->getFocusNavigationMode() == FocusNavigationMode::directional);
        }
    }
};

static DialogWindowFocusNavigationTests dialogWindowFocusNavigationTests;

#endif

} // namespace juce
