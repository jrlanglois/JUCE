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

namespace juce
{

/** The direction of a focus-navigation request.

    @tags{GUI}
*/
enum class FocusNavigationDirection
{
    left,
    right,
    up,
    down
};

/** Describes how a Component responded to a focus-navigation request.

    @tags{GUI}
*/
enum class FocusNavigationResult
{
    /** The Component did not use the request.

        The request may be offered to an owning Component and then used for
        outward focus traversal.
    */
    unhandled,

    /** The Component performed a local action and retained keyboard focus. */
    handled,

    /** The Component deliberately retained the request without moving keyboard focus.

        This is appropriate for modal scopes, open popup menus, active editors,
        and other intentional focus traps.
    */
    blocked
};

/** Controls whether a Component tree participates in directional focus navigation.

    The effective mode is resolved from the focused Component towards its peer root.
    The first value other than inherit is used. If every Component inherits, the
    platform default is used.

    @tags{GUI}
*/
enum class FocusNavigationMode
{
    /** Inherit the mode from the nearest ancestor that specifies one. */
    inherit,

    /** Resolve to directional navigation on tvOS and disabled navigation elsewhere. */
    platformDefault,

    /** Bypass directional dispatch and preserve the platform's existing behaviour. */
    disabled,

    /** Enable directional widget handling and spatial focus traversal. */
    directional
};

} // namespace juce
