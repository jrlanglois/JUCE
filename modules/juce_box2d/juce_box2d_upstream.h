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

#ifdef BOX2D_USER_CONFIG
#include BOX2D_USER_CONFIG
#endif

#include "juce_box2d/box2d/include/box2d/config.h"
#include "juce_box2d/box2d/include/box2d/base.h"
#include "juce_box2d/box2d/include/box2d/id.h"
#include "juce_box2d/box2d/include/box2d/math_types.h"
#include "juce_box2d/box2d/include/box2d/math_functions.h"
#include "juce_box2d/box2d/include/box2d/collision.h"
#include "juce_box2d/box2d/include/box2d/constants.h"
#include "juce_box2d/box2d/include/box2d/types.h"
#include "juce_box2d/box2d/include/box2d/box2d.h"

#ifdef __cplusplus
namespace juce
{
extern "C"
{
#endif

/** Reports a failed Box2D assertion as `jassertfalse` reports its own, using Box2D's file and line.

    The module makes this Box2D's default assertion handler, and `b2SetAssertFcn (handleBox2DAssertion)` reinstalls it after an application has replaced it.
    C linkage gives the function one global symbol, whatever the enclosing C++ namespace.

    @param condition    The source text of the failed condition.
    @param fileName     The source file containing the assertion.
    @param lineNumber   The line of the assertion within that file.

    @returns zero, so that Box2D continues instead of executing its own breakpoint.
*/
int handleBox2DAssertion (const char* condition, const char* fileName, int lineNumber);

#ifdef __cplusplus
}
}
#endif
