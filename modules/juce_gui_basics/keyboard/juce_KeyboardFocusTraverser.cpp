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

//==============================================================================
namespace KeyboardFocusTraverserHelpers
{
    static bool isKeyboardFocusable (const Component* comp, const Component* container)
    {
        return comp->getWantsKeyboardFocus() && container->isParentOf (comp);
    }

    static Component* traverse (const Component* current, const Component* container,
                                detail::FocusHelpers::NavigationDirection direction)
    {
        if (auto* comp = detail::FocusHelpers::navigateFocus (current, container, direction,
                                                              &Component::isKeyboardFocusContainer,
                                                              FocusTraverser::SkipDisabledComponents::yes))
        {
            if (isKeyboardFocusable (comp, container))
                return comp;

            return traverse (comp, container, direction);
        }

        return nullptr;
    }
}

Component* KeyboardFocusTraverser::getNextComponent (Component* current)
{
    return KeyboardFocusTraverserHelpers::traverse (current, current->findKeyboardFocusContainer(),
                                                    detail::FocusHelpers::NavigationDirection::forwards);
}

Component* KeyboardFocusTraverser::getPreviousComponent (Component* current)
{
    return KeyboardFocusTraverserHelpers::traverse (current, current->findKeyboardFocusContainer(),
                                                    detail::FocusHelpers::NavigationDirection::backwards);
}

Component* KeyboardFocusTraverser::getComponentInDirection (Component* current,
                                                            FocusNavigationDirection direction)
{
    if (current == nullptr)
        return nullptr;

    auto* container = current->findKeyboardFocusContainer();

    if (container == nullptr)
        return nullptr;

    auto candidates = getAllComponents (container);
    candidates.erase (std::remove (candidates.begin(), candidates.end(), current), candidates.end());

    const auto getScreenBounds = [] (const Component* component)
    {
        return component->localAreaToGlobal (component->getLocalBounds().toFloat());
    };

    const auto candidate = detail::findDirectionalFocusCandidate (getScreenBounds (current),
                                                                  candidates.begin(),
                                                                  candidates.end(),
                                                                  direction,
                                                                  getScreenBounds);

    return candidate != candidates.end() ? *candidate : nullptr;
}

Component* KeyboardFocusTraverser::getDefaultComponent (Component* parentComponent)
{
    for (auto* comp : getAllComponents (parentComponent))
        if (KeyboardFocusTraverserHelpers::isKeyboardFocusable (comp, parentComponent))
            return comp;

    return nullptr;
}

std::vector<Component*> KeyboardFocusTraverser::getAllComponents (Component* parentComponent)
{
    std::vector<Component*> components;
    detail::FocusHelpers::findAllComponents (parentComponent,
                                             components,
                                             &Component::isKeyboardFocusContainer,
                                             FocusTraverser::SkipDisabledComponents::yes);

    auto removePredicate = [parentComponent] (const Component* comp)
    {
        return ! KeyboardFocusTraverserHelpers::isKeyboardFocusable (comp, parentComponent);
    };

    components.erase (std::remove_if (std::begin (components), std::end (components), std::move (removePredicate)),
                      std::end (components));

    return components;
}


//==============================================================================
//==============================================================================
#if JUCE_UNIT_TESTS

struct KeyboardFocusTraverserTests final : public UnitTest
{
    KeyboardFocusTraverserTests()
        : UnitTest ("KeyboardFocusTraverser", UnitTestCategories::gui)
    {}

    void runTest() override
    {
        ScopedJuceInitialiser_GUI libraryInitialiser;
        const auto mml = std::make_unique<MessageManagerLock>();

        beginTest ("No child wants keyboard focus");
        {
            const auto parent = std::make_unique<TestComponent>();

            expect (traverser.getDefaultComponent (parent.get()) == nullptr);
            expect (traverser.getAllComponents (parent.get()).empty());
        }

        beginTest ("Single child wants keyboard focus");
        {
            const auto parent = std::make_unique<TestComponent>();

            parent->children[5].setWantsKeyboardFocus (true);

            auto* defaultComponent = traverser.getDefaultComponent (parent.get());

            expect (defaultComponent == &parent->children[5]);
            expect (defaultComponent->getWantsKeyboardFocus());

            expect (traverser.getNextComponent (defaultComponent) == nullptr);
            expect (traverser.getPreviousComponent (defaultComponent) == nullptr);
            expect (traverser.getAllComponents (parent.get()).size() == 1);
        }

        beginTest ("Multiple children want keyboard focus");
        {
            const auto parent = std::make_unique<TestComponent>();

            Component* focusChildren[]
            {
                &parent->children[1],
                &parent->children[9],
                &parent->children[3],
                &parent->children[5],
                &parent->children[8],
                &parent->children[0]
            };

            for (auto* focusChild : focusChildren)
                focusChild->setWantsKeyboardFocus (true);

            auto allComponents = traverser.getAllComponents (parent.get());

            for (auto* focusChild : focusChildren)
                expect (std::find (allComponents.cbegin(), allComponents.cend(), focusChild) != allComponents.cend());

            auto* componentToTest = traverser.getDefaultComponent (parent.get());

            for (;;)
            {
                expect (componentToTest->getWantsKeyboardFocus());
                expect (std::find (std::begin (focusChildren), std::end (focusChildren), componentToTest) != std::end (focusChildren));

                componentToTest = traverser.getNextComponent (componentToTest);

                if (componentToTest == nullptr)
                    break;
            }

            int focusOrder = 1;
            for (auto* focusChild : focusChildren)
                focusChild->setExplicitFocusOrder (focusOrder++);

            componentToTest = traverser.getDefaultComponent (parent.get());

            for (auto* focusChild : focusChildren)
            {
                expect (componentToTest == focusChild);
                expect (componentToTest->getWantsKeyboardFocus());

                componentToTest = traverser.getNextComponent (componentToTest);
            }
        }

        beginTest ("Single nested child wants keyboard focus");
        {
            const auto parent = std::make_unique<TestComponent>();
            const auto grandparent = std::make_unique<Component>();

            grandparent->addAndMakeVisible (parent.get());

            auto& focusChild = parent->children[5];

            focusChild.setWantsKeyboardFocus (true);

            expect (traverser.getDefaultComponent (grandparent.get()) == &focusChild);
            expect (traverser.getDefaultComponent (parent.get()) == &focusChild);
            expect (traverser.getNextComponent (&focusChild) == nullptr);
            expect (traverser.getPreviousComponent (&focusChild) == nullptr);
            expect (traverser.getAllComponents (parent.get()).size() == 1);
        }

        beginTest ("Multiple nested children want keyboard focus");
        {
            const auto parent = std::make_unique<TestComponent>();
            const auto grandparent = std::make_unique<Component>();

            grandparent->addAndMakeVisible (parent.get());

            Component* focusChildren[]
            {
                &parent->children[1],
                &parent->children[4],
                &parent->children[5]
            };

            for (auto* focusChild : focusChildren)
                focusChild->setWantsKeyboardFocus (true);

            auto allComponents = traverser.getAllComponents (parent.get());

            expect (std::equal (allComponents.cbegin(), allComponents.cend(), focusChildren,
                                [] (const Component* c1, const Component* c2) { return c1 == c2; }));

            const auto front = *focusChildren;
            const auto back  = *std::prev (std::end (focusChildren));

            expect (traverser.getDefaultComponent (grandparent.get()) == front);
            expect (traverser.getDefaultComponent (parent.get()) == front);
            expect (traverser.getNextComponent (front) == *std::next (std::begin (focusChildren)));
            expect (traverser.getPreviousComponent (back) == *std::prev (std::end (focusChildren), 2));

            std::array<std::unique_ptr<Component>, 3> otherParents;

            for (auto& p : otherParents)
            {
                p = std::make_unique<Component>();
                grandparent->addAndMakeVisible (p.get());
                p->setWantsKeyboardFocus (true);
            }

            expect (traverser.getDefaultComponent (grandparent.get()) == front);
            expect (traverser.getDefaultComponent (parent.get()) == front);
            expect (traverser.getNextComponent (back) == otherParents.front().get());
            expect (traverser.getNextComponent (otherParents.back().get()) == nullptr);
            expect (traverser.getAllComponents (grandparent.get()).size() == numElementsInArray (focusChildren) + otherParents.size());
            expect (traverser.getAllComponents (parent.get()).size() == (size_t) numElementsInArray (focusChildren));

            for (auto* focusChild : focusChildren)
                focusChild->setWantsKeyboardFocus (false);

            expect (traverser.getDefaultComponent (grandparent.get()) == otherParents.front().get());
            expect (traverser.getDefaultComponent (parent.get()) == nullptr);
            expect (traverser.getAllComponents (grandparent.get()).size() == otherParents.size());
            expect (traverser.getAllComponents (parent.get()).empty());
        }

        runDirectionalTests();
    }

private:
    static void addFocusableChild (Component& parent, Component& child, Rectangle<int> bounds)
    {
        parent.addAndMakeVisible (child);
        child.setBounds (bounds);
        child.setWantsKeyboardFocus (true);
    }

    void runDirectionalTests()
    {
        beginTest ("Directional traversal in a 3-by-3 grid does not wrap");
        {
            Component parent;
            std::array<Component, 9> children;

            for (size_t i = 0; i < children.size(); ++i)
            {
                const auto column = static_cast<int> (i % 3);
                const auto row = static_cast<int> (i / 3);
                addFocusableChild (parent, children[i], { column * 40, row * 40, 20, 20 });
            }

            expect (traverser.getComponentInDirection (&children[4], FocusNavigationDirection::left) == &children[3]);
            expect (traverser.getComponentInDirection (&children[4], FocusNavigationDirection::right) == &children[5]);
            expect (traverser.getComponentInDirection (&children[4], FocusNavigationDirection::up) == &children[1]);
            expect (traverser.getComponentInDirection (&children[4], FocusNavigationDirection::down) == &children[7]);
            expect (traverser.getComponentInDirection (&children[3], FocusNavigationDirection::left) == nullptr);
            expect (traverser.getComponentInDirection (&children[5], FocusNavigationDirection::right) == nullptr);
            expect (traverser.getComponentInDirection (&children[1], FocusNavigationDirection::up) == nullptr);
            expect (traverser.getComponentInDirection (&children[7], FocusNavigationDirection::down) == nullptr);
        }

        beginTest ("A beam candidate wins before a nearer diagonal candidate");
        {
            Component parent, source, beamCandidate, diagonalCandidate;
            addFocusableChild (parent, source,            {   0,  0, 10, 10 });
            addFocusableChild (parent, beamCandidate,     { 100,  0, 10, 10 });
            addFocusableChild (parent, diagonalCandidate, {  20, 20, 10, 10 });

            expect (traverser.getComponentInDirection (&source, FocusNavigationDirection::right) == &beamCandidate);

            beamCandidate.setWantsKeyboardFocus (false);
            expect (traverser.getComponentInDirection (&source, FocusNavigationDirection::right) == &diagonalCandidate);
        }

        beginTest ("No-beam fallback minimises major-axis distance before minor-axis distance");
        {
            Component parent, source, nearerMajorAxis, nearerMinorAxis;
            addFocusableChild (parent, source,           {  0,  0, 10, 10 });
            addFocusableChild (parent, nearerMajorAxis,  { 20, 40, 10, 10 });
            addFocusableChild (parent, nearerMinorAxis,  { 30, 12, 10, 10 });

            expect (traverser.getComponentInDirection (&source, FocusNavigationDirection::right) == &nearerMajorAxis);
        }

        beginTest ("Directional traversal uses transformed screen bounds");
        {
            Component parent, source, transformedCandidate;
            addFocusableChild (parent, source,               { 100, 0, 10, 10 });
            addFocusableChild (parent, transformedCandidate, { 200, 0, 10, 10 });
            transformedCandidate.setTransform (AffineTransform::translation (-150.0f, 0.0f));

            expect (traverser.getComponentInDirection (&source, FocusNavigationDirection::left) == &transformedCandidate);
            expect (traverser.getComponentInDirection (&source, FocusNavigationDirection::right) == nullptr);
        }

        beginTest ("Parent clipping does not change directional geometry");
        {
            Component parent, source, clippedCandidate;
            parent.setBounds (0, 0, 20, 20);
            addFocusableChild (parent, source,           {  0, 0, 10, 10 });
            addFocusableChild (parent, clippedCandidate, { 40, 0, 10, 10 });

            expect (traverser.getComponentInDirection (&source, FocusNavigationDirection::right) == &clippedCandidate);
        }

        beginTest ("Hidden, disabled, and dynamically ineligible candidates are skipped");
        {
            Component parent, source, hidden, disabled, ineligible;
            addFocusableChild (parent, source,     {  0, 0, 10, 10 });
            addFocusableChild (parent, hidden,     { 20, 0, 10, 10 });
            addFocusableChild (parent, disabled,   { 40, 0, 10, 10 });
            addFocusableChild (parent, ineligible, { 60, 0, 10, 10 });

            hidden.setVisible (false);
            disabled.setEnabled (false);
            ineligible.setWantsKeyboardFocus (false);

            expect (traverser.getComponentInDirection (&source, FocusNavigationDirection::right) == nullptr);

            ineligible.setWantsKeyboardFocus (true);
            expect (traverser.getComponentInDirection (&source, FocusNavigationDirection::right) == &ineligible);
        }

        beginTest ("Nested keyboard-focus containers remain traversal boundaries");
        {
            Component parent, source, container, firstNestedChild, secondNestedChild;
            addFocusableChild (parent, source,    {  0, 0, 10, 10 });
            addFocusableChild (parent, container, { 40, 0, 30, 10 });
            container.setFocusContainerType (Component::FocusContainerType::keyboardFocusContainer);
            addFocusableChild (container, firstNestedChild,  {  0, 0, 10, 10 });
            addFocusableChild (container, secondNestedChild, { 20, 0, 10, 10 });

            expect (traverser.getComponentInDirection (&source, FocusNavigationDirection::right) == &container);
            expect (traverser.getComponentInDirection (&firstNestedChild,
                                                       FocusNavigationDirection::right) == &secondNestedChild);
            expect (traverser.getComponentInDirection (&firstNestedChild,
                                                       FocusNavigationDirection::left) == nullptr);
        }

        beginTest ("Candidate order is the stable final tie-break");
        {
            Component parent, source, firstCandidate, secondCandidate;
            addFocusableChild (parent, source,          {  0, 0, 10, 10 });
            addFocusableChild (parent, firstCandidate,  { 20, 0, 10, 10 });
            addFocusableChild (parent, secondCandidate, { 20, 0, 10, 10 });

            expect (traverser.getComponentInDirection (&source, FocusNavigationDirection::right) == &firstCandidate);
        }

        beginTest ("A custom traverser inherits the sequential compatibility mapping");
        {
            struct LegacyTraverser final : ComponentTraverser
            {
                Component* getDefaultComponent (Component*) override       { return &next; }
                Component* getNextComponent (Component*) override          { return &next; }
                Component* getPreviousComponent (Component*) override      { return &previous; }
                std::vector<Component*> getAllComponents (Component*) override
                {
                    return { &previous, &next };
                }

                Component previous, next;
            };

            LegacyTraverser legacy;
            Component current;

            expect (legacy.getComponentInDirection (&current, FocusNavigationDirection::left) == &legacy.previous);
            expect (legacy.getComponentInDirection (&current, FocusNavigationDirection::up) == &legacy.previous);
            expect (legacy.getComponentInDirection (&current, FocusNavigationDirection::right) == &legacy.next);
            expect (legacy.getComponentInDirection (&current, FocusNavigationDirection::down) == &legacy.next);
        }

        beginTest ("Arrow keys map to cardinal focus directions");
        {
            const auto getDirection = [] (int keyCode)
            {
                return detail::getFocusNavigationDirectionForKeyPress (KeyPress (keyCode));
            };

            expect (getDirection (KeyPress::leftKey) == FocusNavigationDirection::left);
            expect (getDirection (KeyPress::rightKey) == FocusNavigationDirection::right);
            expect (getDirection (KeyPress::upKey) == FocusNavigationDirection::up);
            expect (getDirection (KeyPress::downKey) == FocusNavigationDirection::down);
            expect (! getDirection (KeyPress::returnKey).has_value());
        }
    }

    struct TestComponent final : public Component
    {
        TestComponent()
        {
            for (auto& child : children)
                addAndMakeVisible (child);
        }

        std::array<Component, 10> children;
    };

    KeyboardFocusTraverser traverser;
};

static KeyboardFocusTraverserTests keyboardFocusTraverserTests;

#endif

} // namespace juce
