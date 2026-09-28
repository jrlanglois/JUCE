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

#pragma once

#include "Box2DHostInclude.h"

#ifdef __cplusplus
extern "C"
{
#endif

void* box2dHostGetActiveDrawList (void);

float* box2dHostGetScreenTextY (void);

void box2dHostDrawListAddScreenText (void* drawList, float x, float y, b2HexColor colour, const char* text);

void box2dHostDrawListAddPoint (void* drawList, b2Pos position, float size, b2HexColor colour);

void box2dHostDrawListAddLine (void* drawList, b2Pos start, b2Pos end, b2HexColor colour);

void box2dHostDrawListAddCircle (void* drawList, b2Pos centre, float radius, b2HexColor colour);

void box2dHostDrawListAddCapsule (void* drawList, b2Pos start, b2Pos end, float radius, b2HexColor colour);

void box2dHostDrawListAddPolygon (void* drawList, b2WorldTransform transform, const b2Vec2* vertices, int numVertices, b2HexColor colour);

void box2dHostDrawListAddSolidCircle (void* drawList, b2WorldTransform transform, b2Vec2 centre, float radius, b2HexColor colour);

void box2dHostDrawListAddSolidPolygon (void* drawList, b2WorldTransform transform, const b2Vec2* vertices, int numVertices, float radius, b2HexColor colour);

void box2dHostDrawListAddTransform (void* drawList, b2WorldTransform transform, float scale);

void box2dHostDrawListAddBounds (void* drawList, b2AABB bounds, b2HexColor colour);

void box2dHostDrawListAddWorldText (void* drawList, b2Pos position, b2HexColor colour, const char* text);

void box2dHostResetScreenTextY (float value);

void box2dHostAdvanceScreenTextY (float delta);

#ifdef __cplusplus
}
#endif
