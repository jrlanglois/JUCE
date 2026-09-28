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

/** Draws a Box2D world's debug geometry into a JUCE graphics context.

    The world rectangle centred on the view centre maps onto the target area with world +Y pointing up, scaling each axis independently.
    Outlines keep a constant pixel thickness, and points and labels are drawn upright in component space.

    @tags{Box2D}
*/
class Box2DRenderer
{
public:
    /** Constructs a renderer with Box2D's default debug-draw configuration. */
    Box2DRenderer() = default;

    /** Destroys the renderer. */
    virtual ~Box2DRenderer() = default;

    //==============================================================================
    /** Draws a Box2D world into a target area.

        @param graphics     The JUCE graphics context receiving the debug drawing.
        @param worldId      The valid Box2D world, which must not be stepping.
        @param viewCentre   The world-space centre of the viewport.
        @param viewSize     The positive viewport width and height in Box2D world units.
        @param targetArea   The non-empty destination area in component coordinates.
    */
    void render (Graphics& graphics, b2WorldId worldId, b2Pos viewCentre, b2Vec2 viewSize, const juce::Rectangle<float>& targetArea) const;

    /** @returns the mutable debug-draw configuration, whose callback, context, and drawing-bounds fields `render()` overrides in a per-call copy. */
    [[nodiscard]] b2DebugDraw& getDebugDraw() noexcept { return debugDraw; }

    /** @returns the debug-draw configuration. */
    [[nodiscard]] const b2DebugDraw& getDebugDraw() const noexcept { return debugDraw; }

    /** @returns the opaque JUCE colour corresponding to a Box2D RGB value. */
    [[nodiscard]] virtual Colour getColour (b2HexColor colour) const;

    /** @returns the outline thickness in component pixels. */
    [[nodiscard]] virtual float getLineThickness() const;

private:
    //==============================================================================
    b2DebugDraw debugDraw = b2DefaultDebugDraw();

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Box2DRenderer)
};

} // namespace juce
