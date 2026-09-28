// SPDX-FileCopyrightText: 2026 Raw Material Software Limited
// SPDX-License-Identifier: ISC

#pragma once

#ifdef __cplusplus

#include <cassert>
#include <juce_box2d/juce_box2d.h>

#else

#include <float.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>

#if defined( _MSC_VER )
#pragma warning( push )
#pragma warning( disable : 4201 ) // Box2D uses a C11 anonymous union.
#endif

#include <juce_box2d/juce_box2d_upstream.h>

#if defined( _MSC_VER )
#pragma warning( pop )
#endif

#endif
