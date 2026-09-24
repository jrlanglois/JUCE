/*
  ==============================================================================

   This file is part of the JUCE framework.
   Copyright (c) Raw Material Software Limited

   JUCE is an open source framework subject to commercial or open source
   licensing.

   By downloading, installing, or using the JUCE framework, or combining the
   JUCE framework with any other source code, object code, content or any other
   copyrightable work, you agree to the terms of the JUCE End User Licence
   Agreement, and all incorporated terms including the JUCE Privacy Policy and
   the JUCE Website Terms of Service, as applicable, which will bind you. If you
   do not agree to the terms of these agreements, we will not license the JUCE
   framework to you, and you must discontinue the installation or download
   process and cease use of the JUCE framework.

   JUCE End User Licence Agreement: https://juce.com/legal/juce-9-licence/
   JUCE Privacy Policy: https://juce.com/juce-privacy-policy
   JUCE Website Terms of Service: https://juce.com/juce-website-terms-of-service/

   Or:

   You may also use this code under the terms of the AGPLv3:
   https://www.gnu.org/licenses/agpl-3.0.en.html

   THE JUCE FRAMEWORK IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL
   WARRANTIES, WHETHER EXPRESSED OR IMPLIED, INCLUDING WARRANTY OF
   MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE, ARE DISCLAIMED.

  ==============================================================================
*/

namespace juce::detail
{

struct DirectionalFocusScore
{
    bool isOutsideBeam = false;
    float majorAxisDistance = 0.0f;
    float minorAxisDistance = 0.0f;

    bool operator< (const DirectionalFocusScore& other) const
    {
        return std::tie (isOutsideBeam, majorAxisDistance, minorAxisDistance)
             < std::tie (other.isOutsideBeam, other.majorAxisDistance, other.minorAxisDistance);
    }
};

static std::optional<FocusNavigationDirection> getFocusNavigationDirectionForKeyPress (const KeyPress& key)
{
    if (key.isKeyCode (KeyPress::leftKey))
        return FocusNavigationDirection::left;

    if (key.isKeyCode (KeyPress::rightKey))
        return FocusNavigationDirection::right;

    if (key.isKeyCode (KeyPress::upKey))
        return FocusNavigationDirection::up;

    if (key.isKeyCode (KeyPress::downKey))
        return FocusNavigationDirection::down;

    return std::nullopt;
}

static bool focusIntervalsOverlap (float firstStart, float firstEnd,
                                   float secondStart, float secondEnd)
{
    return firstStart < secondEnd && secondStart < firstEnd;
}

[[maybe_unused]] static std::optional<DirectionalFocusScore> getDirectionalFocusScore (
    Rectangle<float> source,
    Rectangle<float> candidate,
    FocusNavigationDirection direction)
{
    const auto sourceCentre = source.getCentre();
    const auto candidateCentre = candidate.getCentre();

    switch (direction)
    {
        case FocusNavigationDirection::left:
            if (candidateCentre.x < sourceCentre.x)
                return DirectionalFocusScore
                {
                    ! focusIntervalsOverlap (source.getY(), source.getBottom(),
                                             candidate.getY(), candidate.getBottom()),
                    jmax (0.0f, source.getX() - candidate.getRight()),
                    std::abs (sourceCentre.y - candidateCentre.y)
                };

            break;

        case FocusNavigationDirection::right:
            if (sourceCentre.x < candidateCentre.x)
                return DirectionalFocusScore
                {
                    ! focusIntervalsOverlap (source.getY(), source.getBottom(),
                                             candidate.getY(), candidate.getBottom()),
                    jmax (0.0f, candidate.getX() - source.getRight()),
                    std::abs (sourceCentre.y - candidateCentre.y)
                };

            break;

        case FocusNavigationDirection::up:
            if (candidateCentre.y < sourceCentre.y)
                return DirectionalFocusScore
                {
                    ! focusIntervalsOverlap (source.getX(), source.getRight(),
                                             candidate.getX(), candidate.getRight()),
                    jmax (0.0f, source.getY() - candidate.getBottom()),
                    std::abs (sourceCentre.x - candidateCentre.x)
                };

            break;

        case FocusNavigationDirection::down:
            if (sourceCentre.y < candidateCentre.y)
                return DirectionalFocusScore
                {
                    ! focusIntervalsOverlap (source.getX(), source.getRight(),
                                             candidate.getX(), candidate.getRight()),
                    jmax (0.0f, candidate.getY() - source.getBottom()),
                    std::abs (sourceCentre.x - candidateCentre.x)
                };

            break;
    }

    return std::nullopt;
}

template <typename Iterator, typename GetBounds>
Iterator findDirectionalFocusCandidate (Rectangle<float> sourceBounds,
                                        Iterator begin,
                                        Iterator end,
                                        FocusNavigationDirection direction,
                                        GetBounds&& getBounds)
{
    auto best = end;
    std::optional<DirectionalFocusScore> bestScore;

    for (auto candidate = begin; candidate != end; ++candidate)
    {
        const auto score = getDirectionalFocusScore (sourceBounds, getBounds (*candidate), direction);

        if (score.has_value() && (! bestScore.has_value() || *score < *bestScore))
        {
            best = candidate;
            bestScore = score;
        }
    }

    return best;
}

} // namespace juce::detail
