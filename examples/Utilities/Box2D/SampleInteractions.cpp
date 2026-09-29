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

const InteractionHint characterMoverHints[] =
{
    { Identifier ("moveHorizontally"), NEEDS_TRANS ("A / D"), NEEDS_TRANS ("Move left / right.") },
    { Identifier ("jump"), NEEDS_TRANS ("Space"), NEEDS_TRANS ("Jump.") }
};

const InteractionHint collisionCastWorldHints[] =
{
    { Identifier ("primaryInteraction"), NEEDS_TRANS ("Primary drag"), NEEDS_TRANS ("Modify the ray cast.") },
    { Identifier ("rotateCast"), NEEDS_TRANS ("Shift + primary drag"), NEEDS_TRANS ("Rotate the cast.") }
};

const InteractionHint collisionDynamicTreeHints[] =
{
    { Identifier ("primaryInteraction"), NEEDS_TRANS ("Primary drag"), NEEDS_TRANS ("Move the ray cast.") },
    { Identifier ("moveQueryRegion"), NEEDS_TRANS ("Shift + primary drag"), NEEDS_TRANS ("Move the query region.") }
};

const InteractionHint collisionManifoldHints[] =
{
    { Identifier ("primaryInteraction"), NEEDS_TRANS ("Primary drag"), NEEDS_TRANS ("Move the shape.") },
    { Identifier ("rotateShape"), NEEDS_TRANS ("Shift + primary drag"), NEEDS_TRANS ("Rotate the shape.") }
};

const InteractionHint collisionOverlapWorldHints[] =
{
    { Identifier ("primaryInteraction"), NEEDS_TRANS ("Primary drag"), NEEDS_TRANS ("Move the query shape.") },
    { Identifier ("rotateQueryShape"), NEEDS_TRANS ("Shift + primary drag"), NEEDS_TRANS ("Rotate the query shape.") }
};

const InteractionHint collisionRayCastHints[] =
{
    { Identifier ("primaryInteraction"), NEEDS_TRANS ("Primary drag"), NEEDS_TRANS ("Move the ray cast.") },
    { Identifier ("translateFixture"), NEEDS_TRANS ("Shift + primary drag"), NEEDS_TRANS ("Translate the fixture.") },
    { Identifier ("rotateFixture"), NEEDS_TRANS ("Control + primary drag"), NEEDS_TRANS ("Rotate the fixture.") }
};

const InteractionHint collisionShapeCastHints[] =
{
    { Identifier ("primaryInteraction"), NEEDS_TRANS ("Primary drag"), NEEDS_TRANS ("Move the transform.") },
    { Identifier ("rotateTransform"), NEEDS_TRANS ("Shift + primary drag"), NEEDS_TRANS ("Rotate the transform.") },
    { Identifier ("sweepTransform"), NEEDS_TRANS ("Control + primary drag"), NEEDS_TRANS ("Sweep the transform.") }
};

const InteractionHint collisionShapeDistanceHints[] =
{
    { Identifier ("primaryInteraction"), NEEDS_TRANS ("Primary drag"), NEEDS_TRANS ("Move shape B.") },
    { Identifier ("rotateShapeB"), NEEDS_TRANS ("Shift + primary drag"), NEEDS_TRANS ("Rotate shape B.") }
};

const InteractionHint continuousDropHints[] =
{
    { Identifier ("selectScene"), NEEDS_TRANS ("1 / 2 / 3 / 4"), NEEDS_TRANS ("Select a scene.") },
    { Identifier ("toggleContinuousCollision"), NEEDS_TRANS ("C"), NEEDS_TRANS ("Toggle continuous collision.") },
    { Identifier ("toggleFrameSkipping"), NEEDS_TRANS ("S"), NEEDS_TRANS ("Toggle frame skipping.") }
};

const InteractionHint continuousPinballHints[] =
{
    { Identifier ("operateFlippers"), NEEDS_TRANS ("A"), NEEDS_TRANS ("Operate the flippers.") }
};

const InteractionHint eventsContactHints[] =
{
    { Identifier ("applyForce"), NEEDS_TRANS ("W / A / S / D"), NEEDS_TRANS ("Apply force up / left / down / right.") }
};

const InteractionHint eventsFootSensorHints[] =
{
    { Identifier ("moveHorizontally"), NEEDS_TRANS ("A / D"), NEEDS_TRANS ("Move left / right.") }
};

const InteractionHint eventsProjectileEventHints[] =
{
    { Identifier ("primaryInteraction"), NEEDS_TRANS ("Control + primary drag"), NEEDS_TRANS ("Aim and fire the projectile.") }
};

const InteractionHint eventsSensorHitsHints[] =
{
    { Identifier ("launch"), NEEDS_TRANS ("B"), NEEDS_TRANS ("Launch.") }
};

const InteractionHint geometryConvexHullHints[] =
{
    { Identifier ("generateHull"), NEEDS_TRANS ("G"), NEEDS_TRANS ("Generate a hull.") },
    { Identifier ("toggleAutomaticGeneration"), NEEDS_TRANS ("A"), NEEDS_TRANS ("Toggle automatic generation.") },
    { Identifier ("toggleBulkGeneration"), NEEDS_TRANS ("B"), NEEDS_TRANS ("Toggle bulk generation.") }
};

const InteractionHint jointsDrivingHints[] =
{
    { Identifier ("drive"), NEEDS_TRANS ("A / S / D"), NEEDS_TRANS ("Drive left / brake / drive right.") }
};

const InteractionHint jointsGearLiftHints[] =
{
    { Identifier ("adjustMotorSpeed"), NEEDS_TRANS ("A / D"), NEEDS_TRANS ("Decrease / increase motor speed.") }
};

const InteractionHint jointsMotionLocksHints[] =
{
    { Identifier ("applyLinearImpulse"), NEEDS_TRANS ("L"), NEEDS_TRANS ("Apply a linear impulse.") }
};

const InteractionHint jointsTheoJansenHints[] =
{
    { Identifier ("drive"), NEEDS_TRANS ("A / S / D"), NEEDS_TRANS ("Drive left / stop / drive right.") },
    { Identifier ("toggleMotor"), NEEDS_TRANS ("F"), NEEDS_TRANS ("Toggle the motor.") }
};

const InteractionHint replayHints[] =
{
    { Identifier ("selectShape"), NEEDS_TRANS ("Primary click"), NEEDS_TRANS ("Select a shape.") },
    { Identifier ("clearSelection"), NEEDS_TRANS ("Escape"), NEEDS_TRANS ("Clear the shape selection.") },
    { Identifier ("previousFrame"), NEEDS_TRANS ("Comma"), NEEDS_TRANS ("Step back one frame.") },
    { Identifier ("previousFrames"), NEEDS_TRANS ("Shift + comma"), NEEDS_TRANS ("Step back five frames.") }
};

const InteractionHint shapesRollingResistanceHints[] =
{
    { Identifier ("selectRamp"), NEEDS_TRANS ("1 / 2 / 3"), NEEDS_TRANS ("Select a flat / uphill / downhill ramp.") }
};

const InteractionHint stackingVerticalStackHints[] =
{
    { Identifier ("fireBullets"), NEEDS_TRANS ("B"), NEEDS_TRANS ("Fire bullets.") }
};

const InteractionHint worldFarGateHints[] =
{
    { Identifier ("adjustMotorSpeed"), NEEDS_TRANS ("A / D"), NEEDS_TRANS ("Decrease / increase motor speed.") }
};

const InteractionHint worldTilesHints[] =
{
    { Identifier ("drive"), NEEDS_TRANS ("A / S / D"), NEEDS_TRANS ("Drive left / brake / drive right.") }
};

const SampleInteractionHelp entries[] =
{
    { "Character", "Dynamic Mover", characterMoverHints },
    { "Character", "Geometric Mover", characterMoverHints },
    { "Collision", "Cast World", collisionCastWorldHints },
    { "Collision", "Dynamic Tree", collisionDynamicTreeHints },
    { "Collision", "Manifold", collisionManifoldHints },
    { "Collision", "Overlap World", collisionOverlapWorldHints },
    { "Collision", "Ray Cast", collisionRayCastHints },
    { "Collision", "Shape Cast", collisionShapeCastHints },
    { "Collision", "Shape Distance", collisionShapeDistanceHints },
    { "Collision", "Smooth Manifold", collisionManifoldHints },
    { "Continuous", "Drop", continuousDropHints },
    { "Continuous", "Pinball", continuousPinballHints },
    { "Events", "Contact", eventsContactHints },
    { "Events", "Foot Sensor", eventsFootSensorHints },
    { "Events", "Projectile Event", eventsProjectileEventHints },
    { "Events", "Sensor Hits", eventsSensorHitsHints },
    { "Geometry", "Convex Hull", geometryConvexHullHints },
    { "Joints", "Driving", jointsDrivingHints },
    { "Joints", "Gear Lift", jointsGearLiftHints },
    { "Joints", "Motion Locks", jointsMotionLocksHints },
    { "Joints", "Theo Jansen", jointsTheoJansenHints },
    { "Replay", "Viewer", replayHints },
    { "Shapes", "Rolling Resistance", shapesRollingResistanceHints },
    { "Stacking", "Vertical Stack", stackingVerticalStackHints },
    { "World", "Far Gate", worldFarGateHints },
    { "World", "Tiles", worldTilesHints }
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
