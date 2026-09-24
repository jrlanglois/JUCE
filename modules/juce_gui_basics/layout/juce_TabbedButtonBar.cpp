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

TabBarButton::TabBarButton (const String& name, TabbedButtonBar& bar)
    : Button (name), owner (bar)
{
   #if JUCE_TVOS
    setWantsKeyboardFocus (true);
   #else
    setWantsKeyboardFocus (false);
   #endif
}

TabBarButton::~TabBarButton() {}

int TabBarButton::getIndex() const                      { return owner.indexOfTabButton (this); }
Colour TabBarButton::getTabBackgroundColour() const     { return owner.getTabBackgroundColour (getIndex()); }
bool TabBarButton::isFrontTab() const                   { return getToggleState(); }

void TabBarButton::paintButton (Graphics& g, const bool shouldDrawButtonAsHighlighted, const bool shouldDrawButtonAsDown)
{
    getLookAndFeel().drawTabButton (*this, g, shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);
}

void TabBarButton::clicked (const ModifierKeys& mods)
{
    if (mods.isPopupMenu())
        owner.popupMenuClickOnTab (getIndex(), getButtonText());
    else
        owner.setCurrentTabIndex (getIndex());
}

bool TabBarButton::hitTest (int mx, int my)
{
    auto area = getActiveArea();

    if (owner.isVertical())
    {
        if (isPositiveAndBelow (mx, getWidth())
             && my >= area.getY() + overlapPixels && my < area.getBottom() - overlapPixels)
            return true;
    }
    else
    {
        if (isPositiveAndBelow (my, getHeight())
             && mx >= area.getX() + overlapPixels && mx < area.getRight() - overlapPixels)
            return true;
    }

    Path p;
    getLookAndFeel().createTabButtonShape (*this, p, false, false);

    return p.contains ((float) (mx - area.getX()),
                       (float) (my - area.getY()));
}

int TabBarButton::getBestTabLength (const int depth)
{
    return getLookAndFeel().getTabButtonBestWidth (*this, depth);
}

void TabBarButton::calcAreas (Rectangle<int>& extraComp, Rectangle<int>& textArea) const
{
    auto& lf = getLookAndFeel();
    textArea = getActiveArea();

    auto depth = owner.isVertical() ? textArea.getWidth() : textArea.getHeight();
    auto overlap = lf.getTabButtonOverlap (depth);

    if (overlap > 0)
    {
        if (owner.isVertical())
            textArea.reduce (0, overlap);
        else
            textArea.reduce (overlap, 0);
    }

    if (extraComponent != nullptr)
    {
        extraComp = lf.getTabButtonExtraComponentBounds (*this, textArea, *extraComponent);

        auto orientation = owner.getOrientation();

        if (orientation == TabbedButtonBar::TabsAtLeft || orientation == TabbedButtonBar::TabsAtRight)
        {
            if (extraComp.getCentreY() > textArea.getCentreY())
                textArea.setBottom (jmin (textArea.getBottom(), extraComp.getY()));
            else
                textArea.setTop (jmax (textArea.getY(), extraComp.getBottom()));
        }
        else
        {
            if (extraComp.getCentreX() > textArea.getCentreX())
                textArea.setRight (jmin (textArea.getRight(), extraComp.getX()));
            else
                textArea.setLeft (jmax (textArea.getX(), extraComp.getRight()));
        }
    }
}

Rectangle<int> TabBarButton::getTextArea() const
{
    Rectangle<int> extraComp, textArea;
    calcAreas (extraComp, textArea);
    return textArea;
}

Rectangle<int> TabBarButton::getActiveArea() const
{
    auto r = getLocalBounds();
    auto spaceAroundImage = getLookAndFeel().getTabButtonSpaceAroundImage();
    auto orientation = owner.getOrientation();

    if (orientation != TabbedButtonBar::TabsAtLeft)      r.removeFromRight  (spaceAroundImage);
    if (orientation != TabbedButtonBar::TabsAtRight)     r.removeFromLeft   (spaceAroundImage);
    if (orientation != TabbedButtonBar::TabsAtBottom)    r.removeFromTop    (spaceAroundImage);
    if (orientation != TabbedButtonBar::TabsAtTop)       r.removeFromBottom (spaceAroundImage);

    return r;
}

void TabBarButton::setExtraComponent (Component* comp, ExtraComponentPlacement placement)
{
    jassert (extraCompPlacement == beforeText || extraCompPlacement == afterText);
    extraCompPlacement = placement;
    extraComponent.reset (comp);
    addAndMakeVisible (extraComponent.get());
    resized();
}

void TabBarButton::childBoundsChanged (Component* c)
{
    if (c == extraComponent.get())
    {
        owner.resized();
        resized();
    }
}

void TabBarButton::resized()
{
    if (extraComponent != nullptr)
    {
        Rectangle<int> extraComp, textArea;
        calcAreas (extraComp, textArea);

        if (! extraComp.isEmpty())
            extraComponent->setBounds (extraComp);
    }
}

void TabBarButton::focusGained (FocusChangeType cause)
{
    Button::focusGained (cause);
    owner.tabButtonFocusChanged (*this, true);
}

void TabBarButton::focusLost (FocusChangeType cause)
{
    Button::focusLost (cause);
    owner.tabButtonFocusChanged (*this, false);
}

//==============================================================================
class TabbedButtonBar::BehindFrontTabComp final : public Component
{
public:
    BehindFrontTabComp (TabbedButtonBar& tb)  : owner (tb)
    {
        setInterceptsMouseClicks (false, false);
    }

    void paint (Graphics& g) override
    {
        getLookAndFeel().drawTabAreaBehindFrontButton (owner, g, getWidth(), getHeight());
    }

    void enablementChanged() override
    {
        repaint();
    }

    TabbedButtonBar& owner;

    JUCE_DECLARE_NON_COPYABLE (BehindFrontTabComp)
};

class TabbedButtonBar::FocusSelectionController final : private Timer,
                                                        private FocusChangeListener,
                                                        private ComponentListener
{
public:
    explicit FocusSelectionController (TabbedButtonBar& ownerIn)
        : owner (ownerIn)
    {
        Desktop::getInstance().addFocusChangeListener (this);
        owner.addComponentListener (this);
    }

    ~FocusSelectionController() override
    {
        cancel();
        Desktop::getInstance().removeFocusChangeListener (this);
        owner.removeComponentListener (this);

        for (auto* tab : owner.tabs)
            tab->button->removeComponentListener (this);
    }

    void watch (TabBarButton& button)
    {
        button.addComponentListener (this);
    }

    void cancel()
    {
        stopTimer();
        pendingSelection = nullptr;
    }

    void applyCurrentFocus()
    {
        applyFocus (Component::getCurrentlyFocusedComponent(), true);
    }

    void applyFocus (Component* focusedComponent, bool force = false)
    {
        if (! force && lastHandledFocus.get() == focusedComponent)
            return;

        lastHandledFocus = focusedComponent;
        cancel();

        auto* button = findTabButton (focusedComponent);

        if (button == nullptr || ! button->isEnabled())
            return;

        switch (owner.focusSelectionMode)
        {
            case FocusSelectionMode::delayed:
                if (owner.focusSelectionDelay > 0)
                {
                    pendingSelection = button;
                    startTimer (owner.focusSelectionDelay);
                    return;
                }

                select (*button);
                return;

            case FocusSelectionMode::immediate:
                select (*button);
                return;

            case FocusSelectionMode::selectOnly:
                return;
        }

        jassertfalse;
    }

private:
    void globalFocusChanged (Component* focusedComponent) override
    {
        applyFocus (focusedComponent);
    }

    void timerCallback() override
    {
        const auto pending = pendingSelection;
        cancel();
        auto* button = findTabButton (pending.get());

        if (button != nullptr
            && button->isShowing()
            && button->isEnabled()
            && button->hasKeyboardFocus (false)
            && owner.focusSelectionMode == FocusSelectionMode::delayed)
        {
            select (*button);
        }
    }

    void select (TabBarButton& button)
    {
        if (const auto index = owner.indexOfTabButton (&button); index >= 0)
            owner.setCurrentTabIndex (index);
    }

    void componentVisibilityChanged (Component& component) override
    {
        cancelIfPendingSelectionWasInvalidated (component);
    }

    void componentEnablementChanged (Component& component) override
    {
        cancelIfPendingSelectionWasInvalidated (component);
    }

    void componentBeingDeleted (Component& component) override
    {
        cancelIfPendingSelectionWasInvalidated (component);
    }

    void cancelIfPendingSelectionWasInvalidated (Component& component)
    {
        if (lastHandledFocus.get() == &component)
            lastHandledFocus = nullptr;

        if (&component == &owner || pendingSelection.get() == &component)
            cancel();
    }

    TabBarButton* findTabButton (const Component* component) const
    {
        for (auto* tab : owner.tabs)
            if (tab->button.get() == component)
                return tab->button.get();

        return nullptr;
    }

    TabbedButtonBar& owner;
    WeakReference<Component> pendingSelection;
    WeakReference<Component> lastHandledFocus;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FocusSelectionController)
};


//==============================================================================
TabbedButtonBar::TabbedButtonBar (Orientation orientationToUse)
    : orientation (orientationToUse)
{
    focusSelectionController = std::make_unique<FocusSelectionController> (*this);
    setInterceptsMouseClicks (false, true);
    behindFrontTab.reset (new BehindFrontTabComp (*this));
    addAndMakeVisible (behindFrontTab.get());
    setFocusContainerType (FocusContainerType::keyboardFocusContainer);
}

TabbedButtonBar::~TabbedButtonBar()
{
    focusSelectionController.reset();
    tabs.clear();
    extraTabsButton.reset();
}

//==============================================================================
void TabbedButtonBar::setOrientation (const Orientation newOrientation)
{
    orientation = newOrientation;

    for (auto* child : getChildren())
        child->resized();

    resized();
}

void TabbedButtonBar::setFocusSelectionMode (FocusSelectionMode newMode)
{
    if (focusSelectionMode != newMode)
    {
        focusSelectionMode = newMode;
        focusSelectionController->applyCurrentFocus();
    }
}

void TabbedButtonBar::setFocusSelectionDelay (int delayMilliseconds)
{
    jassert (delayMilliseconds >= 0);
    delayMilliseconds = jmax (0, delayMilliseconds);

    if (focusSelectionDelay != delayMilliseconds)
    {
        focusSelectionDelay = delayMilliseconds;

        if (focusSelectionMode == FocusSelectionMode::delayed)
            focusSelectionController->applyCurrentFocus();
    }
}

void TabbedButtonBar::tabButtonFocusChanged (TabBarButton& button, bool isNowFocused)
{
    if (focusSelectionController != nullptr)
        focusSelectionController->applyFocus (isNowFocused ? &button : nullptr);
}

TabBarButton* TabbedButtonBar::createTabButton (const String& name, const int /*index*/)
{
    return new TabBarButton (name, *this);
}

void TabbedButtonBar::setMinimumTabScaleFactor (double newMinimumScale)
{
    minimumScale = newMinimumScale;
    resized();
}

//==============================================================================
void TabbedButtonBar::clearTabs()
{
    tabs.clear();
    extraTabsButton.reset();
    setCurrentTabIndex (-1);
}

void TabbedButtonBar::addTab (const String& tabName,
                              Colour tabBackgroundColour,
                              int insertIndex)
{
    jassert (tabName.isNotEmpty()); // you have to give them all a name

    if (tabName.isNotEmpty())
    {
        if (! isPositiveAndBelow (insertIndex, tabs.size()))
            insertIndex = tabs.size();

        auto* currentTab = tabs[currentTabIndex];

        auto* newTab = new TabInfo();
        newTab->name = tabName;
        newTab->colour = tabBackgroundColour;
        newTab->button.reset (createTabButton (tabName, insertIndex));
        jassert (newTab->button != nullptr);

        tabs.insert (insertIndex, newTab);
        currentTabIndex = tabs.indexOf (currentTab);
        focusSelectionController->watch (*newTab->button);
        addAndMakeVisible (newTab->button.get(), insertIndex);

        resized();

        if (currentTabIndex < 0)
            setCurrentTabIndex (0);
    }
}

void TabbedButtonBar::setTabName (int tabIndex, const String& newName)
{
    if (auto* tab = tabs[tabIndex])
    {
        if (tab->name != newName)
        {
            tab->name = newName;
            tab->button->setButtonText (newName);
            resized();
        }
    }
}

void TabbedButtonBar::removeTab (const int indexToRemove, const bool animate)
{
    if (isPositiveAndBelow (indexToRemove, tabs.size()))
    {
        auto oldSelectedIndex = currentTabIndex;

        if (indexToRemove == currentTabIndex)
            oldSelectedIndex = -1;
        else if (indexToRemove < oldSelectedIndex)
            --oldSelectedIndex;

        tabs.remove (indexToRemove);

        setCurrentTabIndex (oldSelectedIndex);
        updateTabPositions (animate);
    }
}

void TabbedButtonBar::moveTab (const int currentIndex, const int newIndex, const bool animate)
{
    auto* currentTab = tabs[currentTabIndex];
    tabs.move (currentIndex, newIndex);
    currentTabIndex = tabs.indexOf (currentTab);
    updateTabPositions (animate);
}

int TabbedButtonBar::getNumTabs() const
{
    return tabs.size();
}

String TabbedButtonBar::getCurrentTabName() const
{
    if (auto* tab = tabs [currentTabIndex])
        return tab->name;

    return {};
}

StringArray TabbedButtonBar::getTabNames() const
{
    StringArray names;

    for (auto* t : tabs)
        names.add (t->name);

    return names;
}

void TabbedButtonBar::setCurrentTabIndex (int newIndex, bool shouldSendChangeMessage)
{
    if (focusSelectionController != nullptr)
        focusSelectionController->cancel();

    if (currentTabIndex != newIndex)
    {
        if (! isPositiveAndBelow (newIndex, tabs.size()))
            newIndex = -1;

        currentTabIndex = newIndex;

        for (int i = 0; i < tabs.size(); ++i)
            tabs.getUnchecked (i)->button->setToggleState (i == newIndex, dontSendNotification);

        resized();

        if (shouldSendChangeMessage)
            sendChangeMessage();

        currentTabChanged (newIndex, getCurrentTabName());
    }
}

TabBarButton* TabbedButtonBar::getTabButton (const int index) const
{
    if (auto* tab = tabs[index])
        return static_cast<TabBarButton*> (tab->button.get());

    return nullptr;
}

int TabbedButtonBar::indexOfTabButton (const TabBarButton* button) const
{
    for (int i = tabs.size(); --i >= 0;)
        if (tabs.getUnchecked (i)->button.get() == button)
            return i;

    return -1;
}

Rectangle<int> TabbedButtonBar::getTargetBounds (TabBarButton* button) const
{
    if (button == nullptr || indexOfTabButton (button) == -1)
        return {};

    auto& animator = Desktop::getInstance().getAnimator();

    return animator.isAnimating (button) ? animator.getComponentDestination (button)
                                         : button->getBounds();
}

void TabbedButtonBar::lookAndFeelChanged()
{
    extraTabsButton.reset();
    resized();
}

void TabbedButtonBar::paint (Graphics& g)
{
    getLookAndFeel().drawTabbedButtonBarBackground (*this, g);
}

void TabbedButtonBar::resized()
{
    updateTabPositions (false);
}

//==============================================================================
void TabbedButtonBar::updateTabPositions (bool animate)
{
    auto& lf = getLookAndFeel();

    auto depth = getWidth();
    auto length = getHeight();

    if (! isVertical())
        std::swap (depth, length);

    auto overlap = lf.getTabButtonOverlap (depth) + lf.getTabButtonSpaceAroundImage() * 2;

    auto totalLength = jmax (0, overlap);
    auto numVisibleButtons = tabs.size();

    for (int i = 0; i < tabs.size(); ++i)
    {
        auto* tb = tabs.getUnchecked (i)->button.get();

        totalLength += tb->getBestTabLength (depth) - overlap;
        tb->overlapPixels = jmax (0, overlap / 2);
    }

    double scale = 1.0;

    if (totalLength > length)
        scale = jmax (minimumScale, length / (double) totalLength);

    const bool isTooBig = (int) (totalLength * scale) > length;
    int tabsButtonPos = 0;

    if (isTooBig)
    {
        if (extraTabsButton == nullptr)
        {
            extraTabsButton.reset (lf.createTabBarExtrasButton());
            addAndMakeVisible (extraTabsButton.get());
            extraTabsButton->setAlwaysOnTop (true);
            extraTabsButton->setTriggeredOnMouseDown (true);
            extraTabsButton->onClick = [this] { showExtraItemsMenu(); };
        }

        auto buttonSize = jmin (proportionOfWidth (0.7f), proportionOfHeight (0.7f));
        extraTabsButton->setSize (buttonSize, buttonSize);

        if (isVertical())
        {
            tabsButtonPos = getHeight() - buttonSize / 2 - 1;
            extraTabsButton->setCentrePosition (getWidth() / 2, tabsButtonPos);
        }
        else
        {
            tabsButtonPos = getWidth() - buttonSize / 2 - 1;
            extraTabsButton->setCentrePosition (tabsButtonPos, getHeight() / 2);
        }

        totalLength = 0;

        for (int i = 0; i < tabs.size(); ++i)
        {
            auto* tb = tabs.getUnchecked (i)->button.get();
            auto newLength = totalLength + tb->getBestTabLength (depth);

            if (i > 0 && newLength * minimumScale > tabsButtonPos)
            {
                totalLength += overlap;
                break;
            }

            numVisibleButtons = i + 1;
            totalLength = newLength - overlap;
        }

        scale = jmax (minimumScale, tabsButtonPos / (double) totalLength);
    }
    else
    {
        extraTabsButton.reset();
    }

    int pos = 0;

    TabBarButton* frontTab = nullptr;
    auto& animator = Desktop::getInstance().getAnimator();

    for (int i = 0; i < tabs.size(); ++i)
    {
        if (auto* tb = getTabButton (i))
        {
            auto bestLength = roundToInt (scale * tb->getBestTabLength (depth));

            if (i < numVisibleButtons)
            {
                auto newBounds = isVertical() ? Rectangle<int> (0, pos, getWidth(), bestLength)
                                              : Rectangle<int> (pos, 0, bestLength, getHeight());

                if (animate)
                {
                    animator.animateComponent (tb, newBounds, 1.0f, 200, false, 3.0, 0.0);
                }
                else
                {
                    animator.cancelAnimation (tb, false);
                    tb->setBounds (newBounds);
                }

                tb->toBack();

                if (i == currentTabIndex)
                    frontTab = tb;

                tb->setVisible (true);
            }
            else
            {
                tb->setVisible (false);
            }

            pos += bestLength - overlap;
        }
    }

    behindFrontTab->setBounds (getLocalBounds());

    if (frontTab != nullptr)
    {
        frontTab->toFront (false);
        behindFrontTab->toBehind (frontTab);
    }
}

//==============================================================================
Colour TabbedButtonBar::getTabBackgroundColour (int tabIndex)
{
    if (auto* tab = tabs[tabIndex])
        return tab->colour;

    return Colours::transparentBlack;
}

void TabbedButtonBar::setTabBackgroundColour (int tabIndex, Colour newColour)
{
    if (auto* tab = tabs [tabIndex])
    {
        if (tab->colour != newColour)
        {
            tab->colour = newColour;
            repaint();
        }
    }
}

void TabbedButtonBar::showExtraItemsMenu()
{
    PopupMenu m;

    for (int i = 0; i < tabs.size(); ++i)
    {
        auto* tab = tabs.getUnchecked (i);

        if (! tab->button->isVisible())
            m.addItem (PopupMenu::Item (tab->name)
                         .setTicked (i == currentTabIndex)
                         .setAction ([this, i] { setCurrentTabIndex (i); }));
    }

    m.showMenuAsync (PopupMenu::Options()
                        .withDeletionCheck (*this)
                        .withTargetComponent (extraTabsButton.get()));
}

//==============================================================================
void TabbedButtonBar::currentTabChanged (int, const String&) {}
void TabbedButtonBar::popupMenuClickOnTab (int, const String&) {}

//==============================================================================
std::unique_ptr<AccessibilityHandler> TabbedButtonBar::createAccessibilityHandler()
{
    return std::make_unique<AccessibilityHandler> (*this, AccessibilityRole::group);
}

#if JUCE_UNIT_TESTS

struct TabbedButtonBarFocusTests final : UnitTest
{
    TabbedButtonBarFocusTests()
        : UnitTest ("TabbedButtonBar focus navigation", UnitTestCategories::gui)
    {}

    void runTest() override
    {
        ScopedJuceInitialiser_GUI libraryInitialiser;
        TabbedButtonBar bar (TabbedButtonBar::TabsAtTop);
        bar.setBounds (0, 0, 400, 40);
        bar.addTab ("First", Colours::red, -1);
        bar.addTab ("Second", Colours::blue, -1);

        beginTest ("The platform defaults preserve existing non-tvOS behaviour");
        expectEquals (bar.getFocusSelectionDelay(), 500);

       #if JUCE_TVOS
        expect (bar.getFocusSelectionMode() == TabbedButtonBar::FocusSelectionMode::delayed);
        expect (bar.getTabButton (0)->getWantsKeyboardFocus());
       #else
        expect (bar.getFocusSelectionMode() == TabbedButtonBar::FocusSelectionMode::selectOnly);
        expect (! bar.getTabButton (0)->getWantsKeyboardFocus());
       #endif

        beginTest ("The focus-selection policy and delay are configurable");
        bar.setFocusSelectionMode (TabbedButtonBar::FocusSelectionMode::immediate);
        bar.setFocusSelectionDelay (275);
        expect (bar.getFocusSelectionMode() == TabbedButtonBar::FocusSelectionMode::immediate);
        expectEquals (bar.getFocusSelectionDelay(), 275);

        beginTest ("Immediate and zero-delay policies select synchronously");
        auto* secondTab = bar.getTabButton (1);
        bar.setCurrentTabIndex (0);
        secondTab->focusGained (Component::focusChangedDirectly);
        expectEquals (bar.getCurrentTabIndex(), 1);
        secondTab->focusLost (Component::focusChangedDirectly);

        bar.setCurrentTabIndex (0);
        bar.setFocusSelectionDelay (0);
        bar.setFocusSelectionMode (TabbedButtonBar::FocusSelectionMode::delayed);
        secondTab->focusGained (Component::focusChangedDirectly);
        expectEquals (bar.getCurrentTabIndex(), 1);
        secondTab->focusLost (Component::focusChangedDirectly);

        beginTest ("Select-only focus does not change the current tab");
        bar.setCurrentTabIndex (0);
        bar.setFocusSelectionMode (TabbedButtonBar::FocusSelectionMode::selectOnly);
        secondTab->focusGained (Component::focusChangedDirectly);
        expectEquals (bar.getCurrentTabIndex(), 0);
        secondTab->focusLost (Component::focusChangedDirectly);

        beginTest ("Tab buttons offer directional navigation to the dispatcher");
        expect (bar.getTabButton (0)->handleFocusNavigation (FocusNavigationDirection::right)
                == FocusNavigationResult::unhandled);

        beginTest ("Activating a tab selects it immediately");
        bar.setCurrentTabIndex (0);
        secondTab->clicked (ModifierKeys {});
        expectEquals (bar.getCurrentTabIndex(), 1);
    }
};

static TabbedButtonBarFocusTests tabbedButtonBarFocusTests;

#endif

} // namespace juce
