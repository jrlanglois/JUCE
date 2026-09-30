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

#include "Box2DSamples.h"

namespace Box2DSamples
{

void Camera::reset() noexcept
{
    centre = { 0.0f, 20.0f };
    zoom = 1.0f;
}

void Camera::setDrawableSize (float newWidth, float newHeight) noexcept
{
    drawableWidth = newWidth;
    drawableHeight = newHeight;
}

void Camera::setViewToBounds (b2AABB bounds) noexcept
{
    if (drawableWidth <= 0.0f || drawableHeight <= 0.0f)
        return;

    const auto extents = b2AABB_Extents (bounds);

    if (extents.x <= 0.0f && extents.y <= 0.0f)
        return;

    zoom = std::max (extents.x / getAspectRatio(), extents.y);
    zoom = std::max (zoom, 0.01f);
    centre = b2ToPos (b2AABB_Center (bounds));
}

float Camera::getAspectRatio() const noexcept
{
    return drawableWidth > 0.0f && drawableHeight > 0.0f ? drawableWidth / drawableHeight : 16.0f / 9.0f;
}

b2Vec2 Camera::getViewSize() const noexcept
{
    const auto positiveZoom = std::max (zoom, 0.01f);

    return { 2.0f * positiveZoom * getAspectRatio(), 2.0f * positiveZoom };
}

b2AABB Camera::getVisibleBounds() const noexcept
{
    const auto viewSize = getViewSize();
    const b2Vec2 halfSize = { 0.5f * viewSize.x, 0.5f * viewSize.y };

    return { { (float) (centre.x - halfSize.x), (float) (centre.y - halfSize.y) },
             { (float) (centre.x + halfSize.x), (float) (centre.y + halfSize.y) } };
}

juce::Point<float> Camera::convertWorldToComponent (b2Pos worldPosition, const juce::Rectangle<float>& targetArea) const noexcept
{
    const auto viewSize = getViewSize();

    if (viewSize.x <= 0.0f || viewSize.y <= 0.0f)
        return targetArea.getCentre();

    const auto scaleX = targetArea.getWidth() / viewSize.x;
    const auto scaleY = targetArea.getHeight() / viewSize.y;

    const auto localX = (float) (worldPosition.x - centre.x) * scaleX;
    const auto localY = (float) (centre.y - worldPosition.y) * scaleY;

    return { targetArea.getCentreX() + localX,
             targetArea.getCentreY() + localY };
}

b2Pos Camera::convertComponentToWorld (juce::Point<float> componentPosition, const juce::Rectangle<float>& targetArea) const noexcept
{
    const auto viewSize = getViewSize();

    if (viewSize.x <= 0.0f || viewSize.y <= 0.0f)
        return centre;

    const auto scaleX = targetArea.getWidth() / viewSize.x;
    const auto scaleY = targetArea.getHeight() / viewSize.y;

    const auto localX = componentPosition.x - targetArea.getCentreX();
    const auto localY = componentPosition.y - targetArea.getCentreY();

    return { centre.x + localX / scaleX,
             centre.y - localY / scaleY };
}

} // namespace Box2DSamples
