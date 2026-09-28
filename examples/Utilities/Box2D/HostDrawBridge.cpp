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
#include "HostDrawBridgeApi.h"
#include "UpstreamBridge.h"

namespace Box2DSamples
{

DrawList* activeHostDrawList = nullptr;

void setActiveHostDrawList (DrawList* drawList) noexcept
{
    activeHostDrawList = drawList;
}

} // namespace Box2DSamples

extern "C"
{

void* box2dHostGetActiveDrawList (void)
{
    return Box2DSamples::activeHostDrawList;
}

float* box2dHostGetScreenTextY (void)
{
    static float screenTextY = 24.0f;
    return &screenTextY;
}

void box2dHostDrawListAddPoint (void* drawList, b2Pos position, float size, b2HexColor colour)
{
    if (auto* list = static_cast<Box2DSamples::DrawList*> (drawList))
        list->addPoint (position, size, colour);
}

void box2dHostDrawListAddLine (void* drawList, b2Pos start, b2Pos end, b2HexColor colour)
{
    if (auto* list = static_cast<Box2DSamples::DrawList*> (drawList))
        list->addLine (start, end, colour);
}

void box2dHostDrawListAddCircle (void* drawList, b2Pos centre, float radius, b2HexColor colour)
{
    if (auto* list = static_cast<Box2DSamples::DrawList*> (drawList))
        list->addCircle (centre, radius, colour);
}

void box2dHostDrawListAddCapsule (void* drawList, b2Pos start, b2Pos end, float radius, b2HexColor colour)
{
    if (auto* list = static_cast<Box2DSamples::DrawList*> (drawList))
        list->addCapsule (start, end, radius, colour);
}

void box2dHostDrawListAddScreenText (void* drawList, float x, float y, b2HexColor colour, const char* text)
{
    if (auto* list = static_cast<Box2DSamples::DrawList*> (drawList))
        list->addScreenText ({ x, y }, colour, String (CharPointer_UTF8 (text)));
}

void box2dHostDrawListAddWorldText (void* drawList, b2Pos position, b2HexColor colour, const char* text)
{
    if (auto* list = static_cast<Box2DSamples::DrawList*> (drawList))
        list->addWorldText (position, colour, String (CharPointer_UTF8 (text)));
}

void box2dHostResetScreenTextY (float value)
{
    if (auto* screenTextY = box2dHostGetScreenTextY())
        *screenTextY = value;
}

void box2dHostAdvanceScreenTextY (float delta)
{
    if (auto* screenTextY = box2dHostGetScreenTextY())
        *screenTextY += delta;
}

void box2dHostDrawListAddPolygon (void* drawList, b2WorldTransform transform, const b2Vec2* vertices, int numVertices, b2HexColor colour)
{
    if (auto* list = static_cast<Box2DSamples::DrawList*> (drawList))
        list->addPolygon (transform, vertices, numVertices, colour);
}

void box2dHostDrawListAddSolidCircle (void* drawList, b2WorldTransform transform, b2Vec2 centre, float radius, b2HexColor colour)
{
    if (auto* list = static_cast<Box2DSamples::DrawList*> (drawList))
        list->addSolidCircle (transform, centre, radius, colour);
}

void box2dHostDrawListAddSolidPolygon (void* drawList, b2WorldTransform transform, const b2Vec2* vertices, int numVertices, float radius, b2HexColor colour)
{
    if (auto* list = static_cast<Box2DSamples::DrawList*> (drawList))
        list->addSolidPolygon (transform, vertices, numVertices, radius, colour);
}

void box2dHostDrawListAddTransform (void* drawList, b2WorldTransform transform, float scale)
{
    if (auto* list = static_cast<Box2DSamples::DrawList*> (drawList))
        list->addTransform (transform, scale);
}

void box2dHostDrawListAddBounds (void* drawList, b2AABB bounds, b2HexColor colour)
{
    if (auto* list = static_cast<Box2DSamples::DrawList*> (drawList))
        list->addBounds (bounds, colour);
}

} // extern "C"
