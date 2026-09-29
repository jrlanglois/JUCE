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
#include "ReplaySample.h"

namespace Box2DSamples
{

class Runtime::Pimpl
{
public:
    static inline constexpr auto maxPresentationDeltaSeconds = 0.25;
    static inline constexpr auto maxAutomaticStepsPerPresentation = 8;

    DrawList drawList;
    Context context { drawList };
    std::unique_ptr<Sample> currentSample;
    double lastPresentationTimeSeconds = 0.0,
           elapsedAccumulatorSeconds = 0.0;
    int selectedSampleIndex = 0;
    bool hasPresentationClock = false;
};

Runtime::Runtime() : pimpl (std::make_unique<Pimpl>()) { Catalog::initialise(); }

Runtime::~Runtime() = default;

Result Runtime::selectSample (int sampleIndex, bool shouldRestart)
{
    const auto& entries = Catalog::getEntries();

    if (! isPositiveAndBelow (sampleIndex, (int) entries.size()))
        return Result::fail (TRANS ("Invalid sample index."));

    if (! shouldRestart)
    {
        const SimulationSettings defaultSettings;
        pimpl->context.camera.reset();
        pimpl->context.settings.numSubSteps = defaultSettings.numSubSteps;
        pimpl->context.settings.numRestitutionIterations = defaultSettings.numRestitutionIterations;
        pimpl->context.settings.isPaused = defaultSettings.isPaused;
        pimpl->context.settings.isRestitutionPropagationEnabled = defaultSettings.isRestitutionPropagationEnabled;
        pimpl->context.debugDraw.drawJoints = true;
    }

    pimpl->context.settings.numSingleSteps = 0;
    const auto& entry = entries[(size_t) sampleIndex];
    pimpl->context.capacity = entry.getCapacity != nullptr ? entry.getCapacity() : b2Capacity {};
    pimpl->context.shouldRestart = shouldRestart;

    auto sample = entry.createSample (pimpl->context);
    pimpl->context.shouldRestart = false;

    if (sample == nullptr)
        return Result::fail (TRANS ("Sample construction failed."));

    const Result creationResult = getReplaySampleCreationResult (*sample);

    if (creationResult.failed())
        return creationResult;

    if (! shouldRestart)
    {
        pimpl->context.homeView = entry.isReplayViewer ? nullptr : HomeViewCatalog::find (entry.category, entry.name);
        jassert (entry.isReplayViewer || pimpl->context.homeView != nullptr);
        sample->resetCamera();
    }

    pimpl->currentSample = std::move (sample);
    pimpl->selectedSampleIndex = sampleIndex;
    resetPresentationClock();
    return Result::ok();
}

Result Runtime::selectReplay (const MemoryBlock& recordingData, const String& displayName)
{
    if (recordingData.isEmpty())
        return Result::fail (TRANS ("The selected recording is empty."));

    const auto replayIndex = Catalog::getReplayIndex();

    if (! replayIndex.has_value())
        return Result::fail (TRANS ("Replay Viewer is not registered."));

    MemoryBlock previousReplayData = pimpl->context.replayData;
    String previousReplayName = pimpl->context.replayName;
    pimpl->context.replayData = recordingData;
    pimpl->context.replayName = displayName;
    const Result result = selectSample (*replayIndex);

    if (result.failed())
    {
        pimpl->context.replayData = std::move (previousReplayData);
        pimpl->context.replayName = std::move (previousReplayName);
    }

    return result;
}

Result Runtime::restartSample() { return selectSample (pimpl->selectedSampleIndex, true); }

void Runtime::updateForPresentation (double presentationTimeSeconds)
{
    if (pimpl->currentSample != nullptr)
        applyReplayPendingChanges (*pimpl->currentSample);

    if (! pimpl->hasPresentationClock || presentationTimeSeconds <= pimpl->lastPresentationTimeSeconds)
    {
        pimpl->lastPresentationTimeSeconds = presentationTimeSeconds;
        pimpl->elapsedAccumulatorSeconds = 0.0;
        pimpl->hasPresentationClock = true;
    }
    else if (pimpl->context.settings.isPaused)
    {
        pimpl->lastPresentationTimeSeconds = presentationTimeSeconds;
        pimpl->elapsedAccumulatorSeconds = 0.0;
    }
    else
    {
        const double elapsedSeconds = presentationTimeSeconds - pimpl->lastPresentationTimeSeconds;
        pimpl->lastPresentationTimeSeconds = presentationTimeSeconds;
        pimpl->elapsedAccumulatorSeconds += std::min (elapsedSeconds, Pimpl::maxPresentationDeltaSeconds);
    }

    if (pimpl->currentSample == nullptr)
        return;

    const double stepIntervalSeconds = pimpl->currentSample->getStepIntervalSeconds();
    int numAutomaticSteps = 0;

    if (! pimpl->context.settings.isPaused && stepIntervalSeconds > 0.0)
    {
        const int numElapsedSteps = (int) std::floor (pimpl->elapsedAccumulatorSeconds / stepIntervalSeconds);
        pimpl->elapsedAccumulatorSeconds -= (double) numElapsedSteps * stepIntervalSeconds;
        numAutomaticSteps = std::min (numElapsedSteps, Pimpl::maxAutomaticStepsPerPresentation);
    }

    while (pimpl->context.settings.numSingleSteps > 0)
    {
        pimpl->currentSample->advanceSimulation();
        --pimpl->context.settings.numSingleSteps;
    }

    for (int stepIndex = 0; stepIndex < numAutomaticSteps; ++stepIndex)
        pimpl->currentSample->advanceSimulation();

    pimpl->drawList.clear();
    pimpl->currentSample->prepareFrame();
}

void Runtime::resetPresentationClock() noexcept
{
    pimpl->hasPresentationClock = false;
    pimpl->elapsedAccumulatorSeconds = 0.0;
}

void Runtime::appendCurrentSampleControls (ControlModel& controls)
{
    if (pimpl->currentSample != nullptr)
        pimpl->currentSample->updateControls (controls);
}

void Runtime::startRecording()
{
    if (pimpl->currentSample != nullptr)
        pimpl->currentSample->startRecording();
}

std::optional<MemoryBlock> Runtime::stopRecording()
{
    if (pimpl->currentSample == nullptr)
        return std::nullopt;

    return pimpl->currentSample->stopRecording();
}

Sample* Runtime::getCurrentSample() noexcept { return pimpl->currentSample.get(); }

const Sample* Runtime::getCurrentSample() const noexcept { return pimpl->currentSample.get(); }

Context& Runtime::getContext() noexcept { return pimpl->context; }

DrawList& Runtime::getDrawList() noexcept { return pimpl->drawList; }

} // namespace Box2DSamples
