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

/*******************************************************************************
 The block below describes the properties of this PIP. A PIP is a short snippet
 of code that can be read by the Projucer and used to generate a JUCE project.

 BEGIN_JUCE_PIP_METADATA

 name:             Box2DDemo
 version:          1.0.0
 vendor:           JUCE
 website:          http://juce.com
 description:      Showcases 2D graphics features.

 dependencies:     juce_box2d, juce_core, juce_data_structures, juce_events,
                   juce_graphics, juce_gui_basics
 exporters:        xcode_mac, vs2022, vs2026, linux_make, androidstudio,
                   xcode_iphone

 moduleFlags:      JUCE_STRICT_REFCOUNTEDPOINTER=1

 type:             Component
 mainClass:        Box2DDemo

 useLocalCopy:     1

 END_JUCE_PIP_METADATA

*******************************************************************************/

#pragma once

#include "../Assets/DemoUtilities.h"

namespace Box2DDemoScenes
{

static constexpr int subStepCount = 4;
static constexpr float timeStepSeconds = 1.0f / 60.0f;

static b2Pos makePosition (float x, float y) noexcept
{
#if defined (BOX2D_DOUBLE_PRECISION)
    return b2Pos { (double) x, (double) y };
#else
    return b2Pos { x, y };
#endif
}

static float randomFloat (float lower, float upper)
{
    return Random::getSystemRandom().nextFloat() * (upper - lower) + lower;
}

struct ScopedBox2DWorld final
{
    explicit ScopedBox2DWorld (const b2WorldDef& worldDef) noexcept
        : worldId (b2CreateWorld (&worldDef))
    {
    }

    ~ScopedBox2DWorld()
    {
        if (b2World_IsValid (worldId))
            b2DestroyWorld (worldId);
    }

    void reset (const b2WorldDef& worldDef)
    {
        if (b2World_IsValid (worldId))
            b2DestroyWorld (worldId);

        worldId = b2CreateWorld (&worldDef);
    }

    b2WorldId worldId;

    JUCE_DECLARE_NON_COPYABLE (ScopedBox2DWorld)
    JUCE_DECLARE_NON_MOVEABLE (ScopedBox2DWorld)
};

static void createRevoluteJoint (b2WorldId worldId,
                                 b2BodyId bodyA,
                                 b2BodyId bodyB,
                                 float anchorX,
                                 float anchorY,
                                 bool collideConnected)
{
    b2RevoluteJointDef jointDef = b2DefaultRevoluteJointDef();
    jointDef.base.bodyIdA = bodyA;
    jointDef.base.bodyIdB = bodyB;
    jointDef.base.collideConnected = collideConnected;

    const b2Pos anchor = makePosition (anchorX, anchorY);
    jointDef.base.localFrameA.p = b2Body_GetLocalPoint (bodyA, anchor);
    jointDef.base.localFrameB.p = b2Body_GetLocalPoint (bodyB, anchor);

    b2CreateRevoluteJoint (worldId, &jointDef);
}

static void attachMotorFriction (b2WorldId worldId,
                                 b2BodyId groundId,
                                 b2BodyId bodyId,
                                 float gravityMagnitude)
{
    const float mass = b2Body_GetMass (bodyId);
    const float inertia = b2Body_GetRotationalInertia (bodyId);
    const float radius = std::sqrt (2.0f * inertia / mass);

    b2MotorJointDef jointDef = b2DefaultMotorJointDef();
    jointDef.base.bodyIdA = groundId;
    jointDef.base.bodyIdB = bodyId;
    jointDef.base.collideConnected = true;
    jointDef.maxVelocityForce = mass * gravityMagnitude;
    jointDef.maxVelocityTorque = mass * radius * gravityMagnitude;

    b2CreateMotorJoint (worldId, &jointDef);
}

static b2Polygon makeTriangle (const b2Vec2 (&vertices)[3])
{
    b2Hull hull = b2ComputeHull (vertices, 3);
    return b2MakePolygon (&hull, 0.0f);
}

static void buildAddPairStressTest (b2WorldId worldId)
{
    b2World_SetGravity (worldId, { 0.0f, 0.0f });

    {
        b2Circle circle = { { 0.0f, 0.0f }, 0.1f };
        b2ShapeDef shapeDef = b2DefaultShapeDef();
        shapeDef.density = 0.01f;

        constexpr float minX = -6.0f;
        constexpr float maxX = 0.0f;
        constexpr float minY = 4.0f;
        constexpr float maxY = 6.0f;

        for (int index = 0; index < 400; ++index)
        {
            b2BodyDef bodyDef = b2DefaultBodyDef();
            bodyDef.type = b2_dynamicBody;
            bodyDef.position = makePosition (randomFloat (minX, maxX), randomFloat (minY, maxY));
            const b2BodyId bodyId = b2CreateBody (worldId, &bodyDef);
            b2CreateCircleShape (bodyId, &shapeDef, &circle);
        }
    }

    {
        b2BodyDef bodyDef = b2DefaultBodyDef();
        bodyDef.type = b2_dynamicBody;
        bodyDef.position = makePosition (-40.0f, 5.0f);
        bodyDef.isBullet = true;
        bodyDef.linearVelocity = { 150.0f, 0.0f };
        const b2BodyId bodyId = b2CreateBody (worldId, &bodyDef);

        b2ShapeDef shapeDef = b2DefaultShapeDef();
        shapeDef.density = 1.0f;
        b2Polygon box = b2MakeBox (1.5f, 1.5f);
        b2CreatePolygonShape (bodyId, &shapeDef, &box);
    }
}

static b2BodyId buildApplyForce (b2WorldId worldId)
{
    b2World_SetGravity (worldId, { 0.0f, 0.0f });

    constexpr float restitution = 0.4f;
    constexpr float gravityMagnitude = 10.0f;

    b2BodyId groundId {};
    {
        b2BodyDef groundDef = b2DefaultBodyDef();
        groundDef.position = makePosition (0.0f, 20.0f);
        groundId = b2CreateBody (worldId, &groundDef);

        b2ShapeDef shapeDef = b2DefaultShapeDef();
        shapeDef.density = 0.0f;
        shapeDef.material.restitution = restitution;

        b2Segment segment = { { -20.0f, -20.0f }, { -20.0f, 20.0f } };
        b2CreateSegmentShape (groundId, &shapeDef, &segment);

        segment = { { 20.0f, -20.0f }, { 20.0f, 20.0f } };
        b2CreateSegmentShape (groundId, &shapeDef, &segment);

        segment = { { -20.0f, 20.0f }, { 20.0f, 20.0f } };
        b2CreateSegmentShape (groundId, &shapeDef, &segment);

        segment = { { -20.0f, -20.0f }, { 20.0f, -20.0f } };
        b2CreateSegmentShape (groundId, &shapeDef, &segment);
    }

    b2BodyId controlledBodyId {};
    {
        const b2Rot rotationA = b2MakeRot (0.3524f * B2_PI);
        const b2Rot rotationB = b2MakeRot (-0.3524f * B2_PI);

        b2Vec2 triangleOne[] =
        {
            b2RotateVector (rotationA, { -1.0f, 0.0f }),
            b2RotateVector (rotationA, { 1.0f, 0.0f }),
            b2RotateVector (rotationA, { 0.0f, 0.5f }),
        };

        b2Vec2 triangleTwo[] =
        {
            b2RotateVector (rotationB, { -1.0f, 0.0f }),
            b2RotateVector (rotationB, { 1.0f, 0.0f }),
            b2RotateVector (rotationB, { 0.0f, 0.5f }),
        };

        b2BodyDef bodyDef = b2DefaultBodyDef();
        bodyDef.type = b2_dynamicBody;
        bodyDef.position = makePosition (0.0f, 2.0f);
        bodyDef.rotation = b2MakeRot (B2_PI);
        bodyDef.angularDamping = 5.0f;
        bodyDef.linearDamping = 0.1f;
        bodyDef.enableSleep = false;
        controlledBodyId = b2CreateBody (worldId, &bodyDef);

        b2ShapeDef shapeDefOne = b2DefaultShapeDef();
        shapeDefOne.density = 4.0f;
        const b2Polygon polygonOne = makeTriangle (triangleOne);
        b2CreatePolygonShape (controlledBodyId, &shapeDefOne, &polygonOne);

        b2ShapeDef shapeDefTwo = b2DefaultShapeDef();
        shapeDefTwo.density = 2.0f;
        const b2Polygon polygonTwo = makeTriangle (triangleTwo);
        b2CreatePolygonShape (controlledBodyId, &shapeDefTwo, &polygonTwo);

        attachMotorFriction (worldId, groundId, controlledBodyId, gravityMagnitude);
    }

    {
        b2ShapeDef shapeDef = b2DefaultShapeDef();
        shapeDef.density = 1.0f;
        shapeDef.material.friction = 0.3f;
        b2Polygon box = b2MakeBox (0.5f, 0.5f);

        for (int index = 0; index < 10; ++index)
        {
            b2BodyDef bodyDef = b2DefaultBodyDef();
            bodyDef.type = b2_dynamicBody;
            bodyDef.position = makePosition (0.0f, 5.0f + 1.54f * (float) index);
            const b2BodyId bodyId = b2CreateBody (worldId, &bodyDef);
            b2CreatePolygonShape (bodyId, &shapeDef, &box);

            attachMotorFriction (worldId, groundId, bodyId, gravityMagnitude);
        }
    }

    return controlledBodyId;
}

static void buildDominoes (b2WorldId worldId)
{
    b2BodyId floorId {};
    {
        b2BodyDef bodyDef = b2DefaultBodyDef();
        floorId = b2CreateBody (worldId, &bodyDef);

        b2ShapeDef shapeDef = b2DefaultShapeDef();
        b2Segment segment = { { -40.0f, 0.0f }, { 40.0f, 0.0f } };
        b2CreateSegmentShape (floorId, &shapeDef, &segment);
    }

    {
        b2BodyDef bodyDef = b2DefaultBodyDef();
        bodyDef.position = makePosition (-1.5f, 10.0f);
        const b2BodyId platformId = b2CreateBody (worldId, &bodyDef);

        b2ShapeDef shapeDef = b2DefaultShapeDef();
        shapeDef.density = 0.0f;
        b2Polygon box = b2MakeBox (6.0f, 0.25f);
        b2CreatePolygonShape (platformId, &shapeDef, &box);
    }

    {
        b2ShapeDef shapeDef = b2DefaultShapeDef();
        shapeDef.density = 20.0f;
        shapeDef.material.friction = 0.1f;
        b2Polygon domino = b2MakeBox (0.1f, 1.0f);

        for (int index = 0; index < 10; ++index)
        {
            b2BodyDef bodyDef = b2DefaultBodyDef();
            bodyDef.type = b2_dynamicBody;
            bodyDef.position = makePosition (-6.0f + 1.0f * (float) index, 11.25f);
            const b2BodyId bodyId = b2CreateBody (worldId, &bodyDef);
            b2CreatePolygonShape (bodyId, &shapeDef, &domino);
        }
    }

    {
        b2BodyDef bodyDef = b2DefaultBodyDef();
        bodyDef.position = makePosition (1.0f, 6.0f);
        const b2BodyId rampId = b2CreateBody (worldId, &bodyDef);

        b2ShapeDef shapeDef = b2DefaultShapeDef();
        shapeDef.density = 0.0f;
        b2Polygon ramp = b2MakeOffsetBox (7.0f, 0.25f, { 0.0f, 0.0f }, b2MakeRot (0.3f));
        b2CreatePolygonShape (rampId, &shapeDef, &ramp);
    }

    b2BodyId pivotBodyId {};
    {
        b2BodyDef bodyDef = b2DefaultBodyDef();
        bodyDef.position = makePosition (-7.0f, 4.0f);
        pivotBodyId = b2CreateBody (worldId, &bodyDef);

        b2ShapeDef shapeDef = b2DefaultShapeDef();
        shapeDef.density = 0.0f;
        b2Polygon pillar = b2MakeBox (0.25f, 1.5f);
        b2CreatePolygonShape (pivotBodyId, &shapeDef, &pillar);
    }

    b2BodyId plankBodyId {};
    {
        b2BodyDef bodyDef = b2DefaultBodyDef();
        bodyDef.type = b2_dynamicBody;
        bodyDef.position = makePosition (-0.9f, 1.0f);
        bodyDef.rotation = b2MakeRot (-0.15f);
        plankBodyId = b2CreateBody (worldId, &bodyDef);

        b2ShapeDef shapeDef = b2DefaultShapeDef();
        shapeDef.density = 10.0f;
        b2Polygon plank = b2MakeBox (6.0f, 0.125f);
        b2CreatePolygonShape (plankBodyId, &shapeDef, &plank);
    }

    createRevoluteJoint (worldId, floorId, plankBodyId, -2.0f, 1.0f, true);

    b2BodyId suspendedBodyId {};
    {
        b2BodyDef bodyDef = b2DefaultBodyDef();
        bodyDef.type = b2_dynamicBody;
        bodyDef.position = makePosition (-10.0f, 15.0f);
        suspendedBodyId = b2CreateBody (worldId, &bodyDef);

        b2ShapeDef shapeDef = b2DefaultShapeDef();
        shapeDef.density = 10.0f;
        b2Polygon box = b2MakeBox (0.25f, 0.25f);
        b2CreatePolygonShape (suspendedBodyId, &shapeDef, &box);
    }

    createRevoluteJoint (worldId, pivotBodyId, suspendedBodyId, -7.0f, 15.0f, true);

    b2BodyId compoundBodyId {};
    {
        b2BodyDef bodyDef = b2DefaultBodyDef();
        bodyDef.type = b2_dynamicBody;
        bodyDef.position = makePosition (6.5f, 3.0f);
        compoundBodyId = b2CreateBody (worldId, &bodyDef);

        b2ShapeDef shapeDef = b2DefaultShapeDef();
        shapeDef.density = 10.0f;
        shapeDef.material.friction = 0.1f;

        b2Polygon base = b2MakeOffsetBox (1.0f, 0.1f, { 0.0f, -0.9f }, b2MakeRot (0.0f));
        b2CreatePolygonShape (compoundBodyId, &shapeDef, &base);

        b2Polygon leftArm = b2MakeOffsetBox (0.1f, 1.0f, { -0.9f, 0.0f }, b2MakeRot (0.0f));
        b2CreatePolygonShape (compoundBodyId, &shapeDef, &leftArm);

        b2Polygon rightArm = b2MakeOffsetBox (0.1f, 1.0f, { 0.9f, 0.0f }, b2MakeRot (0.0f));
        b2CreatePolygonShape (compoundBodyId, &shapeDef, &rightArm);
    }

    createRevoluteJoint (worldId, floorId, compoundBodyId, 6.0f, 2.0f, true);

    b2BodyId upperArmId {};
    {
        b2BodyDef bodyDef = b2DefaultBodyDef();
        bodyDef.type = b2_dynamicBody;
        bodyDef.position = makePosition (6.5f, 4.1f);
        upperArmId = b2CreateBody (worldId, &bodyDef);

        b2ShapeDef shapeDef = b2DefaultShapeDef();
        shapeDef.density = 30.0f;
        b2Polygon arm = b2MakeBox (1.0f, 0.1f);
        b2CreatePolygonShape (upperArmId, &shapeDef, &arm);
    }

    createRevoluteJoint (worldId, compoundBodyId, upperArmId, 7.5f, 4.0f, true);

    b2BodyId lowerArmId {};
    {
        b2BodyDef bodyDef = b2DefaultBodyDef();
        bodyDef.type = b2_dynamicBody;
        bodyDef.position = makePosition (7.4f, 1.0f);
        lowerArmId = b2CreateBody (worldId, &bodyDef);

        b2ShapeDef shapeDef = b2DefaultShapeDef();
        shapeDef.density = 10.0f;
        b2Polygon arm = b2MakeBox (0.1f, 1.0f);
        b2CreatePolygonShape (lowerArmId, &shapeDef, &arm);
    }

    {
        b2DistanceJointDef jointDef = b2DefaultDistanceJointDef();
        jointDef.base.bodyIdA = plankBodyId;
        jointDef.base.bodyIdB = lowerArmId;

        const b2Pos worldAnchorA = b2Body_GetWorldPoint (plankBodyId, { 6.0f, 0.0f });
        const b2Pos worldAnchorB = b2Body_GetWorldPoint (lowerArmId, { 0.0f, -1.0f });

        jointDef.base.localFrameA.p = b2Body_GetLocalPoint (plankBodyId, worldAnchorA);
        jointDef.base.localFrameB.p = b2Body_GetLocalPoint (lowerArmId, worldAnchorB);
        jointDef.length = b2Distance (b2ToVec2 (worldAnchorA), b2ToVec2 (worldAnchorB));

        b2CreateDistanceJoint (worldId, &jointDef);
    }

    {
        constexpr float radius = 0.2f;
        b2Circle circle = { { 0.0f, 0.0f }, radius };
        b2ShapeDef shapeDef = b2DefaultShapeDef();
        shapeDef.density = 10.0f;

        for (int index = 0; index < 4; ++index)
        {
            b2BodyDef bodyDef = b2DefaultBodyDef();
            bodyDef.type = b2_dynamicBody;
            bodyDef.position = makePosition (5.9f + 2.0f * radius * (float) index, 2.4f);
            const b2BodyId bodyId = b2CreateBody (worldId, &bodyDef);
            b2CreateCircleShape (bodyId, &shapeDef, &circle);
        }
    }
}

static void buildChain (b2WorldId worldId)
{
    b2BodyId groundId {};
    {
        b2BodyDef bodyDef = b2DefaultBodyDef();
        groundId = b2CreateBody (worldId, &bodyDef);

        b2ShapeDef shapeDef = b2DefaultShapeDef();
        b2Segment segment = { { -40.0f, 0.0f }, { 40.0f, 0.0f } };
        b2CreateSegmentShape (groundId, &shapeDef, &segment);
    }

    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.density = 20.0f;
    shapeDef.material.friction = 0.2f;
    b2Polygon link = b2MakeBox (0.6f, 0.125f);

    constexpr float chainY = 25.0f;
    b2BodyId previousBodyId = groundId;

    for (int index = 0; index < 30; ++index)
    {
        b2BodyDef bodyDef = b2DefaultBodyDef();
        bodyDef.type = b2_dynamicBody;
        bodyDef.position = makePosition (0.5f + (float) index, chainY);
        const b2BodyId bodyId = b2CreateBody (worldId, &bodyDef);
        b2CreatePolygonShape (bodyId, &shapeDef, &link);

        createRevoluteJoint (worldId,
                             previousBodyId,
                             bodyId,
                             (float) index,
                             chainY,
                             false);

        previousBodyId = bodyId;
    }
}

} // namespace Box2DDemoScenes

//==============================================================================
class Box2DTestList final : public ListBoxModel,
                            public ChangeBroadcaster
{
public:
    explicit Box2DTestList (const StringArray& testList)
        : tests (testList)
    {
    }

    int getNumRows() override
    {
        return tests.size();
    }

    String getNameForRow (int rowNumber) override
    {
        return tests[rowNumber];
    }

    void selectedRowsChanged (int /*lastRowSelected*/) override
    {
        sendChangeMessage();
    }

    void paintListBoxItem (int row, Graphics& g, int width, int height, bool rowIsSelected) override
    {
        auto& lf = LookAndFeel::getDefaultLookAndFeel();

        if (rowIsSelected)
            g.fillAll (Colour::contrasting (lf.findColour (ListBox::textColourId),
                                            lf.findColour (ListBox::backgroundColourId)));

        g.setColour (lf.findColour (ListBox::textColourId));
        g.setFont ((float) height * 0.7f);
        g.drawText (tests[row], Rectangle<int> (0, 0, width, height).reduced (2),
                    Justification::centredLeft, true);
    }

private:
    StringArray tests;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Box2DTestList)
};

//==============================================================================
struct Box2DSceneRuntime
{
    enum SceneIndex
    {
        addPairStressTest = 0,
        applyForce,
        dominoes,
        chain,
        numScenes
    };

    Box2DDemoScenes::ScopedBox2DWorld world { b2DefaultWorldDef() };
    b2BodyId controlledBodyId {};
    b2Pos viewCentre { Box2DDemoScenes::makePosition (0.0f, 14.5f) };
    b2Vec2 viewSize { 32.0f, 31.0f };
    int activeSceneIndex = dominoes;

    void loadScene (int sceneIndex)
    {
        activeSceneIndex = sceneIndex;
        controlledBodyId = {};

        b2WorldDef worldDef = b2DefaultWorldDef();

        if (sceneIndex == addPairStressTest)
            worldDef.gravity = { 0.0f, 0.0f };

        world.reset (worldDef);

        if (! b2World_IsValid (world.worldId))
            return;

        switch (sceneIndex)
        {
            case addPairStressTest:
                Box2DDemoScenes::buildAddPairStressTest (world.worldId);
                viewCentre = Box2DDemoScenes::makePosition (0.0f, 5.0f);
                viewSize = { 48.0f, 12.0f };
                break;

            case applyForce:
                controlledBodyId = Box2DDemoScenes::buildApplyForce (world.worldId);
                viewCentre = Box2DDemoScenes::makePosition (0.0f, 12.0f);
                viewSize = { 44.0f, 44.0f };
                break;

            case dominoes:
                Box2DDemoScenes::buildDominoes (world.worldId);
                viewCentre = Box2DDemoScenes::makePosition (0.0f, 8.0f);
                viewSize = { 52.0f, 22.0f };
                break;

            case chain:
                Box2DDemoScenes::buildChain (world.worldId);
                viewCentre = Box2DDemoScenes::makePosition (15.0f, 25.0f);
                viewSize = { 36.0f, 10.0f };
                break;

            default:
                break;
        }
    }

    void step()
    {
        if (! b2World_IsValid (world.worldId))
            return;

        b2World_Step (world.worldId, Box2DDemoScenes::timeStepSeconds, Box2DDemoScenes::subStepCount);
    }

    void handleKey (juce_wchar key)
    {
        if (activeSceneIndex != applyForce || ! b2Body_IsValid (controlledBodyId))
            return;

        switch (key)
        {
            case 'w':
            {
                const b2Vec2 force = b2Body_GetWorldVector (controlledBodyId, { 0.0f, -200.0f });
                const b2Pos point = b2Body_GetWorldPoint (controlledBodyId, { 0.0f, 2.0f });
                b2Body_ApplyForce (controlledBodyId, force, point, true);
                break;
            }

            case 'a':
                b2Body_ApplyTorque (controlledBodyId, 50.0f, true);
                break;

            case 'd':
                b2Body_ApplyTorque (controlledBodyId, -50.0f, true);
                break;

            default:
                break;
        }
    }
};

//==============================================================================
struct Box2DRenderComponent final : public Component
{
    Box2DRenderComponent()
    {
        setOpaque (true);
        setWantsKeyboardFocus (true);
        box2DRenderer.getDebugDraw().drawJoints = true;
    }

    void setSceneRuntime (Box2DSceneRuntime* runtimeIn)
    {
        runtime = runtimeIn;
    }

    void setSceneTitle (const String& title)
    {
        if (getTitle() == title)
            return;

        setTitle (title);

        if (auto* handler = getAccessibilityHandler())
            handler->notifyAccessibilityEvent (AccessibilityEvent::titleChanged);
    }

    void paint (Graphics& g) override
    {
        g.fillAll (Colours::white);

        if (runtime == nullptr || ! b2World_IsValid (runtime->world.worldId))
            return;

        box2DRenderer.render (g,
                              runtime->world.worldId,
                              runtime->viewCentre,
                              runtime->viewSize,
                              getLocalBounds().toFloat().reduced (8.0f));
    }

    bool keyPressed (const KeyPress& key) override
    {
        switch (key.getTextCharacter())
        {
            case 'a':
            case 'w':
            case 'd':
                if (runtime != nullptr)
                    runtime->handleKey (key.getTextCharacter());

                return true;

            default:
                break;
        }

        return false;
    }

private:
    Box2DSceneRuntime* runtime = nullptr;
    Box2DRenderer box2DRenderer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Box2DRenderComponent)
};

//==============================================================================
class Box2DDemo final : public Component,
                        private Timer,
                        private ChangeListener
{
public:
    Box2DDemo()
        : testsList (getTestsList())
    {
        setOpaque (true);

        testsListModel.addChangeListener (this);

        renderComponent.setSceneRuntime (&sceneRuntime);
        addAndMakeVisible (renderComponent);

        addAndMakeVisible (testsListBox);
        testsListBox.setModel (&testsListModel);
        testsListBox.selectRow (Box2DSceneRuntime::dominoes);

        sceneRuntime.loadScene (Box2DSceneRuntime::dominoes);
        renderComponent.setSceneTitle (testsList[Box2DSceneRuntime::dominoes]);
        instructions.setText (getInstructions (Box2DSceneRuntime::dominoes));

        addAndMakeVisible (instructions);
        instructions.setMultiLine (true);
        instructions.setReadOnly (true);

        startTimerHz (60);

        setSize (500, 500);
    }

    ~Box2DDemo() override
    {
        testsListModel.removeChangeListener (this);
    }

    void paint (Graphics& g) override
    {
        g.fillAll (getUIColourIfAvailable (LookAndFeel_V4::ColourScheme::UIColour::windowBackground));
    }

    void resized() override
    {
        auto bounds = getLocalBounds().reduced (4);

        auto bottomArea = bounds.removeFromBottom (150);
        testsListBox.setBounds (bottomArea.removeFromLeft (150));

        bottomArea.removeFromLeft (4);
        instructions.setBounds (bottomArea);

        bounds.removeFromBottom (6);
        renderComponent.setBounds (bounds);
    }

private:
    StringArray testsList;
    Box2DTestList testsListModel { testsList };

    Box2DSceneRuntime sceneRuntime;
    Box2DRenderComponent renderComponent;
    ListBox testsListBox;
    TextEditor instructions;

    static String getInstructions (int index)
    {
        if (index == Box2DSceneRuntime::applyForce)
            return String ("Keys:") + newLine + "Left: 'a'" + newLine
                 + "Right: 'd'" + newLine + "Forward: 'w'";

        return {};
    }

    void timerCallback() override
    {
        if (! b2World_IsValid (sceneRuntime.world.worldId))
            return;

        if (isShowing())
            sceneRuntime.step();

        renderComponent.repaint();
    }

    void changeListenerCallback (ChangeBroadcaster* source) override
    {
        if (source != &testsListModel)
            return;

        const int index = testsListBox.getSelectedRow();

        if (! isPositiveAndBelow (index, testsList.size()))
            return;

        sceneRuntime.loadScene (index);
        instructions.setText (getInstructions (index));
        renderComponent.setSceneTitle (testsList[index]);
        renderComponent.repaint();
    }

    void lookAndFeelChanged() override
    {
        instructions.applyFontToAllText (instructions.getFont());
    }

    static StringArray getTestsList()
    {
        return { "Add Pair Stress Test", "Apply Force", "Dominoes", "Chain" };
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Box2DDemo)
};
