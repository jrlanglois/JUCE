// SPDX-FileCopyrightText: 2026 Erin Catto
// SPDX-License-Identifier: MIT

#if defined( _MSC_VER ) && !defined( _CRT_SECURE_NO_WARNINGS )
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "sample.h"
#include "host_controls.h"

#include "benchmarks.h"
#include "draw.h"
#include "utils.h"

// consider using https://github.com/skeeto/pdjson

#include "Box2DHostInclude.h"

#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#define INFO_PANEL_WIDTH 16.0f

void SampleContext::Save()
{
}



void DrawPolygonFcn( b2WorldTransform transform, const b2Vec2* vertices, int vertexCount, b2HexColor color, void* context )
{
	SampleContext* sampleContext = static_cast<SampleContext*>( context );
	DrawPolygon( sampleContext->draw, transform, vertices, vertexCount, color );
}

void DrawSolidPolygonFcn( b2WorldTransform transform, const b2Vec2* vertices, int vertexCount, float radius, b2HexColor color,
						  void* context )
{
	SampleContext* sampleContext = static_cast<SampleContext*>( context );
	DrawSolidPolygon( sampleContext->draw, transform, vertices, vertexCount, radius, color );
}

void DrawCircleFcn( b2Pos center, float radius, b2HexColor color, void* context )
{
	SampleContext* sampleContext = static_cast<SampleContext*>( context );
	DrawCircle( sampleContext->draw, center, radius, color );
}

void DrawSolidCircleFcn( b2WorldTransform transform, b2Vec2 center, float radius, b2HexColor color, void* context )
{
	SampleContext* sampleContext = static_cast<SampleContext*>( context );
	DrawSolidCircle( sampleContext->draw, transform, center, radius, color );
}

void DrawSolidCapsuleFcn( b2Pos p1, b2Pos p2, float radius, b2HexColor color, void* context )
{
	SampleContext* sampleContext = static_cast<SampleContext*>( context );
	DrawCapsule( sampleContext->draw, p1, p2, radius, color );
}

void DrawLineFcn( b2Pos p1, b2Pos p2, b2HexColor color, void* context )
{
	SampleContext* sampleContext = static_cast<SampleContext*>( context );
	DrawLine( sampleContext->draw, p1, p2, color );
}

void DrawTransformFcn( b2WorldTransform transform, void* context )
{
	SampleContext* sampleContext = static_cast<SampleContext*>( context );
	DrawTransform( sampleContext->draw, transform, 1.0f );
}

void DrawPointFcn( b2Pos p, float size, b2HexColor color, void* context )
{
	SampleContext* sampleContext = static_cast<SampleContext*>( context );
	DrawPoint( sampleContext->draw, p, size, color );
}

void DrawStringFcn( b2Pos p, const char* s, b2HexColor color, void* context )
{
	SampleContext* sampleContext = static_cast<SampleContext*>( context );
	DrawString( sampleContext->draw, &sampleContext->camera, p, color, "%s", s );
}

void DrawBoundsFcn( b2AABB aabb, b2HexColor color, void* context )
{
	SampleContext* sampleContext = static_cast<SampleContext*>( context );
	DrawBounds( sampleContext->draw, aabb, color );
}

#define MAX_TOKENS 32

void SampleContext::Load()
{
	camera = GetDefaultCamera();
	debugDraw = b2DefaultDebugDraw();
	debugDraw.context = this;
	recycleDistance = B2_CONTACT_RECYCLE_DISTANCE;
}


static void TestMathCpp()
{
	b2Vec2 a = { 1.0f, 2.0f };
	b2Vec2 b = { 3.0f, 4.0f };

	b2Vec2 c = a;
	c += b;
	c -= b;
	c *= 2.0f;
	c = -a;
	c = c + b;
	c = c - a;
	c = 2.0f * a;
	c = a * 2.0f;

	if ( b == a )
	{
		c = a;
	}

	if ( b != a )
	{
		c = b;
	}

	c += c;
}

Sample::Sample( SampleContext* context, bool createWorld )
{
	m_context = context;
	m_camera = &context->camera;
	m_draw = context->draw;
	SetBenchmarkReducedWorkload( context->reducedWorkload );

	m_worldId = b2_nullWorldId;

	m_mouseJointId = b2_nullJointId;

	m_stepCount = 0;
	m_didStep = false;
	m_screenTextX = 5.0f;
	m_screenTextY = 0.0f;

	m_mouseBodyId = b2_nullBodyId;
	m_mousePoint = {};
	m_mouseForceScale = 100.0f;

	memset( m_profiles, 0, sizeof( m_profiles ) );
	m_currentProfileIndex = 0;
	m_profileReadIndex = 0;
	m_profileWriteIndex = 0;

	g_randomSeed = RAND_SEED;

	m_recording = nullptr;
	m_recordStartStep = 0;

	if ( createWorld )
	{
		CreateWorld();
	}
	TestMathCpp();
}

Sample::~Sample()
{
	if ( B2_IS_NON_NULL( m_worldId ) )
	{
		FinishRecording();
		b2DestroyWorld( m_worldId );
	}
}

void Sample::StartRecording()
{
	if ( m_recording != nullptr )
	{
		return;
	}

	uint64_t ticks = b2GetTicks();

	// Snapshots the live world as the seed, so recording can begin at any step boundary
	m_recording = b2CreateRecording( 0 );
	b2World_StartRecording( m_worldId, m_recording );
	m_recordStartStep = m_stepCount;

	float ms = b2GetMilliseconds( ticks );
	printf( "b2World_StartRecording took : %g ms", ms );
}

void Sample::FinishRecording()
{
	if ( m_recording == nullptr )
	{
		return;
	}

	b2World_StopRecording( m_worldId );
	if ( b2Recording_GetSize( m_recording ) > 0 )
	{
		snprintf( m_context->savedRecordingFile, sizeof( m_context->savedRecordingFile ), "%s", m_context->recordingFile );
	}
	b2DestroyRecording( m_recording );
	m_recording = nullptr;
}

void Sample::CreateWorld()
{
	if ( B2_IS_NON_NULL( m_worldId ) )
	{
		FinishRecording();
		b2DestroyWorld( m_worldId );
		m_worldId = b2_nullWorldId;
	}

	b2WorldDef worldDef = b2DefaultWorldDef();
	worldDef.workerCount = m_context->workerCount;
	worldDef.userTaskContext = this;
	worldDef.enableSleep = m_context->enableSleep;
	worldDef.capacity = m_context->capacity;
	m_worldId = b2CreateWorld( &worldDef );

	b2World_SetContactRecycleDistance( m_worldId, m_context->recycleDistance );
}

void Sample::ResetText()
{
	float fontSize = HostControls::getFontSize();
	if ( m_context->showUI )
	{
		m_screenTextY = HostControls::getFrameHeight() + 1.5f * fontSize;
	}
	else
	{
		m_screenTextY = 3.0f * fontSize;
	}

	m_screenTextX = 5.0f;
	if ( IsProfileVisible() )
	{
		m_screenTextX += GetProfilePanelWidth() + 0.5f * fontSize;
	}
}

struct QueryContext
{
	b2Pos point;
	b2BodyId bodyId = b2_nullBodyId;
};

bool QueryCallback( b2ShapeId shapeId, void* context )
{
	QueryContext* queryContext = static_cast<QueryContext*>( context );

	b2BodyId bodyId = b2Shape_GetBody( shapeId );
	b2BodyType bodyType = b2Body_GetType( bodyId );
	if ( bodyType != b2_dynamicBody )
	{
		// continue query
		return true;
	}

	bool overlap = b2Shape_TestPoint( shapeId, queryContext->point );
	if ( overlap )
	{
		// found shape
		queryContext->bodyId = bodyId;
		return false;
	}

	return true;
}

void Sample::MouseDown( b2Pos p, int button, int )
{
	if ( B2_IS_NON_NULL( m_mouseJointId ) )
	{
		return;
	}

	if ( button == HOST_MOUSE_BUTTON_PRIMARY )
	{
		// A tiny box around the click point, exact at any distance with the click as the origin
		b2Vec2 d = { 0.001f, 0.001f };
		b2AABB box = { b2Neg( d ), d };

		m_mousePoint = p;

		// Query the world for overlapping shapes.
		QueryContext queryContext = { p, b2_nullBodyId };
		b2World_OverlapAABB( m_worldId, p, box, b2DefaultQueryFilter(), QueryCallback, &queryContext );

		if ( B2_IS_NON_NULL( queryContext.bodyId ) )
		{
			b2BodyDef bodyDef = b2DefaultBodyDef();
			bodyDef.type = b2_kinematicBody;
			bodyDef.position = m_mousePoint;
			bodyDef.enableSleep = false;
			m_mouseBodyId = b2CreateBody( m_worldId, &bodyDef );

			b2MotorJointDef jointDef = b2DefaultMotorJointDef();
			jointDef.base.bodyIdA = m_mouseBodyId;
			jointDef.base.bodyIdB = queryContext.bodyId;
			jointDef.base.localFrameB.p = b2Body_GetLocalPoint( queryContext.bodyId, p );
			jointDef.linearHertz = 7.5f;
			jointDef.linearDampingRatio = 1.0f;

			b2MassData massData = b2Body_GetMassData( queryContext.bodyId );
			float g = b2Length( b2World_GetGravity( m_worldId ) );
			float mg = massData.mass * g;

			jointDef.maxSpringForce = m_mouseForceScale * mg;

			if ( massData.mass > 0.0f )
			{
				// This acts like angular friction
				float lever = sqrtf( massData.rotationalInertia / massData.mass );
				jointDef.maxVelocityTorque = 0.25f * lever * mg;
			}

			m_mouseJointId = b2CreateMotorJoint( m_worldId, &jointDef );
		}
	}
}

void Sample::MouseUp( b2Pos, int button )
{
	if ( B2_IS_NON_NULL( m_mouseJointId ) && button == HOST_MOUSE_BUTTON_PRIMARY )
	{
		b2DestroyJoint( m_mouseJointId );
		m_mouseJointId = b2_nullJointId;

		b2DestroyBody( m_mouseBodyId );
		m_mouseBodyId = b2_nullBodyId;
	}
}

void Sample::MouseMove( b2Pos p )
{
	if ( b2Joint_IsValid( m_mouseJointId ) == false )
	{
		// The world or attached body was destroyed.
		m_mouseJointId = b2_nullJointId;
	}

	m_mousePoint = p;
}

void Sample::DrawScreenTextLine( const char* text, ... )
{
	char buffer[256];
	va_list arg;
	va_start( arg, text );
	vsnprintf( buffer, sizeof( buffer ), text, arg );
	va_end( arg );
	buffer[sizeof( buffer ) - 1] = 0;
	DrawScreenString( m_draw, m_screenTextX, m_screenTextY, b2_colorWhite, "%s", buffer );
	m_screenTextY += 1.5f * HostControls::getFontSize();
}

float Sample::InfoPanelWidthEm() const
{
	return INFO_PANEL_WIDTH;
}

void Sample::FocusHome()
{
	m_context->camera.center = m_context->homeCenter;
	m_context->camera.zoom = m_context->homeZoom;
}

void Sample::ResetProfile()
{
	// Keeps the elapsed recording length intact across a profile reset
	m_recordStartStep -= m_stepCount;
	m_stepCount = 0;
	memset( m_profiles, 0, sizeof( m_profiles ) );
	m_currentProfileIndex = 0;
	m_profileReadIndex = 0;
	m_profileWriteIndex = 0;
}

void Sample::AdvanceSimulationTimeStep( float timeStep )
{
	m_didStep = false;

	if ( B2_IS_NON_NULL( m_mouseJointId ) && b2Joint_IsValid( m_mouseJointId ) == false )
	{
		m_mouseJointId = b2_nullJointId;

		if ( B2_IS_NON_NULL( m_mouseBodyId ) )
		{
			b2DestroyBody( m_mouseBodyId );
			m_mouseBodyId = b2_nullBodyId;
		}
	}

	if ( B2_IS_NON_NULL( m_mouseBodyId ) && timeStep > 0.0f )
	{
		bool wake = true;
		b2Body_SetTargetTransform( m_mouseBodyId, { m_mousePoint, b2Rot_identity }, timeStep, wake );
	}

	m_context->debugDraw.drawingBounds = GetViewBounds( &m_context->camera );

	b2World_EnableSleeping( m_worldId, m_context->enableSleep );
	b2World_EnableWarmStarting( m_worldId, m_context->enableWarmStarting );
	b2World_EnableContinuous( m_worldId, m_context->enableContinuous );
	b2World_SetRestitutionIterations( m_worldId, m_context->restitutionIterations );
	b2World_EnableRestitutionPropagation( m_worldId, m_context->enableRestitutionPropagation );

	for ( int i = 0; i < 1; ++i )
	{
		b2World_Step( m_worldId, timeStep, m_context->subStepCount );
	}

	if ( timeStep > 0.0f )
	{
		m_stepCount += 1;
		m_didStep = true;

		if ( m_profileWriteIndex == m_profileCapacity + m_profileReadIndex )
		{
			m_profileReadIndex += 1;
		}

		m_currentProfileIndex = static_cast<int>( m_profileWriteIndex & ( m_profileCapacity - 1 ) );
		m_profiles[m_currentProfileIndex] = b2World_GetProfile( m_worldId );

		m_profileWriteIndex += 1;
	}
}

void Sample::PreparePresentation()
{
}

void Sample::Step()
{
	m_didStep = false;

	float timeStep = 0.0f;
	if ( m_context->pause == false || m_context->singleStep > 0 )
	{
		timeStep = m_context->hertz > 0.0f ? 1.0f / m_context->hertz : 0.0f;
		m_context->singleStep = b2MaxInt( 0, m_context->singleStep - 1 );
	}

	AdvanceSimulationTimeStep( timeStep );
}


bool Sample::IsProfileVisible() const
{
	return false;
}

float Sample::GetProfilePanelWidth() const
{
	return 0.0f;
}

void Sample::DrawProfile()
{
}

void Sample::DrawMetrics()
{
}

void Sample::DrawHud( float frameTime )
{
	(void)frameTime;
	const SampleEntry& entry = g_sampleEntries[m_context->sampleIndex];
	DrawScreenString( m_context->draw, 5.0f, 24.0f, b2_colorYellow, "%s : %s", entry.category, entry.name );
}

void DrawUI( SampleContext* context, float frameTime )
{
	(void)context;
	(void)frameTime;
}

int Sample::ParsePath( const char* svgPath, b2Vec2 offset, b2Vec2* points, int capacity, float scale, bool reverseOrder )
{
	int pointCount = 0;
	b2Vec2 currentPoint = {};
	const char* ptr = svgPath;
	char command = *ptr;

	while ( *ptr != '\0' )
	{
		if ( isdigit( *ptr ) == 0 && *ptr != '-' )
		{
			// note: command can be implicitly repeated
			command = *ptr;

			if ( command == 'M' || command == 'L' || command == 'H' || command == 'V' || command == 'm' || command == 'l' ||
				 command == 'h' || command == 'v' )
			{
				ptr += 2; // Skip the command character and space
			}

			if ( command == 'z' )
			{
				break;
			}
		}

		assert( isdigit( *ptr ) != 0 || *ptr == '-' );

		float x = 0.0f, y = 0.0f;
		switch ( command )
		{
			case 'M':
			case 'L':
				if ( sscanf( ptr, "%f,%f", &x, &y ) == 2 )
				{
					currentPoint.x = x;
					currentPoint.y = y;
				}
				else
				{
					assert( false );
				}
				break;
			case 'H':
				if ( sscanf( ptr, "%f", &x ) == 1 )
				{
					currentPoint.x = x;
				}
				else
				{
					assert( false );
				}
				break;
			case 'V':
				if ( sscanf( ptr, "%f", &y ) == 1 )
				{
					currentPoint.y = y;
				}
				else
				{
					assert( false );
				}
				break;
			case 'm':
			case 'l':
				if ( sscanf( ptr, "%f,%f", &x, &y ) == 2 )
				{
					currentPoint.x += x;
					currentPoint.y += y;
				}
				else
				{
					assert( false );
				}
				break;
			case 'h':
				if ( sscanf( ptr, "%f", &x ) == 1 )
				{
					currentPoint.x += x;
				}
				else
				{
					assert( false );
				}
				break;
			case 'v':
				if ( sscanf( ptr, "%f", &y ) == 1 )
				{
					currentPoint.y += y;
				}
				else
				{
					assert( false );
				}
				break;

			default:
				assert( false );
				break;
		}

		points[pointCount] = { scale * ( currentPoint.x + offset.x ), -scale * ( currentPoint.y + offset.y ) };
		pointCount += 1;
		if ( pointCount == capacity )
		{
			break;
		}

		// Move to the next space or end of string
		while ( *ptr != '\0' && isspace( *ptr ) == 0 )
		{
			ptr++;
		}

		// Skip contiguous spaces
		while ( isspace( *ptr ) )
		{
			ptr++;
		}

		ptr += 0;
	}

	if ( pointCount == 0 )
	{
		return 0;
	}

	if ( reverseOrder )
	{
		// todo
	}

	// Remove the loop point for chain shapes.
	if ( pointCount > 2 && b2Distance( points[0], points[pointCount - 1] ) <= B2_LINEAR_SLOP)
	{
		pointCount -= 1;
	}

	return pointCount;
}

// Case-insensitive subsequence match. Returns >=0 score on match, -1 on no match.
// Empty needle returns 0 so an empty query lets all samples through with a neutral score.
SampleEntry g_sampleEntries[MAX_SAMPLES] = {};
int g_sampleCount = 0;
int g_replayIndex = -1;

int RegisterSample( const char* category, const char* name, SampleCreateFcn* fcn )
{
	int index = g_sampleCount;
	if ( index < MAX_SAMPLES )
	{
		g_sampleEntries[index] = { category, name, fcn, nullptr };
		++g_sampleCount;
		return index;
	}

	return -1;
}

int RegisterSampleWithCapacity( const char* category, const char* name, SampleCreateFcn* fcn, SampleCapacityFcn* capacityFcn )
{
	int index = g_sampleCount;
	if ( index < MAX_SAMPLES )
	{
		g_sampleEntries[index] = { category, name, fcn, capacityFcn };
		++g_sampleCount;
		return index;
	}

	return -1;
}

int RegisterReplay( const char* category, const char* name, SampleCreateFcn* fcn )
{
	int index = g_sampleCount;
	if ( index < MAX_SAMPLES )
	{
		g_sampleEntries[index] = { category, name, fcn, nullptr };
		g_replayIndex = index;
		++g_sampleCount;
		return index;
	}

	return -1;
}

void SelectSample( SampleContext* context, int selection, bool restart )
{
	if ( restart == false )
	{
		ResetView( &context->camera );
		context->sampleIndex = selection;
		context->pause = false;
		context->subStepCount = 4;
		context->restitutionIterations = 2;
		context->enableRestitutionPropagation = false;
		context->debugDraw.drawJoints = true;
	}

	// Steps queued in a sample that never consumes them must not play out in the next one
	context->singleStep = 0;

	delete context->sample;
	context->sample = nullptr;
	if ( g_sampleEntries[context->sampleIndex].capacityFcn != nullptr )
	{
		context->capacity = g_sampleEntries[context->sampleIndex].capacityFcn();
	}
	else
	{
		context->capacity = b2DefaultWorldDef().capacity;
	}
	context->restart = restart;
	context->sample = g_sampleEntries[context->sampleIndex].createFcn( context );
	context->restart = false;

	// A restart keeps the camera where it was, so the original starting view stays home
	if ( restart == false )
	{
		context->homeCenter = context->camera.center;
		context->homeZoom = context->camera.zoom;
	}
}
