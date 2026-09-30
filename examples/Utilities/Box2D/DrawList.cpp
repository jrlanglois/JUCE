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

class DrawList::Pimpl
{
public:
    struct Command
    {
        enum class Type
        {
            point,
            line,
            circle,
            capsule,
            polygon,
            solidCircle,
            solidPolygon,
            transformMarker,
            bounds,
            worldText,
            screenText
        };

        Type type = Type::point;
        b2HexColor colour = b2_colorWhite;
        b2Pos posA {}, posB {};
        b2WorldTransform transform {};
        float radius = 0.0f;
        float scale = 1.0f;
        float diameter = 1.0f;
        String text;
        juce::Point<float> screenPosition;
        std::vector<b2Vec2> vertices;
    };

    std::vector<Command> commands;
    Box2DRenderer colourHelper;

    b2DebugDraw debugDraw = b2DefaultDebugDraw();

    Pimpl()
    {
        debugDraw.DrawPointFcn = &Pimpl::drawPointCallback;
        debugDraw.DrawLineFcn = &Pimpl::drawSegmentCallback;
        debugDraw.DrawCircleFcn = &Pimpl::drawCircleCallback;
        debugDraw.DrawSolidCircleFcn = &Pimpl::drawSolidCircleCallback;
        debugDraw.DrawPolygonFcn = &Pimpl::drawPolygonCallback;
        debugDraw.DrawSolidPolygonFcn = &Pimpl::drawSolidPolygonCallback;
        debugDraw.DrawTransformFcn = &Pimpl::drawTransformCallback;
        debugDraw.context = this;
    }

    static void drawPointCallback (b2Pos position, float size, b2HexColor colour, void* context)
    {
        if (auto* self = static_cast<Pimpl*> (context))
            self->appendPoint (position, size, colour);
    }

    static void drawSegmentCallback (b2Pos p1, b2Pos p2, b2HexColor colour, void* context)
    {
        if (auto* self = static_cast<Pimpl*> (context))
            self->appendLine (p1, p2, colour);
    }

    static void drawCircleCallback (b2Pos center, float radius, b2HexColor colour, void* context)
    {
        if (auto* self = static_cast<Pimpl*> (context))
            self->appendCircle (center, radius, colour);
    }

    static void drawSolidCircleCallback (b2WorldTransform transform, b2Vec2 center, float radius, b2HexColor colour, void* context)
    {
        if (auto* self = static_cast<Pimpl*> (context))
            self->appendSolidCircle (transform, center, radius, colour);
    }

    static void drawPolygonCallback (b2WorldTransform transform, const b2Vec2* vertices, int vertexCount, b2HexColor colour, void* context)
    {
        if (auto* self = static_cast<Pimpl*> (context))
            self->appendPolygon (transform, vertices, vertexCount, colour);
    }

    static void drawSolidPolygonCallback (b2WorldTransform transform, const b2Vec2* vertices, int vertexCount, float radius, b2HexColor colour, void* context)
    {
        if (auto* self = static_cast<Pimpl*> (context))
            self->appendSolidPolygon (transform, vertices, vertexCount, radius, colour);
    }

    static void drawTransformCallback (b2WorldTransform transform, void* context)
    {
        if (auto* self = static_cast<Pimpl*> (context))
            self->appendTransform (transform, 1.0f);
    }

    void appendPoint (b2Pos position, float size, b2HexColor colour)
    {
        Command command;
        command.type = Command::Type::point;
        command.posA = position;
        command.diameter = size;
        command.colour = colour;
        commands.push_back (std::move (command));
    }

    void appendLine (b2Pos start, b2Pos end, b2HexColor colour)
    {
        Command command;
        command.type = Command::Type::line;
        command.posA = start;
        command.posB = end;
        command.colour = colour;
        commands.push_back (std::move (command));
    }

    void appendCircle (b2Pos centre, float radius, b2HexColor colour)
    {
        Command command;
        command.type = Command::Type::circle;
        command.posA = centre;
        command.radius = radius;
        command.colour = colour;
        commands.push_back (std::move (command));
    }

    void appendCapsule (b2Pos start, b2Pos end, float radius, b2HexColor colour)
    {
        Command command;
        command.type = Command::Type::capsule;
        command.posA = start;
        command.posB = end;
        command.radius = radius;
        command.colour = colour;
        commands.push_back (std::move (command));
    }

    void appendPolygon (b2WorldTransform transform, const b2Vec2* vertices, int numVertices, b2HexColor colour)
    {
        Command command;
        command.type = Command::Type::polygon;
        command.transform = transform;
        command.colour = colour;
        command.vertices.assign (vertices, vertices + numVertices);
        commands.push_back (std::move (command));
    }

    void appendSolidCircle (b2WorldTransform transform, b2Vec2 centre, float radius, b2HexColor colour)
    {
        Command command;
        command.type = Command::Type::solidCircle;
        command.transform = transform;
        command.posA = { centre.x, centre.y };
        command.radius = radius;
        command.colour = colour;
        commands.push_back (std::move (command));
    }

    void appendSolidPolygon (b2WorldTransform transform, const b2Vec2* vertices, int numVertices, float radius, b2HexColor colour)
    {
        Command command;
        command.type = Command::Type::solidPolygon;
        command.transform = transform;
        command.radius = radius;
        command.colour = colour;
        command.vertices.assign (vertices, vertices + numVertices);
        commands.push_back (std::move (command));
    }

    void appendTransform (b2WorldTransform transform, float scale)
    {
        Command command;
        command.type = Command::Type::transformMarker;
        command.transform = transform;
        command.scale = scale;
        commands.push_back (std::move (command));
    }

    void appendBounds (b2AABB bounds, b2HexColor colour)
    {
        Command command;
        command.type = Command::Type::bounds;
        command.posA = { bounds.lowerBound.x, bounds.lowerBound.y };
        command.posB = { bounds.upperBound.x, bounds.upperBound.y };
        command.colour = colour;
        commands.push_back (std::move (command));
    }

    void appendWorldText (b2Pos position, b2HexColor colour, const String& text)
    {
        Command command;
        command.type = Command::Type::worldText;
        command.posA = position;
        command.colour = colour;
        command.text = text;
        commands.push_back (std::move (command));
    }

    void appendScreenText (juce::Point<float> position, b2HexColor colour, const String& text)
    {
        Command command;
        command.type = Command::Type::screenText;
        command.screenPosition = position;
        command.colour = colour;
        command.text = text;
        commands.push_back (std::move (command));
    }

    void renderCommand (Graphics& graphics, const Camera& camera, const juce::Rectangle<float>& targetArea, const Command& command) const
    {
        graphics.setColour (colourHelper.getColour (command.colour));

        switch (command.type)
        {
            case Command::Type::point:
            {
                const auto point = camera.convertWorldToComponent (command.posA, targetArea);
                graphics.fillEllipse (point.x - command.diameter * 0.5f,
                                      point.y - command.diameter * 0.5f,
                                      command.diameter,
                                      command.diameter);
                break;
            }

            case Command::Type::line:
            {
                const auto start = camera.convertWorldToComponent (command.posA, targetArea);
                const auto end = camera.convertWorldToComponent (command.posB, targetArea);
                graphics.drawLine ({ start.x, start.y, end.x, end.y }, colourHelper.getLineThickness());
                break;
            }

            case Command::Type::circle:
            {
                const auto centre = camera.convertWorldToComponent (command.posA, targetArea);
                const auto viewSize = camera.getViewSize();
                const auto scale = targetArea.getWidth() / viewSize.x;
                const auto diameter = 2.0f * command.radius * scale;
                graphics.drawEllipse (centre.x - diameter * 0.5f,
                                      centre.y - diameter * 0.5f,
                                      diameter,
                                      diameter,
                                      colourHelper.getLineThickness());
                break;
            }

            case Command::Type::capsule:
            {
                const auto start = camera.convertWorldToComponent (command.posA, targetArea);
                const auto end = camera.convertWorldToComponent (command.posB, targetArea);
                const auto viewSize = camera.getViewSize();
                const auto worldScale = std::min (targetArea.getWidth() / viewSize.x, targetArea.getHeight() / viewSize.y);
                const auto diameter = 2.0f * command.radius * worldScale;
                graphics.drawLine ({ start.x, start.y, end.x, end.y }, diameter);
                graphics.drawEllipse (start.x - diameter * 0.5f, start.y - diameter * 0.5f, diameter, diameter, colourHelper.getLineThickness());
                graphics.drawEllipse (end.x - diameter * 0.5f, end.y - diameter * 0.5f, diameter, diameter, colourHelper.getLineThickness());
                break;
            }

            case Command::Type::bounds:
            {
                const auto topLeft = camera.convertWorldToComponent ({ command.posA.x, command.posB.y }, targetArea);
                const auto bottomRight = camera.convertWorldToComponent ({ command.posB.x, command.posA.y }, targetArea);
                graphics.drawRect (juce::Rectangle<float>::leftTopRightBottom (topLeft.x, topLeft.y, bottomRight.x, bottomRight.y),
                                   colourHelper.getLineThickness());
                break;
            }

            case Command::Type::worldText:
            {
                const auto position = camera.convertWorldToComponent (command.posA, targetArea);
                graphics.drawText (command.text, juce::Rectangle<float> (position.x, position.y, 200.0f, 20.0f), Justification::centredLeft, false);
                break;
            }

            case Command::Type::screenText:
            {
                graphics.drawText (command.text,
                                   juce::Rectangle<float> (targetArea.getX() + command.screenPosition.x,
                                                           targetArea.getY() + command.screenPosition.y,
                                                           400.0f,
                                                           20.0f),
                                   Justification::centredLeft,
                                   false);
                break;
            }

            case Command::Type::polygon:
            {
                if (command.vertices.size() < 2)
                    break;

                Path path;

                for (int vertexIndex = 0; vertexIndex < (int) command.vertices.size(); ++vertexIndex)
                {
                    const auto worldVertex = b2TransformWorldPoint (command.transform, command.vertices[(size_t) vertexIndex]);
                    const auto componentVertex = camera.convertWorldToComponent (worldVertex, targetArea);

                    if (vertexIndex == 0)
                        path.startNewSubPath (componentVertex);
                    else
                        path.lineTo (componentVertex);
                }

                path.closeSubPath();
                graphics.strokePath (path, PathStrokeType (colourHelper.getLineThickness()));
                break;
            }

            case Command::Type::solidCircle:
            {
                const auto worldCentre = b2TransformWorldPoint (command.transform, { (float) command.posA.x, (float) command.posA.y });
                const auto componentCentre = camera.convertWorldToComponent (worldCentre, targetArea);
                const auto viewSize = camera.getViewSize();
                const auto scale = targetArea.getWidth() / viewSize.x;
                const auto diameter = 2.0f * command.radius * scale;
                graphics.fillEllipse (componentCentre.x - diameter * 0.5f,
                                      componentCentre.y - diameter * 0.5f,
                                      diameter,
                                      diameter);
                break;
            }

            case Command::Type::solidPolygon:
            {
                if (command.vertices.size() < 3)
                    break;

                Path path;

                for (int vertexIndex = 0; vertexIndex < (int) command.vertices.size(); ++vertexIndex)
                {
                    const auto worldVertex = b2TransformWorldPoint (command.transform, command.vertices[(size_t) vertexIndex]);
                    const auto componentVertex = camera.convertWorldToComponent (worldVertex, targetArea);

                    if (vertexIndex == 0)
                        path.startNewSubPath (componentVertex);
                    else
                        path.lineTo (componentVertex);
                }

                path.closeSubPath();
                graphics.fillPath (path);
                break;
            }

            case Command::Type::transformMarker:
            {
                const auto axisLength = command.scale;
                const auto origin = b2TransformWorldPoint (command.transform, b2Vec2_zero);
                const auto xAxis = b2TransformWorldPoint (command.transform, { axisLength, 0.0f });
                const auto yAxis = b2TransformWorldPoint (command.transform, { 0.0f, axisLength });
                const auto originComponent = camera.convertWorldToComponent (origin, targetArea);
                const auto xComponent = camera.convertWorldToComponent (xAxis, targetArea);
                const auto yComponent = camera.convertWorldToComponent (yAxis, targetArea);
                graphics.drawLine ({ originComponent.x, originComponent.y, xComponent.x, xComponent.y }, colourHelper.getLineThickness());
                graphics.drawLine ({ originComponent.x, originComponent.y, yComponent.x, yComponent.y }, colourHelper.getLineThickness());
                break;
            }
        }
    }
};

DrawList::DrawList() : pimpl (std::make_unique<Pimpl>()) {}

DrawList::~DrawList() = default;

void DrawList::clear()
{
    pimpl->commands.clear();
}

void DrawList::addPoint (b2Pos position, float diameter, b2HexColor colour)
{
    pimpl->appendPoint (position, diameter, colour);
}

void DrawList::addLine (b2Pos start, b2Pos end, b2HexColor colour)
{
    pimpl->appendLine (start, end, colour);
}

void DrawList::addCircle (b2Pos centre, float radius, b2HexColor colour)
{
    pimpl->appendCircle (centre, radius, colour);
}

void DrawList::addCapsule (b2Pos start, b2Pos end, float radius, b2HexColor colour)
{
    pimpl->appendCapsule (start, end, radius, colour);
}

void DrawList::addPolygon (b2WorldTransform transform, const b2Vec2* vertices, int numVertices, b2HexColor colour)
{
    pimpl->appendPolygon (transform, vertices, numVertices, colour);
}

void DrawList::addSolidCircle (b2WorldTransform transform, b2Vec2 centre, float radius, b2HexColor colour)
{
    pimpl->appendSolidCircle (transform, centre, radius, colour);
}

void DrawList::addSolidPolygon (b2WorldTransform transform, const b2Vec2* vertices, int numVertices, float radius, b2HexColor colour)
{
    pimpl->appendSolidPolygon (transform, vertices, numVertices, radius, colour);
}

void DrawList::addTransform (b2WorldTransform transform, float scale)
{
    pimpl->appendTransform (transform, scale);
}

void DrawList::addBounds (b2AABB bounds, b2HexColor colour)
{
    pimpl->appendBounds (bounds, colour);
}

void DrawList::addWorldText (b2Pos position, b2HexColor colour, const String& text)
{
    pimpl->appendWorldText (position, colour, text);
}

void DrawList::addScreenText (juce::Point<float> position, b2HexColor colour, const String& text)
{
    pimpl->appendScreenText (position, colour, text);
}

void DrawList::render (Graphics& graphics, const Camera& camera, const juce::Rectangle<float>& targetArea) const
{
    graphics.saveState();
    graphics.reduceClipRegion (targetArea.toNearestInt());

    for (const auto& command : pimpl->commands)
        pimpl->renderCommand (graphics, camera, targetArea, command);

    graphics.restoreState();
}

b2DebugDraw& DrawList::getDebugDraw() noexcept
{
    return pimpl->debugDraw;
}

void DrawList::append (const DrawList& other)
{
    pimpl->commands.insert (pimpl->commands.end(), other.pimpl->commands.begin(), other.pimpl->commands.end());
}

} // namespace Box2DSamples
