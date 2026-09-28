"""Import and adapt pinned Box2D sample sources for JUCE examples.

Used by vendor.py and by unit tests. Adaptation removes GLFW, ImGui, ImPlot,
NFD, glad/OpenGL, jsmn, and path-based recording I/O while preserving every
RegisterSample, RegisterSampleWithCapacity, and RegisterReplay registration.
"""

from __future__ import annotations

import json
import os
import re
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Literal

SAMPLES_SUBDIR = "samples"
SHARED_SUBDIR = "shared"
SAMPLE_TRANSLATIONS_FILE_NAME = "sample_translations.cpp"

SAMPLE_FILE_NAMES = [
    "container.c",
    "container.h",
    "car.cpp",
    "car.h",
    "donut.cpp",
    "donut.h",
    "doohickey.cpp",
    "doohickey.h",
    "dynamic_mover.cpp",
    "dynamic_mover.h",
    "geometric_mover.cpp",
    "geometric_mover.h",
    "sample.cpp",
    "sample.h",
    "sample_replay.cpp",
    "sample_benchmark.cpp",
    "sample_bodies.cpp",
    "sample_character.cpp",
    "sample_collision.cpp",
    "sample_continuous.cpp",
    "sample_determinism.cpp",
    "sample_events.cpp",
    "sample_geometry.cpp",
    "sample_issues.cpp",
    "sample_joints.cpp",
    "sample_restitution.cpp",
    "sample_robustness.cpp",
    "sample_shapes.cpp",
    "sample_stacking.cpp",
    "sample_world.cpp",
]

SHARED_FILE_NAMES = [
    "benchmarks.c",
    "benchmarks.h",
    "human.c",
    "human.h",
    "utils.c",
    "utils.h",
    "determinism.c",
    "determinism.h",
]

REGISTER_SAMPLE_RE = re.compile(
    r"RegisterSample\s*\(\s*\"([^\"]+)\"\s*,\s*\"([^\"]+)\"\s*,\s*([\w:]+::Create)\s*\)"
)
REGISTER_SAMPLE_CAPACITY_RE = re.compile(
    r"RegisterSampleWithCapacity\s*\(\s*\"([^\"]+)\"\s*,\s*\"([^\"]+)\"\s*,\s*([\w:]+::Create)\s*,\s*([\w:]+::GetCapacity)\s*\)"
)
CPP_STRING_LITERAL_PATTERN = r'"((?:\\.|[^"\\])*)"'
HOST_CONTROL_LABEL_RE = re.compile(
    rf"HostControls::(?:Button|Checkbox|RadioButton|SliderFloat2?|SliderInt|Combo|"
    rf"CollapsingHeader|BeginTabItem|BeginPopupModal|Selectable|Begin|MenuItem|"
    rf"BeginMenu|beginPanel)\s*\(\s*{CPP_STRING_LITERAL_PATTERN}"
)
HOST_CONTROL_TEXT_RE = re.compile(
    rf"HostControls::(?:Text|TextDisabled|TextUnformatted)\s*\(\s*{CPP_STRING_LITERAL_PATTERN}"
)
HOST_CONTROL_TEXT_COLOURED_RE = re.compile(
    rf"HostControls::TextColored\s*\([^;]*?{CPP_STRING_LITERAL_PATTERN}",
    re.DOTALL,
)
STRING_ARRAY_RE = re.compile(
    r"const\s+char\s*\*\s*[A-Za-z_][A-Za-z0-9_]*\s*\[\s*\]\s*=\s*\{(.*?)\};",
    re.DOTALL,
)
STRING_LITERAL_RE = re.compile(CPP_STRING_LITERAL_PATTERN)
REGISTER_REPLAY_RE = re.compile(
    r"RegisterReplay\s*\(\s*\"([^\"]+)\"\s*,\s*\"([^\"]+)\"\s*,\s*([\w:]+::Create)\s*\)"
)

FORBIDDEN_PATTERNS: tuple[tuple[re.Pattern[str], str], ...] = (
    (re.compile(r"ImGui"), "ImGui"),
    (re.compile(r"ImPlot"), "ImPlot"),
    (re.compile(r"GLFW"), "GLFW"),
    (re.compile(r"glfw"), "GLFW"),
    (re.compile(r"\bnfd\w*", re.I), "NFD"),
    (re.compile(r"\bglad\b", re.I), "glad"),
    (re.compile(r"\bOpenGL\b", re.I), "OpenGL"),
    (re.compile(r"\bjsmn\b"), "jsmn"),
    (re.compile(r"\bb2SaveRecordingToFile\b"), "path recording save"),
    (re.compile(r"\bb2LoadRecordingFromFile\b"), "path recording load"),
)

RegistrationKind = Literal["sample", "sampleWithCapacity", "replay"]


@dataclass(frozen=True)
class RegistrationEntry:
    kind: RegistrationKind
    category: str
    name: str
    create_symbol: str
    capacity_symbol: str | None
    source_file: str
    line_number: int


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def write_text(path: Path, text: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(text, encoding="utf-8", newline=os.linesep)


def copy_sample_payload(upstream_root: Path, dest_root: Path) -> None:
    samples_src = upstream_root / "samples"
    shared_src = upstream_root / "shared"

    for name in SAMPLE_FILE_NAMES:
        shutil_copy(samples_src / name, dest_root / SAMPLES_SUBDIR / name)

    for name in SHARED_FILE_NAMES:
        shutil_copy(shared_src / name, dest_root / SHARED_SUBDIR / name)


def shutil_copy(source: Path, dest: Path) -> None:
    if not source.is_file():
        raise FileNotFoundError(source)
    dest.parent.mkdir(parents=True, exist_ok=True)
    dest.write_bytes(source.read_bytes())


HOST_CONTROLS_H = """\
// SPDX-FileCopyrightText: 2026 Raw Material Software Limited
// SPDX-License-Identifier: ISC

#pragma once

#include <stdint.h>

struct HostVec2
{
	float x = 0.0f;
	float y = 0.0f;
};

struct HostVec4
{
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
	float w = 0.0f;
};

struct HostDrawList
{
	void AddRectFilled( HostVec2, HostVec2, uint32_t ) {}
	void AddLine( HostVec2, HostVec2, uint32_t, float = 1.0f ) {}
	void AddCircleFilled( HostVec2, float, uint32_t ) {}
};

#define HOST_COL32( red, green, blue, alpha ) \
	( (uint32_t)( ( (uint32_t)( alpha ) << 24 ) | ( (uint32_t)( blue ) << 16 ) | ( (uint32_t)( green ) << 8 ) | (uint32_t)( red ) ) )

namespace HostControls
{
inline bool beginPanel( const char* ) { return true; }
inline void endPanel() {}
inline bool button( const char* ) { return false; }
inline bool checkbox( const char*, bool* ) { return false; }
inline bool radioButton( const char*, bool ) { return false; }
inline bool sliderFloat( const char*, float*, float, float, const char* = "%.3f" ) { return false; }
inline bool sliderInt( const char*, int*, int, int, const char* = "%d" ) { return false; }
inline void text( const char* ) {}
inline void textDisabled( const char* ) {}
inline void textUnformatted( const char* ) {}
inline void textColored( HostVec4, const char*, ... ) {}
inline void separator() {}
inline void spacing() {}
inline void dummy( HostVec2 ) {}
inline void pushItemWidth( float ) {}
inline void popItemWidth() {}
inline void pushStyleColor( int, HostVec4 ) {}
inline void popStyleColor( int = 1 ) {}
inline void sameLine( float = 0.0f, float = 0.0f ) {}
inline bool collapsingHeader( const char*, int = 0 ) { return false; }
inline bool beginTabBar( const char*, int = 0 ) { return false; }
inline void endTabBar() {}
inline bool beginTabItem( const char*, bool* = nullptr, int = 0 ) { return false; }
inline void endTabItem() {}
inline bool combo( const char*, int*, const char* const*, int ) { return false; }
inline bool inputText( const char*, char*, int ) { return false; }
inline void progressBar( float, HostVec2, const char* ) {}
inline void openPopup( const char* ) {}
inline bool beginPopupModal( const char*, bool*, int ) { return false; }
inline void closeCurrentPopup() {}
inline bool selectable( const char*, bool, int = 0 ) { return false; }
inline bool isKeyPressed( int, bool = true ) { return false; }
inline float getFontSize() { return 16.0f; }
inline float getFrameHeight() { return 20.0f; }
inline void setItemTooltip( const char*, ... ) {}
inline bool begin( const char*, bool*, int ) { return false; }
inline void end() {}
inline bool beginTable( const char*, int, int, float = 0.0f, HostVec2 = {} ) { return false; }
inline void endTable() {}
inline void tableNextRow( int = 0, float = 0.0f ) {}
inline bool tableNextColumn() { return false; }
inline void tableSetupColumn( const char*, int, float = 0.0f ) {}
inline void tableHeadersRow() {}
inline void setCursorPosX( float ) {}
inline HostVec2 getCursorScreenPos() { return {}; }
inline HostVec2 getItemRectMin() { return {}; }
inline HostVec2 getItemRectMax() { return {}; }
inline float getContentRegionAvailX() { return 0.0f; }
inline HostVec2 getContentRegionAvail() { return {}; }
inline float getTextLineHeight() { return 16.0f; }
inline HostDrawList* getWindowDrawList() { static HostDrawList list; return &list; }
inline void setNextWindowPos( HostVec2, int, HostVec2 = {} ) {}
inline void setNextWindowSize( HostVec2, int ) {}
inline bool treeNodeEx( const char*, int, const char*, ... ) { return false; }
inline void treePop() {}
inline bool menuItem( const char*, const char*, bool*, bool = true ) { return false; }
inline bool beginMenu( const char*, bool = true ) { return false; }
inline void endMenu() {}
inline bool beginMainMenuBar() { return false; }
inline void endMainMenuBar() {}
inline bool beginChild( const char*, HostVec2, int, int = 0 ) { return false; }
inline void endChild() {}
inline void setScrollHereY( float = 0.5f ) {}
inline bool isItemHovered( int = 0 ) { return false; }
inline uint32_t getColorU32( int, float = 1.0f ) { return 0; }
inline HostVec4 getStyleColorVec4( int ) { return {}; }

inline bool Button( const char* label ) { return button( label ); }
inline bool Checkbox( const char* label, bool* value ) { return checkbox( label, value ); }
inline bool RadioButton( const char* label, bool selected ) { return radioButton( label, selected ); }
inline bool SliderFloat( const char* label, float* value, float minValue, float maxValue, const char* format = "%.3f" )
{
	return sliderFloat( label, value, minValue, maxValue, format );
}
inline bool SliderInt( const char* label, int* value, int minValue, int maxValue, const char* format = "%d" )
{
	return sliderInt( label, value, minValue, maxValue, format );
}
inline void Text( const char* text ) { textUnformatted( text ); }
inline void TextDisabled( const char* text ) { textDisabled( text ); }
inline void TextUnformatted( const char* text ) { textUnformatted( text ); }
inline void TextColored( HostVec4 colour, const char* format, ... ) { (void)colour; (void)format; }
inline void Separator() { separator(); }
inline void Spacing() { spacing(); }
inline void Dummy( HostVec2 size ) { dummy( size ); }
inline void PushItemWidth( float width ) { pushItemWidth( width ); }
inline void PopItemWidth() { popItemWidth(); }
inline void SameLine( float offset = 0.0f, float spacing = 0.0f ) { sameLine( offset, spacing ); }
inline bool CollapsingHeader( const char* label, int flags = 0 ) { return collapsingHeader( label, flags ); }
inline bool BeginTabBar( const char* id, int flags = 0 ) { return beginTabBar( id, flags ); }
inline void EndTabBar() { endTabBar(); }
inline bool BeginTabItem( const char* label, bool* open = nullptr, int flags = 0 ) { return beginTabItem( label, open, flags ); }
inline void EndTabItem() { endTabItem(); }
inline bool Combo( const char* label, int* index, const char* const* items, int itemCount )
{
	return combo( label, index, items, itemCount );
}
inline bool InputText( const char* label, char* buffer, int bufferLength ) { return inputText( label, buffer, bufferLength ); }
inline void ProgressBar( float fraction, HostVec2 size, const char* overlay ) { progressBar( fraction, size, overlay ); }
inline void OpenPopup( const char* id ) { openPopup( id ); }
inline bool BeginPopupModal( const char* name, bool* open = nullptr, int flags = 0 ) { return beginPopupModal( name, open, flags ); }
inline void CloseCurrentPopup() { closeCurrentPopup(); }
inline bool Selectable( const char* label, bool selected, int flags = 0 ) { return selectable( label, selected, flags ); }
inline bool IsKeyPressed( int key, bool repeat = true ) { return isKeyPressed( key, repeat ); }
inline float GetFontSize() { return getFontSize(); }
inline float GetFrameHeight() { return getFrameHeight(); }
inline void SetItemTooltip( const char* format, ... ) { (void)format; }
inline bool Begin( const char* name, bool* open = nullptr, int flags = 0 ) { return begin( name, open, flags ); }
inline void End() { end(); }
inline bool BeginTable( const char* id, int columns, int flags = 0, HostVec2 outerSize = {}, float innerWidth = 0.0f )
{
	return beginTable( id, columns, flags, innerWidth, outerSize );
}
inline void EndTable() { endTable(); }
inline void TableNextRow( int rowFlags = 0, float minRowHeight = 0.0f ) { tableNextRow( rowFlags, minRowHeight ); }
inline bool TableNextColumn() { return tableNextColumn(); }
inline void TableSetupColumn( const char* label, int flags = 0, float initWidthOrWeight = 0.0f )
{
	tableSetupColumn( label, flags, initWidthOrWeight );
}
inline void TableHeadersRow() { tableHeadersRow(); }
inline void SetCursorPosX( float x ) { setCursorPosX( x ); }
inline HostVec2 GetCursorScreenPos() { return getCursorScreenPos(); }
inline HostVec2 GetItemRectMin() { return getItemRectMin(); }
inline HostVec2 GetItemRectMax() { return getItemRectMax(); }
inline float GetContentRegionAvailX() { return getContentRegionAvailX(); }
inline HostVec2 GetContentRegionAvail() { return getContentRegionAvail(); }
inline float GetTextLineHeight() { return getTextLineHeight(); }
inline HostDrawList* GetWindowDrawList() { return getWindowDrawList(); }
inline void SetNextWindowPos( HostVec2 pos, int condition, HostVec2 pivot = {} ) { setNextWindowPos( pos, condition, pivot ); }
inline void SetNextWindowSize( HostVec2 size, int condition ) { setNextWindowSize( size, condition ); }
inline bool TreeNodeEx( const char* id, int flags, const char* label, ... ) { (void)id; (void)flags; (void)label; return false; }
inline void TreePop() { treePop(); }
inline bool MenuItem( const char* label, const char* shortcut, bool* selected, bool enabled = true )
{
	return menuItem( label, shortcut, selected, enabled );
}
inline bool BeginMenu( const char* label, bool enabled = true ) { return beginMenu( label, enabled ); }
inline void EndMenu() { endMenu(); }
inline bool BeginMainMenuBar() { return beginMainMenuBar(); }
inline void EndMainMenuBar() { endMainMenuBar(); }
inline bool BeginChild( const char* id, HostVec2 size, int childFlags, int windowFlags = 0 )
{
	return beginChild( id, size, childFlags, windowFlags );
}
inline void EndChild() { endChild(); }
inline void SetScrollHereY( float centerYRatio = 0.5f ) { setScrollHereY( centerYRatio ); }
inline bool IsItemHovered( int flags = 0 ) { return isItemHovered( flags ); }
inline uint32_t GetColorU32( int index, float alphaMul = 1.0f ) { return getColorU32( index, alphaMul ); }
inline HostVec4 GetStyleColorVec4( int index ) { return getStyleColorVec4( index ); }
} // namespace HostControls

#define HOST_MOUSE_BUTTON_PRIMARY 1
"""

HOST_INPUT_H = """\
// SPDX-FileCopyrightText: 2026 Raw Material Software Limited
// SPDX-License-Identifier: ISC

#pragma once

struct HostWindow;
"""

DRAW_H = """\
// SPDX-FileCopyrightText: 2023 Erin Catto
// SPDX-License-Identifier: MIT

#pragma once

#include "box2d/types.h"

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
"""

DRAW_STUB_C = """\
// SPDX-FileCopyrightText: 2026 Raw Material Software Limited
// SPDX-License-Identifier: ISC

#include "draw.h"

#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

struct Draw
{
	b2Pos origin;
};

Camera GetDefaultCamera( void )
{
	Camera camera = {};
	camera.center = (b2Pos){ 0.0f, 20.0f };
	camera.zoom = 1.0f;
	camera.width = 1280.0f;
	camera.height = 720.0f;
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
	(void)camera;
	(void)screenPoint;
	return (b2Pos){ 0.0f, 0.0f };
}

b2Vec2 ConvertWorldToScreen( Camera* camera, b2Pos worldPoint )
{
	(void)camera;
	(void)worldPoint;
	return (b2Vec2){ 0.0f, 0.0f };
}

b2Vec2 ConvertViewToScreen( Camera* camera, b2Vec2 viewPoint )
{
	(void)camera;
	(void)viewPoint;
	return (b2Vec2){ 0.0f, 0.0f };
}

b2AABB GetViewBounds( Camera* camera )
{
	(void)camera;
	return (b2AABB){ (b2Vec2){ -50.0f, -50.0f }, (b2Vec2){ 50.0f, 50.0f } };
}

void FocusOnBounds( Camera* camera, b2AABB bounds )
{
	if ( camera == NULL )
	{
		return;
	}

	b2Vec2 extent = b2Sub( bounds.upperBound, bounds.lowerBound );
	camera->center = (b2Pos){ 0.5f * ( bounds.lowerBound.x + bounds.upperBound.x ),
							  0.5f * ( bounds.lowerBound.y + bounds.upperBound.y ) };
	float maxExtent = b2MaxFloat( extent.x, extent.y );
	camera->zoom = maxExtent > 0.0f ? 0.45f * camera->height / maxExtent : camera->zoom;
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
	(void)x;
	(void)y;
	(void)color;
	char buffer[256];
	va_list args;
	va_start( args, string );
	formatString( buffer, sizeof( buffer ), string, args );
	va_end( args );
}

void DrawPoint( Draw* draw, b2Pos p, float size, b2HexColor color )
{
	(void)draw;
	(void)p;
	(void)size;
	(void)color;
}

void DrawLine( Draw* draw, b2Pos p1, b2Pos p2, b2HexColor color )
{
	(void)draw;
	(void)p1;
	(void)p2;
	(void)color;
}

void DrawCircle( Draw* draw, b2Pos center, float radius, b2HexColor color )
{
	(void)draw;
	(void)center;
	(void)radius;
	(void)color;
}

void DrawCapsule( Draw* draw, b2Pos p1, b2Pos p2, float radius, b2HexColor color )
{
	(void)draw;
	(void)p1;
	(void)p2;
	(void)radius;
	(void)color;
}

void DrawPolygon( Draw* draw, b2WorldTransform transform, const b2Vec2* vertices, int vertexCount, b2HexColor color )
{
	(void)draw;
	(void)transform;
	(void)vertices;
	(void)vertexCount;
	(void)color;
}

void DrawSolidCircle( Draw* draw, b2WorldTransform transform, b2Vec2 center, float radius, b2HexColor color )
{
	(void)draw;
	(void)transform;
	(void)center;
	(void)radius;
	(void)color;
}

void DrawSolidPolygon( Draw* draw, b2WorldTransform transform, const b2Vec2* vertices, int vertexCount, float radius,
					   b2HexColor color )
{
	(void)draw;
	(void)transform;
	(void)vertices;
	(void)vertexCount;
	(void)radius;
	(void)color;
}

void DrawTransform( Draw* draw, b2WorldTransform transform, float scale )
{
	(void)draw;
	(void)transform;
	(void)scale;
}

void DrawBounds( Draw* draw, b2AABB aabb, b2HexColor color )
{
	(void)draw;
	(void)aabb;
	(void)color;
}

void DrawString( Draw* draw, Camera* camera, b2Pos p, b2HexColor color, const char* string, ... )
{
	(void)draw;
	(void)camera;
	(void)p;
	(void)color;
	char buffer[256];
	va_list args;
	va_start( args, string );
	formatString( buffer, sizeof( buffer ), string, args );
	va_end( args );
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
"""

DRAW_STUB_INCLUDES = '#include <stdlib.h>\n'


def write_host_support_files(samples_dir: Path) -> None:
    write_text(samples_dir / "host_controls.h", HOST_CONTROLS_H)
    write_text(samples_dir / "host_input.h", HOST_INPUT_H)
    write_text(samples_dir / "draw.h", DRAW_H)
    stub = DRAW_STUB_C.replace("#include \"draw.h\"\n", "#include \"draw.h\"\n" + DRAW_STUB_INCLUDES)
    write_text(samples_dir / "draw_stub.c", stub)


def strip_forbidden_includes(text: str) -> str:
    lines = text.splitlines()
    filtered: list[str] = []
    for line in lines:
        if re.search(r"#include\s*[<\"]imgui", line, re.I):
            continue
        if re.search(r"#include\s*[<\"]implot", line, re.I):
            continue
        if re.search(r"#include\s*[<\"]GLFW/", line):
            continue
        if re.search(r"#include\s*\"jsmn.h\"", line):
            continue
        if re.search(r"#include\s*[<\"]nfd.h", line, re.I):
            continue
        filtered.append(line)

    joined = "\n".join(filtered)
    if filtered:
        joined += "\n"
    return joined


def ensure_host_includes(text: str) -> str:
    if "host_controls.h" not in text and "ImGui::" in text:
        text = text.replace("#include \"sample.h\"", "#include \"sample.h\"\n#include \"host_controls.h\"")
    if "#include \"host_controls.h\"" not in text and "HostControls::" not in text and "ImGui::" in text:
        lines = text.splitlines()
        insert_at = 0
        for index, line in enumerate(lines):
            if line.startswith("#include"):
                insert_at = index + 1
        lines.insert(insert_at, "#include \"host_controls.h\"")
        text = "\n".join(lines) + "\n"
    return text


def scrub_glfw_references(text: str) -> str:
    text = re.sub(r"glfwGetKey\s*\([^)]*\)\s*==\s*GLFW_PRESS", "false", text, flags=re.I)
    text = re.sub(r"glfwGetKey\s*\([^)]*\)", "false", text, flags=re.I)
    text = re.sub(r"glfwSetWindowShouldClose\s*\([^)]*\)\s*;", "", text, flags=re.I)
    text = re.sub(r"GLFW_KEY_[A-Z0-9]+", "0", text)
    text = re.sub(r"GLFW_PRESS", "0", text)
    text = re.sub(r"GLFW_MOUSE_BUTTON_[0-9]+", "HOST_MOUSE_BUTTON_PRIMARY", text)
    return text


def scrub_host_ui_tokens(text: str) -> str:
    text = re.sub(r"\bImVec2\b", "HostVec2", text)
    text = re.sub(r"\bImVec4\b", "HostVec4", text)
    text = re.sub(r"\bImU32\b", "uint32_t", text)
    text = re.sub(r"\bImDrawList\b", "HostDrawList", text)
    text = re.sub(r"\bImColor\b", "HostVec4", text)
    text = re.sub(r"ImGuiWindowFlags_[A-Za-z0-9_]+", "0", text)
    text = re.sub(r"ImGuiCond_[A-Za-z0-9_]+", "0", text)
    text = re.sub(r"ImGuiCol_[A-Za-z0-9_]+", "0", text)
    text = re.sub(r"ImGuiTreeNodeFlags_[A-Za-z0-9_]+", "0", text)
    text = re.sub(r"ImGuiTabItemFlags_[A-Za-z0-9_]+", "0", text)
    text = re.sub(r"ImGuiSliderFlags_[A-Za-z0-9_]+", "0", text)
    text = re.sub(r"ImGuiChildFlags_[A-Za-z0-9_]+", "0", text)
    text = re.sub(r"ImGuiTableColumnFlags_[A-Za-z0-9_]+", "0", text)
    text = re.sub(r"ImGuiTableFlags_[A-Za-z0-9_]+", "0", text)
    text = re.sub(r"ImGuiTabBarFlags_[A-Za-z0-9_]+", "0", text)
    text = re.sub(r"GLFW_MOD_[A-Z_]+", "0", text)
    text = re.sub(r"\bImGuiTreeNodeFlags\b", "int", text)
    text = re.sub(r"\bImGuiTabItemFlags\b", "int", text)
    text = re.sub(r"IM_COL32\s*\(", "HOST_COL32(", text)
    return text


def rewrite_imgui_identifiers(text: str) -> str:
    text = re.sub(r"\bImGui::", "HostControls::", text)
    text = re.sub(r"\bImPlot::", "HostControls::", text)
    text = scrub_host_ui_tokens(text)
    text = scrub_glfw_references(text)
    return text


def adapt_sample_h(text: str) -> str:
    text = strip_forbidden_includes(text)
    text = text.replace('#include "imgui.h"\n', "")
    text = text.replace("struct ImFont;\n\n", "")
    text = text.replace("struct GLFWwindow* window = nullptr;\n", "HostWindow* window = nullptr;\n")
    text = re.sub(
        r"inline ImVec4 MakeColor\( b2HexColor hexColor \)\s*\{[^}]+\}\n",
        "",
        text,
        flags=re.S,
    )
    if "#include \"host_input.h\"" not in text:
        text = text.replace('#include "draw.h"\n', '#include "draw.h"\n#include "host_input.h"\n')
    return text


SAMPLE_CPP_STUB_BLOCK = """
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
"""


def adapt_sample_cpp(text: str) -> str:
    text = strip_forbidden_includes(text)
    text = text.replace('#include "implot.h"\n', "")
    text = text.replace('#include "jsmn.h"\n', "")

    text = re.sub(
        r"static bool ReadFile\( char\*& data, int& size, const char\* filename \)\s*\{[\s\S]*?\n\}\n",
        "",
        text,
        count=1,
    )
    text = re.sub(
        r"void SampleContext::Save\(\)\s*\{[\s\S]*?\n\}\n",
        "void SampleContext::Save()\n{\n}\n\n",
        text,
        count=1,
    )
    text = re.sub(
        r"static int jsoneq\([\s\S]*?\n\}\n",
        "",
        text,
        count=1,
    )
    text = re.sub(
        r"void SampleContext::Load\(\)\s*\{[\s\S]*?\n\}\n",
        "void SampleContext::Load()\n{\n\tcamera = GetDefaultCamera();\n\tdebugDraw = b2DefaultDebugDraw();\n\tdebugDraw.context = this;\n\trecycleDistance = B2_CONTACT_RECYCLE_DISTANCE;\n}\n\n",
        text,
        count=1,
    )

    text = text.replace(
        "if ( b2SaveRecordingToFile( m_recording, m_context->recordingFile ) )",
        "if ( b2Recording_GetSize( m_recording ) > 0 )",
    )

    text = text.replace("ImGui::GetFontSize()", "HostControls::getFontSize()")
    text = text.replace("ImGui::GetFrameHeight()", "HostControls::getFrameHeight()")
    text = rewrite_imgui_identifiers(text)

    start = text.find("struct RowDef")
    parse_path = text.find("int Sample::ParsePath")
    if start >= 0 and parse_path > start:
        text = text[:start] + SAMPLE_CPP_STUB_BLOCK + "\n" + text[parse_path:]

    fuzzy = text.find("static int FuzzyScore")
    registry = text.find("SampleEntry g_sampleEntries")
    if fuzzy >= 0 and registry > fuzzy:
        text = text[:fuzzy] + text[registry:]

    menu = text.find("static void DrawRow")
    if menu >= 0:
        text = text[:menu].rstrip() + "\n"

    if "#include \"host_controls.h\"" not in text:
        text = text.replace('#include "sample.h"\n', '#include "sample.h"\n#include "host_controls.h"\n')

    return text


def adapt_generic_sample_source(text: str) -> str:
    text = strip_forbidden_includes(text)
    text = rewrite_imgui_identifiers(text)
    text = text.replace("HostControls::GetFontSize()", "HostControls::getFontSize()")
    text = text.replace("HostControls::GetFrameHeight()", "HostControls::getFrameHeight()")
    if "HostControls::" in text and "#include \"host_controls.h\"" not in text:
        text = text.replace(
            '#include "sample.h"\n',
            '#include "sample.h"\n#include "host_controls.h"\n',
        )
    return text


def adapt_sample_replay_cpp(text: str) -> str:
    text = adapt_generic_sample_source(text)
    text = text.replace(
        "b2Recording* recording = b2LoadRecordingFromFile( m_path );",
        "b2Recording* recording = nullptr; // recording bytes are supplied by the JUCE host",
    )
    return text


def adapt_file(path: Path, text: str) -> str:
    name = path.name
    if name == "sample.h":
        return adapt_sample_h(text)
    if name == "sample.cpp":
        return adapt_sample_cpp(text)
    if name == "sample_replay.cpp":
        return adapt_sample_replay_cpp(text)
    if path.suffix in {".cpp", ".c", ".h"} and path.parent.name == SAMPLES_SUBDIR:
        return adapt_generic_sample_source(text)
    return text


def adapt_sample_tree(dest_root: Path) -> None:
    for path in dest_root.rglob("*"):
        if not path.is_file():
            continue
        if path.suffix not in {".cpp", ".c", ".h"}:
            continue
        write_text(path, adapt_file(path, read_text(path)))


def parse_registrations_from_text(text: str, source_file: str) -> list[RegistrationEntry]:
    entries: list[RegistrationEntry] = []
    for line_number, line in enumerate(text.splitlines(), start=1):
        for match in REGISTER_SAMPLE_RE.finditer(line):
            entries.append(
                RegistrationEntry(
                    kind="sample",
                    category=match.group(1),
                    name=match.group(2),
                    create_symbol=match.group(3),
                    capacity_symbol=None,
                    source_file=source_file,
                    line_number=line_number,
                )
            )
        for match in REGISTER_SAMPLE_CAPACITY_RE.finditer(line):
            entries.append(
                RegistrationEntry(
                    kind="sampleWithCapacity",
                    category=match.group(1),
                    name=match.group(2),
                    create_symbol=match.group(3),
                    capacity_symbol=match.group(4),
                    source_file=source_file,
                    line_number=line_number,
                )
            )
        for match in REGISTER_REPLAY_RE.finditer(line):
            entries.append(
                RegistrationEntry(
                    kind="replay",
                    category=match.group(1),
                    name=match.group(2),
                    create_symbol=match.group(3),
                    capacity_symbol=None,
                    source_file=source_file,
                    line_number=line_number,
                )
            )
    return entries


def collect_registrations(dest_root: Path) -> list[RegistrationEntry]:
    entries: list[RegistrationEntry] = []
    samples_dir = dest_root / SAMPLES_SUBDIR
    for path in sorted(samples_dir.rglob("*.cpp")):
        rel = path.relative_to(dest_root).as_posix()
        entries.extend(parse_registrations_from_text(read_text(path), rel))
    return sorted(
        entries,
        key=lambda item: (item.category, item.name, item.kind, item.source_file),
    )


def collect_upstream_registrations(upstream_root: Path) -> list[RegistrationEntry]:
    entries: list[RegistrationEntry] = []
    for name in SAMPLE_FILE_NAMES:
        if not name.endswith(".cpp"):
            continue
        path = upstream_root / SAMPLES_SUBDIR / name
        entries.extend(parse_registrations_from_text(read_text(path), f"samples/{name}"))
    return sorted(
        entries,
        key=lambda item: (item.category, item.name, item.kind, item.source_file),
    )


def registration_identity(entry: RegistrationEntry) -> tuple[str, str, str, str, str | None, str]:
    return (
        entry.kind,
        entry.category,
        entry.name,
        entry.create_symbol,
        entry.capacity_symbol,
        entry.source_file,
    )


def validate_registration_parity(
    upstream_entries: list[RegistrationEntry],
    adapted_entries: list[RegistrationEntry],
) -> None:
    expected = [registration_identity(entry) for entry in upstream_entries]
    actual = [registration_identity(entry) for entry in adapted_entries]
    if actual != expected:
        missing = sorted(set(expected) - set(actual))
        extra = sorted(set(actual) - set(expected))
        raise RuntimeError(
            "adapted sample registry does not match the pinned upstream registry"
            f"\nmissing: {missing}\nextra: {extra}"
        )


def manifest_dict(entries: list[RegistrationEntry], provenance_pointer: str) -> dict:
    return {
        "provenanceAuthority": provenance_pointer,
        "registrations": [
            {
                "kind": entry.kind,
                "category": entry.category,
                "name": entry.name,
                "createSymbol": entry.create_symbol,
                "capacitySymbol": entry.capacity_symbol,
                "sourceFile": entry.source_file,
                "line": entry.line_number,
            }
            for entry in entries
        ],
    }


def write_registry_manifest(dest_root: Path, provenance_pointer: str) -> list[RegistrationEntry]:
    entries = collect_registrations(dest_root)
    manifest_path = dest_root / "sample_registry.json"
    write_text(
        manifest_path,
        json.dumps(manifest_dict(entries, provenance_pointer), indent=2) + "\n",
    )
    return entries


def decode_cpp_string_literal(value: str) -> str:
    return json.loads(f'"{value}"')


def collect_sample_translation_keys(
    sample_root: Path, entries: list[RegistrationEntry]
) -> set[str]:
    translation_keys = {entry.category for entry in entries} | {
        entry.name for entry in entries
    }

    for source_path in sorted((sample_root / SAMPLES_SUBDIR).glob("*.cpp")):
        source = read_text(source_path)

        for match in HOST_CONTROL_LABEL_RE.finditer(source):
            label = decode_cpp_string_literal(match.group(1)).split("##", 1)[0]

            if label:
                translation_keys.add(label)

        for pattern in (HOST_CONTROL_TEXT_RE, HOST_CONTROL_TEXT_COLOURED_RE):
            translation_keys.update(
                decode_cpp_string_literal(match.group(1))
                for match in pattern.finditer(source)
                if match.group(1)
            )

        for array_match in STRING_ARRAY_RE.finditer(source):
            translation_keys.update(
                decode_cpp_string_literal(match.group(1))
                for match in STRING_LITERAL_RE.finditer(array_match.group(1))
                if match.group(1)
            )

    return translation_keys


def build_sample_translations_source(
    sample_root: Path, entries: list[RegistrationEntry]
) -> str:
    translation_keys = sorted(collect_sample_translation_keys(sample_root, entries))
    translation_rows = ",\n".join(
        f"    NEEDS_TRANS ({json.dumps(key, ensure_ascii=True)})"
        for key in translation_keys
    )
    return f"""\
// SPDX-FileCopyrightText: 2026 Raw Material Software Limited
// SPDX-License-Identifier: ISC

#include <juce_core/juce_core.h>

namespace
{{

[[maybe_unused]] constexpr const char* sampleTranslationKeys[] =
{{
{translation_rows}
}};

}} // namespace
"""


def write_sample_translations(
    dest_root: Path, entries: list[RegistrationEntry]
) -> None:
    write_text(
        dest_root / SAMPLE_TRANSLATIONS_FILE_NAME,
        build_sample_translations_source(dest_root, entries),
    )


PROVENANCE_NOTE = """\
Sample sources under this directory are imported from the Box2D commit recorded in
modules/juce_box2d/box2d/JUCE_UPSTREAM.txt. Regenerate with:

  python modules/juce_box2d/box2d/vendor.py

Registry entries are listed in sample_registry.json beside this file.
"""


def write_provenance_note(dest_root: Path) -> None:
    write_text(dest_root / "JUCE_SAMPLE_PROVENANCE.txt", PROVENANCE_NOTE)


def scan_forbidden_references(dest_root: Path) -> list[str]:
    findings: list[str] = []
    for path in sorted(dest_root.rglob("*")):
        if not path.is_file():
            continue
        if path.suffix not in {".cpp", ".c", ".h"}:
            continue
        rel = path.relative_to(dest_root).as_posix()
        for line_number, line in enumerate(read_text(path).splitlines(), start=1):
            stripped = line.strip()
            if stripped.startswith("//"):
                continue
            for pattern, label in FORBIDDEN_PATTERNS:
                if pattern.search(line):
                    findings.append(f"{rel}:{line_number}: forbidden {label}: {stripped}")
    return findings


def normalise_source_line_endings(dest_root: Path) -> None:
    for path in sorted(dest_root.rglob("*")):
        if path.is_file() and path.suffix in {".cpp", ".c", ".h"}:
            write_text(path, read_text(path))


def validate_spdx_headers(dest_root: Path) -> list[str]:
    findings: list[str] = []
    for path in sorted(dest_root.rglob("*")):
        if not path.is_file() or path.suffix not in {".cpp", ".c", ".h"}:
            continue
        opening = "\n".join(read_text(path).splitlines()[:8])
        if "SPDX-FileCopyrightText:" not in opening or "SPDX-License-Identifier:" not in opening:
            findings.append(path.relative_to(dest_root).as_posix())
    return findings


def compare_directory_trees(
    expected_root: Path,
    actual_root: Path,
    ignored_names: set[str] | None = None,
) -> list[str]:
    mismatches: list[str] = []
    ignored = ignored_names or set()
    expected_files = {
        path.relative_to(expected_root)
        for path in expected_root.rglob("*")
        if path.is_file() and not any(part in ignored for part in path.relative_to(expected_root).parts)
    }
    actual_files = {
        path.relative_to(actual_root)
        for path in actual_root.rglob("*")
        if path.is_file() and not any(part in ignored for part in path.relative_to(actual_root).parts)
    }

    for missing in sorted(expected_files - actual_files):
        mismatches.append(f"missing file: {missing.as_posix()}")
    for extra in sorted(actual_files - expected_files):
        mismatches.append(f"unexpected file: {extra.as_posix()}")

    for rel in sorted(expected_files & actual_files):
        expected_bytes = (expected_root / rel).read_bytes()
        actual_bytes = (actual_root / rel).read_bytes()
        if expected_bytes != actual_bytes:
            mismatches.append(f"content differs: {rel.as_posix()}")

    return mismatches


def apply_unified_patch(dest_root: Path, patch_path: Path) -> None:
    if not patch_path.is_file():
        raise FileNotFoundError(patch_path)

    environment = os.environ.copy()
    ceiling_directories = [str(dest_root.parent.resolve())]
    if existing_ceiling_directories := environment.get("GIT_CEILING_DIRECTORIES"):
        ceiling_directories.append(existing_ceiling_directories)
    environment["GIT_CEILING_DIRECTORIES"] = os.pathsep.join(ceiling_directories)

    result = subprocess.run(
        [
            "git",
            "apply",
            "--no-index",
            "--unsafe-paths",
            "--whitespace=nowarn",
            str(patch_path),
        ],
        cwd=dest_root,
        env=environment,
        capture_output=True,
        text=True,
    )
    if result.returncode != 0:
        raise RuntimeError(result.stderr or result.stdout)


def import_samples(
    upstream_root: Path,
    dest_root: Path,
    patch_path: Path,
    provenance_pointer: str,
) -> list[RegistrationEntry]:
    import shutil

    upstream_entries = collect_upstream_registrations(upstream_root)

    if dest_root.exists():
        for child in dest_root.iterdir():
            if child.is_dir():
                shutil.rmtree(child)
            else:
                child.unlink()
    else:
        dest_root.mkdir(parents=True)

    copy_sample_payload(upstream_root, dest_root)
    apply_unified_patch(dest_root, patch_path)
    normalise_source_line_endings(dest_root)
    write_sample_translations(dest_root, collect_registrations(dest_root))

    forbidden = scan_forbidden_references(dest_root)
    if forbidden:
        raise RuntimeError(
            "adapted sample tree still contains forbidden host references:\n"
            + "\n".join(forbidden[:40])
        )

    missing_spdx = validate_spdx_headers(dest_root)
    if missing_spdx:
        raise RuntimeError(
            "adapted sample files are missing SPDX headers:\n"
            + "\n".join(missing_spdx)
        )

    write_provenance_note(dest_root)
    entries = write_registry_manifest(dest_root, provenance_pointer)
    validate_registration_parity(upstream_entries, entries)
    return entries


def fail(message: str) -> None:
    print(f"box2d_sample_import.py: {message}", file=sys.stderr)
    sys.exit(1)
