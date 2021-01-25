/*
  ==============================================================================

   This file is part of the JUCE library.
   Copyright (c) 2020 - Raw Material Software Limited

   JUCE is an open source library subject to commercial or open-source
   licensing.

   By using JUCE, you agree to the terms of both the JUCE 6 End-User License
   Agreement and JUCE Privacy Policy (both effective as of the 16th June 2020).

   End User License Agreement: www.juce.com/juce-6-licence
   Privacy Policy: www.juce.com/juce-privacy-policy

   Or: You may also use this code under the terms of the GPL v3 (see
   www.gnu.org/licenses).

   JUCE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES, WHETHER
   EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE, ARE
   DISCLAIMED.

  ==============================================================================
*/

namespace juce
{

//==============================================================================
String BMPImageFormat::getFormatName()                  { return "BMP"; }
bool BMPImageFormat::usesFileExtension (const File& f)  { return f.hasFileExtension ("bmp"); }

bool BMPImageFormat::canUnderstand (InputStream& in)
{
    char header [4];

    return (in.read (header, sizeof (header)) == (int) sizeof (header))
             && header[0] == 'B'
             && header[1] == 'M'
             && header[2] == 'P';
}

Image BMPImageFormat::decodeImage (InputStream&)
{
    return {};
}

bool BMPImageFormat::writeImageToStream (const Image& /*sourceImage*/, OutputStream& /*destStream*/)
{
    jassertfalse; // writing isn't implemented for Bitmaps!
    return false;
}

} // namespace juce
