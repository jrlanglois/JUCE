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

#include "UpstreamBridge.h"
#include "host_controls.h"

extern "C" void box2dHostResetScreenTextY (float value);

namespace Box2DSamples
{

namespace
{

bool isHostKeyDown (void* userData, int keyCode)
{
    auto* hostContext = static_cast<Context*> (userData);
    return hostContext != nullptr && hostContext->isKeyDown (keyCode);
}

} // namespace

UpstreamBridge::UpstreamBridge (Context& hostContextIn)
    : hostContext (hostContextIn)
{
    sampleContext.Load();
    sampleContext.isKeyDown = isHostKeyDown;
    sampleContext.keyStateUserData = &hostContext;
    drawHandle = CreateDraw();
    sampleContext.draw = drawHandle;
    sampleContext.debugDraw = hostContext.debugDraw;
    sampleContext.capacity = hostContext.capacity;
}

UpstreamBridge::~UpstreamBridge()
{
    if (drawHandle != nullptr)
        DestroyDraw (drawHandle);
}

void UpstreamBridge::syncHostToUpstream (bool shouldRestart)
{
    sampleContext.restart = shouldRestart;
    sampleContext.hertz = hostContext.settings.hertz;
    sampleContext.recycleDistance = hostContext.settings.recycleDistance;
    sampleContext.subStepCount = hostContext.settings.numSubSteps;
    sampleContext.restitutionIterations = hostContext.settings.numRestitutionIterations;
    sampleContext.workerCount = hostContext.settings.numWorkers;
    sampleContext.pause = hostContext.settings.isPaused;
    sampleContext.singleStep = hostContext.settings.numSingleSteps;
    sampleContext.enableWarmStarting = hostContext.settings.isWarmStartingEnabled;
    sampleContext.enableContinuous = hostContext.settings.isContinuousCollisionEnabled;
    sampleContext.enableRestitutionPropagation = hostContext.settings.isRestitutionPropagationEnabled;
    sampleContext.enableSleep = hostContext.settings.isSleepingEnabled;
    sampleContext.reducedWorkload = hostContext.shouldUseReducedWorkload;
    sampleContext.capacity = hostContext.capacity;
    sampleContext.camera.center = hostContext.camera.centre;
    sampleContext.camera.zoom = hostContext.camera.zoom;

    if (hostContext.camera.drawableWidth > 0.0f && hostContext.camera.drawableHeight > 0.0f)
    {
        sampleContext.camera.width = hostContext.camera.drawableWidth;
        sampleContext.camera.height = hostContext.camera.drawableHeight;
    }

    sampleContext.homeCenter = hostContext.homeCameraCentre;
    sampleContext.homeZoom = hostContext.homeCameraZoom;
    sampleContext.debugDraw = hostContext.debugDraw;
}

void UpstreamBridge::syncHostFromUpstream()
{
    hostContext.settings.hertz = sampleContext.hertz;
    hostContext.settings.recycleDistance = sampleContext.recycleDistance;
    hostContext.settings.numSubSteps = sampleContext.subStepCount;
    hostContext.settings.numRestitutionIterations = sampleContext.restitutionIterations;
    hostContext.settings.numWorkers = sampleContext.workerCount;
    hostContext.settings.isPaused = sampleContext.pause;
    hostContext.settings.isWarmStartingEnabled = sampleContext.enableWarmStarting;
    hostContext.settings.isContinuousCollisionEnabled = sampleContext.enableContinuous;
    hostContext.settings.isRestitutionPropagationEnabled = sampleContext.enableRestitutionPropagation;
    hostContext.settings.isSleepingEnabled = sampleContext.enableSleep;
    hostContext.camera.centre = sampleContext.camera.center;
    hostContext.camera.zoom = sampleContext.camera.zoom;
    hostContext.homeCameraCentre = sampleContext.homeCenter;
    hostContext.homeCameraZoom = sampleContext.homeZoom;
    hostContext.debugDraw = sampleContext.debugDraw;
}

UpstreamSampleAdapter::UpstreamSampleAdapter (Context& hostContext, SampleCreateFcn* createSample)
    : Sample (hostContext, false),
      bridge (hostContext)
{
    jassert (createSample != nullptr);
    bridge.syncHostToUpstream (hostContext.shouldRestart);

    if (createSample != nullptr)
        upstreamSample.reset (createSample (&bridge.getSampleContext()));

    jassert (upstreamSample != nullptr);

    if (upstreamSample != nullptr)
    {
        bridge.getSampleContext().sample = upstreamSample.get();
        worldId = upstreamSample->GetWorldId();

        if (! hostContext.shouldRestart)
        {
            bridge.getSampleContext().homeCenter = bridge.getSampleContext().camera.center;
            bridge.getSampleContext().homeZoom = bridge.getSampleContext().camera.zoom;
        }

        bridge.syncHostFromUpstream();
    }
}

UpstreamSampleAdapter::~UpstreamSampleAdapter()
{
    worldId = b2_nullWorldId;
}

void UpstreamSampleAdapter::advanceSimulation()
{
    if (upstreamSample == nullptr)
        return;

    bridge.syncHostToUpstream (context.shouldRestart);
    context.shouldRestart = false;
    simulationPresentation.clear();
    setActiveHostDrawList (&simulationPresentation);
    box2dHostResetScreenTextY (24.0f);
    upstreamSample->ResetText();
    upstreamSample->Step();
    setActiveHostDrawList (nullptr);
    worldId = upstreamSample->GetWorldId();

    if (upstreamSample->DidStep())
        recordSimulationStep();

    bridge.syncHostFromUpstream();
}

void UpstreamSampleAdapter::prepareFrame()
{
    if (upstreamSample == nullptr)
        return;

    bridge.syncHostToUpstream (false);
    context.drawList.append (simulationPresentation);
    setActiveHostDrawList (&context.drawList);
    box2dHostResetScreenTextY (24.0f);
    upstreamSample->ResetText();
    upstreamSample->PreparePresentation();
    setActiveHostDrawList (nullptr);
    bridge.syncHostFromUpstream();
}

void UpstreamSampleAdapter::updateControls (ControlPanel& controls)
{
    if (upstreamSample == nullptr)
        return;

    controlsBridge.beginFrame (controls);
    upstreamSample->DrawControls();
    controlsBridge.endFrame();
}

bool UpstreamSampleAdapter::handleKeyPress (const KeyPress& key)
{
    if (upstreamSample == nullptr)
        return false;

    upstreamSample->Keyboard (key.getTextCharacter(), 0, 0);
    return true;
}

void UpstreamSampleAdapter::handleMouseDown (b2Pos position, MouseButton button, const ModifierKeys& modifiers)
{
    if (upstreamSample != nullptr)
        upstreamSample->MouseDown (position, mapMouseButton (button), mapModifiers (modifiers));
}

void UpstreamSampleAdapter::handleMouseUp (b2Pos position, MouseButton button)
{
    if (upstreamSample != nullptr)
        upstreamSample->MouseUp (position, mapMouseButton (button));
}

void UpstreamSampleAdapter::handleMouseMove (b2Pos position)
{
    if (upstreamSample != nullptr)
        upstreamSample->MouseMove (position);
}

void UpstreamSampleAdapter::resetCamera()
{
    Sample::resetCamera();
    bridge.syncHostToUpstream (false);
}

bool UpstreamSampleAdapter::hasSolverControls() const noexcept
{
    return upstreamSample != nullptr && upstreamSample->HasSolverControls();
}

bool UpstreamSampleAdapter::hasProfile() const noexcept
{
    return upstreamSample != nullptr && upstreamSample->HasProfile();
}

int UpstreamSampleAdapter::mapMouseButton (MouseButton button) noexcept
{
    switch (button)
    {
        case MouseButton::primary:   return HOST_MOUSE_BUTTON_PRIMARY;
        case MouseButton::secondary: return 2;
        case MouseButton::middle:    return 3;
    }

    return HOST_MOUSE_BUTTON_PRIMARY;
}

int UpstreamSampleAdapter::mapModifiers (const ModifierKeys& modifiers) noexcept
{
    int result = 0;

    if (modifiers.isShiftDown()) result |= 1;
    if (modifiers.isCtrlDown())  result |= 2;
    if (modifiers.isAltDown())   result |= 4;

    return result;
}

} // namespace Box2DSamples
