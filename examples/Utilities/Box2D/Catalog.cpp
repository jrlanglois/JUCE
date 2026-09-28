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
#include "UpstreamBridge.h"
#include "sample.h"

namespace Box2DSamples::Catalog
{

namespace
{

std::vector<Entry> entries;
std::optional<int> replayIndex;
bool isInitialised = false;

template<size_t entryIndex>
std::unique_ptr<Sample> createUpstreamSample (Context& context)
{
    static_assert (entryIndex < MAX_SAMPLES);
    jassert ((int) entryIndex < g_sampleCount);
    return std::make_unique<UpstreamSampleAdapter> (context, g_sampleEntries[entryIndex].createFcn);
}

template<size_t... entryIndices>
constexpr auto makeUpstreamFactories (std::index_sequence<entryIndices...>) { return std::array<SampleCreateFunction, sizeof... (entryIndices)> { &createUpstreamSample<entryIndices>... }; }

constexpr auto upstreamFactories = makeUpstreamFactories (std::make_index_sequence<MAX_SAMPLES>());

} // namespace

int registerSample (const char* category, const char* name, SampleCreateFunction createSample, SampleCapacityFunction getCapacity, bool isReplayViewer)
{
    jassert (! isInitialised);
    entries.push_back ({ category, name, createSample, getCapacity, isReplayViewer });
    return (int) entries.size() - 1;
}

void initialise()
{
    if (isInitialised)
        return;

    entries.reserve ((size_t) g_sampleCount);

    for (int upstreamIndex = 0; upstreamIndex < g_sampleCount; ++upstreamIndex)
    {
        const auto& upstreamEntry = g_sampleEntries[upstreamIndex];
        const bool isReplayViewer = upstreamIndex == g_replayIndex;
        registerSample (upstreamEntry.category,
                        upstreamEntry.name,
                        isReplayViewer ? createReplaySample : upstreamFactories[(size_t) upstreamIndex],
                        upstreamEntry.capacityFcn,
                        isReplayViewer);
    }

    std::stable_sort (entries.begin(), entries.end(), [] (const Entry& first, const Entry& second)
    {
        const int categoryComparison = std::strcmp (first.category, second.category);
        return categoryComparison != 0 ? categoryComparison < 0 : std::strcmp (first.name, second.name) < 0;
    });

    for (size_t index = 0; index < entries.size(); ++index)
    {
        if (index > 0)
        {
            jassert (std::strcmp (entries[index - 1].category, entries[index].category) != 0
                     || std::strcmp (entries[index - 1].name, entries[index].name) != 0);
        }

        if (entries[index].isReplayViewer)
            replayIndex = (int) index;
    }

    isInitialised = true;
}

const std::vector<Entry>& getEntries() { return entries; }

std::optional<int> getReplayIndex() { return replayIndex; }

} // namespace Box2DSamples::Catalog
