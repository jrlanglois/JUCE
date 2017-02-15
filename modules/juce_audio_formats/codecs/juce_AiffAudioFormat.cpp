/*
  ==============================================================================

   This file is part of the JUCE library.
   Copyright (c) 2015 - ROLI Ltd.

   Permission is granted to use this software under the terms of either:
   a) the GPL v2 (or any later version)
   b) the Affero GPL v3

   Details of these licenses can be found at: www.gnu.org/licenses

   JUCE is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
   A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

   ------------------------------------------------------------------------------

   To release a closed-source product which uses JUCE, commercial licenses are
   available: visit www.juce.com for more information.

  ==============================================================================
*/

static const char* const aiffFormatName = "AIFF file";

//==============================================================================
const char* const AiffAudioFormat::appleOneShot         = "apple one shot";
const char* const AiffAudioFormat::appleRootSet         = "apple root set";
const char* const AiffAudioFormat::appleRootNote        = "apple root note";
const char* const AiffAudioFormat::appleBeats           = "apple beats";
const char* const AiffAudioFormat::appleDenominator     = "apple denominator";
const char* const AiffAudioFormat::appleNumerator       = "apple numerator";
const char* const AiffAudioFormat::appleTag             = "apple tag";
const char* const AiffAudioFormat::appleKey             = "apple key";

//==============================================================================
namespace AiffFileHelpers
{
    inline int chunkName (const char* name) noexcept    { return (int) ByteOrder::littleEndianInt (name); }

   #if JUCE_MSVC
    #pragma pack (push, 1)
   #endif

    //==============================================================================
    struct InstChunk
    {
        struct Loop
        {
            uint16 type; // these are different in AIFF and WAV
            uint16 startIdentifier;
            uint16 endIdentifier;
        } JUCE_PACKED;

        int8 baseNote;
        int8 detune;
        int8 lowNote;
        int8 highNote;
        int8 lowVelocity;
        int8 highVelocity;
        int16 gain;
        Loop sustainLoop;
        Loop releaseLoop;

        void copyTo (StringPairArray& values) const
        {
            values.set ("MidiUnityNote",        String (baseNote));
            values.set ("Detune",               String (detune));

            values.set ("LowNote",              String (lowNote));
            values.set ("HighNote",             String (highNote));
            values.set ("LowVelocity",          String (lowVelocity));
            values.set ("HighVelocity",         String (highVelocity));

            values.set ("Gain",                 String ((int16) ByteOrder::swapIfLittleEndian ((uint16) gain)));

            values.set ("NumSampleLoops",       String (2));        // always 2 with AIFF, WAV can have more
            values.set ("Loop0Type",            String (ByteOrder::swapIfLittleEndian (sustainLoop.type)));
            values.set ("Loop0StartIdentifier", String (ByteOrder::swapIfLittleEndian (sustainLoop.startIdentifier)));
            values.set ("Loop0EndIdentifier",   String (ByteOrder::swapIfLittleEndian (sustainLoop.endIdentifier)));
            values.set ("Loop1Type",            String (ByteOrder::swapIfLittleEndian (releaseLoop.type)));
            values.set ("Loop1StartIdentifier", String (ByteOrder::swapIfLittleEndian (releaseLoop.startIdentifier)));
            values.set ("Loop1EndIdentifier",   String (ByteOrder::swapIfLittleEndian (releaseLoop.endIdentifier)));
        }

        static uint16 getValue16 (const StringPairArray& values, const char* name, const char* def)
        {
            return ByteOrder::swapIfLittleEndian ((uint16) values.getValue (name, def).getIntValue());
        }

        static int8 getValue8 (const StringPairArray& values, const char* name, const char* def)
        {
            return (int8) values.getValue (name, def).getIntValue();
        }

        static void create (MemoryBlock& block, const StringPairArray& values)
        {
            if (values.getAllKeys().contains ("MidiUnityNote", true))
            {
                block.setSize ((sizeof (InstChunk) + 3) & ~(size_t) 3, true);
                InstChunk& inst = *static_cast<InstChunk*> (block.getData());

                inst.baseNote      = getValue8 (values, "MidiUnityNote", "60");
                inst.detune        = getValue8 (values, "Detune", "0");
                inst.lowNote       = getValue8 (values, "LowNote", "0");
                inst.highNote      = getValue8 (values, "HighNote", "127");
                inst.lowVelocity   = getValue8 (values, "LowVelocity", "1");
                inst.highVelocity  = getValue8 (values, "HighVelocity", "127");
                inst.gain          = (int16) getValue16 (values, "Gain", "0");

                inst.sustainLoop.type              = getValue16 (values, "Loop0Type", "0");
                inst.sustainLoop.startIdentifier   = getValue16 (values, "Loop0StartIdentifier", "0");
                inst.sustainLoop.endIdentifier     = getValue16 (values, "Loop0EndIdentifier", "0");
                inst.releaseLoop.type              = getValue16 (values, "Loop1Type", "0");
                inst.releaseLoop.startIdentifier   = getValue16 (values, "Loop1StartIdentifier", "0");
                inst.releaseLoop.endIdentifier     = getValue16 (values, "Loop1EndIdentifier", "0");
            }
        }

    } JUCE_PACKED;

    //==============================================================================
    struct BASCChunk
    {
        enum Key
        {
            minor = 1,
            major = 2,
            neither = 3,
            both = 4
        };

        BASCChunk (InputStream& input)
        {
            zerostruct (*this);

            flags       = (uint32) input.readIntBigEndian();
            numBeats    = (uint32) input.readIntBigEndian();
            rootNote    = (uint16) input.readShortBigEndian();
            key         = (uint16) input.readShortBigEndian();
            timeSigNum  = (uint16) input.readShortBigEndian();
            timeSigDen  = (uint16) input.readShortBigEndian();
            oneShot     = (uint16) input.readShortBigEndian();
            input.read (unknown, sizeof (unknown));
        }

        void addToMetadata (StringPairArray& metadata) const
        {
            const bool rootNoteSet = rootNote != 0;

            setBoolFlag (metadata, AiffAudioFormat::appleOneShot, oneShot == 2);
            setBoolFlag (metadata, AiffAudioFormat::appleRootSet, rootNoteSet);

            if (rootNoteSet)
                metadata.set (AiffAudioFormat::appleRootNote,   String (rootNote));

            metadata.set (AiffAudioFormat::appleBeats,          String (numBeats));
            metadata.set (AiffAudioFormat::appleDenominator,    String (timeSigDen));
            metadata.set (AiffAudioFormat::appleNumerator,      String (timeSigNum));

            const char* keyString = nullptr;

            switch (key)
            {
                case minor:     keyString = "major";        break;
                case major:     keyString = "major";        break;
                case neither:   keyString = "neither";      break;
                case both:      keyString = "both";         break;
            }

            if (keyString != nullptr)
                metadata.set (AiffAudioFormat::appleKey, keyString);
        }

        void setBoolFlag (StringPairArray& values, const char* name, bool shouldBeSet) const
        {
            values.set (name, shouldBeSet ? "1" : "0");
        }

        uint32 flags;
        uint32 numBeats;
        uint16 rootNote;
        uint16 key;
        uint16 timeSigNum;
        uint16 timeSigDen;
        uint16 oneShot;
        uint8 unknown[66];
    } JUCE_PACKED;

   #if JUCE_MSVC
    #pragma pack (pop)
   #endif

    //==============================================================================
    namespace CATEChunk
    {
        static bool isValidTag (const char* d) noexcept
        {
            return CharacterFunctions::isLetterOrDigit (d[0]) && CharacterFunctions::isUpperCase (static_cast<juce_wchar> (d[0]))
                && CharacterFunctions::isLetterOrDigit (d[1]) && CharacterFunctions::isLowerCase (static_cast<juce_wchar> (d[1]))
                && CharacterFunctions::isLetterOrDigit (d[2]) && CharacterFunctions::isLowerCase (static_cast<juce_wchar> (d[2]));
        }

        static bool isAppleGenre (const String& tag) noexcept
        {
            static const char* appleGenres[] =
            {
                "Rock/Blues",
                "Electronic/Dance",
                "Jazz",
                "Urban",
                "World/Ethnic",
                "Cinematic/New Age",
                "Orchestral",
                "Country/Folk",
                "Experimental",
                "Other Genre"
            };

            for (int i = 0; i < numElementsInArray (appleGenres); ++i)
                if (tag == appleGenres[i])
                    return true;

            return false;
        }

        static String read (InputStream& input, const uint32 length)
        {
            MemoryBlock mb;
            input.skipNextBytes (4);
            input.readIntoMemoryBlock (mb, (ssize_t) length - 4);

            StringArray tagsArray;

            const char* data = static_cast<const char*> (mb.getData());
            const char* dataEnd = data + mb.getSize();

            while (data < dataEnd)
            {
                bool isGenre = false;

                if (isValidTag (data))
                {
                    const String tag = String (CharPointer_UTF8 (data), CharPointer_UTF8 (dataEnd));
                    isGenre = isAppleGenre (tag);
                    tagsArray.add (tag);
                }

                data += isGenre ? 118 : 50;

                if (data < dataEnd && data[0] == 0)
                {
                    if      (data + 52  < dataEnd && isValidTag (data + 50))   data += 50;
                    else if (data + 120 < dataEnd && isValidTag (data + 118))  data += 118;
                    else if (data + 170 < dataEnd && isValidTag (data + 168))  data += 168;
                }
            }

            return tagsArray.joinIntoString (";");
        }
    }

    //==============================================================================
    namespace MarkChunk
    {
        static bool metaDataContainsZeroIdentifiers (const StringPairArray& values)
        {
            // (zero cue identifiers are valid for WAV but not for AIFF)
            const String cueString ("Cue");
            const String noteString ("CueNote");
            const String identifierString ("Identifier");

            const StringArray& keys = values.getAllKeys();

            for (int i = 0; i < keys.size(); ++i)
            {
                const String key (keys[i]);

                if (key.startsWith (noteString))
                    continue; // zero identifier IS valid in a COMT chunk

                if (key.startsWith (cueString) && key.contains (identifierString))
                {
                    const int value = values.getValue (key, "-1").getIntValue();

                    if (value == 0)
                        return true;
                }
            }

            return false;
        }

        static void create (MemoryBlock& block, const StringPairArray& values)
        {
            const int numCues = values.getValue ("NumCuePoints", "0").getIntValue();

            if (numCues > 0)
            {
                MemoryOutputStream out (block, false);

                out.writeShortBigEndian ((short) numCues);

                const int numCueLabels = values.getValue ("NumCueLabels", "0").getIntValue();
                const int idOffset = metaDataContainsZeroIdentifiers (values) ? 1 : 0; // can't have zero IDs in AIFF

               #if JUCE_DEBUG
                Array<int> identifiers;
               #endif

                for (int i = 0; i < numCues; ++i)
                {
                    const String prefixCue ("Cue" + String (i));
                    const int identifier = idOffset + values.getValue (prefixCue + "Identifier", "1").getIntValue();

                   #if JUCE_DEBUG
                    jassert (! identifiers.contains (identifier));
                    identifiers.add (identifier);
                   #endif

                    const int offset = values.getValue (prefixCue + "Offset", "0").getIntValue();
                    String label ("CueLabel" + String (i));

                    for (int labelIndex = 0; labelIndex < numCueLabels; ++labelIndex)
                    {
                        const String prefixLabel ("CueLabel" + String (labelIndex));
                        const int labelIdentifier = idOffset + values.getValue (prefixLabel + "Identifier", "1").getIntValue();

                        if (labelIdentifier == identifier)
                        {
                            label = values.getValue (prefixLabel + "Text", label);
                            break;
                        }
                    }

                    out.writeShortBigEndian ((short) identifier);
                    out.writeIntBigEndian (offset);

                    const size_t labelLength = jmin ((size_t) 254, label.getNumBytesAsUTF8()); // seems to need null terminator even though it's a pstring
                    out.writeByte ((char) labelLength + 1);
                    out.write (label.toUTF8(), labelLength);
                    out.writeByte (0);

                    if ((out.getDataSize() & 1) != 0)
                        out.writeByte (0);
                }
            }
        }
    }

    //==============================================================================
    namespace COMTChunk
    {
        static void create (MemoryBlock& block, const StringPairArray& values)
        {
            const int numNotes = values.getValue ("NumCueNotes", "0").getIntValue();

            if (numNotes > 0)
            {
                MemoryOutputStream out (block, false);
                out.writeShortBigEndian ((short) numNotes);

                for (int i = 0; i < numNotes; ++i)
                {
                    const String prefix ("CueNote" + String (i));

                    out.writeIntBigEndian (values.getValue (prefix + "TimeStamp", "0").getIntValue());
                    out.writeShortBigEndian ((short) values.getValue (prefix + "Identifier", "0").getIntValue());

                    const String comment (values.getValue (prefix + "Text", String()));

                    const size_t commentLength = jmin (comment.getNumBytesAsUTF8(), (size_t) 65534);
                    out.writeShortBigEndian ((short) commentLength + 1);
                    out.write (comment.toUTF8(), commentLength);
                    out.writeByte (0);

                    if ((out.getDataSize() & 1) != 0)
                        out.writeByte (0);
                }
            }
        }
    }
}

//==============================================================================
class AiffAudioFormatReader  : public AudioFormatReader
{
public:
    AiffAudioFormatReader (InputStream* in)
        : AudioFormatReader (in, aiffFormatName)
    {
        using namespace AiffFileHelpers;

        if (input->readInt() == chunkName ("FORM"))
        {
            const int len = input->readIntBigEndian();
            const int64 end = input->getPosition() + len;

            const int nextType = input->readInt();
            if (nextType == chunkName ("AIFF") || nextType == chunkName ("AIFC"))
            {
                bool hasGotVer = false;
                bool hasGotData = false;
                bool hasGotType = false;

                while (input->getPosition() < end)
                {
                    const int type = input->readInt();
                    const uint32 length = (uint32) input->readIntBigEndian();
                    const int64 chunkEnd = input->getPosition() + length;

                    if (type == chunkName ("FVER"))
                    {
                        hasGotVer = true;

                        const int ver = input->readIntBigEndian();
                        if (ver != 0 && ver != (int) 0xa2805140)
                            break;
                    }
                    else if (type == chunkName ("COMM"))
                    {
                        hasGotType = true;

                        numChannels = (unsigned int) input->readShortBigEndian();
                        lengthInSamples = input->readIntBigEndian();
                        bitsPerSample = (unsigned int) input->readShortBigEndian();
                        bytesPerFrame = (int) ((numChannels * bitsPerSample) >> 3);

                        unsigned char sampleRateBytes[10];
                        input->read (sampleRateBytes, 10);
                        const int byte0 = sampleRateBytes[0];

                        if ((byte0 & 0x80) != 0
                             || byte0 <= 0x3F || byte0 > 0x40
                             || (byte0 == 0x40 && sampleRateBytes[1] > 0x1C))
                            break;

                        unsigned int sampRate = ByteOrder::bigEndianInt (sampleRateBytes + 2);
                        sampRate >>= (16414 - ByteOrder::bigEndianShort (sampleRateBytes));
                        sampleRate = (int) sampRate;

                        if (length <= 18)
                        {
                            // some types don't have a chunk large enough to include a compression
                            // type, so assume it's just big-endian pcm
                            littleEndian = false;
                        }
                        else
                        {
                            const int compType = input->readInt();

                            if (compType == chunkName ("NONE") || compType == chunkName ("twos"))
                            {
                                littleEndian = false;
                            }
                            else if (compType == chunkName ("sowt"))
                            {
                                littleEndian = true;
                            }
                            else if (compType == chunkName ("fl32") || compType == chunkName ("FL32"))
                            {
                                littleEndian = false;
                                usesFloatingPointData = true;
                            }
                            else
                            {
                                sampleRate = 0;
                                break;
                            }
                        }
                    }
                    else if (type == chunkName ("SSND"))
                    {
                        hasGotData = true;

                        const int offset = input->readIntBigEndian();
                        dataChunkStart = input->getPosition() + 4 + offset;
                        lengthInSamples = (bytesPerFrame > 0) ? jmin (lengthInSamples, ((int64) length) / (int64) bytesPerFrame) : 0;
                    }
                    else if (type == chunkName ("MARK"))
                    {
                        const uint16 numCues = (uint16) input->readShortBigEndian();

                        // these two are always the same for AIFF-read files
                        metadataValues.set ("NumCuePoints", String (numCues));
                        metadataValues.set ("NumCueLabels", String (numCues));

                        for (uint16 i = 0; i < numCues; ++i)
                        {
                            uint16 identifier = (uint16) input->readShortBigEndian();
                            uint32 offset = (uint32) input->readIntBigEndian();
                            uint8 stringLength = (uint8) input->readByte();
                            MemoryBlock textBlock;
                            input->readIntoMemoryBlock (textBlock, stringLength);

                            // if the stringLength is even then read one more byte as the
                            // string needs to be an even number of bytes INCLUDING the
                            // leading length character in the pascal string
                            if ((stringLength & 1) == 0)
                                input->readByte();

                            const String prefixCue ("Cue" + String (i));
                            metadataValues.set (prefixCue + "Identifier", String (identifier));
                            metadataValues.set (prefixCue + "Offset", String (offset));

                            const String prefixLabel ("CueLabel" + String (i));
                            metadataValues.set (prefixLabel + "Identifier", String (identifier));
                            metadataValues.set (prefixLabel + "Text", textBlock.toString());
                        }
                    }
                    else if (type == chunkName ("COMT"))
                    {
                        const uint16 numNotes = (uint16) input->readShortBigEndian();
                        metadataValues.set ("NumCueNotes", String (numNotes));

                        for (uint16 i = 0; i < numNotes; ++i)
                        {
                            uint32 timestamp = (uint32) input->readIntBigEndian();
                            uint16 identifier = (uint16) input->readShortBigEndian(); // may be zero in this case
                            uint16 stringLength = (uint16) input->readShortBigEndian();

                            MemoryBlock textBlock;
                            input->readIntoMemoryBlock (textBlock, stringLength + (stringLength & 1));

                            const String prefix ("CueNote" + String (i));
                            metadataValues.set (prefix + "TimeStamp", String (timestamp));
                            metadataValues.set (prefix + "Identifier", String (identifier));
                            metadataValues.set (prefix + "Text", textBlock.toString());
                        }
                    }
                    else if (type == chunkName ("INST"))
                    {
                        HeapBlock<InstChunk> inst;
                        inst.calloc (jmax ((size_t) length + 1, sizeof (InstChunk)), 1);
                        input->read (inst, (int) length);
                        inst->copyTo (metadataValues);
                    }
                    else if (type == chunkName ("basc"))
                    {
                        AiffFileHelpers::BASCChunk (*input).addToMetadata (metadataValues);
                    }
                    else if (type == chunkName ("cate"))
                    {
                        metadataValues.set (AiffAudioFormat::appleTag,
                                            AiffFileHelpers::CATEChunk::read (*input, length));
                    }
                    else if ((hasGotVer && hasGotData && hasGotType)
                              || chunkEnd < input->getPosition()
                              || input->isExhausted())
                    {
                        break;
                    }

                    input->setPosition (chunkEnd + (chunkEnd & 1)); // (chunks should be aligned to an even byte address)
                }
            }
        }

        if (metadataValues.size() > 0)
            metadataValues.set ("MetaDataSource", "AIFF");
    }

    //==============================================================================
    bool readSamples (int** destSamples, int numDestChannels, int startOffsetInDestBuffer,
                      int64 startSampleInFile, int numSamples) override
    {
        clearSamplesBeyondAvailableLength (destSamples, numDestChannels, startOffsetInDestBuffer,
                                           startSampleInFile, numSamples, lengthInSamples);

        if (numSamples <= 0)
            return true;

        input->setPosition (dataChunkStart + startSampleInFile * bytesPerFrame);

        while (numSamples > 0)
        {
            const int tempBufSize = 480 * 3 * 4; // (keep this a multiple of 3)
            char tempBuffer [tempBufSize];

            const int numThisTime = jmin (tempBufSize / bytesPerFrame, numSamples);
            const int bytesRead = input->read (tempBuffer, numThisTime * bytesPerFrame);

            if (bytesRead < numThisTime * bytesPerFrame)
            {
                jassert (bytesRead >= 0);
                zeromem (tempBuffer + bytesRead, (size_t) (numThisTime * bytesPerFrame - bytesRead));
            }

            if (littleEndian)
                copySampleData<AudioData::LittleEndian> (bitsPerSample, usesFloatingPointData,
                                                         destSamples, startOffsetInDestBuffer, numDestChannels,
                                                         tempBuffer, (int) numChannels, numThisTime);
            else
                copySampleData<AudioData::BigEndian> (bitsPerSample, usesFloatingPointData,
                                                      destSamples, startOffsetInDestBuffer, numDestChannels,
                                                      tempBuffer, (int) numChannels, numThisTime);

            startOffsetInDestBuffer += numThisTime;
            numSamples -= numThisTime;
        }

        return true;
    }

    template <typename Endianness>
    static void copySampleData (unsigned int bitsPerSample, const bool usesFloatingPointData,
                                int* const* destSamples, int startOffsetInDestBuffer, int numDestChannels,
                                const void* sourceData, int numChannels, int numSamples) noexcept
    {
        switch (bitsPerSample)
        {
            case 8:     ReadHelper<AudioData::Int32, AudioData::Int8,  Endianness>::read (destSamples, startOffsetInDestBuffer, numDestChannels, sourceData, numChannels, numSamples); break;
            case 16:    ReadHelper<AudioData::Int32, AudioData::Int16, Endianness>::read (destSamples, startOffsetInDestBuffer, numDestChannels, sourceData, numChannels, numSamples); break;
            case 24:    ReadHelper<AudioData::Int32, AudioData::Int24, Endianness>::read (destSamples, startOffsetInDestBuffer, numDestChannels, sourceData, numChannels, numSamples); break;
            case 32:    if (usesFloatingPointData) ReadHelper<AudioData::Float32, AudioData::Float32, Endianness>::read (destSamples, startOffsetInDestBuffer, numDestChannels, sourceData, numChannels, numSamples);
                        else                       ReadHelper<AudioData::Int32,   AudioData::Int32,   Endianness>::read (destSamples, startOffsetInDestBuffer, numDestChannels, sourceData, numChannels, numSamples); break;
            default:    jassertfalse; break;
        }
    }

    int bytesPerFrame;
    int64 dataChunkStart;
    bool littleEndian;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AiffAudioFormatReader)
};

//==============================================================================
class AiffAudioFormatWriter  : public AudioFormatWriter
{
public:
    AiffAudioFormatWriter (OutputStream* out, double rate,
                           unsigned int numChans, unsigned int bits,
                           const StringPairArray& metadataValues)
        : AudioFormatWriter (out, aiffFormatName, rate, numChans, bits),
          lengthInSamples (0),
          bytesWritten (0),
          writeFailed (false)
    {
        using namespace AiffFileHelpers;

        if (metadataValues.size() > 0)
        {
            // The meta data should have been sanitised for the AIFF format.
            // If it was originally sourced from a WAV file the MetaDataSource
            // key should be removed (or set to "AIFF") once this has been done
            jassert (metadataValues.getValue ("MetaDataSource", "None") != "WAV");

            MarkChunk::create (markChunk, metadataValues);
            COMTChunk::create (comtChunk, metadataValues);
            InstChunk::create (instChunk, metadataValues);
        }

        headerPosition = out->getPosition();
        writeHeader();
    }

    ~AiffAudioFormatWriter()
    {
        if ((bytesWritten & 1) != 0)
            output->writeByte (0);

        writeHeader();
    }

    //==============================================================================
    bool write (const int** data, int numSamples) override
    {
        jassert (numSamples >= 0);
        jassert (data != nullptr && *data != nullptr); // the input must contain at least one channel!

        if (writeFailed)
            return false;

        const size_t bytes = numChannels * (size_t) numSamples * bitsPerSample / 8;
        tempBlock.ensureSize (bytes, false);

        switch (bitsPerSample)
        {
            case 8:     WriteHelper<AudioData::Int8,  AudioData::Int32, AudioData::BigEndian>::write (tempBlock.getData(), (int) numChannels, data, numSamples); break;
            case 16:    WriteHelper<AudioData::Int16, AudioData::Int32, AudioData::BigEndian>::write (tempBlock.getData(), (int) numChannels, data, numSamples); break;
            case 24:    WriteHelper<AudioData::Int24, AudioData::Int32, AudioData::BigEndian>::write (tempBlock.getData(), (int) numChannels, data, numSamples); break;
            case 32:    WriteHelper<AudioData::Int32, AudioData::Int32, AudioData::BigEndian>::write (tempBlock.getData(), (int) numChannels, data, numSamples); break;
            default:    jassertfalse; break;
        }

        if (bytesWritten + bytes >= (size_t) 0xfff00000
             || ! output->write (tempBlock.getData(), bytes))
        {
            // failed to write to disk, so let's try writing the header.
            // If it's just run out of disk space, then if it does manage
            // to write the header, we'll still have a useable file..
            writeHeader();
            writeFailed = true;
            return false;
        }

        bytesWritten += bytes;
        lengthInSamples += (uint64) numSamples;
        return true;
    }

private:
    MemoryBlock tempBlock, markChunk, comtChunk, instChunk;
    uint64 lengthInSamples, bytesWritten;
    int64 headerPosition;
    bool writeFailed;

    void writeHeader()
    {
        using namespace AiffFileHelpers;

        const bool couldSeekOk = output->setPosition (headerPosition);
        ignoreUnused (couldSeekOk);

        // if this fails, you've given it an output stream that can't seek! It needs
        // to be able to seek back to write the header
        jassert (couldSeekOk);

        const int headerLen = (int) (54 + (markChunk.getSize() > 0 ? markChunk.getSize() + 8 : 0)
                                        + (comtChunk.getSize() > 0 ? comtChunk.getSize() + 8 : 0)
                                        + (instChunk.getSize() > 0 ? instChunk.getSize() + 8 : 0));
        int audioBytes = (int) (lengthInSamples * ((bitsPerSample * numChannels) / 8));
        audioBytes += (audioBytes & 1);

        output->writeInt (chunkName ("FORM"));
        output->writeIntBigEndian (headerLen + audioBytes - 8);
        output->writeInt (chunkName ("AIFF"));
        output->writeInt (chunkName ("COMM"));
        output->writeIntBigEndian (18);
        output->writeShortBigEndian ((short) numChannels);
        output->writeIntBigEndian ((int) lengthInSamples);
        output->writeShortBigEndian ((short) bitsPerSample);

        uint8 sampleRateBytes[10] = { 0 };

        if (sampleRate <= 1)
        {
            sampleRateBytes[0] = 0x3f;
            sampleRateBytes[1] = 0xff;
            sampleRateBytes[2] = 0x80;
        }
        else
        {
            int mask = 0x40000000;
            sampleRateBytes[0] = 0x40;

            if (sampleRate >= mask)
            {
                jassertfalse;
                sampleRateBytes[1] = 0x1d;
            }
            else
            {
                int n = (int) sampleRate;

                int i;
                for (i = 0; i <= 32 ; ++i)
                {
                    if ((n & mask) != 0)
                        break;

                    mask >>= 1;
                }

                n = n << (i + 1);

                sampleRateBytes[1] = (uint8) (29 - i);
                sampleRateBytes[2] = (uint8) ((n >> 24) & 0xff);
                sampleRateBytes[3] = (uint8) ((n >> 16) & 0xff);
                sampleRateBytes[4] = (uint8) ((n >>  8) & 0xff);
                sampleRateBytes[5] = (uint8) (n & 0xff);
            }
        }

        output->write (sampleRateBytes, 10);

        if (markChunk.getSize() > 0)
        {
            output->writeInt (chunkName ("MARK"));
            output->writeIntBigEndian ((int) markChunk.getSize());
            *output << markChunk;
        }

        if (comtChunk.getSize() > 0)
        {
            output->writeInt (chunkName ("COMT"));
            output->writeIntBigEndian ((int) comtChunk.getSize());
            *output << comtChunk;
        }

        if (instChunk.getSize() > 0)
        {
            output->writeInt (chunkName ("INST"));
            output->writeIntBigEndian ((int) instChunk.getSize());
            *output << instChunk;
        }

        output->writeInt (chunkName ("SSND"));
        output->writeIntBigEndian (audioBytes + 8);
        output->writeInt (0);
        output->writeInt (0);

        jassert (output->getPosition() == headerLen);
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AiffAudioFormatWriter)
};

//==============================================================================
class MemoryMappedAiffReader   : public MemoryMappedAudioFormatReader
{
public:
    MemoryMappedAiffReader (const File& f, const AiffAudioFormatReader& reader)
        : MemoryMappedAudioFormatReader (f, reader, reader.dataChunkStart,
                                         reader.bytesPerFrame * reader.lengthInSamples, reader.bytesPerFrame),
          littleEndian (reader.littleEndian)
    {
    }

    bool readSamples (int** destSamples, int numDestChannels, int startOffsetInDestBuffer,
                      int64 startSampleInFile, int numSamples) override
    {
        clearSamplesBeyondAvailableLength (destSamples, numDestChannels, startOffsetInDestBuffer,
                                           startSampleInFile, numSamples, lengthInSamples);

        if (map == nullptr || ! mappedSection.contains (Range<int64> (startSampleInFile, startSampleInFile + numSamples)))
        {
            jassertfalse; // you must make sure that the window contains all the samples you're going to attempt to read.
            return false;
        }

        if (littleEndian)
            AiffAudioFormatReader::copySampleData<AudioData::LittleEndian>
                    (bitsPerSample, usesFloatingPointData, destSamples, startOffsetInDestBuffer,
                     numDestChannels, sampleToPointer (startSampleInFile), (int) numChannels, numSamples);
        else
            AiffAudioFormatReader::copySampleData<AudioData::BigEndian>
                    (bitsPerSample, usesFloatingPointData, destSamples, startOffsetInDestBuffer,
                     numDestChannels, sampleToPointer (startSampleInFile), (int) numChannels, numSamples);

        return true;
    }

    void getSample (int64 sample, float* result) const noexcept override
    {
        const int num = (int) numChannels;

        if (map == nullptr || ! mappedSection.contains (sample))
        {
            jassertfalse; // you must make sure that the window contains all the samples you're going to attempt to read.

            zeromem (result, sizeof (float) * (size_t) num);
            return;
        }

        float** dest = &result;
        const void* source = sampleToPointer (sample);

        if (littleEndian)
        {
            switch (bitsPerSample)
            {
                case 8:     ReadHelper<AudioData::Float32, AudioData::UInt8, AudioData::LittleEndian>::read (dest, 0, 1, source, 1, num); break;
                case 16:    ReadHelper<AudioData::Float32, AudioData::Int16, AudioData::LittleEndian>::read (dest, 0, 1, source, 1, num); break;
                case 24:    ReadHelper<AudioData::Float32, AudioData::Int24, AudioData::LittleEndian>::read (dest, 0, 1, source, 1, num); break;
                case 32:    if (usesFloatingPointData) ReadHelper<AudioData::Float32, AudioData::Float32, AudioData::LittleEndian>::read (dest, 0, 1, source, 1, num);
                            else                       ReadHelper<AudioData::Float32, AudioData::Int32,   AudioData::LittleEndian>::read (dest, 0, 1, source, 1, num); break;
                default:    jassertfalse; break;
            }
        }
        else
        {
            switch (bitsPerSample)
            {
                case 8:     ReadHelper<AudioData::Float32, AudioData::UInt8, AudioData::BigEndian>::read (dest, 0, 1, source, 1, num); break;
                case 16:    ReadHelper<AudioData::Float32, AudioData::Int16, AudioData::BigEndian>::read (dest, 0, 1, source, 1, num); break;
                case 24:    ReadHelper<AudioData::Float32, AudioData::Int24, AudioData::BigEndian>::read (dest, 0, 1, source, 1, num); break;
                case 32:    if (usesFloatingPointData) ReadHelper<AudioData::Float32, AudioData::Float32, AudioData::BigEndian>::read (dest, 0, 1, source, 1, num);
                            else                       ReadHelper<AudioData::Float32, AudioData::Int32,   AudioData::BigEndian>::read (dest, 0, 1, source, 1, num); break;
                default:    jassertfalse; break;
            }
        }
    }

    void readMaxLevels (int64 startSampleInFile, int64 numSamples, Range<float>* results, int numChannelsToRead) override
    {
        numSamples = jmin (numSamples, lengthInSamples - startSampleInFile);

        if (map == nullptr || numSamples <= 0 || ! mappedSection.contains (Range<int64> (startSampleInFile, startSampleInFile + numSamples)))
        {
            jassert (numSamples <= 0); // you must make sure that the window contains all the samples you're going to attempt to read.

            for (int i = 0; i < numChannelsToRead; ++i)
                results[i] = Range<float>();

            return;
        }

        switch (bitsPerSample)
        {
            case 8:     scanMinAndMax<AudioData::UInt8> (startSampleInFile, numSamples, results, numChannelsToRead); break;
            case 16:    scanMinAndMax<AudioData::Int16> (startSampleInFile, numSamples, results, numChannelsToRead); break;
            case 24:    scanMinAndMax<AudioData::Int24> (startSampleInFile, numSamples, results, numChannelsToRead); break;
            case 32:    if (usesFloatingPointData) scanMinAndMax<AudioData::Float32> (startSampleInFile, numSamples, results, numChannelsToRead);
                        else                       scanMinAndMax<AudioData::Int32>   (startSampleInFile, numSamples, results, numChannelsToRead); break;
            default:    jassertfalse; break;
        }
    }

private:
    const bool littleEndian;

    template <typename SampleType>
    void scanMinAndMax (int64 startSampleInFile, int64 numSamples, Range<float>* results, int numChannelsToRead) const noexcept
    {
        for (int i = 0; i < numChannelsToRead; ++i)
            results[i] = scanMinAndMaxForChannel<SampleType> (i, startSampleInFile, numSamples);
    }

    template <typename SampleType>
    Range<float> scanMinAndMaxForChannel (int channel, int64 startSampleInFile, int64 numSamples) const noexcept
    {
        return littleEndian ? scanMinAndMaxInterleaved<SampleType, AudioData::LittleEndian> (channel, startSampleInFile, numSamples)
                            : scanMinAndMaxInterleaved<SampleType, AudioData::BigEndian>    (channel, startSampleInFile, numSamples);
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MemoryMappedAiffReader)
};

//==============================================================================
AiffAudioFormat::AiffAudioFormat()   : AudioFormat (aiffFormatName, ".aiff .aif")
{
}

AiffAudioFormat::~AiffAudioFormat()
{
}

Array<int> AiffAudioFormat::getPossibleSampleRates()
{
    const int rates[] = { 22050, 32000, 44100, 48000, 88200, 96000, 176400, 192000, 0 };
    return Array<int> (rates);
}

Array<int> AiffAudioFormat::getPossibleBitDepths()
{
    const int depths[] = { 8, 16, 24, 0 };
    return Array<int> (depths);
}

bool AiffAudioFormat::canDoStereo() { return true; }
bool AiffAudioFormat::canDoMono()   { return true; }

#if JUCE_MAC
bool AiffAudioFormat::canHandleFile (const File& f)
{
    if (AudioFormat::canHandleFile (f))
        return true;

    const OSType type = f.getMacOSType();

    // (NB: written as hex to avoid four-char-constant warnings)
    return type == 0x41494646 /* AIFF */ || type == 0x41494643 /* AIFC */
        || type == 0x61696666 /* aiff */ || type == 0x61696663 /* aifc */;
}
#endif

AudioFormatReader* AiffAudioFormat::createReaderFor (InputStream* sourceStream, const bool deleteStreamIfOpeningFails)
{
    ScopedPointer<AiffAudioFormatReader> w (new AiffAudioFormatReader (sourceStream));

    if (w->sampleRate > 0 && w->numChannels > 0)
        return w.release();

    if (! deleteStreamIfOpeningFails)
        w->input = nullptr;

    return nullptr;
}

MemoryMappedAudioFormatReader* AiffAudioFormat::createMemoryMappedReader (const File& file)
{
    return createMemoryMappedReader (file.createInputStream());
}

MemoryMappedAudioFormatReader* AiffAudioFormat::createMemoryMappedReader (FileInputStream* fin)
{
    if (fin != nullptr)
    {
        AiffAudioFormatReader reader (fin);

        if (reader.lengthInSamples > 0)
            return new MemoryMappedAiffReader (fin->getFile(), reader);
    }

    return nullptr;
}

AudioFormatWriter* AiffAudioFormat::createWriterFor (OutputStream* out,
                                                     double sampleRate,
                                                     unsigned int numChannels,
                                                     int bitsPerSample,
                                                     const StringPairArray& metadataValues,
                                                     int /*qualityOptionIndex*/)
{
    if (out != nullptr
        && numChannels > 0
        && getPossibleBitDepths().contains (bitsPerSample)
        && getPossibleSampleRates().contains ((int) sampleRate))
    {
        return new AiffAudioFormatWriter (out, sampleRate, numChannels, (unsigned int) bitsPerSample, metadataValues);
    }

    return nullptr;
}

//==============================================================================
#if JUCE_UNIT_TESTS

namespace AiffUnitTestData
{
    static const uint8 cello_aiff[] =
    {
        0,79,82,77,0,0,178,170,65,73,70,70,67,79,77,77,0,0,0,18,0,1,0,0,89,62,0,16,64,13,172,68,0,0,0,0,0,0,83,83,78,68,0,0,178,132,0,0,0,0,0,0,0,0,0,0,0,0,255,250,255,248,0,0,255,253,255,255,0,0,255,253,255,249,255,249,255,248,255,252,255,249,0,0,0,4,0,1,255,
        255,0,4,0,7,255,255,0,4,255,249,255,252,0,1,255,255,0,0,255,255,255,255,0,3,0,3,0,1,0,4,255,252,0,4,255,253,255,255,255,250,255,248,255,241,255,248,0,3,0,6,0,6,0,1,0,7,0,1,255,253,255,253,255,246,255,245,255,242,255,246,255,252,255,252,255,249,0,1,255,
        255,0,0,255,255,255,255,255,253,0,3,0,0,255,253,255,250,255,249,255,245,255,246,255,249,255,248,255,255,0,3,0,6,0,0,255,255,255,250,0,4,0,7,0,6,0,0,255,255,255,249,255,252,0,0,0,0,0,6,0,3,255,255,0,0,255,250,0,0,0,3,0,4,0,3,0,1,0,4,0,4,0,7,0,3,255,255,
        255,255,255,246,255,248,255,253,0,3,255,253,0,1,255,252,0,4,255,253,0,6,0,3,255,253,255,253,255,252,255,255,255,253,255,253,0,0,255,255,0,0,0,0,0,3,0,7,0,4,0,1,255,255,255,242,255,250,255,255,0,8,0,0,0,3,0,0,0,3,0,0,255,249,255,248,255,250,0,0,255,252,
        0,0,255,253,255,250,255,248,255,253,0,0,255,252,0,0,0,1,255,252,0,0,255,250,255,253,0,0,0,4,0,0,255,250,255,255,255,253,255,252,0,3,255,253,255,248,0,0,255,252,0,0,255,249,255,244,255,249,255,252,255,255,255,249,255,253,255,246,255,250,255,248,0,1,0,
        6,0,3,0,0,255,253,255,246,255,250,255,255,255,246,255,249,0,1,255,255,255,253,0,1,0,3,0,6,0,1,255,250,255,255,255,252,0,1,255,253,0,0,255,255,255,253,0,0,255,253,255,250,255,250,255,253,255,248,255,255,255,250,255,248,0,0,255,253,255,252,0,6,0,3,0,3,
        255,250,255,255,255,255,255,250,255,249,255,253,0,1,0,4,0,6,0,4,255,255,255,253,255,253,255,255,255,255,255,250,0,1,255,253,255,250,255,255,255,249,255,252,255,255,255,255,0,6,0,6,0,8,0,4,255,255,0,1,0,1,0,3,255,255,0,4,0,4,0,1,255,250,0,3,255,253,255,
        253,0,4,255,250,255,255,255,255,255,255,255,249,255,248,255,255,0,3,255,248,0,3,255,248,0,0,0,4,0,4,255,253,255,252,255,252,255,246,255,248,255,250,255,250,255,253,0,6,0,3,0,6,0,4,0,6,0,7,0,12,0,6,0,10,0,15,0,14,0,6,0,6,255,255,255,248,255,252,0,1,0,
        3,0,8,0,3,0,3,0,8,0,0,0,1,0,10,0,10,0,7,0,6,0,6,0,10,0,6,0,4,255,252,0,4,0,11,0,18,0,14,0,10,0,15,0,11,0,10,0,14,0,15,0,17,0,18,0,17,0,17,0,17,0,11,0,12,0,11,0,17,0,12,0,14,0,7,0,10,0,12,0,11,0,4,0,3,255,253,255,255,0,1,0,0,255,250,255,253,0,6,0,4,0,
        7,0,7,0,4,0,1,0,3,0,1,0,3,255,252,255,246,255,248,255,249,255,252,0,0,0,0,255,253,255,255,255,250,255,246,255,249,255,253,0,8,0,6,0,25,0,19,0,19,0,18,0,18,0,24,0,24,0,25,0,26,0,30,0,26,0,22,0,28,0,15,0,15,0,8,0,11,0,10,0,7,0,25,0,24,0,32,0,44,0,43,0,
        57,0,51,0,53,0,48,0,46,0,40,0,35,0,29,0,24,0,21,0,8,0,0,255,250,255,239,255,237,255,228,255,227,255,223,255,228,255,227,255,228,255,228,255,239,255,239,255,242,255,244,255,245,255,245,255,237,255,223,255,214,255,202,255,191,255,174,255,162,255,155,255,
        138,255,138,255,130,255,127,255,120,255,111,255,111,255,108,255,105,255,102,255,98,255,99,255,99,255,97,255,99,255,104,255,113,255,127,255,145,255,151,255,165,255,169,255,180,255,192,255,187,255,180,255,166,255,162,255,156,255,154,255,148,255,149,255,
        156,255,172,255,187,255,196,255,188,255,194,255,188,255,191,255,181,255,181,255,180,255,173,255,170,255,167,255,156,255,141,255,112,255,87,255,57,255,26,255,0,254,231,254,216,254,214,254,211,254,220,254,229,254,239,254,254,255,29,255,69,255,104,255,133,
        255,142,255,166,255,195,255,210,255,232,255,250,0,4,255,253,255,235,255,209,255,180,255,165,255,162,255,165,255,170,255,185,255,191,255,184,255,183,255,173,255,154,255,127,255,104,255,99,255,112,255,133,255,156,255,170,255,176,255,173,255,136,255,115,
        255,79,255,52,255,37,255,37,255,50,255,62,255,65,255,65,255,72,255,51,255,11,254,218,254,185,254,171,254,196,255,3,255,79,255,167,0,3,0,89,0,169,0,233,1,33,1,78,1,132,1,191,1,243,2,28,2,52,2,48,2,20,1,222,1,141,1,35,0,172,0,54,255,221,255,158,255,130,
        255,142,255,185,255,238,0,44,0,101,0,147,0,150,0,127,0,76,0,7,255,195,255,98,254,254,254,175,254,131,254,105,254,98,254,108,254,119,254,135,254,152,254,163,254,178,254,195,254,221,254,254,255,18,255,15,254,254,254,209,254,141,254,72,254,23,253,244,254,
        6,254,38,254,110,254,213,255,58,255,142,255,220,0,28,0,83,0,134,0,180,0,220,1,22,1,90,1,161,1,236,2,54,2,126,2,194,2,250,3,42,3,84,3,107,3,132,3,143,3,143,3,139,3,127,3,91,3,70,3,41,2,244,2,187,2,147,2,104,2,84,2,79,2,77,2,78,2,85,2,93,2,84,2,54,2,10,
        1,202,1,144,1,81,1,14,0,194,0,126,0,72,0,55,0,60,0,105,0,157,0,216,1,14,1,54,1,96,1,117,1,136,1,155,1,154,1,164,1,175,1,180,1,194,1,222,2,3,2,49,2,125,2,211,3,39,3,103,3,159,3,186,3,172,3,128,3,75,3,19,2,200,2,125,2,50,1,248,1,179,1,117,1,54,0,248,0,
        183,0,118,0,61,0,24,0,0,255,255,0,7,0,43,0,90,0,148,0,213,1,28,1,108,1,193,2,10,2,75,2,147,2,211,3,15,3,67,3,95,3,117,3,141,3,142,3,131,3,109,3,85,3,55,3,15,2,226,2,196,2,142,2,85,2,14,1,198,1,117,1,39,0,205,0,101,255,244,255,122,254,242,254,112,253,
        228,253,109,253,1,252,172,252,114,252,89,252,82,252,92,252,128,252,196,253,10,253,91,253,168,253,228,254,19,254,52,254,63,254,52,254,30,253,242,253,206,253,166,253,128,253,100,253,96,253,102,253,123,253,154,253,193,253,242,254,37,254,85,254,124,254,163,
        254,177,254,167,254,153,254,139,254,116,254,80,254,11,253,171,253,96,253,42,253,9,252,237,252,219,252,240,253,24,253,77,253,177,254,49,254,175,255,39,255,140,255,209,0,11,0,65,0,115,0,125,0,96,0,73,0,51,0,1,255,205,255,176,255,191,255,228,255,249,255,
        252,255,255,255,245,255,232,255,209,255,174,255,147,255,116,255,61,254,227,254,124,254,1,253,142,253,74,253,59,253,27,252,240,252,186,252,142,252,96,252,68,252,24,251,245,251,229,251,241,252,21,252,84,252,151,252,193,252,200,252,161,252,77,251,219,251,
        94,250,244,250,163,250,143,250,177,251,4,251,97,251,207,252,79,252,185,252,248,253,21,253,75,253,207,254,146,255,131,0,127,1,71,1,225,2,90,2,181,2,239,3,13,3,21,3,44,3,41,3,37,2,251,2,207,2,149,2,79,1,233,1,56,0,86,255,115,254,191,254,65,253,239,253,
        203,253,204,253,224,254,36,254,128,254,239,255,80,255,151,255,166,255,131,255,54,254,167,253,218,252,243,252,27,251,132,251,58,251,40,251,56,251,96,251,125,251,129,251,74,250,234,250,130,250,57,250,21,250,14,250,49,250,94,250,151,250,231,251,86,251,193,
        252,18,252,63,252,86,252,113,252,120,252,96,252,61,252,24,252,23,252,48,252,118,252,228,253,134,254,78,255,40,0,10,0,201,1,93,1,191,2,13,2,84,2,174,3,5,3,82,3,154,3,213,4,26,4,74,4,90,4,108,4,144,4,186,4,254,5,75,5,174,6,10,6,104,6,148,6,162,6,125,6,
        78,6,29,5,252,5,246,6,4,6,21,6,61,6,118,6,162,6,166,6,119,6,36,5,201,5,77,4,211,4,106,3,253,3,128,3,17,2,153,2,50,1,208,1,132,1,97,1,104,1,132,1,184,1,252,2,75,2,189,3,45,3,163,3,251,4,76,4,155,4,228,5,27,5,69,5,91,5,99,5,87,5,80,5,58,5,39,5,19,4,231,
        4,193,4,167,4,137,4,90,4,51,4,24,4,30,4,59,4,106,4,166,4,231,5,45,5,112,5,173,5,239,6,48,6,93,6,109,6,80,6,29,5,221,5,135,5,43,4,220,4,134,4,47,3,208,3,143,3,78,3,24,2,222,2,153,2,90,2,43,1,251,1,216,1,187,1,171,1,178,1,211,2,7,2,43,2,74,2,68,1,255,1,
        162,1,49,0,172,0,43,255,191,255,115,255,86,255,87,255,115,255,180,0,4,0,87,0,140,0,151,0,130,0,66,255,226,255,99,254,217,254,54,253,148,252,244,252,110,251,241,251,151,251,90,251,57,251,65,251,104,251,190,252,35,252,149,253,9,253,99,253,174,253,221,253,
        239,253,235,253,208,253,185,253,152,253,99,253,33,252,225,252,158,252,99,252,49,252,17,252,5,252,2,252,27,252,82,252,176,253,49,253,192,254,78,254,220,255,81,255,169,255,224,255,221,255,184,255,91,254,214,254,62,253,152,253,3,252,139,252,46,251,237,251,
        196,251,182,251,227,252,48,252,121,252,199,253,35,253,174,254,36,254,134,254,235,255,66,255,130,255,167,255,151,255,99,255,19,254,156,254,27,253,181,253,103,253,49,252,235,252,147,252,60,251,244,251,209,251,204,251,190,251,150,251,104,251,60,251,10,250,
        224,250,202,250,184,250,184,250,215,251,65,251,226,252,163,253,132,254,102,255,14,255,83,255,57,254,223,254,94,253,207,253,60,252,160,252,20,251,193,251,147,251,118,251,101,251,99,251,83,251,57,251,14,251,7,251,11,251,64,251,183,252,86,252,241,253,100,
        253,179,254,9,254,113,254,224,255,68,255,163,0,7,0,90,0,165,0,217,1,6,1,7,0,183,0,8,255,22,254,8,253,24,252,107,252,17,252,9,252,35,252,86,252,163,252,253,253,92,253,142,253,100,252,192,251,197,250,130,249,57,247,254,247,26,246,120,246,5,245,200,245,
        180,245,211,245,254,246,24,246,3,245,204,245,168,245,164,245,198,246,25,246,147,247,52,247,197,248,37,248,112,248,185,249,14,249,142,250,69,251,72,252,131,253,215,255,23,0,54,1,33,1,191,2,2,2,25,2,35,2,63,2,117,2,207,3,107,4,73,5,108,6,184,7,251,9,60,
        10,149,11,214,12,240,13,197,14,110,14,239,15,77,15,132,15,139,15,89,14,211,14,13,13,54,12,105,11,201,11,84,11,5,10,232,10,222,10,189,10,144,10,70,9,239,9,96,8,146,7,157,6,150,5,126,4,116,3,114,2,138,1,165,0,202,0,15,255,112,254,236,254,148,254,105,254,
        120,254,211,255,104,0,43,1,20,2,14,3,3,3,221,4,116,4,227,5,69,5,185,6,33,6,155,7,33,7,157,8,2,8,118,8,216,9,35,9,64,9,64,9,49,9,30,9,12,8,255,8,240,8,208,8,150,8,67,7,243,7,155,7,45,6,205,6,107,6,21,5,202,5,153,5,102,5,83,5,84,5,81,5,102,5,108,5,106,
        5,134,5,202,6,42,6,180,7,93,8,32,8,236,9,170,10,103,10,246,11,66,11,73,11,7,10,108,9,149,8,133,7,83,6,37,4,239,3,224,2,211,1,226,1,22,0,129,255,255,255,113,254,198,254,9,253,42,252,57,251,42,250,14,248,236,247,214,246,215,246,12,245,97,245,14,245,6,245,
        68,245,197,246,111,247,12,247,160,248,33,248,148,248,216,248,242,248,235,248,234,248,221,248,241,249,28,249,98,249,176,250,15,250,119,250,240,251,117,251,241,252,93,252,193,253,16,253,81,253,125,253,153,253,167,253,163,253,168,253,167,253,128,253,81,
        253,56,253,89,253,152,253,210,253,218,253,197,253,186,253,215,254,38,254,119,254,196,255,1,255,65,255,137,255,220,0,46,0,134,0,209,1,9,1,54,1,83,1,79,1,49,1,32,1,25,0,249,0,145,0,4,255,91,254,164,254,12,253,159,253,55,252,194,252,103,252,43,252,2,251,
        200,251,118,251,40,250,249,250,198,250,133,250,37,249,187,249,77,248,235,248,149,248,48,247,196,247,130,247,107,247,114,247,124,247,114,247,24,246,125,245,203,245,4,244,53,243,152,243,66,243,26,243,37,243,79,243,108,243,133,243,177,243,233,244,35,244,
        73,244,144,245,53,246,74,247,179,249,102,251,75,253,39,254,227,0,112,1,227,3,45,4,110,5,115,6,39,6,127,6,80,5,209,5,1,3,225,2,63,0,55,253,228,251,154,249,158,248,56,247,121,247,69,247,121,247,243,248,159,249,116,250,55,250,202,251,21,251,33,250,197,249,
        255,249,25,248,49,247,67,246,103,245,129,244,168,243,248,243,109,243,33,243,14,243,46,243,98,243,142,243,191,244,6,244,115,245,20,245,197,246,88,246,187,247,34,247,148,248,19,248,203,249,147,250,98,251,25,251,162,252,42,252,163,253,10,253,105,253,195,
        254,78,255,5,0,4,1,74,2,229,4,149,6,44,7,147,8,186,9,189,10,108,10,214,11,1,11,16,11,52,11,152,11,253,12,144,13,69,13,243,14,180,15,106,16,23,16,182,17,61,17,217,18,154,19,25,19,78,19,47,18,197,18,61,17,167,16,250,16,73,15,179,15,116,15,114,15,161,15,
        217,16,16,16,30,15,226,15,113,14,174,13,151,12,41,10,144,8,241,7,85,5,206,4,110,3,64,2,78,1,172,1,69,1,51,1,86,1,198,2,140,3,145,4,153,5,149,6,100,7,10,7,112,7,130,7,81,6,238,6,115,5,237,5,140,5,95,5,122,5,225,6,123,7,13,7,132,7,254,8,131,9,6,9,142,10,
        22,10,168,11,94,11,246,12,125,12,220,13,22,13,36,12,231,12,98,11,188,10,250,10,51,9,123,8,236,8,139,8,56,7,227,7,140,7,38,6,183,6,89,5,235,5,153,5,79,5,25,4,235,4,196,4,157,4,85,3,254,3,132,2,232,2,74,1,139,0,213,0,7,255,21,254,23,253,67,252,121,251,
        186,251,70,251,25,251,56,251,150,252,31,252,215,253,145,254,58,254,178,254,200,254,113,253,190,252,204,251,168,250,100,249,37,247,232,246,194,245,201,245,2,244,129,244,65,244,55,244,90,244,158,244,233,245,74,245,190,246,53,246,135,246,186,246,194,246,
        205,246,207,246,254,247,78,247,150,247,213,247,243,248,51,248,134,248,252,249,118,249,219,250,75,250,181,251,25,251,125,251,240,252,84,252,171,252,240,253,69,253,175,254,49,254,174,255,44,255,136,255,178,255,169,255,105,255,34,254,239,254,247,255,54,
        255,115,255,141,255,144,255,134,255,140,255,191,0,29,0,173,1,57,1,129,1,118,1,47,1,6,0,213,0,132,0,8,255,77,254,92,253,34,251,182,250,107,249,111,248,157,247,245,247,95,246,247,246,161,246,73,245,216,245,7,243,216,242,55,240,64,238,47,236,112,235,55,
        234,156,234,100,234,111,234,228,235,182,236,195,237,220,238,193,239,131,240,84,241,73,242,119,243,235,245,64,246,70,246,232,247,94,247,202,248,12,248,119,249,39,250,54,251,70,252,77,253,44,253,195,254,2,253,211,253,70,252,85,251,36,249,223,248,221,248,
        119,248,223,249,228,251,49,252,133,253,224,255,7,0,4,0,162,0,217,0,125,255,162,254,106,253,39,252,5,251,32,250,151,250,158,250,229,251,29,251,6,250,169,250,24,249,95,248,133,247,132,246,103,245,56,244,31,243,23,242,19,241,17,240,45,239,133,239,53,239,
        57,239,190,240,172,241,251,243,137,245,49,246,167,247,184,248,106,248,224,249,61,249,145,250,31,251,49,252,194,254,182,0,224,3,39,5,147,7,255,10,53,12,68,14,6,15,136,16,203,17,176,18,77,18,166,19,8,19,159,20,81,21,44,22,73,23,152,24,249,26,88,27,196,
        29,25,30,29,30,245,31,109,31,118,30,249,30,65,29,98,28,92,27,97,26,89,25,72,24,50,23,66,22,116,21,212,21,78,20,225,20,131,19,249,19,22,17,230,16,96,14,168,12,240,11,90,10,4,8,236,7,238,7,36,6,145,6,42,5,230,5,159,5,137,5,173,5,235,6,72,6,198,7,94,8,34,
        8,216,9,128,10,2,10,74,10,80,10,27,9,204,9,111,8,250,8,126,7,243,7,61,6,129,5,253,5,223,6,47,6,201,7,154,8,102,9,49,10,27,11,11,11,219,12,122,12,231,13,26,13,21,12,237,12,202,12,174,12,152,12,112,12,29,11,165,10,234,9,246,8,240,8,3,7,56,6,161,6,47,6,
        6,6,50,6,134,6,223,7,24,7,61,7,68,7,40,6,226,6,90,5,159,4,166,3,124,2,50,0,188,255,34,253,134,252,23,250,227,250,18,249,162,249,158,249,209,250,51,250,137,250,219,250,244,250,186,250,33,248,252,247,80,245,81,243,47,241,40,239,85,237,219,236,241,236,149,
        236,209,237,155,238,193,240,32,241,164,243,40,244,139,245,201,246,181,247,81,247,145,247,163,247,128,247,31,246,109,245,143,244,139,243,126,242,170,242,48,242,11,242,0,241,250,242,54,242,192,243,203,245,16,246,96,247,181,248,242,250,54,251,96,252,67,
        252,190,252,200,252,146,252,34,251,81,250,31,249,26,248,144,248,101,248,117,248,224,249,130,250,51,250,248,252,12,253,114,255,7,0,151,1,254,2,229,3,82,3,71,2,236,2,59,1,28,255,199,254,72,252,128,250,150,248,181,247,88,246,118,245,187,244,247,244,91,244,
        35,243,255,243,208,243,90,242,135,241,110,240,9,238,112,236,173,234,218,233,95,232,159,232,47,231,233,231,249,232,101,233,23,233,172,233,180,233,77,232,220,233,7,233,200,234,251,236,79,237,163,238,198,239,236,240,243,241,225,242,187,243,145,244,130,245,
        165,246,172,247,178,248,105,249,3,249,98,249,108,248,239,248,29,247,84,247,34,247,192,249,16,250,226,253,10,255,21,0,222,2,60,3,45,3,177,3,202,3,105,2,142,1,35,255,76,253,62,251,112,249,250,248,224,248,11,247,172,247,156,247,196,248,18,248,31,247,213,
        247,44,246,61,245,97,244,132,243,195,242,180,241,121,240,98,239,216,239,254,240,226,242,61,243,235,245,155,246,253,248,9,248,229,249,88,249,149,249,185,250,65,251,79,252,212,255,36,2,20,5,93,8,218,12,47,15,20,17,79,19,20,20,101,21,104,22,67,23,51,24,
        62,25,40,25,244,26,160,27,48,27,163,28,24,28,189,29,134,30,90,31,93,32,180,34,3,35,9,35,157,35,171,35,88,35,6,34,168,34,49,33,192,33,118,33,81,33,39,32,254,32,184,32,101,31,204,30,221,29,139,27,232,26,47,24,77,22,124,20,237,19,129,18,48,17,30,16,67,15,
        138,14,216,14,65,13,198,13,105,13,85,13,130,13,226,14,104,14,246,15,85,15,102,15,21,14,118,13,162,12,170,11,192,10,227,9,214,8,214,7,237,7,36,6,105,5,171,4,216,3,247,3,62,2,183,2,129,2,139,2,218,3,77,3,254,4,189,5,160,6,138,7,86,7,219,8,9,8,28,8,32,8,
        41,8,61,8,107,8,198,9,27,9,102,9,124,9,114,9,67,8,248,8,132,8,6,7,117,6,215,6,73,5,220,5,147,5,98,5,98,5,137,5,209,6,1,6,6,5,188,5,41,4,47,2,183,0,220,254,210,252,168,250,144,248,164,247,33,246,12,245,101,245,29,245,53,245,107,245,128,245,136,245,74,
        244,229,244,49,243,76,242,56,241,15,239,226,238,186,237,174,236,228,236,102,236,98,236,188,237,109,238,80,239,72,240,88,241,137,242,207,244,51,245,72,245,251,246,48,245,250,245,139,245,13,244,98,243,119,242,99,241,94,240,98,239,140,238,187,237,248,237,
        82,236,155,235,254,235,204,236,70,237,83,238,132,239,244,241,171,243,140,245,86,246,186,247,181,248,103,249,54,249,196,249,209,249,115,249,15,249,15,249,123,250,65,251,75,252,106,253,102,254,0,254,8,254,1,254,134,255,148,0,120,0,222,0,212,0,97,255,172,
        254,243,254,29,252,248,251,82,249,154,248,13,246,207,245,208,244,249,243,241,242,178,241,60,239,75,236,212,234,1,231,80,228,254,227,3,225,93,223,243,223,72,223,45,223,80,223,72,223,44,223,40,223,130,224,120,226,62,228,100,230,166,232,168,234,57,235,38,
        235,156,235,242,236,115,237,36,238,53,239,149,241,52,242,228,244,211,247,2,249,15,250,108,250,202,250,53,249,61,248,133,248,98,248,239,250,21,251,204,254,51,1,18,4,31,7,46,9,113,10,184,10,163,9,34,6,148,3,85,255,248,252,171,249,216,247,152,245,227,244,
        169,243,212,243,155,243,216,244,7,244,31,244,43,244,86,244,121,244,175,244,177,244,140,244,57,243,213,243,106,243,90,243,221,244,249,246,111,248,48,249,239,251,101,252,99,252,250,253,17,252,246,252,207,252,222,253,172,255,52,1,135,4,116,7,101,10,59,12,
        238,15,80,17,109,19,86,20,232,22,178,24,192,26,153,28,59,29,210,31,53,32,83,32,196,32,182,32,81,31,201,31,177,32,97,33,194,35,151,37,145,39,97,40,229,42,14,42,238,43,115,43,130,43,68,42,252,42,202,42,130,41,250,41,88,40,177,39,228,38,222,37,172,36,101,
        35,23,33,233,32,191,31,111,30,57,29,36,27,247,26,178,25,90,23,235,22,174,21,200,21,114,21,120,21,163,22,9,22,137,22,195,22,148,21,225,20,219,19,119,17,224,16,67,14,169,13,51,12,35,11,129,11,40,10,231,10,127,9,190,8,135,6,235,5,69,3,177,2,23,0,211,255,
        227,255,50,254,217,254,177,254,241,255,130,0,64,1,31,2,17,3,20,4,88,5,176,6,253,8,16,8,218,9,159,10,73,10,186,10,235,10,239,10,234,10,192,10,130,10,33,9,188,9,80,8,237,8,143,7,218,7,46,6,104,5,195,5,48,4,173,4,58,3,146,2,171,1,100,255,201,254,18,252,
        124,251,10,249,167,248,84,247,46,246,59,245,82,244,184,244,15,243,100,242,202,241,240,241,27,240,35,239,49,238,107,237,177,236,239,236,23,235,76,234,106,233,205,233,119,233,83,233,136,234,28,235,15,236,177,238,206,241,74,243,198,245,201,247,31,247,182,
        247,207,247,102,246,127,245,9,243,33,240,243,238,154,236,50,234,98,233,14,231,252,230,206,229,185,229,29,229,44,229,160,230,65,231,23,232,25,233,64,234,164,236,14,237,77,238,134,240,12,241,156,242,148,242,191,242,135,242,105,242,73,242,55,242,112,243,
        26,243,172,243,208,243,166,243,137,243,237,244,205,246,55,247,236,249,154,250,245,252,24,253,1,253,178,253,220,253,91,252,84,250,241,249,91,247,182,245,254,244,32,242,67,240,121,238,134,236,144,234,102,232,119,230,217,229,166,228,171,227,203,227,56,227,
        17,227,36,226,245,226,101,225,194,225,120,225,202,226,209,228,167,231,4,233,124,235,196,237,165,239,35,240,79,241,27,241,125,241,160,241,240,242,109,243,48,244,35,245,75,246,149,247,95,247,163,247,2,245,189,244,101,243,109,242,205,242,146,242,232,243,
        237,245,169,247,195,249,248,252,89,254,77,255,145,255,234,255,169,254,195,253,109,251,169,250,24,249,41,248,182,248,49,247,149,246,154,245,168,244,119,243,82,242,62,241,114,241,38,241,98,242,34,242,242,243,158,244,33,244,166,245,18,245,171,246,187,248,
        79,250,109,252,250,255,136,1,214,3,62,3,202,3,168,2,176,1,31,255,210,255,16,254,245,255,102,0,111,2,82,4,214,7,200,11,48,14,192,18,116,21,254,25,111,28,146,31,90,33,139,35,105,36,183,37,94,37,175,37,162,37,101,37,62,37,79,37,191,38,215,40,124,42,198,
        45,85,48,5,50,187,53,58,55,28,56,101,57,2,56,213,55,255,54,138,52,217,50,230,48,224,47,47,45,192,44,159,43,198,42,220,42,9,41,63,40,154,39,251,39,81,38,107,37,107,36,27,34,127,32,218,31,61,29,227,28,185,27,157,26,144,25,141,24,166,24,48,23,241,23,189,
        23,166,23,130,23,59,22,202,22,53,21,96,20,54,18,165,17,54,15,168,14,23,12,116,10,227,9,127,8,41,6,191,5,61,3,179,2,102,1,132,0,208,0,136,0,140,0,234,1,87,1,232,2,146,3,81,4,40,5,75,6,190,8,42,9,69,9,206,9,237,9,196,9,48,7,241,6,120,5,25,3,254,3,53,2,
        217,3,8,3,147,4,44,4,221,5,106,5,159,5,144,5,76,4,198,3,222,2,203,1,144,0,98,255,50,253,242,252,147,251,29,249,177,248,130,247,125,246,243,246,197,247,5,247,99,247,159,247,190,247,74,246,115,245,13,243,84,241,51,238,206,236,162,234,92,232,64,230,130,
        229,77,228,188,228,168,229,15,229,216,230,217,232,43,233,178,235,38,236,91,237,1,236,245,236,138,236,14,235,132,234,197,233,229,232,223,231,217,231,1,230,80,229,234,229,124,229,23,228,195,228,82,227,182,227,162,228,19,228,163,229,69,229,243,230,145,231,
        93,232,83,233,125,234,199,236,86,237,198,238,150,238,176,238,97,238,10,237,220,238,10,238,143,239,16,239,10,238,90,237,69,236,68,235,210,236,9,236,213,237,219,238,206,239,222,241,99,243,3,244,42,244,170,244,173,244,44,243,10,241,184,240,131,239,89,238,
        11,236,149,234,219,232,212,230,188,228,88,225,227,223,232,222,73,220,223,219,209,219,147,220,61,221,61,221,237,221,241,221,138,221,142,222,23,223,37,224,195,227,28,229,236,232,164,234,242,237,19,239,21,240,253,242,145,243,201,244,143,245,74,246,49,247,
        71,248,127,249,142,249,238,249,21,247,33,244,97,241,149,239,71,237,237,237,141,238,49,239,125,241,166,244,118,248,23,251,247,255,130,1,237,2,248,2,189,1,161,255,212,253,202,251,251,250,245,250,118,250,58,249,138,248,109,246,247,245,76,243,163,242,69,
        241,134,241,130,242,77,243,113,244,139,245,201,246,221,247,148,247,246,248,8,248,123,249,54,250,108,252,10,253,220,255,94,0,129,1,27,0,147,255,50,253,56,251,110,250,79,250,89,251,179,254,74,1,158,5,187,10,135,15,102,20,0,23,245,27,142,30,219,33,117,35,
        70,36,147,37,132,38,90,39,41,39,236,40,123,41,8,41,213,43,12,44,164,46,175,49,75,52,3,54,158,57,82,59,235,62,19,63,114,64,23,64,54,63,203,62,166,60,234,58,206,56,147,54,162,52,203,50,241,49,101,47,234,46,116,44,209,43,21,41,66,39,154,38,38,36,231,35,
        218,34,184,33,111,32,24,30,202,29,163,28,153,27,224,27,140,27,95,27,97,27,153,27,246,28,121,28,240,29,20,28,207,28,20,27,27,25,242,24,179,23,77,21,222,20,82,18,191,17,1,15,2,13,28,11,138,10,66,9,32,8,42,7,118,7,36,6,235,6,248,7,22,7,46,7,81,7,114,7,76,
        6,162,5,207,5,9,4,189,4,163,4,220,4,189,4,180,4,126,4,77,3,220,3,163,3,123,3,136,3,197,4,149,5,188,6,224,7,238,8,193,9,66,9,63,8,223,8,43,7,105,6,181,6,7,5,57,4,62,3,39,2,36,0,209,255,47,253,171,252,49,250,233,249,237,249,37,248,166,248,126,248,164,248,
        163,248,91,247,81,245,154,243,65,240,77,237,68,234,14,230,216,228,39,226,21,224,191,224,8,223,243,224,97,225,50,226,133,228,78,230,178,233,60,235,168,237,180,238,205,239,35,238,249,238,86,237,71,235,204,234,13,232,125,231,32,230,90,230,62,230,47,230,
        23,229,189,229,88,228,231,228,139,228,74,228,106,228,160,228,213,228,244,229,5,229,27,229,102,229,210,230,207,232,8,232,229,233,74,233,180,234,17,234,92,234,182,235,44,235,137,235,91,234,172,233,236,232,244,232,4,231,202,232,115,233,68,233,219,234,189,
        236,43,237,228,239,133,240,154,240,237,240,199,240,163,239,216,238,13,235,239,234,20,232,64,230,27,227,175,224,246,222,57,219,174,217,173,216,26,214,168,213,130,212,249,213,76,214,140,216,39,217,134,218,86,219,21,220,28,221,153,223,152,226,28,228,232,
        231,180,234,66,236,140,238,180,240,193,242,151,244,10,245,82,246,109,247,128,248,139,249,209,251,22,251,233,251,136,249,252,247,188,245,89,243,69,241,214,241,41,241,98,242,87,244,58,247,19,250,181,254,138,1,220,4,98,5,217,5,255,4,245,2,233,0,54,253,188,
        251,212,250,180,250,14,249,228,249,202,249,159,249,68,248,159,247,156,246,124,245,166,244,244,244,209,244,218,244,241,244,212,244,133,244,152,244,220,245,52,245,171,246,183,248,94,250,201,253,166,0,58,1,250,2,185,2,179,1,168,255,209,253,206,252,63,251,
        139,252,82,254,173,2,23,6,134,11,169,17,152,23,95,28,117,32,176,36,30,38,147,40,111,41,192,42,94,42,221,43,58,43,163,44,11,44,79,44,252,46,64,48,0,50,68,53,27,56,46,59,96,62,132,65,120,67,247,69,167,70,108,70,95,69,93,67,114,64,238,61,185,58,64,54,228,
        52,2,49,186,47,218,46,145,45,176,44,220,43,231,43,12,42,51,41,110,40,192,40,17,38,230,37,61,35,109,33,244,32,171,31,90,30,127,29,210,29,135,29,144,29,170,29,198,30,4,30,19,29,252,29,196,29,54,28,145,27,172,26,144,25,44,23,140,22,5,20,121,18,105,16,81,
        14,72,12,183,11,74,9,247,8,95,6,154,5,3,3,196,2,194,1,251,1,68,0,209,0,104,0,39,255,230,255,106,254,234,254,216,255,59,255,216,0,143,1,128,2,114,3,67,3,233,4,130,4,186,4,199,4,232,5,66,5,191,6,162,7,108,8,63,8,248,9,178,9,244,9,193,9,69,8,182,7,243,6,
        194,5,142,4,54,2,171,0,238,254,243,252,183,250,151,249,52,248,109,248,4,247,153,247,38,246,193,246,56,245,90,244,3,241,170,238,216,235,238,233,13,230,84,227,255,226,33,224,163,223,207,223,137,223,235,224,159,225,205,227,82,229,11,231,21,233,27,234,244,
        236,112,237,119,237,244,237,223,237,134,236,188,235,171,234,96,233,34,232,79,231,180,231,108,231,41,230,180,230,74,229,164,228,199,227,204,226,220,226,32,225,224,225,210,225,156,225,125,225,209,226,150,227,229,229,1,230,1,231,3,231,217,232,132,233,2,
        233,47,233,83,233,136,233,135,233,39,232,78,231,69,230,61,229,87,229,4,229,58,229,211,230,178,231,251,233,93,234,246,236,159,238,56,239,131,240,55,240,55,239,47,237,59,234,153,231,173,228,96,224,225,221,86,218,55,215,156,213,163,212,43,210,226,209,215,
        209,73,209,24,209,74,209,174,210,5,209,246,209,235,210,56,211,41,212,224,215,113,218,169,222,12,225,118,229,30,232,202,236,87,239,149,242,120,244,230,246,183,248,74,250,4,252,41,254,128,0,157,2,31,2,142,2,7,0,123,254,76,252,16,250,36,249,59,249,98,250,
        159,252,175,255,105,2,106,5,162,8,117,10,178,11,178,11,29,9,51,6,151,3,226,1,135,255,241,255,147,255,120,255,172,255,109,254,178,253,17,250,186,247,245,245,169,243,216,242,95,240,244,239,248,239,96,239,50,239,28,239,96,239,204,240,74,241,103,243,106,
        245,239,248,206,251,175,254,56,0,21,0,187,0,137,255,183,254,128,253,145,253,56,253,179,255,27,1,87,4,204,9,82,14,93,19,187,24,234,29,180,33,244,37,125,39,248,41,149,42,181,43,144,44,114,45,75,46,44,47,85,48,116,49,213,51,147,53,181,56,18,58,118,61,7,
        63,209,66,151,69,8,70,203,71,207,72,87,72,30,70,250,68,187,65,154,62,68,58,200,55,147,52,246,51,10,49,219,49,55,48,180,48,12,47,8,45,212,44,118,43,11,41,135,39,239,38,83,36,216,35,97,34,56,33,86,32,222,32,155,32,146,32,130,32,60,31,244,31,180,31,96,30,
        224,29,241,28,251,27,182,26,128,25,71,24,25,22,99,20,36,17,239,15,217,13,193,11,155,9,196,8,153,7,218,7,129,7,22,6,104,5,191,4,185,3,186,2,117,1,76,255,226,254,200,254,67,253,229,253,210,254,22,254,227,255,190,0,177,1,141,2,21,2,169,3,52,3,145,3,186,
        3,232,4,72,4,245,5,250,7,33,8,132,9,174,10,159,11,91,11,171,11,135,11,5,10,17,8,228,7,141,6,1,4,37,2,21,0,42,254,103,252,179,250,212,248,243,247,175,246,160,245,143,244,161,244,3,243,115,243,28,242,180,241,161,239,214,237,113,234,199,232,89,229,217,227,
        156,225,127,223,202,222,156,222,57,222,85,222,199,223,127,224,195,226,117,228,143,230,195,232,225,234,200,236,58,237,47,237,181,237,183,237,5,235,225,234,135,233,79,232,58,231,149,231,108,231,148,231,202,231,195,231,58,230,112,229,76,228,38,227,81,226,
        159,225,255,225,126,225,86,225,162,226,25,226,231,228,38,229,157,230,134,230,188,230,237,231,55,231,104,231,138,232,0,232,58,231,177,230,87,228,255,228,31,227,212,228,44,228,197,229,125,230,74,231,79,232,141,233,164,234,125,235,40,235,120,235,40,234,
        17,232,122,230,80,227,210,225,55,222,103,219,116,216,127,213,125,210,109,207,169,205,146,203,230,202,186,202,57,202,132,203,7,203,38,202,236,202,204,203,24,203,255,205,188,208,28,211,61,214,243,219,11,223,65,227,47,230,183,234,82,238,40,242,25,245,150,
        248,37,250,3,251,225,254,33,0,151,2,194,4,15,4,47,3,124,2,9,0,40,254,52,252,223,252,136,252,219,253,186,255,70,1,154,4,175,7,227,10,138,12,44,12,150,11,235,10,106,8,120,6,66,4,76,3,19,2,133,2,193,3,114,3,192,3,70,2,45,0,181,254,192,252,147,250,76,248,
        26,246,125,244,197,243,102,242,180,242,108,242,173,243,115,244,170,246,41,247,200,250,80,253,19,255,231,2,93,4,85,5,19,4,210,3,232,2,186,1,118,0,126,0,114,1,97,3,44,5,183,9,12,12,244,17,74,21,253,26,222,31,150,35,232,39,136,42,16,43,216,44,233,45,139,
        45,140,45,21,44,208,45,54,46,45,47,229,50,53,53,31,56,133,60,1,63,61,66,22,68,108,70,109,71,244,72,210,72,230,72,62,70,243,68,246,66,122,63,201,61,26,58,92,56,60,54,162,53,121,52,125,51,96,50,31,48,177,47,55,45,214,44,148,43,7,41,63,39,96,37,204,36,105,
        35,45,33,254,33,39,32,119,31,205,31,13,30,23,29,2,28,23,27,79,26,117,25,40,23,194,22,152,21,154,20,157,19,98,17,158,15,215,14,57,12,180,11,98,10,37,9,78,8,186,8,68,7,143,6,115,4,241,3,109,1,237,0,133,255,137,254,169,254,138,254,203,255,94,0,25,0,249,
        1,236,2,250,4,12,4,236,5,144,6,8,6,86,6,162,7,4,7,159,8,52,8,228,9,226,11,50,12,181,14,124,16,69,17,160,18,133,18,181,18,73,17,22,15,57,13,51,10,253,8,139,6,24,3,188,1,101,254,220,252,49,249,119,247,56,245,150,244,96,243,174,243,73,242,230,242,162,241,
        233,241,73,239,250,237,235,235,34,232,92,229,98,226,151,223,250,221,179,219,206,218,87,217,127,217,31,217,36,217,247,219,172,222,20,224,165,226,238,228,163,229,144,230,45,230,128,230,104,230,20,229,116,229,38,228,250,229,62,229,222,230,126,230,239,231,
        127,231,234,232,7,231,113,230,199,229,238,229,15,228,95,227,140,226,118,225,205,225,181,226,111,227,85,228,42,229,69,230,178,231,233,232,141,232,236,233,18,233,99,233,196,233,154,232,144,230,250,229,218,229,150,229,221,230,30,229,252,229,235,229,238,
        230,34,230,192,231,130,231,233,231,172,231,127,231,89,230,141,229,12,227,106,225,151,222,250,219,201,217,23,214,147,211,105,208,50,205,79,203,14,201,35,199,119,198,152,198,138,198,228,199,74,199,38,198,225,198,193,199,69,200,191,203,89,206,115,209,243,
        214,29,218,210,223,150,227,246,231,240,236,47,240,110,243,216,246,81,248,91,250,158,253,145,0,234,4,76,6,197,7,204,7,189,7,21,5,194,4,47,2,129,1,92,0,206,0,234,1,230,3,200,6,93,9,64,12,23,14,65,15,77,15,10,13,137,11,187,9,160,7,164,5,216,4,218,5,41,6,
        26,6,237,6,195,5,134,3,163,1,160,255,165,253,222,252,102,250,229,249,88,248,128,248,37,248,2,248,4,248,22,248,61,248,196,249,244,251,230,254,145,1,168,4,236,7,193,9,44,9,14,7,229,6,143,4,227,3,67,2,79,2,14,2,167,4,15,6,187,10,144,15,16,19,247,25,0,29,
        199,33,228,37,40,39,12,39,251,40,74,40,89,40,71,40,34,40,53,41,0,42,203,45,92,48,104,51,214,55,137,59,191,63,243,67,204,70,242,73,96,75,64,76,174,77,98,77,36,75,232,73,150,70,221,67,255,65,61,62,161,60,108,58,176,57,115,56,29,54,141,52,218,50,212,48,
        250,47,109,46,60,45,16,43,111,41,196,40,93,38,245,37,118,35,170,33,197,32,88,31,98,30,167,29,160,28,102,27,42,26,54,24,255,23,144,22,33,20,207,19,238,19,86,18,108,16,245,15,28,13,22,11,40,8,226,6,177,4,186,3,62,2,30,1,87,0,190,0,51,255,214,255,159,255,
        165,255,181,255,221,0,51,0,190,1,72,1,178,2,3,2,85,2,217,3,134,4,114,5,159,6,208,7,158,8,41,8,178,9,105,10,17,11,43,12,190,14,135,16,105,18,71,19,220,21,63,22,38,22,98,21,233,20,97,18,59,15,163,12,145,9,113,6,68,3,105,0,198,254,4,251,107,248,224,247,
        20,245,160,244,161,243,203,243,32,242,103,241,245,241,42,240,21,238,49,235,182,233,28,230,79,227,200,225,25,222,78,219,252,217,242,216,74,215,13,214,45,214,24,214,142,215,174,217,65,219,40,221,5,222,147,223,224,224,238,225,152,225,234,225,163,225,53,
        224,131,223,224,223,184,224,90,225,132,226,200,227,232,228,240,229,160,229,177,229,102,228,236,228,114,227,164,226,171,226,0,225,73,224,158,224,199,225,166,226,164,227,92,228,28,228,196,228,240,228,232,229,22,229,198,230,138,231,83,232,22,232,83,231,
        234,231,53,230,162,230,63,229,228,229,157,229,238,230,77,230,188,231,65,231,194,231,205,231,33,230,9,228,201,227,53,225,66,223,44,220,236,218,144,215,208,212,226,209,248,207,6,204,82,202,25,200,87,199,7,197,226,197,104,197,211,198,137,198,233,198,233,
        198,156,198,186,199,202,201,186,204,124,207,236,212,34,216,235,221,230,226,110,230,148,234,157,238,252,243,44,246,183,249,241,253,34,0,151,4,67,7,122,9,228,11,58,11,88,10,164,9,66,7,171,6,22,5,81,5,87,5,246,6,141,7,165,9,153,12,37,14,193,16,191,17,199,
        18,12,17,72,15,233,13,215,11,201,10,62,9,85,8,239,9,12,9,24,8,142,7,96,5,213,4,99,2,133,0,166,255,111,254,69,253,12,251,191,250,222,250,101,250,72,250,91,250,145,251,50,252,73,254,52,0,107,2,214,5,5,6,154,7,54,6,55,4,54,1,153,255,18,253,33,251,255,252,
        7,252,244,254,181,1,179,5,145,9,250,14,243,19,230,24,120,28,171,32,6,34,17,34,213,35,4,35,9,34,251,34,242,35,27,35,253,37,176,40,87,43,183,47,145,51,139,55,138,59,137,63,122,66,235,69,207,72,24,73,135,73,255,73,132,72,1,69,215,67,36,64,130,61,235,59,
        161,57,254,56,213,55,226,55,29,54,25,52,253,51,154,49,249,48,149,47,121,46,67,44,159,42,171,41,0,39,153,38,58,36,133,34,192,33,1,31,191,30,167,29,72,27,240,26,219,25,238,25,98,24,231,24,70,23,148,22,189,21,164,20,53,18,48,15,192,13,35,10,204,8,193,6,
        202,4,218,3,145,3,42,3,19,2,247,2,239,3,52,3,106,3,152,3,147,3,171,3,170,3,177,4,19,4,124,5,4,5,212,6,163,7,121,8,82,9,34,9,207,10,65,10,204,11,73,11,142,12,7,12,217,14,28,15,175,17,141,19,82,20,230,22,42,22,203,22,235,22,81,20,232,19,29,16,202,14,117,
        12,30,9,139,7,79,5,5,2,52,254,238,251,227,249,184,247,247,246,93,244,212,243,126,242,69,241,15,239,255,238,89,236,45,233,233,231,79,228,120,225,191,222,196,220,81,218,21,216,16,214,9,212,18,210,136,210,35,210,81,211,69,213,8,215,21,217,51,219,78,220,
        158,221,79,221,61,221,49,221,97,221,193,222,51,222,217,223,209,225,8,226,81,227,180,228,210,229,153,229,167,228,235,227,180,226,164,226,35,225,90,224,57,223,162,223,185,223,220,223,185,223,191,224,105,226,29,227,158,228,92,228,138,229,5,229,163,230,14,
        230,50,230,70,230,195,231,71,231,98,230,230,230,8,229,220,230,188,231,199,232,139,233,154,234,219,235,103,235,55,234,206,234,28,232,240,231,108,229,192,227,233,225,190,223,180,221,151,219,98,216,197,213,75,209,145,206,26,203,164,202,97,201,50,200,0,199,
        83,199,86,199,242,200,158,200,165,199,253,199,74,199,36,200,14,202,10,205,70,210,27,215,139,220,146,225,17,229,189,234,153,239,101,243,210,247,247,251,218,255,41,2,30,4,235,7,153,9,175,11,18,11,127,10,202,9,20,6,252,4,224,3,206,3,210,4,90,5,124,7,130,
        10,192,14,86,17,230,20,218,23,51,24,184,24,238,23,116,20,219,17,207,15,8,12,210,11,187,11,76,10,239,10,51,9,177,8,207,7,112,5,205,4,191,4,44,3,221,3,78,2,142,1,85,0,4,254,253,254,44,253,132,252,253,252,196,253,150,254,225,0,119,2,90,4,33,5,43,4,153,2,
        239,0,147,254,0,251,104,249,169,248,229,248,239,250,15,251,226,254,85,1,105,5,87,9,196,14,85,18,215,23,81,26,240,29,90,30,194,31,172,32,65,32,220,33,229,35,72,36,226,39,5,41,162,45,31,49,51,53,128,57,216,62,7,65,223,69,129,72,126,74,138,75,177,76,60,
        75,237,74,144,72,50,69,87,66,114,63,176,61,40,59,49,57,209,56,177,55,173,54,160,53,33,51,43,49,55,47,178,46,59,44,128,42,91,40,17,37,190,35,126,33,125,31,123,29,145,28,182,28,45,27,250,27,185,27,115,27,49,26,196,26,2,25,37,23,220,22,168,21,152,20,114,
        18,224,17,61,15,3,12,210,10,224,8,182,6,191,5,115,4,243,4,196,4,177,4,159,4,84,4,23,4,22,4,52,4,58,4,145,5,133,6,123,7,29,7,166,8,95,9,23,9,218,10,200,11,160,12,48,12,241,13,193,14,174,15,142,16,85,17,41,18,29,19,74,20,93,21,49,21,224,22,124,23,12,22,
        254,22,202,22,71,21,49,19,158,17,130,15,37,12,102,9,75,6,50,3,35,255,221,252,100,249,51,246,164,244,155,243,5,241,174,240,114,239,114,238,103,237,105,236,47,234,197,232,237,230,185,228,145,225,201,222,136,219,73,216,56,213,245,211,245,210,91,209,135,
        209,157,210,157,212,29,214,13,216,16,218,36,219,122,220,64,220,3,219,161,219,242,220,172,221,23,221,59,221,146,222,15,222,236,223,216,224,239,225,212,226,83,226,101,225,255,225,48,224,45,223,225,223,188,223,139,223,177,224,20,224,122,224,248,225,242,
        227,128,228,206,229,101,229,117,229,119,229,95,229,81,229,106,229,70,229,38,229,152,229,156,228,215,228,6,227,198,228,71,229,85,230,248,232,155,233,194,234,77,234,250,235,110,234,255,233,197,232,182,231,165,230,61,228,103,226,241,225,181,223,234,221,
        226,219,133,216,246,214,20,211,79,209,106,208,89,207,112,206,94,205,92,204,192,205,1,205,135,205,134,204,218,204,112,205,16,206,187,209,70,212,252,217,185,222,190,227,107,231,172,235,171,239,187,243,205,247,175,251,28,253,222,0,62,2,183,5,23,7,115,9,
        193,11,81,11,180,10,196,9,16,7,87,5,239,5,52,4,249,5,108,6,65,7,187,10,12,13,64,16,175,19,211,21,247,22,210,22,63,20,95,17,200,14,246,12,155,11,83,11,102,11,137,11,144,11,76,10,240,10,48,9,30,7,244,6,130,5,3,3,179,2,138,1,103,0,64,255,122,254,247,254,
        157,254,106,254,239,255,213,1,69,2,204,4,83,5,151,6,76,5,244,4,149,1,196,254,16,250,122,247,116,245,168,244,238,245,125,247,45,249,206,253,89,1,162,6,71,10,184,14,203,18,126,21,154,23,246,25,152,26,176,27,160,28,148,29,159,30,142,31,190,33,150,35,245,
        38,215,42,134,46,199,51,71,55,204,60,20,63,221,66,217,69,3,70,58,70,195,70,168,69,190,67,238,65,68,62,176,60,45,57,252,56,46,54,174,53,121,52,111,51,136,50,62,48,180,47,51,45,168,43,230,42,22,40,63,37,233,35,141,33,118,31,107,29,191,28,52,27,2,26,126,
        26,89,26,112,26,79,26,54,26,22,25,218,25,71,24,130,23,173,22,155,21,129,20,29,18,137,16,207,14,233,13,11,11,45,9,102,7,194,6,93,5,149,4,246,4,108,3,253,3,236,3,249,4,73,4,199,5,148,6,152,7,115,8,60,8,246,9,142,9,229,10,44,10,110,10,192,10,213,11,54,11,
        148,12,21,12,159,13,6,13,128,14,24,14,246,16,58,17,108,18,190,19,252,21,59,22,15,22,146,22,130,22,6,21,16,19,133,17,2,13,240,10,108,6,216,3,131,0,48,253,53,249,249,247,23,244,191,242,228,241,137,240,116,239,211,239,53,238,100,237,23,235,92,233,42,230,
        102,227,38,223,250,220,216,217,213,215,20,213,10,211,227,211,43,210,228,211,15,211,167,212,221,214,85,216,131,218,160,220,69,221,194,222,56,222,150,222,197,222,164,222,182,222,204,222,213,222,246,223,98,224,63,225,72,226,28,226,100,226,28,225,151,225,
        8,225,0,225,4,224,248,225,62,225,230,226,93,226,62,226,68,226,146,227,65,227,186,228,28,228,134,228,199,229,31,229,181,230,131,231,55,231,147,231,241,232,7,231,180,231,136,231,231,232,194,233,136,234,111,235,105,236,70,236,152,236,194,237,40,237,94,237,
        12,236,134,236,90,235,178,234,154,232,255,231,97,229,192,227,186,225,166,223,51,220,147,218,36,215,241,214,0,212,62,210,228,209,248,209,160,209,240,210,100,210,218,211,36,211,98,211,242,212,220,214,147,217,148,221,201,226,74,230,219,235,83,239,173,243,
        57,246,113,249,100,251,248,254,70,0,130,2,225,5,11,6,230,8,117,9,156,9,239,9,1,7,58,4,242,3,64,2,157,2,205,3,103,4,128,6,87,9,44,12,180,15,251,18,83,19,198,20,5,19,104,17,219,15,128,12,212,10,195,9,203,9,64,9,106,9,75,9,62,8,201,7,233,7,0,6,32,4,192,
        3,41,1,137,0,26,254,134,253,10,251,216,251,36,250,238,251,82,251,255,253,80,255,111,1,220,4,81,6,48,7,18,6,137,4,218,2,52,254,249,251,194,248,206,246,158,245,107,245,96,246,200,249,52,252,78,0,8,4,34,8,102,12,122,15,242,18,141,20,173,22,110,23,239,25,
        46,26,68,27,55,28,84,29,188,31,71,33,38,35,130,38,90,41,173,45,39,48,159,51,247,55,14,57,164,59,221,61,100,62,112,62,186,62,56,60,220,58,211,56,51,53,173,51,156,50,28,49,57,48,73,47,121,46,109,45,5,43,46,41,4,38,219,36,253,35,22,33,95,31,216,30,50,29,
        16,28,41,27,142,27,2,26,154,26,45,26,3,25,234,25,190,25,79,24,152,23,189,22,243,21,221,20,204,19,230,19,64,18,177,17,225,16,168,15,2,13,11,10,247,9,2,7,57,5,252,5,54,4,160,4,26,3,213,3,192,3,165,3,171,3,217,4,65,4,184,5,62,5,184,6,57,6,140,6,244,7,61,
        7,137,7,186,7,234,8,7,8,79,8,167,9,45,9,153,10,33,10,193,11,158,12,174,13,238,15,30,16,40,17,22,17,234,18,113,18,165,18,61,17,77,15,181,13,112,10,199,7,85,3,142,255,142,251,190,248,108,245,178,243,234,242,120,241,164,240,163,240,24,239,208,239,97,238,
        211,237,181,236,43,234,81,232,8,229,148,226,227,223,243,221,79,219,78,217,228,217,109,217,137,218,57,219,40,220,110,221,160,223,8,224,220,226,103,227,237,229,27,229,216,230,61,230,185,231,22,230,248,230,185,230,99,230,86,230,117,230,181,231,80,232,68,
        232,236,233,117,233,197,233,196,233,132,233,35,232,170,232,42,231,148,230,232,230,130,230,124,230,245,231,172,232,104,233,125,234,132,235,77,235,149,235,189,236,4,236,66,236,117,236,123,236,94,235,202,235,83,235,49,235,119,235,231,236,223,238,46,239,
        54,239,186,240,102,241,74,242,97,242,198,242,205,242,121,241,175,240,129,238,245,237,90,235,179,234,24,232,130,230,183,229,33,227,208,226,254,226,56,225,64,224,54,223,193,223,227,224,52,224,174,225,90,226,53,227,41,228,91,229,231,232,22,234,203,237,188,
        240,193,243,152,246,42,248,164,251,3,253,85,255,173,1,241,3,181,5,48,6,94,7,128,8,110,9,1,9,37,8,139,7,112,5,241,4,152,3,200,3,120,3,206,4,124,5,180,6,227,8,92,10,47,12,30,13,245,15,24,15,2,14,53,12,212,11,174,10,221,10,108,10,65,10,96,10,144,10,173,
        10,132,9,183,8,38,6,108,5,16,3,154,2,179,1,244,1,49,0,119,255,220,255,90,255,9,255,5,255,113,0,114,1,179,3,75,4,249,6,98,7,60,7,43,6,62,4,159,2,54,255,155,253,24,251,17,249,249,249,122,249,212,250,252,252,205,255,39,1,137,3,183,5,187,7,216,9,249,11,206,
        13,94,14,162,15,136,16,45,16,124,16,206,17,41,17,126,18,22,19,31,20,167,22,106,24,136,26,214,29,15,31,15,33,2,34,153,35,189,36,122,36,118,35,199,34,160,33,133,32,128,31,103,30,55,29,56,28,107,27,132,26,135,25,67,23,195,22,60,21,16,19,240,18,210,17,189,
        16,231,16,31,15,74,14,157,13,193,13,8,12,204,12,179,12,154,12,86,12,82,12,115,12,180,12,237,12,212,12,181,12,210,12,252,12,252,12,161,12,75,11,196,11,22,10,58,9,45,7,230,6,168,5,148,4,180,3,249,3,35,2,111,2,17,2,27,2,71,2,157,3,5,3,120,3,231,4,92,4,174,
        4,218,5,11,5,70,5,137,5,246,6,64,6,102,6,101,6,114,6,137,6,168,6,209,7,28,7,97,7,205,8,64,8,240,9,150,10,65,10,225,11,26,11,50,11,2,10,132,10,41,9,106,8,149,7,97,5,245,4,59,2,84,0,68,254,18,252,3,250,60,248,243,247,192,247,10,246,140,246,17,245,96,244,
        144,243,191,242,227,242,30,240,145,239,47,237,221,236,133,235,135,234,190,233,167,232,254,232,150,232,97,232,209,233,39,233,187,234,179,235,243,236,249,237,194,238,118,238,213,239,71,239,106,239,196,240,43,240,89,240,59,240,66,240,103,240,111,240,135,
        240,243,241,60,241,98,241,130,241,132,241,98,241,66,241,26,240,229,240,157,240,50,239,185,239,151,239,161,239,241,240,131,241,56,241,251,242,72,242,159,242,220,242,249,243,14,243,21,243,30,243,14,242,166,242,38,241,213,241,197,242,4,242,156,243,82,244,
        53,244,237,245,184,246,114,247,51,247,196,248,15,248,45,248,83,248,65,247,220,247,62,246,146,245,196,244,201,243,174,242,166,241,188,241,22,240,212,240,229,240,248,240,248,241,1,241,94,241,228,242,83,242,145,243,28,243,234,244,248,246,103,248,43,249,
        250,251,227,253,213,255,162,1,93,2,230,4,37,5,95,6,151,7,216,8,251,9,253,10,184,11,122,12,41,12,143,12,93,11,119,10,51,8,218,7,201,7,49,6,197,6,194,6,217,7,104,8,64,9,53,10,55,11,63,12,64,12,208,12,183,12,0,11,18,10,35,9,138,9,134,9,237,10,142,10,235,
        11,19,10,253,10,78,9,109,8,41,6,181,5,84,4,11,2,244,2,74,1,223,1,164,1,135,1,122,1,79,1,57,1,99,1,194,2,117,3,82,3,232,4,37,3,213,2,247,1,130,255,174,253,142,251,250,250,222,250,50,250,50,250,183,251,179,253,9,254,139,0,11,1,125,2,182,3,190,4,178,5,117,
        5,252,6,47,6,19,5,195,5,108,5,66,5,101,5,162,6,69,7,31,8,46,9,117,11,29,12,177,14,11,15,73,16,116,17,63,17,164,17,238,18,32,17,206,17,0,16,11,14,193,13,126,12,69,11,36,10,33,9,100,8,218,8,78,7,158,6,212,6,30,5,99,4,163,4,13,3,135,3,8,2,178,2,120,2,70,
        2,28,2,23,2,106,2,210,3,39,3,128,3,240,4,145,5,57,5,162,5,203,6,28,6,127,6,188,6,231,6,213,6,144,6,11,5,124,5,33,4,153,4,29,3,202,3,152,3,109,3,70,3,26,2,222,2,172,2,151,2,182,2,235,3,39,3,127,3,232,4,66,4,141,4,166,4,167,4,189,4,231,5,4,5,33,5,76,5,
        159,5,203,5,212,5,220,6,4,6,47,6,147,6,228,7,122,8,27,8,143,8,201,8,179,8,157,8,75,7,187,7,54,6,83,5,144,4,138,3,145,2,114,1,78,0,26,254,182,253,118,252,28,250,240,250,37,249,116,249,39,249,5,248,188,248,51,247,80,246,78,245,153,244,188,243,253,243,58,
        242,109,241,209,241,76,240,211,240,125,240,13,239,212,239,223,240,25,240,107,241,2,241,189,242,130,243,90,243,169,243,206,243,172,243,138,243,162,243,251,244,61,244,151,244,238,245,100,245,229,246,67,246,95,246,55,245,244,245,88,244,227,244,101,244,7,
        244,0,244,36,244,116,244,147,244,202,245,21,245,108,245,151,245,130,245,103,245,43,244,212,244,158,244,191,244,248,245,46,245,140,245,214,245,186,245,148,245,125,245,165,246,86,247,23,247,229,248,178,249,65,249,201,250,89,251,7,251,168,252,7,252,75,252,
        60,252,17,251,198,251,103,251,39,251,11,250,223,250,151,250,87,250,83,250,118,250,179,250,230,251,88,251,189,251,245,252,23,252,60,252,114,252,151,252,196,253,35,253,214,254,189,255,228,1,60,2,185,4,19,5,68,6,72,7,103,8,88,9,57,9,222,10,120,11,30,11,
        241,12,181,13,125,13,236,14,41,14,29,13,190,13,17,12,39,10,252,9,229,8,251,8,79,8,13,7,194,7,158,8,70,9,41,10,2,10,220,11,54,11,48,10,247,10,139,9,249,9,135,8,203,8,125,8,52,7,238,7,143,7,57,6,238,6,253,7,17,7,17,6,223,6,143,6,3,5,113,4,184,4,18,3,125,
        3,6,2,248,2,193,2,156,2,161,2,114,2,34,1,223,1,86,0,127,255,158,254,166,253,189,252,235,252,35,251,140,251,60,250,251,251,31,251,151,252,75,253,78,254,137,255,230,1,21,2,42,2,218,3,87,3,93,3,8,2,110,1,179,0,231,0,32,255,134,255,41,255,47,255,181,0,141,
        1,118,2,102,3,53,3,188,3,251,4,40,4,112,4,162,4,185,4,227,4,206,4,130,4,38,3,143,2,165,1,172,0,234,0,91,255,159,255,25,254,225,254,159,254,116,254,137,254,120,254,87,254,65,253,228,253,134,252,236,252,82,251,223,251,197,251,252,252,74,252,190,253,113,
        254,112,255,55,255,227,0,111,0,191,1,9,1,114,1,222,2,48,2,138,2,255,3,143,3,211,3,175,3,109,3,53,2,232,2,194,2,201,2,215,3,1,3,63,3,117,3,118,3,70,2,225,2,140,2,103,2,99,2,89,2,75,2,117,2,214,3,30,3,100,3,168,3,225,4,37,4,77,4,85,4,81,4,73,4,84,4,102,
        4,135,4,232,5,51,5,134,5,207,5,228,5,228,5,199,5,147,5,75,5,12,4,189,4,117,3,247,3,89,2,194,1,255,1,47,0,107,255,130,254,239,254,127,254,27,253,185,253,39,252,120,251,232,251,94,250,215,250,139,250,101,250,3,249,169,248,221,248,52,247,196,246,203,246,
        45,245,136,244,255,244,188,244,172,244,223,245,11,245,36,245,104,245,252,246,154,246,239,247,53,247,121,247,124,247,146,247,123,247,53,247,35,247,6,247,110,248,4,248,98,248,181,248,200,248,177,248,76,247,208,247,96,247,22,246,212,246,160,246,156,246,
        160,246,221,247,40,247,113,247,168,247,178,247,161,247,128,247,100,247,63,247,66,247,80,247,77,247,102,247,154,247,254,248,83,248,137,248,246,249,87,249,214,250,104,250,249,251,121,251,202,252,70,252,194,253,110,254,22,254,166,254,243,255,25,255,54,255,
        58,255,5,254,159,254,73,254,33,254,16,254,49,254,115,254,234,255,142,0,15,0,143,0,240,1,97,1,175,1,241,2,92,2,181,2,235,3,27,3,93,3,214,4,138,5,131,6,119,7,125,8,104,9,67,9,239,10,91,10,134,10,188,11,18,11,141,11,246,12,29,12,90,12,156,13,26,13,94,13,
        74,12,219,12,95,11,228,11,93,10,164,9,208,9,91,9,59,9,59,9,92,9,170,9,197,10,23,10,73,10,35,9,236,9,127,9,27,8,179,8,135,8,84,7,225,7,151,7,31,6,198,6,69,5,217,5,79,4,228,4,163,4,213,4,247,5,52,5,111,5,40,4,217,4,90,3,197,3,46,2,133,2,28,1,223,1,164,
        1,75,0,166,255,226,255,29,254,99,253,128,252,133,251,173,250,226,250,111,250,83,250,118,251,27,252,9,253,70,254,108,255,130,0,60,0,168,0,241,1,17,1,24,0,230,0,125,0,10,255,102,254,209,254,106,254,51,254,12,254,24,254,62,254,130,254,162,254,170,255,5,
        255,141,255,217,0,0,255,228,255,148,255,44,254,199,254,42,253,118,252,240,252,196,252,165,252,143,252,139,252,79,252,2,251,154,251,56,250,172,250,53,249,246,249,199,249,199,249,221,250,10,250,44,250,89,250,140,250,197,251,6,251,93,251,166,251,247,252,
        103,252,237,253,123,254,58,255,39,255,205,0,94,0,202,1,51,1,133,1,204,2,32,2,118,2,211,3,26,3,88,3,121,3,109,3,92,3,99,3,132,3,141,3,157,3,142,3,116,3,73,2,251,2,131,2,31,1,208,1,178,1,141,1,114,1,103,1,105,1,126,1,154,1,202,1,233,2,14,2,63,2,107,2,124,
        2,140,2,186,3,37,3,95,3,136,3,116,3,118,3,174,3,210,4,22,4,2,3,231,3,153,3,3,2,93,1,166,0,234,0,79,255,250,255,205,255,145,255,127,255,84,255,54,254,250,254,191,254,159,254,127,254,58,253,244,253,152,253,75,253,5,252,189,252,114,252,20,251,101,250,109,
        249,144,248,216,248,99,248,61,248,1,247,190,247,220,248,37,248,121,248,227,249,54,249,94,249,165,249,250,250,57,250,112,250,96,250,65,250,37,249,230,249,165,249,73,249,32,249,26,249,73,249,105,249,98,249,93,249,91,249,77,249,86,249,70,249,84,249,136,
        249,142,249,134,249,119,249,72,249,11,248,177,248,94,248,52,248,29,248,91,248,164,248,239,249,19,249,62,249,94,249,100,249,124,249,184,249,255,250,155,251,72,252,30,252,255,253,208,254,91,254,243,255,105,255,217,0,51,0,105,0,145,0,184,0,227,1,11,1,39,
        1,28,1,15,0,240,0,198,0,212,0,233,1,14,1,89,1,169,1,222,2,71,2,183,3,67,3,220,4,96,4,171,4,241,5,45,5,151,6,53,6,206,7,99,7,226,8,95,8,247,9,124,9,235,10,119,10,229,11,36,11,47,11,56,11,61,11,72,11,50,11,37,11,40,11,26,11,25,10,199,10,83,9,226,9,154,
        9,132,9,143,9,163,9,201,9,246,10,31,10,23,9,218,9,131,9,9,8,149,8,43,7,165,6,235,6,73,5,174,5,84,5,44,5,15,4,241,4,227,4,211,4,180,4,124,4,62,3,243,3,189,3,139,3,135,3,196,3,185,3,159,3,99,2,248,2,126,2,23,1,175,1,89,1,27,0,193,0,73,255,180,254,254,254,
        20,253,45,252,78,251,139,251,11,250,202,250,224,251,97,252,43,253,13,253,202,254,121,254,235,255,76,255,113,255,140,255,174,255,194,255,187,255,246,255,244,255,196,255,123,255,39,254,211,254,142,254,84,254,55,254,31,254,1,253,208,253,153,253,63,252,211,
        252,164,252,133,252,61,251,220,251,93,250,206,250,104,250,3,249,178,249,122,249,109,249,142,249,162,249,167,249,123,249,43,248,207,248,128,248,79,248,40,248,43,248,123,248,252,249,155,250,58,250,234,251,136,252,14,252,106,252,149,252,176,252,236,253,
        28,253,110,253,233,254,106,254,253,255,129,0,0,0,133,0,234,1,72,1,151,1,209,2,0,2,25,2,111,2,215,3,28,3,88,3,89,3,49,2,254,2,189,2,117,2,52,1,241,1,202,1,164,1,126,1,78,0,255,0,197,0,140,0,108,0,62,0,19,255,235,255,209,255,208,255,234,0,29,0,129,0,248,
        1,89,1,141,1,128,1,90,1,79,1,75,1,92,1,132,1,132,1,139,1,135,1,96,1,21,0,172,0,69,255,241,255,187,255,116,255,69,255,62,255,34,255,58,255,106,255,123,255,151,255,142,255,129,255,162,255,191,255,203,255,195,255,166,255,105,254,249,254,134,253,244,253,
        120,253,34,252,186,252,125,251,254,251,153,251,68,251,75,251,43,251,79,251,112,251,161,252,18,252,99,252,127,252,145,252,74,252,52,252,31,252,23,251,205,251,142,251,117,251,82,251,67,251,43,250,223,250,165,250,159,250,191,250,235,251,6,251,28,251,61,
        251,93,251,50,250,252,250,144,250,31,249,227,249,158,249,134,249,130,249,165,249,190,249,255,250,44,250,60,250,109,250,159,250,169,250,181,250,204,251,17,251,126,251,255,252,174,253,113,254,38,254,210,255,140,0,39,0,168,1,17,1,126,1,209,2,35,2,90,2,132,
        2,161,2,207,2,250,3,45,3,62,3,51,3,37,3,3,2,225,2,215,2,236,3,17,3,116,3,221,4,95,4,218,5,62,5,183,6,105,6,240,7,126,7,225,8,48,8,128,8,197,8,251,9,44,9,75,9,150,9,244,10,63,10,114,10,144,10,130,10,149,10,166,10,148,10,74,9,249,9,174,9,121,9,67,9,8,8,
        212,8,210,8,214,9,3,9,41,9,75,9,102,9,96,9,74,9,3,8,172,8,50,7,118,6,158,5,207,5,15,4,145,4,76,3,250,3,218,3,243,4,12,3,253,3,246,3,210,3,159,3,139,3,64,2,182,2,23,1,137,1,22,0,212,0,162,0,155,0,223,1,17,1,76,1,65,1,4,0,163,0,101,0,0,255,152,255,50,254,
        169,254,49,253,174,253,49,252,218,252,192,252,182,252,214,253,30,253,95,253,159,253,197,253,231,254,26,254,76,254,108,254,153,254,187,254,200,254,205,254,214,254,169,254,135,254,126,254,76,254,41,253,239,253,138,253,38,252,163,252,63,251,243,251,146,
        251,46,250,217,250,140,250,64,250,60,250,12,249,190,249,75,248,198,248,40,247,131,246,247,246,150,246,114,246,160,246,243,247,48,247,130,247,185,247,250,248,34,248,80,248,115,248,214,249,86,250,6,250,155,251,47,251,196,252,82,252,232,253,78,253,171,253,
        244,254,34,254,84,254,112,254,174,254,254,255,98,255,198,0,51,0,144,1,11,1,111,1,198,2,16,2,48,2,66,2,107,2,140,2,158,2,146,2,110,2,56,1,237,1,136,1,61,0,244,0,205,0,199,0,190,0,169,0,119,0,40,255,196,255,75,254,221,254,163,254,144,254,175,254,209,255,
        19,255,69,255,94,255,97,255,75,255,119,255,154,255,195,255,178,255,149,255,140,255,101,255,69,255,9,254,221,254,203,254,232,254,227,254,223,254,229,255,0,255,16,255,33,255,55,255,75,255,91,255,91,255,104,255,147,255,184,255,238,0,47,0,107,0,150,0,155,
        0,133,0,98,0,3,255,140,255,25,254,178,254,113,254,49,254,5,253,236,253,231,253,207,253,186,253,177,253,153,253,153,253,107,253,63,253,57,253,78,253,71,253,91,253,63,253,20,252,244,252,217,252,211,252,183,252,164,252,133,252,124,252,111,252,89,252,70,
        252,50,252,60,252,34,252,2,251,218,251,169,251,126,251,100,251,67,251,56,251,90,251,122,251,139,251,151,251,153,251,136,251,154,251,164,251,202,252,7,252,73,252,157,253,3,253,124,253,233,254,73,254,175,255,34,255,163,0,33,0,186,1,76,1,223,2,131,3,46,
        3,185,4,41,4,88,4,126,4,142,4,162,4,141,4,108,4,78,4,69,4,120,4,146,4,167,4,206,4,243,5,25,5,63,5,108,5,145,5,174,5,244,6,54,6,140,7,2,7,130,8,3,8,96,8,142,8,197,8,215,9,9,9,71,9,131,9,106,9,81,9,118,9,131,9,160,9,167,9,149,9,157,9,146,9,125,9,62,8,243,
        8,186,8,144,8,79,7,254,7,159,7,115,7,87,7,115,7,141,7,175,7,201,7,158,7,75,6,158,5,224,4,241,4,88,3,202,3,116,3,110,3,135,3,121,3,103,3,57,3,38,3,5,2,232,2,149,2,57,1,222,1,121,1,7,0,165,0,65,255,217,255,201,255,199,255,202,255,228,0,14,0,62,0,120,0,
        127,0,94,0,40,255,227,255,130,255,41,254,231,254,192,254,146,254,103,254,62,253,243,253,188,253,143,253,103,253,88,253,89,253,103,253,107,253,114,253,114,253,124,253,120,253,96,253,44,253,5,252,246,253,26,253,19,252,197,252,128,252,77,252,49,252,21,251,
        209,251,137,251,114,251,117,251,96,251,32,250,193,250,78,249,209,249,80,248,196,248,62,247,175,247,80,247,2,246,190,246,125,246,75,246,73,246,136,246,255,247,127,248,5,248,177,249,97,249,234,250,90,250,129,250,165,250,208,251,57,251,197,252,93,252,247,
        253,161,254,34,254,162,254,243,255,66,255,131,255,201,255,253,0,40,0,64,0,94,0,173,1,13,1,104,1,211,2,43,2,110,2,131,2,115,2,52,1,230,1,153,1,114,1,103,1,103,1,117,1,130,1,107,1,51,0,216,0,134,0,55,255,235,255,144,255,59,255,0,254,211,254,157,254,113,
        254,81,254,56,254,31,254,9,253,232,253,226,253,228,253,243,253,240,254,1,253,242,253,251,253,238,254,8,254,0,254,24,254,23,254,18,254,6,254,44,254,60,254,81,254,139,254,171,254,224,254,246,254,252,254,249,254,246,255,9,255,51,255,130,255,185,0,1,0,36,
        0,66,0,87,0,86,0,65,0,58,0,17,0,14,255,235,255,173,255,80,255,7,254,196,254,189,254,180,254,162,254,159,254,151,254,113,254,48,254,4,253,229,253,207,253,202,253,193,253,210,253,225,253,232,253,229,253,231,253,181,253,161,253,154,253,153,253,166,253,163,
        253,156,253,120,253,80,253,6,252,165,252,73,251,234,251,194,251,190,251,215,251,252,252,46,252,102,252,110,252,103,252,71,252,63,252,60,252,78,252,124,252,186,253,27,253,116,253,240,254,101,254,198,255,68,255,192,0,39,0,115,0,187,1,22,1,123,1,226,2,115,
        2,235,3,89,3,171,3,250,4,56,4,123,4,181,4,217,5,1,5,0,5,0,5,27,5,40,5,66,5,131,5,216,6,43,6,115,6,147,6,156,6,147,6,138,6,122,6,129,6,163,6,191,6,224,7,45,7,128,7,201,8,31,8,93,8,126,8,129,8,131,8,165,8,179,8,180,8,208,9,17,9,59,9,91,9,87,9,32,8,236,
        8,182,8,113,8,18,7,130,6,223,6,152,6,46,5,225,5,145,5,122,5,91,5,81,5,50,4,214,4,103,3,251,3,111,3,12,2,189,2,135,2,99,2,56,2,35,1,234,1,193,1,168,1,94,1,58,0,245,0,202,0,163,0,108,0,65,0,26,255,238,255,209,255,142,255,77,255,43,254,254,254,241,255,44,
        255,124,255,208,0,21,0,55,0,62,0,18,255,198,255,137,255,75,255,66,255,73,255,62,255,61,255,33,255,21,254,242,254,206,254,135,254,83,254,29,253,226,253,179,253,91,253,21,252,237,252,210,252,176,252,121,252,46,251,211,251,146,251,148,251,151,251,169,251,
        209,252,36,252,122,252,181,252,160,252,103,252,24,251,164,251,36,250,122,249,180,249,28,248,174,248,92,248,43,248,8,247,245,248,12,248,44,248,37,248,31,248,23,248,49,248,113,248,216,249,83,249,221,250,121,251,35,251,223,252,113,252,221,253,53,253,107,
        253,164,253,224,254,27,254,108,254,209,255,76,255,191,0,40,0,107,0,143,0,159,0,191,0,211,0,229,1,0,1,24,1,51,1,58,1,56,1,64,1,67,1,75,1,85,1,83,1,76,1,50,1,21,0,235,0,217,0,204,0,168,0,130,0,54,255,205,255,109,255,23,254,191,254,112,254,45,253,244,253,
        186,253,156,253,125,253,81,253,27,252,205,252,165,252,138,252,117,252,104,252,106,252,140,252,182,252,217,252,235,253,2,253,46,253,94,253,141,253,178,253,210,253,236,254,15,254,38,254,59,254,87,254,112,254,163,254,214,255,21,255,72,255,131,255,163,255,
        219,0,15,0,65,0,91,0,108,0,105,0,108,0,108,0,102,0,84,0,71,0,69,0,51,0,15,255,199,255,98,255,9,254,207,254,173,254,167,254,169,254,177,254,164,254,130,254,131,254,167,254,171,254,167,254,155,254,153,254,145,254,167,254,195,254,202,254,178,254,157,254,
        141,254,130,254,126,254,85,254,44,254,20,253,217,253,136,253,60,253,5,252,228,252,233,252,235,252,241,252,241,252,244,252,250,253,6,253,24,253,33,253,67,253,138,253,214,254,66,254,188,255,43,255,160,0,32,0,130,0,237,1,105,1,225,2,86,2,178,2,255,3,49,
        3,98,3,128,3,167,3,195,3,226,4,20,4,92,4,204,5,55,5,148,5,219,6,17,6,58,6,101,6,155,6,195,6,210,6,253,7,46,7,79,7,94,7,115,7,112,7,76,7,53,7,50,7,36,7,17,7,27,7,39,7,69,7,75,7,130,7,223,8,13,8,61,8,97,8,151,8,182,8,234,8,251,8,218,8,172,8,99,8,57,7,225,
        7,105,7,0,6,144,6,30,5,183,5,83,4,246,4,145,4,29,3,179,3,88,3,9,2,200,2,142,2,108,2,57,1,251,1,179,1,153,1,105,1,81,1,33,0,248,0,198,0,127,0,105,0,55,255,253,255,242,255,206,255,202,255,191,255,173,255,137,255,116,255,95,255,72,255,34,255,3,254,238,254,
        254,255,34,255,84,255,131,255,192,255,226,255,242,255,252,255,241,255,232,255,252,0,6,255,242,255,217,255,206,255,205,255,188,255,156,255,115,255,55,254,227,254,103,253,239,253,134,253,24,252,178,252,81,251,230,251,147,251,70,251,13,250,216,250,206,250,
        229,251,13,251,61,251,117,251,161,251,204,251,236,251,222,251,158,251,81,250,251,250,127,250,7,249,155,249,50,248,218,248,160,248,124,248,110,248,108,248,153,248,236,249,57,249,119,249,154,249,167,249,169,249,176,249,219,250,31,250,173,251,108,252,70,
        253,9,253,177,254,41,254,113,254,148,254,169,254,184,254,207,254,242,255,40,255,112,255,176,255,216,0,10,0,75,0,144,0,197,0,226,0,234,0,230,0,233,0,204,0,176,0,155,0,150,0,162,0,175,0,180,0,172,0,173,0,159,0,129,0,51,255,228,255,159,255,119,255,93,255,
        75,255,18,254,200,254,116,254,19,253,196,253,114,253,56,253,30,252,239,252,208,252,157,252,107,252,77,252,30,251,248,251,211,251,197,251,186,251,186,251,227,252,16,252,102,252,179,253,10,253,88,253,146,253,188,253,220,253,239,254,12,254,56,254,85,254,
        148,254,223,255,69,255,156,255,230,0,24,0,69,0,105,0,114,0,118,0,102,0,98,0,107,0,111,0,123,0,116,0,102,0,69,0,29,0,4,0,3,255,248,255,206,255,177,255,112,255,58,255,29,254,245,255,15,255,62,255,95,255,133,255,167,255,141,255,120,255,117,255,90,255,65,
        255,47,255,33,255,52,255,70,255,95,255,95,255,43,255,12,254,247,254,234,254,224,254,151,254,109,254,90,254,49,254,33,253,243,253,178,253,152,253,148,253,145,253,127,253,130,253,153,253,226,254,48,254,135,254,235,255,79,255,187,0,51,0,168,1,27,1,147,1,
        244,2,72,2,142,2,218,3,28,3,56,3,59,3,44,3,41,3,63,3,114,3,167,3,235,4,59,4,126,4,223,5,59,5,165,6,15,6,94,6,183,6,248,7,46,7,68,7,96,7,117,7,162,7,194,7,200,7,220,7,216,7,200,7,194,7,153,7,96,7,86,7,78,7,89,7,105,7,122,7,155,7,225,8,43,8,96,8,142,8,
        147,8,143,8,122,8,67,7,247,7,136,6,248,6,118,6,32,5,198,5,120,5,55,4,210,4,108,4,4,3,181,3,75,2,218,2,120,2,16,1,166,1,96,1,38,1,9,0,244,0,217,0,208,0,170,0,109,0,50,255,242,255,170,255,115,255,84,255,63,255,69,255,52,255,58,255,44,255,23,254,242,254,
        202,254,191,254,188,254,191,254,193,254,210,254,220,254,243,255,29,255,79,255,130,255,149,255,165,255,191,255,212,255,237,255,241,255,248,0,24,0,66,0,73,0,97,0,87,0,47,0,8,255,201,255,137,255,58,254,192,254,42,253,139,252,254,252,132,252,32,251,194,251,
        97,251,17,250,215,250,134,250,103,250,85,250,115,250,152,250,179,250,190,250,190,250,194,250,213,250,219,250,216,250,198,250,180,250,150,250,122,250,69,250,40,250,6,249,252,250,36,250,65,250,87,250,98,250,121,250,159,250,188,250,215,251,0,251,71,251,
        153,251,234,252,46,252,117,252,196,253,52,253,179,254,58,254,162,254,234,255,36,255,75,255,93,255,105,255,94,255,97,255,112,255,148,255,188,255,239,0,39,0,116,0,181,0,234,1,0,0,247,0,211,0,157,0,105,0,54,0,6,255,223,255,202,255,208,255,202,255,169,255,
        138,255,75,255,0,254,177,254,112,254,72,254,37,254,29,254,37,254,23,253,242,253,171,253,116,253,44,252,219,252,158,252,97,252,67,252,71,252,96,252,95,252,85,252,73,252,31,252,25,252,13,252,20,252,59,252,120,252,182,253,19,253,92,253,146,253,200,253,242,
        254,42,254,90,254,121,254,171,254,221,255,34,255,102,255,183,255,244,0,61,0,122,0,162,0,176,0,159,0,154,0,158,0,150,0,140,0,141,0,108,0,69,0,55,0,32,0,25,0,1,0,12,0,18,0,22,0,22,0,1,255,231,255,196,255,183,255,219,0,14,0,72,0,89,0,98,0,48,255,250,255,
        217,255,176,255,162,255,129,255,133,255,160,255,185,255,224,255,239,255,238,255,213,255,165,255,145,255,93,255,57,255,26,255,47,255,57,255,22,254,207,254,121,254,48,253,251,254,11,254,33,254,72,254,123,254,216,255,44,255,134,255,216,0,57,0,157,0,248,
        1,85,1,190,2,43,2,139,2,214,2,253,3,20,3,31,3,49,3,67,3,75,3,96,3,160,3,222,4,30,4,87,4,121,4,170,4,217,5,29,5,120,5,219,6,58,6,123,6,201,6,252,7,17,7,46,7,42,7,50,7,72,7,100,7,132,7,144,7,165,7,168,7,162,7,194,7,168,7,161,7,162,7,161,7,122,7,68,7,42,
        7,32,7,45,7,65,7,82,7,61,7,18,6,219,6,134,6,40,5,178,5,37,4,149,4,19,3,152,3,57,3,2,2,218,2,219,2,203,2,133,2,23,1,136,1,7,0,133,0,48,255,253,255,244,255,212,255,221,255,199,255,191,255,178,255,159,255,144,255,127,255,105,255,98,255,88,255,83,255,72,
        255,62,255,62,255,40,255,14,254,238,254,220,254,205,254,216,255,1,255,21,255,50,255,69,255,86,255,113,255,147,255,147,255,137,255,129,255,145,255,163,255,173,255,174,255,188,255,206,255,241,0,15,0,21,255,248,255,187,255,112,255,16,254,166,254,60,253,
        206,253,109,253,19,252,175,252,78,252,7,251,175,251,65,250,216,250,111,250,17,249,237,249,228,249,249,250,54,250,104,250,163,250,213,250,233,250,251,250,229,250,253,251,50,251,110,251,165,251,211,251,229,251,244,251,248,252,16,252,14,252,6,251,248,251,
        251,252,18,252,38,252,66,252,124,252,205,253,30,253,105,253,154,253,171,253,178,253,204,253,240,254,37,254,94,254,174,254,254,255,65,255,109,255,133,255,123,255,106,255,105,255,104,255,115,255,165,255,235,0,72,0,147,0,187,0,194,0,168,0,109,0,46,255,219,
        255,145,255,75,255,40,255,7,254,239,254,228,254,192,254,152,254,102,254,49,253,253,253,190,253,148,253,134,253,127,253,107,253,102,253,99,253,73,253,27,252,217,252,114,252,57,252,32,252,41,252,57,252,84,252,113,252,110,252,106,252,75,252,63,252,56,252,
        81,252,131,252,193,252,255,253,87,253,161,253,221,254,12,254,52,254,120,254,171,254,247,255,72,255,151,255,250,0,64,0,122,0,154,0,181,0,176,0,177,0,172,0,180,0,176,0,179,0,166,0,173,0,170,0,175,0,183,0,145,0,134,0,136,0,132,0,152,0,172,0,176,0,180,0,
        180,0,180,0,154,0,133,0,129,0,141,0,147,0,136,0,141,0,94,0,39,255,248,255,212,255,230,255,234,0,11,0,54,0,87,0,101,0,98,0,71,0,25,255,231,255,198,255,209,255,232,255,231,255,227,255,239,255,206,255,160,255,88,255,33,254,254,255,4,255,40,255,80,255,158,
        255,230,0,64,0,136,0,169,0,222,1,13,1,75,1,151,1,229,2,59,2,129,2,217,3,17,3,51,3,55,3,41,3,46,3,93,3,125,3,168,3,208,3,217,3,239,4,8,4,23,4,62,4,87,4,160,5,1,5,119,5,205,6,8,6,64,6,104,6,126,6,133,6,137,6,162,6,180,6,212,6,252,7,27,7,72,7,86,7,50,6,
        251,6,162,6,65,5,244,5,188,5,141,5,97,5,41,4,249,4,234,4,206,4,192,4,188,4,182,4,146,4,69,3,228,3,110,2,241,2,96,1,252,1,208,1,184,1,212,1,222,1,189,1,94,0,251,0,115,255,238,255,133,255,63,255,16,255,3,255,5,255,29,255,95,255,131,255,144,255,136,255,
        116,255,90,255,84,255,75,255,59,255,61,255,75,255,106,255,134,255,141,255,149,255,142,255,156,255,145,255,122,255,105,255,87,255,93,255,84,255,97,255,113,255,162,255,210,255,250,255,248,255,235,255,202,255,176,255,134,255,84,255,50,255,47,255,69,255,
        72,255,41,255,0,254,211,254,170,254,101,254,9,253,178,253,92,252,255,252,150,252,35,251,178,251,79,251,3,250,208,250,176,250,144,250,108,250,85,250,89,250,93,250,108,250,123,250,177,250,247,251,46,251,94,251,154,251,191,251,234,252,75,252,146,252,200,
        252,255,253,42,253,89,253,128,253,128,253,131,253,127,253,121,253,112,253,114,253,134,253,163,253,190,253,229,253,249,253,253,254,1,254,4,254,8,254,18,254,38,254,70,254,120,254,181,254,231,255,30,255,76,255,109,255,160,255,194,255,196,255,205,255,216,
        255,212,255,217,255,235,255,250,255,242,255,226,255,183,255,124,255,69,255,7,254,213,254,180,254,128,254,84,254,37,254,8,253,226,253,202,253,182,253,189,253,185,253,167,253,167,253,148,253,125,253,94,253,49,253,27,253,13,253,15,253,1,253,9,253,13,253,
        5,253,17,253,5,252,251,252,255,252,251,252,254,253,27,253,64,253,112,253,152,253,196,253,246,254,40,254,81,254,116,254,156,254,228,255,44,255,112,255,209,0,36,0,122,0,176,0,201,0,201,0,191,0,199,0,184,0,206,0,206,0,224,0,233,1,2,1,7,1,6,1,7,1,9,1,17,
        1,9,1,25,1,29,1,21,1,22,1,13,1,14,1,0,0,212,0,183,0,114,0,61,0,25,255,255,0,10,0,29,0,65,0,78,0,84,0,82,0,82,0,115,0,101,0,93,0,87,0,84,0,90,0,82,0,96,0,82,0,86,0,105,0,125,0,145,0,154,0,159,0,136,0,87,0,39,255,226,255,185,255,184,255,216,255,244,0,28,
        0,64,0,100,0,129,0,169,0,209,1,27,1,108,1,219,2,72,2,178,3,15,3,66,3,96,3,128,3,139,3,146,3,175,3,185,3,192,3,213,3,244,3,249,3,250,4,6,4,29,4,45,4,73,4,110,4,166,5,0,5,66,5,130,5,178,5,214,6,10,6,50,6,76,6,97,6,101,6,123,6,126,6,129,6,98,6,75,6,19,5,
        221,5,137,5,65,4,225,4,146,4,77,4,0,3,189,3,125,3,75,3,28,3,5,2,215,2,199,2,176,2,164,2,120,2,66,1,241,1,151,1,65,0,234,0,163,0,126,0,66,0,24,255,230,255,178,255,109,255,37,254,250,254,196,254,182,254,187,254,206,254,205,254,228,254,241,254,243,254,252,
        255,18,255,51,255,113,255,167,255,202,255,223,255,239,0,4,0,6,0,7,255,255,255,241,255,224,255,212,255,201,255,180,255,166,255,154,255,174,255,198,255,252,0,33,0,65,0,65,0,35,255,246,255,220,255,166,255,123,255,76,255,47,255,0,254,246,254,232,254,206,
        254,184,254,138,254,70,254,2,253,186,253,112,253,46,252,214,252,142,252,92,252,49,252,28,252,2,251,216,251,193,251,166,251,136,251,104,251,75,251,42,251,25,251,27,251,42,251,67,251,104,251,166,251,255,252,99,252,196,253,33,253,121,253,202,254,13,254,
        67,254,108,254,135,254,163,254,181,254,181,254,167,254,149,254,119,254,98,254,77,254,56,254,33,254,27,254,30,254,48,254,56,254,65,254,76,254,77,254,91,254,113,254,135,254,171,254,217,255,14,255,73,255,98,255,124,255,117,255,113,255,83,255,77,255,65,255,
        39,255,43,255,54,255,57,255,66,255,66,255,50,255,15,254,223,254,164,254,105,254,55,254,6,253,228,253,210,253,185,253,164,253,154,253,141,253,135,253,113,253,94,253,99,253,109,253,123,253,112,253,98,253,88,253,81,253,88,253,103,253,134,253,153,253,172,
        253,182,253,199,253,207,253,224,254,0,254,29,254,59,254,98,254,139,254,187,254,223,255,30,255,75,255,140,255,210,0,8,0,69,0,100,0,115,0,139,0,152,0,161,0,176,0,194,0,204,0,224,0,249,1,29,1,51,1,92,1,133,1,157,1,183,1,200,1,197,1,180,1,165,1,140,1,125,
        1,121,1,89,1,64,1,31,0,238,0,181,0,118,0,54,0,3,255,228,255,228,255,245,0,24,0,68,0,94,0,91,0,72,0,54,0,51,0,54,0,46,0,54,0,79,0,120,0,159,0,170,0,186,0,175,0,183,0,188,0,215,0,224,0,233,0,219,0,206,0,148,0,91,0,36,0,0,255,241,255,249,0,22,0,68,0,127,
        0,198,0,255,1,85,1,147,1,234,2,34,2,85,2,154,2,219,3,28,3,96,3,159,3,183,3,220,3,251,4,38,4,58,4,55,4,41,4,29,4,18,4,9,4,9,4,20,4,74,4,126,4,202,5,27,5,81,5,142,5,173,5,183,5,202,5,213,5,230,5,237,5,228,5,201,5,183,5,144,5,122,5,91,5,43,4,228,4,144,4,
        40,3,200,3,118,3,24,2,200,2,104,2,27,1,200,1,125,1,76,1,33,1,21,1,10,1,20,1,28,0,244,0,183,0,97,0,17,255,181,255,102,255,50,255,15,254,247,254,238,254,235,254,229,254,211,254,195,254,169,254,134,254,117,254,101,254,88,254,110,254,159,254,195,254,250,
        255,48,255,94,255,122,255,144,255,165,255,178,255,174,255,173,255,173,255,165,255,140,255,134,255,129,255,130,255,140,255,178,255,219,255,232,255,248,0,6,0,14,0,29,0,24,0,0,255,248,255,234,255,198,255,149,255,98,255,65,255,21,254,211,254,152,254,88,254,
        16,253,203,253,171,253,132,253,100,253,85,253,82,253,88,253,98,253,74,253,48,252,243,252,215,252,199,252,183,252,187,252,168,252,160,252,138,252,133,252,106,252,71,252,54,252,60,252,73,252,103,252,142,252,197,253,28,253,130,253,221,254,40,254,92,254,
        134,254,173,254,216,255,0,255,30,255,54,255,58,255,70,255,61,255,43,255,16,254,249,254,229,254,213,254,200,254,187,254,167,254,130,254,103,254,87,254,70,254,83,254,110,254,128,254,139,254,142,254,133,254,123,254,121,254,116,254,108,254,115,254,138,254,
        180,254,202,254,227,254,241,254,249,254,235,254,223,254,192,254,180,254,152,254,134,254,102,254,69,254,41,253,249,253,211,253,186,253,166,253,125,253,94,253,70,253,63,253,60,253,46,253,63,253,81,253,102,253,116,253,125,253,132,253,113,253,112,253,124,
        253,142,253,190,253,249,254,40,254,94,254,138,254,157,254,170,254,184,254,213,254,246,255,52,255,112,255,165,255,224,0,19,0,50,0,57,0,93,0,118,0,130,0,148,0,150,0,141,0,159,0,186,0,213,0,242,1,17,1,60,1,108,1,150,1,189,1,202,1,193,1,187,1,169,1,150,1,
        125,1,99,1,68,1,45,1,18,0,251,0,212,0,180,0,134,0,90,0,68,0,32,255,255,255,223,255,198,255,178,255,195,255,191,255,201,255,202,255,220,255,234,0,6,0,58,0,104,0,126,0,151,0,190,0,217,1,0,1,21,1,36,1,35,1,29,1,27,1,22,1,20,1,27,0,252,0,241,0,219,0,194,
        0,155,0,141,0,158,0,204,1,21,1,79,1,100,1,133,1,171,1,191,1,222,1,254,2,30,2,74,2,153,2,229,3,41,3,118,3,156,3,195,3,231,3,253,4,20,4,37,4,33,4,37,4,56,4,48,4,63,4,78,4,84,4,98,4,109,4,130,4,155,4,199,4,242,5,8,5,11,4,239,4,218,4,204,4,185,4,166,4,141,
        4,113,4,76,4,31,3,236,3,160,3,67,2,240,2,142,2,66,1,219,1,140,1,60,0,238,0,159,0,97,0,57,0,32,0,24,0,12,0,6,255,237,255,199,255,136,255,77,255,18,254,223,254,198,254,178,254,167,254,157,254,157,254,138,254,123,254,112,254,69,254,62,254,78,254,92,254,
        115,254,134,254,156,254,187,254,232,255,21,255,62,255,104,255,112,255,119,255,108,255,90,255,72,255,45,255,36,255,50,255,88,255,137,255,163,255,195,255,209,255,248,0,15,0,18,0,26,0,50,0,79,0,80,0,71,0,30,255,230,255,172,255,97,255,18,254,196,254,139,
        254,102,254,67,254,54,254,22,253,243,253,196,253,160,253,132,253,112,253,113,253,117,253,152,253,189,253,210,253,220,253,213,253,192,253,181,253,177,253,179,253,196,253,210,253,211,253,221,253,222,253,211,253,213,253,220,253,221,253,221,253,229,253,238,
        254,5,254,19,254,56,254,99,254,141,254,171,254,200,254,245,255,16,255,65,255,119,255,158,255,181,255,206,255,214,255,224,255,228,255,212,255,194,255,151,255,111,255,65,255,16,254,216,254,170,254,131,254,127,254,134,254,141,254,141,254,134,254,120,254,
        105,254,91,254,83,254,77,254,96,254,137,254,167,254,193,254,207,254,214,254,227,254,213,254,202,254,177,254,139,254,109,254,73,254,45,254,27,253,249,253,218,253,195,253,177,253,174,253,166,253,160,253,152,253,136,253,130,253,110,253,125,253,142,253,167,
        253,217,253,240,253,239,253,243,253,240,253,253,254,19,254,52,254,102,254,142,254,184,254,245,255,27,255,83,255,112,255,134,255,167,255,187,255,226,255,248,0,35,0,78,0,116,0,147,0,168,0,180,0,186,0,199,0,216,0,227,0,231,0,255,1,14,1,31,1,50,1,75,1,114,
        1,135,1,144,1,140,1,122,1,89,1,56,1,24,0,253,0,233,0,202,0,177,0,150,0,132,0,116,0,100,0,101,0,91,0,76,0,48,0,6,255,232,255,172,255,129,255,113,255,129,255,165,255,208,0,6,0,40,0,62,0,102,0,141,0,204,0,240,1,21,1,65,1,101,1,125,1,130,1,141,1,144,1,136,
        1,158,1,172,1,190,1,176,1,191,1,204,1,204,1,204,1,186,1,171,1,136,1,137,1,160,1,183,1,197,1,225,1,250,2,28,2,52,2,86,2,146,2,185,2,228,3,48,3,96,3,136,3,186,3,243,4,70,4,121,4,170,4,195,4,189,4,167,4,148,4,130,4,90,4,54,4,29,4,26,4,30,4,51,4,69,4,73,
        4,87,4,96,4,96,4,74,4,33,3,240,3,164,3,113,3,62,3,35,2,236,2,192,2,142,2,85,2,18,1,172,1,86,1,0,0,170,0,104,0,39,255,238,255,173,255,116,255,81,255,50,255,41,255,16,254,242,254,199,254,160,254,130,254,90,254,73,254,56,254,55,254,62,254,70,254,87,254,
        85,254,98,254,109,254,126,254,133,254,139,254,137,254,121,254,134,254,148,254,155,254,184,254,200,254,211,254,231,254,235,254,234,254,223,254,209,254,221,254,246,255,14,255,68,255,113,255,152,255,185,255,214,255,242,0,8,0,11,0,7,0,22,0,26,0,8,255,228,
        255,187,255,148,255,102,255,30,254,243,254,191,254,156,254,124,254,105,254,80,254,67,254,54,254,51,254,62,254,59,254,48,254,44,254,34,254,22,254,15,254,9,254,24,254,45,254,65,254,102,254,141,254,177,254,203,254,234,255,7,255,26,255,33,255,12,254,249,
        254,241,254,228,254,223,254,207,254,180,254,171,254,160,254,149,254,124,254,105,254,113,254,131,254,174,254,227,255,23,255,79,255,140,255,188,255,219,255,227,255,235,255,230,255,227,255,220,255,210,255,187,255,160,255,130,255,88,255,39,254,241,254,184,
        254,144,254,116,254,96,254,83,254,80,254,92,254,113,254,124,254,128,254,120,254,115,254,106,254,88,254,78,254,80,254,90,254,95,254,112,254,128,254,146,254,138,254,119,254,63,254,4,253,218,253,192,253,181,253,175,253,174,253,177,253,189,253,207,253,206,
        253,184,253,175,253,168,253,163,253,156,253,157,253,152,253,170,253,184,253,208,253,244,254,22,254,52,254,112,254,169,254,217,255,11,255,45,255,99,255,144,255,165,255,184,255,205,255,237,0,12,0,43,0,75,0,107,0,129,0,175,0,206,0,231,0,255,1,11,1,18,1,
        38,1,54,1,54,1,50,1,38,1,31,1,11,1,11,1,14,1,20,1,29,1,32,1,11,0,231,0,186,0,143,0,104,0,89,0,71,0,73,0,76,0,76,0,72,0,61,0,39,0,8,255,238,255,224,255,228,255,213,255,209,255,188,255,188,255,198,255,230,0,19,0,58,0,111,0,127,0,175,0,220,0,233,1,33,1,
        71,1,126,1,176,1,225,2,3,2,32,2,46,2,56,2,63,2,77,2,88,2,84,2,85,2,79,2,74,2,45,2,21,1,247,1,243,1,243,1,237,1,244,2,6,2,39,2,89,2,132,2,150,2,171,2,192,2,230,3,6,3,34,3,98,3,160,3,214,4,6,4,31,4,36,4,42,4,27,4,12,4,0,3,242,3,236,3,229,3,221,3,210,3,
        186,3,161,3,141,3,123,3,113,3,88,3,60,3,27,2,233,2,165,2,103,2,52,1,240,1,180,1,146,1,107,1,79,1,57,1,17,0,215,0,155,0,82,255,253,255,177,255,115,255,57,255,12,254,221,254,199,254,169,254,153,254,128,254,98,254,94,254,74,254,66,254,58,254,65,254,58,254,
        56,254,56,254,66,254,94,254,113,254,139,254,166,254,184,254,206,254,207,254,203,254,187,254,173,254,156,254,119,254,110,254,110,254,130,254,146,254,171,254,210,255,0,255,27,255,48,255,65,255,84,255,113,255,147,255,159,255,160,255,174,255,173,255,170,
        255,145,255,126,255,117,255,129,255,156,255,177,255,183,255,187,255,160,255,127,255,73,255,11,254,228,254,193,254,191,254,211,254,221,254,223,254,205,254,196,254,182,254,171,254,153,254,141,254,135,254,152,254,175,254,203,254,223,254,239,255,16,255,51,
        255,73,255,94,255,112,255,140,255,169,255,174,255,170,255,160,255,133,255,99,255,66,255,27,255,3,254,235,254,234,254,227,254,217,254,218,254,221,254,228,254,241,255,3,255,26,255,45,255,61,255,65,255,75,255,84,255,99,255,122,255,127,255,126,255,124,255,
        113,255,102,255,73,255,57,255,36,255,9,254,225,254,175,254,133,254,102,254,83,254,85,254,98,254,109,254,113,254,121,254,106,254,106,254,95,254,70,254,40,254,26,254,11,254,8,254,0,254,8,254,22,254,58,254,62,254,62,254,52,254,27,253,254,253,222,253,195,
        253,178,253,177,253,181,253,192,253,215,253,228,253,247,253,251,253,249,253,250,253,254,254,4,254,11,254,29,254,62,254,106,254,155,254,202,255,0,255,48,255,81,255,124,255,162,255,213,255,250,0,33,0,53,0,80,0,101,0,127,0,157,0,186,0,205,0,231,1,4,1,25,
        1,49,1,67,1,75,1,87,1,87,1,81,1,68,1,47,1,39,1,27,1,10,0,244,0,223,0,191,0,154,0,127,0,115,0,105,0,101,0,102,0,104,0,94,0,79,0,50,0,14,255,242,255,226,255,221,255,232,255,244,0,24,0,40,0,53,0,51,0,51,0,36,0,18,0,25,0,42,0,65,0,97,0,141,0,175,0,223,0,
        241,1,21,1,69,1,111,1,150,1,168,1,205,1,245,2,25,2,66,2,88,2,107,2,100,2,118,2,117,2,107,2,85,2,70,2,57,2,42,2,34,2,6,2,12,2,31,2,46,2,78,2,93,2,113,2,135,2,154,2,181,2,200,2,223,2,240,2,246,2,255,3,10,3,27,3,60,3,74,3,95,3,121,3,150,3,167,3,177,3,170,
        3,164,3,168,3,172,3,165,3,132,3,96,3,34,2,229,2,174,2,120,2,66,2,23,1,240,1,186,1,140,1,87,1,32,0,241,0,197,0,147,0,120,0,98,0,73,0,37,0,4,255,214,255,181,255,123,255,75,255,8,254,229,254,189,254,153,254,144,254,135,254,127,254,134,254,133,254,119,254,
        108,254,98,254,102,254,110,254,119,254,116,254,109,254,105,254,96,254,108,254,138,254,151,254,169,254,200,254,214,254,211,254,191,254,157,254,133,254,133,254,148,254,175,254,205,255,0,255,61,255,102,255,108,255,115,255,112,255,97,255,80,255,52,255,22,
        255,8,254,245,255,4,255,15,255,32,255,45,255,47,255,66,255,84,255,108,255,136,255,149,255,162,255,162,255,152,255,145,255,133,255,116,255,95,255,75,255,73,255,63,255,68,255,59,255,54,255,66,255,76,255,90,255,109,255,129,255,140,255,141,255,133,255,119,
        255,126,255,137,255,149,255,160,255,173,255,210,255,235,255,246,255,244,255,219,255,185,255,163,255,142,255,126,255,105,255,111,255,105,255,102,255,102,255,87,255,73,255,50,255,25,254,253,254,211,254,203,254,207,254,224,254,241,254,252,255,12,255,14,
        255,33,255,43,255,45,255,40,255,23,255,21,255,18,255,12,255,5,254,247,254,224,254,202,254,162,254,123,254,85,254,62,254,44,254,41,254,56,254,67,254,92,254,106,254,103,254,87,254,59,254,24,254,12,254,1,254,12,254,40,254,81,254,120,254,139,254,152,254,
        128,254,91,254,69,254,38,254,6,253,233,253,236,253,231,253,246,254,16,254,41,254,69,254,85,254,92,254,131,254,142,254,163,254,174,254,213,255,1,255,23,255,51,255,98,255,162,255,227,0,3,0,43,0,79,0,108,0,139,0,173,0,190,0,204,0,229,0,244,0,251,1,6,1,7,
        1,9,1,25,1,39,1,49,1,68,1,90,1,104,1,97,1,87,1,51,1,9,0,224,0,195,0,169,0,147,0,144,0,130,0,127,0,119,0,101,0,80,0,47,0,7,255,231,255,206,255,203,255,210,255,212,255,231,255,235,255,235,255,253,0,1,0,17,0,32,0,58,0,84,0,101,0,114,0,123,0,130,0,148,0,
        177,0,205,0,244,1,28,1,60,1,85,1,97,1,110,1,129,1,166,1,202,1,236,2,0,2,24,2,59,2,81,2,96,2,104,2,96,2,89,2,81,2,95,2,99,2,118,2,128,2,139,2,149,2,128,2,129,2,140,2,142,2,158,2,175,2,172,2,158,2,139,2,121,2,121,2,110,2,103,2,131,2,160,2,192,2,221,2,248,
        3,28,3,62,3,92,3,116,3,116,3,107,3,75,3,46,2,241,2,199,2,153,2,111,2,59,1,250,1,183,1,115,1,60,1,15,0,224,0,184,0,150,0,98,0,69,0,26,255,226,255,180,255,129,255,109,255,95,255,93,255,84,255,66,255,45,255,29,255,7,254,245,254,210,254,198,254,181,254,174,
        254,171,254,175,254,175,254,162,254,149,254,144,254,139,254,138,254,127,254,137,254,126,254,103,254,70,254,40,254,40,254,54,254,67,254,101,254,139,254,184,254,225,254,247,254,249,254,253,255,5,255,8,255,5,254,249,254,249,255,9,255,15,255,12,254,242,254,
        229,254,210,254,189,254,177,254,187,254,195,254,193,254,211,254,228,254,236,254,250,255,19,255,65,255,117,255,156,255,185,255,210,255,223,255,214,255,201,255,181,255,160,255,160,255,170,255,188,255,202,255,219,255,219,255,241,255,245,255,245,255,231,
        255,227,255,223,255,220,255,208,255,203,255,196,255,199,255,206,255,226,255,231,255,238,255,224,255,224,255,224,255,228,255,224,255,235,255,235,255,230,255,231,255,230,255,226,255,205,255,178,255,142,255,108,255,66,255,19,254,241,254,223,254,199,254,
        181,254,170,254,167,254,187,254,187,254,192,254,203,254,223,254,231,254,250,254,252,254,253,255,3,254,254,255,1,254,246,254,235,254,203,254,169,254,130,254,102,254,69,254,51,254,38,254,41,254,38,254,52,254,60,254,69,254,85,254,92,254,108,254,108,254,
        113,254,109,254,110,254,123,254,146,254,159,254,192,254,214,254,216,254,207,254,187,254,156,254,113,254,90,254,56,254,55,254,52,254,65,254,85,254,131,254,180,254,209,254,252,255,16,255,40,255,54,255,83,255,129,255,167,255,224,0,14,0,46,0,86,0,111,0,137,
        0,165,0,181,0,187,0,195,0,208,0,217,0,233,0,240,0,241,0,251,0,253,1,6,1,13,1,17,1,9,0,255,0,238,0,235,0,222,0,217,0,204,0,193,0,180,0,180,0,170,0,166,0,169,0,151,0,134,0,90,0,35,255,250,255,205,255,191,255,188,255,190,255,195,255,199,255,192,255,187,
        255,188,255,191,255,206,255,232,0,15,0,50,0,101,0,150,0,191,0,217,0,247,1,10,1,7,1,2,0,252,0,252,1,20,1,43,1,75,1,105,1,125,1,137,1,151,1,161,1,191,1,227,1,255,2,3,2,14,2,27,2,48,2,84,2,100,2,113,2,131,2,158,2,175,2,169,2,174,2,157,2,162,2,144,2,125,
        2,78,2,45,2,34,2,5,1,254,1,240,1,254,2,2,2,21,2,46,2,45,2,71,2,96,2,115,2,158,2,179,2,192,2,194,2,179,2,176,2,139,2,115,2,86,2,57,2,20,1,230,1,178,1,126,1,82,1,25,0,230,0,173,0,114,0,76,0,33,255,253,255,212,255,160,255,120,255,94,255,63,255,45,255,19,
        255,15,255,9,255,9,255,14,255,21,255,15,255,15,255,0,254,236,254,232,254,228,254,238,254,231,254,221,254,203,254,182,254,146,254,124,254,101,254,103,254,108,254,109,254,119,254,110,254,92,254,74,254,74,254,84,254,110,254,157,254,206,255,7,255,41,255,
        55,255,62,255,54,255,41,255,12,254,236,254,206,254,185,254,184,254,184,254,175,254,175,254,185,254,195,254,200,254,213,254,217,254,228,254,216,254,242,255,1,255,21,255,29,255,61,255,98,255,144,255,188,255,216,255,232,255,244,0,3,0,4,255,246,255,237,255,
        246,0,3,0,29,0,50,0,55,0,53,0,37,0,25,0,22,0,10,0,8,0,1,0,1,0,10,0,7,0,3,0,0,255,252,255,235,255,230,255,228,255,239,0,7,0,28,0,37,0,46,0,39,0,33,0,8,255,252,255,221,255,199,255,180,255,155,255,141,255,120,255,99,255,70,255,43,255,11,254,229,254,207,
        254,188,254,175,254,167,254,169,254,178,254,192,254,213,254,234,254,252,254,247,254,241,254,235,254,227,254,214,254,206,254,189,254,177,254,164,254,144,254,141,254,110,254,90,254,58,254,31,254,20,254,33,254,51,254,90,254,121,254,157,254,193,254,216,254,
        221,254,218,254,210,254,221,254,224,254,223,254,245,255,4,255,22,255,32,255,30,255,9,254,250,254,224,254,207,254,198,254,198,254,199,254,211,254,250,255,43,255,75,255,111,255,152,255,203,255,227,255,253,0,21,0,47,0,72,0,97,0,123,0,145,0,159,0,173,0,183,
        0,184,0,190,0,183,0,183,0,199,0,219,0,251,1,20,1,28,1,20,0,252,0,224,0,198,0,166,0,141,0,129,0,126,0,133,0,154,0,172,0,195,0,206,0,198,0,176,0,133,0,90,0,43,0,14,255,246,255,232,255,223,255,206,255,194,255,187,255,163,255,160,255,159,255,165,255,172,
        255,196,255,220,0,12,0,62,0,120,0,166,0,206,0,249,1,17,1,32,1,36,1,43,1,57,1,64,1,79,1,78,1,76,1,71,1,81,1,86,1,97,1,123,1,135,1,150,1,157,1,176,1,197,1,225,1,245,2,9,2,42,2,75,2,118,2,140,2,143,2,149,2,151,2,157,2,161,2,124,2,100,2,67,2,36,2,3,1,232,
        1,205,1,194,1,197,1,196,1,211,1,215,1,244,2,17,2,42,2,61,2,61,2,61,2,46,2,20,2,10,1,240,1,230,1,211,1,197,1,178,1,164,1,143,1,123,1,96,1,74,1,35,0,253,0,216,0,169,0,104,0,29,255,202,255,136,255,93,255,69,255,50,255,41,255,41,255,33,255,29,255,12,254,
        239,254,220,254,202,254,205,254,214,254,236,254,245,255,7,255,19,255,18,255,12,254,239,254,206,254,170,254,133,254,94,254,78,254,60,254,40,254,37,254,41,254,63,254,98,254,116,254,139,254,157,254,175,254,185,254,191,254,209,254,220,254,229,254,232,254,
        245,254,246,254,234,254,232,254,214,254,193,254,173,254,173,254,171,254,166,254,156,254,156,254,173,254,180,254,210,254,231,254,247,255,15,255,33,255,52,255,76,255,101,255,117,255,141,255,177,255,196,255,227,255,245,0,14,0,36,0,46,0,48,0,57,0,65,0,80,
        0,86,0,84,0,82,0,80,0,80,0,73,0,64,0,60,0,53,0,53,0,57,0,53,0,44,0,50,0,53,0,61,0,44,0,37,0,35,0,39,0,50,0,57,0,75,0,89,0,91,0,68,0,36,0,0,255,228,255,201,255,190,255,181,255,173,255,174,255,170,255,154,255,129,255,104,255,76,255,48,255,27,254,253,254,
        238,254,234,254,234,254,229,254,234,254,236,254,234,254,229,254,217,254,203,254,198,254,189,254,199,254,202,254,207,254,207,254,192,254,162,254,139,254,121,254,123,254,112,254,108,254,117,254,119,254,121,254,145,254,157,254,173,254,202,254,234,255,4,
        255,29,255,44,255,48,255,62,255,65,255,51,255,43,255,44,255,43,255,48,255,51,255,55,255,57,255,51,255,45,255,51,255,63,255,70,255,102,255,138,255,177,255,220,255,252,0,28,0,46,0,65,0,89,0,98,0,111,0,122,0,132,0,147,0,162,0,176,0,190,0,195,0,201,0,199,
        0,205,0,212,0,209,0,206,0,209,0,204,0,183,0,172,0,145,0,123,0,109,0,97,0,108,0,114,0,144,0,159,0,162,0,162,0,137,0,115,0,75,0,51,0,29,0,17,255,255,255,234,255,212,255,180,255,167,255,162,255,156,255,160,255,173,255,181,255,206,255,238,0,24,0,65,0,104,
        0,133,0,166,0,188,0,217,0,224,0,249,1,15,1,47,1,78,1,100,1,108,1,119,1,126,1,123,1,125,1,119,1,107,1,107,1,122,1,144,1,155,1,179,1,191,1,226,1,233,1,248,2,0,2,5,2,17,2,38,2,64,2,81,2,81,2,81,2,64,2,54,2,23,1,252,1,250,1,247,1,241,1,227,1,214,1,208,1,
        198,1,190,1,187,1,191,1,202,1,208,1,205,1,189,1,176,1,151,1,114,1,89,1,61,1,45,1,42,1,21,1,18,1,24,1,25,1,15,1,11,0,255,0,231,0,186,0,130,0,68,255,252,255,195,255,159,255,129,255,102,255,72,255,51,255,37,255,21,255,18,255,11,254,247,254,238,254,232,254,
        234,254,223,254,223,254,220,254,216,254,221,254,220,254,218,254,210,254,187,254,188,254,157,254,126,254,83,254,59,254,29,254,23,254,19,254,23,254,54,254,87,254,126,254,148,254,160,254,175,254,180,254,185,254,187,254,187,254,196,254,203,254,207,254,217,
        254,228,254,242,254,236,254,236,254,238,254,249,254,247,254,245,254,228,254,220,254,217,254,224,254,231,255,0,255,23,255,62,255,94,255,120,255,144,255,180,255,199,255,232,0,6,0,21,0,48,0,62,0,73,0,90,0,87,0,91,0,98,0,101,0,107,0,109,0,109,0,108,0,101,
        0,87,0,86,0,87,0,86,0,90,0,83,0,78,0,80,0,84,0,93,0,109,0,111,0,109,0,97,0,87,0,79,0,72,0,55,0,57,0,57,0,44,0,43,0,37,0,17,255,248,255,228,255,205,255,198,255,187,255,173,255,166,255,162,255,154,255,145,255,131,255,117,255,106,255,87,255,72,255,45,255,
        9,254,245,254,224,254,213,254,211,254,203,254,189,254,187,254,188,254,196,254,193,254,202,254,209,254,207,254,191,254,187,254,181,254,180,254,174,254,162,254,171,254,177,254,181,254,175,254,175,254,170,254,175,254,203,254,235,255,21,255,57,255,91,255,
        120,255,124,255,124,255,104,255,80,255,69,255,58,255,55,255,63,255,73,255,106,255,126,255,151,255,167,255,176,255,174,255,188,255,212,255,246,0,11,0,36,0,47,0,60,0,66,0,71,0,71,0,72,0,83,0,86,0,94,0,118,0,133,0,139,0,161,0,175,0,180,0,188,0,180,0,166,
        0,148,0,139,0,129,0,120,0,132,0,129,0,140,0,143,0,151,0,151,0,136,0,127,0,115,0,104,0,87,0,79,0,66,0,64,0,55,0,50,0,30,0,17,255,245,255,216,255,191,255,180,255,184,255,188,255,208,255,228,255,250,0,19,0,42,0,64,0,76,0,93,0,116,0,130,0,145,0,173,0,208,
        0,231,1,10,1,36,1,53,1,81,1,107,1,126,1,139,1,151,1,165,1,169,1,164,1,162,1,146,1,141,1,140,1,154,1,165,1,171,1,176,1,179,1,168,1,165,1,168,1,171,1,182,1,183,1,196,1,205,1,211,1,200,1,193,1,183,1,178,1,173,1,191,1,209,1,208,1,200,1,205,1,194,1,171,1,
        148,1,123,1,99,1,64,1,35,1,7,0,241,0,222,0,206,0,199,0,184,0,177,0,165,0,159,0,163,0,155,0,157,0,155,0,137,0,105,0,73,0,32,0,3,255,230,255,209,255,188,255,173,255,154,255,123,255,79,255,47,255,25,255,11,255,15,255,25,255,37,255,41,255,30,255,14,254,249,
        254,218,254,187,254,177,254,166,254,163,254,162,254,163,254,151,254,139,254,130,254,105,254,91,254,76,254,66,254,67,254,76,254,96,254,117,254,108,254,102,254,92,254,95,254,102,254,120,254,141,254,159,254,189,254,206,254,217,254,216,254,231,254,245,255,
        12,255,18,255,27,255,37,255,48,255,39,255,26,255,3,254,246,255,3,255,5,255,29,255,50,255,83,255,122,255,159,255,196,255,230,0,0,0,32,0,58,0,84,0,109,0,134,0,145,0,145,0,143,0,130,0,136,0,136,0,136,0,144,0,162,0,159,0,150,0,143,0,134,0,134,0,122,0,118,
        0,133,0,139,0,130,0,130,0,130,0,137,0,140,0,133,0,129,0,125,0,107,0,97,0,66,0,43,0,21,255,250,255,231,255,245,255,241,255,250,0,10,0,12,0,18,255,250,255,230,255,202,255,183,255,170,255,156,255,155,255,148,255,147,255,134,255,112,255,91,255,65,255,41,
        255,1,254,220,254,184,254,159,254,139,254,144,254,169,254,188,254,210,254,224,254,224,254,216,254,200,254,205,254,205,254,209,254,211,254,217,254,232,254,232,254,235,254,238,254,249,254,254,255,7,255,14,255,25,255,33,255,57,255,81,255,102,255,123,255,
        126,255,133,255,140,255,129,255,124,255,126,255,137,255,141,255,156,255,181,255,196,255,220,255,244,0,7,0,29,0,39,0,30,0,25,0,15,0,22,0,22,0,19,0,29,0,42,0,47,0,65,0,75,0,82,0,87,0,87,0,94,0,96,0,102,0,109,0,115,0,120,0,126,0,118,0,111,0,104,0,109,0,
        107,0,109,0,122,0,122,0,127,0,137,0,133,0,126,0,102,0,68,0,58,0,43,0,33,0,26,0,26,0,22,0,14,255,252,255,238,255,238,255,246,0,3,0,8,0,15,0,32,0,42,0,47,0,55,0,61,0,66,0,79,0,86,0,104,0,116,0,139,0,163,0,201,0,222,1,3,1,42,1,71,1,87,1,108,1,128,1,158,
        1,179,1,196,1,205,1,211,1,215,1,204,1,200,1,175,1,153,1,122,1,97,1,83,1,75,1,83,1,90,1,105,1,107,1,119,1,122,1,123,1,126,1,118,1,129,1,137,1,157,1,175,1,172,1,182,1,178,1,173,1,173,1,166,1,161,1,150,1,126,1,82,1,36,0,238,0,188,0,154,0,122,0,118,0,108,
        0,107,0,107,0,108,0,105,0,105,0,89,0,66,0,54,0,47,0,33,0,32,0,24,0,25,0,24,0,10,0,8,255,249,255,221,255,185,255,141,255,106,255,79,255,65,255,54,255,58,255,55,255,45,255,30,255,21,255,14,254,234,254,209,254,184,254,169,254,138,254,123,254,117,254,113,
        254,116,254,115,254,119,254,119,254,121,254,121,254,108,254,101,254,96,254,92,254,80,254,60,254,54,254,41,254,30,254,45,254,70,254,108,254,137,254,167,254,193,254,220,254,246,255,7,255,25,255,41,255,57,255,63,255,59,255,51,255,45,255,40,255,58,255,73,
        255,97,255,117,255,140,255,169,255,188,255,198,255,206,255,224,255,249,0,24,0,55,0,89,0,125,0,151,0,169,0,175,0,162,0,157,0,144,0,150,0,155,0,172,0,186,0,184,0,195,0,195,0,190,0,198,0,193,0,202,0,199,0,191,0,177,0,170,0,166,0,158,0,155,0,151,0,137,0,
        129,0,118,0,101,0,82,0,60,0,40,0,26,0,14,0,7,255,253,255,244,255,248,0,3,0,19,0,33,0,37,0,35,0,29,0,11,255,246,255,231,255,213,255,199,255,188,255,167,255,149,255,131,255,102,255,79,255,55,255,27,255,0,254,225,254,193,254,199,254,193,254,203,254,217,
        254,216,254,223,254,225,254,228,254,228,254,229,254,243,254,246,254,253,255,15,255,23,255,43,255,62,255,73,255,84,255,99,255,95,255,104,255,98,255,98,255,93,255,97,255,93,255,109,255,116,255,138,255,152,255,167,255,181,255,187,255,195,255,188,255,192,
        255,199,255,210,255,226,255,235,255,249,0,0,0,8,0,17,0,6,255,253,255,253,255,255,255,252,0,7,0,11,0,26,0,30,0,50,0,61,0,68,0,72,0,72,0,68,0,64,0,53,0,62,0,60,0,71,0,80,0,98,0,104,0,101,0,109,0,108,0,102,0,94,0,76,0,60,0,55,0,35,0,35,0,32,0,22,0,21,0,
        11,255,252,255,241,255,231,255,237,255,239,255,255,0,19,0,36,0,48,0,47,0,50,0,46,0,32,0,24,0,29,0,36,0,46,0,55,0,82,0,108,0,139,0,162,0,191,0,215,0,238,1,10,1,36,1,76,1,112,1,150,1,176,1,184,1,187,1,194,1,193,1,191,1,193,1,189,1,178,1,157,1,135,1,103,
        1,71,1,54,1,43,1,45,1,36,1,46,1,47,1,45,1,46,1,49,1,65,1,93,1,94,1,105,1,126,1,140,1,132,1,128,1,136,1,130,1,118,1,92,1,74,1,36,1,0,0,220,0,188,0,150,0,112,0,79,0,51,0,42,0,28,0,24,0,8,0,1,255,252,255,244,255,234,255,235,255,224,255,219,255,223,255,238,
        255,245,0,3,0,7,0,3,255,231,255,210,255,191,255,178,255,154,255,137,255,122,255,98,255,83,255,68,255,43,255,22,254,253,254,234,254,211,254,195,254,192,254,170,254,167,254,151,254,139,254,128,254,117,254,108,254,109,254,120,254,123,254,135,254,127,254,
        108,254,102,254,94,254,81,254,87,254,77,254,77,254,80,254,92,254,92,254,115,254,127,254,155,254,185,254,216,254,246,255,19,255,37,255,51,255,52,255,54,255,58,255,51,255,61,255,83,255,119,255,163,255,203,255,235,255,250,0,7,0,0,255,241,255,238,255,245,
        0,6,0,17,0,36,0,72,0,102,0,126,0,145,0,151,0,159,0,162,0,169,0,170,0,170,0,170,0,183,0,199,0,220,0,216,0,227,0,222,0,229,0,238,0,229,0,216,0,204,0,181,0,172,0,165,0,150,0,148,0,130,0,111,0,93,0,76,0,62,0,46,0,36,0,30,0,26,0,22,0,19,0,14,0,26,0,25,0,26,
        0,21,0,21,0,22,0,33,0,36,0,30,0,15,255,253,255,224,255,194,255,163,255,133,255,116,255,95,255,88,255,77,255,65,255,54,255,44,255,33,255,18,255,1,254,243,254,220,254,214,254,213,254,214,254,227,254,239,255,4,255,16,255,32,255,50,255,63,255,76,255,98,255,
        119,255,147,255,159,255,170,255,190,255,187,255,188,255,159,255,160,255,159,255,155,255,158,255,165,255,169,255,185,255,206,255,234,255,242,0,0,255,252,255,234,255,223,255,212,255,202,255,201,255,194,255,212,255,230,255,249,0,11,0,24,0,18,0,11,0,1,255,
        248,255,250,0,10,0,17,0,30,0,57,0,66,0,71,0,79,0,66,0,64,0,54,0,40,0,40,0,36,0,48,0,75,0,87,0,97,0,97,0,98,0,82,0,62,0,36,0,11,255,253,255,249,0,10,0,29,0,36,0,29,0,19,0,3,255,245,255,235,255,237,255,241,255,250,0,6,0,19,0,28,0,25,0,32,0,35,0,22,0,29,
        0,28,0,28,0,42,0,53,0,73,0,93,0,112,0,145,0,163,0,188,0,220,0,252,1,29,1,51,1,68,1,82,1,100,1,114,1,121,1,132,1,139,1,146,1,137,1,130,1,119,1,110,1,101,1,90,1,76,1,50,1,36,1,7,0,249,0,249,0,242,0,242,0,255,0,252,1,11,1,13,1,31,1,32,1,49,1,54,1,53,1,56,
        1,57,1,56,1,38,1,24,1,6,0,242,0,226,0,198,0,177,0,150,0,134,0,109,0,73,0,35,0,7,255,241,255,224,255,210,255,202,255,195,255,196,255,192,255,202,255,212,255,213,255,231,255,230,255,231,255,220,255,214,255,209,255,203,255,201,255,191,255,180,255,180,255,
        172,255,154,255,133,255,102,255,73,255,39,255,5,254,235,254,207,254,191,254,185,254,188,254,195,254,181,254,171,254,170,254,153,254,135,254,113,254,91,254,87,254,87,254,92,254,106,254,124,254,133,254,127,254,128,254,113,254,103,254,94,254,95,254,112,
        254,127,254,151,254,169,254,191,254,210,254,223,254,246,255,4,255,37,255,44,255,51,255,61,255,81,255,97,255,133,255,173,255,210,255,248,0,24,0,40,0,43,0,40,0,25,0,14,0,12,0,14,0,24,0,35,0,62,0,87,0,101,0,111,0,136,0,150,0,151,0,154,0,155,0,165,0,177,
        0,193,0,206,0,230,0,229,0,230,0,235,0,234,0,227,0,222,0,209,0,215,0,206,0,194,0,179,0,166,0,141,0,122,0,104,0,96,0,84,0,76,0,64,0,58,0,53,0,50,0,48,0,42,0,29,0,21,0,11,0,1,255,246,255,235,255,232,255,242,255,244,255,248,255,253,255,242,255,221,255,212,
        255,184,255,165,255,149,255,138,255,129,255,126,255,126,255,134,255,129,255,113,255,99,255,75,255,54,255,40,255,22,255,12,254,254,254,253,255,14,255,23,255,25,255,47,255,69,255,99,255,131,255,147,255,176,255,202,255,214,255,230,255,232,255,241,255,248,
        255,249,0,11,0,11,0,14,0,12,255,245,255,238,255,237,255,232,255,224,255,221,255,228,255,245,255,248,255,235,255,231,255,227,255,223,255,213,255,209,255,195,255,201,255,208,255,216,255,237,255,246,0,0,0,6,0,4,0,4,0,7,0,10,0,22,0,22,0,36,0,51,0,54,0,57,
        0,61,0,64,0,61,0,55,0,54,0,62,0,61,0,60,0,54,0,47,0,33,0,24,0,7,0,6,0,15,0,15,0,15,0,26,0,29,0,24,0,18,0,12,0,3,0,4,255,249,255,253,0,3,0,7,0,10,0,8,0,6,0,6,0,10,0,17,0,35,0,44,0,61,0,89,0,93,0,101,0,114,0,126,0,145,0,169,0,183,0,206,0,244,1,14,1,29,
        1,45,1,45,1,38,1,43,1,54,1,58,1,63,1,57,1,63,1,69,1,74,1,86,1,79,1,81,1,74,1,61,1,36,1,17,0,244,0,231,0,219,0,220,0,226,0,238,0,235,0,234,0,233,0,229,0,231,0,241,0,241,0,253,0,245,0,240,0,240,0,230,0,226,0,206,0,198,0,183,0,169,0,145,0,126,0,107,0,73,
        0,48,0,26,255,242,255,213,255,201,255,190,255,185,255,184,255,185,255,180,255,190,255,206,255,209,255,201,255,187,255,166,255,165,255,156,255,165,255,162,255,165,255,180,255,178,255,167,255,159,255,142,255,112,255,91,255,61,255,29,255,1,254,229,254,214,
        254,188,254,171,254,169,254,153,254,151,254,146,254,135,254,126,254,113,254,112,254,109,254,102,254,96,254,102,254,126,254,135,254,145,254,148,254,145,254,144,254,149,254,151,254,138,254,146,254,155,254,167,254,188,254,196,254,207,254,224,254,235,255,
        7,255,34,255,61,255,91,255,129,255,165,255,201,255,226,255,241,0,0,0,8,0,19,0,17,0,17,0,33,0,37,0,47,0,53,0,62,0,69,0,79,0,97,0,91,0,105,0,116,0,118,0,129,0,137,0,147,0,162,0,177,0,186,0,195,0,195,0,195,0,205,0,201,0,208,0,205,0,204,0,206,0,198,0,191,
        0,186,0,187,0,175,0,168,0,152,0,133,0,111,0,107,0,104,0,100,0,84,0,76,0,55,0,43,0,28,0,17,0,7,0,10,255,248,255,248,255,241,255,242,255,234,255,219,255,212,255,202,255,191,255,185,255,185,255,169,255,170,255,174,255,177,255,181,255,188,255,187,255,174,
        255,160,255,151,255,134,255,124,255,105,255,104,255,94,255,73,255,61,255,50,255,45,255,44,255,79,255,81,255,93,255,113,255,133,255,160,255,187,255,209,255,234,255,244,255,252,0,15,0,40,0,55,0,71,0,89,0,80,0,83,0,64,0,44,0,17,255,253,255,239,255,228,255,
        221,255,216,255,221,255,219,255,234,255,237,255,234,255,230,255,220,255,220,255,212,255,198,255,191,255,194,255,206,255,212,255,238,255,250,0,12,0,24,0,25,0,22,0,19,0,14,0,11,0,15,0,25,0,35,0,43,0,51,0,61,0,61,0,53,0,40,0,21,0,12,0,3,255,253,0,6,0,14,
        0,32,0,26,0,25,0,22,0,22,0,6,255,255,255,252,0,0,0,0,0,3,0,7,0,7,0,11,0,6,0,1,255,255,255,250,255,249,255,255,0,18,0,39,0,53,0,84,0,107,0,120,0,127,0,137,0,150,0,158,0,173,0,186,0,197,0,217,0,229,0,244,0,255,1,4,1,17,1,27,1,27,1,28,1,15,1,15,1,25,1,38,
        1,42,1,57,1,58,1,63,1,51,1,40,1,29,1,17,0,255,0,237,0,226,0,216,0,209,0,205,0,201,0,193,0,187,0,175,0,169,0,166,0,162,0,162,0,162,0,165,0,168,0,168,0,173,0,172,0,158,0,150,0,139,0,133,0,129,0,123,0,114,0,100,0,72,0,47,0,3,255,227,255,203,255,192,255,
        195,255,185,255,187,255,180,255,173,255,170,255,162,255,152,255,145,255,138,255,134,255,133,255,129,255,144,255,148,255,156,255,147,255,145,255,141,255,136,255,115,255,81,255,57,255,33,255,4,254,231,254,203,254,182,254,171,254,170,254,174,254,173,254,
        171,254,166,254,163,254,151,254,134,254,127,254,127,254,133,254,139,254,141,254,151,254,151,254,156,254,152,254,156,254,167,254,177,254,184,254,196,254,207,254,205,254,216,254,227,254,241,255,4,255,30,255,57,255,97,255,130,255,169,255,191,255,206,255,
        210,255,213,255,210,255,213,255,216,255,220,255,234,0,11,0,37,0,68,0,90,0,102,0,102,0,119,0,120,0,118,0,122,0,109,0,104,0,120,0,127,0,143,0,151,0,162,0,175,0,186,0,191,0,188,0,191,0,198,0,206,0,212,0,220,0,226,0,216,0,205,0,195,0,180,0,172,0,162,0,157,
        0,151,0,148,0,133,0,119,0,107,0,89,0,73,0,65,0,47,0,33,0,15,0,4,255,255,255,246,255,241,255,235,255,223,255,208,255,210,255,202,255,195,255,184,255,181,255,177,255,173,255,174,255,177,255,173,255,188,255,191,255,192,255,198,255,194,255,184,255,173,255,
        166,255,158,255,154,255,154,255,142,255,134,255,127,255,123,255,126,255,116,255,129,255,129,255,144,255,158,255,177,255,195,255,206,255,228,255,249,0,14,0,36,0,51,0,69,0,83,0,79,0,82,0,79,0,66,0,58,0,51,0,42,0,24,0,4,255,244,255,238,255,234,255,227,255,
        227,255,228,255,231,255,228,255,221,255,216,255,216,255,205,255,196,255,191,255,192,255,192,255,203,255,214,255,235,255,245,0,3,0,17,0,14,0,14,255,253,255,253,255,252,255,244,255,250,0,7,0,17,0,26,0,24,0,26,0,18,0,14,0,8,0,4,0,0,255,255,0,3,0,0,0,1,0,
        0,255,255,0,1,0,1,0,0,0,7,0,8,0,11,0,14,0,17,0,11,0,3,0,7,0,1,0,1,0,11,0,17,0,37,0,51,0,60,0,68,0,78,0,87,0,116,0,137,0,150,0,163,0,173,0,179,0,186,0,197,0,195,0,199,0,209,0,220,0,219,0,229,0,222,0,231,0,231,0,241,0,242,0,247,1,0,1,3,0,253,1,4,1,13,1,
        13,1,13,1,3,1,4,1,0,0,235,0,216,0,202,0,197,0,180,0,172,0,162,0,147,0,133,0,114,0,105,0,89,0,80,0,78,0,72,0,84,0,101,0,114,0,118,0,122,0,130,0,125,0,118,0,107,0,93,0,71,0,57,0,40,0,21,255,252,255,220,255,198,255,181,255,173,255,159,255,163,255,156,255,
        148,255,140,255,129,255,120,255,109,255,105,255,104,255,102,255,104,255,112,255,109,255,112,255,111,255,101,255,90,255,68,255,47,255,29,255,8,254,254,254,252,254,239,254,235,254,227,254,223,254,216,254,213,254,216,254,210,254,196,254,184,254,167,254,
        149,254,137,254,130,254,128,254,138,254,135,254,135,254,139,254,149,254,160,254,178,254,195,254,209,254,220,254,232,254,241,254,253,255,1,255,14,255,39,255,59,255,94,255,120,255,138,255,166,255,183,255,194,255,199,255,202,255,201,255,212,255,216,255,
        221,255,245,0,7,0,26,0,55,0,97,0,134,0,157,0,172,0,172,0,161,0,145,0,123,0,108,0,105,0,111,0,129,0,152,0,172,0,206,0,223,0,230,0,224,0,223,0,209,0,197,0,194,0,188,0,197,0,191,0,197,0,195,0,201,0,184,0,188,0,184,0,179,0,166,0,150,0,137,0,129,0,108,0,91,
        0,78,0,66,0,57,0,46,0,33,0,22,0,7,255,249,255,237,255,216,255,202,255,192,255,192,255,192,255,201,255,212,255,210,255,209,255,209,255,206,255,203,255,201,255,205,255,210,255,213,255,223,255,227,255,230,255,221,255,213,255,208,255,196,255,199,255,212,
        255,208,255,217,255,216,255,219,255,220,255,209,255,208,255,212,255,206,255,205,255,219,255,235,255,253,0,11,0,28,0,35,0,44,0,46,0,54,0,61,0,60,0,60,0,53,0,40,0,39,0,32,0,26,0,18,0,17,0,11,0,3,255,253,255,239,255,237,255,231,255,231,255,220,255,216,255,
        209,255,208,255,206,255,214,255,216,255,216,255,223,255,221,255,230,255,232,255,234,255,244,255,250,0,3,255,252,255,255,255,246,255,248,255,255,0,0,0,4,0,11,0,17,0,19,0,28,0,22,0,21,0,22,0,24,0,14,0,19,0,22,0,24,0,25,0,17,0,28,0,21,0,28,0,36,0,33,0,32,
        0,35,0,32,0,32,0,30,0,29,0,32,0,42,0,53,0,53,0,57,0,69,0,75,0,86,0,93,0,109,0,123,0,139,0,158,0,169,0,175,0,193,0,198,0,201,0,204,0,201,0,206,0,201,0,206,0,202,0,217,0,223,0,222,0,217,0,216,0,220,0,217,0,222,0,222,0,216,0,220,0,229,0,226,0,231,0,219,
        0,206,0,201,0,170,0,152,0,133,0,107,0,91,0,80,0,64,0,58,0,53,0,46,0,53,0,57,0,61,0,60,0,60,0,71,0,71,0,69,0,62,0,60,0,54,0,50,0,43,0,28,0,18,0,10,255,255,255,239,255,216,255,202,255,183,255,176,255,176,255,166,255,162,255,155,255,147,255,144,255,136,
        255,120,255,99,255,83,255,66,255,63,255,62,255,58,255,55,255,54,255,58,255,57,255,55,255,39,255,32,255,26,255,23,255,5,255,7,254,253,255,14,255,8,255,9,255,4,254,253,254,239,254,223,254,205,254,191,254,180,254,166,254,169,254,169,254,166,254,167,254,
        173,254,174,254,170,254,175,254,177,254,184,254,199,254,210,254,223,254,239,255,4,255,15,255,40,255,62,255,77,255,91,255,101,255,115,255,126,255,140,255,159,255,176,255,190,255,210,255,223,255,235,255,244,0,0,0,11,0,26,0,46,0,60,0,84,0,102,0,125,0,141,
        0,162,0,159,0,155,0,141,0,133,0,143,0,151,0,166,0,190,0,205,0,219,0,224,0,226,0,216,0,206,0,199,0,191,0,186,0,180,0,172,0,172,0,169,0,180,0,179,0,177,0,180,0,175,0,165,0,154,0,144,0,132,0,111,0,96,0,86,0,80,0,79,0,72,0,57,0,40,0,19,255,253,255,244,255,
        224,255,220,255,217,255,203,255,212,255,220,255,217,255,223,255,227,255,232,255,232,255,228,255,230,255,224,255,234,255,230,255,234,255,237,255,244,255,252,0,3,0,12,0,14,0,12,0,6,255,255,0,0,255,246,255,245,255,248,255,239,255,235,255,245,255,248,0,0,
        0,6,0,11,0,6,0,14,0,6,0,17,0,28,0,29,0,29,0,36,0,32,0,26,0,26,0,17,0,18,0,21,0,14,0,15,0,12,0,3,0,7,255,253,255,237,255,223,255,210,255,213,255,213,255,208,255,210,255,199,255,194,255,194,255,194,255,190,255,194,255,201,255,210,255,217,255,230,255,230,
        255,231,255,234,255,235,255,235,255,232,255,235,255,238,255,245,255,255,0,8,0,18,0,25,0,21,0,19,0,19,0,28,0,33,0,47,0,47,0,55,0,66,0,62,0,60,0,55,0,54,0,53,0,50,0,51,0,51,0,54,0,55,0,61,0,54,0,42,0,30,0,18,0,28,0,26,0,43,0,55,0,73,0,83,0,94,0,108,0,116,
        0,137,0,144,0,161,0,175,0,186,0,198,0,197,0,194,0,197,0,193,0,197,0,197,0,205,0,201,0,205,0,204,0,199,0,193,0,186,0,198,0,204,0,193,0,193,0,187,0,187,0,180,0,173,0,170,0,168,0,155,0,140,0,112,0,91,0,60,0,39,0,28,0,32,0,33,0,43,0,47,0,47,0,44,0,32,0,29,
        0,12,0,10,255,255,0,0,0,1,255,245,255,246,255,241,255,244,255,244,255,237,255,227,255,227,255,216,255,208,255,199,255,185,255,173,255,165,255,160,255,163,255,156,255,154,255,142,255,129,255,106,255,83,255,62,255,41,255,36,255,27,255,30,255,32,255,41,
        255,41,255,50,255,50,255,62,255,52,255,44,255,51,255,40,255,41,255,36,255,34,255,27,255,18,255,12,255,4,254,253,254,250,254,243,254,234,254,223,254,210,254,206,254,210,254,216,254,217,254,207,254,203,254,193,254,198,254,195,254,200,254,223,254,231,254,
        250,255,15,255,33,255,40,255,52,255,65,255,73,255,73,255,81,255,98,255,116,255,144,255,170,255,184,255,209,255,228,255,245,255,252,0,14,0,19,0,29,0,30,0,42,0,57,0,73,0,101,0,122,0,147,0,175,0,193,0,208,0,209,0,205,0,194,0,191,0,194,0,201,0,205,0,217,
        0,220,0,227,0,224,0,213,0,202,0,191,0,188,0,190,0,191,0,195,0,194,0,186,0,180,0,181,0,177,0,176,0,162,0,166,0,151,0,143,0,127,0,122,0,109,0,97,0,82,0,66,0,58,0,47,0,25,0,15,0,6,0,0,255,252,255,242,255,228,255,216,255,212,255,219,255,228,255,230,255,238,
        255,244,0,0,255,248,255,241,255,242,255,238,255,248,255,250,0,3,0,17,0,25,0,21,0,30,0,29,0,35,0,32,0,35,0,29,0,21,0,14,0,10,0,11,0,15,0,25,0,21,0,24,0,26,0,22,0,24,0,18,0,14,0,8,0,8,255,255,0,3,0,7,0,15,0,21,0,18,0,25,0,35,0,36,0,35,0,28,0,19,0,10,255,
        249,255,238,255,221,255,219,255,214,255,209,255,195,255,190,255,178,255,169,255,169,255,167,255,167,255,184,255,201,255,216,255,227,255,239,255,235,255,230,255,220,255,217,255,217,255,223,255,242,255,250,0,0,0,14,0,22,0,26,0,25,0,26,0,25,0,24,0,28,0,
        37,0,50,0,65,0,69,0,90,0,93,0,98,0,98,0,89,0,80,0,78,0,75,0,69,0,72,0,65,0,61,0,48,0,37,0,30,0,25,0,28,0,26,0,36,0,47,0,57,0,71,0,79,0,91,0,107,0,122,0,140,0,143,0,145,0,152,0,155,0,158,0,172,0,181,0,172,0,176,0,170,0,175,0,169,0,180,0,176,0,175,0,179,
        0,168,0,173,0,175,0,168,0,158,0,150,0,145,0,133,0,119,0,102,0,93,0,76,0,72,0,57,0,54,0,53,0,39,0,36,0,32,0,22,0,15,0,11,0,10,0,1,255,241,255,227,255,209,255,191,255,183,255,174,255,169,255,176,255,183,255,185,255,183,255,185,255,195,255,198,255,190,255,
        190,255,187,255,178,255,177,255,173,255,176,255,177,255,158,255,144,255,119,255,94,255,75,255,59,255,55,255,51,255,48,255,50,255,32,255,26,255,18,255,26,255,33,255,47,255,55,255,59,255,59,255,73,255,69,255,59,255,50,255,45,255,41,255,37,255,33,255,27,
        255,21,255,23,255,23,255,19,255,22,255,12,255,9,255,8,255,0,254,247,254,236,254,234,254,223,254,220,254,214,254,218,254,227,254,229,254,247,255,0,255,12,255,23,255,25,255,26,255,40,255,48,255,61,255,70,255,91,255,116,255,142,255,167,255,191,255,208,255,
        224,255,238,255,252,0,14,0,22,0,36,0,46,0,60,0,79,0,96,0,104,0,123,0,141,0,173,0,186,0,201,0,213,0,230,0,237,0,231,0,229,0,215,0,202,0,194,0,187,0,193,0,202,0,215,0,215,0,212,0,201,0,201,0,204,0,201,0,191,0,188,0,186,0,190,0,183,0,175,0,169,0,166,0,159,
        0,148,0,139,0,140,0,120,0,105,0,86,0,76,0,62,0,57,0,46,0,44,0,35,0,28,0,12,0,6,255,252,255,252,255,241,255,239,255,244,255,244,255,248,0,0,0,10,0,12,0,25,0,37,0,42,0,46,0,48,0,37,0,30,0,30,0,35,0,35,0,40,0,44,0,47,0,58,0,72,0,71,0,72,0,65,0,58,0,51,0,
        42,0,39,0,37,0,29,0,30,0,35,0,28,0,15,0,17,0,12,0,12,0,14,0,17,0,24,0,29,0,33,0,37,0,36,0,28,0,24,0,24,0,12,0,4,255,253,255,239,255,226,255,219,255,202,255,198,255,198,255,199,255,194,255,190,255,187,255,190,255,190,255,191,255,187,255,198,255,195,255,
        201,255,209,255,217,255,227,255,237,255,239,255,244,255,250,255,255,0,0,0,10,0,17,0,29,0,39,0,47,0,43,0,54,0,57,0,62,0,78,0,82,0,94,0,100,0,104,0,98,0,102,0,97,0,96,0,84,0,82,0,76,0,68,0,64,0,60,0,54,0,57,0,50,0,51,0,46,0,42,0,40,0,37,0,47,0,51,0,65,
        0,79,0,83,0,101,0,111,0,125,0,139,0,148,0,148,0,152,0,155,0,157,0,161,0,162,0,165,0,169,0,173,0,176,0,175,0,172,0,165,0,161,0,152,0,148,0,139,0,134,0,126,0,114,0,94,0,76,0,61,0,54,0,39,0,39,0,46,0,39,0,40,0,33,0,29,0,22,0,4,255,241,255,216,255,203,255,
        188,255,173,255,172,255,159,255,165,255,159,255,145,255,145,255,145,255,142,255,134,255,136,255,144,255,151,255,165,255,163,255,162,255,158,255,154,255,156,255,144,255,136,255,123,255,119,255,108,255,95,255,86,255,79,255,81,255,73,255,63,255,61,255,57,
        255,54,255,48,255,51,255,54,255,44,255,43,255,47,255,44,255,48,255,57,255,62,255,68,255,63,255,59,255,54,255,54,255,55,255,57,255,58,255,62,255,54,255,61,255,65,255,58,255,50,255,36,255,23,255,9,255,8,255,3,255,1,255,7,255,7,255,3,255,0,254,243,254,242,
        254,239,255,1,255,14,255,26,255,43,255,57,255,70,255,84,255,81,255,101,255,112,255,133,255,147,255,165,255,177,255,194,255,201,255,224,255,234,0,6,0,30,0,51,0,72,0,93,0,109,0,123,0,125,0,141,0,151,0,175,0,186,0,205,0,219,0,234,0,235,0,230,0,220,0,213,
        0,209,0,202,0,204,0,201,0,209,0,216,0,216,0,213,0,204,0,206,0,194,0,183,0,184,0,179,0,186,0,181,0,173,0,165,0,155,0,147,0,130,0,125,0,129,0,122,0,120,0,119,0,105,0,107,0,89,0,78,0,66,0,51,0,46,0,40,0,35,0,33,0,25,0,17,0,15,0,14,0,11,0,4,0,15,0,25,0,22,
        0,21,0,28,0,26,0,35,0,33,0,39,0,36,0,35,0,29,0,32,0,37,0,40,0,53,0,65,0,78,0,79,0,79,0,83,0,86,0,72,0,68,0,62,0,46,0,36,0,40,0,37,0,42,0,37,0,42,0,62,0,55,0,57,0,60,0,55,0,55,0,46,0,43,0,33,0,28,0,19,0,17,255,253,255,245,255,244,255,234,255,220,255,208,
        255,208,255,201,255,201,255,205,255,205,255,216,255,216,255,212,255,203,255,191,255,187,255,178,255,177,255,183,255,196,255,208,255,227,255,237,255,252,255,255,255,253,255,252,0,4,0,11,0,15,0,26,0,28,0,47,0,61,0,69,0,80,0,83,0,80,0,75,0,79,0,78,0,68,
        0,75,0,75,0,80,0,86,0,83,0,89,0,86,0,84,0,84,0,78,0,71,0,69,0,61,0,58,0,54,0,46,0,50,0,51,0,48,0,50,0,58,0,61,0,72,0,75,0,87,0,94,0,100,0,114,0,119,0,122,0,126,0,136,0,137,0,152,0,151,0,158,0,154,0,155,0,158,0,157,0,155,0,151,0,147,0,139,0,122,0,97,0,
        91,0,68,0,65,0,53,0,43,0,32,0,30,0,26,0,30,0,30,0,22,0,30,0,17,255,253,255,237,255,214,255,194,255,172,255,165,255,155,255,149,255,145,255,144,255,148,255,138,255,133,255,124,255,120,255,123,255,126,255,117,255,122,255,115,255,117,255,117,255,117,255,
        116,255,115,255,115,255,117,255,120,255,126,255,133,255,127,255,123,255,115,255,104,255,95,255,81,255,91,255,90,255,80,255,81,255,75,255,70,255,63,255,61,255,55,255,47,255,43,255,52,255,54,255,65,255,66,255,81,255,81,255,80,255,75,255,81,255,83,255,75,
        255,79,255,73,255,68,255,55,255,47,255,39,255,26,255,25,255,15,255,19,255,12,255,22,255,23,255,21,255,9,255,3,254,252,254,252,254,250,255,1,255,14,255,23,255,36,255,43,255,55,255,69,255,88,255,104,255,116,255,126,255,130,255,155,255,177,255,198,255,221,
        255,238,0,7,0,29,0,50,0,69,0,97,0,114,0,126,0,139,0,140,0,145,0,161,0,170,0,179,0,195,0,201,0,212,0,219,0,222,0,217,0,213,0,206,0,211,0,211,0,206,0,205,0,215,0,223,0,220,0,223,0,205,0,198,0,187,0,184,0,175,0,169,0,162,0,159,0,154,0,143,0,134,0,129,0,
        127,0,126,0,122,0,130,0,122,0,119,0,104,0,94,0,80,0,75,0,61,0,51,0,51,0,42,0,37,0,37,0,35,0,30,0,25,0,30,0,24,0,33,0,30,0,25,0,25,0,28,0,29,0,36,0,37,0,44,0,58,0,71,0,73,0,75,0,78,0,75,0,71,0,76,0,71,0,76,0,82,0,72,0,76,0,64,0,51,0,48,0,43,0,42,0,39,
        0,50,0,58,0,64,0,73,0,79,0,80,0,71,0,55,0,50,0,46,0,28,0,24,0,19,0,12,0,7,255,253,255,245,255,231,255,223,255,223,255,216,255,219,255,216,255,223,255,224,255,221,255,213,255,202,255,195,255,187,255,178,255,183,255,184,255,181,255,185,255,191,255,202,
        255,212,255,226,255,232,255,252,0,1,0,8,0,11,0,18,0,25,0,29,0,46,0,53,0,60,0,58,0,54,0,54,0,54,0,57,0,51,0,60,0,60,0,60,0,68,0,61,0,62,0,73,0,76,0,76,0,82,0,83,0,75,0,71,0,60,0,55,0,46,0,47,0,47,0,51,0,54,0,60,0,47,0,51,0,53,0,55,0,58,0,55,0,68,0,79,
        0,89,0,97,0,104,0,120,0,132,0,134,0,141,0,139,0,140,0,139,0,148,0,148,0,140,0,136,0,125,0,116,0,96,0,91,0,78,0,66,0,51,0,46,0,40,0,32,0,26,0,14,0,10,0,6,0,0,255,245,255,232,255,219,255,205,255,184,255,170,255,165,255,156,255,158,255,154,255,141,255,142,
        255,138,255,129,255,129,255,130,255,124,255,117,255,101,255,91,255,97,255,91,255,95,255,98,255,99,255,108,255,115,255,116,255,124,255,129,255,129,255,129,255,138,255,140,255,140,255,136,255,140,255,141,255,142,255,126,255,119,255,117,255,109,255,109,
        255,94,255,83,255,81,255,75,255,72,255,65,255,69,255,66,255,68,255,70,255,76,255,76,255,87,255,97,255,94,255,91,255,80,255,77,255,77,255,70,255,73,255,66,255,63,255,63,255,50,255,40,255,30,255,25,255,19,255,26,255,32,255,18,255,21,255,14,255,8,255,9,
        255,14,255,21,255,32,255,47,255,57,255,73,255,90,255,106,255,116,255,123,255,136,255,147,255,163,255,183,255,202,255,234,0,6,0,18,0,33,0,54,0,65,0,87,0,97,0,116,0,134,0,144,0,154,0,152,0,161,0,169,0,175,0,191,0,212,0,217,0,227,0,219,0,222,0,212,0,204,
        0,213,0,209,0,213,0,212,0,212,0,211,0,208,0,204,0,193,0,181,0,173,0,166,0,154,0,148,0,141,0,140,0,133,0,125,0,129,0,123,0,132,0,118,0,127,0,120,0,120,0,118,0,108,0,105,0,94,0,84,0,71,0,61,0,60,0,50,0,61,0,47,0,46,0,40,0,40,0,32,0,40,0,39,0,30,0,32,0,
        28,0,29,0,35,0,48,0,61,0,61,0,66,0,79,0,83,0,83,0,78,0,75,0,60,0,62,0,58,0,61,0,64,0,66,0,68,0,68,0,66,0,68,0,72,0,73,0,73,0,76,0,79,0,83,0,83,0,82,0,72,0,66,0,58,0,57,0,48,0,35,0,32,0,15,0,18,0,10,255,248,255,253,255,245,255,239,255,235,255,230,255,
        231,255,228,255,226,255,216,255,206,255,198,255,187,255,181,255,184,255,187,255,190,255,194,255,198,255,206,255,216,255,227,255,241,255,245,255,253,0,8,0,19,0,29,0,33,0,33,0,29,0,26,0,17,0,17,0,22,0,15,0,22,0,36,0,33,0,35,0,44,0,50,0,61,0,64,0,75,0,78,
        0,83,0,79,0,79,0,76,0,80,0,79,0,79,0,82,0,73,0,68,0,65,0,55,0,57,0,44,0,48,0,43,0,48,0,51,0,57,0,53,0,51,0,48,0,55,0,72,0,84,0,96,0,107,0,115,0,119,0,119,0,130,0,130,0,133,0,136,0,136,0,127,0,123,0,104,0,93,0,83,0,73,0,62,0,48,0,47,0,33,0,26,0,14,255,
        255,255,250,255,246,255,226,255,206,255,180,255,180,255,170,255,170,255,170,255,162,255,159,255,155,255,147,255,148,255,136,255,124,255,116,255,111,255,104,255,97,255,91,255,97,255,95,255,97,255,97,255,91,255,87,255,88,255,97,255,98,255,101,255,112,255,
        116,255,116,255,120,255,126,255,129,255,148,255,160,255,173,255,176,255,178,255,172,255,170,255,162,255,152,255,136,255,120,255,111,255,116,255,111,255,106,255,111,255,108,255,97,255,94,255,88,255,84,255,87,255,86,255,87,255,93,255,97,255,98,255,87,255,
        88,255,86,255,87,255,88,255,91,255,91,255,80,255,80,255,70,255,55,255,36,255,37,255,48,255,40,255,44,255,41,255,40,255,41,255,44,255,36,255,40,255,52,255,63,255,73,255,90,255,98,255,106,255,123,255,129,255,149,255,160,255,178,255,198,255,221,255,237,
        0,6,0,21,0,25,0,43,0,54,0,75,0,98,0,107,0,115,0,125,0,126,0,143,0,155,0,163,0,175,0,191,0,209,0,213,0,206,0,211,0,213,0,205,0,208,0,209,0,215,0,212,0,215,0,215,0,213,0,198,0,188,0,177,0,163,0,150,0,150,0,139,0,144,0,145,0,144,0,141,0,145,0,139,0,139,
        0,136,0,133,0,123,0,126,0,111,0,104,0,105,0,98,0,91,0,84,0,75,0,62,0,55,0,48,0,51,0,48,0,42,0,44,0,40,0,37,0,42,0,35,0,28,0,29,0,32,0,39,0,39,0,51,0,72,0,69,0,79,0,86,0,82,0,84,0,87,0,83,0,72,0,66,0,55,0,58,0,61,0,62,0,62,0,84,0,83,0,80,0,82,0,72,0,65,
        0,65,0,69,0,71,0,69,0,71,0,68,0,64,0,66,0,66,0,66,0,65,0,57,0,44,0,28,0,17,0,8,0,8,0,7,255,252,255,248,255,239,255,232,255,221,255,213,255,205,255,198,255,202,255,202,255,201,255,202,255,208,255,209,255,203,255,206,255,210,255,223,255,234,255,237,255,
        242,255,245,255,252,0,6,255,253,0,1,0,12,0,1,0,8,0,4,0,4,0,3,0,11,0,17,0,21,0,26,0,30,0,35,0,48,0,61,0,64,0,58,0,58,0,54,0,54,0,57,0,58,0,54,0,60,0,65,0,62,0,65,0,64,0,65,0,55,0,55,0,47,0,43,0,37,0,35,0,26,0,28,0,29,0,26,0,26,0,37,0,46,0,53,0,69,0,82,
        0,86,0,93,0,100,0,111,0,118,0,109,0,105,0,101,0,96,0,84,0,72,0,72,0,62,0,51,0,36,0,26,0,21,0,11,0,6,255,248,255,232,255,220,255,203,255,183,255,177,255,160,255,154,255,148,255,149,255,151,255,154,255,141,255,133,255,123,255,115,255,120,255,113,255,112,
        255,112,255,104,255,101,255,95,255,94,255,88,255,90,255,90,255,86,255,90,255,88,255,95,255,102,255,104,255,108,255,115,255,111,255,122,255,131,255,149,255,170,255,181,255,187,255,192,255,196,255,195,255,202,255,194,255,192,255,180,255,169,255,162,255,
        155,255,147,255,142,255,134,255,133,255,123,255,120,255,109,255,98,255,91,255,93,255,93,255,80,255,80,255,84,255,93,255,98,255,102,255,111,255,116,255,112,255,109,255,97,255,87,255,70,255,58,255,47,255,37,255,34,255,39,255,50,255,63,255,66,255,70,255,
        76,255,79,255,87,255,86,255,95,255,102,255,111,255,127,255,144,255,156,255,167,255,176,255,195,255,216,255,227,255,244,255,250,0,4,0,14,0,22,0,35,0,42,0,48,0,65,0,75,0,79,0,94,0,111,0,119,0,144,0,144,0,163,0,175,0,180,0,186,0,194,0,195,0,197,0,198,0,
        202,0,208,0,206,0,209,0,206,0,204,0,199,0,190,0,180,0,161,0,150,0,139,0,139,0,143,0,145,0,150,0,147,0,150,0,148,0,147,0,136,0,114,0,112,0,119,0,112,0,107,0,105,0,100,0,91,0,84,0,72,0,69,0,60,0,68,0,62,0,58,0,58,0,54,0,43,0,43,0,43,0,43,0,36,0,39,0,36,
        0,42,0,48,0,55,0,53,0,65,0,62,0,72,0,78,0,76,0,71,0,73,0,80,0,72,0,66,0,64,0,65,0,64,0,65,0,65,0,61,0,62,0,73,0,69,0,71,0,80,0,86,0,84,0,91,0,93,0,97,0,96,0,93,0,83,0,86,0,76,0,66,0,64,0,57,0,42,0,36,0,25,0,25,0,15,0,12,0,4,255,245,255,242,255,235,255,
        230,255,228,255,226,255,226,255,230,255,227,255,234,255,227,255,220,255,231,255,235,255,230,255,231,255,238,255,235,255,238,255,241,255,238,255,242,255,248,255,241,255,242,255,237,255,237,255,244,255,250,255,253,0,1,0,14,0,22,0,25,0,30,0,37,0,47,0,55,
        0,39,0,43,0,47,0,47,0,54,0,64,0,58,0,61,0,65,0,64,0,53,0,57,0,54,0,57,0,55,0,42,0,37,0,43,0,32,0,22,0,12,0,10,0,17,0,26,0,40,0,42,0,48,0,46,0,61,0,61,0,65,0,76,0,75,0,82,0,75,0,73,0,79,0,71,0,61,0,58,0,47,0,44,0,40,0,32,0,25,0,14,0,6,255,245,255,226,
        255,214,255,203,255,190,255,174,255,165,255,148,255,142,255,137,255,137,255,126,255,123,255,120,255,113,255,104,255,102,255,95,255,98,255,95,255,97,255,99,255,109,255,102,255,104,255,95,255,97,255,94,255,95,255,94,255,97,255,98,255,104,255,106,255,115,
        255,119,255,119,255,133,255,144,255,149,255,165,255,174,255,191,255,198,255,209,255,208,255,205,255,205,255,203,255,195,255,194,255,194,255,190,255,178,255,167,255,156,255,154,255,136,255,130,255,123,255,120,255,117,255,105,255,98,255,91,255,88,255,87,
        255,90,255,99,255,105,255,113,255,122,255,126,255,120,255,122,255,115,255,105,255,95,255,86,255,77,255,72,255,72,255,61,255,68,255,68,255,80,255,87,255,104,255,98,255,109,255,109,255,115,255,127,255,134,255,149,255,169,255,183,255,199,255,214,255,223,
        255,227,255,239,255,239,255,253,0,7,0,11,0,17,0,29,0,35,0,40,0,43,0,55,0,68,0,72,0,82,0,93,0,115,0,127,0,136,0,150,0,161,0,172,0,181,0,184,0,193,0,187,0,184,0,184,0,176,0,181,0,183,0,183,0,181,0,173,0,170,0,159,0,147,0,130,0,130,0,133,0,136,0,134,0,141,
        0,134,0,130,0,129,0,120,0,107,0,107,0,100,0,100,0,96,0,94,0,80,0,80,0,76,0,69,0,65,0,62,0,65,0,71,0,72,0,68,0,69,0,66,0,66,0,53,0,47,0,44,0,55,0,54,0,53,0,51,0,54,0,51,0,53,0,64,0,71,0,64,0,57,0,64,0,69,0,76,0,80,0,73,0,69,0,64,0,62,0,53,0,53,0,50,0,
        58,0,66,0,66,0,80,0,89,0,100,0,100,0,109,0,116,0,118,0,114,0,107,0,108,0,96,0,101,0,78,0,78,0,66,0,71,0,69,0,55,0,46,0,39,0,22,0,10,255,253,255,245,255,237,255,235,255,237,255,241,255,244,255,242,255,249,255,242,255,238,255,237,255,234,255,237,255,238,
        255,224,255,228,255,230,255,232,255,231,255,231,255,230,255,230,255,242,255,237,255,241,255,239,255,245,255,255,0,1,0,3,0,14,0,22,0,25,0,29,0,28,0,35,0,33,0,40,0,37,0,36,0,29,0,29,0,26,0,37,0,43,0,46,0,53,0,61,0,60,0,64,0,55,0,48,0,44,0,39,0,29,0,24,
        0,29,0,29,0,33,0,26,0,36,0,36,0,37,0,48,0,50,0,50,0,50,0,51,0,54,0,51,0,53,0,57,0,51,0,48,0,43,0,42,0,35,0,26,0,26,0,24,0,18,0,8,0,0,255,238,255,227,255,217,255,199,255,192,255,184,255,173,255,169,255,155,255,141,255,130,255,120,255,119,255,115,255,117,
        255,115,255,116,255,112,255,113,255,106,255,109,255,109,255,109,255,119,255,123,255,129,255,131,255,130,255,126,255,124,255,108,255,111,255,119,255,120,255,123,255,133,255,144,255,147,255,149,255,156,255,163,255,178,255,180,255,185,255,191,255,201,255,
        209,255,217,255,217,255,221,255,221,255,223,255,219,255,212,255,202,255,188,255,183,255,169,255,154,255,140,255,129,255,130,255,123,255,123,255,122,255,117,255,115,255,111,255,117,255,117,255,116,255,111,255,120,255,119,255,126,255,117,255,113,255,113,
        255,116,255,106,255,106,255,94,255,93,255,98,255,94,255,102,255,102,255,120,255,117,255,117,255,131,255,137,255,142,255,151,255,159,255,173,255,185,255,194,255,199,255,217,255,226,255,227,255,242,255,252,0,3,0,12,0,8,0,18,0,22,0,32,0,37,0,43,0,53,0,55,
        0,68,0,72,0,82,0,93,0,100,0,109,0,120,0,141,0,144,0,169,0,168,0,168,0,165,0,168,0,159,0,154,0,148,0,158,0,162,0,170,0,161,0,165,0,158,0,148,0,141,0,134,0,132,0,125,0,127,0,115,0,116,0,120,0,108,0,94,0,86,0,89,0,80,0,76,0,71,0,64,0,64,0,60,0,62,0,65,0,
        58,0,58,0,65,0,65,0,66,0,75,0,69,0,72,0,79,0,73,0,66,0,61,0,60,0,54,0,62,0,61,0,65,0,54,0,55,0,50,0,53,0,50,0,44,0,42,0,43,0,48,0,51,0,50,0,54,0,53,0,57,0,58,0,50,0,46,0,51,0,51,0,55,0,68,0,80,0,89,0,100,0,107,0,118,0,127,0,120,0,108,0,105,0,97,0,80,
        0,71,0,64,0,66,0,76,0,69,0,75,0,69,0,60,0,46,0,30,0,19,0,1,255,248,255,244,255,242,255,241,255,249,255,246,255,246,255,248,255,238,255,235,255,227,255,220,255,214,255,213,255,224,255,224,255,228,255,235,255,228,255,234,255,228,255,234,255,234,255,232,
        255,246,255,250,255,249,255,255,0,0,0,0,0,4,0,3,0,0,0,0,0,7,0,10,0,8,0,17,0,19,0,24,0,22,0,29,0,28,0,32,0,32,0,43,0,48,0,48,0,43,0,40,0,36,0,33,0,29,0,37,0,36,0,33,0,36,0,32,0,35,0,32,0,33,0,29,0,26,0,28,0,25,0,36,0,46,0,42,0,33,0,28,0,30,0,29,0,25,0,
        25,0,14,0,18,0,17,0,6,0,1,255,253,255,245,255,239,255,230,255,224,255,216,255,203,255,201,255,190,255,181,255,162,255,147,255,129,255,126,255,126,255,124,255,119,255,120,255,127,255,119,255,119,255,126,255,124,255,124,255,129,255,131,255,137,255,138,
        255,138,255,142,255,145,255,138,255,138,255,136,255,140,255,145,255,144,255,147,255,149,255,152,255,159,255,165,255,166,255,174,255,180,255,174,255,184,255,190,255,196,255,208,255,216,255,221,255,223,255,231,255,227,255,227,255,216,255,212,255,203,255,
        191,255,184,255,172,255,169,255,159,255,145,255,138,255,137,255,133,255,130,255,127,255,122,255,123,255,123,255,123,255,116,255,123,255,113,255,119,255,120,255,124,255,117,255,131,255,129,255,123,255,127,255,130,255,129,255,137,255,136,255,144,255,141,
        255,145,255,142,255,145,255,151,255,163,255,178,255,191,255,198,255,214,255,216,255,220,255,223,255,230,255,232,255,244,0,1,0,11,0,18,0,28,0,22,0,29,0,29,0,35,0,39,0,44,0,54,0,62,0,69,0,83,0,86,0,91,0,97,0,104,0,109,0,132,0,133,0,143,0,140,0,141,0,141,
        0,137,0,145,0,143,0,141,0,144,0,141,0,139,0,148,0,144,0,140,0,126,0,126,0,130,0,119,0,119,0,114,0,105,0,100,0,96,0,90,0,83,0,80,0,83,0,76,0,71,0,62,0,58,0,50,0,46,0,44,0,48,0,48,0,55,0,58,0,64,0,75,0,78,0,78,0,76,0,76,0,73,0,66,0,65,0,61,0,58,0,55,0,
        60,0,68,0,69,0,64,0,55,0,47,0,44,0,42,0,35,0,36,0,33,0,37,0,44,0,50,0,51,0,53,0,55,0,46,0,60,0,61,0,72,0,71,0,73,0,82,0,97,0,104,0,105,0,109,0,112,0,115,0,111,0,115,0,105,0,101,0,86,0,73,0,68,0,73,0,68,0,58,0,55,0,53,0,39,0,36,0,24,0,17,0,12,0,7,255,
        255,255,239,255,231,255,231,255,228,255,224,255,220,255,220,255,220,255,217,255,219,255,228,255,224,255,224,255,230,255,232,255,235,255,237,255,238,255,244,255,239,255,241,255,230,255,244,255,244,255,241,255,239,255,242,255,242,255,245,255,246,0,6,0,
        0,0,12,0,7,0,8,0,3,0,11,0,8,0,17,0,18,0,19,0,24,0,26,0,22,0,24,0,26,0,25,0,26,0,29,0,42,0,40,0,42,0,37,0,44,0,44,0,33,0,37,0,28,0,26,0,22,0,12,0,19,0,18,0,22,0,18,0,14,0,12,0,14,0,19,0,17,0,12,0,10,0,4,0,3,0,0,255,253,255,237,255,231,255,227,255,216,
        255,206,255,198,255,185,255,177,255,166,255,158,255,149,255,151,255,138,255,140,255,140,255,144,255,136,255,136,255,134,255,134,255,137,255,137,255,137,255,142,255,154,255,149,255,152,255,149,255,155,255,154,255,156,255,155,255,155,255,156,255,151,255,
        152,255,158,255,158,255,162,255,169,255,172,255,176,255,170,255,169,255,174,255,178,255,183,255,184,255,190,255,196,255,209,255,216,255,217,255,209,255,213,255,214,255,214,255,214,255,199,255,201,255,201,255,191,255,180,255,173,255,160,255,155,255,145,
        255,136,255,133,255,134,255,133,255,130,255,130,255,133,255,133,255,129,255,126,255,127,255,127,255,131,255,127,255,131,255,136,255,141,255,149,255,155,255,166,255,173,255,170,255,178,255,170,255,176,255,184,255,184,255,185,255,198,255,199,255,206,255,
        220,255,231,255,232,255,241,255,237,255,245,255,249,255,253,0,3,0,10,0,19,0,29,0,35,0,32,0,30,0,42,0,44,0,54,0,54,0,55,0,65,0,71,0,82,0,87,0,98,0,107,0,108,0,116,0,130,0,130,0,130,0,129,0,134,0,127,0,127,0,127,0,118,0,127,0,126,0,126,0,126,0,123,0,118,
        0,118,0,115,0,105,0,107,0,100,0,86,0,86,0,89,0,82,0,79,0,75,0,73,0,75,0,76,0,61,0,60,0,43,0,39,0,35,0,42,0,50,0,51,0,57,0,66,0,71,0,73,0,68,0,71,0,73,0,66,0,65,0,53,0,61,0,61,0,60,0,54,0,57,0,57,0,51,0,50,0,40,0,42,0,39,0,35,0,35,0,33,0,29,0,32,0,42,
        0,40,0,48,0,46,0,55,0,57,0,69,0,76,0,80,0,79,0,86,0,93,0,101,0,105,0,101,0,108,0,115,0,111,0,108,0,112,0,112,0,111,0,104,0,104,0,86,0,75,0,71,0,64,0,61,0,54,0,40,0,35,0,18,0,12,0,3,255,252,255,238,255,237,255,228,255,219,255,212,255,199,255,202,255,203,
        255,209,255,216,255,217,255,220,255,221,255,224,255,228,255,226,255,231,255,239,255,242,255,239,255,242,255,242,255,246,255,230,255,234,255,234,255,228,255,228,255,231,255,237,255,246,0,1,0,3,0,6,0,11,0,3,0,3,255,250,255,252,255,253,0,1,0,3,0,6,0,11,
        0,12,0,14,0,28,0,24,0,35,0,32,0,24,0,15,0,21,0,15,0,10,0,12,0,18,0,22,0,18,0,19,0,19,0,10,0,7,0,10,0,15,0,8,0,1,0,12,0,4,0,0,255,252,255,255,255,244,255,246,255,238,255,234,255,227,255,221,255,213,255,208,255,198,255,191,255,188,255,181,255,174,255,177,
        255,167,255,163,255,162,255,159,255,162,255,158,255,156,255,145,255,145,255,144,255,142,255,144,255,145,255,141,255,151,255,160,255,170,255,170,255,167,255,167,255,173,255,181,255,187,255,183,255,187,255,178,255,173,255,169,255,169,255,170,255,176,255,
        183,255,181,255,190,255,188,255,196,255,203,255,209,255,205,255,205,255,209,255,209,255,214,255,214,255,206,255,210,255,206,255,203,255,209,255,206,255,196,255,199,255,199,255,198,255,183,255,174,255,165,255,156,255,152,255,148,255,144,255,149,255,148,
        255,152,255,147,255,142,255,133,255,138,255,138,255,136,255,142,255,144,255,140,255,148,255,142,255,154,255,160,255,170,255,177,255,176,255,183,255,183,255,190,255,194,255,198,255,212,255,217,255,223,255,224,255,238,255,238,255,242,255,248,255,250,255,
        248,255,253,0,6,0,12,0,17,0,21,0,21,0,32,0,37,0,43,0,48,0,53,0,58,0,62,0,68,0,75,0,84,0,94,0,100,0,105,0,115,0,112,0,112,0,119,0,120,0,122,0,125,0,120,0,129,0,127,0,123,0,120,0,109,0,114,0,109,0,112,0,109,0,111,0,107,0,102,0,90,0,91,0,83,0,82,0,75,0,
        69,0,68,0,72,0,75,0,75,0,80,0,76,0,76,0,64,0,61,0,50,0,55,0,54,0,61,0,61,0,69,0,66,0,66,0,55,0,65,0,65,0,55,0,57,0,55,0,50,0,43,0,43,0,50,0,51,0,47,0,48,0,51,0,54,0,53,0,53,0,54,0,51,0,48,0,43,0,46,0,39,0,37,0,37,0,50,0,54,0,65,0,66,0,72,0,69,0,71,0,
        79,0,86,0,82,0,89,0,84,0,94,0,94,0,94,0,93,0,94,0,93,0,96,0,93,0,89,0,91,0,90,0,78,0,69,0,61,0,50,0,37,0,24,0,11,0,6,255,249,255,246,255,230,255,231,255,226,255,212,255,212,255,208,255,198,255,199,255,201,255,203,255,205,255,210,255,216,255,217,255,220,
        255,216,255,217,255,226,255,216,255,216,255,232,255,228,255,227,255,231,255,232,255,234,255,230,255,235,255,232,255,230,255,237,255,238,255,244,255,245,255,244,255,248,255,246,255,249,0,1,0,4,0,14,0,6,0,7,0,8,0,8,0,10,0,17,0,21,0,17,0,21,0,11,0,6,0,7,
        0,3,0,8,0,4,0,7,0,17,0,19,0,19,0,19,0,15,0,19,0,10,0,6,0,7,0,7,0,6,0,4,255,250,255,241,255,244,255,227,255,224,255,219,255,217,255,210,255,210,255,210,255,206,255,202,255,209,255,210,255,209,255,205,255,199,255,203,255,195,255,192,255,184,255,176,255,
        170,255,172,255,166,255,170,255,163,255,158,255,162,255,169,255,172,255,183,255,177,255,180,255,185,255,185,255,185,255,190,255,184,255,185,255,194,255,191,255,184,255,181,255,181,255,174,255,177,255,181,255,185,255,194,255,188,255,201,255,210,255,209,
        255,210,255,205,255,209,255,201,255,206,255,212,255,210,255,210,255,209,255,206,255,205,255,196,255,196,255,195,255,201,255,196,255,192,255,196,255,178,255,176,255,177,255,170,255,167,255,166,255,166,255,163,255,165,255,167,255,162,255,159,255,155,255,
        148,255,142,255,147,255,147,255,151,255,147,255,158,255,165,255,173,255,173,255,178,255,184,255,190,255,195,255,199,255,201,255,210,255,217,255,220,255,226,255,228,255,235,255,238,255,241,255,249,0,0,0,7,0,8,0,12,0,17,0,18,0,28,0,25,0,24,0,28,0,29,0,
        35,0,47,0,58,0,58,0,65,0,76,0,84,0,84,0,91,0,94,0,91,0,96,0,102,0,107,0,107,0,111,0,112,0,122,0,120,0,123,0,126,0,118,0,108,0,101,0,89,0,97,0,91,0,90,0,91,0,97,0,91,0,82,0,73,0,64,0,61,0,65,0,57,0,62,0,64,0,66,0,79,0,80,0,80,0,78,0,75,0,75,0,71,0,64,
        0,64,0,60,0,62,0,62,0,58,0,57,0,60,0,60,0,54,0,50,0,46,0,47,0,47,0,37,0,39,0,39,0,39,0,43,0,50,0,47,0,46,0,48,0,54,0,51,0,43,0,40,0,30,0,37,0,42,0,48,0,53,0,61,0,55,0,62,0,66,0,75,0,78,0,80,0,82,0,93,0,96,0,93,0,94,0,93,0,96,0,93,0,89,0,87,0,87,0,87,
        0,79,0,78,0,80,0,68,0,60,0,50,0,36,0,26,0,12,0,12,0,3,255,249,255,242,255,239,255,239,255,228,255,226,255,219,255,210,255,208,255,206,255,196,255,194,255,196,255,201,255,208,255,205,255,208,255,205,255,205,255,209,255,209,255,208,255,214,255,221,255,
        226,255,227,255,234,255,237,255,241,255,246,255,246,255,242,255,249,255,248,255,252,0,1,255,255,0,0,255,253,0,6,0,3,0,7,0,6,0,0,0,10,0,12,0,17,0,11,0,8,0,4,0,3,255,255,255,252,0,6,0,4,0,7,0,4,0,0,0,6,0,8,0,11,0,14,0,21,0,19,0,15,0,14,0,14,0,10,0,1,255,
        249,255,245,255,234,255,230,255,228,255,232,255,220,255,223,255,216,255,217,255,208,255,206,255,209,255,206,255,205,255,205,255,205,255,201,255,199,255,201,255,198,255,201,255,198,255,194,255,190,255,184,255,191,255,191,255,187,255,187,255,187,255,188,
        255,194,255,192,255,192,255,192,255,187,255,192,255,191,255,191,255,194,255,192,255,185,255,188,255,191,255,194,255,191,255,187,255,187,255,192,255,191,255,195,255,199,255,202,255,212,255,219,255,214,255,216,255,206,255,206,255,210,255,212,255,209,255,
        205,255,206,255,203,255,205,255,198,255,196,255,201,255,198,255,191,255,192,255,191,255,195,255,191,255,187,255,181,255,183,255,184,255,187,255,188,255,190,255,184,255,183,255,173,255,178,255,170,255,167,255,159,255,165,255,165,255,167,255,174,255,181,
        255,188,255,184,255,198,255,196,255,195,255,196,255,203,255,205,255,214,255,220,255,223,255,234,255,237,255,235,255,244,255,250,255,255,0,4,0,18,0,12,0,11,0,15,0,25,0,37,0,35,0,39,0,48,0,43,0,43,0,44,0,53,0,57,0,62,0,61,0,68,0,71,0,72,0,76,0,79,0,87,
        0,90,0,96,0,102,0,97,0,98,0,101,0,115,0,120,0,125,0,119,0,107,0,105,0,105,0,90,0,87,0,93,0,90,0,94,0,97,0,93,0,80,0,80,0,73,0,65,0,60,0,64,0,61,0,66,0,73,0,76,0,78,0,84,0,82,0,80,0,75,0,73,0,71,0,65,0,65,0,62,0,51,0,48,0,48,0,46,0,44,0,40,0,47,0,44,0,
        44,0,43,0,50,0,44,0,40,0,39,0,42,0,42,0,46,0,40,0,40,0,30,0,37,0,36,0,37,0,39,0,48,0,47,0,51,0,55,0,58,0,65,0,66,0,73,0,80,0,84,0,91,0,84,0,94,0,93,0,83,0,83,0,82,0,82,0,75,0,71,0,66,0,60,0,60,0,62,0,50,0,36,0,28,0,18,0,11,0,4,0,1,0,0,0,0,255,253,255,
        246,255,237,255,228,255,224,255,220,255,214,255,206,255,198,255,201,255,199,255,201,255,194,255,194,255,196,255,195,255,202,255,196,255,199,255,201,255,208,255,206,255,214,255,221,255,224,255,220,255,221,255,228,255,227,255,234,255,239,255,252,255,249,
        0,1,0,1,255,252,255,253,255,249,255,255,0,3,255,253,0,0,0,1,0,7,0,4,0,12,0,7,0,7,0,7,0,7,0,3,255,244,255,253,255,248,255,249,255,245,0,0,0,0,0,3,0,11,0,12,0,12,0,15,0,6,0,3,0,6,255,253,255,249,255,245,255,245,255,244,255,244,255,241,255,234,255,230,255,
        226,255,216,255,217,255,208,255,208,255,206,255,216,255,219,255,210,255,216,255,205,255,210,255,213,255,213,255,208,255,206,255,202,255,203,255,196,255,187,255,185,255,190,255,181,255,181,255,192,255,187,255,190,255,195,255,199,255,203,255,202,255,190,
        255,199,255,203,255,198,255,194,255,187,255,188,255,187,255,191,255,187,255,188,255,180,255,191,255,184,255,190,255,187,255,195,255,198,255,202,255,202,255,209,255,208,255,210,255,213,255,212,255,212,255,214,255,216,255,206,255,213,255,210,255,208,255,
        205,255,202,255,198,255,203,255,199,255,195,255,194,255,191,255,192,255,190,255,183,255,191,255,187,255,187,255,180,255,177,255,173,255,167,255,169,255,166,255,173,255,178,255,180,255,181,255,188,255,194,255,198,255,196,255,203,255,203,255,199,255,199,
        255,206,255,213,255,213,255,219,255,230,255,231,255,242,255,242,255,242,255,250,255,252,0,7,0,15,0,19,0,30,0,30,0,26,0,30,0,30,0,30,0,42,0,47,0,55,0,54,0,51,0,61,0,55,0,64,0,62,0,62,0,65,0,76,0,75,0,79,0,82,0,82,0,82,0,86,0,84,0,83,0,89,0,97,0,90,0,101,
        0,100,0,97,0,101,0,102,0,97,0,97,0,94,0,84,0,90,0,76,0,72,0,80,0,72,0,76,0,64,0,61,0,60,0,61,0,58,0,75,0,75,0,75,0,79,0,69,0,71,0,66,0,57,0,53,0,48,0,50,0,51,0,46,0,42,0,46,0,53,0,47,0,47,0,42,0,37,0,36,0,32,0,32,0,33,0,35,0,36,0,39,0,29,0,28,0,30,0,
        32,0,28,0,29,0,32,0,33,0,35,0,44,0,51,0,55,0,64,0,68,0,69,0,80,0,83,0,86,0,82,0,79,0,87,0,83,0,72,0,62,0,64,0,57,0,58,0,61,0,64,0,66,0,62,0,53,0,44,0,30,0,25,0,21,0,18,255,255,255,255,255,250,255,245,255,245,255,244,255,234,255,231,255,228,255,217,255,
        213,255,210,255,213,255,212,255,210,255,214,255,206,255,209,255,209,255,205,255,206,255,206,255,198,255,205,255,206,255,206,255,205,255,212,255,216,255,217,255,227,255,231,255,235,255,242,255,244,0,0,0,3,0,0,0,4,0,6,0,4,0,7,255,255,0,0,255,255,0,6,0,
        8,0,14,0,15,0,17,0,18,0,8,0,3,0,0,0,1,0,4,0,7,0,7,0,4,0,3,0,4,0,1,255,250,0,11,0,12,0,10,0,7,255,255,0,3,255,250,255,241,255,234,255,237,255,230,255,238,255,237,255,241,255,237,255,235,255,238,255,228,255,228,255,226,255,219,255,212,255,217,255,221,255,
        208,255,212,255,212,255,209,255,201,255,210,255,210,255,206,255,210,255,205,255,203,255,194,255,196,255,190,255,192,255,194,255,196,255,198,255,199,255,196,255,192,255,194,255,190,255,195,255,198,255,188,255,184,255,180,255,181,255,187,255,195,255,196,
        255,191,255,195,255,195,255,188,255,188,255,187,255,185,255,194,255,191,255,195,255,196,255,199,255,195,255,201,255,201,255,205,255,201,255,202,255,203,255,213,255,216,255,213,255,212,255,208,255,199,255,198,255,209,255,209,255,209,255,210,255,208,255,
        206,255,199,255,191,255,176,255,172,255,172,255,176,255,187,255,178,255,185,255,187,255,184,255,185,255,191,255,195,255,195,255,202,255,203,255,210,255,210,255,208,255,212,255,213,255,219,255,226,255,230,255,231,255,232,255,234,255,238,255,234,255,237,
        255,245,255,255,0,4,0,7,0,12,0,12,0,24,0,24,0,26,0,28,0,30,0,33,0,33,0,33,0,39,0,47,0,43,0,48,0,55,0,60,0,61,0,61,0,57,0,57,0,57,0,55,0,57,0,68,0,69,0,71,0,71,0,72,0,78,0,72,0,82,0,83,0,93,0,96,0,93,0,93,0,89,0,87,0,89,0,91,0,82,0,83,0,84,0,78,0,76,0,
        71,0,69,0,75,0,68,0,69,0,71,0,68,0,78,0,69,0,65,0,69,0,61,0,64,0,64,0,53,0,57,0,58,0,47,0,54,0,51,0,55,0,46,0,43,0,43,0,43,0,33,0,36,0,32,0,30,0,29,0,29,0,22,0,14,0,14,0,21,0,36,0,39,0,36,0,35,0,40,0,43,0,42,0,42,0,48,0,60,0,48,0,54,0,66,0,55,0,62,0,
        62,0,62,0,69,0,64,0,66,0,65,0,64,0,62,0,64,0,64,0,58,0,46,0,51,0,47,0,40,0,35,0,30,0,19,0,15,0,8,0,1,255,255,255,238,255,238,255,238,255,237,255,239,255,235,255,232,255,226,255,228,255,221,255,213,255,219,255,213,255,212,255,208,255,205,255,213,255,212,
        255,212,255,216,255,213,255,209,255,206,255,206,255,214,255,216,255,219,255,220,255,228,255,234,255,244,255,246,255,255,0,1,0,1,0,8,0,4,0,4,0,12,0,14,0,12,0,10,0,15,0,22,0,18,0,28,0,24,0,21,0,15,0,18,0,12,0,14,0,17,0,17,0,19,0,19,0,17,0,12,0,4,0,1,0,
        0,255,249,255,252,0,1,0,3,255,250,255,248,255,248,255,242,255,237,255,237,255,227,255,230,255,231,255,235,255,235,255,238,255,230,255,235,255,219,255,221,255,227,255,228,255,221,255,210,255,209,255,210,255,210,255,210,255,212,255,216,255,208,255,209,
        255,195,255,195,255,195,255,196,255,196,255,191,255,187,255,195,255,198,255,201,255,203,255,206,255,196,255,195,255,205,255,201,255,195,255,194,255,191,255,195,255,192,255,190,255,190,255,187,255,195,255,198,255,196,255,196,255,192,255,192,255,187,255,
        195,255,195,255,195,255,202,255,206,255,210,255,208,255,212,255,206,255,205,255,213,255,213,255,217,255,223,255,223,255,223,255,228,255,227,255,220,255,221,255,216,255,217,255,223,255,212,255,203,255,201,255,195,255,196,255,190,255,192,255,191,255,191,
        255,188,255,192,255,194,255,201,255,201,255,205,255,206,255,209,255,209,255,216,255,210,255,212,255,217,255,221,255,220,255,226,255,230,255,235,255,241,255,242,255,245,255,250,255,237,255,241,255,250,255,252,0,7,0,11,0,18,0,18,0,29,0,24,0,24,0,33,0,32,
        0,36,0,35,0,33,0,39,0,48,0,46,0,43,0,44,0,60,0,64,0,57,0,62,0,68,0,62,0,60,0,64,0,66,0,65,0,61,0,73,0,75,0,78,0,72,0,79,0,75,0,83,0,76,0,83,0,86,0,80,0,86,0,89,0,90,0,90,0,87,0,89,0,84,0,87,0,83,0,82,0,82,0,71,0,65,0,61,0,60,0,54,0,55,0,60,0,60,0,66,
        0,64,0,69,0,61,0,57,0,61,0,48,0,42,0,32,0,37,0,36,0,39,0,39,0,39,0,37,0,36,0,36,0,24,0,28,0,25,0,24,0,25,0,22,0,28,0,33,0,28,0,30,0,33,0,33,0,35,0,35,0,37,0,39,0,36,0,33,0,30,0,33,0,33,0,33,0,30,0,33,0,32,0,43,0,40,0,40,0,44,0,44,0,40,0,42,0,33,0,32,
        0,28,0,28,0,22,0,30,0,28,0,24,0,15,0,6,0,3,255,253,255,244,255,237,255,226,255,221,255,226,255,226,255,223,255,234,255,226,255,230,255,228,255,220,255,212,255,212,255,212,255,208,255,203,255,203,255,206,255,210,255,213,255,220,255,230,255,224,255,223,
        255,227,255,226,255,226,255,226,255,231,255,238,255,237,255,238,255,241,255,245,255,250,0,0,255,250,255,255,0,7,0,11,0,15,0,15,0,15,0,17,0,14,0,15,0,30,0,25,0,24,0,21,0,22,0,24,0,24,0,12,0,18,0,15,0,15,0,7,0,8,0,7,0,0,255,252,255,246,255,245,255,244,
        255,252,255,248,255,237,255,239,255,244,255,238,255,237,255,237,255,235,255,230,255,231,255,221,255,228,255,232,255,234,255,232,255,231,255,226,255,228,255,219,255,214,255,216,255,216,255,212,255,212,255,212,255,205,255,210,255,206,255,195,255,199,255,
        187,255,190,255,190,255,187,255,188,255,196,255,201,255,205,255,209,255,194,255,190,255,199,255,192,255,191,255,181,255,181,255,183,255,181,255,183,255,185,255,191,255,199,255,195,255,190,255,188,255,185,255,190,255,194,255,192,255,192,255,195,255,202,
        255,205,255,205,255,206,255,213,255,212,255,210,255,208,255,209,255,210,255,217,255,213,255,213,255,219,255,221,255,219,255,219,255,217,255,213,255,208,255,203,255,201,255,198,255,196,255,195,255,192,255,194,255,196,255,192,255,192,255,198,255,203,255,
        205,255,213,255,213,255,220,255,227,255,234,255,231,255,227,255,235,255,237,255,238,255,245,255,245,255,249,255,248,255,248,255,253,255,255,0,4,0,4,0,3,0,0,0,3,0,8,0,10,0,17,0,15,0,26,0,32,0,33,0,32,0,30,0,29,0,39,0,40,0,37,0,39,0,37,0,50,0,53,0,62,0,
        60,0,54,0,61,0,65,0,66,0,66,0,69,0,64,0,60,0,64,0,71,0,76,0,73,0,78,0,83,0,80,0,76,0,78,0,83,0,82,0,79,0,87,0,80,0,76,0,78,0,82,0,86,0,90,0,83,0,76,0,72,0,61,0,61,0,57,0,61,0,53,0,55,0,68,0,72,0,69,0,66,0,66,0,60,0,48,0,51,0,47,0,39,0,42,0,42,0,46,0,
        47,0,42,0,37,0,35,0,39,0,32,0,28,0,28,0,19,0,19,0,29,0,28,0,35,0,28,0,32,0,32,0,37,0,36,0,39,0,32,0,35,0,25,0,14,0,18,0,17,0,24,0,24,0,35,0,30,0,29,0,33,0,33,0,30,0,32,0,30,0,24,0,21,0,21,0,21,0,22,0,18,0,14,0,12,0,8,0,14,0,10,0,8,0,8,0,1,0,1,255,250,
        255,241,255,237,255,234,255,231,255,234,255,237,255,231,255,221,255,217,255,220,255,223,255,224,255,226,255,228,255,224,255,221,255,219,255,214,255,220,255,220,255,227,255,226,255,228,255,227,255,230,255,228,255,232,255,232,255,234,255,235,255,239,255,
        245,255,249,255,255,0,3,0,6,0,11,0,18,0,24,0,19,0,18,0,19,0,26,0,24,0,26,0,24,0,24,0,29,0,36,0,26,0,29,0,25,0,19,0,18,0,18,0,12,0,17,0,11,0,10,0,8,0,12,0,7,255,255,255,246,255,241,255,242,255,244,255,246,255,246,255,245,255,242,255,235,255,228,255,230,
        255,223,255,226,255,227,255,230,255,237,255,232,255,230,255,231,255,231,255,232,255,231,255,213,255,212,255,206,255,196,255,196,255,194,255,192,255,195,255,194,255,195,255,192,255,188,255,195,255,195,255,190,255,187,255,183,255,191,255,187,255,190,255,
        190,255,187,255,190,255,187,255,188,255,187,255,188,255,190,255,188,255,192,255,194,255,191,255,194,255,190,255,191,255,191,255,192,255,187,255,190,255,190,255,190,255,201,255,203,255,198,255,203,255,206,255,209,255,212,255,210,255,206,255,201,255,209,
        255,210,255,213,255,217,255,217,255,214,255,214,255,217,255,219,255,216,255,210,255,208,255,212,255,213,255,208,255,213,255,210,255,209,255,208,255,206,255,209,255,212,255,210,255,214,255,219,255,221,255,220,255,228,255,231,255,238,255,238,255,234,255,
        239,255,238,255,246,255,244,255,244,255,250,0,4,255,255,255,253,0,1,0,4,0,3,0,14,0,14,0,18,0,19,0,19,0,29,0,24,0,24,0,21,0,29,0,33,0,39,0,39,0,42,0,46,0,48,0,46,0,47,0,51,0,39,0,50,0,50,0,61,0,57,0,66,0,61,0,62,0,64,0,62,0,61,0,61,0,66,0,68,0,71,0,79,
        0,79,0,80,0,75,0,76,0,75,0,73,0,83,0,91,0,86,0,93,0,83,0,73,0,83,0,84,0,80,0,79,0,79,0,73,0,71,0,69,0,68,0,58,0,57,0,54,0,62,0,61,0,58,0,57,0,55,0,57,0,54,0,51,0,48,0,39,0,36,0,32,0,33,0,33,0,33,0,36,0,32,0,37,0,32,0,32,0,29,0,25,0,19,0,21,0,19,0,22,
        0,22,0,21,0,18,0,24,0,21,0,21,0,19,0,18,0,17,0,15,0,15,0,11,0,14,0,19,0,10,0,11,0,10,0,15,0,8,0,14,0,11,0,14,0,14,0,11,0,6,0,8,0,10,0,8,0,4,0,6,0,1,0,3,0,1,255,253,255,255,255,249,255,241,255,231,255,232,255,232,255,231,255,230,255,231,255,228,255,230,
        255,228,255,241,255,232,255,232,255,230,255,231,255,235,255,242,255,238,255,232,255,227,255,231,255,238,255,239,255,235,255,237,255,235,255,238,255,241,255,244,255,249,255,246,255,253,0,4,0,14,0,14,0,12,0,17,0,14,0,24,0,24,0,22,0,25,0,22,0,25,0,21,0,
        21,0,24,0,30,0,30,0,30,0,36,0,24,0,19,0,24,0,21,0,21,0,11,0,12,0,10,0,8,0,8,0,1,0,3,0,1,0,3,255,255,255,249,255,249,255,241,255,238,255,237,255,239,255,238,255,230,255,231,255,230,255,232,255,228,255,234,255,234,255,231,255,235,255,237,255,228,255,224,
        255,213,255,217,255,210,255,210,255,214,255,212,255,214,255,206,255,205,255,198,255,198,255,196,255,191,255,184,255,174,255,170,255,173,255,187,255,184,255,180,255,178,255,177,255,178,255,187,255,198,255,194,255,195,255,192,255,195,255,203,255,198,255,
        192,255,196,255,198,255,198,255,188,255,198,255,196,255,203,255,206,255,209,255,199,255,198,255,199,255,206,255,214,255,210,255,212,255,213,255,213,255,220,255,217,255,217,255,214,255,214,255,206,255,209,255,208,255,210,255,217,255,216,255,216,255,219,
        255,220,255,223,255,223,255,221,255,223,255,220,255,220,255,216,255,219,255,216,255,217,255,219,255,226,255,232,255,228,255,231,255,234,255,244,255,237,255,234,255,235,255,248,255,248,255,250,255,255,0,6,0,0,0,14,0,17,0,12,0,14,0,18,0,15,0,18,0,19,0,
        19,0,18,0,14,0,22,0,19,0,26,0,24,0,29,0,33,0,35,0,37,0,37,0,42,0,44,0,43,0,47,0,46,0,50,0,48,0,47,0,51,0,57,0,60,0,57,0,65,0,62,0,65,0,66,0,58,0,61,0,64,0,66,0,68,0,65,0,66,0,68,0,66,0,73,0,76,0,76,0,78,0,76,0,69,0,69,0,78,0,72,0,69,0,66,0,66,0,61,0,
        61,0,58,0,69,0,57,0,57,0,53,0,54,0,46,0,46,0,46,0,47,0,48,0,40,0,47,0,42,0,46,0,40,0,39,0,46,0,47,0,46,0,40,0,37,0,33,0,30,0,35,0,25,0,26,0,24,0,12,0,14,0,10,0,12,0,14,0,8,0,14,0,10,0,10,0,12,0,15,0,12,0,11,0,14,0,7,0,11,0,1,0,1,0,0,255,255,255,252,0,
        8,0,3,0,3,0,4,255,250,255,252,255,252,255,252,255,249,255,252,255,253,255,252,255,249,255,250,255,252,255,245,255,248,255,248,255,239,255,242,255,238,255,235,255,230,255,235,255,232,255,230,255,228,255,228,255,235,255,226,255,238,255,231,255,227,255,
        226,255,230,255,230,255,235,255,230,255,231,255,230,255,239,255,232,255,239,255,237,255,238,255,241,255,238,255,250,0,4,0,3,0,10,0,8,0,7,0,10,0,12,0,17,0,22,0,19,0,19,0,29,0,21,0,26,0,25,0,21,0,24,0,28,0,28,0,33,0,25,0,28,0,24,0,24,0,18,0,14,0,14,0,10,
        0,6,0,3,0,3,0,3,255,252,255,250,255,246,255,246,255,248,255,242,255,238,255,241,255,242,255,244,255,242,255,237,255,237,255,230,255,232,255,234,255,228,255,224,255,223,255,226,255,220,255,221,255,226,255,224,255,221,255,219,255,212,255,205,255,208,255,
        201,255,199,255,198,255,201,255,192,255,192,255,185,255,188,255,181,255,180,255,177,255,181,255,180,255,184,255,181,255,178,255,195,255,201,255,202,255,195,255,187,255,191,255,199,255,195,255,202,255,203,255,205,255,202,255,208,255,213,255,203,255,210,
        255,206,255,209,255,209,255,208,255,203,255,203,255,202,255,212,255,213,255,224,255,227,255,214,255,221,255,219,255,223,255,219,255,212,255,220,255,216,255,219,255,219,255,224,255,228,255,216,255,223,255,220,255,228,255,227,255,230,255,228,255,227,255,
        224,255,219,255,224,255,223,255,221,255,226,255,227,255,234,255,239,255,241,255,245,255,249,255,255,0,6,255,255,0,0,0,1,255,253,0,0,0,0,0,4,0,15,0,11,0,7,0,14,0,14,0,24,0,26,0,28,0,18,0,26,0,22,0,21,0,22,0,18,0,22,0,28,0,26,0,25,0,29,0,37,0,37,0,37,0,
        44,0,40,0,47,0,48,0,55,0,55,0,55,0,54,0,62,0,62,0,60,0,66,0,68,0,64,0,60,0,65,0,64,0,57,0,60,0,57,0,60,0,64,0,64,0,66,0,73,0,76,0,76,0,75,0,73,0,76,0,75,0,75,0,75,0,73,0,71,0,78,0,72,0,68,0,64,0,64,0,57,0,48,0,47,0,48,0,46,0,46,0,42,0,40,0,42,0,43,0,
        47,0,47,0,42,0,33,0,30,0,29,0,32,0,29,0,29,0,29,0,29,0,24,0,28,0,22,0,21,0,19,0,22,0,17,0,19,0,12,0,15,0,10,0,17,0,11,0,7,0,6,0,6,0,6,0,4,0,11,0,10,0,8,0,3,255,255,255,253,255,248,255,249,255,248,255,249,255,246,255,252,255,249,255,245,255,250,255,249,
        255,246,255,248,255,245,255,241,255,246,255,248,255,246,255,246,255,252,255,250,255,252,255,252,255,237,255,242,255,235,255,241,255,241,255,239,255,231,255,232,255,234,255,238,255,228,255,231,255,227,255,239,255,242,255,238,255,234,255,230,255,238,255,
        239,255,234,255,237,255,238,255,248,255,255,255,248,255,250,255,250,0,3,0,4,0,10,0,3,0,8,255,252,0,7,0,10,0,12,0,11,0,15,0,24,0,19,0,28,0,30,0,32,0,33,0,30,0,25,0,21,0,25,0,25,0,21,0,18,0,18,0,14,0,12,0,21,0,17,0,8,0,7,0,6,255,252,255,250,255,246,255,
        249,255,252,255,249,255,249,255,242,255,235,255,238,255,244,255,242,255,241,255,234,255,235,255,232,255,231,255,221,255,214,255,224,255,224,255,223,255,228,255,226,255,227,255,221,255,220,255,226,255,219,255,208,255,206,255,202,255,199,255,195,255,191,
        255,187,255,187,255,177,255,180,255,181,255,184,255,185,255,184,255,190,255,195,255,195,255,201,255,203,255,202,255,201,255,199,255,205,255,205,255,209,255,209,255,216,255,216,255,209,255,208,255,214,255,210,255,216,255,214,255,214,255,208,255,210,255,
        216,255,217,255,221,255,220,255,223,255,221,255,219,255,221,255,221,255,227,255,214,255,217,255,216,255,216,255,221,255,224,255,226,255,228,255,228,255,226,255,234,255,231,255,220,255,227,255,230,255,230,255,235,255,234,255,235,255,238,255,244,255,238,
        255,239,255,239,255,245,255,241,255,246,255,244,255,250,255,253,0,0,0,0,255,253,0,3,0,8,0,6,0,15,0,10,0,12,0,11,0,15,0,11,0,15,0,21,0,24,0,15,0,15,0,18,0,25,0,24,0,30,0,22,0,32,0,28,0,29,0,37,0,36,0,36,0,36,0,35,0,35,0,43,0,43,0,46,0,43,0,43,0,46,0,37,
        0,44,0,47,0,46,0,48,0,53,0,53,0,54,0,57,0,53,0,54,0,48,0,53,0,53,0,58,0,51,0,54,0,55,0,55,0,62,0,61,0,61,0,62,0,57,0,55,0,51,0,57,0,50,0,51,0,53,0,51,0,54,0,48,0,48,0,42,0,43,0,46,0,44,0,39,0,32,0,33,0,35,0,32,0,36,0,29,0,28,0,28,0,24,0,28,0,28,0,25,
        0,24,0,22,0,24,0,17,0,15,0,17,0,18,0,17,0,8,0,6,0,4,0,4,0,6,0,6,0,12,0,11,0,11,0,6,255,255,255,252,255,246,255,252,255,249,255,239,255,239,255,234,255,244,255,238,255,241,255,242,255,245,255,246,255,244,255,248,255,249,255,245,255,245,255,241,255,242,
        255,241,255,249,255,250,255,248,255,248,255,249,255,250,255,246,255,237,255,241,255,239,255,238,255,234,255,238,255,237,255,237,255,238,255,231,255,237,255,231,255,231,255,234,255,235,255,231,255,239,255,244,255,245,255,245,255,246,255,249,255,253,255,
        249,255,252,255,250,0,1,0,3,255,252,255,252,0,0,0,1,0,7,0,15,0,15,0,10,0,17,0,17,0,28,0,22,0,30,0,30,0,32,0,37,0,30,0,36,0,30,0,25,0,21,0,18,0,19,0,24,0,24,0,26,0,21,0,25,0,11,0,10,0,8,255,255,0,0,255,253,255,249,255,253,255,246,255,248,255,245,255,237,
        255,239,255,237,255,234,255,232,255,239,255,238,255,231,255,234,255,226,255,231,255,231,255,237,255,237,255,234,255,231,255,226,255,210,255,209,255,214,255,213,255,214,255,214,255,217,255,213,255,206,255,201,255,196,255,190,255,183,255,183,255,184,255,
        187,255,191,255,185,255,194,255,199,255,202,255,205,255,195,255,202,255,203,255,209,255,208,255,214,255,219,255,223,255,214,255,216,255,210,255,216,255,219,255,219,255,223,255,219,255,223,255,216,255,217,255,220,255,223,255,221,255,221,255,227,255,228,
        255,217,255,220,255,223,255,221,255,228,255,226,255,230,255,226,255,232,255,231,255,228,255,238,255,232,255,234,255,238,255,245,255,246,255,249,255,244,255,249,255,245,255,248,255,241,255,245,255,248,255,248,255,249,255,246,255,248,255,246,255,249,255,
        252,255,245,0,3,255,255,0,0,255,255,0,1,0,4,0,4,0,4,0,6,0,7,0,6,0,8,0,18,0,21,0,24,0,19,0,26,0,22,0,15,0,22,0,22,0,19,0,25,0,25,0,36,0,26,0,22,0,24,0,30,0,22,0,32,0,30,0,35,0,32,0,39,0,36,0,35,0,39,0,46,0,43,0,43,0,46,0,43,0,46,0,53,0,48,0,48,0,58,0,
        60,0,50,0,50,0,50,0,55,0,53,0,44,0,46,0,50,0,60,0,62,0,60,0,60,0,61,0,53,0,48,0,48,0,50,0,48,0,46,0,48,0,50,0,48,0,50,0,47,0,44,0,43,0,39,0,32,0,36,0,26,0,29,0,25,0,21,0,17,0,21,0,18,0,26,0,19,0,18,0,15,0,18,0,17,0,15,0,12,0,11,0,10,0,14,0,15,0,11,0,
        15,0,19,0,12,0,7,0,4,0,1,0,8,0,8,0,8,0,3,0,4,255,253,255,250,255,246,255,252,255,248,255,244,255,238,255,238,255,235,255,232,255,235,255,237,255,237,255,231,255,234,255,239,255,238,255,249,255,252,255,252,0,0,255,250,255,255,0,0,0,1,255,252,255,248,255,
        241,255,238,255,237,255,235,255,230,255,237,255,237,255,239,255,241,255,237,255,237,255,226,255,234,255,234,255,238,255,238,255,242,255,239,255,241,255,246,255,244,255,244,255,249,0,0,255,248,0,3,0,3,0,7,0,8,0,7,0,10,0,0,0,4,0,8,0,6,0,7,0,12,0,15,0,24,
        0,19,0,22,0,24,0,22,0,25,0,26,0,22,0,24,0,25,0,28,0,22,0,18,0,18,0,11,0,15,0,21,0,21,0,18,0,15,0,8,0,12,0,4,0,7,0,10,0,1,255,250,255,246,255,244,255,235,255,235,255,235,255,232,255,239,255,234,255,231,255,239,255,248,255,241,255,241,255,238,255,230,255,
        227,255,226,255,217,255,223,255,220,255,223,255,221,255,216,255,212,255,209,255,199,255,199,255,205,255,206,255,202,255,201,255,212,255,205,255,202,255,203,255,205,255,210,255,208,255,206,255,208,255,208,255,209,255,203,255,206,255,213,255,208,255,212,
        255,214,255,213,255,217,255,214,255,220,255,213,255,214,255,213,255,219,255,221,255,220,255,223,255,230,255,224,255,224,255,231,255,230,255,226,255,230,255,227,255,234,255,231,255,230,255,235,255,228,255,224,255,227,255,231,255,231,255,232,255,234,255,
        238,255,241,255,231,255,231,255,235,255,242,255,237,255,238,255,241,255,248,255,246,255,244,255,250,255,249,255,248,255,248,255,245,255,250,255,253,255,246,0,1,0,0,255,255,255,253,255,248,255,245,255,249,255,250,255,248,255,248,255,255,255,255,0,4,0,
        1,0,6,0,8,0,8,0,10,0,10,0,12,0,18,0,25,0,26,0,22,0,30,0,25,0,28,0,24,0,28,0,29,0,33,0,28,0,29,0,32,0,22,0,25,0,33,0,32,0,35,0,37,0,36,0,29,0,28,0,28,0,32,0,37,0,37,0,43,0,43,0,48,0,46,0,44,0,36,0,33,0,32,0,33,0,36,0,46,0,42,0,47,0,48,0,48,0,46,0,36,0,
        42,0,32,0,37,0,33,0,29,0,40,0,39,0,39,0,33,0,37,0,36,0,32,0,30,0,28,0,28,0,29,0,29,0,25,0,18,0,14,0,15,0,18,0,17,0,10,0,15,0,8,0,4,0,17,0,12,0,19,0,15,0,11,0,17,0,14,0,19,0,17,0,14,0,7,0,8,0,14,0,10,0,12,0,14,0,11,0,8,0,11,0,0,0,4,255,248,255,245,255,
        241,255,238,255,239,255,238,255,237,255,245,255,246,255,238,255,238,255,238,255,241,255,242,255,244,255,250,0,0,255,246,0,1,255,252,0,0,255,253,255,252,255,255,255,255,255,252,255,248,255,250,255,250,255,246,255,250,255,244,255,244,255,245,255,242,255,
        237,255,234,255,241,255,239,255,239,255,239,255,239,255,237,255,249,255,248,255,255,255,248,255,249,255,244,255,249,255,252,255,252,0,0,0,3,0,7,0,4,0,7,0,14,0,8,0,14,0,18,0,12,0,15,0,17,0,26,0,19,0,19,0,19,0,24,0,22,0,25,0,32,0,26,0,30,0,24,0,22,0,22,
        0,19,0,22,0,22,0,18,0,18,0,25,0,12,0,7,0,10,0,11,0,11,0,7,255,255,255,252,255,253,255,250,255,250,255,245,255,244,255,245,255,244,255,242,255,237,255,241,255,231,255,235,255,230,255,221,255,224,255,223,255,224,255,232,255,227,255,221,255,217,255,219,
        255,213,255,221,255,214,255,216,255,210,255,210,255,202,255,202,255,196,255,202,255,209,255,210,255,210,255,217,255,214,255,219,255,212,255,213,255,213,255,214,255,214,255,209,255,209,255,209,255,212,255,210,255,216,255,216,255,216,255,217,255,227,255,
        226,255,228,255,228,255,221,255,224,255,227,255,235,255,228,255,238,255,237,255,231,255,237,255,241,255,242,255,235,255,235,255,238,255,244,255,235,255,242,255,242,255,241,255,242,255,239,255,245,255,246,255,250,255,244,255,244,255,245,255,244,255,239,
        255,242,255,239,255,245,255,246,255,245,255,252,255,255,255,253,0,0,0,3,0,6,0,8,0,3,0,0,255,252,0,0,255,255,255,255,255,252,255,253,255,253,255,252,255,246,255,248,0,0,255,253,0,0,0,0,255,255,255,253,255,252,0,1,0,8,0,18,0,22,0,22,0,24,0,26,0,18,0,26,
        0,24,0,19,0,19,0,18,0,19,0,15,0,18,0,28,0,22,0,22,0,17,0,25,0,30,0,33,0,29,0,24,0,32,0,35,0,35,0,36,0,35,0,33,0,35,0,48,0,37,0,35,0,32,0,39,0,43,0,37,0,40,0,39,0,40,0,39,0,43,0,37,0,30,0,33,0,35,0,33,0,26,0,29,0,30,0,33,0,35,0,32,0,33,0,24,0,19,0,25,
        0,21,0,17,0,12,0,4,0,14,0,11,0,10,0,12,0,15,0,11,0,8,0,4,0,3,0,6,0,6,0,10,0,6,0,6,0,10,0,4,0,3,0,8,0,3,0,6,0,1,0,3,0,0,255,253,255,253,0,4,0,7,0,6,0,7,0,4,0,0,0,0,255,248,255,245,255,242,255,235,255,239,255,239,255,238,255,241,255,237,255,232,255,231,
        255,241,255,234,255,252,255,252,255,252,255,249,255,248,255,252,255,253,0,0,0,1,255,252,0,3,0,4,0,1,255,250,255,253,255,252,255,250,255,248,255,242,255,246,255,249,255,249,255,248,255,248,255,245,255,248,255,245,255,252,255,244,255,242,255,241,255,245,
        0,3,255,252,0,3,0,4,0,0,255,255,255,255,0,0,0,11,0,18,0,17,0,15,0,17,0,17,0,22,0,29,0,22,0,24,0,22,0,26,0,30,0,33,0,35,0,35,0,39,0,32,0,32,0,32,0,25,0,22,0,21,0,17,0,15,0,14,0,11,0,12,0,11,0,7,0,14,0,14,0,8,255,255,0,1,0,1,255,252,255,249,255,248,255,
        246,255,242,255,238,255,242,255,242,255,242,255,232,255,235,255,235,255,235,255,234,255,237,255,234,255,239,255,228,255,226,255,227,255,223,255,220,255,221,255,216,255,212,255,212,255,220,255,216,255,223,255,221,255,212,255,210,255,208,255,212,255,213,
        255,213,255,217,255,214,255,212,255,210,255,224,255,226,255,224,255,221,255,221,255,219,255,220,255,226,255,214,255,219,255,223,255,226,255,223,255,227,255,220,255,226,255,227,255,232,255,235,255,237,255,235,255,239,255,238,255,242,255,244,255,245,255,
        248,255,252,255,249,0,0,0,1,0,1,255,252,255,255,255,250,255,244,255,245,255,248,255,245,255,244,255,246,255,245,255,246,255,246,255,248,255,246,255,249,255,245,255,242,255,245,255,250,255,255,255,252,255,255,0,3,255,252,255,249,255,252,0,4,0,7,0,4,0,
        0,0,0,0,0,255,253,0,0,255,253,255,250,0,4,255,255,0,4,0,4,0,1,0,8,0,3,0,0,0,0,0,6,0,3,0,6,0,1,0,17,0,18,0,26,0,21,0,24,0,25,0,24,0,22,0,22,0,22,0,19,0,18,0,22,0,22,0,28,0,25,0,22,0,25,0,25,0,18,0,24,0,21,0,21,0,30,0,30,0,29,0,33,0,42,0,37,0,42,0,37,0,
        36,0,40,0,39,0,43,0,37,0,35,0,37,0,24,0,25,0,25,0,25,0,22,0,19,0,25,0,26,0,19,0,17,0,21,0,17,0,15,0,11,0,19,0,19,0,19,0,15,0,17,0,12,0,15,0,15,0,19,0,12,0,6,0,1,255,252,0,0,0,7,0,1,0,3,0,8,0,6,0,7,0,1,0,14,0,14,0,6,0,8,0,8,0,1,0,3,255,255,255,252,255,
        252,255,250,255,255,255,250,255,253,0,0,0,6,0,0,255,249,255,255,255,255,255,249,255,250,255,244,255,248,255,245,255,239,255,231,255,232,255,237,255,241,255,244,255,252,255,248,255,245,255,239,255,245,255,241,255,250,255,250,0,1,255,246,255,255,255,255,
        255,255,255,255,255,253,255,250,255,255,0,1,255,253,255,250,255,248,255,246,255,246,255,246,255,252,255,249,255,252,255,250,255,255,255,249,255,250,255,250,255,249,255,248,255,252,255,249,0,6,0,3,0,11,0,8,0,15,0,17,0,14,0,17,0,17,0,21,0,26,0,26,0,29,
        0,29,0,29,0,24,0,19,0,17,0,19,0,19,0,14,0,21,0,19,0,24,0,19,0,21,0,21,0,19,0,14,0,21,0,6,0,10,0,15,0,6,0,3,0,4,0,3,0,4,255,255,255,253,255,245,255,244,255,244,255,241,255,241,255,238,255,245,255,241,255,238,255,235,255,235,255,232,255,232,255,231,255,
        232,255,235,255,235,255,231,255,228,255,221,255,221,255,223,255,223,255,230,255,213,255,213,255,216,255,210,255,208,255,203,255,212,255,212,255,206,255,210,255,212,255,208,255,216,255,210,255,209,255,213,255,206,255,213,255,223,255,221,255,227,255,230,
        255,232,255,228,255,226,255,230,255,224,255,226,255,224,255,226,255,230,255,234,255,228,255,230,255,232,255,231,255,227,255,232,255,235,255,239,255,244,255,237,255,252,255,249,255,249,255,250,255,252,255,252,0,0,255,248,255,255,255,252,255,250,0,3,0,
        1,255,252,255,248,255,246,255,245,255,245,255,245,255,246,255,242,255,249,255,249,255,245,255,250,255,249,255,252,255,253,0,1,0,1,0,4,0,0,0,3,0,6,0,3,0,8,0,7,0,7,255,250,0,4,255,252,255,246,255,253,255,252,0,3,0,4,255,250,255,249,255,239,255,250,255,
        249,0,4,0,0,0,11,0,11,0,15,0,17,0,21,0,14,0,17,0,21,0,21,0,25,0,22,0,22,0,28,0,35,0,25,0,26,0,24,0,26,0,25,0,24,0,18,0,26,0,24,0,26,0,28,0,19,0,29,0,30,0,25,0,26,0,35,0,35,0,28,0,22,0,28,0,28,0,33,0,30,0,30,0,37,0,37,0,39,0,28,0,22,0,19,0,18,0,10,0,12,
        0,14,0,15,0,11,0,12,0,14,0,17,0,8,0,8,0,8,0,10,0,19,0,4,0,6,0,8,0,11,0,8,0,6,0,6,0,8,0,1,255,252,255,253,0,1,255,253,0,1,0,1,0,8,0,7,0,7,0,7,0,4,0,6,255,255,255,252,255,253,255,255,255,252,255,250,0,6,0,6,0,0,0,6,0,3,0,0,255,249,255,249,255,255,255,246,
        255,249,255,250,255,248,255,244,255,245,255,249,255,244,255,252,255,245,255,244,255,242,255,245,255,237,255,237,255,244,255,244,255,250,255,255,0,1,255,253,255,255,255,255,0,6,0,4,0,4,255,253,255,255,0,0,0,1,255,252,255,241,255,250,255,252,255,255,0,
        0,255,255,0,1,255,253,255,244,255,245,255,239,255,244,255,248,0,3,0,8,0,7,0,11,0,3,0,10,0,14,0,14,0,14,0,15,0,22,0,26,0,24,0,26,0,26,0,22,0,26,0,32,0,33,0,33,0,33,0,29,0,30,0,30,0,36,0,29,0,21,0,19,0,18,0,19,0,21,0,17,0,17,0,18,0,12,0,7,0,4,0,1,255,252,
        255,255,0,4,0,3,255,248,255,242,255,242,255,238,255,241,255,238,255,239,255,237,255,239,255,241,255,235,255,232,255,231,255,230,255,234,255,231,255,232,255,230,255,234,255,235,255,234,255,227,255,230,255,231,255,220,255,217,255,210,255,213,255,217,255,
        217,255,219,255,220,255,219,255,226,255,223,255,212,255,220,255,220,255,220,255,210,255,223,255,230,255,224,255,227,255,231,255,231,255,231,255,234,255,226,255,232,255,237,255,241,255,235,255,232,255,231,255,232,255,237,255,235,255,234,0,0 };
    };

    static const int cello_aiffSize = numElementsInArray (cello_aiff);
}

class AiffAudioFormatUnitTests : public AudioFormatUnitTests<AiffAudioFormat>
{
public:
    AiffAudioFormatUnitTests() {}

    InputStream* getValidTestFileData() override
    {
        return new MemoryInputStream (AiffUnitTestData::cello_aiff, AiffUnitTestData::cello_aiffSize, false);
    }

    StringPairArray createTestMetadata() override
    {
        StringPairArray metadataValues;

        static const char* const props[] =
        {
            AiffAudioFormat::appleBeats,
            AiffAudioFormat::appleDenominator,
            AiffAudioFormat::appleKey,
            AiffAudioFormat::appleNumerator,
            AiffAudioFormat::appleOneShot,
            AiffAudioFormat::appleOneShot,
            AiffAudioFormat::appleRootNote,
            AiffAudioFormat::appleRootSet,
            AiffAudioFormat::appleTag
        };

        for (int i = numElementsInArray (props); --i >= 0;)
            metadataValues.set (props[i], props[i]);

        return metadataValues;
    }

private:
    JUCE_DECLARE_NON_COPYABLE (AiffAudioFormatUnitTests)
};

static const AiffAudioFormatUnitTests aiffAudioFormatUnitTests;

#endif
