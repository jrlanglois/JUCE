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

class Sample::Pimpl
{
public:
    static constexpr size_t profileCapacity = 512;

    int numSteps = 0;
    b2Profile profiles[profileCapacity] = {};
    uint64 profileReadIndex = 0,
           profileWriteIndex = 0;
    b2Recording* recording = nullptr;
};

Sample::Sample (Context& newContext, bool shouldCreateWorld)
    : context (newContext),
      pimpl (std::make_unique<Pimpl>()),
      ownsWorld (shouldCreateWorld)
{
    if (shouldCreateWorld)
        createWorld();
}

Sample::~Sample()
{
    if (pimpl->recording != nullptr)
    {
        if (b2World_IsValid (worldId))
            b2World_StopRecording (worldId);

        b2DestroyRecording (pimpl->recording);
    }

    if (ownsWorld && b2World_IsValid (worldId))
        b2DestroyWorld (worldId);
}

double Sample::getStepIntervalSeconds() const noexcept
{
    return context.settings.hertz > 0.0f ? 1.0 / (double) context.settings.hertz : 0.0;
}

void Sample::advanceSimulation()
{
    if (! ownsWorld || ! b2World_IsValid (worldId))
        return;

    const float timeStepSeconds = (float) getStepIntervalSeconds();

    if (timeStepSeconds <= 0.0f)
        return;

    b2World_EnableSleeping (worldId, context.settings.isSleepingEnabled);
    b2World_EnableWarmStarting (worldId, context.settings.isWarmStartingEnabled);
    b2World_EnableContinuous (worldId, context.settings.isContinuousCollisionEnabled);
    b2World_SetRestitutionIterations (worldId, context.settings.numRestitutionIterations);
    b2World_EnableRestitutionPropagation (worldId, context.settings.isRestitutionPropagationEnabled);
    b2World_Step (worldId, timeStepSeconds, context.settings.numSubSteps);
    recordSimulationStep();
}

void Sample::prepareFrame() {}

void Sample::updateControls (ControlModel&) {}

bool Sample::handleKeyPress (const KeyPress&) { return false; }

void Sample::handleMouseDown (b2Pos, const ModifierKeys&) {}

void Sample::handleMouseUp (b2Pos, const ModifierKeys&) {}

void Sample::handleMouseMove (b2Pos, const ModifierKeys&) {}

void Sample::resetCamera()
{
    if (context.homeView != nullptr
        && context.camera.drawableWidth > 0.0f
        && context.camera.drawableHeight > 0.0f)
    {
        context.camera.setViewToBounds (context.homeView->getBoundsForAspectRatio (context.camera.getAspectRatio()));
        context.homeCameraCentre = context.camera.centre;
        context.homeCameraZoom = context.camera.zoom;
        return;
    }

    context.camera.centre = context.homeCameraCentre;
    context.camera.zoom = context.homeCameraZoom;
}

bool Sample::hasSolverControls() const noexcept { return true; }

bool Sample::hasProfile() const noexcept { return true; }

std::unique_ptr<Component> Sample::createInspectorComponent() { return {}; }

std::unique_ptr<Component> Sample::createMetricsComponent() { return {}; }

b2WorldId Sample::getWorldId() const noexcept
{
    return worldId;
}

std::vector<b2Profile> Sample::getProfileHistory() const
{
    std::vector<b2Profile> history;
    const auto numProfiles = (size_t) (pimpl->profileWriteIndex - pimpl->profileReadIndex);
    history.reserve (numProfiles);

    for (size_t index = 0; index < numProfiles; ++index)
    {
        const auto ringIndex = (size_t) (pimpl->profileReadIndex + index) % Pimpl::profileCapacity;
        history.push_back (pimpl->profiles[ringIndex]);
    }

    return history;
}

int Sample::getNumSteps() const noexcept
{
    return pimpl->numSteps;
}

void Sample::startRecording()
{
    if (pimpl->recording != nullptr || ! b2World_IsValid (worldId))
        return;

    pimpl->recording = b2CreateRecording (0);

    if (pimpl->recording != nullptr)
        b2World_StartRecording (worldId, pimpl->recording);
}

std::optional<MemoryBlock> Sample::stopRecording()
{
    if (pimpl->recording == nullptr)
        return std::nullopt;

    if (! b2World_IsValid (worldId))
    {
        b2DestroyRecording (pimpl->recording);
        pimpl->recording = nullptr;
        return std::nullopt;
    }

    b2World_StopRecording (worldId);
    const int numBytes = b2Recording_GetSize (pimpl->recording);
    const auto* data = b2Recording_GetData (pimpl->recording);
    std::optional<MemoryBlock> result;

    if (numBytes > 0 && data != nullptr)
        result.emplace (data, (size_t) numBytes);

    b2DestroyRecording (pimpl->recording);
    pimpl->recording = nullptr;
    return result;
}

void Sample::createWorld()
{
    if (! ownsWorld)
        return;

    if (b2World_IsValid (worldId))
    {
        if (pimpl->recording != nullptr)
        {
            const auto previousRecording = stopRecording();
            ignoreUnused (previousRecording);
        }

        b2DestroyWorld (worldId);
    }

    b2WorldDef worldDefinition = b2DefaultWorldDef();
    worldDefinition.restitutionIterations = context.settings.numRestitutionIterations;
    worldDefinition.enableRestitutionPropagation = context.settings.isRestitutionPropagationEnabled;
    worldDefinition.enableSleep = context.settings.isSleepingEnabled;
    worldDefinition.enableContinuous = context.settings.isContinuousCollisionEnabled;
    worldDefinition.workerCount = context.settings.numWorkers;
    worldDefinition.capacity = context.capacity;
    worldId = b2CreateWorld (&worldDefinition);

    if (b2World_IsValid (worldId))
    {
        b2World_SetContactRecycleDistance (worldId, context.settings.recycleDistance);
        b2World_EnableWarmStarting (worldId, context.settings.isWarmStartingEnabled);
    }
}

void Sample::recordSimulationStep()
{
    if (! b2World_IsValid (worldId))
        return;

    if (pimpl->profileWriteIndex - pimpl->profileReadIndex == Pimpl::profileCapacity)
        ++pimpl->profileReadIndex;

    const auto profileIndex = (size_t) pimpl->profileWriteIndex % Pimpl::profileCapacity;
    pimpl->profiles[profileIndex] = b2World_GetProfile (worldId);
    ++pimpl->profileWriteIndex;
    ++pimpl->numSteps;
}

void Sample::addScreenTextLine (const String& text)
{
    context.drawList.addScreenText ({ 8.0f, 24.0f }, b2_colorWhite, text);
}

} // namespace Box2DSamples
