// SPDX-FileCopyrightText: 2023 Erin Catto
// SPDX-License-Identifier: MIT

#pragma once

#include "Box2DHostInclude.h"

typedef struct Camera
{
	b2Pos center;
	float zoom;
	float width;
	float height;
} Camera;

typedef struct Draw Draw;

#ifdef __cplusplus
extern "C"
{
#endif

Camera GetDefaultCamera( void );
void ResetView( Camera* camera );
b2Pos ConvertScreenToWorld( Camera* camera, b2Vec2 screenPoint );
b2Vec2 ConvertWorldToScreen( Camera* camera, b2Pos worldPoint );
b2Vec2 ConvertViewToScreen( Camera* camera, b2Vec2 viewPoint );
b2AABB GetViewBounds( Camera* camera );
void FocusOnBounds( Camera* camera, b2AABB bounds );

Draw* CreateDraw( void );
void DestroyDraw( Draw* draw );

void DrawScreenString( Draw* draw, float x, float y, b2HexColor color, const char* string, ... );

void SetDrawOrigin( Draw* draw, b2Pos origin );

void DrawPoint( Draw* draw, b2Pos p, float size, b2HexColor color );
void DrawLine( Draw* draw, b2Pos p1, b2Pos p2, b2HexColor color );
void DrawCircle( Draw* draw, b2Pos center, float radius, b2HexColor color );
void DrawCapsule( Draw* draw, b2Pos p1, b2Pos p2, float radius, b2HexColor color );
void DrawPolygon( Draw* draw, b2WorldTransform transform, const b2Vec2* vertices, int vertexCount, b2HexColor color );
void DrawSolidCircle( Draw* draw, b2WorldTransform transform, b2Vec2 center, float radius, b2HexColor color );
void DrawSolidPolygon( Draw* draw, b2WorldTransform transform, const b2Vec2* vertices, int vertexCount, float radius,
							b2HexColor color );
void DrawTransform( Draw* draw, b2WorldTransform transform, float scale );
void DrawBounds( Draw* draw, b2AABB aabb, b2HexColor color );
void DrawString( Draw* draw, Camera* camera, b2Pos p, b2HexColor color, const char* string, ... );

void FlushDraw( Draw* draw, Camera* camera );
void DrawBackground( Draw* draw, Camera* camera );

#ifdef __cplusplus
}
#endif
