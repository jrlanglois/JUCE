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

#if defined( __linux__ ) && ! defined( _GNU_SOURCE )
#define _GNU_SOURCE
#endif

#if defined( _MSC_VER ) && ! defined( _CRT_SECURE_NO_WARNINGS )
#define _CRT_SECURE_NO_WARNINGS
#endif

#if defined( _MSC_VER ) && ! defined( __clang__ ) && ! defined( _CRTDBG_MAP_ALLOC )
#define _CRTDBG_MAP_ALLOC
#endif

#if defined( _WIN32 )

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif

#ifndef NOGDI
#define NOGDI
#endif

#ifndef NOUSER
#define NOUSER
#endif

#ifndef NOMINMAX
#define NOMINMAX
#endif

#endif

#include <juce_core/system/juce_CompilerWarnings.h>


/* Hoisted system headers */
#include <assert.h>
#include <float.h>
#include <limits.h>
#include <math.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined( __APPLE__ )
#include <TargetConditionals.h>
#endif

#if defined( BOX2D_PROFILE )
#include <tracy/TracyC.h>
#endif

#if defined( _MSC_VER ) && ! defined( __clang__ )
#include <crtdbg.h>
#include <vcruntime_string.h>
#endif

#if defined( _MSC_VER ) && ( defined( _M_ARM ) || defined( _M_ARM64 ) || defined( _M_ARM64EC ) )
#include <intrin.h>
#elif defined( _MSC_VER )
#include <intrin0.h>
#endif

#if defined( _M_X64 ) || defined( __x86_64__ ) || defined( _M_IX86 ) || defined( __i386__ )
#include <xmmintrin.h>
#endif

#if ! defined( BOX2D_DISABLE_SIMD ) && defined( BOX2D_AVX2 ) && ( defined( _M_X64 ) || defined( __x86_64__ ) || defined( _M_IX86 ) || defined( __i386__ ) )
#include <immintrin.h>
#endif

#if ! defined( BOX2D_DISABLE_SIMD ) && ( defined( __aarch64__ ) || defined( _M_ARM64 ) )
#include <arm_neon.h>
#endif

#if ! defined( BOX2D_DISABLE_SIMD ) && ( defined( _M_X64 ) || defined( __x86_64__ ) || defined( _M_IX86 ) || defined( __i386__ ) || defined( __EMSCRIPTEN__ ) )
#include <emmintrin.h>
#endif

#if defined( _WIN32 )
#include <windows.h>
#endif

#if defined( __linux__ ) || defined( __EMSCRIPTEN__ )
#include <pthread.h>
#include <sched.h>
#include <semaphore.h>
#include <sys/time.h>
#include <time.h>
#endif

#if defined( __APPLE__ )
#include <dispatch/dispatch.h>
#include <mach/mach_time.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/time.h>
#include <time.h>
#endif


/* MSVC default C-mode compatibility */
#if defined( _MSC_VER ) && ! defined( __clang__ ) && ! defined( __STDC_VERSION__ )
#ifndef _Alignas
#define _Alignas( alignment ) __declspec (align (alignment))
#endif
#ifndef _Static_assert
#define JUCE_BOX2D_CONCATENATE_HELPER( first, second ) first##second
#define JUCE_BOX2D_CONCATENATE( first, second ) JUCE_BOX2D_CONCATENATE_HELPER (first, second)
#define _Static_assert( condition, message )     typedef char JUCE_BOX2D_CONCATENATE (juce_box2d_static_assertion_, __COUNTER__)[( condition ) ? 1 : -1]
#endif
#endif

#if defined( __clang__ )
#pragma clang fp contract (off)
#elif defined( _MSC_VER )
#pragma fp_contract (off)
#elif defined( __GNUC__ )
#pragma GCC optimize ("fp-contract=off")
#endif

JUCE_BEGIN_IGNORE_WARNINGS_GCC_LIKE ("-Wsign-conversion",
                                     "-Wswitch-enum",
                                     "-Wlanguage-extension-token",
                                     "-Wimplicit-int-conversion",
                                     "-Wimplicit-int-float-conversion",
                                     "-Wfloat-equal",
                                     "-Wcast-align")
JUCE_BEGIN_IGNORE_WARNINGS_MSVC (4201)

#include "juce_box2d/juce_box2d_upstream.h"
#include "juce_box2d/box2d/src/recording_ops.inl"
#include "juce_box2d/box2d/src/aabb.h"
#include "juce_box2d/box2d/src/core.h"
#include "juce_box2d/box2d/src/container.h"
#include "juce_box2d/box2d/src/arena_allocator.h"
#include "juce_box2d/box2d/src/bitset.h"
#include "juce_box2d/box2d/src/dynamic_tree.h"
#include "juce_box2d/box2d/src/table.h"
#include "juce_box2d/box2d/src/broad_phase.h"
#include "juce_box2d/box2d/src/contact.h"
#include "juce_box2d/box2d/src/solver.h"
#include "juce_box2d/box2d/src/joint.h"
#include "juce_box2d/box2d/src/constraint_graph.h"
#include "juce_box2d/box2d/src/id_pool.h"
#include "juce_box2d/box2d/src/sensor.h"
#include "juce_box2d/box2d/src/shape.h"
#include "juce_box2d/box2d/src/island.h"
#include "juce_box2d/box2d/src/solver_set.h"
#include "juce_box2d/box2d/src/physics_world.h"
#include "juce_box2d/box2d/src/body.h"
#include "juce_box2d/box2d/src/contact_solver.h"
#include "juce_box2d/box2d/src/parallel_for.h"
#include "juce_box2d/box2d/src/platform.h"
#include "juce_box2d/box2d/src/recording.h"
#include "juce_box2d/box2d/src/replay.h"
#include "juce_box2d/box2d/src/scheduler.h"
#include "juce_box2d/box2d/src/simd.h"
#include "juce_box2d/box2d/src/snapshot.h"
#include "juce_box2d/box2d/src/aabb.c"
#include "juce_box2d/box2d/src/arena_allocator.c"
#include "juce_box2d/box2d/src/bitset.c"
#include "juce_box2d/box2d/src/body.c"
#include "juce_box2d/box2d/src/qsort.h"
#if defined( _M_ARM64EC )
JUCE_BEGIN_IGNORE_WARNINGS_MSVC (4189)
#endif
#include "juce_box2d/box2d/src/broad_phase.c"
#if defined( _M_ARM64EC )
JUCE_END_IGNORE_WARNINGS_MSVC
#endif
#include "juce_box2d/box2d/src/constraint_graph.c"
#include "juce_box2d/box2d/src/contact_solver.c"
#include "juce_box2d/box2d/src/contact.c"
#include "juce_box2d/box2d/src/core.c"
#include "juce_box2d/box2d/src/distance_joint.c"
#include "juce_box2d/box2d/src/distance.c"
#include "juce_box2d/box2d/src/dynamic_tree.c"
#include "juce_box2d/box2d/src/geometry.c"
#include "juce_box2d/box2d/src/hull.c"
#include "juce_box2d/box2d/src/id_pool.c"
#include "juce_box2d/box2d/src/island.c"
#include "juce_box2d/box2d/src/joint.c"
#include "juce_box2d/box2d/src/manifold.c"
#include "juce_box2d/box2d/src/math_functions.c"
#include "juce_box2d/box2d/src/motor_joint.c"
#include "juce_box2d/box2d/src/mover_joint.c"
#include "juce_box2d/box2d/src/mover.c"
#include "juce_box2d/box2d/src/parallel_for.c"
#include "juce_box2d/box2d/src/physics_world.c"
#include "juce_box2d/box2d/src/pogo_joint.c"
#include "juce_box2d/box2d/src/prismatic_joint.c"
#include "juce_box2d/box2d/src/recording.c"
#include "juce_box2d/box2d/src/replay.c"
#include "juce_box2d/box2d/src/revolute_joint.c"
#include "juce_box2d/box2d/src/scheduler.c"
#include "juce_box2d/box2d/src/sensor.c"
#include "juce_box2d/box2d/src/shape.c"
#include "juce_box2d/box2d/src/snapshot.c"
#include "juce_box2d/box2d/src/solver_set.c"
#include "juce_box2d/box2d/src/solver.c"
#include "juce_box2d/box2d/src/table.c"
#include "juce_box2d/box2d/src/timer.c"
#include "juce_box2d/box2d/src/types.c"
#include "juce_box2d/box2d/src/weld_joint.c"
#include "juce_box2d/box2d/src/wheel_joint.c"

#if JUCE_UNIT_TESTS
JUCE_BEGIN_IGNORE_WARNINGS_GCC_LIKE ("-Wsign-conversion",
                                     "-Wfloat-equal")
JUCE_BEGIN_IGNORE_WARNINGS_MSVC (4201)
#include "juce_box2d/box2d/shared/determinism.h"
#include "juce_box2d/box2d/shared/determinism.c"
JUCE_END_IGNORE_WARNINGS_MSVC
JUCE_END_IGNORE_WARNINGS_GCC_LIKE
#endif

JUCE_END_IGNORE_WARNINGS_MSVC
JUCE_END_IGNORE_WARNINGS_GCC_LIKE
