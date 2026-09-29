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

#pragma once

#include "Box2DSamples.h"
#include "HostControlsBridge.h"
#include "sample.h"

namespace Box2DSamples
{

/** Bridges host `Context` state to one upstream `SampleContext`. */
class UpstreamBridge final
{
public:
    /** Constructs a bridge that forwards drawing into `hostContext`. */
    explicit UpstreamBridge (Context& hostContext);

    /** Destroys upstream draw resources. */
    ~UpstreamBridge();

    //==============================================================================
    /** Copies host simulation settings into the upstream context. */
    void syncHostToUpstream (bool shouldRestart);

    /** Copies upstream simulation and presentation settings back into the host context. */
    void syncHostFromUpstream();

    /** @returns the upstream context passed to imported samples. */
    [[nodiscard]] SampleContext& getSampleContext() noexcept { return sampleContext; }

private:
    //==============================================================================
    Context& hostContext;
    SampleContext sampleContext = {};
    Draw* drawHandle = nullptr;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UpstreamBridge)
};

/** Adapts one upstream `::Sample` to the native host `Sample` interface. */
class UpstreamSampleAdapter final : public Sample
{
public:
    /** Creates one upstream sample through `createSample`. */
    UpstreamSampleAdapter (Context& hostContext, SampleCreateFcn* createSample);

    /** Clears the borrowed world before the upstream sample destroys it. */
    ~UpstreamSampleAdapter() override;

    //==============================================================================
    /** @internal */
    void advanceSimulation() override;
    /** @internal */
    void prepareFrame() override;
    /** @internal */
    void updateControls (ControlModel& controls) override;
    /** @internal */
    bool handleKeyPress (const KeyPress& key) override;
    /** @internal */
    void handleMouseDown (b2Pos position, const ModifierKeys& modifiers) override;
    /** @internal */
    void handleMouseUp (b2Pos position, const ModifierKeys& modifiers) override;
    /** @internal */
    void handleMouseMove (b2Pos position, const ModifierKeys& modifiers) override;
    /** @internal */
    void resetCamera() override;
    /** @internal */
    bool hasSolverControls() const noexcept override;
    /** @internal */
    bool hasProfile() const noexcept override;

private:
    //==============================================================================
    UpstreamBridge bridge;
    HostControlsBridge controlsBridge;
    DrawList simulationPresentation;
    std::unique_ptr<::Sample> upstreamSample;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UpstreamSampleAdapter)
};

/** Installs the draw-list sink used by the upstream C draw helpers. */
void setActiveHostDrawList (DrawList* drawList) noexcept;

} // namespace Box2DSamples
