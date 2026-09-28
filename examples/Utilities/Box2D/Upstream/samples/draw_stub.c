// SPDX-FileCopyrightText: 2026 Raw Material Software Limited
// SPDX-License-Identifier: ISC

#include <float.h>
#include <math.h>

#include "draw.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "HostDrawBridgeApi.h"

struct Draw
{
	b2Pos origin;
};

Camera GetDefaultCamera( void )
{
	Camera camera = { 0 };
	camera.center = (b2Pos){ 0.0f, 20.0f };
	camera.zoom = 1.0f;
	camera.width = 1920.0f;
	camera.height = 1080.0f;
	return camera;
}

void ResetView( Camera* camera )
{
	if ( camera == NULL )
	{
		return;
	}

	camera->center = (b2Pos){ 0.0f, 20.0f };
	camera->zoom = 1.0f;
}

b2Pos ConvertScreenToWorld( Camera* camera, b2Vec2 screenPoint )
{
	if ( camera == NULL || camera->width <= 0.0f || camera->height <= 0.0f )
	{
		return (b2Pos){ 0.0f, 0.0f };
	}

	float u = screenPoint.x / camera->width;
	float v = ( camera->height - screenPoint.y ) / camera->height;
	float ratio = camera->width / camera->height;
	b2Vec2 extents = { camera->zoom * ratio, camera->zoom };
	b2Vec2 offset = { extents.x * ( 2.0f * u - 1.0f ), extents.y * ( 2.0f * v - 1.0f ) };
	return b2OffsetPos( camera->center, offset );
}

b2Vec2 ConvertWorldToScreen( Camera* camera, b2Pos worldPoint )
{
	if ( camera == NULL )
	{
		return (b2Vec2){ 0.0f, 0.0f };
	}

	return ConvertViewToScreen( camera, b2SubPos( worldPoint, camera->center ) );
}

b2Vec2 ConvertViewToScreen( Camera* camera, b2Vec2 viewPoint )
{
	if ( camera == NULL || camera->zoom <= 0.0f || camera->width <= 0.0f || camera->height <= 0.0f )
	{
		return (b2Vec2){ 0.0f, 0.0f };
	}

	float ratio = camera->width / camera->height;
	b2Vec2 extents = { camera->zoom * ratio, camera->zoom };
	float u = ( viewPoint.x + extents.x ) / ( 2.0f * extents.x );
	float v = ( viewPoint.y + extents.y ) / ( 2.0f * extents.y );
	return (b2Vec2){ u * camera->width, ( 1.0f - v ) * camera->height };
}

b2AABB GetViewBounds( Camera* camera )
{
	if ( camera == NULL || camera->width <= 0.0f || camera->height <= 0.0f )
	{
		return (b2AABB){ (b2Vec2){ 0.0f, 0.0f }, (b2Vec2){ 0.0f, 0.0f } };
	}

	b2Pos lower = ConvertScreenToWorld( camera, (b2Vec2){ 0.0f, camera->height } );
	b2Pos upper = ConvertScreenToWorld( camera, (b2Vec2){ camera->width, 0.0f } );
	return (b2AABB){ (b2Vec2){ b2RoundDownFloat( lower.x ), b2RoundDownFloat( lower.y ) },
					 (b2Vec2){ b2RoundUpFloat( upper.x ), b2RoundUpFloat( upper.y ) } };
}

void FocusOnBounds( Camera* camera, b2AABB bounds )
{
	if ( camera == NULL || camera->width <= 0.0f || camera->height <= 0.0f )
	{
		return;
	}

	b2Vec2 extents = b2AABB_Extents( bounds );
	if ( extents.x < B2_LINEAR_SLOP || extents.y < B2_LINEAR_SLOP )
	{
		return;
	}

	float invRatio = camera->height / camera->width;
	camera->zoom = b2MaxFloat( extents.x * invRatio, extents.y );
	camera->zoom = b2MaxFloat( camera->zoom, 0.01f );
	camera->center = b2ToPos( b2AABB_Center( bounds ) );
}

Draw* CreateDraw( void )
{
	Draw* draw = (Draw*)malloc( sizeof( Draw ) );
	if ( draw != NULL )
	{
		memset( draw, 0, sizeof( *draw ) );
	}

	return draw;
}

void DestroyDraw( Draw* draw )
{
	free( draw );
}

void SetDrawOrigin( Draw* draw, b2Pos origin )
{
	if ( draw != NULL )
	{
		draw->origin = origin;
	}
}

static void formatString( char* buffer, size_t bufferSize, const char* string, va_list args )
{
	vsnprintf( buffer, bufferSize, string, args );
	buffer[bufferSize - 1] = 0;
}

void DrawScreenString( Draw* draw, float x, float y, b2HexColor color, const char* string, ... )
{
	(void)draw;
	char buffer[256];
	va_list args;
	va_start( args, string );
	formatString( buffer, sizeof( buffer ), string, args );
	va_end( args );

	void* hostDrawList = box2dHostGetActiveDrawList();
	if ( hostDrawList != NULL )
	{
		box2dHostDrawListAddScreenText( hostDrawList, x, y, color, buffer );
	}
}

void DrawPoint( Draw* draw, b2Pos p, float size, b2HexColor color )
{
	(void)draw;
	void* hostDrawList = box2dHostGetActiveDrawList();
	if ( hostDrawList != NULL )
	{
		box2dHostDrawListAddPoint( hostDrawList, p, size, color );
	}
}

void DrawLine( Draw* draw, b2Pos p1, b2Pos p2, b2HexColor color )
{
	(void)draw;
	void* hostDrawList = box2dHostGetActiveDrawList();
	if ( hostDrawList != NULL )
	{
		box2dHostDrawListAddLine( hostDrawList, p1, p2, color );
	}
}

void DrawCircle( Draw* draw, b2Pos center, float radius, b2HexColor color )
{
	(void)draw;
	void* hostDrawList = box2dHostGetActiveDrawList();
	if ( hostDrawList != NULL )
	{
		box2dHostDrawListAddCircle( hostDrawList, center, radius, color );
	}
}

void DrawCapsule( Draw* draw, b2Pos p1, b2Pos p2, float radius, b2HexColor color )
{
	(void)draw;
	void* hostDrawList = box2dHostGetActiveDrawList();
	if ( hostDrawList != NULL )
	{
		box2dHostDrawListAddCapsule( hostDrawList, p1, p2, radius, color );
	}
}

void DrawPolygon( Draw* draw, b2WorldTransform transform, const b2Vec2* vertices, int vertexCount, b2HexColor color )
{
	(void)draw;
	void* hostDrawList = box2dHostGetActiveDrawList();
	if ( hostDrawList != NULL )
	{
		box2dHostDrawListAddPolygon( hostDrawList, transform, vertices, vertexCount, color );
	}
}

void DrawSolidCircle( Draw* draw, b2WorldTransform transform, b2Vec2 center, float radius, b2HexColor color )
{
	(void)draw;
	void* hostDrawList = box2dHostGetActiveDrawList();
	if ( hostDrawList != NULL )
	{
		box2dHostDrawListAddSolidCircle( hostDrawList, transform, center, radius, color );
	}
}

void DrawSolidPolygon( Draw* draw, b2WorldTransform transform, const b2Vec2* vertices, int vertexCount, float radius,
					   b2HexColor color )
{
	(void)draw;
	void* hostDrawList = box2dHostGetActiveDrawList();
	if ( hostDrawList != NULL )
	{
		box2dHostDrawListAddSolidPolygon( hostDrawList, transform, vertices, vertexCount, radius, color );
	}
}

void DrawTransform( Draw* draw, b2WorldTransform transform, float scale )
{
	(void)draw;
	void* hostDrawList = box2dHostGetActiveDrawList();
	if ( hostDrawList != NULL )
	{
		box2dHostDrawListAddTransform( hostDrawList, transform, scale );
	}
}

void DrawBounds( Draw* draw, b2AABB aabb, b2HexColor color )
{
	(void)draw;
	void* hostDrawList = box2dHostGetActiveDrawList();
	if ( hostDrawList != NULL )
	{
		box2dHostDrawListAddBounds( hostDrawList, aabb, color );
	}
}

void DrawString( Draw* draw, Camera* camera, b2Pos p, b2HexColor color, const char* string, ... )
{
	(void)draw;
	(void)camera;
	char buffer[256];
	va_list args;
	va_start( args, string );
	formatString( buffer, sizeof( buffer ), string, args );
	va_end( args );

	void* hostDrawList = box2dHostGetActiveDrawList();
	if ( hostDrawList != NULL )
	{
		box2dHostDrawListAddWorldText( hostDrawList, p, color, buffer );
	}
}

void FlushDraw( Draw* draw, Camera* camera )
{
	(void)draw;
	(void)camera;
}

void DrawBackground( Draw* draw, Camera* camera )
{
	(void)draw;
	(void)camera;
}
