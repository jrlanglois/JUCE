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

struct RenderCallContext
{
    Graphics& graphics;
    const Box2DRenderer& renderer;
    AffineTransform worldToComponent;
    b2Pos viewCentre;
};

static void setGraphicsColour (RenderCallContext& context, b2HexColor colour)
{
    context.graphics.setColour (context.renderer.getColour (colour));
}

static void buildRelativePolygonPath (Path& path,
                                      b2WorldTransform transform,
                                      const b2Vec2* localVertices,
                                      int vertexCount,
                                      b2Pos viewCentre)
{
    const b2Transform relativeTransform = b2ToRelativeTransform (transform, viewCentre);

    if (vertexCount <= 0)
        return;

    const b2Vec2 firstPoint = b2TransformPoint (relativeTransform, localVertices[0]);
    path.startNewSubPath (firstPoint.x, firstPoint.y);

    for (int vertexIndex = 1; vertexIndex < vertexCount; ++vertexIndex)
    {
        const b2Vec2 point = b2TransformPoint (relativeTransform, localVertices[vertexIndex]);
        path.lineTo (point.x, point.y);
    }

    path.closeSubPath();
}

static void drawPolygonOutline (RenderCallContext& context,
                                b2WorldTransform transform,
                                const b2Vec2* vertices,
                                int vertexCount,
                                b2HexColor colour)
{
    Path path;
    buildRelativePolygonPath (path, transform, vertices, vertexCount, context.viewCentre);

    setGraphicsColour (context, colour);
    context.graphics.strokePath (path,
                                 PathStrokeType (context.renderer.getLineThickness()),
                                 context.worldToComponent);
}

static void drawSolidPolygon (RenderCallContext& context,
                              b2WorldTransform transform,
                              const b2Vec2* vertices,
                              int vertexCount,
                              float radius,
                              b2HexColor colour)
{
    Path path;
    buildRelativePolygonPath (path, transform, vertices, vertexCount, context.viewCentre);

    setGraphicsColour (context, colour);
    context.graphics.fillPath (path, context.worldToComponent);

    if (radius > 0.0f)
    {
        Path skinPath;
        const PathStrokeType skinStroke (2.0f * radius,
                                         PathStrokeType::curved,
                                         PathStrokeType::rounded);
        skinStroke.createStrokedPath (skinPath, path);
        context.graphics.fillPath (skinPath, context.worldToComponent);
    }
}

static void drawWorldCircleOutline (RenderCallContext& context, b2Vec2 centre, float radius, b2HexColor colour)
{
    Path path;
    path.addEllipse (centre.x - radius, centre.y - radius, radius * 2.0f, radius * 2.0f);

    setGraphicsColour (context, colour);
    context.graphics.strokePath (path,
                                 PathStrokeType (context.renderer.getLineThickness()),
                                 context.worldToComponent);
}

static void drawWorldCircleFill (RenderCallContext& context, b2Vec2 centre, float radius, b2HexColor colour)
{
    Path path;
    path.addEllipse (centre.x - radius, centre.y - radius, radius * 2.0f, radius * 2.0f);

    setGraphicsColour (context, colour);
    context.graphics.fillPath (path, context.worldToComponent);
}

static void drawWorldLine (RenderCallContext& context, b2Vec2 start, b2Vec2 end, b2HexColor colour)
{
    Path path;
    path.startNewSubPath (start.x, start.y);
    path.lineTo (end.x, end.y);

    setGraphicsColour (context, colour);
    context.graphics.strokePath (path,
                                 PathStrokeType (context.renderer.getLineThickness()),
                                 context.worldToComponent);
}

static juce::Point<float> worldPointToComponent (const RenderCallContext& context, b2Vec2 worldPoint)
{
    float componentX = worldPoint.x,
          componentY = worldPoint.y;
    context.worldToComponent.transformPoint (componentX, componentY);
    return { componentX, componentY };
}

static void drawPolygonCallback (b2WorldTransform transform,
                                 const b2Vec2* vertices,
                                 int vertexCount,
                                 b2HexColor colour,
                                 void* userContext)
{
    auto* context = static_cast<RenderCallContext*> (userContext);
    drawPolygonOutline (*context, transform, vertices, vertexCount, colour);
}

static void drawSolidPolygonCallback (b2WorldTransform transform,
                                      const b2Vec2* vertices,
                                      int vertexCount,
                                      float radius,
                                      b2HexColor colour,
                                      void* userContext)
{
    auto* context = static_cast<RenderCallContext*> (userContext);
    drawSolidPolygon (*context, transform, vertices, vertexCount, radius, colour);
}

static void drawCircleCallback (b2Pos centre, float radius, b2HexColor colour, void* userContext)
{
    auto* context = static_cast<RenderCallContext*> (userContext);
    const b2Vec2 relativeCentre = b2SubPos (centre, context->viewCentre);
    drawWorldCircleOutline (*context, relativeCentre, radius, colour);
}

static void drawSolidCircleCallback (b2WorldTransform transform,
                                     b2Vec2 localCentre,
                                     float radius,
                                     b2HexColor colour,
                                     void* userContext)
{
    auto* context = static_cast<RenderCallContext*> (userContext);
    const b2Transform relativeTransform = b2ToRelativeTransform (transform, context->viewCentre);
    const b2Vec2 relativeCentre = b2TransformPoint (relativeTransform, localCentre);
    drawWorldCircleFill (*context, relativeCentre, radius, colour);
}

static void drawSolidCapsuleCallback (b2Pos point1, b2Pos point2, float radius, b2HexColor colour, void* userContext)
{
    auto* context = static_cast<RenderCallContext*> (userContext);
    const b2Vec2 relativePoint1 = b2SubPos (point1, context->viewCentre);
    const b2Vec2 relativePoint2 = b2SubPos (point2, context->viewCentre);

    Path path;
    path.startNewSubPath (relativePoint1.x, relativePoint1.y);
    path.lineTo (relativePoint2.x, relativePoint2.y);

    Path skinPath;
    const PathStrokeType skinStroke (2.0f * radius,
                                     PathStrokeType::curved,
                                     PathStrokeType::rounded);
    skinStroke.createStrokedPath (skinPath, path);

    setGraphicsColour (*context, colour);
    context->graphics.fillPath (skinPath, context->worldToComponent);
}

static void drawLineCallback (b2Pos point1, b2Pos point2, b2HexColor colour, void* userContext)
{
    auto* context = static_cast<RenderCallContext*> (userContext);
    const b2Vec2 relativePoint1 = b2SubPos (point1, context->viewCentre);
    const b2Vec2 relativePoint2 = b2SubPos (point2, context->viewCentre);
    drawWorldLine (*context, relativePoint1, relativePoint2, colour);
}

static void drawTransformCallback (b2WorldTransform transform, void* userContext)
{
    auto* context = static_cast<RenderCallContext*> (userContext);
    const b2Transform relativeTransform = b2ToRelativeTransform (transform, context->viewCentre);
    const float axisLength = 0.3f * b2GetLengthUnitsPerMeter();

    const b2Vec2 xAxisEnd = b2TransformPoint (relativeTransform, B2_LITERAL (b2Vec2) { axisLength, 0.0f });
    const b2Vec2 yAxisEnd = b2TransformPoint (relativeTransform, B2_LITERAL (b2Vec2) { 0.0f, axisLength });

    drawWorldLine (*context, relativeTransform.p, xAxisEnd, b2_colorRed);
    drawWorldLine (*context, relativeTransform.p, yAxisEnd, b2_colorGreen);
}

static void drawPointCallback (b2Pos point, float size, b2HexColor colour, void* userContext)
{
    auto* context = static_cast<RenderCallContext*> (userContext);
    const b2Vec2 relativePoint = b2SubPos (point, context->viewCentre);
    const auto componentPoint = worldPointToComponent (*context, relativePoint);
    const float halfSize = 0.5f * size;

    setGraphicsColour (*context, colour);
    context->graphics.fillEllipse (componentPoint.x - halfSize,
                                   componentPoint.y - halfSize,
                                   size,
                                   size);
}

static void drawStringCallback (b2Pos point, const char* text, b2HexColor colour, void* userContext)
{
    if (text == nullptr || text[0] == 0)
        return;

    auto* context = static_cast<RenderCallContext*> (userContext);
    const b2Vec2 relativePoint = b2SubPos (point, context->viewCentre);
    const auto componentPoint = worldPointToComponent (*context, relativePoint);

    setGraphicsColour (*context, colour);
    context->graphics.drawSingleLineText (text,
                                          (int) std::round (componentPoint.x),
                                          (int) std::round (componentPoint.y));
}

static void drawBoundsCallback (b2AABB bounds, b2HexColor colour, void* userContext)
{
    auto* context = static_cast<RenderCallContext*> (userContext);

    const b2Vec2 relativeLower { b2SubPos (b2ToPos (bounds.lowerBound), context->viewCentre) };
    const b2Vec2 relativeUpper { b2SubPos (b2ToPos (bounds.upperBound), context->viewCentre) };

    Path path;
    path.addRectangle (relativeLower.x,
                       relativeLower.y,
                       relativeUpper.x - relativeLower.x,
                       relativeUpper.y - relativeLower.y);

    setGraphicsColour (*context, colour);
    context->graphics.strokePath (path,
                                 PathStrokeType (context->renderer.getLineThickness()),
                                 context->worldToComponent);
}

static void installDebugDrawCallbacks (b2DebugDraw& draw)
{
    draw.DrawPolygonFcn = drawPolygonCallback;
    draw.DrawSolidPolygonFcn = drawSolidPolygonCallback;
    draw.DrawCircleFcn = drawCircleCallback;
    draw.DrawSolidCircleFcn = drawSolidCircleCallback;
    draw.DrawSolidCapsuleFcn = drawSolidCapsuleCallback;
    draw.DrawLineFcn = drawLineCallback;
    draw.DrawTransformFcn = drawTransformCallback;
    draw.DrawPointFcn = drawPointCallback;
    draw.DrawStringFcn = drawStringCallback;
    draw.DrawBoundsFcn = drawBoundsCallback;
}

} // namespace

Colour Box2DRenderer::getColour (b2HexColor colour) const
{
    return Colour ((uint8) ((colour >> 16) & 0xff),
                   (uint8) ((colour >> 8) & 0xff),
                   (uint8) (colour & 0xff));
}

float Box2DRenderer::getLineThickness() const
{
    return 1.0f;
}

void Box2DRenderer::render (Graphics& graphics,
                            b2WorldId worldId,
                            b2Pos viewCentre,
                            b2Vec2 viewSize,
                            const juce::Rectangle<float>& targetArea) const
{
    jassert (b2World_IsValid (worldId));
    jassert (viewSize.x > 0.0f && viewSize.y > 0.0f);
    jassert (! targetArea.isEmpty());

    if (! b2World_IsValid (worldId))
        return;

    if (viewSize.x <= 0.0f || viewSize.y <= 0.0f)
        return;

    if (targetArea.isEmpty())
        return;

    const double halfViewWidth  = 0.5 * (double) viewSize.x;
    const double halfViewHeight = 0.5 * (double) viewSize.y;

    const float worldLeft   = b2RoundDownFloat (viewCentre.x - halfViewWidth);
    const float worldRight  = b2RoundUpFloat (viewCentre.x + halfViewWidth);
    const float worldBottom = b2RoundDownFloat (viewCentre.y - halfViewHeight);
    const float worldTop    = b2RoundUpFloat (viewCentre.y + halfViewHeight);

    const float relativeLeft   = -0.5f * viewSize.x;
    const float relativeRight  =  0.5f * viewSize.x;
    const float relativeBottom = -0.5f * viewSize.y;
    const float relativeTop    =  0.5f * viewSize.y;

    const AffineTransform worldToComponent = AffineTransform::fromTargetPoints (relativeLeft,  relativeTop,
                                                                                  targetArea.getX(),     targetArea.getY(),
                                                                                  relativeRight, relativeTop,
                                                                                  targetArea.getRight(), targetArea.getY(),
                                                                                  relativeLeft,  relativeBottom,
                                                                                  targetArea.getX(),     targetArea.getBottom());

    Graphics::ScopedSaveState saveState (graphics);
    graphics.reduceClipRegion (targetArea.toNearestIntEdges());

    RenderCallContext callContext { graphics, *this, worldToComponent, viewCentre };

    b2DebugDraw drawCopy = debugDraw;
    installDebugDrawCallbacks (drawCopy);
    drawCopy.context = &callContext;
    drawCopy.drawingBounds.lowerBound.x = worldLeft;
    drawCopy.drawingBounds.lowerBound.y = worldBottom;
    drawCopy.drawingBounds.upperBound.x = worldRight;
    drawCopy.drawingBounds.upperBound.y = worldTop;

    b2World_Draw (worldId, &drawCopy);
}

} // namespace juce
