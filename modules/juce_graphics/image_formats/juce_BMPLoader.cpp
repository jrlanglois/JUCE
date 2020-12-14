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
/** Good test suites:
    - https://sourceforge.net/projects/bmptestsuite/
    - http://entropymine.com/jason/bmpsuite/bmpsuite/html/bmpsuite.html
    
    Documentation:
    - https://itnext.io/bits-to-bitmaps-a-simple-walkthrough-of-bmp-image-format-765dc6857393
    - https://en.wikipedia.org/wiki/BMP_file_format
    - http://www.ece.ualberta.ca/~elliott/ee552/studentAppNotes/2003_w/misc/bmp_file_format/bmp_file_format.htm
    - http://atlc.sourceforge.net/bmp.html

    Forum stuff:
    - https://forum.juce.com/t/bmp-support/16349/5
    - https://forum.juce.com/t/fr-support-more-image-formats/34290/4
*/
struct BMPHeader
{
    enum class CompressionType
    {
        RGB = 0,
        RLE8,
        RLE4,
        bitfields,
        JPEG,
        PNG,
        alphaBitfields,
        CMYK = 11,
        CMYKRLE8,
        CMYKRLE4
    };

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

        {
            uint32 compression = 0;
            readValue (input, compression);
            compressionType = (CompressionType) compression;
        }

        readValue (input, imageDataSize);
        readValue (input, hPixelsPerMeter);
        readValue (input, vPixelsPerMeter);
        readValue (input, coloursUsed);
        readValue (input, coloursRequired);

        if (compressionType == CompressionType::bitfields)
        {
            readValue (input, redChannelBitmask);
            readValue (input, greenChannelBitmask);
            readValue (input, blueChannelBitmask);
            readValue (input, alphaChannelBitmask);
        }

        isBottomUp = height < 0;
        height = std::abs (height);

        recoverAgainstMalformedColoursUsed();
    }

    bool isUnsupportedFormat() const noexcept
    {
        return (bitsPerPixel != 24 && ! isPositiveAndNotGreaterThan (bitsPerPixel, 32) && ! isPowerOfTwo (bitsPerPixel))
            || (compressionType != CompressionType::RGB && coloursUsed == 0);
    }

    bool isPossiblyCorrupted() const noexcept
    {
        return bitsPerPixel == 0
            || (coloursUsed == 0 && bitsPerPixel < 8)
            || width <= 0 || height == 0 || planes != 1
            || headerSize == 0 || dataOffset == 0
            || (compressionType == CompressionType::bitfields && (bitsPerPixel != 16 && bitsPerPixel != 32));
    }

    uint16 magic = 0;
    uint32 fileSize = 0;
    uint16 reserved1 = 0, reserved2 = 0;
    uint32 dataOffset = 0, headerSize = 0;
    int width = 0, height = 0;
    uint16 planes = 0, bitsPerPixel = 0;
    CompressionType compressionType = CompressionType::RGB;
    uint32 imageDataSize = 0;
    int hPixelsPerMeter = 0, vPixelsPerMeter = 0;
    uint32 coloursUsed = 0, coloursRequired = 0;
    uint8 redChannelBitmask = 0, greenChannelBitmask = 0, blueChannelBitmask = 0, alphaChannelBitmask = 0;

    bool isBottomUp = false;

private:
    template<typename ValueType>
    static inline void readValue (InputStream& input, ValueType& destination, bool readShort = false)
    {
        destination = static_cast<ValueType> (readShort ? input.readShort() : input.readInt());
    }

    void limitColoursUsedAsPerBPP (int lowerLimit)
    {
        const auto upperLimit = std::pow (2.0, (double) bitsPerPixel);
        coloursUsed = (uint32) jlimit ((double) lowerLimit, upperLimit, (double) coloursUsed);
    }

    /** This tries to recover against funky values for colours used. */
    void recoverAgainstMalformedColoursUsed()
    {
        switch (bitsPerPixel)
        {
            case 1:
            case 4:
            case 8:
                limitColoursUsedAsPerBPP (1);
                break;

            case 16:
            case 24:
            case 32:
                limitColoursUsedAsPerBPP (0);
                break;

            default:
                bitsPerPixel = 0; // Unsupported format or corrupted file.
                break;
        };
    }
};

//==============================================================================
namespace BMPHelpers
{
    Array<PixelARGB> createColourTable (const BMPHeader& header, InputStream& input)
    {
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
        return colourTable;
    }
}

//==============================================================================
String BMPImageFormat::getFormatName()                  { return "Bitmap"; }
bool BMPImageFormat::usesFileExtension (const File& f)  { return f.hasFileExtension ("bmp") || f.hasFileExtension ("dib"); }
bool BMPImageFormat::canUnderstand (InputStream& input) { return input.readByte() == 'B' && input.readByte() == 'M'; }

Image BMPImageFormat::decodeImage (InputStream& input)
{
    const auto startPos = input.getPosition();
    const BMPHeader header (input);

    //This section simply lets subformats that we already know about handle the details and parsing:
    if (header.compressionType == BMPHeader::CompressionType::JPEG)
    {
        input.setPosition (startPos);
        return JPEGImageFormat().decodeImage (input);
    }
    else if (header.compressionType == BMPHeader::CompressionType::PNG)
    {
        input.setPosition (startPos);
        return PNGImageFormat().decodeImage (input);
    }

    //...back to Bitmap parsing:
    if (header.isUnsupportedFormat() || header.isPossiblyCorrupted())
        return {};

    const auto colourTable = BMPHelpers::createColourTable (header, input);

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

                //NB: Though rare, > 8 bpp images do support colour tables!
                if (! colourTable.isEmpty())
                {
                    auto* byte = rowData.getData();

                    auto getNextColourTableIndex = [&]()
                    {
*byte++;
                        int colourTableIndex =
                              ((*byte++ >> 16) & 0xff)
                            | ((*byte++ >> 8) & 0xff)
                            | (*byte++ & 0xff)
                        ;

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
                else
                {
                    for (int x = 0; x < header.width; ++x)
                    {
                        auto* d = &rowData[x * bytesPerPixel];
                        auto* p = (PixelARGB*) data.getPixelPointer (x, header.isBottomUp ? y : header.height - y - 1);

                        //NB: in ARGB scenarios, what might be perceived as an alpha byte is actually a reserved byte in the format.
                        p->setARGB ((uint8) 0xff, d[2], d[1], d[0]);
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
