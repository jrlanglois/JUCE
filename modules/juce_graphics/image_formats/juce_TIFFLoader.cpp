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
#undef deflate

//==============================================================================
namespace tiff
{
    //==============================================================================
    /** */
    enum class ByteOrderType
    {
        unknown,
        intel,
        motorola
    };

    /** */
    enum class TagType
    {
        none = 0,
        byteType,
        asciiType,
        shortType,
        longType,
        rationalType,
        sbyteType,
        undefinedType,
        sshortType,
        slongType,
        srationalType,
        floatType,
        doubleType
    };

    /** */
    enum class PhotometricInterpretation
    {
        unknown = -1,
        whiteIsZero,
        blackIsZero
    };

    /** */
    enum class CompressionType
    {
        unknown = 0,
        none,
        modifiedHuffmanRLE,
        packBits
    };

    /** */
    enum class ResolutionUnit
    {
        unknown = 0,
        none,
        inch,
        centimetre
    };

    /** */
    enum class CompressionScheme
    {
        unknown = 0,
        uncompressed,
        jpeg,
        webp
    };

    //==============================================================================
    /** */
    struct Header final
    {
        Header() noexcept = default;

        bool read (InputStream& in)
        {
            char bytes[8];
            zerostruct (bytes);
            return in.read (bytes, 8) == 8 && read (bytes);
        }

        bool read (char bytes[8])
        {
            const auto f = bytes[0];
            const auto s = bytes[1];

            if (f == 'I' && s == 'I')
                byteOrder = ByteOrderType::intel;
            else if (f == 'M' && s == 'M')
                byteOrder = ByteOrderType::motorola;

            if (byteOrder == ByteOrderType::unknown)
                return false;

            if (isBigEndian())
            {
                versionNumber = (int16) ByteOrder::bigEndianShort (bytes + 2);
                firstIFDOffsetLocationBytes = (int64) ByteOrder::makeInt (bytes[7], bytes[6], bytes[5], bytes[4]);
            }
            else
            {
                versionNumber = (int16) ByteOrder::littleEndianShort (bytes + 2);
                firstIFDOffsetLocationBytes = (int64) ByteOrder::makeInt (bytes[4], bytes[5], bytes[6], bytes[7]);
            }

            return isValid();
        }

        bool isValid() const noexcept
        {
            return byteOrder != ByteOrderType::unknown
                && versionNumber == 42                  // You read that right; 42 is the answer to versioning here, as per the specs.
                && firstIFDOffsetLocationBytes > 8;
        }

        bool isBigEndian() const noexcept { return byteOrder == ByteOrderType::motorola; }

        ByteOrderType byteOrder = ByteOrderType::unknown;
        int16 versionNumber = -1;
        int64 firstIFDOffsetLocationBytes = -1;
    };

    //==============================================================================
    /** */
    struct TagEntry final
    {
        TagEntry() noexcept = default;

        bool read (const Header& header, InputStream& in)
        {
            char bytes[12];
            zerostruct (bytes);
            return in.read (bytes, 12) == 12 && read (header, bytes);
        }

        bool read (const Header& header, char bytes[12])
        {
            int tagTypeValue = 0;

            if (header.isBigEndian())
            {
                tag = (int) ByteOrder::bigEndianShort (bytes);
                tagTypeValue = (int) ByteOrder::bigEndianShort (bytes + 2);
                count = (int) ByteOrder::makeInt (bytes[7], bytes[6], bytes[5], bytes[4]);
                offset = (int) ByteOrder::makeInt (bytes[11], bytes[10], bytes[9], bytes[8]);
            }
            else
            {
                tag = (int) ByteOrder::littleEndianShort (bytes);
                tagTypeValue = (int) ByteOrder::littleEndianShort (bytes + 2);
                count = (int) ByteOrder::makeInt (bytes[4], bytes[5], bytes[6], bytes[7]);
                offset = (int) ByteOrder::makeInt (bytes[8], bytes[9], bytes[10], bytes[11]);
            }

            if (tagTypeValue < 0)
                return false;

            type = (TagType) tagTypeValue;
            return isValid();
        }

        bool isValid() const
        {
            if (type == TagType::none)
                return true;

            return tag >= 0
                && count > 0
                && offset >= 0;
        }

        uint8 toUInt8()
        {
            jassert (type == TagType::byteType);
            return 0;
        }

        uint16 toUInt16()
        {
            jassert (type == TagType::shortType);
            return 0;
        }

        uint32 toUInt32()
        {
            jassert (type == TagType::longType);
            return 0;
        }

        uint64 toUInt64()
        {
            jassert (type == TagType::rationalType);
            return 0;
        }

        int8 toInt8()
        {
            jassert (type == TagType::sbyteType);
            return 0;
        }

        int16 toInt16()
        {
            jassert (type == TagType::sshortType);
            return 0;
        }

        int32 toInt32()
        {
            jassert (type == TagType::slongType);
            return 0;
        }

        int64 toInt64()
        {
            jassert (type == TagType::srationalType);
            return 0;
        }

        float toFloat()
        {
            jassert (type == TagType::floatType);
            return 0.0f;
        }

        double toDouble()
        {
            jassert (type == TagType::doubleType);
            return 0.0;
        }

        String toString()
        {
            jassert (type == TagType::asciiType);
            return {};
        }

        int tag = -1, count = -1, offset = -1;
        TagType type = TagType::none;
    };

    //==============================================================================
    class DecoderBase
    {
    public:
        DecoderBase() noexcept = default;
        virtual ~DecoderBase() = default;

        virtual Image decode (int64 startPos, int64 endPos, InputStream& in) = 0;
    };

    struct UncompressedDecoder final : public DecoderBase
    {
        UncompressedDecoder() = default;

        Image decode (int64 startPos, int64, InputStream& in) override
        {
            if (! in.setPosition (startPos))
                return {};

            return {};
        }
    };

    struct JPEGDecoder final : public DecoderBase
    {
        JPEGDecoder() = default;

        Image decode (int64 startPos, int64, InputStream& in) override
        {
            if (! in.setPosition (startPos))
                return {};

            return JPEGImageFormat().decodeImage (in);
        }
    };

    struct WebPDecoder final : public DecoderBase
    {
        WebPDecoder() = default;

        Image decode (int64 startPos, int64, InputStream& in) override
        {
            if (! in.setPosition (startPos))
                return {};

            return WebPImageFormat().decodeImage (in);
        }
    };

    //==============================================================================
    /** */
    struct ImageData final
    {
        ImageData() noexcept
        {
            zerostruct (pageNumber);
            zerostruct (bitsPerSample);
            zerostruct (sampleFormat);
        }

        void setCompressionScheme (int compressionCode)
        {
            switch (compressionCode)
            {
                case 1:
                    compression = CompressionScheme::uncompressed;
                    decoder.reset (new UncompressedDecoder());
                break;

                case 6:
                case 7:
                case 99:
                case 34712:
                case 34892:
                    compression = CompressionScheme::jpeg;
                    decoder.reset (new JPEGDecoder());
                break;

                case 50001:
                    compression = CompressionScheme::webp;
                    decoder.reset (new WebPDecoder());
                break;

                default:
                    compression = CompressionScheme::unknown;
                    decoder.reset();
                break;
            };
        }

        int imageWidth = -1, imageHeight = -1, photometric = -1,
            samplesPerPixel = -1, rowsPerStrip = -1, planarConfig = -1;

        CompressionScheme compression = CompressionScheme::unknown;
        int pageNumber[2], bitsPerSample[3], sampleFormat[3];
        Array<int> stripOffsets, stripByteCounts;
        NamedValueSet metadata;

        std::unique_ptr<DecoderBase> decoder;
    };

    //==============================================================================
    /** Manages reading and writing the image file directory structure of a TIFF file.

        @note The file reading here gets quite weird because there
              are three possible physical arrangements of image file entries in a TIFF file:

              1) Header, IFE0,      IFE1,       IFE n,      Image 0,    Image 1,    Image n...
              2) Header, IFE0,      Image 0,    IFE1,       Image 1,    IFE n,      Image n...
              3) Header, Image 0,   Image 1, I  mage n,     IFE0,       IFE1,       IFE n...
    */
    struct ImageFileDirectory final
    {
        ImageFileDirectory() = default;

        bool read (const Header& header, InputStream& in)
        {
            if (header.isBigEndian())
                numEntries = (int) in.readShortBigEndian();
            else
                numEntries = (int) in.readShort();

            for (int i = 0; i < numEntries; ++i)
                if (auto entry = std::make_unique<TagEntry>())
                    if (entry->read (header, in))
                        entries.add (entry.release());

            return numEntries == entries.size();
        }

        std::unique_ptr<ImageData> createImageData() const
        {
            auto id = std::make_unique<ImageData>();

            for (auto* e : entries)
            {
                switch (e->tag)
                {
                    case 256: id->imageHeight = e->toInt32(); break;
                    case 257: id->imageWidth = e->toInt32(); break;
                    case 259: id->setCompressionScheme (e->toInt32()); break;
                    case 262: id->photometric = e->toInt32(); break;

                    case 269:   id->metadata.set (TIFFImageFormat::documentNameId, e->toString()); break;
                    case 270:   id->metadata.set (TIFFImageFormat::imageDescriptionId, e->toString()); break;
                    case 271:   id->metadata.set (TIFFImageFormat::makeId, e->toString()); break;
                    case 272:   id->metadata.set (TIFFImageFormat::modelId, e->toString()); break;
                    case 285:   id->metadata.set (TIFFImageFormat::pageNameId, e->toString()); break;
                    case 305:   id->metadata.set (TIFFImageFormat::softwareId, e->toString()); break;
                    case 306:   id->metadata.set (TIFFImageFormat::dateTimeId, e->toString()); break;
                    case 315:   id->metadata.set (TIFFImageFormat::artistId, e->toString()); break;
                    case 316:   id->metadata.set (TIFFImageFormat::hostComputerId, e->toString()); break;
                    case 333:   id->metadata.set (TIFFImageFormat::inkNamesId, e->toString()); break;
                    case 337:   id->metadata.set (TIFFImageFormat::targetPrinterId, e->toString()); break;
                    case 33432: id->metadata.set (TIFFImageFormat::copyrightId, e->toString()); break;

                    default:
                    break;
                };
            }

            return std::move (id);
        }

        int numEntries = -1;
        OwnedArray<TagEntry> entries;
    };
}

//==============================================================================
const Identifier TIFFImageFormat::artistId = "artist";
const Identifier TIFFImageFormat::copyrightId = "copyright";
const Identifier TIFFImageFormat::dateTimeId = "dateTime";
const Identifier TIFFImageFormat::documentNameId = "documentName";
const Identifier TIFFImageFormat::hostComputerId = "hostComputer";
const Identifier TIFFImageFormat::imageDescriptionId = "imageDescription";
const Identifier TIFFImageFormat::inkNamesId = "inkNames";
const Identifier TIFFImageFormat::makeId = "make";
const Identifier TIFFImageFormat::modelId = "model";
const Identifier TIFFImageFormat::pageNameId = "pageName";
const Identifier TIFFImageFormat::softwareId = "software";
const Identifier TIFFImageFormat::targetPrinterId = "targetPrinter";

//==============================================================================
String TIFFImageFormat::getFormatName() { return "TIFF"; }

bool TIFFImageFormat::usesFileExtension (const File& f)
{
    /** As per the TIFF 6.0 specs:

        The recommended MS-DOS, UNIX, and OS/2 file extension for TIFF files is ".TIF".
        On an Apple Macintosh computer, the recommended Filetype is ".TIFF".
    */
    return f.hasFileExtension ("tif") || f.hasFileExtension ("tiff");
}

bool TIFFImageFormat::canUnderstand (InputStream& in)
{
    return tiff::Header().read (in);
}

Image TIFFImageFormat::decodeImage (InputStream& in)
{
    const auto startPos = in.getPosition();

    tiff::Header header;
    if (! header.read (in))
        return {};

    //const int16 numDirectories = header.isBigEndian() ? in.readShortBigEndian() : in.readShort();
    //if (numDirectories < 1)
    //    return {};

    if (! in.setPosition (startPos + header.firstIFDOffsetLocationBytes))
        return {};

    tiff::ImageFileDirectory imageFileDirectory;
    if (! imageFileDirectory.read (header, in))
        return {};

    if (auto id = imageFileDirectory.createImageData())
    {
        if (auto* dec = id->decoder.get())
        {
            Image image;

            if (auto* uncompDec = dynamic_cast<tiff::UncompressedDecoder*> (dec))
                image = uncompDec->decode (startPos, 0, in);
            else
                image = dec->decode (startPos, 0, in);

            if (image.isNull())
                return {};

            for (const auto& v : id->metadata)
                image.getProperties()->set (v.name, v.value);

            return image;
        }
    }

    return {};
}

bool TIFFImageFormat::writeImageToStream (const Image& /*sourceImage*/, OutputStream& /*destStream*/)
{
    jassertfalse; // writing isn't implemented for TIFFs!
    return false;
}

} // namespace juce
