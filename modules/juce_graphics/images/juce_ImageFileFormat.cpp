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

struct DefaultImageFormats final
{
    static ImageFileFormat** get()
    {
        static DefaultImageFormats formats;
        return formats.formats;
    }

private:
    DefaultImageFormats() noexcept
    {
        formats[0] = &png;
        formats[1] = &jpg;
        formats[2] = &gif;
        formats[3] = &webp;
        formats[4] = nullptr;
    }

    PNGImageFormat  png;
    JPEGImageFormat jpg;
    GIFImageFormat  gif;
    WebPImageFormat webp;

    ImageFileFormat* formats[5];
};

Array<Image> ImageFileFormat::decodeFrames (InputStream& input)
{
    Array<Image> frames;

    auto image = decodeImage (input);
    if (image.isValid())
        frames.add (std::move (image));

    return frames;
}

ImageFileFormat* ImageFileFormat::findImageFormatForStream (InputStream& input)
{
    const auto streamPos = input.getPosition();

    for (auto** i = DefaultImageFormats::get(); *i != nullptr; ++i)
    {
        const bool found = (*i)->canUnderstand (input);
        input.setPosition (streamPos);

        if (found)
            return *i;
    }

    return nullptr;
}

ImageFileFormat* ImageFileFormat::findImageFormatForFileExtension (const File& file)
{
    for (auto** i = DefaultImageFormats::get(); *i != nullptr; ++i)
        if ((*i)->usesFileExtension (file))
            return *i;

    return nullptr;
}

//==============================================================================
Image ImageFileFormat::loadFrom (InputStream& input)
{
    if (auto* format = findImageFormatForStream (input))
        return format->decodeImage (input);

    return {};
}

Image ImageFileFormat::loadFrom (const File& file)
{
    FileInputStream stream (file);

    if (stream.openedOk())
    {
        BufferedInputStream b (stream, 8192);
        return loadFrom (b);
    }

    return {};
}

Image ImageFileFormat::loadFrom (const void* rawData, const size_t numBytes)
{
    if (rawData != nullptr && numBytes > 4)
    {
        MemoryInputStream stream (rawData, numBytes, false);
        return loadFrom (stream);
    }

    return {};
}

} // namespace juce
