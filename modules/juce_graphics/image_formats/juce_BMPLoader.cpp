/*
  ==============================================================================

   This file is part of the JUCE library.
   Copyright (c) 2017 - ROLI Ltd.

   JUCE is an open source library subject to commercial or open-source
   licensing.

   By using JUCE, you agree to the terms of both the JUCE 5 End-User License
   Agreement and JUCE 5 Privacy Policy (both updated and effective as of the
   27th April 2017).

   End User License Agreement: www.juce.com/juce-5-licence
   Privacy Policy: www.juce.com/juce-5-privacy-policy

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
/** @see a good test suite: https://sourceforge.net/projects/bmptestsuite/
    @see https://itnext.io/bits-to-bitmaps-a-simple-walkthrough-of-bmp-image-format-765dc6857393
    @see https://en.wikipedia.org/wiki/BMP_file_format
    @see https://forum.juce.com/t/bmp-support/16349/5
    @see https://forum.juce.com/t/fr-support-more-image-formats/34290/4
*/
struct BMPHeader
{
    BMPHeader (InputStream& input)
    {
        readValue (input, magic, true);
        readValue (input, fileSize);
        readValue (input, reserved1, true);
        readValue (input, reserved2, true);
        readValue (input, dataOffset);
        readValue (input, headerSize);
        readValue (input, width);
        readValue (input, height);
        readValue (input, planes, true);
        readValue (input, bitsPerPixel, true);
        readValue (input, compression);
        readValue (input, imageDataSize);
        readValue (input, hPixelsPerMeter);
        readValue (input, vPixelsPerMeter);
        readValue (input, coloursUsed);
        readValue (input, coloursRequired);

        isBottomUp = height < 0;
        height = std::abs (height);
    }

    bool isUnsupportedFormat() const noexcept
    {
        return (bitsPerPixel != 24 && ! isPositiveAndNotGreaterThan (bitsPerPixel, 32) && ! isPowerOfTwo (bitsPerPixel))
            || compression != 0;
    }

    bool isPossiblyCorrupted() const noexcept
    {
        return bitsPerPixel == 0
            || (coloursUsed == 0 && bitsPerPixel < 8)
            || width <= 0 || height == 0 || planes != 1
            || headerSize == 0 || dataOffset == 0;
    }

    uint16 magic = 0;
    uint32 fileSize = 0;
    uint16 reserved1 = 0, reserved2 = 0;
    uint32 dataOffset = 0, headerSize = 0;
    int width = 0, height = 0;
    uint16 planes = 0, bitsPerPixel = 0;
    uint32 compression = 0, imageDataSize = 0;
    int hPixelsPerMeter = 0, vPixelsPerMeter = 0;
    uint32 coloursUsed = 0, coloursRequired = 0;
    bool isBottomUp = false;

private:
    template<typename ValueType>
    static inline void readValue (InputStream& input, ValueType& destination, bool readShort = false)
    {
        destination = static_cast<ValueType> (readShort ? input.readShort() : input.readInt());
    }
};

//==============================================================================
BMPImageFormat::BMPImageFormat() {}
BMPImageFormat::~BMPImageFormat() {}

String BMPImageFormat::getFormatName()                  { return "Bitmap"; }
bool BMPImageFormat::usesFileExtension (const File& f)  { return f.hasFileExtension ("bmp") || f.hasFileExtension ("dib"); }
bool BMPImageFormat::canUnderstand (InputStream& input) { return input.readByte() == 'B' && input.readByte() == 'M'; }

Image BMPImageFormat::decodeImage (InputStream& input)
{
    BMPHeader header (input);

    if (header.isUnsupportedFormat() || header.isPossiblyCorrupted())
        return {};

    //This tries to recover against funky values for colours used:
    auto limitColoursUsedAsPerBPP = [&] (uint32 lowerLimit)
    {
        header.coloursUsed = jlimit (lowerLimit, (uint32) std::pow (2.0, (double) header.bitsPerPixel), header.coloursUsed);
    };

    switch (header.bitsPerPixel)
    {
        case 1:
        case 4:
        case 8:
            limitColoursUsedAsPerBPP (1u);
            break;

        case 16:
        case 24:
        case 32:
            limitColoursUsedAsPerBPP (0u);
            break;

        default:
            return {}; // Unsupported format or corrupted file.
    };

    Array<PixelARGB> colourTable;

    if (header.coloursUsed > 0)
    {
        colourTable.ensureStorageAllocated ((int) header.coloursUsed);

        //This particular case is to recover from broken colour table configurations:
        if (header.coloursUsed == 1u && header.bitsPerPixel <= 8)
        {
            colourTable.add ({ 0xff, 0, 0, 0 });
        }
        else
        {
            //Fill the colour table as expected:
            for (decltype (header.coloursUsed) i = 0; i < header.coloursUsed; ++i)
            {
                const auto b = (uint8) input.readByte();
                const auto g = (uint8) input.readByte();
                const auto r = (uint8) input.readByte();

                auto alphaOrSkip = (uint8) input.readByte();
                if (header.bitsPerPixel <= 8)
                    alphaOrSkip = 0xff; //...skip this byte for anything <= 8 bpp, as per the spec.

                colourTable.add ({ alphaOrSkip, r, g, b });
            }
        }
    }

    colourTable.minimiseStorageOverheads();

    Image image (Image::ARGB, header.width, header.height, true);
    Image::BitmapData data (image, Image::BitmapData::writeOnly);

    input.setPosition (header.dataOffset);

    const auto bytesPerRow = (int) (std::floor ((double) (header.bitsPerPixel * header.width + 31) / 32.0) * 4.0);

    HeapBlock<uint8> rowData (bytesPerRow, true);

    for (int y = 0; y < (int) header.height; ++y)
    {
        input.read (rowData.getData(), bytesPerRow);

        switch (header.bitsPerPixel)
        {
            case 1:
            {
                int bit = 7;
                auto* byte = rowData.getData();

                auto getNextColourTableIndex = [&]()
                {
                    const auto lastByte = *byte;
                    const auto colourTableIndex = (int) ((lastByte & (1 << bit)) >> bit);

                    --bit;
                    if (bit < 0)
                    {
                        bit = 7;
                        ++byte;
                    }

                    return colourTableIndex;
                };

                for (int x = 0; x < header.width; ++x)
                {
                    auto* p = (PixelARGB*) data.getPixelPointer (x, header.isBottomUp ? y : header.height - y - 1);
                    const auto colourTableIndex = getNextColourTableIndex();
                    *p = colourTable[colourTableIndex];
                }
            }
            break;

            case 4:
            {
                int halfByte = 1;
                auto* byte = rowData.getData();

                auto getNextColourTableIndex = [&]()
                {
                    const auto lastByte = *byte;
                    int colourTableIndex = 0;

                    if (halfByte == 1)
                        colourTableIndex = lastByte >> 4;   //Get the high 4-bit set
                    else
                        colourTableIndex = lastByte & 0x0F; //Get the low 4-bit set

                    --halfByte;
                    if (halfByte < 0)
                    {
                        halfByte = 1;
                        ++byte;
                    }

                    return colourTableIndex;
                };

                for (int x = 0; x < header.width; ++x)
                {
                    auto* p = (PixelARGB*) data.getPixelPointer (x, header.isBottomUp ? y : header.height - y - 1);

                    const auto colourTableIndex = getNextColourTableIndex();

                    if (isPositiveAndBelow (colourTableIndex, colourTable.size()))
                        *p = colourTable[colourTableIndex];
                    else
                        p->setARGB ((uint8) 0xff, 0, 0, 0); //Must set to black when the index is out of range, as per the spec.
                }
            }
            break;

            default:
            {
                const auto bytesPerPixel = jmax (1, (int) header.bitsPerPixel / 8);

                for (int x = 0; x < header.width; ++x)
                {
                    auto* d = &rowData[x * bytesPerPixel];
                    auto* p = (PixelARGB*) data.getPixelPointer (x, header.isBottomUp ? y : header.height - y - 1);

                    if (header.bitsPerPixel == 8)
                    {
                        const auto colourTableIndex = (int) d[0];

                        if (isPositiveAndBelow (colourTableIndex, colourTable.size()))
                            *p = colourTable[colourTableIndex];
                        else
                            p->setARGB ((uint8) 0xff, 0, 0, 0); //Must set to black when the index is out of range, as per the spec.
                    }
                    else
                    {
                        p->setARGB (bytesPerPixel == 4 ? d[3] : (uint8) 0xff, d[2], d[1], d[0]);
                    }
                }
            }
            break;
        }
    }

    return image;
}

bool BMPImageFormat::writeImageToStream (const Image& sourceImage, OutputStream& stream)
{
    const auto image = sourceImage.convertedToFormat (Image::ARGB);
    const auto w = image.getWidth();
    const auto h = image.getHeight();
    const auto size = w * h * 4;

    stream.writeByte ('B');
    stream.writeByte ('M');
    stream.writeInt (40 + size);
    stream.writeShort (0);
    stream.writeShort (0);
    stream.writeInt (54);
    stream.writeInt (40);
    stream.writeInt (w);
    stream.writeInt (h);
    stream.writeShort (1);
    stream.writeShort (32);
    stream.writeInt (0);
    stream.writeInt (size);
    stream.writeInt (2835);
    stream.writeInt (2835);
    stream.writeInt (0);
    stream.writeInt (0);

    const Image::BitmapData data (image, Image::BitmapData::readOnly);
    for (int y = 0; y < h; ++y)
    {
        for (int x = 0; x < w; ++x)
        {
            auto* p = (PixelARGB*) data.getPixelPointer (x, h - y - 1);
            stream.writeByte ((int8) p->getBlue());
            stream.writeByte ((int8) p->getGreen());
            stream.writeByte ((int8) p->getRed());
            stream.writeByte ((int8) p->getAlpha());
        }
    }

    return true;
}

} // namespace juce
