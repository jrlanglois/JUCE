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

namespace
{

const InteractionHint genericLiveHints[] =
{
    { Identifier ("zoomViewport"), NEEDS_TRANS ("Mouse wheel"), NEEDS_TRANS ("Zoom the viewport.") },
    { Identifier ("panViewport"), NEEDS_TRANS ("Middle drag"), NEEDS_TRANS ("Pan the viewport.") },
    { Identifier ("primaryInteraction"), NEEDS_TRANS ("Primary drag"), NEEDS_TRANS ("Move an interactive body.") },
    { Identifier ("resetViewport"), NEEDS_TRANS ("Home"), NEEDS_TRANS ("Reset the viewport.") }
};

const InteractionHint replayHints[] =
{
    { Identifier ("selectShape"), NEEDS_TRANS ("Primary click"), NEEDS_TRANS ("Select a shape.") },
    { Identifier ("clearSelection"), NEEDS_TRANS ("Escape"), NEEDS_TRANS ("Clear the shape selection.") },
    { Identifier ("previousFrame"), NEEDS_TRANS ("Comma"), NEEDS_TRANS ("Step back one frame.") },
    { Identifier ("previousFrames"), NEEDS_TRANS ("Shift + comma"), NEEDS_TRANS ("Step back five frames.") }
};

const SampleInteractionHelp entries[] =
{
    { "Replay", "Viewer", replayHints }
};

} // namespace

Span<const InteractionHint> SampleInteractionCatalog::getGenericLiveHints() noexcept { return genericLiveHints; }

Span<const SampleInteractionHelp> SampleInteractionCatalog::getEntries() noexcept { return entries; }

const SampleInteractionHelp* SampleInteractionCatalog::find (StringRef category, StringRef sampleName) noexcept
{
    for (const auto& entry : entries)
    {
        if (entry.category == category && entry.sampleName == sampleName)
            return &entry;
    }

    return nullptr;
}

} // namespace Box2DSamples
