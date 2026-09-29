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

const HomeView homeViews[] =
{
    { "Benchmark", "Barrel", { { -96.444437663f, -5.749996185f }, { 112.444437663f, 111.749996185f } }, {} },
    { "Benchmark", "Barrel 2.4", { { -96.444437663f, -5.749996185f }, { 112.444437663f, 111.749996185f } }, {} },
    { "Benchmark", "Capacity", { { -355.555555556f, -50.000000000f }, { 355.555555556f, 350.000000000f } }, {} },
    { "Benchmark", "Cast", { { -433.333333333f, -25.000000000f }, { 1433.333333333f, 1025.000000000f } }, {} },
    { "Benchmark", "Compounds", { { -97.777777778f, -5.000000000f }, { 97.777777778f, 105.000000000f } }, {} },
    { "Benchmark", "CreateDestroy", { { -97.777777778f, -5.000000000f }, { 97.777777778f, 105.000000000f } }, {} },
    { "Benchmark", "Joint Grid", { { -51.111111111f, -119.500000000f }, { 171.111111111f, 5.500000000f } }, {} },
    { "Benchmark", "Junkyard", { { -98.666666667f, -35.000000000f }, { 114.666666667f, 85.000000000f } }, {} },
    { "Benchmark", "Kinematic", { { -266.666666667f, -150.000000000f }, { 266.666666667f, 150.000000000f } }, {} },
    { "Benchmark", "Large Compounds", { { -226.444444444f, -22.500000000f }, { 262.444444444f, 252.500000000f } }, {} },
    { "Benchmark", "Large Pyramid", { { -97.777777778f, -5.000000000f }, { 97.777777778f, 105.000000000f } }, {} },
    { "Benchmark", "Many Pyramids", { { -270.333333333f, -92.500000000f }, { 316.333333333f, 237.500000000f } }, {} },
    { "Benchmark", "Many Tumblers", { { -150.111111111f, -90.500000000f }, { 152.111111111f, 79.500000000f } }, {} },
    { "Benchmark", "Queries", { { -14.333333333f, -1.500000000f }, { 44.333333333f, 31.500000000f } }, {} },
    { "Benchmark", "Rain", { { -222.222222222f, -15.000000000f }, { 222.222222222f, 235.000000000f } }, {} },
    { "Benchmark", "Sensor", { { -222.222222222f, -20.000000000f }, { 222.222222222f, 230.000000000f } }, {} },
    { "Benchmark", "Shape Distance", { { -5.333333333f, -3.000000000f }, { 5.333333333f, 3.000000000f } }, {} },
    { "Benchmark", "Sleep", { { -266.666666667f, -130.000000000f }, { 266.666666667f, 170.000000000f } }, {} },
    { "Benchmark", "Smash", { { -11.111111111f, -34.000000000f }, { 131.111111111f, 46.000000000f } }, {} },
    { "Benchmark", "Spinner", { { -74.666666667f, -10.000000000f }, { 74.666666667f, 74.000000000f } }, {} },
    { "Benchmark", "Tumbler", { { -25.166666667f, -5.000000000f }, { 28.166666667f, 25.000000000f } }, {} },
    { "Benchmark", "Washer", { { -34.055555556f, -10.000000000f }, { 37.055555556f, 30.000000000f } }, {} },
    { "Bodies", "Bad", { { -19.922222270f, -2.500000000f }, { 24.522222175f, 22.500000000f } }, {} },
    { "Bodies", "Body Type", { { -16.977777766f, -3.599999905f }, { 18.577777790f, 16.400000095f } }, {} },
    { "Bodies", "Kinematic", { { -7.111111111f, -4.000000000f }, { 7.111111111f, 4.000000000f } }, {} },
    { "Bodies", "Mixed Locks", { { -6.222222222f, -1.000000000f }, { 6.222222222f, 6.000000000f } }, {} },
    { "Bodies", "Pivot", { { -16.977777766f, -3.599999905f }, { 18.577777790f, 16.400000095f } }, {} },
    { "Bodies", "Set Velocity", { { -6.222222222f, -1.000000000f }, { 6.222222222f, 6.000000000f } }, {} },
    { "Bodies", "Sleep", { { -94.777777778f, -5.000000000f }, { 100.777777778f, 105.000000000f } }, {} },
    { "Bodies", "Wake Touching", { { -14.222222222f, -4.000000000f }, { 14.222222222f, 12.000000000f } }, {} },
    { "Bodies", "Weeble", { { -19.922222270f, -2.500000000f }, { 24.522222175f, 22.500000000f } }, {} },
    { "Character", "Dynamic Mover", { { 2.222222222f, -1.000000000f }, { 37.777777778f, 19.000000000f } }, {} },
    { "Character", "Geometric Mover", { { 2.222222222f, -1.000000000f }, { 37.777777778f, 19.000000000f } }, {} },
    { "Character", "Ghost Collision", { { -21.333333333f, -10.500000000f }, { 21.333333333f, 13.500000000f } }, {} },
    { "Collision", "Cast World", { { -31.333333333f, -4.750000000f }, { 35.333333333f, 32.750000000f } }, {} },
    { "Collision", "Dynamic Tree", { { -433.333333333f, -25.000000000f }, { 1433.333333333f, 1025.000000000f } }, {} },
    { "Collision", "Manifold", { { -18.200000048f, -11.250000000f }, { 21.799999952f, 11.250000000f } }, {} },
    { "Collision", "Overlap World", { { -31.111111111f, -7.500000000f }, { 31.111111111f, 27.500000000f } }, {} },
    { "Collision", "Ray Cast", { { -31.111111111f, 2.500000000f }, { 31.111111111f, 37.500000000f } }, {} },
    { "Collision", "Shape Cast", { { -5.333333333f, -2.750000000f }, { 5.333333333f, 3.250000000f } }, {} },
    { "Collision", "Shape Distance", { { -5.333333333f, -3.000000000f }, { 5.333333333f, 3.000000000f } }, {} },
    { "Collision", "Smooth Manifold", { { -35.333333333f, -1.000000000f }, { 39.333333333f, 41.000000000f } }, {} },
    { "Collision", "Time of Impact", { { -16.800000000f, 44.200000000f }, { -15.200000000f, 45.800000000f } }, {} },
    { "Continuous", "Bounce House", { { -20.000000000f, -11.250000000f }, { 20.000000000f, 11.250000000f } }, {} },
    { "Continuous", "Bounce Humans", { { -21.333333333f, -12.000000000f }, { 21.333333333f, 12.000000000f } }, {} },
    { "Continuous", "Chain Drop", { { -15.555555556f, -8.750000000f }, { 15.555555556f, 8.750000000f } }, {} },
    { "Continuous", "Chain Slide", { { -26.666666667f, -5.000000000f }, { 26.666666667f, 25.000000000f } }, {} },
    { "Continuous", "Drop", { { -5.333333333f, -1.500000000f }, { 5.333333333f, 4.500000000f } }, {} },
    { "Continuous", "Ghost Bumps", { { -34.055555556f, -4.000000000f }, { 37.055555556f, 36.000000000f } }, {} },
    { "Continuous", "Pinball", { { -22.222222222f, -3.500000000f }, { 22.222222222f, 21.500000000f } }, {} },
    { "Continuous", "Pixel Imperfect", { { 4.500000000f, 4.000000000f }, { 8.000000000f, 10.500000000f } }, {} },
    { "Continuous", "Restitution Threshold", { { -3.666666667f, -1.000000000f }, { 17.666666667f, 11.000000000f } }, {} },
    { "Continuous", "Safety Factor", { { -10.666666667f, -3.000000000f }, { 10.666666667f, 9.000000000f } }, {} },
    { "Continuous", "Segment Slide", { { -26.666666667f, -5.000000000f }, { 26.666666667f, 25.000000000f } }, {} },
    { "Continuous", "Skinny Box", { { -10.111111111f, -1.250000000f }, { 12.111111111f, 11.250000000f } }, {} },
    { "Continuous", "Speculative Fallback", { { -10.111111111f, -1.250000000f }, { 12.111111111f, 11.250000000f } }, {} },
    { "Continuous", "Speculative Ghost", { { -3.555555556f, -0.250000000f }, { 3.555555556f, 3.750000000f } }, {} },
    { "Continuous", "Speculative Sliver", { { -4.444444444f, -0.750000000f }, { 4.444444444f, 4.250000000f } }, {} },
    { "Continuous", "Wedge", { { -10.666666667f, -0.500000000f }, { 10.666666667f, 11.500000000f } }, {} },
    { "Determinism", "Falling Hinges", { { -17.777777778f, -2.500000000f }, { 17.777777778f, 17.500000000f } }, {} },
    { "Determinism", "Rollback", { { -17.777777778f, -2.500000000f }, { 17.777777778f, 17.500000000f } }, {} },
    { "Events", "Body Move", { { -22.444444444f, -5.750000000f }, { 26.444444444f, 21.750000000f } }, {} },
    { "Events", "Circle Impulse", { { -6.044444614f, -0.700000048f }, { 6.044444614f, 6.100000143f } }, {} },
    { "Events", "Contact", { { -77.777777778f, -43.750000000f }, { 77.777777778f, 43.750000000f } }, {} },
    { "Events", "Foot Sensor", { { -13.333333333f, -1.500000000f }, { 13.333333333f, 13.500000000f } }, {} },
    { "Events", "Joint", { { -31.111111111f, -9.500000000f }, { 31.111111111f, 25.500000000f } }, {} },
    { "Events", "Persistent Contact", { { -13.333333333f, -1.500000000f }, { 13.333333333f, 13.500000000f } }, {} },
    { "Events", "Projectile Event", { { -31.888888889f, -5.000000000f }, { 17.888888889f, 23.000000000f } }, {} },
    { "Events", "Sensor Bookend", { { -13.333333333f, -1.500000000f }, { 13.333333333f, 13.500000000f } }, {} },
    { "Events", "Sensor Funnel", { { -59.244439019f, -33.324996948f }, { 59.244439019f, 33.324996948f } }, {} },
    { "Events", "Sensor Hits", { { -13.333333333f, -2.500000000f }, { 13.333333333f, 12.500000000f } }, {} },
    { "Events", "Sensor Types", { { -8.000000000f, -1.500000000f }, { 8.000000000f, 7.500000000f } }, {} },
    { "Geometry", "Convex Hull", { { -12.833334181f, -7.500000477f }, { 13.833334181f, 7.500000477f } }, {} },
    { "Issues", "Unstable Prismatic Joints", { { -7.000000000f, -1.000000000f }, { 7.000000000f, 7.000000000f } }, {} },
    { "Issues", "Unstable Windmill", { { -56.888888889f, -30.250000000f }, { 56.888888889f, 33.750000000f } }, {} },
    { "Joints", "Ball & Chain", { { -48.888888889f, -35.500000000f }, { 48.888888889f, 19.500000000f } }, {} },
    { "Joints", "Breakable", { { -31.111111111f, -9.500000000f }, { 31.111111111f, 25.500000000f } }, {} },
    { "Joints", "Bridge", { { -111.111111111f, -42.500000000f }, { 111.111111111f, 82.500000000f } }, {} },
    { "Joints", "Cantilever", { { -1.000000000f, -4.000000000f }, { 9.000000000f, 2.000000000f } }, {} },
    { "Joints", "Desk Lamp", { { -2.644444484f, -0.180000067f }, { 3.044444490f, 3.019999981f } }, {} },
    { "Joints", "Distance Joint", { { -1.000000000f, 17.000000000f }, { 4.000000000f, 22.000000000f } }, {} },
    { "Joints", "Doohickey", { { -15.555555556f, -3.750000000f }, { 15.555555556f, 13.750000000f } }, {} },
    { "Joints", "Door", { { -2.000000000f, -4.000000000f }, { 2.000000000f, 4.000000000f } }, {} },
    { "Joints", "Driving", { { -14.222222222f, -4.000000000f }, { 14.222222222f, 12.000000000f } }, {} },
    { "Joints", "Filter Joint", { { -17.777777778f, -3.000000000f }, { 17.777777778f, 17.000000000f } }, {} },
    { "Joints", "Gear Lift", { { -12.444444444f, -1.000000000f }, { 12.444444444f, 13.000000000f } }, {} },
    { "Joints", "Motion Locks", { { -15.000000000f, 5.000000000f }, { 15.000000000f, 15.000000000f } }, {} },
    { "Joints", "Motor Joint", { { -17.777777778f, -3.000000000f }, { 17.777777778f, 17.000000000f } }, {} },
    { "Joints", "Prismatic", { { -22.222222222f, -4.500000000f }, { 22.222222222f, 20.500000000f } }, {} },
    { "Joints", "Ragdoll", { { -6.000000000f, -4.000000000f }, { 6.000000000f, 28.000000000f } }, {} },
    { "Joints", "Revolute", { { -31.111111111f, -2.000000000f }, { 31.111111111f, 33.000000000f } }, {} },
    { "Joints", "Scale Ragdoll", { { -3.000000000f, -1.500000000f }, { 3.000000000f, 10.500000000f } }, {} },
    { "Joints", "Scissor Lift", { { -17.777777778f, -1.000000000f }, { 17.777777778f, 19.000000000f } }, {} },
    { "Joints", "Separation", { { -44.444444444f, -17.000000000f }, { 44.444444444f, 33.000000000f } }, {} },
    { "Joints", "Soft Body", { { -11.111111111f, -1.250000000f }, { 11.111111111f, 11.250000000f } }, {} },
    { "Joints", "Theo Jansen", { { -22.222222222f, -6.500000000f }, { 22.222222222f, 18.500000000f } }, {} },
    { "Joints", "Top Down Friction", { { -21.333333333f, -2.000000000f }, { 21.333333333f, 22.000000000f } }, {} },
    { "Joints", "User Constraint", { { -3.666667091f, -4.750000238f }, { 9.666667091f, 2.750000238f } }, {} },
    { "Joints", "Wheel", { { -6.666667091f, 6.249999762f }, { 6.666667091f, 13.750000238f } }, {} },
    { "Restitution", "Box Restitution", { { -5.000000000f, -2.000000000f }, { 5.000000000f, 10.000000000f } }, b2AABB { { -3.000000000f, -2.000000000f }, { 3.000000000f, 10.000000000f } } },
    { "Restitution", "Circle Restitution", { { -17.777777778f, -5.000000000f }, { 17.777777778f, 15.000000000f } }, {} },
    { "Restitution", "Circle Stack Restitution", { { -3.000000000f, -3.500000000f }, { 3.000000000f, 8.500000000f } }, {} },
    { "Restitution", "Restitution Propagation", { { -17.777777778f, -6.000000000f }, { 17.777777778f, 14.000000000f } }, {} },
    { "Restitution", "Rotated Box Restitution", { { -24.888888889f, -10.000000000f }, { 24.888888889f, 18.000000000f } }, {} },
    { "Restitution", "Single Box Restitution", { { -3.000000000f, -3.000000000f }, { 3.000000000f, 13.000000000f } }, {} },
    { "Restitution", "Single Circle Restitution", { { -3.000000000f, -3.000000000f }, { 3.000000000f, 13.000000000f } }, {} },
    { "Restitution", "Two Box Restitution", { { -3.000000000f, -2.000000000f }, { 3.000000000f, 8.000000000f } }, {} },
    { "Restitution", "Varying", { { -44.888888889f, -10.500000000f }, { 52.888888889f, 44.500000000f } }, {} },
    { "Robustness", "Cart", { { -2.666666667f, -0.500000000f }, { 2.666666667f, 2.500000000f } }, {} },
    { "Robustness", "HighMassRatio1", { { -41.444444444f, -11.000000000f }, { 47.444444444f, 39.000000000f } }, {} },
    { "Robustness", "HighMassRatio2", { { -44.444444444f, -8.500000000f }, { 44.444444444f, 41.500000000f } }, {} },
    { "Robustness", "HighMassRatio3", { { -44.444444444f, -8.500000000f }, { 44.444444444f, 41.500000000f } }, {} },
    { "Robustness", "Multiple Prismatic", { { -22.222222222f, -4.500000000f }, { 22.222222222f, 20.500000000f } }, {} },
    { "Robustness", "Overlap Recovery", { { -6.666666667f, -1.250000000f }, { 6.666666667f, 6.250000000f } }, {} },
    { "Robustness", "Tiny Pyramid", { { -1.777777778f, -0.199999988f }, { 1.777777778f, 1.800000012f } }, {} },
    { "Shapes", "Chain Link", { { -8.000000000f, -2.000000000f }, { 8.000000000f, 4.000000000f } }, {} },
    { "Shapes", "Chain Segment", { { -44.444444444f, -25.000000000f }, { 44.444444444f, 25.000000000f } }, {} },
    { "Shapes", "Chain Shape", { { -77.777777778f, -43.750000000f }, { 77.777777778f, 43.750000000f } }, {} },
    { "Shapes", "Compound Shapes", { { -22.222222222f, -6.500000000f }, { 22.222222222f, 18.500000000f } }, {} },
    { "Shapes", "Conveyor Belt", { { -19.333333333f, -4.500000000f }, { 23.333333333f, 19.500000000f } }, {} },
    { "Shapes", "Custom Filter", { { -17.777777778f, -5.000000000f }, { 17.777777778f, 15.000000000f } }, {} },
    { "Shapes", "Ellipse", { { -22.444444444f, -5.750000000f }, { 26.444444444f, 21.750000000f } }, {} },
    { "Shapes", "Explosion", { { -24.888888889f, -14.000000000f }, { 24.888888889f, 14.000000000f } }, {} },
    { "Shapes", "Filter", { { -22.222222222f, -7.500000000f }, { 22.222222222f, 17.500000000f } }, {} },
    { "Shapes", "Friction", { { -26.666668362f, -1.000000954f }, { 26.666668362f, 29.000000954f } }, {} },
    { "Shapes", "Modify Geometry", { { -11.111111111f, -1.250000000f }, { 11.111111111f, 11.250000000f } }, {} },
    { "Shapes", "Offset", { { 7.000000000f, -2.000000000f }, { 11.000000000f, 3.500000000f } }, {} },
    { "Shapes", "Recreate Static", { { -6.222222222f, -1.000000000f }, { 6.222222222f, 6.000000000f } }, {} },
    { "Shapes", "Rolling Resistance", { { -43.888888889f, -7.500000000f }, { 53.888888889f, 47.500000000f } }, {} },
    { "Shapes", "Rounded", { { -22.444444444f, -5.750000000f }, { 26.444444444f, 21.750000000f } }, {} },
    { "Shapes", "Tangent Speed", { { -7.555555556f, -53.000000000f }, { 127.555555556f, 23.000000000f } }, {} },
    { "Shapes", "Wind", { { -3.555555556f, -1.000000000f }, { 3.555555556f, 3.000000000f } }, {} },
    { "Stacking", "Arch", { { -6.000000000f, -0.750000000f }, { 6.000000000f, 16.750000000f } }, {} },
    { "Stacking", "Capsule Stack", { { -3.000000000f, -1.000000000f }, { 3.000000000f, 11.000000000f } }, {} },
    { "Stacking", "Card House", { { -1.472222222f, -0.350000024f }, { 2.972222222f, 2.149999976f } }, {} },
    { "Stacking", "Circle Stack", { { -3.000000000f, -1.000000000f }, { 3.000000000f, 11.000000000f } }, {} },
    { "Stacking", "Cliff", { { -22.222222222f, -7.500000000f }, { 22.222222222f, 17.500000000f } }, {} },
    { "Stacking", "Confined", { { -22.222222222f, -2.500000000f }, { 22.222222222f, 22.500000000f } }, {} },
    { "Stacking", "Double Domino", { { -11.111111111f, -2.250000000f }, { 11.111111111f, 10.250000000f } }, {} },
    { "Stacking", "Single Box", { { -6.222222222f, -1.000000000f }, { 6.222222222f, 6.000000000f } }, {} },
    { "Stacking", "Tilted Stack", { { -28.055555556f, -12.500000000f }, { 43.055555556f, 27.500000000f } }, {} },
    { "Stacking", "Vertical Stack", { { -31.888888889f, -5.000000000f }, { 17.888888889f, 23.000000000f } }, {} },
    { "World", "Far Gate", { { 999987.555555556f, -1.000000000f }, { 1000012.444444444f, 13.000000000f } }, {} },
    { "World", "Far Pyramid", { { 9999969.777777778f, -5.000000000f }, { 10000030.222222222f, 29.000000000f } }, {} },
    { "World", "Far Ragdolls", { { 9999982.222222222f, -4.000000000f }, { 10000017.777777778f, 16.000000000f } }, {} },
    { "World", "Tiles", { { -253.333333333f, -10.000000000f }, { -146.666666667f, 50.000000000f } }, {} }
};

} // namespace

b2AABB HomeView::getBoundsForAspectRatio (float aspectRatio) const noexcept
{
    if (aspectRatio > 0.0f && aspectRatio < 1.0f && portraitBounds.has_value())
        return *portraitBounds;

    return defaultBounds;
}

Span<const HomeView> HomeViewCatalog::getEntries() noexcept { return homeViews; }

const HomeView* HomeViewCatalog::find (const char* category, const char* sampleName) noexcept
{
    if (category == nullptr || sampleName == nullptr)
        return nullptr;

    for (const auto& homeView : homeViews)
    {
        if (std::strcmp (homeView.category, category) == 0 && std::strcmp (homeView.sampleName, sampleName) == 0)
            return &homeView;
    }

    return nullptr;
}

} // namespace Box2DSamples
