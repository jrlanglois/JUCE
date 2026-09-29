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

namespace Box2DSamples
{

/** Creates the native sample used for the imported Replay Viewer entry. */
[[nodiscard]] std::unique_ptr<Sample> createReplaySample (Context& context);

/** @returns the replay creation result, or success for an ordinary sample. */
[[nodiscard]] Result getReplaySampleCreationResult (const Sample& sample);

/** Applies transport values queued by native replay components. */
void applyReplayPendingChanges (Sample& sample);

/** Refreshes a native replay component when `component` is one. */
void refreshReplayComponent (Component& component);

/** @returns whether `sample` is the native Replay Viewer. */
[[nodiscard]] bool isReplaySample (const Sample& sample) noexcept;

/** @returns the replay's current frame, or no value for an ordinary sample. */
[[nodiscard]] std::optional<int> getReplayFrame (const Sample& sample) noexcept;

/** @returns the replay's total frame quantity, or no value for an ordinary sample. */
[[nodiscard]] std::optional<int> getReplayNumFrames (const Sample& sample) noexcept;

/** @returns the current replay frame's recorded query quantity, or no value for an ordinary sample. */
[[nodiscard]] std::optional<int> getReplayNumQueries (const Sample& sample) noexcept;

/** Queues a replay seek for the next presentation boundary. */
void queueReplaySeek (Sample& sample, int frame);

/** Queues replay play or pause state for the next presentation boundary. */
void queueReplayPlaying (Sample& sample, bool shouldPlay);

/** Queues a positive replay speed multiplier for the next presentation boundary. */
void queueReplaySpeed (Sample& sample, float speedMultiplier);

} // namespace Box2DSamples
