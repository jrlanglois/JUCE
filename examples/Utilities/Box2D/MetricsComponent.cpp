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

namespace
{

class ProfileChart final : public Component
{
public:
    ProfileChart()
    {
        setTitle (TRANS ("Step profile history"));
        setDescription (TRANS ("No step profile data is available."));
    }

    void setHistory (std::vector<b2Profile> newHistory)
    {
        history = std::move (newHistory);

        if (history.empty())
        {
            setDescription (TRANS ("No step profile data is available."));
        }
        else
        {
            const auto& latest = history.back();
            double totalStepMilliseconds = 0.0,
                   totalCollisionMilliseconds = 0.0,
                   totalSolveMilliseconds = 0.0;

            for (const auto& profile : history)
            {
                totalStepMilliseconds += (double) profile.step;
                totalCollisionMilliseconds += (double) profile.collide;
                totalSolveMilliseconds += (double) profile.solve;
            }

            const auto numProfiles = (double) history.size();
            setDescription (TRANS ("Step profile. Latest step: {latestStepMilliseconds} milliseconds; collision: {latestCollisionMilliseconds} milliseconds; solve: {latestSolveMilliseconds} milliseconds. Average step: {averageStepMilliseconds} milliseconds; collision: {averageCollisionMilliseconds} milliseconds; solve: {averageSolveMilliseconds} milliseconds.")
                                .replace ("{latestStepMilliseconds}", String (latest.step, 3))
                                .replace ("{latestCollisionMilliseconds}", String (latest.collide, 3))
                                .replace ("{latestSolveMilliseconds}", String (latest.solve, 3))
                                .replace ("{averageStepMilliseconds}", String (totalStepMilliseconds / numProfiles, 3))
                                .replace ("{averageCollisionMilliseconds}", String (totalCollisionMilliseconds / numProfiles, 3))
                                .replace ("{averageSolveMilliseconds}", String (totalSolveMilliseconds / numProfiles, 3)));
        }

        invalidateAccessibilityHandler();
        repaint();
    }

    void paint (Graphics& graphics) override
    {
        graphics.fillAll (Colours::darkgrey);
        auto bounds = getLocalBounds().toFloat().reduced (6.0f);

        if (history.empty() || bounds.isEmpty())
        {
            graphics.setColour (Colours::white);
            graphics.drawFittedText (TRANS ("No profile data"), getLocalBounds(), Justification::centred, 1);
            return;
        }

        auto maxMilliseconds = 0.001f;

        for (const auto& profile : history)
            maxMilliseconds = std::max (maxMilliseconds, profile.step);

        drawSeries (graphics, bounds, maxMilliseconds, Colours::white, [] (const b2Profile& profile) { return profile.step; });
        drawSeries (graphics, bounds, maxMilliseconds, Colours::orange, [] (const b2Profile& profile) { return profile.collide; });
        drawSeries (graphics, bounds, maxMilliseconds, Colours::lightgreen, [] (const b2Profile& profile) { return profile.solve; });

        graphics.setColour (Colours::white);
        graphics.drawText (TRANS ("Step"), bounds.removeFromTop (18.0f), Justification::topLeft);
        graphics.setColour (Colours::orange);
        graphics.drawText (TRANS ("Collision"), bounds.removeFromTop (18.0f), Justification::topLeft);
        graphics.setColour (Colours::lightgreen);
        graphics.drawText (TRANS ("Solve"), bounds.removeFromTop (18.0f), Justification::topLeft);
    }

    std::unique_ptr<AccessibilityHandler> createAccessibilityHandler() override { return std::make_unique<AccessibilityHandler> (*this, AccessibilityRole::image); }

private:
    std::vector<b2Profile> history;

    template<typename ValueFunction>
    void drawSeries (Graphics& graphics, juce::Rectangle<float> bounds, float maxMilliseconds, Colour colour, ValueFunction getValue)
    {
        Path path;

        for (size_t index = 0; index < history.size(); ++index)
        {
            const auto proportion = history.size() > 1 ? (float) index / (float) (history.size() - 1) : 0.0f;
            const auto x = bounds.getX() + proportion * bounds.getWidth();
            const auto y = bounds.getBottom() - std::clamp (getValue (history[index]) / maxMilliseconds, 0.0f, 1.0f) * bounds.getHeight();

            if (index == 0)
                path.startNewSubPath (x, y);
            else
                path.lineTo (x, y);
        }

        graphics.setColour (colour);
        graphics.strokePath (path, PathStrokeType (1.5f));
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ProfileChart)
};

} // namespace

class MetricsComponent::Pimpl
{
public:
    Runtime* runtime = nullptr;
    std::unique_ptr<Component> sampleMetrics;
    Label stepLabel,
          counterLabel,
          profileLabel;
    ProfileChart profileChart;
    bool profilerVisible = false;
};

MetricsComponent::MetricsComponent() : pimpl (std::make_unique<Pimpl>())
{
    setOpaque (true);
    setTitle (TRANS ("Box2D metrics"));
    pimpl->stepLabel.setTitle (TRANS ("Simulation step count"));
    pimpl->counterLabel.setTitle (TRANS ("World counters"));
    pimpl->profileLabel.setTitle (TRANS ("Latest step profile"));
    pimpl->counterLabel.setJustificationType (Justification::topLeft);
    pimpl->profileLabel.setJustificationType (Justification::centredLeft);
    pimpl->counterLabel.setMinimumHorizontalScale (0.7f);
    pimpl->profileLabel.setMinimumHorizontalScale (0.7f);
    addAndMakeVisible (pimpl->stepLabel);
    addAndMakeVisible (pimpl->counterLabel);
    addAndMakeVisible (pimpl->profileLabel);
    addAndMakeVisible (pimpl->profileChart);
    refresh();
}

MetricsComponent::~MetricsComponent() = default;

void MetricsComponent::setRuntime (Runtime* newRuntime)
{
    pimpl->sampleMetrics.reset();
    pimpl->runtime = newRuntime;

    if (pimpl->runtime != nullptr)
    {
        if (auto* sample = pimpl->runtime->getCurrentSample())
        {
            pimpl->sampleMetrics = sample->createMetricsComponent();

            if (pimpl->sampleMetrics != nullptr)
                addAndMakeVisible (*pimpl->sampleMetrics);
        }
    }

    resized();
    refresh();
}

void MetricsComponent::setProfilerVisible (bool shouldShowProfiler)
{
    if (pimpl->profilerVisible == shouldShowProfiler)
        return;

    pimpl->profilerVisible = shouldShowProfiler;
    refresh();
    invalidateAccessibilityHandler();
}

bool MetricsComponent::isProfilerVisible() const noexcept { return pimpl->profilerVisible; }

void MetricsComponent::refresh()
{
    const auto* sample = pimpl->runtime != nullptr ? pimpl->runtime->getCurrentSample() : nullptr;
    pimpl->stepLabel.setVisible (pimpl->profilerVisible);

    if (sample == nullptr)
    {
        pimpl->stepLabel.setText (TRANS ("No sample is selected."), dontSendNotification);
        pimpl->counterLabel.setVisible (false);
        pimpl->profileLabel.setVisible (false);
        pimpl->profileChart.setVisible (false);
        pimpl->profileChart.setHistory ({});
    }
    else
    {
        pimpl->stepLabel.setText (TRANS ("Steps: {numSteps}.").replace ("{numSteps}", String (sample->getNumSteps())),
                                  dontSendNotification);

        const auto worldId = sample->getWorldId();
        const auto hasWorld = b2World_IsValid (worldId);
        pimpl->counterLabel.setVisible (pimpl->profilerVisible && hasWorld);

        if (pimpl->profilerVisible && hasWorld)
        {
            const auto counters = b2World_GetCounters (worldId);
            const auto capacity = b2World_GetMaxCapacity (worldId);
            const auto bodyCapacity = capacity.staticBodyCount + capacity.dynamicBodyCount,
                       shapeCapacity = capacity.staticShapeCount + capacity.dynamicShapeCount;
            pimpl->counterLabel.setText (TRANS ("Bodies: {numBodies} of {bodyCapacity}. Shapes: {numShapes} of {shapeCapacity}.\nContacts: {numContacts}. Joints: {numJoints}. Islands: {numIslands}.\nTasks: {numTasks}. Memory: {numKilobytes} KB.")
                                               .replace ("{numBodies}", String (counters.bodyCount))
                                               .replace ("{bodyCapacity}", String (bodyCapacity))
                                               .replace ("{numShapes}", String (counters.shapeCount))
                                               .replace ("{shapeCapacity}", String (shapeCapacity))
                                               .replace ("{numContacts}", String (counters.contactCount))
                                               .replace ("{numJoints}", String (counters.jointCount))
                                               .replace ("{numIslands}", String (counters.islandCount))
                                               .replace ("{numTasks}", String (counters.taskCount))
                                               .replace ("{numKilobytes}", String (counters.byteCount / 1024)),
                                           dontSendNotification);
        }

        const auto history = pimpl->profilerVisible && sample->hasProfile() ? sample->getProfileHistory() : std::vector<b2Profile>();
        const auto hasProfile = ! history.empty();
        pimpl->profileLabel.setVisible (hasProfile);
        pimpl->profileChart.setVisible (hasProfile);
        pimpl->profileChart.setHistory (history);

        if (hasProfile)
        {
            const auto& latest = history.back();
            pimpl->profileLabel.setText (TRANS ("Latest profile: step {stepMilliseconds} ms; collision {collisionMilliseconds} ms; solve {solveMilliseconds} ms.")
                                                .replace ("{stepMilliseconds}", String (latest.step, 3))
                                                .replace ("{collisionMilliseconds}", String (latest.collide, 3))
                                                .replace ("{solveMilliseconds}", String (latest.solve, 3)),
                                          dontSendNotification);
        }
    }

    if (pimpl->sampleMetrics != nullptr)
        refreshReplayComponent (*pimpl->sampleMetrics);

    resized();
    repaint();
}

void MetricsComponent::paint (Graphics& graphics)
{
    graphics.fillAll (Colours::darkgrey);
}

void MetricsComponent::resized()
{
    auto bounds = getLocalBounds().reduced (4);

    if (pimpl->stepLabel.isVisible())
        pimpl->stepLabel.setBounds (bounds.removeFromTop (24));

    if (pimpl->counterLabel.isVisible())
        pimpl->counterLabel.setBounds (bounds.removeFromTop (64));

    juce::Rectangle<int> sampleMetricsBounds;

    if (pimpl->sampleMetrics != nullptr)
        sampleMetricsBounds = pimpl->profilerVisible ? bounds.removeFromBottom (std::min (190, bounds.getHeight() / 2)) : bounds;

    if (pimpl->profileLabel.isVisible())
        pimpl->profileLabel.setBounds (bounds.removeFromTop (24));

    if (pimpl->profileChart.isVisible())
        pimpl->profileChart.setBounds (bounds);

    if (pimpl->sampleMetrics != nullptr)
        pimpl->sampleMetrics->setBounds (sampleMetricsBounds);
}

std::unique_ptr<AccessibilityHandler> MetricsComponent::createAccessibilityHandler() { return std::make_unique<AccessibilityHandler> (*this, AccessibilityRole::group); }

} // namespace Box2DSamples
