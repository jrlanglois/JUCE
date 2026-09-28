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
   MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE, ARE DISCLAIMED.

  ==============================================================================
*/

namespace juce
{

namespace
{

static b2Pos makeWorldPosition (double x, double y) noexcept
{
#if defined (BOX2D_DOUBLE_PRECISION)
    return b2Pos { x, y };
#else
    return b2Pos { (float) x, (float) y };
#endif
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

    b2WorldId worldId;

    JUCE_DECLARE_NON_COPYABLE (ScopedBox2DWorld)
    JUCE_DECLARE_NON_MOVEABLE (ScopedBox2DWorld)
};

struct ScopedFallingHingeData final
{
    ScopedFallingHingeData() = default;

    ~ScopedFallingHingeData()
    {
        DestroyFallingHinges (&data);
    }

    FallingHingeData data {};

    JUCE_DECLARE_NON_COPYABLE (ScopedFallingHingeData)
    JUCE_DECLARE_NON_MOVEABLE (ScopedFallingHingeData)
};

static bool runFallingHingesDeterminism (int workerCount, int stepLimit)
{
    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.workerCount = workerCount;

    ScopedBox2DWorld world (worldDef);
    if (! b2World_IsValid (world.worldId))
        return false;

    ScopedFallingHingeData hingeData;
    hingeData.data = CreateFallingHinges (world.worldId);

    const float timeStep = 1.0f / 60.0f;
    const int subStepCount = 4;

    for (int stepIndex = 0; stepIndex < stepLimit; ++stepIndex)
    {
        b2World_Step (world.worldId, timeStep, subStepCount);

        if (UpdateFallingHinges (world.worldId, &hingeData.data))
            break;
    }

    return hingeData.data.sleepStep == EXPECTED_SLEEP_STEP
        && hingeData.data.hash == (uint32_t) EXPECTED_HASH;
}

static b2WorldId createBoxRenderScene (b2Pos sceneOrigin)
{
    b2WorldDef worldDef = b2DefaultWorldDef();
    const b2WorldId worldId = b2CreateWorld (&worldDef);

    if (! b2World_IsValid (worldId))
        return worldId;

    {
        b2BodyDef groundDef = b2DefaultBodyDef();
        groundDef.position = makeWorldPosition (sceneOrigin.x, sceneOrigin.y - 1.0);
        const b2BodyId groundId = b2CreateBody (worldId, &groundDef);

        b2Polygon groundBox = b2MakeBox (4.0f, 0.5f);
        b2ShapeDef groundShapeDef = b2DefaultShapeDef();
        b2CreatePolygonShape (groundId, &groundShapeDef, &groundBox);
    }

    {
        b2BodyDef boxDef = b2DefaultBodyDef();
        boxDef.type = b2_dynamicBody;
        boxDef.position = makeWorldPosition (sceneOrigin.x, sceneOrigin.y + 1.0);
        const b2BodyId boxId = b2CreateBody (worldId, &boxDef);

        b2Polygon dynamicBox = b2MakeBox (0.75f, 0.75f);
        b2ShapeDef boxShapeDef = b2DefaultShapeDef();
        b2CreatePolygonShape (boxId, &boxShapeDef, &dynamicBox);
    }

    return worldId;
}

static Colour readPixel (const Image& image, int x, int y)
{
    return image.getPixelAt (x, y);
}

static void renderSceneToImage (Graphics& graphics,
                                b2WorldId worldId,
                                b2Pos viewCentre,
                                const juce::Rectangle<float>& targetArea)
{
    Box2DRenderer renderer;
    const b2Vec2 viewSize { 8.0f, 8.0f };
    renderer.render (graphics, worldId, viewCentre, viewSize, targetArea);
}

} // namespace

struct Box2DTests final : public UnitTest
{
    Box2DTests() : UnitTest ("Box2D", UnitTestCategories::box2d) {}

    void runTest() override
    {
        beginTest ("A dynamic box dropped on static ground comes to rest");
        {
            b2WorldDef worldDef = b2DefaultWorldDef();
            ScopedBox2DWorld world (worldDef);
            expect (b2World_IsValid (world.worldId));

            b2BodyId groundId = b2_nullBodyId;
            b2BodyId boxId = b2_nullBodyId;

            {
                b2BodyDef groundDef = b2DefaultBodyDef();
                groundDef.position = makeWorldPosition (0.0, -1.0);
                groundId = b2CreateBody (world.worldId, &groundDef);

                b2Polygon groundBox = b2MakeBox (5.0f, 0.5f);
                b2ShapeDef groundShapeDef = b2DefaultShapeDef();
                b2CreatePolygonShape (groundId, &groundShapeDef, &groundBox);
            }

            {
                b2BodyDef boxDef = b2DefaultBodyDef();
                boxDef.type = b2_dynamicBody;
                boxDef.position = makeWorldPosition (0.0, 5.0);
                boxId = b2CreateBody (world.worldId, &boxDef);

                b2Polygon dynamicBox = b2MakeBox (0.5f, 0.5f);
                b2ShapeDef boxShapeDef = b2DefaultShapeDef();
                b2CreatePolygonShape (boxId, &boxShapeDef, &dynamicBox);
            }

            const float timeStep = 1.0f / 60.0f;
            const int subStepCount = 4;

            for (int stepIndex = 0; stepIndex < 600 && b2Body_IsAwake (boxId); ++stepIndex)
                b2World_Step (world.worldId, timeStep, subStepCount);

            expect (! b2Body_IsAwake (boxId));

            const b2Vec2 velocity = b2Body_GetLinearVelocity (boxId);
            expect (std::abs (velocity.x) < 0.05f);
            expect (std::abs (velocity.y) < 0.05f);
        }

        beginTest ("Falling hinges determinism matches the pinned hash serially");
        {
            expect (runFallingHingesDeterminism (1, 500));
        }

#if JUCE_WINDOWS || JUCE_LINUX || JUCE_ANDROID || JUCE_MAC || JUCE_IOS || JUCE_EMSCRIPTEN
        beginTest ("Falling hinges determinism matches the pinned hash with the built-in scheduler");
        {
            for (int workerCount = 2; workerCount <= 8; workerCount += 2)
                expect (runFallingHingesDeterminism (workerCount, 1000));
        }
#endif

        beginTest ("Box2DRenderer changes only the target area and restores caller graphics state");
        {
            const b2Pos sceneOrigin = makeWorldPosition (0.0, 0.0);
            const b2WorldId worldId = createBoxRenderScene (sceneOrigin);
            expect (b2World_IsValid (worldId));

            Image image (Image::ARGB, 64, 64, true);
            image.clear (image.getBounds(), Colours::black);

            const juce::Rectangle<int> clipArea (0, 0, 64, 64);
            const juce::Rectangle<float> targetArea (20.0f, 20.0f, 24.0f, 24.0f);
            const auto targetBounds = targetArea.toNearestIntEdges();

            const juce::Point<int> probeOutsideTarget { 5, 5 };
            const juce::Rectangle<int> restoredFillArea (probeOutsideTarget.x,
                                                         probeOutsideTarget.y,
                                                         4,
                                                         4);

            {
                Graphics graphics (image);
                graphics.setColour (Colours::blue);
                graphics.reduceClipRegion (clipArea);

                renderSceneToImage (graphics, worldId, sceneOrigin, targetArea);
                expect (graphics.getClipBounds() == clipArea);

                graphics.fillRect (restoredFillArea);
            }

            bool hasChangedInsideTarget = false,
                 hasChangedUnexpectedlyOutsideTarget = false;

            for (int y = 0; y < image.getHeight(); ++y)
            {
                for (int x = 0; x < image.getWidth(); ++x)
                {
                    const bool hasChanged = readPixel (image, x, y) != Colours::black;

                    if (targetBounds.contains (x, y))
                        hasChangedInsideTarget = hasChangedInsideTarget || hasChanged;
                    else if (! restoredFillArea.contains (x, y))
                        hasChangedUnexpectedlyOutsideTarget = hasChangedUnexpectedlyOutsideTarget || hasChanged;
                }
            }

            expect (hasChangedInsideTarget);
            expect (! hasChangedUnexpectedlyOutsideTarget);
            expect (readPixel (image, probeOutsideTarget.x, probeOutsideTarget.y) == Colours::blue);

            b2DestroyWorld (worldId);
        }

#if defined (BOX2D_DOUBLE_PRECISION)
        beginTest ("Box2DRenderer matches a far-origin scene to the origin scene");
        {
            auto imagesMatchInArea = [] (const Image& a, const Image& b, juce::Rectangle<int> area)
            {
                if (a.getWidth() != b.getWidth() || a.getHeight() != b.getHeight())
                    return false;

                for (int y = area.getY(); y < area.getBottom(); ++y)
                {
                    for (int x = area.getX(); x < area.getRight(); ++x)
                    {
                        if (readPixel (a, x, y).getARGB() != readPixel (b, x, y).getARGB())
                            return false;
                    }
                }

                return true;
            };

            const b2Pos originScene = makeWorldPosition (0.0, 0.0);
            const b2Pos farScene = makeWorldPosition (1.0e7, 1.0e7);

            const b2WorldId originWorldId = createBoxRenderScene (originScene);
            const b2WorldId farWorldId = createBoxRenderScene (farScene);
            expect (b2World_IsValid (originWorldId));
            expect (b2World_IsValid (farWorldId));

            Image originImage (Image::ARGB, 64, 64, true);
            Image farImage (Image::ARGB, 64, 64, true);
            originImage.clear (originImage.getBounds(), Colours::black);
            farImage.clear (farImage.getBounds(), Colours::black);

            const juce::Rectangle<float> targetArea (0.0f, 0.0f, 64.0f, 64.0f);

            {
                Graphics originGraphics (originImage);
                renderSceneToImage (originGraphics, originWorldId, originScene, targetArea);
            }

            {
                Graphics farGraphics (farImage);
                renderSceneToImage (farGraphics, farWorldId, farScene, targetArea);
            }

            expect (imagesMatchInArea (originImage,
                                     farImage,
                                     { 0, 0, originImage.getWidth(), originImage.getHeight() }));

            b2DestroyWorld (originWorldId);
            b2DestroyWorld (farWorldId);
        }
#endif
    }
};

static Box2DTests box2DTests;

} // namespace juce
