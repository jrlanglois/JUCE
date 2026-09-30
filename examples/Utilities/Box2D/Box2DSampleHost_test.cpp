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

#include "Box2DSamples.h"
#include "HostControlsBridge.h"
#include "ReplayFileIO.h"
#include "ReplaySample.h"
#include "host_controls.h"
#include "sample.h"

struct Box2DDemoTestAccess final
{
    static void updateForPresentation (Box2DDemo& demo, double presentationTimeSeconds) { demo.updateForPresentation (presentationTimeSeconds); }
};

namespace
{

std::optional<int> findSampleIndex (const char* category, const char* name)
{
    const auto& entries = Box2DSamples::Catalog::getEntries();

    for (int index = 0; index < (int) entries.size(); ++index)
    {
        const auto& entry = entries[(size_t) index];

        if (std::strcmp (entry.category, category) == 0 && std::strcmp (entry.name, name) == 0)
            return index;
    }

    return std::nullopt;
}

Result selectSample (Box2DSamples::Runtime& runtime, const char* category, const char* name)
{
    if (const auto sampleIndex = findSampleIndex (category, name))
        return runtime.selectSample (*sampleIndex);

    return Result::fail ("The requested Box2D test sample is not registered.");
}

TextButton* findButton (Component& parent, const String& text)
{
    for (auto* child : parent.getChildren())
    {
        if (auto* button = dynamic_cast<TextButton*> (child))
        {
            if (button->getButtonText() == text)
                return button;
        }

        if (auto* button = findButton (*child, text))
            return button;
    }

    return nullptr;
}

bool containsLabelText (Component& parent, const String& text)
{
    for (auto* child : parent.getChildren())
    {
        if (auto* label = dynamic_cast<Label*> (child))
        {
            if (label->getText().containsIgnoreCase (text))
                return true;
        }

        if (containsLabelText (*child, text))
            return true;
    }

    return false;
}

Label* findLabelContainingText (Component& parent, const String& text)
{
    for (auto* child : parent.getChildren())
    {
        if (auto* label = dynamic_cast<Label*> (child))
        {
            if (label->getText().containsIgnoreCase (text))
                return label;
        }

        if (auto* label = findLabelContainingText (*child, text))
            return label;
    }

    return nullptr;
}

template<typename ComponentType>
ComponentType* findComponent (Component& parent)
{
    for (auto* child : parent.getChildren())
    {
        if (auto* component = dynamic_cast<ComponentType*> (child))
            return component;

        if (auto* component = findComponent<ComponentType> (*child))
            return component;
    }

    return nullptr;
}

template<typename PropertyType>
PropertyType* findProperty (Component& parent, const String& propertyName)
{
    for (auto* child : parent.getChildren())
    {
        if (auto* property = dynamic_cast<PropertyType*> (child))
        {
            if (property->getName() == propertyName)
                return property;
        }

        if (auto* property = findProperty<PropertyType> (*child, propertyName))
            return property;
    }

    return nullptr;
}

template<typename ComponentType>
int countComponents (Component& parent)
{
    int numComponents = 0;

    for (auto* child : parent.getChildren())
    {
        if (dynamic_cast<ComponentType*> (child) != nullptr)
            ++numComponents;

        numComponents += countComponents<ComponentType> (*child);
    }

    return numComponents;
}

Component* findComponentWithTitle (Component& parent, const String& title)
{
    for (auto* child : parent.getChildren())
    {
        if (child->getTitle() == title)
            return child;

        if (auto* component = findComponentWithTitle (*child, title))
            return component;
    }

    return nullptr;
}

class ReadOnlyPropertyPaintLookAndFeel final : public LookAndFeel_V4
{
public:
    void drawPropertyComponentBackground (Graphics& graphics, int width, int height, PropertyComponent& property) override
    {
        ++numBackgroundDraws;
        LookAndFeel_V4::drawPropertyComponentBackground (graphics, width, height, property);
    }

    void drawPropertyComponentLabel (Graphics& graphics, int width, int height, PropertyComponent& property) override
    {
        ++numLabelDraws;
        LookAndFeel_V4::drawPropertyComponentLabel (graphics, width, height, property);
    }

    [[nodiscard]] int getNumBackgroundDraws() const noexcept { return numBackgroundDraws; }

    [[nodiscard]] int getNumLabelDraws() const noexcept { return numLabelDraws; }

private:
    int numBackgroundDraws = 0,
        numLabelDraws = 0;
};

bool isVisibleWithin (const Component& component, const Component& root)
{
    for (auto* current = &component; current != nullptr; current = current->getParentComponent())
    {
        if (current == &root)
            return true;

        if (! current->isVisible())
            return false;
    }

    return false;
}

MouseEvent makeMouseEvent (Component& component,
                           juce::Point<float> position,
                           ModifierKeys modifiers,
                           juce::Point<float> mouseDownPosition,
                           bool wasDragged = false)
{
    const auto eventTime = Time::getCurrentTime();
    return { Desktop::getInstance().getMainMouseSource(),
             position,
             modifiers,
             1.0f,
             0.0f,
             0.0f,
             0.0f,
             0.0f,
             &component,
             &component,
             eventTime,
             mouseDownPosition,
             eventTime,
             1,
             wasDragged };
}

std::optional<MemoryBlock> createRecording (const char* category, const char* name, int numSteps)
{
    Box2DSamples::Runtime runtime;
    runtime.getContext().shouldUseReducedWorkload = true;

    if (selectSample (runtime, category, name).failed())
        return std::nullopt;

    runtime.startRecording();

    for (int stepIndex = 0; stepIndex < numSteps; ++stepIndex)
        runtime.getCurrentSample()->advanceSimulation();

    return runtime.stopRecording();
}

class Box2DSampleHostTests final : public UnitTest
{
public:
    Box2DSampleHostTests() : UnitTest ("Box2D sample host", UnitTestCategories::box2dSamples) {}

    void runTest() override
    {
        const auto expectBoundsEqual = [this] (b2AABB actual, b2AABB expected, float tolerance = 0.001f)
        {
            expectWithinAbsoluteError (actual.lowerBound.x, expected.lowerBound.x, tolerance);
            expectWithinAbsoluteError (actual.lowerBound.y, expected.lowerBound.y, tolerance);
            expectWithinAbsoluteError (actual.upperBound.x, expected.upperBound.x, tolerance);
            expectWithinAbsoluteError (actual.upperBound.y, expected.upperBound.y, tolerance);
        };

        const auto expectBoundsContain = [this] (b2AABB outer, b2AABB inner, float tolerance = 0.001f)
        {
            expect (outer.lowerBound.x <= inner.lowerBound.x + tolerance);
            expect (outer.lowerBound.y <= inner.lowerBound.y + tolerance);
            expect (outer.upperBound.x >= inner.upperBound.x - tolerance);
            expect (outer.upperBound.y >= inner.upperBound.y - tolerance);
        };

        beginTest ("Authored Home catalogue parity");

        {
            Box2DSamples::Catalog::initialise();
            const auto& catalogueEntries = Box2DSamples::Catalog::getEntries();
            const auto homeViews = Box2DSamples::HomeViewCatalog::getEntries();
            int numOrdinaryEntries = 0;

            for (const auto& entry : catalogueEntries)
            {
                const auto* homeView = Box2DSamples::HomeViewCatalog::find (entry.category, entry.name);

                if (entry.isReplayViewer)
                {
                    expect (homeView == nullptr);
                    continue;
                }

                ++numOrdinaryEntries;
                expect (homeView != nullptr, String::fromUTF8 (entry.category) + " / " + String::fromUTF8 (entry.name));
            }

            expectEquals ((int) homeViews.size(), numOrdinaryEntries);
            expect (Box2DSamples::HomeViewCatalog::find (nullptr, "Sample") == nullptr);
            expect (Box2DSamples::HomeViewCatalog::find ("Category", nullptr) == nullptr);

            for (size_t homeViewIndex = 0; homeViewIndex < homeViews.size(); ++homeViewIndex)
            {
                const auto& homeView = homeViews[homeViewIndex];
                expect (homeView.category != nullptr);
                expect (homeView.sampleName != nullptr);
                expect (b2IsValidAABB (homeView.defaultBounds));
                const auto defaultExtents = b2AABB_Extents (homeView.defaultBounds);
                expect (defaultExtents.x > 0.0f);
                expect (defaultExtents.y > 0.0f);

                if (homeView.portraitBounds.has_value())
                {
                    expect (b2IsValidAABB (*homeView.portraitBounds));
                    const auto portraitExtents = b2AABB_Extents (*homeView.portraitBounds);
                    expect (portraitExtents.x > 0.0f);
                    expect (portraitExtents.y > 0.0f);
                }

                int numCatalogueMatches = 0;

                for (const auto& entry : catalogueEntries)
                {
                    if (! entry.isReplayViewer
                        && std::strcmp (entry.category, homeView.category) == 0
                        && std::strcmp (entry.name, homeView.sampleName) == 0)
                        ++numCatalogueMatches;
                }

                expectEquals (numCatalogueMatches, 1);

                if (homeViewIndex > 0)
                {
                    const auto& previous = homeViews[homeViewIndex - 1];
                    const auto categoryComparison = std::strcmp (previous.category, homeView.category);
                    expect (categoryComparison < 0
                            || (categoryComparison == 0 && std::strcmp (previous.sampleName, homeView.sampleName) < 0));
                }
            }

            const b2AABB defaultBounds = { { -4.0f, -2.0f }, { 4.0f, 2.0f } };
            const b2AABB portraitBounds = { { -2.0f, -4.0f }, { 2.0f, 4.0f } };
            const Box2DSamples::HomeView homeView { "Category", "Sample", defaultBounds, portraitBounds };
            expectBoundsEqual (homeView.getBoundsForAspectRatio (0.5f), portraitBounds);
            expectBoundsEqual (homeView.getBoundsForAspectRatio (1.0f), defaultBounds);
            expectBoundsEqual (homeView.getBoundsForAspectRatio (0.0f), defaultBounds);
        }

        beginTest ("Presentation clock cadence");

        {
            Box2DSamples::Runtime runtime;
            runtime.getContext().shouldUseReducedWorkload = true;
            expect (selectSample (runtime, "Bodies", "Bad").wasOk());
            runtime.getContext().settings.hertz = 60.0f;
            runtime.resetPresentationClock();

            auto* sample = runtime.getCurrentSample();
            expect (sample != nullptr);

            expectEquals (sample->getNumSteps(), 0);

            runtime.updateForPresentation (0.0);
            expectEquals (sample->getNumSteps(), 0);

            runtime.updateForPresentation (1.0 / 120.0);
            expectEquals (sample->getNumSteps(), 0);

            runtime.updateForPresentation (1.0 / 60.0);
            expectEquals (sample->getNumSteps(), 1);

            runtime.resetPresentationClock();
            runtime.updateForPresentation (0.0);
            expectEquals (sample->getNumSteps(), 1);

            runtime.updateForPresentation (1.0 / 30.0);
            expectEquals (sample->getNumSteps(), 3);

            runtime.resetPresentationClock();
            runtime.updateForPresentation (0.0);
            runtime.updateForPresentation (1.0);
            expectEquals (sample->getNumSteps(), 11);

            runtime.updateForPresentation (1.001);
            expectEquals (sample->getNumSteps(), 11);
        }

        beginTest ("Pause discards elapsed simulation time");

        {
            Box2DSamples::Runtime runtime;
            runtime.getContext().shouldUseReducedWorkload = true;
            expect (selectSample (runtime, "Bodies", "Bad").wasOk());
            auto& settings = runtime.getContext().settings;
            settings.hertz = 60.0f;
            settings.isPaused = false;
            runtime.resetPresentationClock();

            runtime.updateForPresentation (0.0);
            runtime.updateForPresentation (1.0 / 60.0);
            expectEquals (runtime.getCurrentSample()->getNumSteps(), 1);

            settings.isPaused = true;
            runtime.updateForPresentation (1.0);
            expectEquals (runtime.getCurrentSample()->getNumSteps(), 1);

            settings.isPaused = false;
            runtime.updateForPresentation (1.001);
            expectEquals (runtime.getCurrentSample()->getNumSteps(), 1);
        }

        beginTest ("Queued single steps");

        {
            Box2DSamples::Runtime runtime;
            runtime.getContext().shouldUseReducedWorkload = true;
            expect (selectSample (runtime, "Bodies", "Bad").wasOk());
            auto& settings = runtime.getContext().settings;
            settings.hertz = 60.0f;
            settings.isPaused = true;
            runtime.resetPresentationClock();

            runtime.updateForPresentation (0.0);
            expectEquals (runtime.getCurrentSample()->getNumSteps(), 0);

            settings.numSingleSteps = 1;
            runtime.updateForPresentation (1.0 / 60.0);
            expectEquals (runtime.getCurrentSample()->getNumSteps(), 1);
            expectEquals (settings.numSingleSteps, 0);

            settings.numSingleSteps = 1;
            runtime.updateForPresentation (2.0 / 60.0);
            expectEquals (runtime.getCurrentSample()->getNumSteps(), 2);
            expectEquals (settings.numSingleSteps, 0);
        }

        beginTest ("Selection resets defaults and restart preserves state");

        {
            Box2DSamples::Runtime runtime;
            auto& context = runtime.getContext();
            auto& settings = context.settings;
            settings.numSubSteps = 9;
            settings.numRestitutionIterations = 6;
            settings.numSingleSteps = 3;
            settings.isPaused = true;
            settings.isRestitutionPropagationEnabled = true;
            context.camera.centre = { 123.0f, 456.0f };
            context.camera.zoom = 7.0f;
            context.debugDraw.drawJoints = false;

            expect (runtime.restartSample().failed());
            expect (selectSample (runtime, "Bodies", "Bad").wasOk());
            const Box2DSamples::SimulationSettings defaultSettings;
            expectEquals (settings.numSubSteps, defaultSettings.numSubSteps);
            expectEquals (settings.numRestitutionIterations, defaultSettings.numRestitutionIterations);
            expectEquals (settings.numSingleSteps, 0);
            expect (settings.isPaused == defaultSettings.isPaused);
            expect (settings.isRestitutionPropagationEnabled == defaultSettings.isRestitutionPropagationEnabled);
            expect (context.camera.centre.x != 123.0f || context.camera.centre.y != 456.0f);
            expect (context.debugDraw.drawJoints);
            const auto homeCameraCentre = context.homeCameraCentre;
            const auto homeCameraZoom = context.homeCameraZoom;
            expectWithinAbsoluteError ((float) context.camera.centre.x, (float) homeCameraCentre.x, 0.001f);
            expectWithinAbsoluteError ((float) context.camera.centre.y, (float) homeCameraCentre.y, 0.001f);
            expectWithinAbsoluteError (context.camera.zoom, homeCameraZoom, 0.001f);

            settings.numSubSteps = 7;
            settings.numRestitutionIterations = 5;
            settings.numSingleSteps = 2;
            settings.isPaused = true;
            settings.isRestitutionPropagationEnabled = true;
            context.camera.centre = { -10.0f, 11.0f };
            context.camera.zoom = 3.0f;
            context.debugDraw.drawJoints = false;

            expect (runtime.restartSample().wasOk());
            expectEquals (settings.numSubSteps, 7);
            expectEquals (settings.numRestitutionIterations, 5);
            expectEquals (settings.numSingleSteps, 0);
            expect (settings.isPaused);
            expect (settings.isRestitutionPropagationEnabled);
            expectWithinAbsoluteError ((float) context.camera.centre.x, -10.0f, 0.001f);
            expectWithinAbsoluteError ((float) context.camera.centre.y, 11.0f, 0.001f);
            expectWithinAbsoluteError (context.camera.zoom, 3.0f, 0.001f);
            expect (! context.debugDraw.drawJoints);
            runtime.getCurrentSample()->resetCamera();
            expectWithinAbsoluteError ((float) context.camera.centre.x, (float) homeCameraCentre.x, 0.001f);
            expectWithinAbsoluteError ((float) context.camera.centre.y, (float) homeCameraCentre.y, 0.001f);
            expectWithinAbsoluteError (context.camera.zoom, homeCameraZoom, 0.001f);
        }

        beginTest ("Imported sample settings reach the native host");

        {
            Box2DSamples::Runtime runtime;
            runtime.getContext().shouldUseReducedWorkload = true;
            expect (selectSample (runtime, "Robustness", "Cart").wasOk());
            expectEquals (runtime.getContext().settings.numSubSteps, 12);
            expect (selectSample (runtime, "Events", "Sensor Funnel").wasOk());
            expect (! runtime.getContext().debugDraw.drawJoints);
        }

        beginTest ("Runtime authored camera framing");

        {
            Box2DSamples::Runtime runtime;
            runtime.getContext().shouldUseReducedWorkload = true;
            auto& camera = runtime.getContext().camera;
            camera.setDrawableSize (640.0f, 480.0f);
            expect (selectSample (runtime, "Joints", "Desk Lamp").wasOk());
            expect (runtime.getContext().homeView != nullptr);

            if (runtime.getContext().homeView != nullptr)
            {
                const auto expectedBounds = runtime.getContext().homeView->getBoundsForAspectRatio (camera.getAspectRatio());
                expectBoundsContain (camera.getVisibleBounds(), expectedBounds);
            }
        }

        beginTest ("Camera round trip");

        {
            Box2DSamples::Camera camera;
            camera.setDrawableSize (640.0f, 480.0f);
            camera.centre = { 1.0f, 2.0f };
            camera.zoom = 2.0f;
            const juce::Rectangle<float> area (0.0f, 0.0f, 640.0f, 480.0f);
            const b2Pos worldPoint { 3.0f, 4.0f };
            const auto componentPoint = camera.convertWorldToComponent (worldPoint, area);
            const auto roundTrip = camera.convertComponentToWorld (componentPoint, area);
            expectWithinAbsoluteError<double> ((double) roundTrip.x, (double) worldPoint.x, 0.02);
            expectWithinAbsoluteError<double> ((double) roundTrip.y, (double) worldPoint.y, 0.02);
            const auto viewSize = camera.getViewSize();
            expectWithinAbsoluteError (viewSize.x, 16.0f / 3.0f, 0.001f);
            expectWithinAbsoluteError (viewSize.y, 4.0f, 0.001f);

            camera.setViewToBounds ({ { -4.0f, -1.0f }, { 4.0f, 1.0f } });
            expectWithinAbsoluteError ((float) camera.centre.x, 0.0f, 0.001f);
            expectWithinAbsoluteError ((float) camera.centre.y, 0.0f, 0.001f);
            expectWithinAbsoluteError (camera.zoom, 3.0f, 0.001f);
            expectWithinAbsoluteError (camera.getViewSize().x, 8.0f, 0.001f);
            expectWithinAbsoluteError (camera.getViewSize().y, 6.0f, 0.001f);
        }

        beginTest ("Responsive authored Home framing");

        {
            Box2DSamples::Runtime runtime;
            Box2DSamples::Canvas canvas;
            auto& context = runtime.getContext();
            auto& camera = context.camera;
            context.shouldUseReducedWorkload = true;
            canvas.setRuntime (&runtime);
            expect (selectSample (runtime, "Joints", "Desk Lamp").wasOk());
            expect (context.homeView != nullptr);
            expectWithinAbsoluteError (camera.drawableWidth, 0.0f, 0.001f);
            expectWithinAbsoluteError (camera.drawableHeight, 0.0f, 0.001f);
            expectWithinAbsoluteError (camera.zoom, 1.6f, 0.001f);

            canvas.setSize (640, 480);
            canvas.resized();
            expectWithinAbsoluteError (camera.drawableWidth, 624.0f, 0.001f);
            expectWithinAbsoluteError (camera.drawableHeight, 464.0f, 0.001f);
            expectWithinAbsoluteError (camera.getAspectRatio(), 624.0f / 464.0f, 0.001f);

            if (context.homeView != nullptr)
            {
                const auto expectedBounds = context.homeView->getBoundsForAspectRatio (camera.getAspectRatio());
                expectBoundsContain (camera.getVisibleBounds(), expectedBounds);
                const auto expectedCentre = b2AABB_Center (expectedBounds);
                expectWithinAbsoluteError ((float) camera.centre.x, expectedCentre.x, 0.001f);
                expectWithinAbsoluteError ((float) camera.centre.y, expectedCentre.y, 0.001f);
            }

            const auto landscapeZoom = camera.zoom;
            canvas.setSize (480, 640);
            canvas.resized();
            expectWithinAbsoluteError (camera.getAspectRatio(), 464.0f / 624.0f, 0.001f);
            expect (camera.zoom > landscapeZoom);

            if (context.homeView != nullptr)
                expectBoundsContain (camera.getVisibleBounds(), context.homeView->getBoundsForAspectRatio (camera.getAspectRatio()));

            const auto eventTime = Time::getCurrentTime();
            const juce::Point<float> eventPosition (100.0f, 100.0f);
            const MouseEvent mouseEvent (Desktop::getInstance().getMainMouseSource(),
                                         eventPosition,
                                         {},
                                         MouseInputSource::defaultPressure,
                                         MouseInputSource::defaultOrientation,
                                         MouseInputSource::defaultRotation,
                                         MouseInputSource::defaultTiltX,
                                         MouseInputSource::defaultTiltY,
                                         &canvas,
                                         &canvas,
                                         eventTime,
                                         eventPosition,
                                         eventTime,
                                         1,
                                         false);
            const MouseWheelDetails wheel { 0.0f, 1.0f, false, false, false };
            canvas.mouseWheelMove (mouseEvent, wheel);
            canvas.applyPendingInput();
            const auto manualCentre = camera.centre;
            const auto manualZoom = camera.zoom;
            canvas.setSize (640, 480);
            canvas.resized();
            expectWithinAbsoluteError ((float) camera.centre.x, (float) manualCentre.x, 0.001f);
            expectWithinAbsoluteError ((float) camera.centre.y, (float) manualCentre.y, 0.001f);
            expectWithinAbsoluteError (camera.zoom, manualZoom, 0.001f);

            expect (runtime.restartSample().wasOk());
            expectWithinAbsoluteError ((float) camera.centre.x, (float) manualCentre.x, 0.001f);
            expectWithinAbsoluteError ((float) camera.centre.y, (float) manualCentre.y, 0.001f);
            expectWithinAbsoluteError (camera.zoom, manualZoom, 0.001f);
            canvas.setSize (800, 500);
            canvas.resized();
            expectWithinAbsoluteError ((float) camera.centre.x, (float) manualCentre.x, 0.001f);
            expectWithinAbsoluteError ((float) camera.centre.y, (float) manualCentre.y, 0.001f);
            expectWithinAbsoluteError (camera.zoom, manualZoom, 0.001f);

            expect (canvas.keyPressed (KeyPress (KeyPress::homeKey)));
            canvas.applyPendingInput();

            if (context.homeView != nullptr)
                expectBoundsContain (camera.getVisibleBounds(), context.homeView->getBoundsForAspectRatio (camera.getAspectRatio()));

            const auto resetZoom = camera.zoom;
            canvas.setSize (500, 800);
            canvas.resized();
            expect (std::abs (camera.zoom - resetZoom) > 0.001f);

            expect (selectSample (runtime, "World", "Far Gate").wasOk());
            canvas.resetView();
            expect (camera.centre.x > 900000.0f);
            expect (context.homeView != nullptr);

            if (context.homeView != nullptr)
                expectBoundsContain (camera.getVisibleBounds(), context.homeView->getBoundsForAspectRatio (camera.getAspectRatio()), 1.0f);

            camera.centre = { 1234567.0f, 45.0f };
            camera.zoom = 9.0f;
            runtime.getCurrentSample()->advanceSimulation();
            runtime.getDrawList().clear();
            runtime.getCurrentSample()->prepareFrame();
            expectWithinAbsoluteError ((float) camera.centre.x, 1234567.0f, 0.001f);
            expectWithinAbsoluteError ((float) camera.centre.y, 45.0f, 0.001f);
            expectWithinAbsoluteError (camera.zoom, 9.0f, 0.001f);
        }

        beginTest ("Canvas paint purity");

        {
            Box2DSamples::Runtime runtime;
            runtime.getContext().shouldUseReducedWorkload = true;
            Box2DSamples::Canvas canvas;
            canvas.setSize (320, 240);
            canvas.setRuntime (&runtime);
            runtime.getContext().settings.isPaused = true;
            expect (selectSample (runtime, "Bodies", "Bad").wasOk());
            canvas.resized();

            const auto stepsBefore = runtime.getCurrentSample()->getNumSteps();
            Image firstImage (Image::ARGB, 320, 240, true);
            Image secondImage (Image::ARGB, 320, 240, true);
            Graphics firstGraphics (firstImage);
            Graphics secondGraphics (secondImage);
            canvas.paint (firstGraphics);
            canvas.paint (secondGraphics);
            expectEquals (runtime.getCurrentSample()->getNumSteps(), stepsBefore);
        }

        beginTest ("Canvas accessibility");

        {
            Box2DSamples::Canvas canvas;
            canvas.updateAccessibility ("Falling Boxes", "Use the arrow keys.");
            auto handler = canvas.createAccessibilityHandler();
            expect (handler != nullptr);

            if (handler != nullptr)
            {
                expect (handler->getRole() == AccessibilityRole::image);
                expectEquals (handler->getTitle(), String ("Falling Boxes"));
                expectEquals (handler->getDescription(), String ("Use the arrow keys."));
                expect (handler->getActions().contains (AccessibilityActionType::focus));
            }
        }

        beginTest ("Catalogue properties and full-height controls");

        {
            Box2DDemo demo;
            demo.setBounds (-10000, -10000, 1200, 800);
            demo.addToDesktop (ComponentPeer::windowIsTemporary);
            demo.setVisible (true);
            Box2DDemoTestAccess::updateForPresentation (demo, 0.0);

            auto* properties = dynamic_cast<PropertyPanel*> (findComponentWithTitle (demo, "Box2D controls"));
            auto* canvas = findComponent<Box2DSamples::Canvas> (demo);
            auto* canvasInputHelp = demo.findChildWithID ("box2dCanvasInputHelp");
            expect (properties != nullptr);
            expect (canvas != nullptr);
            expect (canvasInputHelp == nullptr);
            expect (findComponentWithTitle (demo, "Mouse wheel") != nullptr);
            expect (findButton (demo, "Reset view") != nullptr);
            expect (findComponent<ListBox> (demo) == nullptr);
            expect (findComponent<TabbedComponent> (demo) == nullptr);

            if (properties != nullptr)
            {
                const auto sectionNames = properties->getSectionNames();
                expect (sectionNames.contains ("Catalogue"));
                expect (sectionNames.contains ("Controls"));
                expect (sectionNames.contains ("Simulation"));
                expect (! sectionNames.contains ("Metrics"));
                expectEquals (properties->getY(), 0);
                expectEquals (properties->getHeight(), demo.getHeight());
                expectEquals (properties->getExplicitFocusOrder(), 1);
                expectEquals (countComponents<Viewport> (*properties), 1);

                auto* searchProperty = findProperty<TextPropertyComponent> (*properties, "Search");
                auto* categoryProperty = findProperty<ChoicePropertyComponent> (*properties, "Category");
                auto* sampleProperty = findProperty<ChoicePropertyComponent> (*properties, "Sample");
                auto* profilerProperty = findProperty<BooleanPropertyComponent> (*properties, "Profiler");
                expect (searchProperty != nullptr);
                expect (categoryProperty != nullptr);
                expect (sampleProperty != nullptr);
                expect (profilerProperty != nullptr && ! profilerProperty->getState());

                if (searchProperty != nullptr)
                {
                    StringArray expectedLabels;
                    searchProperty->setText ("joint");
                    Box2DDemoTestAccess::updateForPresentation (demo, 0.0);

                    for (const auto& entry : Box2DSamples::Catalog::getEntries())
                    {
                        const String sampleCategory (entry.category);
                        const String sampleName (entry.name);

                        if (sampleCategory.containsIgnoreCase ("joint") || sampleName.containsIgnoreCase ("joint"))
                            expectedLabels.add (sampleCategory + " " + String::fromUTF8 ("\xe2\x80\x94") + " " + sampleName);
                    }

                    sampleProperty = findProperty<ChoicePropertyComponent> (*properties, "Sample");
                    expect (sampleProperty != nullptr);

                    if (sampleProperty != nullptr)
                        expect (sampleProperty->getChoices() == expectedLabels);
                }

                searchProperty = findProperty<TextPropertyComponent> (*properties, "Search");

                if (searchProperty != nullptr)
                {
                    searchProperty->setText ({});
                    Box2DDemoTestAccess::updateForPresentation (demo, 0.0);
                }

                categoryProperty = findProperty<ChoicePropertyComponent> (*properties, "Category");
                expect (categoryProperty != nullptr);

                demo.setSize (1200, 360);

                for (int sectionIndex = 0; sectionIndex < properties->getSectionNames().size(); ++sectionIndex)
                    properties->setSectionOpen (sectionIndex, true);

                properties->getViewport().setViewPosition (0, 120);
                const auto savedScrollPosition = properties->getViewport().getViewPositionY();

                const auto verifyCategorySelection = [&] (const String& categoryName)
                {
                    categoryProperty = findProperty<ChoicePropertyComponent> (*properties, "Category");
                    expect (categoryProperty != nullptr);

                    if (categoryProperty == nullptr)
                        return;

                    auto* categoryControl = findComponent<ComboBox> (*categoryProperty);
                    const auto categoryIndex = categoryProperty->getChoices().indexOf (categoryName);
                    expect (categoryControl != nullptr);
                    expect (categoryIndex >= 0);

                    if (categoryControl == nullptr || categoryIndex < 0)
                        return;

                    categoryControl->setSelectedItemIndex (categoryIndex, sendNotificationSync);
                    Box2DDemoTestAccess::updateForPresentation (demo, 0.0);
                    categoryProperty = findProperty<ChoicePropertyComponent> (*properties, "Category");
                    expect (categoryProperty != nullptr);

                    if (categoryProperty != nullptr)
                    {
                        const auto selectedCategoryIndex = categoryProperty->getIndex();
                        expect (isPositiveAndBelow (selectedCategoryIndex, categoryProperty->getChoices().size()));

                        if (isPositiveAndBelow (selectedCategoryIndex, categoryProperty->getChoices().size()))
                            expectEquals (categoryProperty->getChoices()[selectedCategoryIndex], categoryName);
                    }

                    sampleProperty = findProperty<ChoicePropertyComponent> (*properties, "Sample");
                    expect (sampleProperty != nullptr);

                    if (sampleProperty != nullptr)
                    {
                        for (const auto& sampleName : sampleProperty->getChoices())
                            expect (sampleName.startsWith (categoryName + " "));
                    }
                };

                verifyCategorySelection ("Bodies");
                verifyCategorySelection ("Issues");

                const auto rebuiltSectionNames = properties->getSectionNames();

                for (int sectionIndex = 0; sectionIndex < rebuiltSectionNames.size(); ++sectionIndex)
                    expect (properties->isSectionOpen (sectionIndex));

                expectEquals (properties->getViewport().getViewPositionY(), savedScrollPosition);
            }

            if (properties != nullptr && canvas != nullptr)
            {
                auto* profilerProperty = findProperty<BooleanPropertyComponent> (*properties, "Profiler");
                expect (profilerProperty != nullptr);
                expect (findComponent<Box2DSamples::MetricsComponent> (demo) == nullptr);

                if (profilerProperty != nullptr)
                {
                    profilerProperty->setState (true);
                    Box2DDemoTestAccess::updateForPresentation (demo, 0.0);
                    auto* metrics = findComponent<Box2DSamples::MetricsComponent> (demo);
                    expect (metrics != nullptr && isVisibleWithin (*metrics, demo));
                    expect (properties->getSectionNames().contains ("Metrics"));
                    profilerProperty = findProperty<BooleanPropertyComponent> (*properties, "Profiler");
                    expect (profilerProperty != nullptr);

                    if (profilerProperty != nullptr)
                    {
                        profilerProperty->setState (false);
                        Box2DDemoTestAccess::updateForPresentation (demo, 0.0);
                        expect (findComponent<Box2DSamples::MetricsComponent> (demo) == nullptr);
                        expect (! properties->getSectionNames().contains ("Metrics"));
                    }
                }

                for (const auto size : { juce::Point<int> (700, 800), juce::Point<int> (360, 720) })
                {
                    demo.setSize (size.x, size.y);
                    expectEquals (properties->getY(), 0);
                    expectEquals (properties->getHeight(), demo.getHeight());
                    expect (isVisibleWithin (*canvas, demo));
                    expect (canvas->getWidth() > 0);
                    expectEquals (canvas->getY(), 0);
                    expectEquals (canvas->getHeight(), demo.getHeight());
                    expect (canvas->getX() > properties->getRight());
                }
            }

            demo.setVisible (false);
            demo.removeFromDesktop();
        }

        beginTest ("View calibration controls and localisation");

        {
            expect (LocalisedStrings::getCurrentMappings() == nullptr);

            if (LocalisedStrings::getCurrentMappings() == nullptr)
            {
                Box2DDemo demo;
                demo.setBounds (-10000, -10000, 960, 640);
                auto* properties = dynamic_cast<PropertyPanel*> (findComponentWithTitle (demo, "Box2D controls"));
                auto* sampleProperty = properties != nullptr ? findProperty<ChoicePropertyComponent> (*properties, "Sample") : nullptr;
                expect (properties != nullptr);
                expect (sampleProperty != nullptr);

                if (sampleProperty != nullptr)
                {
                    const auto sampleLabel = "Bodies " + String::fromUTF8 ("\xe2\x80\x94") + " Bad";
                    const auto sampleRow = sampleProperty->getChoices().indexOf (sampleLabel);
                    expect (sampleRow >= 0);

                    if (sampleRow >= 0)
                        sampleProperty->setIndex (sampleRow);
                }

                demo.addToDesktop (ComponentPeer::windowIsTemporary);
                demo.setVisible (true);
                Box2DDemoTestAccess::updateForPresentation (demo, 0.0);
                properties = dynamic_cast<PropertyPanel*> (findComponentWithTitle (demo, "Box2D controls"));
                expect (properties != nullptr);

                if (properties != nullptr)
                {
                    expect (properties->getSectionNames().contains ("View calibration"));
                    auto* aspectLabel = findLabelContainingText (*properties, "Canvas aspect:");
                    auto* lowerBoundLabel = findLabelContainingText (*properties, "Lower bound:");
                    auto* upperBoundLabel = findLabelContainingText (*properties, "Upper bound:");
                    auto* copyButton = findButton (*properties, "Copy current bounds");
                    expect (aspectLabel != nullptr);
                    expect (lowerBoundLabel != nullptr);
                    expect (upperBoundLabel != nullptr);
                    expect (copyButton != nullptr);

                    if (aspectLabel != nullptr)
                    {
                        auto* handler = aspectLabel->getAccessibilityHandler();
                        expect (handler != nullptr);

                        if (handler != nullptr)
                        {
                            expect (handler->getRole() == AccessibilityRole::label);
                            expectEquals (handler->getTitle(), aspectLabel->getText());
                            auto* valueInterface = handler->getValueInterface();
                            expect (valueInterface != nullptr);

                            if (valueInterface != nullptr)
                                expectEquals (valueInterface->getCurrentValueAsString(), aspectLabel->getText());
                        }
                    }

                    if (copyButton != nullptr)
                    {
                        auto* handler = copyButton->getAccessibilityHandler();
                        expect (handler != nullptr);

                        if (handler != nullptr)
                        {
                            expect (handler->getRole() == AccessibilityRole::button);
                            expectEquals (handler->getTitle(), String ("Copy current bounds"));
                            expect (handler->getActions().contains (AccessibilityActionType::press));
                        }

                        expect (copyButton->onClick != nullptr);
                    }
                }

                LocalisedStrings::setCurrentMappings (new LocalisedStrings ("language: Calibration\n"
                                                                            "countries: ca\n"
                                                                            "\"View calibration\" = \"Camera framing\"\n"
                                                                            "\"Canvas aspect: {aspectRatio}.\" = \"{aspectRatio} is the canvas aspect.\"\n"
                                                                            "\"Lower bound: {x}, {y}.\" = \"Lower edge {y}, {x}.\"\n"
                                                                            "\"Upper bound: {x}, {y}.\" = \"Upper edge {y}, {x}.\"\n"
                                                                            "\"Copy current bounds\" = \"Copy camera bounds\"\n",
                                                                            false));
                demo.lookAndFeelChanged();
                properties = dynamic_cast<PropertyPanel*> (findComponentWithTitle (demo, "Box2D controls"));
                expect (properties != nullptr);

                if (properties != nullptr)
                {
                    expect (properties->getSectionNames().contains ("Camera framing"));
                    expect (findLabelContainingText (*properties, "is the canvas aspect.") != nullptr);
                    expect (findLabelContainingText (*properties, "Lower edge") != nullptr);
                    expect (findLabelContainingText (*properties, "Upper edge") != nullptr);
                    expect (findButton (*properties, "Copy camera bounds") != nullptr);
                    expect (! containsLabelText (*properties, "{aspectRatio}"));
                    expect (! containsLabelText (*properties, "{x}"));
                    expect (! containsLabelText (*properties, "{y}"));
                }

                LocalisedStrings::setCurrentMappings (nullptr);
                demo.lookAndFeelChanged();
                demo.setVisible (false);
                demo.removeFromDesktop();
            }
        }

        beginTest ("Property-panel controls and queued edits");

        {
            Box2DSamples::ControlModel controls;
            PropertyPanel properties;
            int numWorkers = 1;
            controls.beginFrame();
            controls.showIntegerSlider ("workers", "Workers", numWorkers, 1, B2_MAX_WORKERS);
            controls.endFrame();
            const auto initialStructureRevision = controls.getStructureRevision();
            Box2DSamples::appendControlProperties (controls, properties);
            properties.setSize (320, 240);

            auto* workersProperty = findProperty<SliderPropertyComponent> (properties, "Workers");
            auto* slider = findComponent<Slider> (properties);
            expect (workersProperty != nullptr);
            expect (slider != nullptr);

            if (workersProperty != nullptr)
            {
                workersProperty->setValue (2.0);
                expectEquals (numWorkers, 1);
                controls.applyQueuedEdits();
                expectEquals (numWorkers, 2);
            }

            numWorkers = 3;
            controls.beginFrame();
            controls.showIntegerSlider ("workers", "Workers", numWorkers, 1, B2_MAX_WORKERS);
            controls.endFrame();
            expectEquals (controls.getStructureRevision(), initialStructureRevision);
            properties.refreshAll();
            expect (findProperty<SliderPropertyComponent> (properties, "Workers") == workersProperty);

            if (workersProperty != nullptr)
                expectEquals (workersProperty->getValue(), 3.0);

            if (slider != nullptr)
            {
                const auto usableTrackWidth = slider->getPositionOfValue (slider->getMaximum()) - slider->getPositionOfValue (slider->getMinimum());
                expectGreaterThan (usableTrackWidth, 100.0f);
            }

            controls.beginFrame();
            controls.showIntegerSlider ("workers", "Workers", numWorkers, 1, B2_MAX_WORKERS);
            controls.showButton ("reset", "Reset", [] {});
            controls.endFrame();
            expect (controls.getStructureRevision() != initialStructureRevision);
        }

        beginTest ("Typed control edits preserve order and reject invalid input");

        {
            Box2DSamples::ControlModel controls;
            bool isEnabled = false;
            int numWorkers = 1,
                numActions = 0;
            String selectedMode = "first",
                   editableText = "before";
            StringArray callbackOrder;
            const std::vector<Box2DSamples::ControlChoice> choices
            {
                { "first", "First" },
                { "second", "Second" }
            };

            controls.beginFrame();
            controls.showToggle ("enabled", "Enabled", isEnabled, [&callbackOrder] { callbackOrder.add ("enabled"); });
            controls.showIntegerSlider ("workers", "Workers", numWorkers, 1, B2_MAX_WORKERS, [&callbackOrder] { callbackOrder.add ("workers"); });
            controls.showChoice ("mode", "Mode", selectedMode, choices, [&callbackOrder] { callbackOrder.add ("mode"); });
            controls.showTextInput ("text", "Text", editableText, [&callbackOrder] { callbackOrder.add ("text"); });
            controls.showButton ("action", "Action", [&]
            {
                ++numActions;
                callbackOrder.add ("action");
            });
            controls.endFrame();

            controls.queueNumberEdit ("workers", std::numeric_limits<double>::infinity());
            controls.queueChoiceEdit ("mode", "missing");
            controls.queueTextEdit ("workers", "wrong kind");
            controls.queueAction ("missing");
            controls.queueBooleanEdit ("enabled", true);
            controls.queueNumberEdit ("workers", 4.0);
            controls.queueChoiceEdit ("mode", "second");
            controls.queueTextEdit ("text", "after");
            controls.queueAction ("action");
            controls.applyQueuedEdits();

            expect (isEnabled);
            expectEquals (numWorkers, 4);
            expectEquals (selectedMode, String ("second"));
            expectEquals (editableText, String ("after"));
            expectEquals (numActions, 1);
            expect (callbackOrder == StringArray { "enabled", "workers", "mode", "text", "action" });

            controls.queueNumberEdit ("workers", (double) B2_MAX_WORKERS + 100.0);
            controls.applyQueuedEdits();
            expectEquals (numWorkers, (int) B2_MAX_WORKERS);

            const auto* staleWorkerSnapshot = controls.findItem ("workers");
            expect (staleWorkerSnapshot != nullptr);

            if (staleWorkerSnapshot != nullptr)
                expectEquals (staleWorkerSnapshot->value, 1.0);

            const auto structureRevision = controls.getStructureRevision();
            controls.beginFrame();
            controls.showToggle ("enabled", "Enabled", isEnabled);
            controls.showIntegerSlider ("workers", "Workers", numWorkers, 1, B2_MAX_WORKERS);
            controls.showChoice ("mode", "Mode", selectedMode, choices);
            controls.showTextInput ("text", "Text", editableText);
            controls.showButton ("action", "Action", [] {});
            controls.endFrame();
            expectEquals (controls.getStructureRevision(), structureRevision);
            const auto* currentWorkerSnapshot = controls.findItem ("workers");
            expect (currentWorkerSnapshot != nullptr);

            if (currentWorkerSnapshot != nullptr)
                expectEquals (currentWorkerSnapshot->value, (double) B2_MAX_WORKERS);
        }

        beginTest ("Interaction properties and catalogue help");

        {
            Box2DSamples::Catalog::initialise();
            const auto genericHints = Box2DSamples::SampleInteractionCatalog::getGenericLiveHints();
            const auto interactionEntries = Box2DSamples::SampleInteractionCatalog::getEntries();
            const auto* replayHelp = Box2DSamples::SampleInteractionCatalog::find ("Replay", "Viewer");
            expect (std::any_of (genericHints.begin(), genericHints.end(), [] (const auto& hint) { return hint.identifier == Identifier ("zoomViewport"); }));
            expect (std::any_of (genericHints.begin(), genericHints.end(), [] (const auto& hint) { return hint.identifier == Identifier ("resetViewport"); }));
            expect (replayHelp != nullptr);
            expect (Box2DSamples::SampleInteractionCatalog::find ("Missing", "Sample") == nullptr);

            StringArray genericIdentifiers;

            for (const auto& hint : genericHints)
            {
                expect (hint.identifier.isValid());
                expect (Identifier::isValidIdentifier (hint.identifier.toString()));
                expect (! genericIdentifiers.contains (hint.identifier.toString()));
                genericIdentifiers.add (hint.identifier.toString());
            }

            const StringArray expectedInteractionEntries
            {
                "Character/Dynamic Mover",
                "Character/Geometric Mover",
                "Collision/Cast World",
                "Collision/Dynamic Tree",
                "Collision/Manifold",
                "Collision/Overlap World",
                "Collision/Ray Cast",
                "Collision/Shape Cast",
                "Collision/Shape Distance",
                "Collision/Smooth Manifold",
                "Continuous/Drop",
                "Continuous/Pinball",
                "Events/Contact",
                "Events/Foot Sensor",
                "Events/Projectile Event",
                "Events/Sensor Hits",
                "Geometry/Convex Hull",
                "Joints/Driving",
                "Joints/Gear Lift",
                "Joints/Motion Locks",
                "Joints/Theo Jansen",
                "Replay/Viewer",
                "Shapes/Rolling Resistance",
                "Stacking/Vertical Stack",
                "World/Far Gate",
                "World/Tiles"
            };
            StringArray actualInteractionEntries;

            for (size_t entryIndex = 0; entryIndex < interactionEntries.size(); ++entryIndex)
            {
                const auto& interactionEntry = interactionEntries[entryIndex];
                actualInteractionEntries.add (interactionEntry.category + "/" + interactionEntry.sampleName);
                expect (interactionEntry.category.isNotEmpty());
                expect (interactionEntry.sampleName.isNotEmpty());
                expect (Box2DSamples::SampleInteractionCatalog::find (interactionEntry.category, interactionEntry.sampleName) == &interactionEntry);

                int numCatalogueMatches = 0;

                for (const auto& catalogueEntry : Box2DSamples::Catalog::getEntries())
                {
                    if (interactionEntry.category == catalogueEntry.category && interactionEntry.sampleName == catalogueEntry.name)
                        ++numCatalogueMatches;
                }

                expectEquals (numCatalogueMatches, 1);

                if (entryIndex > 0)
                {
                    const auto& previous = interactionEntries[entryIndex - 1];
                    const auto categoryComparison = previous.category.compare (interactionEntry.category);
                    expect (categoryComparison < 0
                            || (categoryComparison == 0 && previous.sampleName.compare (interactionEntry.sampleName) < 0));
                }

                StringArray hintIdentifiers;

                for (const auto& hint : interactionEntry.hints)
                {
                    expect (hint.identifier.isValid());
                    expect (Identifier::isValidIdentifier (hint.identifier.toString()));
                    expect (! hintIdentifiers.contains (hint.identifier.toString()));
                    hintIdentifiers.add (hint.identifier.toString());

                    if (hint.identifier == Identifier ("primaryInteraction"))
                        expect (genericIdentifiers.contains (hint.identifier.toString()));
                }
            }

            expect (actualInteractionEntries == expectedInteractionEntries);

            if (replayHelp != nullptr)
            {
                expect (std::any_of (replayHelp->hints.begin(), replayHelp->hints.end(), [] (const auto& hint) { return hint.identifier == Identifier ("previousFrame"); }));
                expect (std::any_of (replayHelp->hints.begin(), replayHelp->hints.end(), [] (const auto& hint) { return hint.identifier == Identifier ("previousFrames"); }));
            }

            Box2DSamples::ControlModel controls;
            PropertyPanel properties;
            controls.beginFrame();
            controls.setGroup (Box2DSamples::ControlItem::Group::interactionHelp);
            controls.showInteraction ("zoomHelp", "Mouse wheel", "Zooms the camera");
            controls.setGroup (Box2DSamples::ControlItem::Group::sampleControls);
            controls.showText ("sampleHeading", "Sample options", Box2DSamples::ControlItem::TextTone::subheading);
            controls.endFrame();
            Box2DSamples::appendControlProperties (controls, properties);
            properties.setSize (320, 160);
            expect (properties.getSectionNames() == StringArray { "Controls" });
            auto* interaction = findComponentWithTitle (properties, "Mouse wheel");
            expect (interaction != nullptr);

            if (interaction != nullptr)
            {
                auto handler = interaction->createAccessibilityHandler();
                expect (handler != nullptr);

                if (handler != nullptr)
                {
                    expect (handler->getRole() == AccessibilityRole::staticText);
                    expectEquals (handler->getTitle(), String ("Mouse wheel"));
                    expect (handler->getDescription().contains ("Zooms the camera"));
                }
            }

            auto* subheading = findLabelContainingText (properties, "Sample options");
            expect (subheading != nullptr);

            if (subheading != nullptr)
                expect (subheading->getFont().isBold());

            auto* subheadingProperty = findProperty<PropertyComponent> (properties, "Sample options");
            expect (subheadingProperty != nullptr);

            if (subheadingProperty != nullptr)
            {
                ReadOnlyPropertyPaintLookAndFeel paintLookAndFeel;
                subheadingProperty->setLookAndFeel (&paintLookAndFeel);
                Image renderedProperty (Image::ARGB, subheadingProperty->getWidth(), subheadingProperty->getHeight(), true);
                Graphics graphics (renderedProperty);
                subheadingProperty->paintEntireComponent (graphics, true);
                expectEquals (paintLookAndFeel.getNumBackgroundDraws(), 1);
                expectEquals (paintLookAndFeel.getNumLabelDraws(), 0);
                subheadingProperty->setLookAndFeel (nullptr);
            }
        }

        beginTest ("Adapted formatted text keeps stable identifiers");

        {
            Box2DSamples::ControlModel controls;
            Box2DSamples::HostControlsBridge bridge;
            const auto describeStep = [&] (int step)
            {
                controls.beginFrame();
                controls.setGroup (Box2DSamples::ControlItem::Group::sampleControls);
                bridge.beginFrame (controls);
                HostControls::Text ("Step %d", step);
                bridge.endFrame();
                controls.endFrame();
            };

            describeStep (1);
            const auto structureRevision = controls.getStructureRevision();
            expectEquals ((int) controls.getItems().size(), 1);
            const auto identifier = controls.getItems()[0].identifier;
            expectEquals (controls.getItems()[0].labelText, String ("Step 1"));
            describeStep (2);
            expectEquals (controls.getStructureRevision(), structureRevision);
            expectEquals ((int) controls.getItems().size(), 1);
            expectEquals (controls.getItems()[0].identifier, identifier);
            expectEquals (controls.getItems()[0].labelText, String ("Step 2"));
        }

        beginTest ("Localisation refresh");

        {
            expect (LocalisedStrings::getCurrentMappings() == nullptr);

            if (LocalisedStrings::getCurrentMappings() == nullptr)
            {
                Box2DDemo demo;
                auto* properties = dynamic_cast<PropertyPanel*> (findComponentWithTitle (demo, "Box2D controls"));
                expect (properties != nullptr);
                LocalisedStrings::setCurrentMappings (new LocalisedStrings ("language: Test\n"
                                                                            "countries: ca\n"
                                                                            "\"Search\" = \"Find physics samples\"\n"
                                                                            "\"Benchmark\" = \"Performance\"\n"
                                                                            "\"Barrel\" = \"Cask\"\n",
                                                                            false));
                demo.lookAndFeelChanged();
                auto* searchProperty = properties != nullptr ? findProperty<TextPropertyComponent> (*properties, "Find physics samples") : nullptr;
                auto* sampleProperty = properties != nullptr ? findProperty<ChoicePropertyComponent> (*properties, "Sample") : nullptr;
                expect (searchProperty != nullptr);
                expect (sampleProperty != nullptr);

                if (sampleProperty != nullptr)
                    expectEquals (sampleProperty->getChoices()[0], String ("Performance ") + String::fromUTF8 ("\xe2\x80\x94") + " Cask");

                LocalisedStrings::setCurrentMappings (nullptr);
                demo.lookAndFeelChanged();
            }
        }

        beginTest ("Profile and counter accessibility");

        {
            Box2DSamples::Runtime runtime;
            runtime.getContext().shouldUseReducedWorkload = true;
            expect (selectSample (runtime, "Bodies", "Bad").wasOk());
            runtime.getCurrentSample()->advanceSimulation();
            Box2DSamples::MetricsComponent metrics;
            metrics.setRuntime (&runtime);
            metrics.refresh();

            auto* profileChart = findComponentWithTitle (metrics, "Step profile history");
            auto* stepLabel = dynamic_cast<Label*> (findComponentWithTitle (metrics, "Simulation step count"));
            auto* counterLabel = dynamic_cast<Label*> (findComponentWithTitle (metrics, "World counters"));
            expect (profileChart != nullptr);
            expect (stepLabel != nullptr);
            expect (counterLabel != nullptr);
            expect (! metrics.isProfilerVisible());
            expect (profileChart != nullptr && ! profileChart->isVisible());
            expect (stepLabel != nullptr && ! stepLabel->isVisible());
            expect (counterLabel != nullptr && ! counterLabel->isVisible());

            metrics.setProfilerVisible (true);
            expect (metrics.isProfilerVisible());

            if (profileChart != nullptr)
            {
                expect (profileChart->isVisible());
                expect (profileChart->getDescription().contains ("Step"));
                expect (profileChart->getDescription().contains ("collision"));
                expect (profileChart->getDescription().contains ("solve"));
            }

            if (stepLabel != nullptr)
            {
                expect (stepLabel->isVisible());
                expect (stepLabel->getText().contains ("Steps:"));
            }

            if (counterLabel != nullptr)
            {
                expect (counterLabel->isVisible());
                expect (counterLabel->getText().contains ("Bodies:"));
            }
        }

        beginTest ("Query-only preparation and queued keyboard input");

        {
            Box2DSamples::Runtime runtime;
            Box2DSamples::Canvas canvas;
            runtime.getContext().shouldUseReducedWorkload = true;
            expect (selectSample (runtime, "Geometry", "Convex Hull").wasOk());
            canvas.setSize (320, 240);
            canvas.setRuntime (&runtime);
            canvas.resized();

            runtime.resetPresentationClock();
            runtime.updateForPresentation (0.0);
            expectEquals (runtime.getCurrentSample()->getNumSteps(), 0);

            expect (canvas.keyPressed (KeyPress ('g')));
            expectEquals (runtime.getCurrentSample()->getNumSteps(), 0);
            canvas.applyPendingInput();
            runtime.updateForPresentation (1.0 / 60.0);
            expectEquals (runtime.getCurrentSample()->getNumSteps(), 0);
        }

        beginTest ("Injected held-key state");

        {
            Box2DSamples::Runtime runtime;
            int numKeyQueries = 0;
            bool receivedModifierFreeKey = true;
            runtime.getContext().shouldUseReducedWorkload = true;
            runtime.getContext().setKeyStateQuery ([&] (const KeyPress& key)
            {
                ++numKeyQueries;
                receivedModifierFreeKey = receivedModifierFreeKey && ! key.getModifiers().isAnyModifierKeyDown();
                return key.isKeyCode ('d');
            });
            expect (selectSample (runtime, "Character", "Dynamic Mover").wasOk());
            runtime.getCurrentSample()->advanceSimulation();
            expect (numKeyQueries > 0);
            expect (receivedModifierFreeKey);
        }

        beginTest ("Direct adapted key and pointer values");

        {
            Box2DSamples::Runtime runtime;
            runtime.getContext().shouldUseReducedWorkload = true;
            expect (selectSample (runtime, "Stacking", "Single Box").wasOk());
            auto* sample = runtime.getCurrentSample();
            expect (sample != nullptr);

            if (sample != nullptr)
            {
                const auto worldId = sample->getWorldId();
                const auto initialJointCount = b2World_GetCounters (worldId).jointCount;
                sample->handleMouseDown ({ 0.0f, 1.0f }, ModifierKeys (ModifierKeys::rightButtonModifier));
                expectEquals (b2World_GetCounters (worldId).jointCount, initialJointCount);
                sample->handleMouseDown ({ 0.0f, 1.0f }, ModifierKeys (ModifierKeys::leftButtonModifier));
                expectEquals (b2World_GetCounters (worldId).jointCount, initialJointCount + 1);
                sample->handleMouseMove ({ 1.0f, 1.0f },
                                         ModifierKeys (ModifierKeys::leftButtonModifier | ModifierKeys::shiftModifier));
                sample->handleMouseUp ({ 1.0f, 1.0f },
                                       ModifierKeys (ModifierKeys::leftButtonModifier | ModifierKeys::shiftModifier));
                expectEquals (b2World_GetCounters (worldId).jointCount, initialJointCount);
            }

            expect (selectSample (runtime, "Stacking", "Vertical Stack").wasOk());
            sample = runtime.getCurrentSample();
            expect (sample != nullptr);

            if (sample != nullptr)
            {
                const auto worldId = sample->getWorldId();
                const auto initialBodyCount = b2World_GetCounters (worldId).bodyCount;
                expect (sample->handleKeyPress (KeyPress ('b', ModifierKeys(), 0)));
                expectGreaterThan (b2World_GetCounters (worldId).bodyCount, initialBodyCount);
            }

            expect (selectSample (runtime, "Shapes", "Rolling Resistance").wasOk());
            sample = runtime.getCurrentSample();
            expect (sample != nullptr);

            if (sample != nullptr)
            {
                const auto initialWorldId = sample->getWorldId();
                expect (sample->handleKeyPress (KeyPress ('2', ModifierKeys(), 0)));
                sample->advanceSimulation();
                expect (b2World_IsValid (sample->getWorldId()));
                expect (b2StoreWorldId (sample->getWorldId()) != b2StoreWorldId (initialWorldId));
            }
        }

        beginTest ("Canvas preserves pointer state and owns middle-button pan");

        {
            Box2DSamples::Runtime runtime;
            Box2DSamples::Canvas canvas;
            runtime.getContext().shouldUseReducedWorkload = true;
            expect (selectSample (runtime, "Stacking", "Single Box").wasOk());
            canvas.setRuntime (&runtime);
            canvas.setSize (320, 240);
            canvas.resized();
            auto* sample = runtime.getCurrentSample();
            expect (sample != nullptr);

            if (sample != nullptr)
            {
                const auto worldId = sample->getWorldId();
                const auto initialJointCount = b2World_GetCounters (worldId).jointCount;
                const auto targetArea = canvas.getLocalBounds().toFloat().reduced (8.0f);
                const auto bodyPosition = runtime.getContext().camera.convertWorldToComponent ({ 0.0f, 1.0f }, targetArea);
                canvas.mouseDown (makeMouseEvent (canvas,
                                                  bodyPosition,
                                                  ModifierKeys (ModifierKeys::rightButtonModifier),
                                                  bodyPosition));
                canvas.mouseUp (makeMouseEvent (canvas,
                                                bodyPosition,
                                                ModifierKeys (ModifierKeys::rightButtonModifier),
                                                bodyPosition));
                canvas.applyPendingInput();
                expectEquals (b2World_GetCounters (worldId).jointCount, initialJointCount);

                canvas.mouseDown (makeMouseEvent (canvas,
                                                  bodyPosition,
                                                  ModifierKeys (ModifierKeys::leftButtonModifier),
                                                  bodyPosition));
                canvas.applyPendingInput();
                expectEquals (b2World_GetCounters (worldId).jointCount, initialJointCount + 1);
                const auto draggedPosition = bodyPosition + juce::Point<float> (12.0f, 0.0f);
                canvas.mouseDrag (makeMouseEvent (canvas,
                                                  draggedPosition,
                                                  ModifierKeys (ModifierKeys::leftButtonModifier | ModifierKeys::shiftModifier),
                                                  bodyPosition,
                                                  true));
                canvas.mouseUp (makeMouseEvent (canvas,
                                                draggedPosition,
                                                ModifierKeys (ModifierKeys::leftButtonModifier | ModifierKeys::shiftModifier),
                                                bodyPosition,
                                                true));
                canvas.applyPendingInput();
                expectEquals (b2World_GetCounters (worldId).jointCount, initialJointCount);

                const auto cameraCentreBeforePan = runtime.getContext().camera.centre;
                const juce::Point<float> panStart (160.0f, 120.0f);
                const juce::Point<float> panEnd (180.0f, 140.0f);
                canvas.mouseDown (makeMouseEvent (canvas,
                                                  panStart,
                                                  ModifierKeys (ModifierKeys::middleButtonModifier),
                                                  panStart));
                canvas.mouseDrag (makeMouseEvent (canvas,
                                                  panEnd,
                                                  ModifierKeys (ModifierKeys::middleButtonModifier),
                                                  panStart,
                                                  true));
                canvas.mouseUp (makeMouseEvent (canvas,
                                                panEnd,
                                                ModifierKeys (ModifierKeys::middleButtonModifier),
                                                panStart,
                                                true));
                canvas.applyPendingInput();
                expect (runtime.getContext().camera.centre.x != cameraCentreBeforePan.x
                        || runtime.getContext().camera.centre.y != cameraCentreBeforePan.y);
                expectEquals (b2World_GetCounters (worldId).jointCount, initialJointCount);
            }
        }

        beginTest ("Capacity overrides and bounded benchmarks");

        {
            Box2DSamples::Runtime runtime;
            const auto& entries = Box2DSamples::Catalog::getEntries();
            int numBenchmarks = 0;
            runtime.getContext().shouldUseReducedWorkload = true;

            for (int sampleIndex = 0; sampleIndex < (int) entries.size(); ++sampleIndex)
            {
                const auto& entry = entries[(size_t) sampleIndex];

                if (std::strcmp (entry.category, "Benchmark") != 0)
                    continue;

                ++numBenchmarks;
                expect (runtime.selectSample (sampleIndex).wasOk(), String::fromUTF8 (entry.name));
                runtime.getCurrentSample()->advanceSimulation();
                runtime.getDrawList().clear();
                runtime.getCurrentSample()->prepareFrame();
            }

            expect (numBenchmarks > 0);

            const auto capacitySampleIndex = findSampleIndex ("Benchmark", "Many Pyramids");
            expect (capacitySampleIndex.has_value());

            if (capacitySampleIndex.has_value())
            {
                const auto& capacityEntry = entries[(size_t) *capacitySampleIndex];
                expect (capacityEntry.getCapacity != nullptr);

                if (capacityEntry.getCapacity != nullptr)
                {
                    const auto expectedCapacity = capacityEntry.getCapacity();
                    expect (runtime.selectSample (*capacitySampleIndex).wasOk());
                    expectEquals (runtime.getContext().capacity.dynamicBodyCount, expectedCapacity.dynamicBodyCount);
                    expectEquals (runtime.getContext().capacity.dynamicShapeCount, expectedCapacity.dynamicShapeCount);
                }
            }
        }

        beginTest ("Determinism rollback");

        {
            Box2DSamples::Runtime runtime;
            Box2DSamples::ControlModel controlModel;
            PropertyPanel controlProperties;
            uint64 renderedControlRevision = 0;
            runtime.getContext().shouldUseReducedWorkload = true;
            expect (selectSample (runtime, "Determinism", "Rollback").wasOk());

            const auto refreshControlProperties = [&]
            {
                if (renderedControlRevision != controlModel.getStructureRevision())
                {
                    controlProperties.clear();
                    Box2DSamples::appendControlProperties (controlModel, controlProperties);
                    renderedControlRevision = controlModel.getStructureRevision();
                }

                controlProperties.refreshAll();
            };

            TextButton* rollbackButton = nullptr;

            for (int stepIndex = 0; stepIndex < 600 && rollbackButton == nullptr; ++stepIndex)
            {
                runtime.getCurrentSample()->advanceSimulation();

                if (stepIndex % 20 == 0)
                {
                    controlModel.beginFrame();
                    runtime.appendCurrentSampleControls (controlModel);
                    controlModel.endFrame();
                    refreshControlProperties();
                    rollbackButton = findButton (controlProperties, "Roll Back");
                }
            }

            expect (rollbackButton != nullptr);

            if (rollbackButton != nullptr)
            {
                if (rollbackButton->onClick != nullptr)
                    rollbackButton->onClick();

                controlModel.applyQueuedEdits();
                controlModel.beginFrame();
                runtime.appendCurrentSampleControls (controlModel);
                controlModel.endFrame();
                refreshControlProperties();

                for (int stepIndex = 0; stepIndex < 600 && ! containsLabelText (controlProperties, "match: hash"); ++stepIndex)
                {
                    runtime.getCurrentSample()->advanceSimulation();

                    if (stepIndex % 20 == 0)
                    {
                        controlModel.beginFrame();
                        runtime.appendCurrentSampleControls (controlModel);
                        controlModel.endFrame();
                        refreshControlProperties();
                    }
                }

                expect (containsLabelText (controlProperties, "match: hash"));
            }
        }

        beginTest ("Recording ownership, malformed input, and replay auxiliaries");

        const auto recording = createRecording ("Bodies", "Bad", 8);
        expect (recording.has_value());

        if (recording.has_value())
        {
            Box2DSamples::Runtime runtime;
            Box2DSamples::Canvas replayCanvas;
            runtime.getContext().shouldUseReducedWorkload = true;
            replayCanvas.setRuntime (&runtime);
            replayCanvas.setSize (960, 540);
            expect (selectSample (runtime, "Bodies", "Bad").wasOk());
            auto* previousSample = runtime.getCurrentSample();
            expect (! Box2DSamples::getReplayFrame (*previousSample).has_value());
            expect (! Box2DSamples::getReplayNumFrames (*previousSample).has_value());
            expect (! Box2DSamples::getReplayNumQueries (*previousSample).has_value());
            const MemoryBlock malformedData ("invalid", 7);
            expect (runtime.selectReplay (malformedData, "malformed.b2rec").failed());
            expect (runtime.getCurrentSample() == previousSample);
            auto releasedRecording = *recording;
            expect (runtime.selectReplay (releasedRecording, "owned.b2rec").wasOk());
            releasedRecording.reset();
            expect (Box2DSamples::isReplaySample (*runtime.getCurrentSample()));
            expect (b2World_IsValid (runtime.getCurrentSample()->getWorldId()));
            expect (runtime.getContext().homeView == nullptr);
            replayCanvas.resetView();
            const auto replayWorldBounds = b2World_GetBounds (runtime.getCurrentSample()->getWorldId());
            expectBoundsContain (runtime.getContext().camera.getVisibleBounds(), replayWorldBounds);
            const auto replayLandscapeZoom = runtime.getContext().camera.zoom;
            replayCanvas.setSize (540, 960);
            replayCanvas.resized();
            expect (runtime.getContext().camera.zoom > replayLandscapeZoom);
            expectBoundsContain (runtime.getContext().camera.getVisibleBounds(), replayWorldBounds);

            Box2DSamples::MetricsComponent metricsWithHiddenProfiler;
            metricsWithHiddenProfiler.setRuntime (&runtime);
            auto* embeddedTimeline = dynamic_cast<Slider*> (findComponentWithTitle (metricsWithHiddenProfiler, "Replay timeline"));
            expect (! metricsWithHiddenProfiler.isProfilerVisible());
            expect (embeddedTimeline == nullptr);

            Box2DSamples::ControlModel replayControlModel;
            replayControlModel.beginFrame();
            runtime.appendCurrentSampleControls (replayControlModel);
            replayControlModel.endFrame();
            const auto* replayTimelineItem = replayControlModel.findItem ("replayTimeline");
            expect (replayTimelineItem != nullptr);

            if (replayTimelineItem != nullptr)
            {
                expect (replayTimelineItem->kind == Box2DSamples::ControlItem::Kind::number);
                expectEquals (replayTimelineItem->labelText, String ("Replay timeline"));
                expectEquals (replayTimelineItem->helpText, String ("Selects the current replay frame."));
            }

            PropertyPanel replayControlProperties;
            Box2DSamples::appendControlProperties (replayControlModel, replayControlProperties);
            auto* replayTimelineProperty = findProperty<SliderPropertyComponent> (replayControlProperties, "Replay timeline");
            expect (replayTimelineProperty != nullptr);

            if (replayTimelineProperty != nullptr)
            {
                auto* replayTimelineSlider = findComponent<Slider> (*replayTimelineProperty);
                expect (replayTimelineSlider != nullptr);

                if (replayTimelineSlider != nullptr)
                    expectEquals (replayTimelineSlider->getDescription(), String ("Selects the current replay frame."));
            }

            auto inspector = runtime.getCurrentSample()->createInspectorComponent();
            auto replayMetrics = runtime.getCurrentSample()->createMetricsComponent();
            expect (inspector != nullptr);
            expect (replayMetrics != nullptr);

            if (inspector != nullptr)
                Box2DSamples::refreshReplayComponent (*inspector);

            if (replayMetrics != nullptr)
                Box2DSamples::refreshReplayComponent (*replayMetrics);

            if (inspector != nullptr)
            {
                auto* outline = dynamic_cast<TreeView*> (findComponentWithTitle (*inspector, "Replay scene outline"));
                expect (outline != nullptr);

                if (outline != nullptr)
                {
                    auto handler = outline->createAccessibilityHandler();
                    expect (handler != nullptr);

                    if (handler != nullptr)
                        expect (handler->getRole() == AccessibilityRole::tree);
                }
            }

            if (replayMetrics != nullptr)
            {
                auto* progress = dynamic_cast<ProgressBar*> (findComponentWithTitle (*replayMetrics, "Replay progress"));
                auto* timeline = dynamic_cast<Slider*> (findComponentWithTitle (*replayMetrics, "Replay timeline"));
                expect (progress != nullptr);
                expect (timeline == nullptr);

                if (progress != nullptr)
                {
                    auto handler = progress->createAccessibilityHandler();
                    expect (handler != nullptr);

                    if (handler != nullptr)
                        expect (handler->getRole() == AccessibilityRole::progressBar);
                }

            }

            replayMetrics.reset();
            inspector.reset();
            expect (selectSample (runtime, "Bodies", "Bad").wasOk());
        }

        beginTest ("Replay cadence, speed, transport, and recorded queries");

        const auto queryRecording = createRecording ("Determinism", "Falling Hinges", 40);
        expect (queryRecording.has_value());

        if (queryRecording.has_value())
        {
            Box2DSamples::Runtime runtime;
            expect (runtime.selectReplay (*queryRecording, "queries.b2rec").wasOk());
            auto* replay = runtime.getCurrentSample();
            Box2DSamples::queueReplaySeek (*replay, 10);
            Box2DSamples::applyReplayPendingChanges (*replay);
            expect (replay->handleKeyPress (KeyPress (',', ModifierKeys::shiftModifier, '<')));
            Box2DSamples::applyReplayPendingChanges (*replay);
            expect (Box2DSamples::getReplayFrame (*replay) == 5);
            expect (replay->handleKeyPress (KeyPress (',', ModifierKeys(), ',')));
            Box2DSamples::applyReplayPendingChanges (*replay);
            expect (Box2DSamples::getReplayFrame (*replay) == 4);
            Box2DSamples::queueReplaySeek (*replay, 0);
            Box2DSamples::applyReplayPendingChanges (*replay);
            const auto recordedInterval = replay->getStepIntervalSeconds();
            expect (recordedInterval > 0.0);
            runtime.getContext().settings.isPaused = false;
            runtime.resetPresentationClock();
            runtime.updateForPresentation (0.0);
            runtime.updateForPresentation (recordedInterval);
            expect (Box2DSamples::getReplayFrame (*replay) == 1);

            Box2DSamples::queueReplaySpeed (*replay, 2.0f);
            runtime.updateForPresentation (recordedInterval * 1.5);
            expect (Box2DSamples::getReplayFrame (*replay) == 2);

            Box2DSamples::queueReplaySeek (*replay, 0);
            runtime.updateForPresentation (recordedInterval * 2.0);
            expect (Box2DSamples::getReplayFrame (*replay) == 0);
            expect (runtime.getContext().settings.isPaused);

            runtime.getContext().settings.numSingleSteps = 1;
            runtime.updateForPresentation (recordedInterval * 3.0);
            expect (Box2DSamples::getReplayFrame (*replay) == 1);

            const auto numReplayFrames = Box2DSamples::getReplayNumFrames (*replay);
            expect (numReplayFrames.has_value());
            std::optional<int> frameWithQueries;

            for (int frame = 0; numReplayFrames.has_value() && frame <= *numReplayFrames; ++frame)
            {
                Box2DSamples::queueReplaySeek (*replay, frame);
                runtime.updateForPresentation (recordedInterval * (double) (frame + 4));

                if (const auto numQueries = Box2DSamples::getReplayNumQueries (*replay);
                    numQueries.has_value() && *numQueries > 0)
                {
                    frameWithQueries = frame;
                    break;
                }
            }

            expect (frameWithQueries.has_value());
            runtime.getDrawList().clear();
            replay->prepareFrame();
        }

        beginTest ("Asynchronous URL recording I/O and completion lifetime");

        if (recording.has_value())
        {
            const auto recordingFile = File::getSpecialLocation (File::tempDirectory)
                                           .getNonexistentChildFile ("juce-box2d-recording", ".b2rec");
            const URL recordingURL (recordingFile);
            WaitableEvent ioCompleted;
            std::optional<Box2DSamples::ReplayFileError> writeError;
            std::optional<Box2DSamples::ReplayFileReadResult> readResult;

            {
                ThreadPool threadPool (1);
                threadPool.addJob ([&]
                {
                    writeError = Box2DSamples::writeReplayFile (recordingURL, *recording);
                    readResult = Box2DSamples::readReplayFile (recordingURL);
                    ioCompleted.signal();
                });

                expect (ioCompleted.wait (5000));
            }

            expect (! writeError.has_value());
            expect (readResult.has_value());
            expect (readResult.has_value() && ! readResult->error.has_value());

            if (readResult.has_value())
                expect (readResult->data == *recording);

           #if JUCE_MODAL_LOOPS_PERMITTED
            WaitableEvent jobPosted,
                          callbackCompleted;
            bool completionSawDestroyedComponent = false,
                 lifetimeReadSucceeded = false;
            auto lifetimeComponent = std::make_unique<Component>();
            Component::SafePointer<Component> safeComponent (lifetimeComponent.get());

            {
                ThreadPool threadPool (1);
                threadPool.addJob ([&, safeComponent]
                {
                    const auto lifetimeRead = Box2DSamples::readReplayFile (recordingURL);
                    const auto readSucceeded = ! lifetimeRead.error.has_value();
                    MessageManager::callAsync ([&, safeComponent, readSucceeded]
                    {
                        completionSawDestroyedComponent = safeComponent == nullptr;
                        lifetimeReadSucceeded = readSucceeded;
                        callbackCompleted.signal();
                    });
                    jobPosted.signal();
                });

                lifetimeComponent.reset();
                expect (jobPosted.wait (5000));

                for (int attempt = 0; attempt < 20 && ! callbackCompleted.wait (0); ++attempt)
                    MessageManager::getInstance()->runDispatchLoopUntil (10);
            }

            expect (callbackCompleted.wait (0));
            expect (completionSawDestroyedComponent);
            expect (lifetimeReadSucceeded);
           #endif

            expect (recordingFile.deleteFile());
        }

        beginTest ("Linked catalogue parity and smoke");

        {
            Box2DSamples::Runtime runtime;
            Box2DSamples::ControlModel controlModel;
            Box2DSamples::Canvas canvas;
            const auto& entries = Box2DSamples::Catalog::getEntries();
            expectEquals ((int) entries.size(), g_sampleCount);
            runtime.getContext().shouldUseReducedWorkload = true;
            canvas.setSize (320, 240);
            canvas.setRuntime (&runtime);
            canvas.resized();

            for (int sampleIndex = 0; sampleIndex < (int) entries.size(); ++sampleIndex)
            {
                controlModel.clear();
                expect (runtime.selectSample (sampleIndex).wasOk(), String::fromUTF8 (entries[(size_t) sampleIndex].name));
                controlModel.beginFrame();
                runtime.appendCurrentSampleControls (controlModel);
                controlModel.endFrame();

                if (auto* sample = runtime.getCurrentSample())
                {
                    sample->advanceSimulation();
                    runtime.getDrawList().clear();
                    sample->prepareFrame();
                }

                Image image (Image::ARGB, 320, 240, true);
                Graphics graphics (image);
                canvas.paint (graphics);
            }

            controlModel.clear();
        }
    }
};

static Box2DSampleHostTests box2DSampleHostTests;

} // namespace
