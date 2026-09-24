/*
  ==============================================================================

   This file is part of the JUCE framework examples.
   Copyright (c) Raw Material Software Limited

   The code included in this file is provided under the terms of the ISC license
   http://www.isc.org/downloads/software-support-policy/isc-license. Permission
   to use, copy, modify, and/or distribute this software for any purpose with or
   without fee is hereby granted provided that the above copyright notice and
   this permission notice appear in all copies.

   THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
   REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
   AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
   INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
   LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
   OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
   PERFORMANCE OF THIS SOFTWARE.

  ==============================================================================
*/

/*******************************************************************************
 The block below describes the properties of this PIP. A PIP is a short snippet
 of code that can be read by the Projucer and used to generate a JUCE project.

 BEGIN_JUCE_PIP_METADATA

 name:             FocusNavigationDemo
 version:          1.0.0
 vendor:           JUCE
 website:          http://juce.com
 description:      Demonstrates directional focus navigation and widget policies.

 dependencies:     juce_core, juce_data_structures, juce_events, juce_graphics,
                   juce_gui_basics, juce_gui_extra
 exporters:        xcode_mac, vs2022, vs2026, linux_make, androidstudio,
                   xcode_iphone, xcode_tvos

 moduleFlags:      JUCE_STRICT_REFCOUNTEDPOINTER=1

 type:             Component
 mainClass:        FocusNavigationDemo

 useLocalCopy:     1

 END_JUCE_PIP_METADATA

*******************************************************************************/

#pragma once

#include "../Assets/DemoUtilities.h"

//==============================================================================
class FocusNavigationDemo final : public Component,
                                  private FocusChangeListener,
                                  private KeyListener
{
public:
    FocusNavigationDemo()
        : gridPage (trace),
          containersPage (trace),
          valuesPage (trace),
          collectionsPage (trace),
          editingPage (trace),
          inputPage (trace),
          policyPage (*this, trace)
    {
        setName ("Focus navigation demo");

        addAndMakeVisible (scenarios);
        addAndMakeVisible (trace);

        addScenario ("Grid",        gridPage);
        addScenario ("Policy",      policyPage);
        addScenario ("Containers",  containersPage);
        addScenario ("Values",      valuesPage);
        addScenario ("Collections", collectionsPage);
        addScenario ("Editing",     editingPage);
        addScenario ("Input",       inputPage);

        scenarios.setTabBarDepth (38);

        policyPage.setTargets ({
            { "Demo root", this },
            { "Grid R2C2", &gridPage.getCentreButton() },
            { "Hosted placeholder", &containersPage.getHostedPlaceholder() },
            { "Primary slider", &valuesPage.getPrimarySlider() },
            { "List", &collectionsPage.getListBox() },
            { "Text editor", &editingPage.getTextEditor() }
        });

        addKeyListener (this);
        Desktop::getInstance().addFocusChangeListener (this);

        setSize (1000, 760);
        trace.log ("Ready. Focus a control and use directional input.");
    }

    ~FocusNavigationDemo() override
    {
        setLookAndFeel (nullptr);
        Desktop::getInstance().removeFocusChangeListener (this);
        removeKeyListener (this);
    }

    void resized() override
    {
        auto bounds = getLocalBounds().reduced (8);
        trace.setBounds (bounds.removeFromBottom (150));
        bounds.removeFromBottom (8);
        scenarios.setBounds (bounds);
    }

    void visibilityChanged() override
    {
        if (! isShowing() || hasKeyboardFocus (true))
            return;

        MessageManager::callAsync ([safeThis = Component::SafePointer<FocusNavigationDemo> (this)]
        {
            if (safeThis != nullptr && safeThis->isShowing() && ! safeThis->hasKeyboardFocus (true))
                safeThis->gridPage.getCentreButton().grabKeyboardFocus();
        });
    }

private:
    //==============================================================================
    static String directionName (FocusNavigationDirection direction)
    {
        switch (direction)
        {
            case FocusNavigationDirection::left:  return "left";
            case FocusNavigationDirection::right: return "right";
            case FocusNavigationDirection::up:    return "up";
            case FocusNavigationDirection::down:  return "down";
        }

        jassertfalse;
        return {};
    }

    static String resultName (FocusNavigationResult result)
    {
        switch (result)
        {
            case FocusNavigationResult::unhandled: return "unhandled";
            case FocusNavigationResult::handled:   return "handled";
            case FocusNavigationResult::blocked:   return "blocked";
        }

        jassertfalse;
        return {};
    }

    static String componentName (const Component* component)
    {
        if (component == nullptr)
            return "<none>";

        if (component->getName().isNotEmpty())
            return component->getName();

        if (component->getTitle().isNotEmpty())
            return component->getTitle();

        return "<unnamed>";
    }

    static void configureLabel (Label& label, const String& text)
    {
        label.setText (text, dontSendNotification);
        label.setJustificationType (Justification::centredLeft);
    }

    static void configureButton (Button& button, const String& name)
    {
        button.setName (name);
        button.setHasFocusOutline (true);
    }

    //==============================================================================
    class TracePanel final : public Component
    {
    public:
        TracePanel()
        {
            setName ("Input and focus trace");
            configureLabel (title, "Input route and focus owner");
            title.setFont (FontOptions { 15.0f, Font::bold });
            addAndMakeVisible (title);

            clearButton.onClick = [this]
            {
                lines.clear();
                repaint();
            };

            configureButton (clearButton, "Clear trace");
            addAndMakeVisible (clearButton);
        }

        void log (const String& message)
        {
            lines.insert (0, Time::getCurrentTime().toString (false, true, true, true)
                                + "  " + message);

            while (lines.size() > 6)
                lines.remove (lines.size() - 1);

            repaint();
        }

        void paint (Graphics& g) override
        {
            auto bounds = getLocalBounds().toFloat();
            g.setColour (findColour (TextEditor::backgroundColourId).contrasting (0.05f));
            g.fillRoundedRectangle (bounds, 6.0f);

            g.setColour (findColour (Label::textColourId));
            g.drawFittedText (lines.joinIntoString ("\n"),
                              getLocalBounds().reduced (10).withTrimmedTop (30),
                              Justification::topLeft, 6, 0.85f);
        }

        void resized() override
        {
            auto header = getLocalBounds().reduced (8).removeFromTop (26);
            clearButton.setBounds (header.removeFromRight (100));
            title.setBounds (header);
        }

    private:
        Label title;
        TextButton clearButton { "Clear" };
        StringArray lines;
    };

    //==============================================================================
    class ResultButton final : public TextButton
    {
    public:
        ResultButton (TracePanel& traceToUse,
                      const String& text,
                      FocusNavigationDirection directionToIntercept,
                      FocusNavigationResult responseToUse)
            : TextButton (text),
              trace (traceToUse),
              interceptedDirection (directionToIntercept),
              response (responseToUse)
        {
            configureButton (*this, text);
            onClick = [this] { trace.log ("activated: " + getName()); };
        }

        FocusNavigationResult handleFocusNavigation (FocusNavigationDirection direction) override
        {
            const auto result = direction == interceptedDirection ? response
                                                                  : FocusNavigationResult::unhandled;
            trace.log ("widget: " + getName() + " | " + directionName (direction)
                       + " -> " + resultName (result));
            return result;
        }

    private:
        TracePanel& trace;
        FocusNavigationDirection interceptedDirection;
        FocusNavigationResult response;
    };

    class LoggingTraverser final : public KeyboardFocusTraverser
    {
    public:
        explicit LoggingTraverser (TracePanel& traceToUse) : trace (traceToUse) {}

        Component* getComponentInDirection (Component* current,
                                            FocusNavigationDirection direction) override
        {
            auto* destination = KeyboardFocusTraverser::getComponentInDirection (current, direction);
            trace.log ("traverser: " + FocusNavigationDemo::componentName (current) + " -> "
                       + FocusNavigationDemo::componentName (destination)
                       + " (" + directionName (direction) + ")");
            return destination;
        }

    private:
        TracePanel& trace;
    };

    class GridPage final : public Component
    {
    public:
        explicit GridPage (TracePanel& traceToUse)
            : trace (traceToUse),
              buttons {
                  std::make_unique<ResultButton> (trace, "R1C1", FocusNavigationDirection::up,
                                                  FocusNavigationResult::unhandled),
                  std::make_unique<ResultButton> (trace, "R1C2", FocusNavigationDirection::up,
                                                  FocusNavigationResult::unhandled),
                  std::make_unique<ResultButton> (trace, "R1C3", FocusNavigationDirection::up,
                                                  FocusNavigationResult::unhandled),
                  std::make_unique<ResultButton> (trace, "Unhandled ->", FocusNavigationDirection::right,
                                                  FocusNavigationResult::unhandled),
                  std::make_unique<ResultButton> (trace, "Handled v", FocusNavigationDirection::down,
                                                  FocusNavigationResult::handled),
                  std::make_unique<ResultButton> (trace, "Blocked ->", FocusNavigationDirection::right,
                                                  FocusNavigationResult::blocked),
                  std::make_unique<ResultButton> (trace, "R3C1", FocusNavigationDirection::down,
                                                  FocusNavigationResult::unhandled),
                  std::make_unique<ResultButton> (trace, "R3C2", FocusNavigationDirection::down,
                                                  FocusNavigationResult::unhandled),
                  std::make_unique<ResultButton> (trace, "R3C3", FocusNavigationDirection::down,
                                                  FocusNavigationResult::unhandled)
              }
        {
            setName ("Spatial grid");
            setFocusContainerType (FocusContainerType::keyboardFocusContainer);
            configureLabel (instructions,
                            "Beam-first 3 x 3 grid. The middle row exposes unhandled, handled, "
                            "and blocked results in the arrow shown.");
            instructions.setJustificationType (Justification::centred);
            addAndMakeVisible (instructions);

            for (auto& button : buttons)
                addAndMakeVisible (*button);
        }

        ResultButton& getCentreButton() const    { return *buttons[4]; }

        std::unique_ptr<ComponentTraverser> createKeyboardFocusTraverser() override
        {
            return std::make_unique<LoggingTraverser> (trace);
        }

        void resized() override
        {
            auto bounds = getLocalBounds().reduced (30);
            instructions.setBounds (bounds.removeFromTop (55));
            bounds.reduce (jmax (0, bounds.getWidth() / 8), 15);

            const auto cellWidth = bounds.getWidth() / 3;
            const auto cellHeight = bounds.getHeight() / 3;

            for (int row = 0; row < 3; ++row)
                for (int column = 0; column < 3; ++column)
                    buttons[(size_t) (row * 3 + column)]->setBounds (
                        bounds.getX() + column * cellWidth + 8,
                        bounds.getY() + row * cellHeight + 8,
                        cellWidth - 16,
                        cellHeight - 16);
        }

    private:
        TracePanel& trace;
        Label instructions;
        std::array<std::unique_ptr<ResultButton>, 9> buttons;
    };

    //==============================================================================
    class DemoLookAndFeel final : public LookAndFeel_V4
    {
    public:
        std::unique_ptr<FocusOutline> createFocusOutlineForComponent (Component&) override
        {
            struct Properties final : public FocusOutline::OutlineWindowProperties
            {
                Rectangle<int> getOutlineBounds (Component& component) override
                {
                    return component.getScreenBounds().expanded (7);
                }

                void drawOutline (Graphics& g, int width, int height) override
                {
                    g.setColour (Colours::magenta);
                    g.drawRoundedRectangle (Rectangle<float> (0.0f, 0.0f,
                                                              (float) width, (float) height).reduced (2.0f),
                                            12.0f, 4.0f);
                }
            };

            return std::make_unique<FocusOutline> (std::make_unique<Properties>());
        }
    };

    class PolicyPage final : public Component
    {
    public:
        struct Target
        {
            String name;
            Component* component = nullptr;
        };

        PolicyPage (FocusNavigationDemo& rootToUse, TracePanel& traceToUse)
            : root (rootToUse), trace (traceToUse)
        {
            configureLabel (instructions,
                            "Change live navigation, eligibility, traversal-order, and outline policy.");
            instructions.setJustificationType (Justification::centred);
            addAndMakeVisible (instructions);

            targetSelector.setName ("Policy target");
            targetSelector.setHasFocusOutline (true);
            targetSelector.onChange = [this] { updateControls(); };
            addAndMakeVisible (targetSelector);

            modeSelector.setName ("Navigation mode");
            modeSelector.setHasFocusOutline (true);
            modeSelector.addItemList ({ "Inherit", "Platform default", "Disabled", "Directional" }, 1);
            modeSelector.onChange = [this]
            {
                if (auto* target = getTarget())
                {
                    target->setFocusNavigationMode (
                        static_cast<FocusNavigationMode> (modeSelector.getSelectedId() - 1));
                    trace.log ("policy: " + FocusNavigationDemo::componentName (target) + " mode -> "
                               + modeSelector.getText()
                               + (target->isDirectionalFocusNavigationEnabled()
                                      ? " (resolved directional)" : " (resolved disabled)"));
                }
            };
            addAndMakeVisible (modeSelector);

            initialiseToggle (wantsFocus, "Wants keyboard focus", [] (Component& target, bool value)
            {
                target.setWantsKeyboardFocus (value);
            });
            initialiseToggle (enabled, "Enabled", [] (Component& target, bool value)
            {
                target.setEnabled (value);
            });
            initialiseToggle (visible, "Visible", [] (Component& target, bool value)
            {
                target.setVisible (value);
            });
            initialiseToggle (outline, "Focus outline", [] (Component& target, bool value)
            {
                target.setHasFocusOutline (value);
            });
            initialiseToggle (explicitOrder, "Explicit order 10", [] (Component& target, bool value)
            {
                target.setExplicitFocusOrder (value ? 10 : 0);
            });

            configureButton (customOutline, "Custom focus outline");
            customOutline.onClick = [this]
            {
                root.setLookAndFeel (customOutline.getToggleState() ? &lookAndFeel : nullptr);
                trace.log (customOutline.getToggleState()
                               ? "look-and-feel: magenta rounded outline"
                               : "look-and-feel: inherited outline");
            };
            addAndMakeVisible (customOutline);

            resolvedState.setJustificationType (Justification::centred);
            addAndMakeVisible (resolvedState);
        }

        void setTargets (std::initializer_list<Target> newTargets)
        {
            targets.assign (newTargets);
            targetSelector.clear();

            for (size_t i = 0; i < targets.size(); ++i)
                targetSelector.addItem (targets[i].name, (int) i + 1);

            targetSelector.setSelectedId (1, sendNotification);
        }

        void resized() override
        {
            auto bounds = getLocalBounds().reduced (30);
            instructions.setBounds (bounds.removeFromTop (50));
            resolvedState.setBounds (bounds.removeFromBottom (50));

            const auto rowHeight = 44;
            targetSelector.setBounds (bounds.removeFromTop (rowHeight).reduced (80, 4));
            modeSelector.setBounds (bounds.removeFromTop (rowHeight).reduced (80, 4));

            auto toggles = bounds.removeFromTop (rowHeight * 3);
            auto left = toggles.removeFromLeft (toggles.getWidth() / 2);
            wantsFocus.setBounds (left.removeFromTop (rowHeight));
            enabled.setBounds (left.removeFromTop (rowHeight));
            visible.setBounds (left.removeFromTop (rowHeight));
            outline.setBounds (toggles.removeFromTop (rowHeight));
            explicitOrder.setBounds (toggles.removeFromTop (rowHeight));
            customOutline.setBounds (toggles.removeFromTop (rowHeight));
        }

    private:
        template <typename Callback>
        void initialiseToggle (ToggleButton& button, const String& text, Callback&& callbackIn)
        {
            button.setButtonText (text);
            configureButton (button, text);
            button.onClick = [this, &button, callback = std::forward<Callback> (callbackIn)]
            {
                if (auto* target = getTarget())
                {
                    callback (*target, button.getToggleState());
                    trace.log ("policy: " + FocusNavigationDemo::componentName (target)
                               + " " + button.getButtonText()
                               + " -> " + (button.getToggleState() ? "on" : "off"));
                    updateControls();
                }
            };
            addAndMakeVisible (button);
        }

        Component* getTarget() const
        {
            const auto index = targetSelector.getSelectedId() - 1;
            return isPositiveAndBelow (index, (int) targets.size()) ? targets[(size_t) index].component
                                                                   : nullptr;
        }

        void updateControls()
        {
            auto* target = getTarget();

            if (target == nullptr)
                return;

            modeSelector.setSelectedId ((int) target->getFocusNavigationMode() + 1,
                                        dontSendNotification);
            wantsFocus.setToggleState (target->getWantsKeyboardFocus(), dontSendNotification);
            enabled.setToggleState (target->isEnabled(), dontSendNotification);
            visible.setToggleState (target->isVisible(), dontSendNotification);
            outline.setToggleState (target->hasFocusOutline(), dontSendNotification);
            explicitOrder.setToggleState (target->getExplicitFocusOrder() > 0, dontSendNotification);

            const auto isRoot = target == &root;
            enabled.setEnabled (! isRoot);
            visible.setEnabled (! isRoot);
            resolvedState.setText ("Selected: " + FocusNavigationDemo::componentName (target)
                                       + " | resolved mode: "
                                       + (target->isDirectionalFocusNavigationEnabled()
                                              ? "directional" : "disabled"),
                                   dontSendNotification);
        }

        FocusNavigationDemo& root;
        TracePanel& trace;
        DemoLookAndFeel lookAndFeel;
        std::vector<Target> targets;
        Label instructions, resolvedState;
        ComboBox targetSelector, modeSelector;
        ToggleButton wantsFocus, enabled, visible, outline, explicitOrder, customOutline;
    };

    //==============================================================================
    class FocusGroup final : public Component
    {
    public:
        FocusGroup (const String& name, TracePanel& traceToUse)
            : trace (traceToUse), first (name + " 1"), second (name + " 2")
        {
            setName (name);
            setFocusContainerType (FocusContainerType::keyboardFocusContainer);

            for (auto* button : { &first, &second })
            {
                configureButton (*button, button->getButtonText());
                button->onClick = [this, button] { trace.log ("activated: " + button->getName()); };
                addAndMakeVisible (*button);
            }
        }

        void paint (Graphics& g) override
        {
            g.setColour (findColour (GroupComponent::outlineColourId));
            g.drawRoundedRectangle (getLocalBounds().toFloat().reduced (1.0f), 5.0f, 2.0f);
            g.drawText (getName(), getLocalBounds().removeFromTop (24),
                        Justification::centred, false);
        }

        void resized() override
        {
            auto bounds = getLocalBounds().reduced (12).withTrimmedTop (22);
            first.setBounds (bounds.removeFromTop (bounds.getHeight() / 2).reduced (5));
            second.setBounds (bounds.reduced (5));
        }

    private:
        TracePanel& trace;
        TextButton first, second;
    };

    class HostedPlaceholder final : public Component
    {
    public:
        HostedPlaceholder()
        {
            setName ("Opaque hosted placeholder");
            setWantsKeyboardFocus (true);
            setHasFocusOutline (true);
        }

        void paint (Graphics& g) override
        {
            g.setColour (Colours::darkslategrey);
            g.fillRoundedRectangle (getLocalBounds().toFloat(), 8.0f);
            g.setColour (Colours::white);
            g.drawFittedText ("Opaque hosted surface\nJUCE focuses this leaf, not its contents",
                              getLocalBounds().reduced (10), Justification::centred, 2);
        }
    };

    class ModalContent final : public Component
    {
    public:
        explicit ModalContent (TracePanel& traceToUse)
            : trace (traceToUse)
        {
            setName ("Modal focus scope");
            setFocusNavigationMode (FocusNavigationMode::directional);
            setFocusContainerType (FocusContainerType::keyboardFocusContainer);

            configureLabel (message, "Focus remains inside this modal. Menu/Back closes one level.");
            message.setJustificationType (Justification::centred);
            addAndMakeVisible (message);

            first.onClick = [this] { trace.log ("modal: first action"); };
            second.onClick = [this] { trace.log ("modal: second action"); };

            for (auto* button : { &first, &second })
            {
                configureButton (*button, button->getButtonText());
                addAndMakeVisible (*button);
            }

            choice.addItemList ({ "Modal choice A", "Modal choice B" }, 1);
            choice.setSelectedId (1);
            choice.setName ("Modal choices");
            choice.setHasFocusOutline (true);
            addAndMakeVisible (choice);
        }

        void resized() override
        {
            auto bounds = getLocalBounds().reduced (20);
            message.setBounds (bounds.removeFromTop (50));
            first.setBounds (bounds.removeFromTop (55).reduced (20, 6));
            choice.setBounds (bounds.removeFromTop (55).reduced (20, 6));
            second.setBounds (bounds.removeFromTop (55).reduced (20, 6));
        }

    private:
        TracePanel& trace;
        Label message;
        TextButton first { "First action" }, second { "Second action" };
        ComboBox choice;
    };

    class ContainersPage final : public Component
    {
    public:
        explicit ContainersPage (TracePanel& traceToUse)
            : trace (traceToUse),
              north ("Inner north", trace),
              south ("Inner south", trace)
        {
            configureLabel (instructions,
                            "Nested keyboard-focus containers bubble at edges. The hosted surface "
                            "is an opaque JUCE leaf.");
            instructions.setJustificationType (Justification::centred);
            addAndMakeVisible (instructions);

            outer.setName ("Outer focus container");
            outer.setFocusContainerType (FocusContainerType::keyboardFocusContainer);
            outer.addAndMakeVisible (north);
            outer.addAndMakeVisible (south);
            addAndMakeVisible (outer);

            addAndMakeVisible (hosted);

            configureButton (openModal, "Open modal focus trap");
            openModal.onClick = [this] { launchModal(); };
            addAndMakeVisible (openModal);

            configureButton (exitButton, "Outside nested containers");
            exitButton.onClick = [this] { trace.log ("activated: outside nested containers"); };
            addAndMakeVisible (exitButton);
        }

        HostedPlaceholder& getHostedPlaceholder()    { return hosted; }

        void resized() override
        {
            auto bounds = getLocalBounds().reduced (20);
            instructions.setBounds (bounds.removeFromTop (50));

            auto left = bounds.removeFromLeft (bounds.getWidth() * 2 / 3).reduced (10);
            auto right = bounds.reduced (10);
            outer.setBounds (left.removeFromTop (left.getHeight() - 60));
            exitButton.setBounds (left.reduced (20, 8));
            hosted.setBounds (right.removeFromTop (right.getHeight() / 2).reduced (5));
            openModal.setBounds (right.reduced (5, 20));

            auto outerBounds = outer.getLocalBounds().reduced (12);
            north.setBounds (outerBounds.removeFromTop (outerBounds.getHeight() / 2).reduced (4));
            south.setBounds (outerBounds.reduced (4));
        }

    private:
        void launchModal()
        {
            DialogWindow::LaunchOptions options;
            options.content.setOwned (new ModalContent (trace));
            options.content->setSize (420, 270);
            options.dialogTitle = "Directional modal scope";
            options.dialogBackgroundColour = findColour (ResizableWindow::backgroundColourId);
            options.componentToCentreAround = this;
            options.escapeKeyTriggersCloseButton = true;
            options.useNativeTitleBar = false;
            options.resizable = false;
            options.launchAsync();
            trace.log ("modal: opened");
        }

        TracePanel& trace;
        Label instructions;
        Component outer;
        FocusGroup north, south;
        HostedPlaceholder hosted;
        TextButton openModal { "Open modal focus trap" }, exitButton { "Outside" };
    };

    //==============================================================================
    class ValuesPage final : public Component
    {
    public:
        explicit ValuesPage (TracePanel& traceToUse)
            : trace (traceToUse),
              tabs (TabbedButtonBar::TabsAtTop)
        {
            configureLabel (instructions,
                            "Select enters slider adjustment. The second slider restores its "
                            "snapshot on Back/Menu.");
            instructions.setJustificationType (Justification::centred);
            addAndMakeVisible (instructions);

            setupSlider (primarySlider, "Keep value on Back", false);
            setupSlider (cancellingSlider, "Cancel value on Back", true);

            configureLabel (primaryLabel, "Default: Back leaves adjustment and value unchanged");
            configureLabel (cancellingLabel, "Opt-in: Back restores the value at Select");
            addAndMakeVisible (primaryLabel);
            addAndMakeVisible (cancellingLabel);

            rangeSlider.setSliderStyle (Slider::TwoValueHorizontal);
            rangeSlider.setRange (0.0, 100.0, 1.0);
            rangeSlider.setMinAndMaxValues (25.0, 75.0);
            rangeSlider.setWantsKeyboardFocus (false);
            addAndMakeVisible (rangeSlider);
            configureLabel (rangeLabel, "Multi-thumb slider: ineligible by default");
            addAndMakeVisible (rangeLabel);

            combo.addItemList ({ "Popup item one", "Popup item two", "Popup item three" }, 1);
            combo.setSelectedId (1);
            combo.setName ("Popup menu example");
            combo.setHasFocusOutline (true);
            combo.onChange = [this] { trace.log ("combo: " + combo.getText()); };
            addAndMakeVisible (combo);

            for (auto* panel : { &alpha, &beta, &gamma })
            {
                panel->setJustificationType (Justification::centred);
                panel->setColour (Label::backgroundColourId, Colours::darkslategrey);
            }

            alpha.setText ("Alpha page", dontSendNotification);
            beta.setText ("Beta page", dontSendNotification);
            gamma.setText ("Gamma page", dontSendNotification);
            tabs.addTab ("Alpha", Colours::transparentBlack, &alpha, false);
            tabs.addTab ("Beta", Colours::transparentBlack, &beta, false);
            tabs.addTab ("Gamma", Colours::transparentBlack, &gamma, false);
            tabs.setTabBarDepth (35);
            addAndMakeVisible (tabs);

            for (int i = 0; i < tabs.getNumTabs(); ++i)
                if (auto* button = tabs.getTabbedButtonBar().getTabButton (i))
                    button->setHasFocusOutline (true);

            tabPolicy.addItemList ({ "Delayed", "Immediate", "Select only" }, 1);
            tabPolicy.setSelectedId (1);
            tabPolicy.setName ("Tab focus selection policy");
            tabPolicy.setHasFocusOutline (true);
            tabPolicy.onChange = [this]
            {
                using Mode = TabbedButtonBar::FocusSelectionMode;
                const std::array modes { Mode::delayed, Mode::immediate, Mode::selectOnly };
                tabs.getTabbedButtonBar().setFocusSelectionMode (
                    modes[(size_t) jlimit (0, 2, tabPolicy.getSelectedId() - 1)]);
                trace.log ("tabs: policy -> " + tabPolicy.getText());
            };
            addAndMakeVisible (tabPolicy);

            delay.setSliderStyle (Slider::LinearHorizontal);
            delay.setTextBoxStyle (Slider::TextBoxRight, false, 70, 22);
            delay.setRange (0.0, 2000.0, 100.0);
            delay.setValue (800.0);
            delay.setName ("Tab selection delay");
            delay.setHasFocusOutline (true);
            delay.onValueChange = [this]
            {
                tabs.getTabbedButtonBar().setFocusSelectionDelay (roundToInt (delay.getValue()));
            };
            addAndMakeVisible (delay);
        }

        Slider& getPrimarySlider()    { return primarySlider; }

        void resized() override
        {
            auto bounds = getLocalBounds().reduced (18);
            instructions.setBounds (bounds.removeFromTop (44));

            auto left = bounds.removeFromLeft (bounds.getWidth() / 2).reduced (8);
            auto right = bounds.reduced (8);

            primaryLabel.setBounds (left.removeFromTop (30));
            primarySlider.setBounds (left.removeFromTop (55));
            cancellingLabel.setBounds (left.removeFromTop (30));
            cancellingSlider.setBounds (left.removeFromTop (55));
            rangeLabel.setBounds (left.removeFromTop (30));
            rangeSlider.setBounds (left.removeFromTop (55));
            combo.setBounds (left.removeFromTop (45).reduced (15, 5));

            auto policyRow = right.removeFromTop (48);
            tabPolicy.setBounds (policyRow.removeFromLeft (policyRow.getWidth() / 2).reduced (4));
            delay.setBounds (policyRow.reduced (4));
            tabs.setBounds (right);
        }

    private:
        void setupSlider (Slider& slider, const String& name, bool cancels)
        {
            slider.setSliderStyle (Slider::LinearHorizontal);
            slider.setTextBoxStyle (Slider::TextBoxRight, false, 70, 22);
            slider.setRange (0.0, 100.0, 1.0);
            slider.setValue (50.0);
            slider.setBackButtonCancelsAdjustment (cancels);
            slider.setName (name);
            slider.setHasFocusOutline (true);
            slider.onValueChange = [this, &slider]
            {
                trace.log ("slider: " + slider.getName() + " -> "
                           + String (slider.getValue(), 0));
            };
            addAndMakeVisible (slider);
        }

        TracePanel& trace;
        Label instructions, primaryLabel, cancellingLabel, rangeLabel;
        Slider primarySlider, cancellingSlider, rangeSlider, delay;
        ComboBox combo, tabPolicy;
        TabbedComponent tabs;
        Label alpha, beta, gamma;
    };

    //==============================================================================
    class NavigationListModel final : public ListBoxModel
    {
    public:
        explicit NavigationListModel (TracePanel& traceToUse) : trace (traceToUse) {}

        int getNumRows() override    { return 12; }

        void paintListBoxItem (int row, Graphics& g, int width, int height, bool selected) override
        {
            if (selected)
                g.fillAll (Colours::cornflowerblue.withAlpha (0.45f));

            const auto focused = owner != nullptr && owner->getFocusNavigationRow() == row;
            g.setColour (focused ? Colours::yellow : owner->findColour (Label::textColourId));
            g.drawText ((focused ? "F  " : "   ") + String (row + 1)
                            + (isRowFocusNavigationEnabled (row) ? "" : "  (skipped)"),
                        8, 0, width - 16, height, Justification::centredLeft);
        }

        bool isRowFocusNavigationEnabled (int row) override    { return row != 2 && row != 8; }

        void focusNavigationRowChanged (int row) override
        {
            trace.log ("list: focus row -> " + String (row + 1));

            if (owner != nullptr)
                owner->repaint();
        }

        void returnKeyPressed (int row) override
        {
            trace.log ("list: activated row " + String (row + 1));
        }

        TracePanel& trace;
        ListBox* owner = nullptr;
    };

    class NavigationTableModel final : public TableListBoxModel
    {
    public:
        explicit NavigationTableModel (TracePanel& traceToUse) : trace (traceToUse) {}

        int getNumRows() override    { return 10; }

        void paintRowBackground (Graphics& g, int, int, int, bool selected) override
        {
            if (selected)
                g.fillAll (Colours::cornflowerblue.withAlpha (0.4f));
        }

        void paintCell (Graphics& g, int row, int column, int width, int height, bool) override
        {
            const auto focused = owner != nullptr && owner->getFocusNavigationRow() == row;
            g.setColour (focused ? Colours::yellow : owner->findColour (Label::textColourId));
            g.drawText (column == 1 ? "Row " + String (row + 1)
                                    : (isRowFocusNavigationEnabled (row) ? "eligible" : "skipped"),
                        5, 0, width - 10, height, Justification::centredLeft);
        }

        bool isRowFocusNavigationEnabled (int row) override    { return row != 4; }

        void focusNavigationRowChanged (int row) override
        {
            trace.log ("table: focus row -> " + String (row + 1));

            if (owner != nullptr)
                owner->repaint();
        }

        void returnKeyPressed (int row) override
        {
            trace.log ("table: activated row " + String (row + 1));
        }

        TracePanel& trace;
        TableListBox* owner = nullptr;
    };

    class NavigationTreeItem final : public TreeViewItem
    {
    public:
        NavigationTreeItem (TracePanel& traceToUse, String textToUse, bool canFocus = true)
            : trace (traceToUse), text (std::move (textToUse)), focusEnabled (canFocus) {}

        String getUniqueName() const override    { return text; }
        bool mightContainSubItems() override     { return getNumSubItems() != 0; }
        bool isFocusNavigationEnabled() const override    { return focusEnabled; }

        void focusNavigationChanged (bool isFocused) override
        {
            if (isFocused)
                trace.log ("tree: focus item -> " + text);

            repaintItem();
        }

        void itemOpennessChanged (bool) override    {}

        void paintItem (Graphics& g, int width, int height) override
        {
            g.setColour (hasFocusNavigation() ? Colours::yellow : Colours::white);
            g.drawText (text + (focusEnabled ? "" : " (skipped)"),
                        4, 0, width - 4, height, Justification::centredLeft);
        }

    private:
        TracePanel& trace;
        String text;
        bool focusEnabled = true;
    };

    class CollectionsPage final : public Component
    {
    public:
        explicit CollectionsPage (TracePanel& traceToUse)
            : trace (traceToUse),
              listModel (trace),
              list ("Navigation list", &listModel),
              tableModel (trace),
              table ("Navigation table", &tableModel),
              root (std::make_unique<NavigationTreeItem> (trace, "Root"))
        {
            configureLabel (instructions,
                            "Navigation focus is separate from selection. Select activates the "
                            "focused row; marked rows are skipped.");
            instructions.setJustificationType (Justification::centred);
            addAndMakeVisible (instructions);

            listModel.owner = &list;
            list.setMultipleSelectionEnabled (true);
            list.setHasFocusOutline (true);
            addAndMakeVisible (list);

            tableModel.owner = &table;
            table.getHeader().addColumn ("Item", 1, 110);
            table.getHeader().addColumn ("Policy", 2, 100);
            table.setMultipleSelectionEnabled (true);
            table.setHasFocusOutline (true);
            addAndMakeVisible (table);

            auto* folderA = new NavigationTreeItem (trace, "Folder A");
            folderA->addSubItem (new NavigationTreeItem (trace, "Leaf A1"));
            folderA->addSubItem (new NavigationTreeItem (trace, "Leaf A2", false));
            auto* folderB = new NavigationTreeItem (trace, "Folder B");
            folderB->addSubItem (new NavigationTreeItem (trace, "Leaf B1"));
            folderB->addSubItem (new NavigationTreeItem (trace, "Leaf B2"));
            root->addSubItem (folderA);
            root->addSubItem (folderB);
            root->setOpen (true);
            folderA->setOpen (true);
            folderB->setOpen (true);

            tree.setRootItem (root.get());
            tree.setRootItemVisible (false);
            tree.setMultiSelectEnabled (true);
            tree.setHasFocusOutline (true);
            addAndMakeVisible (tree);
        }

        ~CollectionsPage() override
        {
            tree.setRootItem (nullptr);
        }

        ListBox& getListBox()    { return list; }

        void resized() override
        {
            auto bounds = getLocalBounds().reduced (14);
            instructions.setBounds (bounds.removeFromTop (52));
            const auto width = bounds.getWidth() / 3;
            list.setBounds (bounds.removeFromLeft (width).reduced (5));
            table.setBounds (bounds.removeFromLeft (width).reduced (5));
            tree.setBounds (bounds.reduced (5));
        }

    private:
        TracePanel& trace;
        Label instructions;
        NavigationListModel listModel;
        ListBox list;
        NavigationTableModel tableModel;
        TableListBox table;
        std::unique_ptr<NavigationTreeItem> root;
        TreeView tree;
    };

    //==============================================================================
    class ScrollContent final : public Component
    {
    public:
        explicit ScrollContent (TracePanel& traceToUse) : trace (traceToUse)
        {
            for (int i = 0; i < 14; ++i)
            {
                auto button = std::make_unique<TextButton> ("Viewport item " + String (i + 1));
                configureButton (*button, button->getButtonText());
                button->onClick = [this, i] { trace.log ("viewport: activated item " + String (i + 1)); };
                addAndMakeVisible (*button);
                buttons.push_back (std::move (button));
            }

            setSize (300, 14 * 46);
        }

        void resized() override
        {
            auto bounds = getLocalBounds().reduced (5);

            for (auto& button : buttons)
                button->setBounds (bounds.removeFromTop (46).reduced (3));
        }

    private:
        TracePanel& trace;
        std::vector<std::unique_ptr<TextButton>> buttons;
    };

    class EditingPage final : public Component
    {
    public:
        explicit EditingPage (TracePanel& traceToUse)
            : trace (traceToUse),
              scrollContent (trace),
              codeEditor (document, &tokeniser)
        {
            configureLabel (instructions,
                            "Select enters editing. Remote directions stay with text input; "
                            "hardware arrows keep caret semantics. Back cancels/exits.");
            instructions.setJustificationType (Justification::centred);
            addAndMakeVisible (instructions);

            textEditor.setName ("Text editor");
            textEditor.setText ("Select to edit this text");
            textEditor.setHasFocusOutline (true);
            textEditor.onTextChange = [this] { trace.log ("text editor: changed"); };
            addAndMakeVisible (textEditor);

            editableLabel.setName ("Editable label");
            editableLabel.setText ("Select to edit this label", dontSendNotification);
            editableLabel.setEditable (true, true, false);
            editableLabel.setHasFocusOutline (true);
            addAndMakeVisible (editableLabel);

            document.replaceAllContent ("// Select to enter code editing\n"
                                        "int directionalFocus = 1;\n"
                                        "// Menu/Back cancels and exits\n");
            codeEditor.setName ("Code editor");
            codeEditor.setHasFocusOutline (true);
            addAndMakeVisible (codeEditor);

            viewport.setName ("Focus-revealing viewport");
            viewport.setViewedComponent (&scrollContent, false);
            viewport.setScrollBarsShown (true, false);
            viewport.setHasFocusOutline (true);
            addAndMakeVisible (viewport);
        }

        TextEditor& getTextEditor()    { return textEditor; }

        void resized() override
        {
            auto bounds = getLocalBounds().reduced (14);
            instructions.setBounds (bounds.removeFromTop (52));

            auto left = bounds.removeFromLeft (bounds.getWidth() / 2).reduced (5);
            textEditor.setBounds (left.removeFromTop (48).reduced (4));
            editableLabel.setBounds (left.removeFromTop (48).reduced (4));
            codeEditor.setBounds (left.reduced (4));
            viewport.setBounds (bounds.reduced (5));
            scrollContent.setSize (jmax (1, viewport.getMaximumVisibleWidth()), 14 * 46);
        }

    private:
        TracePanel& trace;
        Label instructions, editableLabel;
        TextEditor textEditor;
        ScrollContent scrollContent;
        Viewport viewport;
        CodeDocument document;
        CPlusPlusCodeTokeniser tokeniser;
        CodeEditorComponent codeEditor;
    };

    //==============================================================================
    class KeyConsumer final : public Component
    {
    public:
        enum class KeyKind
        {
            back,
            play
        };

        KeyConsumer (TracePanel& traceToUse, String nameToUse, KeyKind kindToUse)
            : trace (traceToUse), kind (kindToUse)
        {
            setName (std::move (nameToUse));
            setWantsKeyboardFocus (true);
            setHasFocusOutline (true);
        }

        bool keyPressed (const KeyPress& key) override
        {
            const auto wantedCode = kind == KeyKind::back ? KeyPress::menuKey : KeyPress::playKey;

            if (key.getKeyCode() != wantedCode)
                return false;

            trace.log ("input: " + getName() + " consumed " + key.getTextDescription());
            return true;
        }

        void paint (Graphics& g) override
        {
            g.setColour (Colours::darkslategrey);
            g.fillRoundedRectangle (getLocalBounds().toFloat(), 8.0f);
            g.setColour (Colours::white);
            g.drawFittedText (getName(), getLocalBounds().reduced (10), Justification::centred, 2);
        }

    private:
        TracePanel& trace;
        KeyKind kind;
    };

    class InputPage final : public Component
    {
    public:
        explicit InputPage (TracePanel& traceToUse)
            : trace (traceToUse),
              backConsumer (trace, "Back/Menu consumer", KeyConsumer::KeyKind::back),
              playConsumer (trace, "Play/Pause media consumer", KeyConsumer::KeyKind::play)
        {
            configureLabel (instructions,
                            "Select activates controls. Back/Menu unwinds one level. Play/Pause "
                            "remains a media command, not generic activation.");
            instructions.setJustificationType (Justification::centred);
            addAndMakeVisible (instructions);

            configureButton (selectButton, "Select activation button");
            selectButton.onClick = [this] { trace.log ("input: Select activated the button"); };
            addAndMakeVisible (selectButton);
            addAndMakeVisible (backConsumer);
            addAndMakeVisible (playConsumer);

            configureLabel (routeNote,
                            "The trace reports JUCE key events and final focus. Native focus-engine "
                            "routing is intentionally not exposed as a public API.");
            routeNote.setJustificationType (Justification::centred);
            addAndMakeVisible (routeNote);
        }

        void resized() override
        {
            auto bounds = getLocalBounds().reduced (24);
            instructions.setBounds (bounds.removeFromTop (60));
            routeNote.setBounds (bounds.removeFromBottom (70));

            auto row = bounds.reduced (20, 45);
            const auto width = row.getWidth() / 3;
            selectButton.setBounds (row.removeFromLeft (width).reduced (8));
            backConsumer.setBounds (row.removeFromLeft (width).reduced (8));
            playConsumer.setBounds (row.reduced (8));
        }

    private:
        TracePanel& trace;
        Label instructions, routeNote;
        TextButton selectButton { "Select activation button" };
        KeyConsumer backConsumer, playConsumer;
    };

    //==============================================================================
    void addScenario (const String& name, Component& page)
    {
        scenarios.addTab (name, Colours::transparentBlack, &page, false);

        if (auto* button = scenarios.getTabbedButtonBar().getTabButton (scenarios.getNumTabs() - 1))
            button->setHasFocusOutline (true);
    }

    bool keyPressed (const KeyPress& key, Component* originatingComponent) override
    {
        trace.log ("key route: " + key.getTextDescription() + " @ "
                   + FocusNavigationDemo::componentName (originatingComponent));
        return false;
    }

    using Component::keyPressed;

    void globalFocusChanged (Component* focusedComponent) override
    {
        if (focusedComponent == this || isParentOf (focusedComponent))
            trace.log ("focus owner: " + FocusNavigationDemo::componentName (focusedComponent)
                       + (focusedComponent->isDirectionalFocusNavigationEnabled()
                              ? " | directional" : " | legacy"));
    }

    TracePanel trace;
    GridPage gridPage;
    ContainersPage containersPage;
    ValuesPage valuesPage;
    CollectionsPage collectionsPage;
    EditingPage editingPage;
    InputPage inputPage;
    PolicyPage policyPage;
    TabbedComponent scenarios { TabbedButtonBar::TabsAtTop };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FocusNavigationDemo)
};
