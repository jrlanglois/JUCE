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

namespace Box2DSamples
{

Canvas::Canvas()
{
    setOpaque (true);
    setWantsKeyboardFocus (true);
}

void Canvas::setRuntime (Runtime* newRuntime) noexcept { runtime = newRuntime; }

void Canvas::resetView()
{
    shouldRefitHomeViewOnResize = true;
    homeRequested = false;

    if (runtime != nullptr)
    {
        if (auto* sample = runtime->getCurrentSample())
            sample->resetCamera();
    }
}

void Canvas::applyPendingInput()
{
    if (runtime == nullptr)
        return;

    auto& camera = runtime->getContext().camera;

    if (homeRequested)
        resetView();

    if (pendingZoomFactor != 1.0f)
    {
        shouldRefitHomeViewOnResize = false;
        camera.zoom *= pendingZoomFactor;
        camera.zoom = std::clamp (camera.zoom, 0.05f, 100.0f);
        pendingZoomFactor = 1.0f;
    }

    if (pendingPanDelta != juce::Point<float>())
    {
        const b2Vec2 viewSize = camera.getViewSize();
        const auto targetArea = getDrawableArea();

        if (! targetArea.isEmpty())
        {
            shouldRefitHomeViewOnResize = false;
            camera.centre.x -= pendingPanDelta.x * viewSize.x / targetArea.getWidth();
            camera.centre.y += pendingPanDelta.y * viewSize.y / targetArea.getHeight();
        }

        pendingPanDelta = {};
    }

    auto* sample = runtime->getCurrentSample();

    if (sample == nullptr)
    {
        pendingKeyPresses.clear();
        pendingPointerEvents.clear();
        return;
    }

    for (const auto& key : pendingKeyPresses)
        sample->handleKeyPress (key);

    pendingKeyPresses.clear();

    const auto targetArea = getDrawableArea();

    for (const auto& event : pendingPointerEvents)
    {
        const b2Pos worldPosition = camera.convertComponentToWorld (event.position, targetArea);

        switch (event.type)
        {
            case PointerEventType::down:
                sample->handleMouseDown (worldPosition, event.button, event.modifiers);
                break;

            case PointerEventType::up:
                sample->handleMouseUp (worldPosition, event.button);
                break;

            case PointerEventType::move:
                sample->handleMouseMove (worldPosition);
                break;
        }
    }

    pendingPointerEvents.clear();
}

void Canvas::updateAccessibility (const String& sampleName, const String& instructions)
{
    if (getTitle() == sampleName && getDescription() == instructions)
        return;

    setTitle (sampleName);
    setDescription (instructions);
    invalidateAccessibilityHandler();

    if (auto* handler = getAccessibilityHandler())
        handler->notifyAccessibilityEvent (AccessibilityEvent::titleChanged);
}

void Canvas::paint (Graphics& graphics)
{
    graphics.fillAll (Colours::black);

    if (runtime == nullptr)
        return;

    const auto* sample = runtime->getCurrentSample();

    if (sample == nullptr)
        return;

    auto& camera = runtime->getContext().camera;
    const auto targetArea = getDrawableArea();
    const b2Vec2 viewSize = camera.getViewSize();
    const b2WorldId worldId = sample->getWorldId();

    if (b2World_IsValid (worldId))
    {
        renderer.getDebugDraw() = runtime->getContext().debugDraw;
        renderer.render (graphics, worldId, camera.centre, viewSize, targetArea);
    }

    runtime->getDrawList().render (graphics, camera, targetArea);
}

void Canvas::resized()
{
    if (runtime == nullptr)
        return;

    const auto targetArea = getDrawableArea();
    auto& camera = runtime->getContext().camera;
    camera.setDrawableSize (targetArea.getWidth(), targetArea.getHeight());

    if (shouldRefitHomeViewOnResize && ! targetArea.isEmpty())
    {
        if (auto* sample = runtime->getCurrentSample())
            sample->resetCamera();
    }
}

bool Canvas::keyPressed (const KeyPress& key)
{
    if (runtime == nullptr)
        return false;

    if (key == KeyPress::homeKey)
    {
        homeRequested = true;
        return true;
    }

    pendingKeyPresses.push_back (key);
    return true;
}

void Canvas::mouseDown (const MouseEvent& event)
{
    if (runtime == nullptr)
        return;

    if (event.mods.isMiddleButtonDown())
    {
        panActive = true;
        shouldRefitHomeViewOnResize = false;
        lastPanPosition = event.position;
        return;
    }

    MouseButton button = MouseButton::primary;

    if (event.mods.isRightButtonDown())
        button = MouseButton::secondary;

    activePointerButton = button;
    pendingPointerEvents.push_back ({ PointerEventType::down, event.position, button, event.mods });
}

void Canvas::mouseDrag (const MouseEvent& event)
{
    if (runtime == nullptr)
        return;

    if (panActive)
    {
        const auto delta = event.position - lastPanPosition;
        lastPanPosition = event.position;
        pendingPanDelta += delta;
        return;
    }

    pendingPointerEvents.push_back ({ PointerEventType::move, event.position, MouseButton::primary, {} });
}

void Canvas::mouseUp (const MouseEvent& event)
{
    if (runtime == nullptr)
        return;

    if (panActive)
    {
        panActive = false;
        return;
    }

    pendingPointerEvents.push_back ({ PointerEventType::up, event.position, activePointerButton, {} });
}

void Canvas::mouseMove (const MouseEvent& event)
{
    if (runtime == nullptr || panActive)
        return;

    pendingPointerEvents.push_back ({ PointerEventType::move, event.position, MouseButton::primary, {} });
}

void Canvas::mouseWheelMove (const MouseEvent& event, const MouseWheelDetails& wheel)
{
    ignoreUnused (event);

    if (wheel.deltaY != 0.0f)
        shouldRefitHomeViewOnResize = false;

    pendingZoomFactor *= std::max (0.1f, 1.0f - 0.1f * (float) wheel.deltaY);
}

juce::Rectangle<float> Canvas::getDrawableArea() const noexcept { return getLocalBounds().toFloat().reduced (8.0f); }

std::unique_ptr<AccessibilityHandler> Canvas::createAccessibilityHandler()
{
    AccessibilityActions actions;
    actions.addAction (AccessibilityActionType::focus, [this] { grabKeyboardFocus(); });
    return std::make_unique<AccessibilityHandler> (*this, AccessibilityRole::image, std::move (actions));
}

} // namespace Box2DSamples
