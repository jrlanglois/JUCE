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

static const char* const wavFormatName = "WAV file";

//==============================================================================
const char* const WavAudioFormat::bwavDescription      = "bwav description";
const char* const WavAudioFormat::bwavOriginator       = "bwav originator";
const char* const WavAudioFormat::bwavOriginatorRef    = "bwav originator ref";
const char* const WavAudioFormat::bwavOriginationDate  = "bwav origination date";
const char* const WavAudioFormat::bwavOriginationTime  = "bwav origination time";
const char* const WavAudioFormat::bwavTimeReference    = "bwav time reference";
const char* const WavAudioFormat::bwavCodingHistory    = "bwav coding history";

StringPairArray WavAudioFormat::createBWAVMetadata (const String& description,
                                                    const String& originator,
                                                    const String& originatorRef,
                                                    Time date,
                                                    const int64 timeReferenceSamples,
                                                    const String& codingHistory)
{
    StringPairArray m;

    m.set (bwavDescription, description);
    m.set (bwavOriginator, originator);
    m.set (bwavOriginatorRef, originatorRef);
    m.set (bwavOriginationDate, date.formatted ("%Y-%m-%d"));
    m.set (bwavOriginationTime, date.formatted ("%H:%M:%S"));
    m.set (bwavTimeReference, String (timeReferenceSamples));
    m.set (bwavCodingHistory, codingHistory);

    return m;
}

const char* const WavAudioFormat::acidOneShot          = "acid one shot";
const char* const WavAudioFormat::acidRootSet          = "acid root set";
const char* const WavAudioFormat::acidStretch          = "acid stretch";
const char* const WavAudioFormat::acidDiskBased        = "acid disk based";
const char* const WavAudioFormat::acidizerFlag         = "acidizer flag";
const char* const WavAudioFormat::acidRootNote         = "acid root note";
const char* const WavAudioFormat::acidBeats            = "acid beats";
const char* const WavAudioFormat::acidDenominator      = "acid denominator";
const char* const WavAudioFormat::acidNumerator        = "acid numerator";
const char* const WavAudioFormat::acidTempo            = "acid tempo";

const char* const WavAudioFormat::riffInfoArchivalLocation      = "IARL";
const char* const WavAudioFormat::riffInfoArtist                = "IART";
const char* const WavAudioFormat::riffInfoBaseURL               = "IBSU";
const char* const WavAudioFormat::riffInfoCinematographer       = "ICNM";
const char* const WavAudioFormat::riffInfoComment               = "CMNT";
const char* const WavAudioFormat::riffInfoComments              = "COMM";
const char* const WavAudioFormat::riffInfoCommissioned          = "ICMS";
const char* const WavAudioFormat::riffInfoCopyright             = "ICOP";
const char* const WavAudioFormat::riffInfoCostumeDesigner       = "ICDS";
const char* const WavAudioFormat::riffInfoCountry               = "ICNT";
const char* const WavAudioFormat::riffInfoCropped               = "ICRP";
const char* const WavAudioFormat::riffInfoDateCreated           = "ICRD";
const char* const WavAudioFormat::riffInfoDateTimeOriginal      = "IDIT";
const char* const WavAudioFormat::riffInfoDefaultAudioStream    = "ICAS";
const char* const WavAudioFormat::riffInfoDimension             = "IDIM";
const char* const WavAudioFormat::riffInfoDirectory             = "DIRC";
const char* const WavAudioFormat::riffInfoDistributedBy         = "IDST";
const char* const WavAudioFormat::riffInfoDotsPerInch           = "IDPI";
const char* const WavAudioFormat::riffInfoEditedBy              = "IEDT";
const char* const WavAudioFormat::riffInfoEighthLanguage        = "IAS8";
const char* const WavAudioFormat::riffInfoEncodedBy             = "CODE";
const char* const WavAudioFormat::riffInfoEndTimecode           = "TCDO";
const char* const WavAudioFormat::riffInfoEngineer              = "IENG";
const char* const WavAudioFormat::riffInfoFifthLanguage         = "IAS5";
const char* const WavAudioFormat::riffInfoFirstLanguage         = "IAS1";
const char* const WavAudioFormat::riffInfoFourthLanguage        = "IAS4";
const char* const WavAudioFormat::riffInfoGenre                 = "GENR";
const char* const WavAudioFormat::riffInfoKeywords              = "IKEY";
const char* const WavAudioFormat::riffInfoLanguage              = "LANG";
const char* const WavAudioFormat::riffInfoLength                = "TLEN";
const char* const WavAudioFormat::riffInfoLightness             = "ILGT";
const char* const WavAudioFormat::riffInfoLocation              = "LOCA";
const char* const WavAudioFormat::riffInfoLogoIconURL           = "ILIU";
const char* const WavAudioFormat::riffInfoLogoURL               = "ILGU";
const char* const WavAudioFormat::riffInfoMedium                = "IMED";
const char* const WavAudioFormat::riffInfoMoreInfoBannerImage   = "IMBI";
const char* const WavAudioFormat::riffInfoMoreInfoBannerURL     = "IMBU";
const char* const WavAudioFormat::riffInfoMoreInfoText          = "IMIT";
const char* const WavAudioFormat::riffInfoMoreInfoURL           = "IMIU";
const char* const WavAudioFormat::riffInfoMusicBy               = "IMUS";
const char* const WavAudioFormat::riffInfoNinthLanguage         = "IAS9";
const char* const WavAudioFormat::riffInfoNumberOfParts         = "PRT2";
const char* const WavAudioFormat::riffInfoOrganisation          = "TORG";
const char* const WavAudioFormat::riffInfoPart                  = "PRT1";
const char* const WavAudioFormat::riffInfoProducedBy            = "IPRO";
const char* const WavAudioFormat::riffInfoProductionDesigner    = "IPDS";
const char* const WavAudioFormat::riffInfoProductionStudio      = "ISDT";
const char* const WavAudioFormat::riffInfoRate                  = "RATE";
const char* const WavAudioFormat::riffInfoRated                 = "AGES";
const char* const WavAudioFormat::riffInfoRating                = "IRTD";
const char* const WavAudioFormat::riffInfoRippedBy              = "IRIP";
const char* const WavAudioFormat::riffInfoSecondaryGenre        = "ISGN";
const char* const WavAudioFormat::riffInfoSecondLanguage        = "IAS2";
const char* const WavAudioFormat::riffInfoSeventhLanguage       = "IAS7";
const char* const WavAudioFormat::riffInfoSharpness             = "ISHP";
const char* const WavAudioFormat::riffInfoSixthLanguage         = "IAS6";
const char* const WavAudioFormat::riffInfoSoftware              = "ISFT";
const char* const WavAudioFormat::riffInfoSoundSchemeTitle      = "DISP";
const char* const WavAudioFormat::riffInfoSource                = "ISRC";
const char* const WavAudioFormat::riffInfoSourceFrom            = "ISRF";
const char* const WavAudioFormat::riffInfoStarring_ISTR         = "ISTR";
const char* const WavAudioFormat::riffInfoStarring_STAR         = "STAR";
const char* const WavAudioFormat::riffInfoStartTimecode         = "TCOD";
const char* const WavAudioFormat::riffInfoStatistics            = "STAT";
const char* const WavAudioFormat::riffInfoSubject               = "ISBJ";
const char* const WavAudioFormat::riffInfoTapeName              = "TAPE";
const char* const WavAudioFormat::riffInfoTechnician            = "ITCH";
const char* const WavAudioFormat::riffInfoThirdLanguage         = "IAS3";
const char* const WavAudioFormat::riffInfoTimeCode              = "ISMP";
const char* const WavAudioFormat::riffInfoTitle                 = "INAM";
const char* const WavAudioFormat::riffInfoTrackNumber           = "TRCK";
const char* const WavAudioFormat::riffInfoURL                   = "TURL";
const char* const WavAudioFormat::riffInfoVegasVersionMajor     = "VMAJ";
const char* const WavAudioFormat::riffInfoVegasVersionMinor     = "VMIN";
const char* const WavAudioFormat::riffInfoVersion               = "TVER";
const char* const WavAudioFormat::riffInfoWatermarkURL          = "IWMU";
const char* const WavAudioFormat::riffInfoWrittenBy             = "IWRI";
const char* const WavAudioFormat::riffInfoYear                  = "YEAR";

const char* const WavAudioFormat::ISRC                 = "ISRC";
const char* const WavAudioFormat::tracktionLoopInfo    = "tracktion loop info";

//==============================================================================
namespace WavFileHelpers
{
    inline int chunkName (const char* const name) noexcept   { return (int) ByteOrder::littleEndianInt (name); }
    inline size_t roundUpSize (size_t sz) noexcept           { return (sz + 3) & ~3u; }

    #if JUCE_MSVC
     #pragma pack (push, 1)
    #endif

    struct BWAVChunk
    {
        char description [256];
        char originator [32];
        char originatorRef [32];
        char originationDate [10];
        char originationTime [8];
        uint32 timeRefLow;
        uint32 timeRefHigh;
        uint16 version;
        uint8 umid[64];
        uint8 reserved[190];
        char codingHistory[1];

        void copyTo (StringPairArray& values, const int totalSize) const
        {
            values.set (WavAudioFormat::bwavDescription,     String::fromUTF8 (description,     sizeof (description)));
            values.set (WavAudioFormat::bwavOriginator,      String::fromUTF8 (originator,      sizeof (originator)));
            values.set (WavAudioFormat::bwavOriginatorRef,   String::fromUTF8 (originatorRef,   sizeof (originatorRef)));
            values.set (WavAudioFormat::bwavOriginationDate, String::fromUTF8 (originationDate, sizeof (originationDate)));
            values.set (WavAudioFormat::bwavOriginationTime, String::fromUTF8 (originationTime, sizeof (originationTime)));

            const uint32 timeLow  = ByteOrder::swapIfBigEndian (timeRefLow);
            const uint32 timeHigh = ByteOrder::swapIfBigEndian (timeRefHigh);
            const int64 time = (((int64) timeHigh) << 32) + timeLow;

            values.set (WavAudioFormat::bwavTimeReference, String (time));
            values.set (WavAudioFormat::bwavCodingHistory,
                        String::fromUTF8 (codingHistory, totalSize - (int) offsetof (BWAVChunk, codingHistory)));
        }

        static MemoryBlock createFrom (const StringPairArray& values)
        {
            MemoryBlock data (roundUpSize (sizeof (BWAVChunk) + values [WavAudioFormat::bwavCodingHistory].getNumBytesAsUTF8()));
            data.fillWith (0);

            BWAVChunk* b = (BWAVChunk*) data.getData();

            // Allow these calls to overwrite an extra byte at the end, which is fine as long
            // as they get called in the right order..
            values [WavAudioFormat::bwavDescription]    .copyToUTF8 (b->description, 257);
            values [WavAudioFormat::bwavOriginator]     .copyToUTF8 (b->originator, 33);
            values [WavAudioFormat::bwavOriginatorRef]  .copyToUTF8 (b->originatorRef, 33);
            values [WavAudioFormat::bwavOriginationDate].copyToUTF8 (b->originationDate, 11);
            values [WavAudioFormat::bwavOriginationTime].copyToUTF8 (b->originationTime, 9);

            const int64 time = values [WavAudioFormat::bwavTimeReference].getLargeIntValue();
            b->timeRefLow = ByteOrder::swapIfBigEndian ((uint32) (time & 0xffffffff));
            b->timeRefHigh = ByteOrder::swapIfBigEndian ((uint32) (time >> 32));

            values [WavAudioFormat::bwavCodingHistory].copyToUTF8 (b->codingHistory, 0x7fffffff);

            if (b->description[0] != 0
                || b->originator[0] != 0
                || b->originationDate[0] != 0
                || b->originationTime[0] != 0
                || b->codingHistory[0] != 0
                || time != 0)
            {
                return data;
            }

            return MemoryBlock();
        }

    } JUCE_PACKED;

    //==============================================================================
    struct SMPLChunk
    {
        struct SampleLoop
        {
            uint32 identifier;
            uint32 type; // these are different in AIFF and WAV
            uint32 start;
            uint32 end;
            uint32 fraction;
            uint32 playCount;
        } JUCE_PACKED;

        uint32 manufacturer;
        uint32 product;
        uint32 samplePeriod;
        uint32 midiUnityNote;
        uint32 midiPitchFraction;
        uint32 smpteFormat;
        uint32 smpteOffset;
        uint32 numSampleLoops;
        uint32 samplerData;
        SampleLoop loops[1];

        template <typename NameType>
        static void setValue (StringPairArray& values, NameType name, uint32 val)
        {
            values.set (name, String (ByteOrder::swapIfBigEndian (val)));
        }

        static void setValue (StringPairArray& values, int prefix, const char* name, uint32 val)
        {
            setValue (values, "Loop" + String (prefix) + name, val);
        }

        void copyTo (StringPairArray& values, const int totalSize) const
        {
            setValue (values, "Manufacturer",      manufacturer);
            setValue (values, "Product",           product);
            setValue (values, "SamplePeriod",      samplePeriod);
            setValue (values, "MidiUnityNote",     midiUnityNote);
            setValue (values, "MidiPitchFraction", midiPitchFraction);
            setValue (values, "SmpteFormat",       smpteFormat);
            setValue (values, "SmpteOffset",       smpteOffset);
            setValue (values, "NumSampleLoops",    numSampleLoops);
            setValue (values, "SamplerData",       samplerData);

            for (int i = 0; i < (int) numSampleLoops; ++i)
            {
                if ((uint8*) (loops + (i + 1)) > ((uint8*) this) + totalSize)
                    break;

                setValue (values, i, "Identifier", loops[i].identifier);
                setValue (values, i, "Type",       loops[i].type);
                setValue (values, i, "Start",      loops[i].start);
                setValue (values, i, "End",        loops[i].end);
                setValue (values, i, "Fraction",   loops[i].fraction);
                setValue (values, i, "PlayCount",  loops[i].playCount);
            }
        }

        template <typename NameType>
        static uint32 getValue (const StringPairArray& values, NameType name, const char* def)
        {
            return ByteOrder::swapIfBigEndian ((uint32) values.getValue (name, def).getIntValue());
        }

        static uint32 getValue (const StringPairArray& values, int prefix, const char* name, const char* def)
        {
            return getValue (values, "Loop" + String (prefix) + name, def);
        }

        static MemoryBlock createFrom (const StringPairArray& values)
        {
            MemoryBlock data;
            const int numLoops = jmin (64, values.getValue ("NumSampleLoops", "0").getIntValue());

            if (numLoops > 0)
            {
                data.setSize (roundUpSize (sizeof (SMPLChunk) + (size_t) (numLoops - 1) * sizeof (SampleLoop)), true);

                SMPLChunk* const s = static_cast<SMPLChunk*> (data.getData());

                s->manufacturer      = getValue (values, "Manufacturer", "0");
                s->product           = getValue (values, "Product", "0");
                s->samplePeriod      = getValue (values, "SamplePeriod", "0");
                s->midiUnityNote     = getValue (values, "MidiUnityNote", "60");
                s->midiPitchFraction = getValue (values, "MidiPitchFraction", "0");
                s->smpteFormat       = getValue (values, "SmpteFormat", "0");
                s->smpteOffset       = getValue (values, "SmpteOffset", "0");
                s->numSampleLoops    = ByteOrder::swapIfBigEndian ((uint32) numLoops);
                s->samplerData       = getValue (values, "SamplerData", "0");

                for (int i = 0; i < numLoops; ++i)
                {
                    SampleLoop& loop = s->loops[i];
                    loop.identifier = getValue (values, i, "Identifier", "0");
                    loop.type       = getValue (values, i, "Type", "0");
                    loop.start      = getValue (values, i, "Start", "0");
                    loop.end        = getValue (values, i, "End", "0");
                    loop.fraction   = getValue (values, i, "Fraction", "0");
                    loop.playCount  = getValue (values, i, "PlayCount", "0");
                }
            }

            return data;
        }
    } JUCE_PACKED;

    //==============================================================================
    struct InstChunk
    {
        int8 baseNote;
        int8 detune;
        int8 gain;
        int8 lowNote;
        int8 highNote;
        int8 lowVelocity;
        int8 highVelocity;

        static void setValue (StringPairArray& values, const char* name, int val)
        {
            values.set (name, String (val));
        }

        void copyTo (StringPairArray& values) const
        {
            setValue (values, "MidiUnityNote",  baseNote);
            setValue (values, "Detune",         detune);
            setValue (values, "Gain",           gain);
            setValue (values, "LowNote",        lowNote);
            setValue (values, "HighNote",       highNote);
            setValue (values, "LowVelocity",    lowVelocity);
            setValue (values, "HighVelocity",   highVelocity);
        }

        static int8 getValue (const StringPairArray& values, const char* name, const char* def)
        {
            return (int8) values.getValue (name, def).getIntValue();
        }

        static MemoryBlock createFrom (const StringPairArray& values)
        {
            MemoryBlock data;
            const StringArray& keys = values.getAllKeys();

            if (keys.contains ("LowNote", true) && keys.contains ("HighNote", true))
            {
                data.setSize (8, true);
                InstChunk* const inst = static_cast<InstChunk*> (data.getData());

                inst->baseNote      = getValue (values, "MidiUnityNote", "60");
                inst->detune        = getValue (values, "Detune", "0");
                inst->gain          = getValue (values, "Gain", "0");
                inst->lowNote       = getValue (values, "LowNote", "0");
                inst->highNote      = getValue (values, "HighNote", "127");
                inst->lowVelocity   = getValue (values, "LowVelocity", "1");
                inst->highVelocity  = getValue (values, "HighVelocity", "127");
            }

            return data;
        }
    } JUCE_PACKED;

    //==============================================================================
    struct CueChunk
    {
        struct Cue
        {
            uint32 identifier;
            uint32 order;
            uint32 chunkID;
            uint32 chunkStart;
            uint32 blockStart;
            uint32 offset;
        } JUCE_PACKED;

        uint32 numCues;
        Cue cues[1];

        static void setValue (StringPairArray& values, int prefix, const char* name, uint32 val)
        {
            values.set ("Cue" + String (prefix) + name, String (ByteOrder::swapIfBigEndian (val)));
        }

        void copyTo (StringPairArray& values, const int totalSize) const
        {
            values.set ("NumCuePoints", String (ByteOrder::swapIfBigEndian (numCues)));

            for (int i = 0; i < (int) numCues; ++i)
            {
                if ((uint8*) (cues + (i + 1)) > ((uint8*) this) + totalSize)
                    break;

                setValue (values, i, "Identifier",  cues[i].identifier);
                setValue (values, i, "Order",       cues[i].order);
                setValue (values, i, "ChunkID",     cues[i].chunkID);
                setValue (values, i, "ChunkStart",  cues[i].chunkStart);
                setValue (values, i, "BlockStart",  cues[i].blockStart);
                setValue (values, i, "Offset",      cues[i].offset);
            }
        }

        static MemoryBlock createFrom (const StringPairArray& values)
        {
            MemoryBlock data;
            const int numCues = values.getValue ("NumCuePoints", "0").getIntValue();

            if (numCues > 0)
            {
                data.setSize (roundUpSize (sizeof (CueChunk) + (size_t) (numCues - 1) * sizeof (Cue)), true);

                CueChunk* const c = static_cast<CueChunk*> (data.getData());

                c->numCues = ByteOrder::swapIfBigEndian ((uint32) numCues);

                const String dataChunkID (chunkName ("data"));
                int nextOrder = 0;

               #if JUCE_DEBUG
                Array<uint32> identifiers;
               #endif

                for (int i = 0; i < numCues; ++i)
                {
                    const String prefix ("Cue" + String (i));
                    const uint32 identifier = (uint32) values.getValue (prefix + "Identifier", "0").getIntValue();

                   #if JUCE_DEBUG
                    jassert (! identifiers.contains (identifier));
                    identifiers.add (identifier);
                   #endif

                    const int order = values.getValue (prefix + "Order", String (nextOrder)).getIntValue();
                    nextOrder = jmax (nextOrder, order) + 1;

                    Cue& cue = c->cues[i];
                    cue.identifier   = ByteOrder::swapIfBigEndian ((uint32) identifier);
                    cue.order        = ByteOrder::swapIfBigEndian ((uint32) order);
                    cue.chunkID      = ByteOrder::swapIfBigEndian ((uint32) values.getValue (prefix + "ChunkID", dataChunkID).getIntValue());
                    cue.chunkStart   = ByteOrder::swapIfBigEndian ((uint32) values.getValue (prefix + "ChunkStart", "0").getIntValue());
                    cue.blockStart   = ByteOrder::swapIfBigEndian ((uint32) values.getValue (prefix + "BlockStart", "0").getIntValue());
                    cue.offset       = ByteOrder::swapIfBigEndian ((uint32) values.getValue (prefix + "Offset", "0").getIntValue());
                }
            }

            return data;
        }

    } JUCE_PACKED;

    //==============================================================================
    namespace ListChunk
    {
        static int getValue (const StringPairArray& values, const String& name)
        {
            return values.getValue (name, "0").getIntValue();
        }

        static int getValue (const StringPairArray& values, const String& prefix, const char* name)
        {
            return getValue (values, prefix + name);
        }

        static void appendLabelOrNoteChunk (const StringPairArray& values, const String& prefix,
                                            const int chunkType, MemoryOutputStream& out)
        {
            const String label (values.getValue (prefix + "Text", prefix));
            const int labelLength = (int) label.getNumBytesAsUTF8() + 1;
            const int chunkLength = 4 + labelLength + (labelLength & 1);

            out.writeInt (chunkType);
            out.writeInt (chunkLength);
            out.writeInt (getValue (values, prefix, "Identifier"));
            out.write (label.toUTF8(), (size_t) labelLength);

            if ((out.getDataSize() & 1) != 0)
                out.writeByte (0);
        }

        static void appendExtraChunk (const StringPairArray& values, const String& prefix, MemoryOutputStream& out)
        {
            const String text (values.getValue (prefix + "Text", prefix));

            const int textLength = (int) text.getNumBytesAsUTF8() + 1; // include null terminator
            int chunkLength = textLength + 20 + (textLength & 1);

            out.writeInt (chunkName ("ltxt"));
            out.writeInt (chunkLength);
            out.writeInt (getValue (values, prefix, "Identifier"));
            out.writeInt (getValue (values, prefix, "SampleLength"));
            out.writeInt (getValue (values, prefix, "Purpose"));
            out.writeShort ((short) getValue (values, prefix, "Country"));
            out.writeShort ((short) getValue (values, prefix, "Language"));
            out.writeShort ((short) getValue (values, prefix, "Dialect"));
            out.writeShort ((short) getValue (values, prefix, "CodePage"));
            out.write (text.toUTF8(), (size_t) textLength);

            if ((out.getDataSize() & 1) != 0)
                out.writeByte (0);
        }

        static MemoryBlock createFrom (const StringPairArray& values)
        {
            const int numCueLabels  = getValue (values, "NumCueLabels");
            const int numCueNotes   = getValue (values, "NumCueNotes");
            const int numCueRegions = getValue (values, "NumCueRegions");

            MemoryOutputStream out;

            if (numCueLabels + numCueNotes + numCueRegions > 0)
            {
                out.writeInt (chunkName ("adtl"));

                for (int i = 0; i < numCueLabels; ++i)
                    appendLabelOrNoteChunk (values, "CueLabel" + String (i), chunkName ("labl"), out);

                for (int i = 0; i < numCueNotes; ++i)
                    appendLabelOrNoteChunk (values, "CueNote" + String (i), chunkName ("note"), out);

                for (int i = 0; i < numCueRegions; ++i)
                    appendExtraChunk (values, "CueRegion" + String (i), out);
            }

            return out.getMemoryBlock();
        }
    }

    //==============================================================================
    /** Reads a RIFF List Info chunk from a stream positioned just after the size byte. */
    namespace ListInfoChunk
    {
        static const char* const types[] =
        {
            WavAudioFormat::riffInfoArchivalLocation,
            WavAudioFormat::riffInfoArtist,
            WavAudioFormat::riffInfoBaseURL,
            WavAudioFormat::riffInfoCinematographer,
            WavAudioFormat::riffInfoComment,
            WavAudioFormat::riffInfoComments,
            WavAudioFormat::riffInfoCommissioned,
            WavAudioFormat::riffInfoCopyright,
            WavAudioFormat::riffInfoCostumeDesigner,
            WavAudioFormat::riffInfoCountry,
            WavAudioFormat::riffInfoCropped,
            WavAudioFormat::riffInfoDateCreated,
            WavAudioFormat::riffInfoDateTimeOriginal,
            WavAudioFormat::riffInfoDefaultAudioStream,
            WavAudioFormat::riffInfoDimension,
            WavAudioFormat::riffInfoDirectory,
            WavAudioFormat::riffInfoDistributedBy,
            WavAudioFormat::riffInfoDotsPerInch,
            WavAudioFormat::riffInfoEditedBy,
            WavAudioFormat::riffInfoEighthLanguage,
            WavAudioFormat::riffInfoEncodedBy,
            WavAudioFormat::riffInfoEndTimecode,
            WavAudioFormat::riffInfoEngineer,
            WavAudioFormat::riffInfoFifthLanguage,
            WavAudioFormat::riffInfoFirstLanguage,
            WavAudioFormat::riffInfoFourthLanguage,
            WavAudioFormat::riffInfoGenre,
            WavAudioFormat::riffInfoKeywords,
            WavAudioFormat::riffInfoLanguage,
            WavAudioFormat::riffInfoLength,
            WavAudioFormat::riffInfoLightness,
            WavAudioFormat::riffInfoLocation,
            WavAudioFormat::riffInfoLogoIconURL,
            WavAudioFormat::riffInfoLogoURL,
            WavAudioFormat::riffInfoMedium,
            WavAudioFormat::riffInfoMoreInfoBannerImage,
            WavAudioFormat::riffInfoMoreInfoBannerURL,
            WavAudioFormat::riffInfoMoreInfoText,
            WavAudioFormat::riffInfoMoreInfoURL,
            WavAudioFormat::riffInfoMusicBy,
            WavAudioFormat::riffInfoNinthLanguage,
            WavAudioFormat::riffInfoNumberOfParts,
            WavAudioFormat::riffInfoOrganisation,
            WavAudioFormat::riffInfoPart,
            WavAudioFormat::riffInfoProducedBy,
            WavAudioFormat::riffInfoProductionDesigner,
            WavAudioFormat::riffInfoProductionStudio,
            WavAudioFormat::riffInfoRate,
            WavAudioFormat::riffInfoRated,
            WavAudioFormat::riffInfoRating,
            WavAudioFormat::riffInfoRippedBy,
            WavAudioFormat::riffInfoSecondaryGenre,
            WavAudioFormat::riffInfoSecondLanguage,
            WavAudioFormat::riffInfoSeventhLanguage,
            WavAudioFormat::riffInfoSharpness,
            WavAudioFormat::riffInfoSixthLanguage,
            WavAudioFormat::riffInfoSoftware,
            WavAudioFormat::riffInfoSoundSchemeTitle,
            WavAudioFormat::riffInfoSource,
            WavAudioFormat::riffInfoSourceFrom,
            WavAudioFormat::riffInfoStarring_ISTR,
            WavAudioFormat::riffInfoStarring_STAR,
            WavAudioFormat::riffInfoStartTimecode,
            WavAudioFormat::riffInfoStatistics,
            WavAudioFormat::riffInfoSubject,
            WavAudioFormat::riffInfoTapeName,
            WavAudioFormat::riffInfoTechnician,
            WavAudioFormat::riffInfoThirdLanguage,
            WavAudioFormat::riffInfoTimeCode,
            WavAudioFormat::riffInfoTitle,
            WavAudioFormat::riffInfoTrackNumber,
            WavAudioFormat::riffInfoURL,
            WavAudioFormat::riffInfoVegasVersionMajor,
            WavAudioFormat::riffInfoVegasVersionMinor,
            WavAudioFormat::riffInfoVersion,
            WavAudioFormat::riffInfoWatermarkURL,
            WavAudioFormat::riffInfoWrittenBy,
            WavAudioFormat::riffInfoYear
        };

        static bool isMatchingTypeIgnoringCase (const int value, const char* const name) noexcept
        {
            for (int i = 0; i < 4; ++i)
                if ((juce_wchar) name[i] != CharacterFunctions::toUpperCase ((juce_wchar) ((value >> (i * 8)) & 0xff)))
                    return false;

            return true;
        }

        static void addToMetadata (StringPairArray& values, InputStream& input, int64 chunkEnd)
        {
            while (input.getPosition() < chunkEnd)
            {
                const int infoType = input.readInt();

                int64 infoLength = chunkEnd - input.getPosition();

                if (infoLength > 0)
                {
                    infoLength = jmin (infoLength, (int64) input.readInt());

                    if (infoLength <= 0)
                        return;

                    for (int i = 0; i < numElementsInArray (types); ++i)
                    {
                        if (isMatchingTypeIgnoringCase (infoType, types[i]))
                        {
                            MemoryBlock mb;
                            input.readIntoMemoryBlock (mb, (ssize_t) infoLength);
                            values.set (types[i], String::createStringFromData ((const char*) mb.getData(),
                                                                                (int) mb.getSize()));
                            break;
                        }
                    }
                }
            }
        }

        static bool writeValue (const StringPairArray& values, MemoryOutputStream& out, const char* paramName)
        {
            const String value (values.getValue (paramName, String()));

            if (value.isEmpty())
                return false;

            const int valueLength = (int) value.getNumBytesAsUTF8() + 1;
            const int chunkLength = valueLength + (valueLength & 1);

            out.writeInt (chunkName (paramName));
            out.writeInt (chunkLength);
            out.write (value.toUTF8(), (size_t) valueLength);

            if ((out.getDataSize() & 1) != 0)
                out.writeByte (0);

            return true;
        }

        static MemoryBlock createFrom (const StringPairArray& values)
        {
            MemoryOutputStream out;
            out.writeInt (chunkName ("INFO"));
            bool anyParamsDefined = false;

            for (int i = 0; i < numElementsInArray (types); ++i)
                if (writeValue (values, out, types[i]))
                    anyParamsDefined = true;

            return anyParamsDefined ? out.getMemoryBlock() : MemoryBlock();
        }
    }

    //==============================================================================
    struct AcidChunk
    {
        /** Reads an acid RIFF chunk from a stream positioned just after the size byte. */
        AcidChunk (InputStream& input, size_t length)
        {
            zerostruct (*this);
            input.read (this, (int) jmin (sizeof (*this), length));
        }

        AcidChunk (const StringPairArray& values)
        {
            zerostruct (*this);

            flags = getFlagIfPresent (values, WavAudioFormat::acidOneShot,   0x01)
                  | getFlagIfPresent (values, WavAudioFormat::acidRootSet,   0x02)
                  | getFlagIfPresent (values, WavAudioFormat::acidStretch,   0x04)
                  | getFlagIfPresent (values, WavAudioFormat::acidDiskBased, 0x08)
                  | getFlagIfPresent (values, WavAudioFormat::acidizerFlag,  0x10);

            if (values[WavAudioFormat::acidRootSet].getIntValue() != 0)
                rootNote = ByteOrder::swapIfBigEndian ((uint16) values[WavAudioFormat::acidRootNote].getIntValue());

            numBeats          = ByteOrder::swapIfBigEndian ((uint32) values[WavAudioFormat::acidBeats].getIntValue());
            meterDenominator  = ByteOrder::swapIfBigEndian ((uint16) values[WavAudioFormat::acidDenominator].getIntValue());
            meterNumerator    = ByteOrder::swapIfBigEndian ((uint16) values[WavAudioFormat::acidNumerator].getIntValue());

            if (values.containsKey (WavAudioFormat::acidTempo))
                tempo = swapFloatByteOrder (values[WavAudioFormat::acidTempo].getFloatValue());
        }

        static MemoryBlock createFrom (const StringPairArray& values)
        {
            return AcidChunk (values).toMemoryBlock();
        }

        MemoryBlock toMemoryBlock() const
        {
            return (flags != 0 || rootNote != 0 || numBeats != 0 || meterDenominator != 0 || meterNumerator != 0)
                      ? MemoryBlock (this, sizeof (*this)) : MemoryBlock();
        }

        void addToMetadata (StringPairArray& values) const
        {
            setBoolFlag (values, WavAudioFormat::acidOneShot,   0x01);
            setBoolFlag (values, WavAudioFormat::acidRootSet,   0x02);
            setBoolFlag (values, WavAudioFormat::acidStretch,   0x04);
            setBoolFlag (values, WavAudioFormat::acidDiskBased, 0x08);
            setBoolFlag (values, WavAudioFormat::acidizerFlag,  0x10);

            if (flags & 0x02) // root note set
                values.set (WavAudioFormat::acidRootNote, String (ByteOrder::swapIfBigEndian (rootNote)));

            values.set (WavAudioFormat::acidBeats,       String (ByteOrder::swapIfBigEndian (numBeats)));
            values.set (WavAudioFormat::acidDenominator, String (ByteOrder::swapIfBigEndian (meterDenominator)));
            values.set (WavAudioFormat::acidNumerator,   String (ByteOrder::swapIfBigEndian (meterNumerator)));
            values.set (WavAudioFormat::acidTempo,       String (swapFloatByteOrder (tempo)));
        }

        void setBoolFlag (StringPairArray& values, const char* name, uint32 mask) const
        {
            values.set (name, (flags & ByteOrder::swapIfBigEndian (mask)) ? "1" : "0");
        }

        static uint32 getFlagIfPresent (const StringPairArray& values, const char* name, uint32 flag)
        {
            return values[name].getIntValue() != 0 ? ByteOrder::swapIfBigEndian (flag) : 0;
        }

        static float swapFloatByteOrder (const float x) noexcept
        {
           #ifdef JUCE_BIG_ENDIAN
            union { uint32 asInt; float asFloat; } n;
            n.asFloat = x;
            n.asInt = ByteOrder::swap (n.asInt);
            return n.asFloat;
           #else
            return x;
           #endif
        }

        uint32 flags;
        uint16 rootNote;
        uint16 reserved1;
        float reserved2;
        uint32 numBeats;
        uint16 meterDenominator;
        uint16 meterNumerator;
        float tempo;

    } JUCE_PACKED;

    //==============================================================================
    struct TracktionChunk
    {
        static MemoryBlock createFrom (const StringPairArray& values)
        {
            MemoryOutputStream out;
            const String s (values[WavAudioFormat::tracktionLoopInfo]);

            if (s.isNotEmpty())
            {
                out.writeString (s);

                if ((out.getDataSize() & 1) != 0)
                    out.writeByte (0);
            }

            return out.getMemoryBlock();
        }
    };

    //==============================================================================
    namespace AXMLChunk
    {
        static void addToMetadata (StringPairArray& destValues, const String& source)
        {
            ScopedPointer<XmlElement> xml (XmlDocument::parse (source));

            if (xml != nullptr && xml->hasTagName ("ebucore:ebuCoreMain"))
            {
                if (XmlElement* xml2 = xml->getChildByName ("ebucore:coreMetadata"))
                {
                    if (XmlElement* xml3 = xml2->getChildByName ("ebucore:identifier"))
                    {
                        if (XmlElement* xml4 = xml3->getChildByName ("dc:identifier"))
                        {
                            const String ISRCCode (xml4->getAllSubText().fromFirstOccurrenceOf ("ISRC:", false, true));

                            if (ISRCCode.isNotEmpty())
                                destValues.set (WavAudioFormat::ISRC, ISRCCode);
                        }
                    }
                }
            }
        }

        static MemoryBlock createFrom (const StringPairArray& values)
        {
            const String ISRC (values.getValue (WavAudioFormat::ISRC, String()));
            MemoryOutputStream xml;

            if (ISRC.isNotEmpty())
            {
                xml << "<ebucore:ebuCoreMain xmlns:dc=\" http://purl.org/dc/elements/1.1/\" "
                                            "xmlns:ebucore=\"urn:ebu:metadata-schema:ebuCore_2012\">"
                         "<ebucore:coreMetadata>"
                           "<ebucore:identifier typeLabel=\"GUID\" "
                                               "typeDefinition=\"Globally Unique Identifier\" "
                                               "formatLabel=\"ISRC\" "
                                               "formatDefinition=\"International Standard Recording Code\" "
                                               "formatLink=\"http://www.ebu.ch/metadata/cs/ebu_IdentifierTypeCodeCS.xml#3.7\">"
                             "<dc:identifier>ISRC:" << ISRC << "</dc:identifier>"
                           "</ebucore:identifier>"
                         "</ebucore:coreMetadata>"
                       "</ebucore:ebuCoreMain>";

                xml.writeRepeatedByte (0, xml.getDataSize());  // ensures even size, null termination and room for future growing
            }

            return xml.getMemoryBlock();
        }
    };

    //==============================================================================
    struct ExtensibleWavSubFormat
    {
        uint32 data1;
        uint16 data2;
        uint16 data3;
        uint8  data4[8];

        bool operator== (const ExtensibleWavSubFormat& other) const noexcept   { return memcmp (this, &other, sizeof (*this)) == 0; }
        bool operator!= (const ExtensibleWavSubFormat& other) const noexcept   { return ! operator== (other); }

    } JUCE_PACKED;

    static const ExtensibleWavSubFormat pcmFormat       = { 0x00000001, 0x0000, 0x0010, { 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71 } };
    static const ExtensibleWavSubFormat IEEEFloatFormat = { 0x00000003, 0x0000, 0x0010, { 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71 } };
    static const ExtensibleWavSubFormat ambisonicFormat = { 0x00000001, 0x0721, 0x11d3, { 0x86, 0x44, 0xC8, 0xC1, 0xCA, 0x00, 0x00, 0x00 } };

    struct DataSize64Chunk   // chunk ID = 'ds64' if data size > 0xffffffff, 'JUNK' otherwise
    {
        uint32 riffSizeLow;     // low 4 byte size of RF64 block
        uint32 riffSizeHigh;    // high 4 byte size of RF64 block
        uint32 dataSizeLow;     // low 4 byte size of data chunk
        uint32 dataSizeHigh;    // high 4 byte size of data chunk
        uint32 sampleCountLow;  // low 4 byte sample count of fact chunk
        uint32 sampleCountHigh; // high 4 byte sample count of fact chunk
        uint32 tableLength;     // number of valid entries in array 'table'
    } JUCE_PACKED;

    #if JUCE_MSVC
     #pragma pack (pop)
    #endif
}

//==============================================================================
class WavAudioFormatReader  : public AudioFormatReader
{
public:
    WavAudioFormatReader (InputStream* const in)
        : AudioFormatReader (in, wavFormatName),
          bwavChunkStart (0),
          bwavSize (0),
          dataLength (0),
          isRF64 (false)
    {
        using namespace WavFileHelpers;
        uint64 len = 0;
        uint64 end = 0;
        int cueNoteIndex = 0;
        int cueLabelIndex = 0;
        int cueRegionIndex = 0;

        const int firstChunkType = input->readInt();

        if (firstChunkType == chunkName ("RF64"))
        {
            input->skipNextBytes (4); // size is -1 for RF64
            isRF64 = true;
        }
        else if (firstChunkType == chunkName ("RIFF"))
        {
            len = (uint64) (uint32) input->readInt();
            end = len + (uint64) input->getPosition();
        }
        else
        {
            return;
        }

        const int64 startOfRIFFChunk = input->getPosition();

        if (input->readInt() == chunkName ("WAVE"))
        {
            if (isRF64 && input->readInt() == chunkName ("ds64"))
            {
                const uint32 length = (uint32) input->readInt();

                if (length < 28)
                    return;

                const int64 chunkEnd = input->getPosition() + length + (length & 1);
                len = (uint64) input->readInt64();
                end = len + (uint64) startOfRIFFChunk;
                dataLength = input->readInt64();
                input->setPosition (chunkEnd);
            }

            while ((uint64) input->getPosition() < end && ! input->isExhausted())
            {
                const int chunkType = input->readInt();
                uint32 length = (uint32) input->readInt();
                const int64 chunkEnd = input->getPosition() + length + (length & 1);

                if (chunkType == chunkName ("fmt "))
                {
                    // read the format chunk
                    const unsigned short format = (unsigned short) input->readShort();
                    numChannels = (unsigned int) input->readShort();
                    sampleRate = input->readInt();
                    const int bytesPerSec = input->readInt();
                    input->skipNextBytes (2);
                    bitsPerSample = (unsigned int) (int) input->readShort();

                    if (bitsPerSample > 64)
                    {
                        bytesPerFrame = bytesPerSec / (int) sampleRate;
                        bitsPerSample = 8 * (unsigned int) bytesPerFrame / numChannels;
                    }
                    else
                    {
                        bytesPerFrame = numChannels * bitsPerSample / 8;
                    }

                    if (format == 3)
                    {
                        usesFloatingPointData = true;
                    }
                    else if (format == 0xfffe /*WAVE_FORMAT_EXTENSIBLE*/)
                    {
                        if (length < 40) // too short
                        {
                            bytesPerFrame = 0;
                        }
                        else
                        {
                            input->skipNextBytes (4); // skip over size and bitsPerSample
                            metadataValues.set ("ChannelMask", String (input->readInt()));

                            ExtensibleWavSubFormat subFormat;
                            subFormat.data1 = (uint32) input->readInt();
                            subFormat.data2 = (uint16) input->readShort();
                            subFormat.data3 = (uint16) input->readShort();
                            input->read (subFormat.data4, sizeof (subFormat.data4));

                            if (subFormat == IEEEFloatFormat)
                                usesFloatingPointData = true;
                            else if (subFormat != pcmFormat && subFormat != ambisonicFormat)
                                bytesPerFrame = 0;
                        }
                    }
                    else if (format != 1)
                    {
                        bytesPerFrame = 0;
                    }
                }
                else if (chunkType == chunkName ("data"))
                {
                    if (! isRF64) // data size is expected to be -1, actual data size is in ds64 chunk
                        dataLength = length;

                    dataChunkStart = input->getPosition();
                    lengthInSamples = (bytesPerFrame > 0) ? (dataLength / bytesPerFrame) : 0;
                }
                else if (chunkType == chunkName ("bext"))
                {
                    bwavChunkStart = input->getPosition();
                    bwavSize = length;

                    HeapBlock<BWAVChunk> bwav;
                    bwav.calloc (jmax ((size_t) length + 1, sizeof (BWAVChunk)), 1);
                    input->read (bwav, (int) length);
                    bwav->copyTo (metadataValues, (int) length);
                }
                else if (chunkType == chunkName ("smpl"))
                {
                    HeapBlock<SMPLChunk> smpl;
                    smpl.calloc (jmax ((size_t) length + 1, sizeof (SMPLChunk)), 1);
                    input->read (smpl, (int) length);
                    smpl->copyTo (metadataValues, (int) length);
                }
                else if (chunkType == chunkName ("inst") || chunkType == chunkName ("INST")) // need to check which...
                {
                    HeapBlock<InstChunk> inst;
                    inst.calloc (jmax ((size_t) length + 1, sizeof (InstChunk)), 1);
                    input->read (inst, (int) length);
                    inst->copyTo (metadataValues);
                }
                else if (chunkType == chunkName ("cue "))
                {
                    HeapBlock<CueChunk> cue;
                    cue.calloc (jmax ((size_t) length + 1, sizeof (CueChunk)), 1);
                    input->read (cue, (int) length);
                    cue->copyTo (metadataValues, (int) length);
                }
                else if (chunkType == chunkName ("axml"))
                {
                    MemoryBlock axml;
                    input->readIntoMemoryBlock (axml, (ssize_t) length);
                    AXMLChunk::addToMetadata (metadataValues, axml.toString());
                }
                else if (chunkType == chunkName ("LIST"))
                {
                    const int subChunkType = input->readInt();

                    if (subChunkType == chunkName ("info") || subChunkType == chunkName ("INFO"))
                    {
                        ListInfoChunk::addToMetadata (metadataValues, *input, chunkEnd);
                    }
                    else if (subChunkType == chunkName ("adtl"))
                    {
                        while (input->getPosition() < chunkEnd)
                        {
                            const int adtlChunkType = input->readInt();
                            const uint32 adtlLength = (uint32) input->readInt();
                            const int64 adtlChunkEnd = input->getPosition() + (adtlLength + (adtlLength & 1));

                            if (adtlChunkType == chunkName ("labl") || adtlChunkType == chunkName ("note"))
                            {
                                String prefix;

                                if (adtlChunkType == chunkName ("labl"))
                                    prefix << "CueLabel" << cueLabelIndex++;
                                else if (adtlChunkType == chunkName ("note"))
                                    prefix << "CueNote" << cueNoteIndex++;

                                const uint32 identifier = (uint32) input->readInt();
                                const int stringLength = (int) adtlLength - 4;

                                MemoryBlock textBlock;
                                input->readIntoMemoryBlock (textBlock, stringLength);

                                metadataValues.set (prefix + "Identifier", String (identifier));
                                metadataValues.set (prefix + "Text", textBlock.toString());
                            }
                            else if (adtlChunkType == chunkName ("ltxt"))
                            {
                                const String prefix ("CueRegion" + String (cueRegionIndex++));
                                const uint32 identifier     = (uint32) input->readInt();
                                const uint32 sampleLength   = (uint32) input->readInt();
                                const uint32 purpose        = (uint32) input->readInt();
                                const uint16 country        = (uint16) input->readInt();
                                const uint16 language       = (uint16) input->readInt();
                                const uint16 dialect        = (uint16) input->readInt();
                                const uint16 codePage       = (uint16) input->readInt();
                                const uint32 stringLength   = adtlLength - 20;

                                MemoryBlock textBlock;
                                input->readIntoMemoryBlock (textBlock, (int) stringLength);

                                metadataValues.set (prefix + "Identifier",   String (identifier));
                                metadataValues.set (prefix + "SampleLength", String (sampleLength));
                                metadataValues.set (prefix + "Purpose",      String (purpose));
                                metadataValues.set (prefix + "Country",      String (country));
                                metadataValues.set (prefix + "Language",     String (language));
                                metadataValues.set (prefix + "Dialect",      String (dialect));
                                metadataValues.set (prefix + "CodePage",     String (codePage));
                                metadataValues.set (prefix + "Text",         textBlock.toString());
                            }

                            input->setPosition (adtlChunkEnd);
                        }
                    }
                }
                else if (chunkType == chunkName ("acid"))
                {
                    AcidChunk (*input, length).addToMetadata (metadataValues);
                }
                else if (chunkType == chunkName ("Trkn"))
                {
                    MemoryBlock tracktion;
                    input->readIntoMemoryBlock (tracktion, (ssize_t) length);
                    metadataValues.set (WavAudioFormat::tracktionLoopInfo, tracktion.toString());
                }
                else if (chunkEnd <= input->getPosition())
                {
                    break;
                }

                input->setPosition (chunkEnd);
            }
        }

        if (cueLabelIndex > 0)          metadataValues.set ("NumCueLabels",    String (cueLabelIndex));
        if (cueNoteIndex > 0)           metadataValues.set ("NumCueNotes",     String (cueNoteIndex));
        if (cueRegionIndex > 0)         metadataValues.set ("NumCueRegions",   String (cueRegionIndex));
        if (metadataValues.size() > 0)  metadataValues.set ("MetaDataSource",  "WAV");
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

            copySampleData (bitsPerSample, usesFloatingPointData,
                            destSamples, startOffsetInDestBuffer, numDestChannels,
                            tempBuffer, (int) numChannels, numThisTime);

            startOffsetInDestBuffer += numThisTime;
            numSamples -= numThisTime;
        }

        return true;
    }

    static void copySampleData (unsigned int bitsPerSample, const bool usesFloatingPointData,
                                int* const* destSamples, int startOffsetInDestBuffer, int numDestChannels,
                                const void* sourceData, int numChannels, int numSamples) noexcept
    {
        switch (bitsPerSample)
        {
            case 8:     ReadHelper<AudioData::Int32, AudioData::UInt8, AudioData::LittleEndian>::read (destSamples, startOffsetInDestBuffer, numDestChannels, sourceData, numChannels, numSamples); break;
            case 16:    ReadHelper<AudioData::Int32, AudioData::Int16, AudioData::LittleEndian>::read (destSamples, startOffsetInDestBuffer, numDestChannels, sourceData, numChannels, numSamples); break;
            case 24:    ReadHelper<AudioData::Int32, AudioData::Int24, AudioData::LittleEndian>::read (destSamples, startOffsetInDestBuffer, numDestChannels, sourceData, numChannels, numSamples); break;
            case 32:    if (usesFloatingPointData) ReadHelper<AudioData::Float32, AudioData::Float32, AudioData::LittleEndian>::read (destSamples, startOffsetInDestBuffer, numDestChannels, sourceData, numChannels, numSamples);
                        else                       ReadHelper<AudioData::Int32,   AudioData::Int32,   AudioData::LittleEndian>::read (destSamples, startOffsetInDestBuffer, numDestChannels, sourceData, numChannels, numSamples); break;
            default:    jassertfalse; break;
        }
    }

    int64 bwavChunkStart, bwavSize;
    int64 dataChunkStart, dataLength;
    int bytesPerFrame;
    bool isRF64;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WavAudioFormatReader)
};

//==============================================================================
class WavAudioFormatWriter  : public AudioFormatWriter
{
public:
    WavAudioFormatWriter (OutputStream* const out, const double rate,
                          const unsigned int numChans, const unsigned int bits,
                          const StringPairArray& metadataValues)
        : AudioFormatWriter (out, wavFormatName, rate, numChans, bits),
          lengthInSamples (0),
          bytesWritten (0),
          writeFailed (false)
    {
        using namespace WavFileHelpers;

        if (metadataValues.size() > 0)
        {
            // The meta data should have been sanitised for the WAV format.
            // If it was originally sourced from an AIFF file the MetaDataSource
            // key should be removed (or set to "WAV") once this has been done
            jassert (metadataValues.getValue ("MetaDataSource", "None") != "AIFF");

            bwavChunk     = BWAVChunk::createFrom (metadataValues);
            axmlChunk     = AXMLChunk::createFrom (metadataValues);
            smplChunk     = SMPLChunk::createFrom (metadataValues);
            instChunk     = InstChunk::createFrom (metadataValues);
            cueChunk      = CueChunk ::createFrom (metadataValues);
            listChunk     = ListChunk::createFrom (metadataValues);
            listInfoChunk = ListInfoChunk::createFrom (metadataValues);
            acidChunk     = AcidChunk::createFrom (metadataValues);
            trckChunk     = TracktionChunk::createFrom (metadataValues);
        }

        headerPosition = out->getPosition();
        writeHeader();
    }

    ~WavAudioFormatWriter()
    {
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
            case 8:     WriteHelper<AudioData::UInt8, AudioData::Int32, AudioData::LittleEndian>::write (tempBlock.getData(), (int) numChannels, data, numSamples); break;
            case 16:    WriteHelper<AudioData::Int16, AudioData::Int32, AudioData::LittleEndian>::write (tempBlock.getData(), (int) numChannels, data, numSamples); break;
            case 24:    WriteHelper<AudioData::Int24, AudioData::Int32, AudioData::LittleEndian>::write (tempBlock.getData(), (int) numChannels, data, numSamples); break;
            case 32:    WriteHelper<AudioData::Int32, AudioData::Int32, AudioData::LittleEndian>::write (tempBlock.getData(), (int) numChannels, data, numSamples); break;
            default:    jassertfalse; break;
        }

        if (! output->write (tempBlock.getData(), bytes))
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

    bool flush() override
    {
        const int64 lastWritePos = output->getPosition();
        writeHeader();

        if (output->setPosition (lastWritePos))
            return true;

        // if this fails, you've given it an output stream that can't seek! It needs
        // to be able to seek back to write the header
        jassertfalse;
        return false;
    }

private:
    MemoryBlock tempBlock, bwavChunk, axmlChunk, smplChunk, instChunk, cueChunk, listChunk, listInfoChunk, acidChunk, trckChunk;
    uint64 lengthInSamples, bytesWritten;
    int64 headerPosition;
    bool writeFailed;

    static int getChannelMask (const int numChannels) noexcept
    {
        switch (numChannels)
        {
            case 1:   return 0;
            case 2:   return 1 + 2;                                 // SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT
            case 3:   return 1 + 2 + 4;                             // SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_FRONT_CENTER
            case 4:   return 1 + 2 + 16 + 32;                       // SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_BACK_LEFT | SPEAKER_BACK_RIGHT
            case 5:   return 1 + 2 + 4 + 16 + 32;                   // SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_FRONT_CENTER | SPEAKER_BACK_LEFT | SPEAKER_BACK_RIGHT
            case 6:   return 1 + 2 + 4 + 8 + 16 + 32;               // SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_FRONT_CENTER | SPEAKER_LOW_FREQUENCY | SPEAKER_BACK_LEFT | SPEAKER_BACK_RIGHT
            case 7:   return 1 + 2 + 4 + 16 + 32 + 512 + 1024;      // SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_FRONT_CENTER | SPEAKER_BACK_LEFT | SPEAKER_BACK_RIGHT | SPEAKER_SIDE_LEFT | SPEAKER_SIDE_RIGHT
            case 8:   return 1 + 2 + 4 + 8 + 16 + 32 + 512 + 1024;  // SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_FRONT_CENTER | SPEAKER_LOW_FREQUENCY | SPEAKER_BACK_LEFT | SPEAKER_BACK_RIGHT | SPEAKER_SIDE_LEFT | SPEAKER_SIDE_RIGHT
            default:  break;
        }

        return 0;
    }

    void writeHeader()
    {
        if ((bytesWritten & 1) != 0) // pad to an even length
            output->writeByte (0);

        using namespace WavFileHelpers;

        if (headerPosition != output->getPosition() && ! output->setPosition (headerPosition))
        {
            // if this fails, you've given it an output stream that can't seek! It needs to be
            // able to seek back to go back and write the header after the data has been written.
            jassertfalse;
            return;
        }

        const size_t bytesPerFrame = numChannels * bitsPerSample / 8;
        uint64 audioDataSize = bytesPerFrame * lengthInSamples;

        const bool isRF64 = (bytesWritten >= 0x100000000LL);
        const bool isWaveFmtEx = isRF64 || (numChannels > 2);

        int64 riffChunkSize = (int64) (4 /* 'RIFF' */ + 8 + 40 /* WAVEFORMATEX */
                                       + 8 + audioDataSize + (audioDataSize & 1)
                                       + chunkSize (bwavChunk)
                                       + chunkSize (axmlChunk)
                                       + chunkSize (smplChunk)
                                       + chunkSize (instChunk)
                                       + chunkSize (cueChunk)
                                       + chunkSize (listChunk)
                                       + chunkSize (listInfoChunk)
                                       + chunkSize (acidChunk)
                                       + chunkSize (trckChunk)
                                       + (8 + 28)); // (ds64 chunk)

        riffChunkSize += (riffChunkSize & 1);

        if (isRF64)
            writeChunkHeader (chunkName ("RF64"), -1);
        else
            writeChunkHeader (chunkName ("RIFF"), (int) riffChunkSize);

        output->writeInt (chunkName ("WAVE"));

        if (! isRF64)
        {
           #if ! JUCE_WAV_DO_NOT_PAD_HEADER_SIZE
            /* NB: This junk chunk is added for padding, so that the header is a fixed size
               regardless of whether it's RF64 or not. That way, we can begin recording a file,
               and when it's finished, can go back and write either a RIFF or RF64 header,
               depending on whether more than 2^32 samples were written.

               The JUCE_WAV_DO_NOT_PAD_HEADER_SIZE macro allows you to disable this feature in case
               you need to create files for crappy WAV players with bugs that stop them skipping chunks
               which they don't recognise. But DO NOT USE THIS option unless you really have no choice,
               because it means that if you write more than 2^32 samples to the file, you'll corrupt it.
            */
            writeChunkHeader (chunkName ("JUNK"), 28 + (isWaveFmtEx? 0 : 24));
            output->writeRepeatedByte (0, 28 /* ds64 */ + (isWaveFmtEx? 0 : 24));
           #endif
        }
        else
        {
           #if JUCE_WAV_DO_NOT_PAD_HEADER_SIZE
            // If you disable padding, then you MUST NOT write more than 2^32 samples to a file.
            jassertfalse;
           #endif

            writeChunkHeader (chunkName ("ds64"), 28);  // chunk size for uncompressed data (no table)
            output->writeInt64 (riffChunkSize);
            output->writeInt64 ((int64) audioDataSize);
            output->writeRepeatedByte (0, 12);
        }

        if (isWaveFmtEx)
        {
            writeChunkHeader (chunkName ("fmt "), 40);
            output->writeShort ((short) (uint16) 0xfffe); // WAVE_FORMAT_EXTENSIBLE
        }
        else
        {
            writeChunkHeader (chunkName ("fmt "), 16);
            output->writeShort (bitsPerSample < 32 ? (short) 1 /*WAVE_FORMAT_PCM*/
                                                   : (short) 3 /*WAVE_FORMAT_IEEE_FLOAT*/);
        }

        output->writeShort ((short) numChannels);
        output->writeInt ((int) sampleRate);
        output->writeInt ((int) (bytesPerFrame * sampleRate)); // nAvgBytesPerSec
        output->writeShort ((short) bytesPerFrame); // nBlockAlign
        output->writeShort ((short) bitsPerSample); // wBitsPerSample

        if (isWaveFmtEx)
        {
            output->writeShort (22); // cbSize (size of the extension)
            output->writeShort ((short) bitsPerSample); // wValidBitsPerSample
            output->writeInt (getChannelMask ((int) numChannels));

            const ExtensibleWavSubFormat& subFormat = bitsPerSample < 32 ? pcmFormat : IEEEFloatFormat;

            output->writeInt ((int) subFormat.data1);
            output->writeShort ((short) subFormat.data2);
            output->writeShort ((short) subFormat.data3);
            output->write (subFormat.data4, sizeof (subFormat.data4));
        }

        writeChunk (bwavChunk,     chunkName ("bext"));
        writeChunk (axmlChunk,     chunkName ("axml"));
        writeChunk (smplChunk,     chunkName ("smpl"));
        writeChunk (instChunk,     chunkName ("inst"), 7);
        writeChunk (cueChunk,      chunkName ("cue "));
        writeChunk (listChunk,     chunkName ("LIST"));
        writeChunk (listInfoChunk, chunkName ("LIST"));
        writeChunk (acidChunk,     chunkName ("acid"));
        writeChunk (trckChunk,     chunkName ("Trkn"));

        writeChunkHeader (chunkName ("data"), isRF64 ? -1 : (int) (lengthInSamples * bytesPerFrame));

        usesFloatingPointData = (bitsPerSample == 32);
    }

    static size_t chunkSize (const MemoryBlock& data) noexcept     { return data.getSize() > 0 ? (8 + data.getSize()) : 0; }

    void writeChunkHeader (int chunkType, int size) const
    {
        output->writeInt (chunkType);
        output->writeInt (size);
    }

    void writeChunk (const MemoryBlock& data, int chunkType, int size = 0) const
    {
        if (data.getSize() > 0)
        {
            writeChunkHeader (chunkType, size != 0 ? size : (int) data.getSize());
            *output << data;
        }
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WavAudioFormatWriter)
};

//==============================================================================
class MemoryMappedWavReader   : public MemoryMappedAudioFormatReader
{
public:
    MemoryMappedWavReader (const File& wavFile, const WavAudioFormatReader& reader)
        : MemoryMappedAudioFormatReader (wavFile, reader, reader.dataChunkStart,
                                         reader.dataLength, reader.bytesPerFrame)
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

        WavAudioFormatReader::copySampleData (bitsPerSample, usesFloatingPointData,
                                              destSamples, startOffsetInDestBuffer, numDestChannels,
                                              sampleToPointer (startSampleInFile), (int) numChannels, numSamples);
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
    template <typename SampleType>
    void scanMinAndMax (int64 startSampleInFile, int64 numSamples, Range<float>* results, int numChannelsToRead) const noexcept
    {
        for (int i = 0; i < numChannelsToRead; ++i)
            results[i] = scanMinAndMaxInterleaved<SampleType, AudioData::LittleEndian> (i, startSampleInFile, numSamples);
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MemoryMappedWavReader)
};

//==============================================================================
WavAudioFormat::WavAudioFormat()  : AudioFormat (wavFormatName, ".wav .bwf") {}
WavAudioFormat::~WavAudioFormat() {}

Array<int> WavAudioFormat::getPossibleSampleRates()
{
    const int rates[] = { 8000, 11025, 12000, 16000, 22050, 32000, 44100,
                          48000, 88200, 96000, 176400, 192000, 352800, 384000 };

    return Array<int> (rates, numElementsInArray (rates));
}

Array<int> WavAudioFormat::getPossibleBitDepths()
{
    const int depths[] = { 8, 16, 24, 32 };

    return Array<int> (depths, numElementsInArray (depths));
}

bool WavAudioFormat::canDoStereo()  { return true; }
bool WavAudioFormat::canDoMono()    { return true; }

AudioFormatReader* WavAudioFormat::createReaderFor (InputStream* sourceStream,
                                                    const bool deleteStreamIfOpeningFails)
{
    ScopedPointer<WavAudioFormatReader> r (new WavAudioFormatReader (sourceStream));

    if (r->sampleRate > 0 && r->numChannels > 0 && r->bytesPerFrame > 0)
        return r.release();

    if (! deleteStreamIfOpeningFails)
        r->input = nullptr;

    return nullptr;
}

MemoryMappedAudioFormatReader* WavAudioFormat::createMemoryMappedReader (const File& file)
{
    return createMemoryMappedReader (file.createInputStream());
}

MemoryMappedAudioFormatReader* WavAudioFormat::createMemoryMappedReader (FileInputStream* fin)
{
    if (fin != nullptr)
    {
        WavAudioFormatReader reader (fin);

        if (reader.lengthInSamples > 0)
            return new MemoryMappedWavReader (fin->getFile(), reader);
    }

    return nullptr;
}

AudioFormatWriter* WavAudioFormat::createWriterFor (OutputStream* out, double sampleRate,
                                                    unsigned int numChannels, int bitsPerSample,
                                                    const StringPairArray& metadataValues, int /*qualityOptionIndex*/)
{
    if (out != nullptr
        && numChannels > 0
        && getPossibleBitDepths().contains (bitsPerSample)
        && getPossibleSampleRates().contains ((int) sampleRate))
    {
        return new WavAudioFormatWriter (out, sampleRate, (unsigned int) numChannels,
                                         (unsigned int) bitsPerSample, metadataValues);
    }

    return nullptr;
}

namespace WavFileHelpers
{
    static bool slowCopyWavFileWithNewMetadata (const File& file, const StringPairArray& metadata)
    {
        TemporaryFile tempFile (file);

        WavAudioFormat wav;
        ScopedPointer<AudioFormatReader> reader (wav.createReaderFor (file.createInputStream(), true));

        if (reader != nullptr)
        {
            ScopedPointer<OutputStream> outStream (tempFile.getFile().createOutputStream());

            if (outStream != nullptr)
            {
                ScopedPointer<AudioFormatWriter> writer (wav.createWriterFor (outStream, reader->sampleRate,
                                                                              reader->numChannels, (int) reader->bitsPerSample,
                                                                              metadata, 0));

                if (writer != nullptr)
                {
                    outStream.release();

                    bool ok = writer->writeFromAudioReader (*reader, 0, -1);
                    writer = nullptr;
                    reader = nullptr;

                    return ok && tempFile.overwriteTargetFileWithTemporary();
                }
            }
        }

        return false;
    }
}

bool WavAudioFormat::replaceMetadataInFile (const File& wavFile, const StringPairArray& newMetadata)
{
    using namespace WavFileHelpers;
    ScopedPointer<WavAudioFormatReader> reader (static_cast<WavAudioFormatReader*> (createReaderFor (wavFile.createInputStream(), true)));

    if (reader != nullptr)
    {
        const int64 bwavPos  = reader->bwavChunkStart;
        const int64 bwavSize = reader->bwavSize;
        reader = nullptr;

        if (bwavSize > 0)
        {
            MemoryBlock chunk (BWAVChunk::createFrom (newMetadata));

            if (chunk.getSize() <= (size_t) bwavSize)
            {
                // the new one will fit in the space available, so write it directly..
                const int64 oldSize = wavFile.getSize();

                {
                    FileOutputStream out (wavFile);

                    if (! out.failedToOpen())
                    {
                        out.setPosition (bwavPos);
                        out << chunk;
                        out.setPosition (oldSize);
                    }
                }

                jassert (wavFile.getSize() == oldSize);

                return true;
            }
        }
    }

    return slowCopyWavFileWithNewMetadata (wavFile, newMetadata);
}

//==============================================================================
#if JUCE_UNIT_TESTS

namespace WaveUnitTestData
{
    static const uint8 cello_wav[] =
    {
        82,73,70,70,4,181,0,0,87,65,86,69,102,109,116,32,16,0,0,0,1,0,1,0,34,86,0,0,68,172,0,0,2,0,16,0,98,101,120,116,92,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,116,114,97,99,107,116,105,111,110,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,50,48,48,54,45,49,48,45,48,52,49,50,58,49,52,58,48,51,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,100,97,116,97,124,178,0,0,0,0,0,0,250,255,248,255,0,0,253,255,255,255,0,0,253,255,249,255,249,255,248,255,252,255,249,255,0,0,4,0,1,0,255,
        255,4,0,7,0,255,255,4,0,249,255,252,255,1,0,255,255,0,0,255,255,255,255,3,0,3,0,1,0,4,0,252,255,4,0,253,255,255,255,250,255,248,255,241,255,248,255,3,0,6,0,6,0,1,0,7,0,1,0,253,255,253,255,246,255,245,255,242,255,246,255,252,255,252,255,249,255,1,0,255,
        255,0,0,255,255,255,255,253,255,3,0,0,0,253,255,250,255,249,255,245,255,246,255,249,255,248,255,255,255,3,0,6,0,0,0,255,255,250,255,4,0,7,0,6,0,0,0,255,255,249,255,252,255,0,0,0,0,6,0,3,0,255,255,0,0,250,255,0,0,3,0,4,0,3,0,1,0,4,0,4,0,7,0,3,0,255,255,
        255,255,246,255,248,255,253,255,3,0,253,255,1,0,252,255,4,0,253,255,6,0,3,0,253,255,253,255,252,255,255,255,253,255,253,255,0,0,255,255,0,0,0,0,3,0,7,0,4,0,1,0,255,255,242,255,250,255,255,255,8,0,0,0,3,0,0,0,3,0,0,0,249,255,248,255,250,255,0,0,252,255,
        0,0,253,255,250,255,248,255,253,255,0,0,252,255,0,0,1,0,252,255,0,0,250,255,253,255,0,0,4,0,0,0,250,255,255,255,253,255,252,255,3,0,253,255,248,255,0,0,252,255,0,0,249,255,244,255,249,255,252,255,255,255,249,255,253,255,246,255,250,255,248,255,1,0,6,
        0,3,0,0,0,253,255,246,255,250,255,255,255,246,255,249,255,1,0,255,255,253,255,1,0,3,0,6,0,1,0,250,255,255,255,252,255,1,0,253,255,0,0,255,255,253,255,0,0,253,255,250,255,250,255,253,255,248,255,255,255,250,255,248,255,0,0,253,255,252,255,6,0,3,0,3,0,
        250,255,255,255,255,255,250,255,249,255,253,255,1,0,4,0,6,0,4,0,255,255,253,255,253,255,255,255,255,255,250,255,1,0,253,255,250,255,255,255,249,255,252,255,255,255,255,255,6,0,6,0,8,0,4,0,255,255,1,0,1,0,3,0,255,255,4,0,4,0,1,0,250,255,3,0,253,255,253,
        255,4,0,250,255,255,255,255,255,255,255,249,255,248,255,255,255,3,0,248,255,3,0,248,255,0,0,4,0,4,0,253,255,252,255,252,255,246,255,248,255,250,255,250,255,253,255,6,0,3,0,6,0,4,0,6,0,7,0,12,0,6,0,10,0,15,0,14,0,6,0,6,0,255,255,248,255,252,255,1,0,3,
        0,8,0,3,0,3,0,8,0,0,0,1,0,10,0,10,0,7,0,6,0,6,0,10,0,6,0,4,0,252,255,4,0,11,0,18,0,14,0,10,0,15,0,11,0,10,0,14,0,15,0,17,0,18,0,17,0,17,0,17,0,11,0,12,0,11,0,17,0,12,0,14,0,7,0,10,0,12,0,11,0,4,0,3,0,253,255,255,255,1,0,0,0,250,255,253,255,6,0,4,0,7,
        0,7,0,4,0,1,0,3,0,1,0,3,0,252,255,246,255,248,255,249,255,252,255,0,0,0,0,253,255,255,255,250,255,246,255,249,255,253,255,8,0,6,0,25,0,19,0,19,0,18,0,18,0,24,0,24,0,25,0,26,0,30,0,26,0,22,0,28,0,15,0,15,0,8,0,11,0,10,0,7,0,25,0,24,0,32,0,44,0,43,0,57,
        0,51,0,53,0,48,0,46,0,40,0,35,0,29,0,24,0,21,0,8,0,0,0,250,255,239,255,237,255,228,255,227,255,223,255,228,255,227,255,228,255,228,255,239,255,239,255,242,255,244,255,245,255,245,255,237,255,223,255,214,255,202,255,191,255,174,255,162,255,155,255,138,
        255,138,255,130,255,127,255,120,255,111,255,111,255,108,255,105,255,102,255,98,255,99,255,99,255,97,255,99,255,104,255,113,255,127,255,145,255,151,255,165,255,169,255,180,255,192,255,187,255,180,255,166,255,162,255,156,255,154,255,148,255,149,255,156,
        255,172,255,187,255,196,255,188,255,194,255,188,255,191,255,181,255,181,255,180,255,173,255,170,255,167,255,156,255,141,255,112,255,87,255,57,255,26,255,0,255,231,254,216,254,214,254,211,254,220,254,229,254,239,254,254,254,29,255,69,255,104,255,133,255,
        142,255,166,255,195,255,210,255,232,255,250,255,4,0,253,255,235,255,209,255,180,255,165,255,162,255,165,255,170,255,185,255,191,255,184,255,183,255,173,255,154,255,127,255,104,255,99,255,112,255,133,255,156,255,170,255,176,255,173,255,136,255,115,255,
        79,255,52,255,37,255,37,255,50,255,62,255,65,255,65,255,72,255,51,255,11,255,218,254,185,254,171,254,196,254,3,255,79,255,167,255,3,0,89,0,169,0,233,0,33,1,78,1,132,1,191,1,243,1,28,2,52,2,48,2,20,2,222,1,141,1,35,1,172,0,54,0,221,255,158,255,130,255,
        142,255,185,255,238,255,44,0,101,0,147,0,150,0,127,0,76,0,7,0,195,255,98,255,254,254,175,254,131,254,105,254,98,254,108,254,119,254,135,254,152,254,163,254,178,254,195,254,221,254,254,254,18,255,15,255,254,254,209,254,141,254,72,254,23,254,244,253,6,
        254,38,254,110,254,213,254,58,255,142,255,220,255,28,0,83,0,134,0,180,0,220,0,22,1,90,1,161,1,236,1,54,2,126,2,194,2,250,2,42,3,84,3,107,3,132,3,143,3,143,3,139,3,127,3,91,3,70,3,41,3,244,2,187,2,147,2,104,2,84,2,79,2,77,2,78,2,85,2,93,2,84,2,54,2,10,
        2,202,1,144,1,81,1,14,1,194,0,126,0,72,0,55,0,60,0,105,0,157,0,216,0,14,1,54,1,96,1,117,1,136,1,155,1,154,1,164,1,175,1,180,1,194,1,222,1,3,2,49,2,125,2,211,2,39,3,103,3,159,3,186,3,172,3,128,3,75,3,19,3,200,2,125,2,50,2,248,1,179,1,117,1,54,1,248,0,
        183,0,118,0,61,0,24,0,0,0,255,255,7,0,43,0,90,0,148,0,213,0,28,1,108,1,193,1,10,2,75,2,147,2,211,2,15,3,67,3,95,3,117,3,141,3,142,3,131,3,109,3,85,3,55,3,15,3,226,2,196,2,142,2,85,2,14,2,198,1,117,1,39,1,205,0,101,0,244,255,122,255,242,254,112,254,228,
        253,109,253,1,253,172,252,114,252,89,252,82,252,92,252,128,252,196,252,10,253,91,253,168,253,228,253,19,254,52,254,63,254,52,254,30,254,242,253,206,253,166,253,128,253,100,253,96,253,102,253,123,253,154,253,193,253,242,253,37,254,85,254,124,254,163,254,
        177,254,167,254,153,254,139,254,116,254,80,254,11,254,171,253,96,253,42,253,9,253,237,252,219,252,240,252,24,253,77,253,177,253,49,254,175,254,39,255,140,255,209,255,11,0,65,0,115,0,125,0,96,0,73,0,51,0,1,0,205,255,176,255,191,255,228,255,249,255,252,
        255,255,255,245,255,232,255,209,255,174,255,147,255,116,255,61,255,227,254,124,254,1,254,142,253,74,253,59,253,27,253,240,252,186,252,142,252,96,252,68,252,24,252,245,251,229,251,241,251,21,252,84,252,151,252,193,252,200,252,161,252,77,252,219,251,94,
        251,244,250,163,250,143,250,177,250,4,251,97,251,207,251,79,252,185,252,248,252,21,253,75,253,207,253,146,254,131,255,127,0,71,1,225,1,90,2,181,2,239,2,13,3,21,3,44,3,41,3,37,3,251,2,207,2,149,2,79,2,233,1,56,1,86,0,115,255,191,254,65,254,239,253,203,
        253,204,253,224,253,36,254,128,254,239,254,80,255,151,255,166,255,131,255,54,255,167,254,218,253,243,252,27,252,132,251,58,251,40,251,56,251,96,251,125,251,129,251,74,251,234,250,130,250,57,250,21,250,14,250,49,250,94,250,151,250,231,250,86,251,193,251,
        18,252,63,252,86,252,113,252,120,252,96,252,61,252,24,252,23,252,48,252,118,252,228,252,134,253,78,254,40,255,10,0,201,0,93,1,191,1,13,2,84,2,174,2,5,3,82,3,154,3,213,3,26,4,74,4,90,4,108,4,144,4,186,4,254,4,75,5,174,5,10,6,104,6,148,6,162,6,125,6,78,
        6,29,6,252,5,246,5,4,6,21,6,61,6,118,6,162,6,166,6,119,6,36,6,201,5,77,5,211,4,106,4,253,3,128,3,17,3,153,2,50,2,208,1,132,1,97,1,104,1,132,1,184,1,252,1,75,2,189,2,45,3,163,3,251,3,76,4,155,4,228,4,27,5,69,5,91,5,99,5,87,5,80,5,58,5,39,5,19,5,231,4,
        193,4,167,4,137,4,90,4,51,4,24,4,30,4,59,4,106,4,166,4,231,4,45,5,112,5,173,5,239,5,48,6,93,6,109,6,80,6,29,6,221,5,135,5,43,5,220,4,134,4,47,4,208,3,143,3,78,3,24,3,222,2,153,2,90,2,43,2,251,1,216,1,187,1,171,1,178,1,211,1,7,2,43,2,74,2,68,2,255,1,162,
        1,49,1,172,0,43,0,191,255,115,255,86,255,87,255,115,255,180,255,4,0,87,0,140,0,151,0,130,0,66,0,226,255,99,255,217,254,54,254,148,253,244,252,110,252,241,251,151,251,90,251,57,251,65,251,104,251,190,251,35,252,149,252,9,253,99,253,174,253,221,253,239,
        253,235,253,208,253,185,253,152,253,99,253,33,253,225,252,158,252,99,252,49,252,17,252,5,252,2,252,27,252,82,252,176,252,49,253,192,253,78,254,220,254,81,255,169,255,224,255,221,255,184,255,91,255,214,254,62,254,152,253,3,253,139,252,46,252,237,251,196,
        251,182,251,227,251,48,252,121,252,199,252,35,253,174,253,36,254,134,254,235,254,66,255,130,255,167,255,151,255,99,255,19,255,156,254,27,254,181,253,103,253,49,253,235,252,147,252,60,252,244,251,209,251,204,251,190,251,150,251,104,251,60,251,10,251,224,
        250,202,250,184,250,184,250,215,250,65,251,226,251,163,252,132,253,102,254,14,255,83,255,57,255,223,254,94,254,207,253,60,253,160,252,20,252,193,251,147,251,118,251,101,251,99,251,83,251,57,251,14,251,7,251,11,251,64,251,183,251,86,252,241,252,100,253,
        179,253,9,254,113,254,224,254,68,255,163,255,7,0,90,0,165,0,217,0,6,1,7,1,183,0,8,0,22,255,8,254,24,253,107,252,17,252,9,252,35,252,86,252,163,252,253,252,92,253,142,253,100,253,192,252,197,251,130,250,57,249,254,247,26,247,120,246,5,246,200,245,180,
        245,211,245,254,245,24,246,3,246,204,245,168,245,164,245,198,245,25,246,147,246,52,247,197,247,37,248,112,248,185,248,14,249,142,249,69,250,72,251,131,252,215,253,23,255,54,0,33,1,191,1,2,2,25,2,35,2,63,2,117,2,207,2,107,3,73,4,108,5,184,6,251,7,60,9,
        149,10,214,11,240,12,197,13,110,14,239,14,77,15,132,15,139,15,89,15,211,14,13,14,54,13,105,12,201,11,84,11,5,11,232,10,222,10,189,10,144,10,70,10,239,9,96,9,146,8,157,7,150,6,126,5,116,4,114,3,138,2,165,1,202,0,15,0,112,255,236,254,148,254,105,254,120,
        254,211,254,104,255,43,0,20,1,14,2,3,3,221,3,116,4,227,4,69,5,185,5,33,6,155,6,33,7,157,7,2,8,118,8,216,8,35,9,64,9,64,9,49,9,30,9,12,9,255,8,240,8,208,8,150,8,67,8,243,7,155,7,45,7,205,6,107,6,21,6,202,5,153,5,102,5,83,5,84,5,81,5,102,5,108,5,106,5,
        134,5,202,5,42,6,180,6,93,7,32,8,236,8,170,9,103,10,246,10,66,11,73,11,7,11,108,10,149,9,133,8,83,7,37,6,239,4,224,3,211,2,226,1,22,1,129,0,255,255,113,255,198,254,9,254,42,253,57,252,42,251,14,250,236,248,214,247,215,246,12,246,97,245,14,245,6,245,68,
        245,197,245,111,246,12,247,160,247,33,248,148,248,216,248,242,248,235,248,234,248,221,248,241,248,28,249,98,249,176,249,15,250,119,250,240,250,117,251,241,251,93,252,193,252,16,253,81,253,125,253,153,253,167,253,163,253,168,253,167,253,128,253,81,253,
        56,253,89,253,152,253,210,253,218,253,197,253,186,253,215,253,38,254,119,254,196,254,1,255,65,255,137,255,220,255,46,0,134,0,209,0,9,1,54,1,83,1,79,1,49,1,32,1,25,1,249,0,145,0,4,0,91,255,164,254,12,254,159,253,55,253,194,252,103,252,43,252,2,252,200,
        251,118,251,40,251,249,250,198,250,133,250,37,250,187,249,77,249,235,248,149,248,48,248,196,247,130,247,107,247,114,247,124,247,114,247,24,247,125,246,203,245,4,245,53,244,152,243,66,243,26,243,37,243,79,243,108,243,133,243,177,243,233,243,35,244,73,
        244,144,244,53,245,74,246,179,247,102,249,75,251,39,253,227,254,112,0,227,1,45,3,110,4,115,5,39,6,127,6,80,6,209,5,1,5,225,3,63,2,55,0,228,253,154,251,158,249,56,248,121,247,69,247,121,247,243,247,159,248,116,249,55,250,202,250,21,251,33,251,197,250,
        255,249,25,249,49,248,67,247,103,246,129,245,168,244,248,243,109,243,33,243,14,243,46,243,98,243,142,243,191,243,6,244,115,244,20,245,197,245,88,246,187,246,34,247,148,247,19,248,203,248,147,249,98,250,25,251,162,251,42,252,163,252,10,253,105,253,195,
        253,78,254,5,255,4,0,74,1,229,2,149,4,44,6,147,7,186,8,189,9,108,10,214,10,1,11,16,11,52,11,152,11,253,11,144,12,69,13,243,13,180,14,106,15,23,16,182,16,61,17,217,17,154,18,25,19,78,19,47,19,197,18,61,18,167,17,250,16,73,16,179,15,116,15,114,15,161,15,
        217,15,16,16,30,16,226,15,113,15,174,14,151,13,41,12,144,10,241,8,85,7,206,5,110,4,64,3,78,2,172,1,69,1,51,1,86,1,198,1,140,2,145,3,153,4,149,5,100,6,10,7,112,7,130,7,81,7,238,6,115,6,237,5,140,5,95,5,122,5,225,5,123,6,13,7,132,7,254,7,131,8,6,9,142,
        9,22,10,168,10,94,11,246,11,125,12,220,12,22,13,36,13,231,12,98,12,188,11,250,10,51,10,123,9,236,8,139,8,56,8,227,7,140,7,38,7,183,6,89,6,235,5,153,5,79,5,25,5,235,4,196,4,157,4,85,4,254,3,132,3,232,2,74,2,139,1,213,0,7,0,21,255,23,254,67,253,121,252,
        186,251,70,251,25,251,56,251,150,251,31,252,215,252,145,253,58,254,178,254,200,254,113,254,190,253,204,252,168,251,100,250,37,249,232,247,194,246,201,245,2,245,129,244,65,244,55,244,90,244,158,244,233,244,74,245,190,245,53,246,135,246,186,246,194,246,
        205,246,207,246,254,246,78,247,150,247,213,247,243,247,51,248,134,248,252,248,118,249,219,249,75,250,181,250,25,251,125,251,240,251,84,252,171,252,240,252,69,253,175,253,49,254,174,254,44,255,136,255,178,255,169,255,105,255,34,255,239,254,247,254,54,
        255,115,255,141,255,144,255,134,255,140,255,191,255,29,0,173,0,57,1,129,1,118,1,47,1,6,1,213,0,132,0,8,0,77,255,92,254,34,253,182,251,107,250,111,249,157,248,245,247,95,247,247,246,161,246,73,246,216,245,7,245,216,243,55,242,64,240,47,238,112,236,55,
        235,156,234,100,234,111,234,228,234,182,235,195,236,220,237,193,238,131,239,84,240,73,241,119,242,235,243,64,245,70,246,232,246,94,247,202,247,12,248,119,248,39,249,54,250,70,251,77,252,44,253,195,253,2,254,211,253,70,253,85,252,36,251,223,249,221,248,
        119,248,223,248,228,249,49,251,133,252,224,253,7,255,4,0,162,0,217,0,125,0,162,255,106,254,39,253,5,252,32,251,151,250,158,250,229,250,29,251,6,251,169,250,24,250,95,249,133,248,132,247,103,246,56,245,31,244,23,243,19,242,17,241,45,240,133,239,53,239,
        57,239,190,239,172,240,251,241,137,243,49,245,167,246,184,247,106,248,224,248,61,249,145,249,31,250,49,251,194,252,182,254,224,0,39,3,147,5,255,7,53,10,68,12,6,14,136,15,203,16,176,17,77,18,166,18,8,19,159,19,81,20,44,21,73,22,152,23,249,24,88,26,196,
        27,25,29,29,30,245,30,109,31,118,31,249,30,65,30,98,29,92,28,97,27,89,26,72,25,50,24,66,23,116,22,212,21,78,21,225,20,131,20,249,19,22,19,230,17,96,16,168,14,240,12,90,11,4,10,236,8,238,7,36,7,145,6,42,6,230,5,159,5,137,5,173,5,235,5,72,6,198,6,94,7,
        34,8,216,8,128,9,2,10,74,10,80,10,27,10,204,9,111,9,250,8,126,8,243,7,61,7,129,6,253,5,223,5,47,6,201,6,154,7,102,8,49,9,27,10,11,11,219,11,122,12,231,12,26,13,21,13,237,12,202,12,174,12,152,12,112,12,29,12,165,11,234,10,246,9,240,8,3,8,56,7,161,6,47,
        6,6,6,50,6,134,6,223,6,24,7,61,7,68,7,40,7,226,6,90,6,159,5,166,4,124,3,50,2,188,0,34,255,134,253,23,252,227,250,18,250,162,249,158,249,209,249,51,250,137,250,219,250,244,250,186,250,33,250,252,248,80,247,81,245,47,243,40,241,85,239,219,237,241,236,149,
        236,209,236,155,237,193,238,32,240,164,241,40,243,139,244,201,245,181,246,81,247,145,247,163,247,128,247,31,247,109,246,143,245,139,244,126,243,170,242,48,242,11,242,0,242,250,241,54,242,192,242,203,243,16,245,96,246,181,247,242,248,54,250,96,251,67,
        252,190,252,200,252,146,252,34,252,81,251,31,250,26,249,144,248,101,248,117,248,224,248,130,249,51,250,248,250,12,252,114,253,7,255,151,0,254,1,229,2,82,3,71,3,236,2,59,2,28,1,199,255,72,254,128,252,150,250,181,248,88,247,118,246,187,245,247,244,91,244,
        35,244,255,243,208,243,90,243,135,242,110,241,9,240,112,238,173,236,218,234,95,233,159,232,47,232,233,231,249,231,101,232,23,233,172,233,180,233,77,233,220,232,7,233,200,233,251,234,79,236,163,237,198,238,236,239,243,240,225,241,187,242,145,243,130,244,
        165,245,172,246,178,247,105,248,3,249,98,249,108,249,239,248,29,248,84,247,34,247,192,247,16,249,226,250,10,253,21,255,222,0,60,2,45,3,177,3,202,3,105,3,142,2,35,1,76,255,62,253,112,251,250,249,224,248,11,248,172,247,156,247,196,247,18,248,31,248,213,
        247,44,247,61,246,97,245,132,244,195,243,180,242,121,241,98,240,216,239,254,239,226,240,61,242,235,243,155,245,253,246,9,248,229,248,88,249,149,249,185,249,65,250,79,251,212,252,36,255,20,2,93,5,218,8,47,12,20,15,79,17,20,19,101,20,104,21,67,22,51,23,
        62,24,40,25,244,25,160,26,48,27,163,27,24,28,189,28,134,29,90,30,93,31,180,32,3,34,9,35,157,35,171,35,88,35,6,35,168,34,49,34,192,33,118,33,81,33,39,33,254,32,184,32,101,32,204,31,221,30,139,29,232,27,47,26,77,24,124,22,237,20,129,19,48,18,30,17,67,16,
        138,15,216,14,65,14,198,13,105,13,85,13,130,13,226,13,104,14,246,14,85,15,102,15,21,15,118,14,162,13,170,12,192,11,227,10,214,9,214,8,237,7,36,7,105,6,171,5,216,4,247,3,62,3,183,2,129,2,139,2,218,2,77,3,254,3,189,4,160,5,138,6,86,7,219,7,9,8,28,8,32,
        8,41,8,61,8,107,8,198,8,27,9,102,9,124,9,114,9,67,9,248,8,132,8,6,8,117,7,215,6,73,6,220,5,147,5,98,5,98,5,137,5,209,5,1,6,6,6,188,5,41,5,47,4,183,2,220,0,210,254,168,252,144,250,164,248,33,247,12,246,101,245,29,245,53,245,107,245,128,245,136,245,74,
        245,229,244,49,244,76,243,56,242,15,241,226,239,186,238,174,237,228,236,102,236,98,236,188,236,109,237,80,238,72,239,88,240,137,241,207,242,51,244,72,245,251,245,48,246,250,245,139,245,13,245,98,244,119,243,99,242,94,241,98,240,140,239,187,238,248,237,
        82,237,155,236,254,235,204,235,70,236,83,237,132,238,244,239,171,241,140,243,86,245,186,246,181,247,103,248,54,249,196,249,209,249,115,249,15,249,15,249,123,249,65,250,75,251,106,252,102,253,0,254,8,254,1,254,134,254,148,255,120,0,222,0,212,0,97,0,172,
        255,243,254,29,254,248,252,82,251,154,249,13,248,207,246,208,245,249,244,241,243,178,242,60,241,75,239,212,236,1,234,80,231,254,228,3,227,93,225,243,223,72,223,45,223,80,223,72,223,44,223,40,223,130,223,120,224,62,226,100,228,166,230,168,232,57,234,38,
        235,156,235,242,235,115,236,36,237,53,238,149,239,52,241,228,242,211,244,2,247,15,249,108,250,202,250,53,250,61,249,133,248,98,248,239,248,21,250,204,251,51,254,18,1,31,4,46,7,113,9,184,10,163,10,34,9,148,6,85,3,248,255,171,252,216,249,152,247,227,245,
        169,244,212,243,155,243,216,243,7,244,31,244,43,244,86,244,121,244,175,244,177,244,140,244,57,244,213,243,106,243,90,243,221,243,249,244,111,246,48,248,239,249,101,251,99,252,250,252,17,253,246,252,207,252,222,252,172,253,52,255,135,1,116,4,101,7,59,
        10,238,12,80,15,109,17,86,19,232,20,178,22,192,24,153,26,59,28,210,29,53,31,83,32,196,32,182,32,81,32,201,31,177,31,97,32,194,33,151,35,145,37,97,39,229,40,14,42,238,42,115,43,130,43,68,43,252,42,202,42,130,42,250,41,88,41,177,40,228,39,222,38,172,37,
        101,36,23,35,233,33,191,32,111,31,57,30,36,29,247,27,178,26,90,25,235,23,174,22,200,21,114,21,120,21,163,21,9,22,137,22,195,22,148,22,225,21,219,20,119,19,224,17,67,16,169,14,51,13,35,12,129,11,40,11,231,10,127,10,190,9,135,8,235,6,69,5,177,3,23,2,211,
        0,227,255,50,255,217,254,177,254,241,254,130,255,64,0,31,1,17,2,20,3,88,4,176,5,253,6,16,8,218,8,159,9,73,10,186,10,235,10,239,10,234,10,192,10,130,10,33,10,188,9,80,9,237,8,143,8,218,7,46,7,104,6,195,5,48,5,173,4,58,4,146,3,171,2,100,1,201,255,18,254,
        124,252,10,251,167,249,84,248,46,247,59,246,82,245,184,244,15,244,100,243,202,242,240,241,27,241,35,240,49,239,107,238,177,237,239,236,23,236,76,235,106,234,205,233,119,233,83,233,136,233,28,234,15,235,177,236,206,238,74,241,198,243,201,245,31,247,182,
        247,207,247,102,247,127,246,9,245,33,243,243,240,154,238,50,236,98,234,14,233,252,231,206,230,185,229,29,229,44,229,160,229,65,230,23,231,25,232,64,233,164,234,14,236,77,237,134,238,12,240,156,241,148,242,191,242,135,242,105,242,73,242,55,242,112,242,
        26,243,172,243,208,243,166,243,137,243,237,243,205,244,55,246,236,247,154,249,245,250,24,252,1,253,178,253,220,253,91,253,84,252,241,250,91,249,182,247,254,245,32,244,67,242,121,240,134,238,144,236,102,234,119,232,217,230,166,229,171,228,203,227,56,227,
        17,227,36,227,245,226,101,226,194,225,120,225,202,225,209,226,167,228,4,231,124,233,196,235,165,237,35,239,79,240,27,241,125,241,160,241,240,241,109,242,48,243,35,244,75,245,149,246,95,247,163,247,2,247,189,245,101,244,109,243,205,242,146,242,232,242,
        237,243,169,245,195,247,248,249,89,252,77,254,145,255,234,255,169,255,195,254,109,253,169,251,24,250,41,249,182,248,49,248,149,247,154,246,168,245,119,244,82,243,62,242,114,241,38,241,98,241,34,242,242,242,158,243,33,244,166,244,18,245,171,245,187,246,
        79,248,109,250,250,252,136,255,214,1,62,3,202,3,168,3,176,2,31,1,210,255,16,255,245,254,102,255,111,0,82,2,214,4,200,7,48,11,192,14,116,18,254,21,111,25,146,28,90,31,139,33,105,35,183,36,94,37,175,37,162,37,101,37,62,37,79,37,191,37,215,38,124,40,198,
        42,85,45,5,48,187,50,58,53,28,55,101,56,2,57,213,56,255,55,138,54,217,52,230,50,224,48,47,47,192,45,159,44,198,43,220,42,9,42,63,41,154,40,251,39,81,39,107,38,107,37,27,36,127,34,218,32,61,31,227,29,185,28,157,27,144,26,141,25,166,24,48,24,241,23,189,
        23,166,23,130,23,59,23,202,22,53,22,96,21,54,20,165,18,54,17,168,15,23,14,116,12,227,10,127,9,41,8,191,6,61,5,179,3,102,2,132,1,208,0,136,0,140,0,234,0,87,1,232,1,146,2,81,3,40,4,75,5,190,6,42,8,69,9,206,9,237,9,196,9,48,9,241,7,120,6,25,5,254,3,53,3,
        217,2,8,3,147,3,44,4,221,4,106,5,159,5,144,5,76,5,198,4,222,3,203,2,144,1,98,0,50,255,242,253,147,252,29,251,177,249,130,248,125,247,243,246,197,246,5,247,99,247,159,247,190,247,74,247,115,246,13,245,84,243,51,241,206,238,162,236,92,234,64,232,130,230,
        77,229,188,228,168,228,15,229,216,229,217,230,43,232,178,233,38,235,91,236,1,237,245,236,138,236,14,236,132,235,197,234,229,233,223,232,217,231,1,231,80,230,234,229,124,229,23,229,195,228,82,228,182,227,162,227,19,228,163,228,69,229,243,229,145,230,93,
        231,83,232,125,233,199,234,86,236,198,237,150,238,176,238,97,238,10,238,220,237,10,238,143,238,16,239,10,239,90,238,69,237,68,236,210,235,9,236,213,236,219,237,206,238,222,239,99,241,3,243,42,244,170,244,173,244,44,244,10,243,184,241,131,240,89,239,11,
        238,149,236,219,234,212,232,188,230,88,228,227,225,232,223,73,222,223,220,209,219,147,219,61,220,61,221,237,221,241,221,138,221,142,221,23,222,37,223,195,224,28,227,236,229,164,232,242,234,19,237,21,239,253,240,145,242,201,243,143,244,74,245,49,246,71,
        247,127,248,142,249,238,249,21,249,33,247,97,244,149,241,71,239,237,237,141,237,49,238,125,239,166,241,118,244,23,248,247,251,130,255,237,1,248,2,189,2,161,1,212,255,202,253,251,251,245,250,118,250,58,250,138,249,109,248,247,246,76,245,163,243,69,242,
        134,241,130,241,77,242,113,243,139,244,201,245,221,246,148,247,246,247,8,248,123,248,54,249,108,250,10,252,220,253,94,255,129,0,27,1,147,0,50,255,56,253,110,251,79,250,89,250,179,251,74,254,158,1,187,5,135,10,102,15,0,20,245,23,142,27,219,30,117,33,70,
        35,147,36,132,37,90,38,41,39,236,39,123,40,8,41,213,41,12,43,164,44,175,46,75,49,3,52,158,54,82,57,235,59,19,62,114,63,23,64,54,64,203,63,166,62,234,60,206,58,147,56,162,54,203,52,241,50,101,49,234,47,116,46,209,44,21,43,66,41,154,39,38,38,231,36,218,
        35,184,34,111,33,24,32,202,30,163,29,153,28,224,27,140,27,95,27,97,27,153,27,246,27,121,28,240,28,20,29,207,28,20,28,27,27,242,25,179,24,77,23,222,21,82,20,191,18,1,17,2,15,28,13,138,11,66,10,32,9,42,8,118,7,36,7,235,6,248,6,22,7,46,7,81,7,114,7,76,7,
        162,6,207,5,9,5,189,4,163,4,220,4,189,4,180,4,126,4,77,4,220,3,163,3,123,3,136,3,197,3,149,4,188,5,224,6,238,7,193,8,66,9,63,9,223,8,43,8,105,7,181,6,7,6,57,5,62,4,39,3,36,2,209,0,47,255,171,253,49,252,233,250,237,249,37,249,166,248,126,248,164,248,163,
        248,91,248,81,247,154,245,65,243,77,240,68,237,14,234,216,230,39,228,21,226,191,224,8,224,243,223,97,224,50,225,133,226,78,228,178,230,60,233,168,235,180,237,205,238,35,239,249,238,86,238,71,237,204,235,13,234,125,232,32,231,90,230,62,230,47,230,23,230,
        189,229,88,229,231,228,139,228,74,228,106,228,160,228,213,228,244,228,5,229,27,229,102,229,210,229,207,230,8,232,229,232,74,233,180,233,17,234,92,234,182,234,44,235,137,235,91,235,172,234,236,233,244,232,4,232,202,231,115,232,68,233,219,233,189,234,43,
        236,228,237,133,239,154,240,237,240,199,240,163,240,216,239,13,238,239,235,20,234,64,232,27,230,175,227,246,224,57,222,174,219,173,217,26,216,168,214,130,213,249,212,76,213,140,214,39,216,134,217,86,218,21,219,28,220,153,221,152,223,28,226,232,228,180,
        231,66,234,140,236,180,238,193,240,151,242,10,244,82,245,109,246,128,247,139,248,209,249,22,251,233,251,136,251,252,249,188,247,89,245,69,243,214,241,41,241,98,241,87,242,58,244,19,247,181,250,138,254,220,1,98,4,217,5,255,5,245,4,233,2,54,0,188,253,212,
        251,180,250,14,250,228,249,202,249,159,249,68,249,159,248,156,247,124,246,166,245,244,244,209,244,218,244,241,244,212,244,133,244,152,244,220,244,52,245,171,245,183,246,94,248,201,250,166,253,58,0,250,1,185,2,179,2,168,1,209,255,206,253,63,252,139,251,
        82,252,173,254,23,2,134,6,169,11,152,17,95,23,117,28,176,32,30,36,147,38,111,40,192,41,94,42,221,42,58,43,163,43,11,44,79,44,252,44,64,46,0,48,68,50,27,53,46,56,96,59,132,62,120,65,247,67,167,69,108,70,95,70,93,69,114,67,238,64,185,61,64,58,228,54,2,
        52,186,49,218,47,145,46,176,45,220,44,231,43,12,43,51,42,110,41,192,40,17,40,230,38,61,37,109,35,244,33,171,32,90,31,127,30,210,29,135,29,144,29,170,29,198,29,4,30,19,30,252,29,196,29,54,29,145,28,172,27,144,26,44,25,140,23,5,22,121,20,105,18,81,16,72,
        14,183,12,74,11,247,9,95,8,154,6,3,5,196,3,194,2,251,1,68,1,209,0,104,0,39,0,230,255,106,255,234,254,216,254,59,255,216,255,143,0,128,1,114,2,67,3,233,3,130,4,186,4,199,4,232,4,66,5,191,5,162,6,108,7,63,8,248,8,178,9,244,9,193,9,69,9,182,8,243,7,194,
        6,142,5,54,4,171,2,238,0,243,254,183,252,151,250,52,249,109,248,4,248,153,247,38,247,193,246,56,246,90,245,3,244,170,241,216,238,238,235,13,233,84,230,255,227,33,226,163,224,207,223,137,223,235,223,159,224,205,225,82,227,11,229,21,231,27,233,244,234,
        112,236,119,237,244,237,223,237,134,237,188,236,171,235,96,234,34,233,79,232,180,231,108,231,41,231,180,230,74,230,164,229,199,228,204,227,220,226,32,226,224,225,210,225,156,225,125,225,209,225,150,226,229,227,1,229,1,230,3,231,217,231,132,232,2,233,
        47,233,83,233,136,233,135,233,39,233,78,232,69,231,61,230,87,229,4,229,58,229,211,229,178,230,251,231,93,233,246,234,159,236,56,238,131,239,55,240,55,240,47,239,59,237,153,234,173,231,96,228,225,224,86,221,55,218,156,215,163,213,43,212,226,210,215,209,
        73,209,24,209,74,209,174,209,5,210,246,209,235,209,56,210,41,211,224,212,113,215,169,218,12,222,118,225,30,229,202,232,87,236,149,239,120,242,230,244,183,246,74,248,4,250,41,252,128,254,157,0,31,2,142,2,7,2,123,0,76,254,16,252,36,250,59,249,98,249,159,
        250,175,252,105,255,106,2,162,5,117,8,178,10,178,11,29,11,51,9,151,6,226,3,135,1,241,255,147,255,120,255,172,255,109,255,178,254,17,253,186,250,245,247,169,245,216,243,95,242,244,240,248,239,96,239,50,239,28,239,96,239,204,239,74,240,103,241,106,243,
        239,245,206,248,175,251,56,254,21,0,187,0,137,0,183,255,128,254,145,253,56,253,179,253,27,255,87,1,204,4,82,9,93,14,187,19,234,24,180,29,244,33,125,37,248,39,149,41,181,42,144,43,114,44,75,45,44,46,85,47,116,48,213,49,147,51,181,53,18,56,118,58,7,61,
        209,63,151,66,8,69,203,70,207,71,87,72,30,72,250,70,187,68,154,65,68,62,200,58,147,55,246,52,10,51,219,49,55,49,180,48,12,48,8,47,212,45,118,44,11,43,135,41,239,39,83,38,216,36,97,35,56,34,86,33,222,32,155,32,146,32,130,32,60,32,244,31,180,31,96,31,224,
        30,241,29,251,28,182,27,128,26,71,25,25,24,99,22,36,20,239,17,217,15,193,13,155,11,196,9,153,8,218,7,129,7,22,7,104,6,191,5,185,4,186,3,117,2,76,1,226,255,200,254,67,254,229,253,210,253,22,254,227,254,190,255,177,0,141,1,21,2,169,2,52,3,145,3,186,3,232,
        3,72,4,245,4,250,5,33,7,132,8,174,9,159,10,91,11,171,11,135,11,5,11,17,10,228,8,141,7,1,6,37,4,21,2,42,0,103,254,179,252,212,250,243,248,175,247,160,246,143,245,161,244,3,244,115,243,28,243,180,242,161,241,214,239,113,237,199,234,89,232,217,229,156,227,
        127,225,202,223,156,222,57,222,85,222,199,222,127,223,195,224,117,226,143,228,195,230,225,232,200,234,58,236,47,237,181,237,183,237,5,237,225,235,135,234,79,233,58,232,149,231,108,231,148,231,202,231,195,231,58,231,112,230,76,229,38,228,81,227,159,226,
        255,225,126,225,86,225,162,225,25,226,231,226,38,228,157,229,134,230,188,230,237,230,55,231,104,231,138,231,0,232,58,232,177,231,87,230,255,228,31,228,212,227,44,228,197,228,125,229,74,230,79,231,141,232,164,233,125,234,40,235,120,235,40,235,17,234,122,
        232,80,230,210,227,55,225,103,222,116,219,127,216,125,213,109,210,169,207,146,205,230,203,186,202,57,202,132,202,7,203,38,203,236,202,204,202,24,203,255,203,188,205,28,208,61,211,243,214,11,219,65,223,47,227,183,230,82,234,40,238,25,242,150,245,37,248,
        3,250,225,251,33,254,151,0,194,2,15,4,47,4,124,3,9,2,40,0,52,254,223,252,136,252,219,252,186,253,70,255,154,1,175,4,227,7,138,10,44,12,150,12,235,11,106,10,120,8,66,6,76,4,19,3,133,2,193,2,114,3,192,3,70,3,45,2,181,0,192,254,147,252,76,250,26,248,125,
        246,197,244,102,243,180,242,108,242,173,242,115,243,170,244,41,246,200,247,80,250,19,253,231,255,93,2,85,4,19,5,210,4,232,3,186,2,118,1,126,0,114,0,97,1,44,3,183,5,12,9,244,12,74,17,253,21,222,26,150,31,232,35,136,39,16,42,216,43,233,44,139,45,140,45,
        21,45,208,44,54,45,45,46,229,47,53,50,31,53,133,56,1,60,61,63,22,66,108,68,109,70,244,71,210,72,230,72,62,72,243,70,246,68,122,66,201,63,26,61,92,58,60,56,162,54,121,53,125,52,96,51,31,50,177,48,55,47,214,45,148,44,7,43,63,41,96,39,204,37,105,36,45,35,
        254,33,39,33,119,32,205,31,13,31,23,30,2,29,23,28,79,27,117,26,40,25,194,23,152,22,154,21,157,20,98,19,158,17,215,15,57,14,180,12,98,11,37,10,78,9,186,8,68,8,143,7,115,6,241,4,109,3,237,1,133,0,137,255,169,254,138,254,203,254,94,255,25,0,249,0,236,1,
        250,2,12,4,236,4,144,5,8,6,86,6,162,6,4,7,159,7,52,8,228,8,226,9,50,11,181,12,124,14,69,16,160,17,133,18,181,18,73,18,22,17,57,15,51,13,253,10,139,8,24,6,188,3,101,1,220,254,49,252,119,249,56,247,150,245,96,244,174,243,73,243,230,242,162,242,233,241,
        73,241,250,239,235,237,34,235,92,232,98,229,151,226,250,223,179,221,206,219,87,218,127,217,31,217,36,217,247,217,172,219,20,222,165,224,238,226,163,228,144,229,45,230,128,230,104,230,20,230,116,229,38,229,250,228,62,229,222,229,126,230,239,230,127,231,
        234,231,7,232,113,231,199,230,238,229,15,229,95,228,140,227,118,226,205,225,181,225,111,226,85,227,42,228,69,229,178,230,233,231,141,232,236,232,18,233,99,233,196,233,154,233,144,232,250,230,218,229,150,229,221,229,30,230,252,229,235,229,238,229,34,230,
        192,230,130,231,233,231,172,231,127,231,89,231,141,230,12,229,106,227,151,225,250,222,201,219,23,217,147,214,105,211,50,208,79,205,14,203,35,201,119,199,152,198,138,198,228,198,74,199,38,199,225,198,193,198,69,199,191,200,89,203,115,206,243,209,29,214,
        210,218,150,223,246,227,240,231,47,236,110,240,216,243,81,246,91,248,158,250,145,253,234,0,76,4,197,6,204,7,189,7,21,7,194,5,47,4,129,2,92,1,206,0,234,0,230,1,200,3,93,6,64,9,23,12,65,14,77,15,10,15,137,13,187,11,160,9,164,7,216,5,218,4,41,5,26,6,237,
        6,195,6,134,5,163,3,160,1,165,255,222,253,102,252,229,250,88,249,128,248,37,248,2,248,4,248,22,248,61,248,196,248,244,249,230,251,145,254,168,1,236,4,193,7,44,9,14,9,229,7,143,6,227,4,67,3,79,2,14,2,167,2,15,4,187,6,144,10,16,15,247,19,0,25,199,29,228,
        33,40,37,12,39,251,39,74,40,89,40,71,40,34,40,53,40,0,41,203,42,92,45,104,48,214,51,137,55,191,59,243,63,204,67,242,70,96,73,64,75,174,76,98,77,36,77,232,75,150,73,221,70,255,67,61,65,161,62,108,60,176,58,115,57,29,56,141,54,218,52,212,50,250,48,109,
        47,60,46,16,45,111,43,196,41,93,40,245,38,118,37,170,35,197,33,88,32,98,31,167,30,160,29,102,28,42,27,54,26,255,24,144,23,33,22,207,20,238,19,86,19,108,18,245,16,28,15,22,13,40,11,226,8,177,6,186,4,62,3,30,2,87,1,190,0,51,0,214,255,159,255,165,255,181,
        255,221,255,51,0,190,0,72,1,178,1,3,2,85,2,217,2,134,3,114,4,159,5,208,6,158,7,41,8,178,8,105,9,17,10,43,11,190,12,135,14,105,16,71,18,220,19,63,21,38,22,98,22,233,21,97,20,59,18,163,15,145,12,113,9,68,6,105,3,198,0,4,254,107,251,224,248,20,247,160,245,
        161,244,203,243,32,243,103,242,245,241,42,241,21,240,49,238,182,235,28,233,79,230,200,227,25,225,78,222,252,219,242,217,74,216,13,215,45,214,24,214,142,214,174,215,65,217,40,219,5,221,147,222,224,223,238,224,152,225,234,225,163,225,53,225,131,224,224,
        223,184,223,90,224,132,225,200,226,232,227,240,228,160,229,177,229,102,229,236,228,114,228,164,227,171,226,0,226,73,225,158,224,199,224,166,225,164,226,92,227,28,228,196,228,240,228,232,228,22,229,198,229,138,230,83,231,22,232,83,232,234,231,53,231,162,
        230,63,230,228,229,157,229,238,229,77,230,188,230,65,231,194,231,205,231,33,231,9,230,201,228,53,227,66,225,44,223,236,220,144,218,208,215,226,212,248,209,6,207,82,204,25,202,87,200,7,199,226,197,104,197,211,197,137,198,233,198,233,198,156,198,186,198,
        202,199,186,201,124,204,236,207,34,212,235,216,230,221,110,226,148,230,157,234,252,238,44,243,183,246,241,249,34,253,151,0,67,4,122,7,228,9,58,11,88,11,164,10,66,9,171,7,22,6,81,5,87,5,246,5,141,6,165,7,153,9,37,12,193,14,191,16,199,17,12,18,72,17,233,
        15,215,13,201,11,62,10,85,9,239,8,12,9,24,9,142,8,96,7,213,5,99,4,133,2,166,0,111,255,69,254,12,253,191,251,222,250,101,250,72,250,91,250,145,250,50,251,73,252,52,254,107,0,214,2,5,5,154,6,54,7,55,6,54,4,153,1,18,255,33,253,255,251,7,252,244,252,181,
        254,179,1,145,5,250,9,243,14,230,19,120,24,171,28,6,32,17,34,213,34,4,35,9,35,251,34,242,34,27,35,253,35,176,37,87,40,183,43,145,47,139,51,138,55,137,59,122,63,235,66,207,69,24,72,135,73,255,73,132,73,1,72,215,69,36,67,130,64,235,61,161,59,254,57,213,
        56,226,55,29,55,25,54,253,52,154,51,249,49,149,48,121,47,67,46,159,44,171,42,0,41,153,39,58,38,133,36,192,34,1,33,191,31,167,30,72,29,240,27,219,26,238,25,98,25,231,24,70,24,148,23,189,22,164,21,53,20,48,18,192,15,35,13,204,10,193,8,202,6,218,4,145,3,
        42,3,19,3,247,2,239,2,52,3,106,3,152,3,147,3,171,3,170,3,177,3,19,4,124,4,4,5,212,5,163,6,121,7,82,8,34,9,207,9,65,10,204,10,73,11,142,11,7,12,217,12,28,14,175,15,141,17,82,19,230,20,42,22,203,22,235,22,81,22,232,20,29,19,202,16,117,14,30,12,139,9,79,
        7,5,5,52,2,238,254,227,251,184,249,247,247,93,246,212,244,126,243,69,242,15,241,255,239,89,238,45,236,233,233,79,231,120,228,191,225,196,222,81,220,21,218,16,216,9,214,18,212,136,210,35,210,81,210,69,211,8,213,21,215,51,217,78,219,158,220,79,221,61,221,
        49,221,97,221,193,221,51,222,217,222,209,223,8,225,81,226,180,227,210,228,153,229,167,229,235,228,180,227,164,226,35,226,90,225,57,224,162,223,185,223,220,223,185,223,191,223,105,224,29,226,158,227,92,228,138,228,5,229,163,229,14,230,50,230,70,230,195,
        230,71,231,98,231,230,230,8,230,220,229,188,230,199,231,139,232,154,233,219,234,103,235,55,235,206,234,28,234,240,232,108,231,192,229,233,227,190,225,180,223,151,221,98,219,197,216,75,213,145,209,26,206,164,203,97,202,50,201,0,200,83,199,86,199,242,199,
        158,200,165,200,253,199,74,199,36,199,14,200,10,202,70,205,27,210,139,215,146,220,17,225,189,229,153,234,101,239,210,243,247,247,218,251,41,255,30,2,235,4,153,7,175,9,18,11,127,11,202,10,20,9,252,6,224,4,206,3,210,3,90,4,124,5,130,7,192,10,86,14,230,
        17,218,20,51,23,184,24,238,24,116,23,219,20,207,17,8,15,210,12,187,11,76,11,239,10,51,10,177,9,207,8,112,7,205,5,191,4,44,4,221,3,78,3,142,2,85,1,4,0,253,254,44,254,132,253,253,252,196,252,150,253,225,254,119,0,90,2,33,4,43,5,153,4,239,2,147,0,0,254,
        104,251,169,249,229,248,239,248,15,250,226,251,85,254,105,1,87,5,196,9,85,14,215,18,81,23,240,26,90,29,194,30,172,31,65,32,220,32,229,33,72,35,226,36,5,39,162,41,31,45,51,49,128,53,216,57,7,62,223,65,129,69,126,72,138,74,177,75,60,76,237,75,144,74,50,
        72,87,69,114,66,176,63,40,61,49,59,209,57,177,56,173,55,160,54,33,53,43,51,55,49,178,47,59,46,128,44,91,42,17,40,190,37,126,35,125,33,123,31,145,29,182,28,45,28,250,27,185,27,115,27,49,27,196,26,2,26,37,25,220,23,168,22,152,21,114,20,224,18,61,17,3,15,
        210,12,224,10,182,8,191,6,115,5,243,4,196,4,177,4,159,4,84,4,23,4,22,4,52,4,58,4,145,4,133,5,123,6,29,7,166,7,95,8,23,9,218,9,200,10,160,11,48,12,241,12,193,13,174,14,142,15,85,16,41,17,29,18,74,19,93,20,49,21,224,21,124,22,12,23,254,22,202,22,71,22,
        49,21,158,19,130,17,37,15,102,12,75,9,50,6,35,3,221,255,100,252,51,249,164,246,155,244,5,243,174,241,114,240,114,239,103,238,105,237,47,236,197,234,237,232,185,230,145,228,201,225,136,222,73,219,56,216,245,213,245,211,91,210,135,209,157,209,157,210,29,
        212,13,214,16,216,36,218,122,219,64,220,3,220,161,219,242,219,172,220,23,221,59,221,146,221,15,222,236,222,216,223,239,224,212,225,83,226,101,226,255,225,48,225,45,224,225,223,188,223,139,223,177,223,20,224,122,224,248,224,242,225,128,227,206,228,101,
        229,117,229,119,229,95,229,81,229,106,229,70,229,38,229,152,229,156,229,215,228,6,228,198,227,71,228,85,229,248,230,155,232,194,233,77,234,250,234,110,235,255,234,197,233,182,232,165,231,61,230,103,228,241,226,181,225,234,223,226,221,133,219,246,216,
        20,214,79,211,106,209,89,208,112,207,94,206,92,205,192,204,1,205,135,205,134,205,218,204,112,204,16,205,187,206,70,209,252,212,185,217,190,222,107,227,172,231,171,235,187,239,205,243,175,247,28,251,222,253,62,0,183,2,23,5,115,7,193,9,81,11,180,11,196,
        10,16,9,87,7,239,5,52,5,249,4,108,5,65,6,187,7,12,10,64,13,175,16,211,19,247,21,210,22,63,22,95,20,200,17,246,14,155,12,83,11,102,11,137,11,144,11,76,11,240,10,48,10,30,9,244,7,130,6,3,5,179,3,138,2,103,1,64,0,122,255,247,254,157,254,106,254,239,254,
        213,255,69,1,204,2,83,4,151,5,76,6,244,5,149,4,196,1,16,254,122,250,116,247,168,245,238,244,125,245,45,247,206,249,89,253,162,1,71,6,184,10,203,14,126,18,154,21,246,23,152,25,176,26,160,27,148,28,159,29,142,30,190,31,150,33,245,35,215,38,134,42,199,46,
        71,51,204,55,20,60,221,63,217,66,3,69,58,70,195,70,168,70,190,69,238,67,68,65,176,62,45,60,252,57,46,56,174,54,121,53,111,52,136,51,62,50,180,48,51,47,168,45,230,43,22,42,63,40,233,37,141,35,118,33,107,31,191,29,52,28,2,27,126,26,89,26,112,26,79,26,54,
        26,22,26,218,25,71,25,130,24,173,23,155,22,129,21,29,20,137,18,207,16,233,14,11,13,45,11,102,9,194,7,93,6,149,5,246,4,108,4,253,3,236,3,249,3,73,4,199,4,148,5,152,6,115,7,60,8,246,8,142,9,229,9,44,10,110,10,192,10,213,10,54,11,148,11,21,12,159,12,6,13,
        128,13,24,14,246,14,58,16,108,17,190,18,252,19,59,21,15,22,146,22,130,22,6,22,16,21,133,19,2,17,240,13,108,10,216,6,131,3,48,0,53,253,249,249,23,247,191,244,228,242,137,241,116,240,211,239,53,239,100,238,23,237,92,235,42,233,102,230,38,227,250,223,216,
        220,213,217,20,215,10,213,227,211,43,211,228,210,15,211,167,211,221,212,85,214,131,216,160,218,69,220,194,221,56,222,150,222,197,222,164,222,182,222,204,222,213,222,246,222,98,223,63,224,72,225,28,226,100,226,28,226,151,225,8,225,0,225,4,225,248,224,
        62,225,230,225,93,226,62,226,68,226,146,226,65,227,186,227,28,228,134,228,199,228,31,229,181,229,131,230,55,231,147,231,241,231,7,232,180,231,136,231,231,231,194,232,136,233,111,234,105,235,70,236,152,236,194,236,40,237,94,237,12,237,134,236,90,236,178,
        235,154,234,255,232,97,231,192,229,186,227,166,225,51,223,147,220,36,218,241,215,0,214,62,212,228,210,248,209,160,209,240,209,100,210,218,210,36,211,98,211,242,211,220,212,147,214,148,217,201,221,74,226,219,230,83,235,173,239,57,243,113,246,100,249,248,
        251,70,254,130,0,225,2,11,5,230,6,117,8,156,9,239,9,1,9,58,7,242,4,64,3,157,2,205,2,103,3,128,4,87,6,44,9,180,12,251,15,83,18,198,19,5,20,104,19,219,17,128,15,212,12,195,10,203,9,64,9,106,9,75,9,62,9,201,8,233,7,0,7,32,6,192,4,41,3,137,1,26,0,134,254,
        10,253,216,251,36,251,238,250,82,251,255,251,80,253,111,255,220,1,81,4,48,6,18,7,137,6,218,4,52,2,249,254,194,251,206,248,158,246,107,245,96,245,200,246,52,249,78,252,8,0,34,4,102,8,122,12,242,15,141,18,173,20,110,22,239,23,46,25,68,26,55,27,84,28,188,
        29,71,31,38,33,130,35,90,38,173,41,39,45,159,48,247,51,14,55,164,57,221,59,100,61,112,62,186,62,56,62,220,60,211,58,51,56,173,53,156,51,28,50,57,49,73,48,121,47,109,46,5,45,46,43,4,41,219,38,253,36,22,35,95,33,216,31,50,30,16,29,41,28,142,27,2,27,154,
        26,45,26,3,26,234,25,190,25,79,25,152,24,189,23,243,22,221,21,204,20,230,19,64,19,177,18,225,17,168,16,2,15,11,13,247,10,2,9,57,7,252,5,54,5,160,4,26,4,213,3,192,3,165,3,171,3,217,3,65,4,184,4,62,5,184,5,57,6,140,6,244,6,61,7,137,7,186,7,234,7,7,8,79,
        8,167,8,45,9,153,9,33,10,193,10,158,11,174,12,238,13,30,15,40,16,22,17,234,17,113,18,165,18,61,18,77,17,181,15,112,13,199,10,85,7,142,3,142,255,190,251,108,248,178,245,234,243,120,242,164,241,163,240,24,240,208,239,97,239,211,238,181,237,43,236,81,234,
        8,232,148,229,227,226,243,223,79,221,78,219,228,217,109,217,137,217,57,218,40,219,110,220,160,221,8,223,220,224,103,226,237,227,27,229,216,229,61,230,185,230,22,231,248,230,185,230,99,230,86,230,117,230,181,230,80,231,68,232,236,232,117,233,197,233,196,
        233,132,233,35,233,170,232,42,232,148,231,232,230,130,230,124,230,245,230,172,231,104,232,125,233,132,234,77,235,149,235,189,235,4,236,66,236,117,236,123,236,94,236,202,235,83,235,49,235,119,235,231,235,223,236,46,238,54,239,186,239,102,240,74,241,97,
        242,198,242,205,242,121,242,175,241,129,240,245,238,90,237,179,235,24,234,130,232,183,230,33,229,208,227,254,226,56,226,64,225,54,224,193,223,227,223,52,224,174,224,90,225,53,226,41,227,91,228,231,229,22,232,203,234,188,237,193,240,152,243,42,246,164,
        248,3,251,85,253,173,255,241,1,181,3,48,5,94,6,128,7,110,8,1,9,37,9,139,8,112,7,241,5,152,4,200,3,120,3,206,3,124,4,180,5,227,6,92,8,47,10,30,12,245,13,24,15,2,15,53,14,212,12,174,11,221,10,108,10,65,10,96,10,144,10,173,10,132,10,183,9,38,8,108,6,16,
        5,154,3,179,2,244,1,49,1,119,0,220,255,90,255,9,255,5,255,113,255,114,0,179,1,75,3,249,4,98,6,60,7,43,7,62,6,159,4,54,2,155,255,24,253,17,251,249,249,122,249,212,249,252,250,205,252,39,255,137,1,183,3,187,5,216,7,249,9,206,11,94,13,162,14,136,15,45,16,
        124,16,206,16,41,17,126,17,22,18,31,19,167,20,106,22,136,24,214,26,15,29,15,31,2,33,153,34,189,35,122,36,118,36,199,35,160,34,133,33,128,32,103,31,55,30,56,29,107,28,132,27,135,26,67,25,195,23,60,22,16,21,240,19,210,18,189,17,231,16,31,16,74,15,157,14,
        193,13,8,13,204,12,179,12,154,12,86,12,82,12,115,12,180,12,237,12,212,12,181,12,210,12,252,12,252,12,161,12,75,12,196,11,22,11,58,10,45,9,230,7,168,6,148,5,180,4,249,3,35,3,111,2,17,2,27,2,71,2,157,2,5,3,120,3,231,3,92,4,174,4,218,4,11,5,70,5,137,5,246,
        5,64,6,102,6,101,6,114,6,137,6,168,6,209,6,28,7,97,7,205,7,64,8,240,8,150,9,65,10,225,10,26,11,50,11,2,11,132,10,41,10,106,9,149,8,97,7,245,5,59,4,84,2,68,0,18,254,3,252,60,250,243,248,192,247,10,247,140,246,17,246,96,245,144,244,191,243,227,242,30,242,
        145,240,47,239,221,237,133,236,135,235,190,234,167,233,254,232,150,232,97,232,209,232,39,233,187,233,179,234,243,235,249,236,194,237,118,238,213,238,71,239,106,239,196,239,43,240,89,240,59,240,66,240,103,240,111,240,135,240,243,240,60,241,98,241,130,
        241,132,241,98,241,66,241,26,241,229,240,157,240,50,240,185,239,151,239,161,239,241,239,131,240,56,241,251,241,72,242,159,242,220,242,249,242,14,243,21,243,30,243,14,243,166,242,38,242,213,241,197,241,4,242,156,242,82,243,53,244,237,244,184,245,114,246,
        51,247,196,247,15,248,45,248,83,248,65,248,220,247,62,247,146,246,196,245,201,244,174,243,166,242,188,241,22,241,212,240,229,240,248,240,248,240,1,241,94,241,228,241,83,242,145,242,28,243,234,243,248,244,103,246,43,248,250,249,227,251,213,253,162,255,
        93,1,230,2,37,4,95,5,151,6,216,7,251,8,253,9,184,10,122,11,41,12,143,12,93,12,119,11,51,10,218,8,201,7,49,7,197,6,194,6,217,6,104,7,64,8,53,9,55,10,63,11,64,12,208,12,183,12,0,12,18,11,35,10,138,9,134,9,237,9,142,10,235,10,19,11,253,10,78,10,109,9,41,
        8,181,6,84,5,11,4,244,2,74,2,223,1,164,1,135,1,122,1,79,1,57,1,99,1,194,1,117,2,82,3,232,3,37,4,213,3,247,2,130,1,174,255,142,253,250,251,222,250,50,250,50,250,183,250,179,251,9,253,139,254,11,0,125,1,182,2,190,3,178,4,117,5,252,5,47,6,19,6,195,5,108,
        5,66,5,101,5,162,5,69,6,31,7,46,8,117,9,29,11,177,12,11,14,73,15,116,16,63,17,164,17,238,17,32,18,206,17,0,17,11,16,193,14,126,13,69,12,36,11,33,10,100,9,218,8,78,8,158,7,212,6,30,6,99,5,163,4,13,4,135,3,8,3,178,2,120,2,70,2,28,2,23,2,106,2,210,2,39,
        3,128,3,240,3,145,4,57,5,162,5,203,5,28,6,127,6,188,6,231,6,213,6,144,6,11,6,124,5,33,5,153,4,29,4,202,3,152,3,109,3,70,3,26,3,222,2,172,2,151,2,182,2,235,2,39,3,127,3,232,3,66,4,141,4,166,4,167,4,189,4,231,4,4,5,33,5,76,5,159,5,203,5,212,5,220,5,4,6,
        47,6,147,6,228,6,122,7,27,8,143,8,201,8,179,8,157,8,75,8,187,7,54,7,83,6,144,5,138,4,145,3,114,2,78,1,26,0,182,254,118,253,28,252,240,250,37,250,116,249,39,249,5,249,188,248,51,248,80,247,78,246,153,245,188,244,253,243,58,243,109,242,209,241,76,241,211,
        240,125,240,13,240,212,239,223,239,25,240,107,240,2,241,189,241,130,242,90,243,169,243,206,243,172,243,138,243,162,243,251,243,61,244,151,244,238,244,100,245,229,245,67,246,95,246,55,246,244,245,88,245,227,244,101,244,7,244,0,244,36,244,116,244,147,244,
        202,244,21,245,108,245,151,245,130,245,103,245,43,245,212,244,158,244,191,244,248,244,46,245,140,245,214,245,186,245,148,245,125,245,165,245,86,246,23,247,229,247,178,248,65,249,201,249,89,250,7,251,168,251,7,252,75,252,60,252,17,252,198,251,103,251,
        39,251,11,251,223,250,151,250,87,250,83,250,118,250,179,250,230,250,88,251,189,251,245,251,23,252,60,252,114,252,151,252,196,252,35,253,214,253,189,254,228,255,60,1,185,2,19,4,68,5,72,6,103,7,88,8,57,9,222,9,120,10,30,11,241,11,181,12,125,13,236,13,41,
        14,29,14,190,13,17,13,39,12,252,10,229,9,251,8,79,8,13,8,194,7,158,7,70,8,41,9,2,10,220,10,54,11,48,11,247,10,139,10,249,9,135,9,203,8,125,8,52,8,238,7,143,7,57,7,238,6,253,6,17,7,17,7,223,6,143,6,3,6,113,5,184,4,18,4,125,3,6,3,248,2,193,2,156,2,161,
        2,114,2,34,2,223,1,86,1,127,0,158,255,166,254,189,253,235,252,35,252,140,251,60,251,251,250,31,251,151,251,75,252,78,253,137,254,230,255,21,1,42,2,218,2,87,3,93,3,8,3,110,2,179,1,231,0,32,0,134,255,41,255,47,255,181,255,141,0,118,1,102,2,53,3,188,3,251,
        3,40,4,112,4,162,4,185,4,227,4,206,4,130,4,38,4,143,3,165,2,172,1,234,0,91,0,159,255,25,255,225,254,159,254,116,254,137,254,120,254,87,254,65,254,228,253,134,253,236,252,82,252,223,251,197,251,252,251,74,252,190,252,113,253,112,254,55,255,227,255,111,
        0,191,0,9,1,114,1,222,1,48,2,138,2,255,2,143,3,211,3,175,3,109,3,53,3,232,2,194,2,201,2,215,2,1,3,63,3,117,3,118,3,70,3,225,2,140,2,103,2,99,2,89,2,75,2,117,2,214,2,30,3,100,3,168,3,225,3,37,4,77,4,85,4,81,4,73,4,84,4,102,4,135,4,232,4,51,5,134,5,207,
        5,228,5,228,5,199,5,147,5,75,5,12,5,189,4,117,4,247,3,89,3,194,2,255,1,47,1,107,0,130,255,239,254,127,254,27,254,185,253,39,253,120,252,232,251,94,251,215,250,139,250,101,250,3,250,169,249,221,248,52,248,196,247,203,246,45,246,136,245,255,244,188,244,
        172,244,223,244,11,245,36,245,104,245,252,245,154,246,239,246,53,247,121,247,124,247,146,247,123,247,53,247,35,247,6,247,110,247,4,248,98,248,181,248,200,248,177,248,76,248,208,247,96,247,22,247,212,246,160,246,156,246,160,246,221,246,40,247,113,247,
        168,247,178,247,161,247,128,247,100,247,63,247,66,247,80,247,77,247,102,247,154,247,254,247,83,248,137,248,246,248,87,249,214,249,104,250,249,250,121,251,202,251,70,252,194,252,110,253,22,254,166,254,243,254,25,255,54,255,58,255,5,255,159,254,73,254,
        33,254,16,254,49,254,115,254,234,254,142,255,15,0,143,0,240,0,97,1,175,1,241,1,92,2,181,2,235,2,27,3,93,3,214,3,138,4,131,5,119,6,125,7,104,8,67,9,239,9,91,10,134,10,188,10,18,11,141,11,246,11,29,12,90,12,156,12,26,13,94,13,74,13,219,12,95,12,228,11,
        93,11,164,10,208,9,91,9,59,9,59,9,92,9,170,9,197,9,23,10,73,10,35,10,236,9,127,9,27,9,179,8,135,8,84,8,225,7,151,7,31,7,198,6,69,6,217,5,79,5,228,4,163,4,213,4,247,4,52,5,111,5,40,5,217,4,90,4,197,3,46,3,133,2,28,2,223,1,164,1,75,1,166,0,226,255,29,255,
        99,254,128,253,133,252,173,251,226,250,111,250,83,250,118,250,27,251,9,252,70,253,108,254,130,255,60,0,168,0,241,0,17,1,24,1,230,0,125,0,10,0,102,255,209,254,106,254,51,254,12,254,24,254,62,254,130,254,162,254,170,254,5,255,141,255,217,255,0,0,228,255,
        148,255,44,255,199,254,42,254,118,253,240,252,196,252,165,252,143,252,139,252,79,252,2,252,154,251,56,251,172,250,53,250,246,249,199,249,199,249,221,249,10,250,44,250,89,250,140,250,197,250,6,251,93,251,166,251,247,251,103,252,237,252,123,253,58,254,
        39,255,205,255,94,0,202,0,51,1,133,1,204,1,32,2,118,2,211,2,26,3,88,3,121,3,109,3,92,3,99,3,132,3,141,3,157,3,142,3,116,3,73,3,251,2,131,2,31,2,208,1,178,1,141,1,114,1,103,1,105,1,126,1,154,1,202,1,233,1,14,2,63,2,107,2,124,2,140,2,186,2,37,3,95,3,136,
        3,116,3,118,3,174,3,210,3,22,4,2,4,231,3,153,3,3,3,93,2,166,1,234,0,79,0,250,255,205,255,145,255,127,255,84,255,54,255,250,254,191,254,159,254,127,254,58,254,244,253,152,253,75,253,5,253,189,252,114,252,20,252,101,251,109,250,144,249,216,248,99,248,61,
        248,1,248,190,247,220,247,37,248,121,248,227,248,54,249,94,249,165,249,250,249,57,250,112,250,96,250,65,250,37,250,230,249,165,249,73,249,32,249,26,249,73,249,105,249,98,249,93,249,91,249,77,249,86,249,70,249,84,249,136,249,142,249,134,249,119,249,72,
        249,11,249,177,248,94,248,52,248,29,248,91,248,164,248,239,248,19,249,62,249,94,249,100,249,124,249,184,249,255,249,155,250,72,251,30,252,255,252,208,253,91,254,243,254,105,255,217,255,51,0,105,0,145,0,184,0,227,0,11,1,39,1,28,1,15,1,240,0,198,0,212,
        0,233,0,14,1,89,1,169,1,222,1,71,2,183,2,67,3,220,3,96,4,171,4,241,4,45,5,151,5,53,6,206,6,99,7,226,7,95,8,247,8,124,9,235,9,119,10,229,10,36,11,47,11,56,11,61,11,72,11,50,11,37,11,40,11,26,11,25,11,199,10,83,10,226,9,154,9,132,9,143,9,163,9,201,9,246,
        9,31,10,23,10,218,9,131,9,9,9,149,8,43,8,165,7,235,6,73,6,174,5,84,5,44,5,15,5,241,4,227,4,211,4,180,4,124,4,62,4,243,3,189,3,139,3,135,3,196,3,185,3,159,3,99,3,248,2,126,2,23,2,175,1,89,1,27,1,193,0,73,0,180,255,254,254,20,254,45,253,78,252,139,251,
        11,251,202,250,224,250,97,251,43,252,13,253,202,253,121,254,235,254,76,255,113,255,140,255,174,255,194,255,187,255,246,255,244,255,196,255,123,255,39,255,211,254,142,254,84,254,55,254,31,254,1,254,208,253,153,253,63,253,211,252,164,252,133,252,61,252,
        220,251,93,251,206,250,104,250,3,250,178,249,122,249,109,249,142,249,162,249,167,249,123,249,43,249,207,248,128,248,79,248,40,248,43,248,123,248,252,248,155,249,58,250,234,250,136,251,14,252,106,252,149,252,176,252,236,252,28,253,110,253,233,253,106,
        254,253,254,129,255,0,0,133,0,234,0,72,1,151,1,209,1,0,2,25,2,111,2,215,2,28,3,88,3,89,3,49,3,254,2,189,2,117,2,52,2,241,1,202,1,164,1,126,1,78,1,255,0,197,0,140,0,108,0,62,0,19,0,235,255,209,255,208,255,234,255,29,0,129,0,248,0,89,1,141,1,128,1,90,1,
        79,1,75,1,92,1,132,1,132,1,139,1,135,1,96,1,21,1,172,0,69,0,241,255,187,255,116,255,69,255,62,255,34,255,58,255,106,255,123,255,151,255,142,255,129,255,162,255,191,255,203,255,195,255,166,255,105,255,249,254,134,254,244,253,120,253,34,253,186,252,125,
        252,254,251,153,251,68,251,75,251,43,251,79,251,112,251,161,251,18,252,99,252,127,252,145,252,74,252,52,252,31,252,23,252,205,251,142,251,117,251,82,251,67,251,43,251,223,250,165,250,159,250,191,250,235,250,6,251,28,251,61,251,93,251,50,251,252,250,144,
        250,31,250,227,249,158,249,134,249,130,249,165,249,190,249,255,249,44,250,60,250,109,250,159,250,169,250,181,250,204,250,17,251,126,251,255,251,174,252,113,253,38,254,210,254,140,255,39,0,168,0,17,1,126,1,209,1,35,2,90,2,132,2,161,2,207,2,250,2,45,3,
        62,3,51,3,37,3,3,3,225,2,215,2,236,2,17,3,116,3,221,3,95,4,218,4,62,5,183,5,105,6,240,6,126,7,225,7,48,8,128,8,197,8,251,8,44,9,75,9,150,9,244,9,63,10,114,10,144,10,130,10,149,10,166,10,148,10,74,10,249,9,174,9,121,9,67,9,8,9,212,8,210,8,214,8,3,9,41,
        9,75,9,102,9,96,9,74,9,3,9,172,8,50,8,118,7,158,6,207,5,15,5,145,4,76,4,250,3,218,3,243,3,12,4,253,3,246,3,210,3,159,3,139,3,64,3,182,2,23,2,137,1,22,1,212,0,162,0,155,0,223,0,17,1,76,1,65,1,4,1,163,0,101,0,0,0,152,255,50,255,169,254,49,254,174,253,49,
        253,218,252,192,252,182,252,214,252,30,253,95,253,159,253,197,253,231,253,26,254,76,254,108,254,153,254,187,254,200,254,205,254,214,254,169,254,135,254,126,254,76,254,41,254,239,253,138,253,38,253,163,252,63,252,243,251,146,251,46,251,217,250,140,250,
        64,250,60,250,12,250,190,249,75,249,198,248,40,248,131,247,247,246,150,246,114,246,160,246,243,246,48,247,130,247,185,247,250,247,34,248,80,248,115,248,214,248,86,249,6,250,155,250,47,251,196,251,82,252,232,252,78,253,171,253,244,253,34,254,84,254,112,
        254,174,254,254,254,98,255,198,255,51,0,144,0,11,1,111,1,198,1,16,2,48,2,66,2,107,2,140,2,158,2,146,2,110,2,56,2,237,1,136,1,61,1,244,0,205,0,199,0,190,0,169,0,119,0,40,0,196,255,75,255,221,254,163,254,144,254,175,254,209,254,19,255,69,255,94,255,97,
        255,75,255,119,255,154,255,195,255,178,255,149,255,140,255,101,255,69,255,9,255,221,254,203,254,232,254,227,254,223,254,229,254,0,255,16,255,33,255,55,255,75,255,91,255,91,255,104,255,147,255,184,255,238,255,47,0,107,0,150,0,155,0,133,0,98,0,3,0,140,
        255,25,255,178,254,113,254,49,254,5,254,236,253,231,253,207,253,186,253,177,253,153,253,153,253,107,253,63,253,57,253,78,253,71,253,91,253,63,253,20,253,244,252,217,252,211,252,183,252,164,252,133,252,124,252,111,252,89,252,70,252,50,252,60,252,34,252,
        2,252,218,251,169,251,126,251,100,251,67,251,56,251,90,251,122,251,139,251,151,251,153,251,136,251,154,251,164,251,202,251,7,252,73,252,157,252,3,253,124,253,233,253,73,254,175,254,34,255,163,255,33,0,186,0,76,1,223,1,131,2,46,3,185,3,41,4,88,4,126,4,
        142,4,162,4,141,4,108,4,78,4,69,4,120,4,146,4,167,4,206,4,243,4,25,5,63,5,108,5,145,5,174,5,244,5,54,6,140,6,2,7,130,7,3,8,96,8,142,8,197,8,215,8,9,9,71,9,131,9,106,9,81,9,118,9,131,9,160,9,167,9,149,9,157,9,146,9,125,9,62,9,243,8,186,8,144,8,79,8,254,
        7,159,7,115,7,87,7,115,7,141,7,175,7,201,7,158,7,75,7,158,6,224,5,241,4,88,4,202,3,116,3,110,3,135,3,121,3,103,3,57,3,38,3,5,3,232,2,149,2,57,2,222,1,121,1,7,1,165,0,65,0,217,255,201,255,199,255,202,255,228,255,14,0,62,0,120,0,127,0,94,0,40,0,227,255,
        130,255,41,255,231,254,192,254,146,254,103,254,62,254,243,253,188,253,143,253,103,253,88,253,89,253,103,253,107,253,114,253,114,253,124,253,120,253,96,253,44,253,5,253,246,252,26,253,19,253,197,252,128,252,77,252,49,252,21,252,209,251,137,251,114,251,
        117,251,96,251,32,251,193,250,78,250,209,249,80,249,196,248,62,248,175,247,80,247,2,247,190,246,125,246,75,246,73,246,136,246,255,246,127,247,5,248,177,248,97,249,234,249,90,250,129,250,165,250,208,250,57,251,197,251,93,252,247,252,161,253,34,254,162,
        254,243,254,66,255,131,255,201,255,253,255,40,0,64,0,94,0,173,0,13,1,104,1,211,1,43,2,110,2,131,2,115,2,52,2,230,1,153,1,114,1,103,1,103,1,117,1,130,1,107,1,51,1,216,0,134,0,55,0,235,255,144,255,59,255,0,255,211,254,157,254,113,254,81,254,56,254,31,254,
        9,254,232,253,226,253,228,253,243,253,240,253,1,254,242,253,251,253,238,253,8,254,0,254,24,254,23,254,18,254,6,254,44,254,60,254,81,254,139,254,171,254,224,254,246,254,252,254,249,254,246,254,9,255,51,255,130,255,185,255,1,0,36,0,66,0,87,0,86,0,65,0,
        58,0,17,0,14,0,235,255,173,255,80,255,7,255,196,254,189,254,180,254,162,254,159,254,151,254,113,254,48,254,4,254,229,253,207,253,202,253,193,253,210,253,225,253,232,253,229,253,231,253,181,253,161,253,154,253,153,253,166,253,163,253,156,253,120,253,80,
        253,6,253,165,252,73,252,234,251,194,251,190,251,215,251,252,251,46,252,102,252,110,252,103,252,71,252,63,252,60,252,78,252,124,252,186,252,27,253,116,253,240,253,101,254,198,254,68,255,192,255,39,0,115,0,187,0,22,1,123,1,226,1,115,2,235,2,89,3,171,3,
        250,3,56,4,123,4,181,4,217,4,1,5,0,5,0,5,27,5,40,5,66,5,131,5,216,5,43,6,115,6,147,6,156,6,147,6,138,6,122,6,129,6,163,6,191,6,224,6,45,7,128,7,201,7,31,8,93,8,126,8,129,8,131,8,165,8,179,8,180,8,208,8,17,9,59,9,91,9,87,9,32,9,236,8,182,8,113,8,18,8,
        130,7,223,6,152,6,46,6,225,5,145,5,122,5,91,5,81,5,50,5,214,4,103,4,251,3,111,3,12,3,189,2,135,2,99,2,56,2,35,2,234,1,193,1,168,1,94,1,58,1,245,0,202,0,163,0,108,0,65,0,26,0,238,255,209,255,142,255,77,255,43,255,254,254,241,254,44,255,124,255,208,255,
        21,0,55,0,62,0,18,0,198,255,137,255,75,255,66,255,73,255,62,255,61,255,33,255,21,255,242,254,206,254,135,254,83,254,29,254,226,253,179,253,91,253,21,253,237,252,210,252,176,252,121,252,46,252,211,251,146,251,148,251,151,251,169,251,209,251,36,252,122,
        252,181,252,160,252,103,252,24,252,164,251,36,251,122,250,180,249,28,249,174,248,92,248,43,248,8,248,245,247,12,248,44,248,37,248,31,248,23,248,49,248,113,248,216,248,83,249,221,249,121,250,35,251,223,251,113,252,221,252,53,253,107,253,164,253,224,253,
        27,254,108,254,209,254,76,255,191,255,40,0,107,0,143,0,159,0,191,0,211,0,229,0,0,1,24,1,51,1,58,1,56,1,64,1,67,1,75,1,85,1,83,1,76,1,50,1,21,1,235,0,217,0,204,0,168,0,130,0,54,0,205,255,109,255,23,255,191,254,112,254,45,254,244,253,186,253,156,253,125,
        253,81,253,27,253,205,252,165,252,138,252,117,252,104,252,106,252,140,252,182,252,217,252,235,252,2,253,46,253,94,253,141,253,178,253,210,253,236,253,15,254,38,254,59,254,87,254,112,254,163,254,214,254,21,255,72,255,131,255,163,255,219,255,15,0,65,0,
        91,0,108,0,105,0,108,0,108,0,102,0,84,0,71,0,69,0,51,0,15,0,199,255,98,255,9,255,207,254,173,254,167,254,169,254,177,254,164,254,130,254,131,254,167,254,171,254,167,254,155,254,153,254,145,254,167,254,195,254,202,254,178,254,157,254,141,254,130,254,126,
        254,85,254,44,254,20,254,217,253,136,253,60,253,5,253,228,252,233,252,235,252,241,252,241,252,244,252,250,252,6,253,24,253,33,253,67,253,138,253,214,253,66,254,188,254,43,255,160,255,32,0,130,0,237,0,105,1,225,1,86,2,178,2,255,2,49,3,98,3,128,3,167,3,
        195,3,226,3,20,4,92,4,204,4,55,5,148,5,219,5,17,6,58,6,101,6,155,6,195,6,210,6,253,6,46,7,79,7,94,7,115,7,112,7,76,7,53,7,50,7,36,7,17,7,27,7,39,7,69,7,75,7,130,7,223,7,13,8,61,8,97,8,151,8,182,8,234,8,251,8,218,8,172,8,99,8,57,8,225,7,105,7,0,7,144,
        6,30,6,183,5,83,5,246,4,145,4,29,4,179,3,88,3,9,3,200,2,142,2,108,2,57,2,251,1,179,1,153,1,105,1,81,1,33,1,248,0,198,0,127,0,105,0,55,0,253,255,242,255,206,255,202,255,191,255,173,255,137,255,116,255,95,255,72,255,34,255,3,255,238,254,254,254,34,255,
        84,255,131,255,192,255,226,255,242,255,252,255,241,255,232,255,252,255,6,0,242,255,217,255,206,255,205,255,188,255,156,255,115,255,55,255,227,254,103,254,239,253,134,253,24,253,178,252,81,252,230,251,147,251,70,251,13,251,216,250,206,250,229,250,13,251,
        61,251,117,251,161,251,204,251,236,251,222,251,158,251,81,251,251,250,127,250,7,250,155,249,50,249,218,248,160,248,124,248,110,248,108,248,153,248,236,248,57,249,119,249,154,249,167,249,169,249,176,249,219,249,31,250,173,250,108,251,70,252,9,253,177,
        253,41,254,113,254,148,254,169,254,184,254,207,254,242,254,40,255,112,255,176,255,216,255,10,0,75,0,144,0,197,0,226,0,234,0,230,0,233,0,204,0,176,0,155,0,150,0,162,0,175,0,180,0,172,0,173,0,159,0,129,0,51,0,228,255,159,255,119,255,93,255,75,255,18,255,
        200,254,116,254,19,254,196,253,114,253,56,253,30,253,239,252,208,252,157,252,107,252,77,252,30,252,248,251,211,251,197,251,186,251,186,251,227,251,16,252,102,252,179,252,10,253,88,253,146,253,188,253,220,253,239,253,12,254,56,254,85,254,148,254,223,254,
        69,255,156,255,230,255,24,0,69,0,105,0,114,0,118,0,102,0,98,0,107,0,111,0,123,0,116,0,102,0,69,0,29,0,4,0,3,0,248,255,206,255,177,255,112,255,58,255,29,255,245,254,15,255,62,255,95,255,133,255,167,255,141,255,120,255,117,255,90,255,65,255,47,255,33,255,
        52,255,70,255,95,255,95,255,43,255,12,255,247,254,234,254,224,254,151,254,109,254,90,254,49,254,33,254,243,253,178,253,152,253,148,253,145,253,127,253,130,253,153,253,226,253,48,254,135,254,235,254,79,255,187,255,51,0,168,0,27,1,147,1,244,1,72,2,142,
        2,218,2,28,3,56,3,59,3,44,3,41,3,63,3,114,3,167,3,235,3,59,4,126,4,223,4,59,5,165,5,15,6,94,6,183,6,248,6,46,7,68,7,96,7,117,7,162,7,194,7,200,7,220,7,216,7,200,7,194,7,153,7,96,7,86,7,78,7,89,7,105,7,122,7,155,7,225,7,43,8,96,8,142,8,147,8,143,8,122,
        8,67,8,247,7,136,7,248,6,118,6,32,6,198,5,120,5,55,5,210,4,108,4,4,4,181,3,75,3,218,2,120,2,16,2,166,1,96,1,38,1,9,1,244,0,217,0,208,0,170,0,109,0,50,0,242,255,170,255,115,255,84,255,63,255,69,255,52,255,58,255,44,255,23,255,242,254,202,254,191,254,188,
        254,191,254,193,254,210,254,220,254,243,254,29,255,79,255,130,255,149,255,165,255,191,255,212,255,237,255,241,255,248,255,24,0,66,0,73,0,97,0,87,0,47,0,8,0,201,255,137,255,58,255,192,254,42,254,139,253,254,252,132,252,32,252,194,251,97,251,17,251,215,
        250,134,250,103,250,85,250,115,250,152,250,179,250,190,250,190,250,194,250,213,250,219,250,216,250,198,250,180,250,150,250,122,250,69,250,40,250,6,250,252,249,36,250,65,250,87,250,98,250,121,250,159,250,188,250,215,250,0,251,71,251,153,251,234,251,46,
        252,117,252,196,252,52,253,179,253,58,254,162,254,234,254,36,255,75,255,93,255,105,255,94,255,97,255,112,255,148,255,188,255,239,255,39,0,116,0,181,0,234,0,0,1,247,0,211,0,157,0,105,0,54,0,6,0,223,255,202,255,208,255,202,255,169,255,138,255,75,255,0,
        255,177,254,112,254,72,254,37,254,29,254,37,254,23,254,242,253,171,253,116,253,44,253,219,252,158,252,97,252,67,252,71,252,96,252,95,252,85,252,73,252,31,252,25,252,13,252,20,252,59,252,120,252,182,252,19,253,92,253,146,253,200,253,242,253,42,254,90,
        254,121,254,171,254,221,254,34,255,102,255,183,255,244,255,61,0,122,0,162,0,176,0,159,0,154,0,158,0,150,0,140,0,141,0,108,0,69,0,55,0,32,0,25,0,1,0,12,0,18,0,22,0,22,0,1,0,231,255,196,255,183,255,219,255,14,0,72,0,89,0,98,0,48,0,250,255,217,255,176,255,
        162,255,129,255,133,255,160,255,185,255,224,255,239,255,238,255,213,255,165,255,145,255,93,255,57,255,26,255,47,255,57,255,22,255,207,254,121,254,48,254,251,253,11,254,33,254,72,254,123,254,216,254,44,255,134,255,216,255,57,0,157,0,248,0,85,1,190,1,43,
        2,139,2,214,2,253,2,20,3,31,3,49,3,67,3,75,3,96,3,160,3,222,3,30,4,87,4,121,4,170,4,217,4,29,5,120,5,219,5,58,6,123,6,201,6,252,6,17,7,46,7,42,7,50,7,72,7,100,7,132,7,144,7,165,7,168,7,162,7,194,7,168,7,161,7,162,7,161,7,122,7,68,7,42,7,32,7,45,7,65,
        7,82,7,61,7,18,7,219,6,134,6,40,6,178,5,37,5,149,4,19,4,152,3,57,3,2,3,218,2,219,2,203,2,133,2,23,2,136,1,7,1,133,0,48,0,253,255,244,255,212,255,221,255,199,255,191,255,178,255,159,255,144,255,127,255,105,255,98,255,88,255,83,255,72,255,62,255,62,255,
        40,255,14,255,238,254,220,254,205,254,216,254,1,255,21,255,50,255,69,255,86,255,113,255,147,255,147,255,137,255,129,255,145,255,163,255,173,255,174,255,188,255,206,255,241,255,15,0,21,0,248,255,187,255,112,255,16,255,166,254,60,254,206,253,109,253,19,
        253,175,252,78,252,7,252,175,251,65,251,216,250,111,250,17,250,237,249,228,249,249,249,54,250,104,250,163,250,213,250,233,250,251,250,229,250,253,250,50,251,110,251,165,251,211,251,229,251,244,251,248,251,16,252,14,252,6,252,248,251,251,251,18,252,38,
        252,66,252,124,252,205,252,30,253,105,253,154,253,171,253,178,253,204,253,240,253,37,254,94,254,174,254,254,254,65,255,109,255,133,255,123,255,106,255,105,255,104,255,115,255,165,255,235,255,72,0,147,0,187,0,194,0,168,0,109,0,46,0,219,255,145,255,75,
        255,40,255,7,255,239,254,228,254,192,254,152,254,102,254,49,254,253,253,190,253,148,253,134,253,127,253,107,253,102,253,99,253,73,253,27,253,217,252,114,252,57,252,32,252,41,252,57,252,84,252,113,252,110,252,106,252,75,252,63,252,56,252,81,252,131,252,
        193,252,255,252,87,253,161,253,221,253,12,254,52,254,120,254,171,254,247,254,72,255,151,255,250,255,64,0,122,0,154,0,181,0,176,0,177,0,172,0,180,0,176,0,179,0,166,0,173,0,170,0,175,0,183,0,145,0,134,0,136,0,132,0,152,0,172,0,176,0,180,0,180,0,180,0,154,
        0,133,0,129,0,141,0,147,0,136,0,141,0,94,0,39,0,248,255,212,255,230,255,234,255,11,0,54,0,87,0,101,0,98,0,71,0,25,0,231,255,198,255,209,255,232,255,231,255,227,255,239,255,206,255,160,255,88,255,33,255,254,254,4,255,40,255,80,255,158,255,230,255,64,0,
        136,0,169,0,222,0,13,1,75,1,151,1,229,1,59,2,129,2,217,2,17,3,51,3,55,3,41,3,46,3,93,3,125,3,168,3,208,3,217,3,239,3,8,4,23,4,62,4,87,4,160,4,1,5,119,5,205,5,8,6,64,6,104,6,126,6,133,6,137,6,162,6,180,6,212,6,252,6,27,7,72,7,86,7,50,7,251,6,162,6,65,
        6,244,5,188,5,141,5,97,5,41,5,249,4,234,4,206,4,192,4,188,4,182,4,146,4,69,4,228,3,110,3,241,2,96,2,252,1,208,1,184,1,212,1,222,1,189,1,94,1,251,0,115,0,238,255,133,255,63,255,16,255,3,255,5,255,29,255,95,255,131,255,144,255,136,255,116,255,90,255,84,
        255,75,255,59,255,61,255,75,255,106,255,134,255,141,255,149,255,142,255,156,255,145,255,122,255,105,255,87,255,93,255,84,255,97,255,113,255,162,255,210,255,250,255,248,255,235,255,202,255,176,255,134,255,84,255,50,255,47,255,69,255,72,255,41,255,0,255,
        211,254,170,254,101,254,9,254,178,253,92,253,255,252,150,252,35,252,178,251,79,251,3,251,208,250,176,250,144,250,108,250,85,250,89,250,93,250,108,250,123,250,177,250,247,250,46,251,94,251,154,251,191,251,234,251,75,252,146,252,200,252,255,252,42,253,
        89,253,128,253,128,253,131,253,127,253,121,253,112,253,114,253,134,253,163,253,190,253,229,253,249,253,253,253,1,254,4,254,8,254,18,254,38,254,70,254,120,254,181,254,231,254,30,255,76,255,109,255,160,255,194,255,196,255,205,255,216,255,212,255,217,255,
        235,255,250,255,242,255,226,255,183,255,124,255,69,255,7,255,213,254,180,254,128,254,84,254,37,254,8,254,226,253,202,253,182,253,189,253,185,253,167,253,167,253,148,253,125,253,94,253,49,253,27,253,13,253,15,253,1,253,9,253,13,253,5,253,17,253,5,253,
        251,252,255,252,251,252,254,252,27,253,64,253,112,253,152,253,196,253,246,253,40,254,81,254,116,254,156,254,228,254,44,255,112,255,209,255,36,0,122,0,176,0,201,0,201,0,191,0,199,0,184,0,206,0,206,0,224,0,233,0,2,1,7,1,6,1,7,1,9,1,17,1,9,1,25,1,29,1,21,
        1,22,1,13,1,14,1,0,1,212,0,183,0,114,0,61,0,25,0,255,255,10,0,29,0,65,0,78,0,84,0,82,0,82,0,115,0,101,0,93,0,87,0,84,0,90,0,82,0,96,0,82,0,86,0,105,0,125,0,145,0,154,0,159,0,136,0,87,0,39,0,226,255,185,255,184,255,216,255,244,255,28,0,64,0,100,0,129,
        0,169,0,209,0,27,1,108,1,219,1,72,2,178,2,15,3,66,3,96,3,128,3,139,3,146,3,175,3,185,3,192,3,213,3,244,3,249,3,250,3,6,4,29,4,45,4,73,4,110,4,166,4,0,5,66,5,130,5,178,5,214,5,10,6,50,6,76,6,97,6,101,6,123,6,126,6,129,6,98,6,75,6,19,6,221,5,137,5,65,5,
        225,4,146,4,77,4,0,4,189,3,125,3,75,3,28,3,5,3,215,2,199,2,176,2,164,2,120,2,66,2,241,1,151,1,65,1,234,0,163,0,126,0,66,0,24,0,230,255,178,255,109,255,37,255,250,254,196,254,182,254,187,254,206,254,205,254,228,254,241,254,243,254,252,254,18,255,51,255,
        113,255,167,255,202,255,223,255,239,255,4,0,6,0,7,0,255,255,241,255,224,255,212,255,201,255,180,255,166,255,154,255,174,255,198,255,252,255,33,0,65,0,65,0,35,0,246,255,220,255,166,255,123,255,76,255,47,255,0,255,246,254,232,254,206,254,184,254,138,254,
        70,254,2,254,186,253,112,253,46,253,214,252,142,252,92,252,49,252,28,252,2,252,216,251,193,251,166,251,136,251,104,251,75,251,42,251,25,251,27,251,42,251,67,251,104,251,166,251,255,251,99,252,196,252,33,253,121,253,202,253,13,254,67,254,108,254,135,254,
        163,254,181,254,181,254,167,254,149,254,119,254,98,254,77,254,56,254,33,254,27,254,30,254,48,254,56,254,65,254,76,254,77,254,91,254,113,254,135,254,171,254,217,254,14,255,73,255,98,255,124,255,117,255,113,255,83,255,77,255,65,255,39,255,43,255,54,255,
        57,255,66,255,66,255,50,255,15,255,223,254,164,254,105,254,55,254,6,254,228,253,210,253,185,253,164,253,154,253,141,253,135,253,113,253,94,253,99,253,109,253,123,253,112,253,98,253,88,253,81,253,88,253,103,253,134,253,153,253,172,253,182,253,199,253,
        207,253,224,253,0,254,29,254,59,254,98,254,139,254,187,254,223,254,30,255,75,255,140,255,210,255,8,0,69,0,100,0,115,0,139,0,152,0,161,0,176,0,194,0,204,0,224,0,249,0,29,1,51,1,92,1,133,1,157,1,183,1,200,1,197,1,180,1,165,1,140,1,125,1,121,1,89,1,64,1,
        31,1,238,0,181,0,118,0,54,0,3,0,228,255,228,255,245,255,24,0,68,0,94,0,91,0,72,0,54,0,51,0,54,0,46,0,54,0,79,0,120,0,159,0,170,0,186,0,175,0,183,0,188,0,215,0,224,0,233,0,219,0,206,0,148,0,91,0,36,0,0,0,241,255,249,255,22,0,68,0,127,0,198,0,255,0,85,
        1,147,1,234,1,34,2,85,2,154,2,219,2,28,3,96,3,159,3,183,3,220,3,251,3,38,4,58,4,55,4,41,4,29,4,18,4,9,4,9,4,20,4,74,4,126,4,202,4,27,5,81,5,142,5,173,5,183,5,202,5,213,5,230,5,237,5,228,5,201,5,183,5,144,5,122,5,91,5,43,5,228,4,144,4,40,4,200,3,118,3,
        24,3,200,2,104,2,27,2,200,1,125,1,76,1,33,1,21,1,10,1,20,1,28,1,244,0,183,0,97,0,17,0,181,255,102,255,50,255,15,255,247,254,238,254,235,254,229,254,211,254,195,254,169,254,134,254,117,254,101,254,88,254,110,254,159,254,195,254,250,254,48,255,94,255,122,
        255,144,255,165,255,178,255,174,255,173,255,173,255,165,255,140,255,134,255,129,255,130,255,140,255,178,255,219,255,232,255,248,255,6,0,14,0,29,0,24,0,0,0,248,255,234,255,198,255,149,255,98,255,65,255,21,255,211,254,152,254,88,254,16,254,203,253,171,
        253,132,253,100,253,85,253,82,253,88,253,98,253,74,253,48,253,243,252,215,252,199,252,183,252,187,252,168,252,160,252,138,252,133,252,106,252,71,252,54,252,60,252,73,252,103,252,142,252,197,252,28,253,130,253,221,253,40,254,92,254,134,254,173,254,216,
        254,0,255,30,255,54,255,58,255,70,255,61,255,43,255,16,255,249,254,229,254,213,254,200,254,187,254,167,254,130,254,103,254,87,254,70,254,83,254,110,254,128,254,139,254,142,254,133,254,123,254,121,254,116,254,108,254,115,254,138,254,180,254,202,254,227,
        254,241,254,249,254,235,254,223,254,192,254,180,254,152,254,134,254,102,254,69,254,41,254,249,253,211,253,186,253,166,253,125,253,94,253,70,253,63,253,60,253,46,253,63,253,81,253,102,253,116,253,125,253,132,253,113,253,112,253,124,253,142,253,190,253,
        249,253,40,254,94,254,138,254,157,254,170,254,184,254,213,254,246,254,52,255,112,255,165,255,224,255,19,0,50,0,57,0,93,0,118,0,130,0,148,0,150,0,141,0,159,0,186,0,213,0,242,0,17,1,60,1,108,1,150,1,189,1,202,1,193,1,187,1,169,1,150,1,125,1,99,1,68,1,45,
        1,18,1,251,0,212,0,180,0,134,0,90,0,68,0,32,0,255,255,223,255,198,255,178,255,195,255,191,255,201,255,202,255,220,255,234,255,6,0,58,0,104,0,126,0,151,0,190,0,217,0,0,1,21,1,36,1,35,1,29,1,27,1,22,1,20,1,27,1,252,0,241,0,219,0,194,0,155,0,141,0,158,0,
        204,0,21,1,79,1,100,1,133,1,171,1,191,1,222,1,254,1,30,2,74,2,153,2,229,2,41,3,118,3,156,3,195,3,231,3,253,3,20,4,37,4,33,4,37,4,56,4,48,4,63,4,78,4,84,4,98,4,109,4,130,4,155,4,199,4,242,4,8,5,11,5,239,4,218,4,204,4,185,4,166,4,141,4,113,4,76,4,31,4,
        236,3,160,3,67,3,240,2,142,2,66,2,219,1,140,1,60,1,238,0,159,0,97,0,57,0,32,0,24,0,12,0,6,0,237,255,199,255,136,255,77,255,18,255,223,254,198,254,178,254,167,254,157,254,157,254,138,254,123,254,112,254,69,254,62,254,78,254,92,254,115,254,134,254,156,
        254,187,254,232,254,21,255,62,255,104,255,112,255,119,255,108,255,90,255,72,255,45,255,36,255,50,255,88,255,137,255,163,255,195,255,209,255,248,255,15,0,18,0,26,0,50,0,79,0,80,0,71,0,30,0,230,255,172,255,97,255,18,255,196,254,139,254,102,254,67,254,54,
        254,22,254,243,253,196,253,160,253,132,253,112,253,113,253,117,253,152,253,189,253,210,253,220,253,213,253,192,253,181,253,177,253,179,253,196,253,210,253,211,253,221,253,222,253,211,253,213,253,220,253,221,253,221,253,229,253,238,253,5,254,19,254,56,
        254,99,254,141,254,171,254,200,254,245,254,16,255,65,255,119,255,158,255,181,255,206,255,214,255,224,255,228,255,212,255,194,255,151,255,111,255,65,255,16,255,216,254,170,254,131,254,127,254,134,254,141,254,141,254,134,254,120,254,105,254,91,254,83,254,
        77,254,96,254,137,254,167,254,193,254,207,254,214,254,227,254,213,254,202,254,177,254,139,254,109,254,73,254,45,254,27,254,249,253,218,253,195,253,177,253,174,253,166,253,160,253,152,253,136,253,130,253,110,253,125,253,142,253,167,253,217,253,240,253,
        239,253,243,253,240,253,253,253,19,254,52,254,102,254,142,254,184,254,245,254,27,255,83,255,112,255,134,255,167,255,187,255,226,255,248,255,35,0,78,0,116,0,147,0,168,0,180,0,186,0,199,0,216,0,227,0,231,0,255,0,14,1,31,1,50,1,75,1,114,1,135,1,144,1,140,
        1,122,1,89,1,56,1,24,1,253,0,233,0,202,0,177,0,150,0,132,0,116,0,100,0,101,0,91,0,76,0,48,0,6,0,232,255,172,255,129,255,113,255,129,255,165,255,208,255,6,0,40,0,62,0,102,0,141,0,204,0,240,0,21,1,65,1,101,1,125,1,130,1,141,1,144,1,136,1,158,1,172,1,190,
        1,176,1,191,1,204,1,204,1,204,1,186,1,171,1,136,1,137,1,160,1,183,1,197,1,225,1,250,1,28,2,52,2,86,2,146,2,185,2,228,2,48,3,96,3,136,3,186,3,243,3,70,4,121,4,170,4,195,4,189,4,167,4,148,4,130,4,90,4,54,4,29,4,26,4,30,4,51,4,69,4,73,4,87,4,96,4,96,4,74,
        4,33,4,240,3,164,3,113,3,62,3,35,3,236,2,192,2,142,2,85,2,18,2,172,1,86,1,0,1,170,0,104,0,39,0,238,255,173,255,116,255,81,255,50,255,41,255,16,255,242,254,199,254,160,254,130,254,90,254,73,254,56,254,55,254,62,254,70,254,87,254,85,254,98,254,109,254,
        126,254,133,254,139,254,137,254,121,254,134,254,148,254,155,254,184,254,200,254,211,254,231,254,235,254,234,254,223,254,209,254,221,254,246,254,14,255,68,255,113,255,152,255,185,255,214,255,242,255,8,0,11,0,7,0,22,0,26,0,8,0,228,255,187,255,148,255,102,
        255,30,255,243,254,191,254,156,254,124,254,105,254,80,254,67,254,54,254,51,254,62,254,59,254,48,254,44,254,34,254,22,254,15,254,9,254,24,254,45,254,65,254,102,254,141,254,177,254,203,254,234,254,7,255,26,255,33,255,12,255,249,254,241,254,228,254,223,
        254,207,254,180,254,171,254,160,254,149,254,124,254,105,254,113,254,131,254,174,254,227,254,23,255,79,255,140,255,188,255,219,255,227,255,235,255,230,255,227,255,220,255,210,255,187,255,160,255,130,255,88,255,39,255,241,254,184,254,144,254,116,254,96,
        254,83,254,80,254,92,254,113,254,124,254,128,254,120,254,115,254,106,254,88,254,78,254,80,254,90,254,95,254,112,254,128,254,146,254,138,254,119,254,63,254,4,254,218,253,192,253,181,253,175,253,174,253,177,253,189,253,207,253,206,253,184,253,175,253,168,
        253,163,253,156,253,157,253,152,253,170,253,184,253,208,253,244,253,22,254,52,254,112,254,169,254,217,254,11,255,45,255,99,255,144,255,165,255,184,255,205,255,237,255,12,0,43,0,75,0,107,0,129,0,175,0,206,0,231,0,255,0,11,1,18,1,38,1,54,1,54,1,50,1,38,
        1,31,1,11,1,11,1,14,1,20,1,29,1,32,1,11,1,231,0,186,0,143,0,104,0,89,0,71,0,73,0,76,0,76,0,72,0,61,0,39,0,8,0,238,255,224,255,228,255,213,255,209,255,188,255,188,255,198,255,230,255,19,0,58,0,111,0,127,0,175,0,220,0,233,0,33,1,71,1,126,1,176,1,225,1,
        3,2,32,2,46,2,56,2,63,2,77,2,88,2,84,2,85,2,79,2,74,2,45,2,21,2,247,1,243,1,243,1,237,1,244,1,6,2,39,2,89,2,132,2,150,2,171,2,192,2,230,2,6,3,34,3,98,3,160,3,214,3,6,4,31,4,36,4,42,4,27,4,12,4,0,4,242,3,236,3,229,3,221,3,210,3,186,3,161,3,141,3,123,3,
        113,3,88,3,60,3,27,3,233,2,165,2,103,2,52,2,240,1,180,1,146,1,107,1,79,1,57,1,17,1,215,0,155,0,82,0,253,255,177,255,115,255,57,255,12,255,221,254,199,254,169,254,153,254,128,254,98,254,94,254,74,254,66,254,58,254,65,254,58,254,56,254,56,254,66,254,94,
        254,113,254,139,254,166,254,184,254,206,254,207,254,203,254,187,254,173,254,156,254,119,254,110,254,110,254,130,254,146,254,171,254,210,254,0,255,27,255,48,255,65,255,84,255,113,255,147,255,159,255,160,255,174,255,173,255,170,255,145,255,126,255,117,
        255,129,255,156,255,177,255,183,255,187,255,160,255,127,255,73,255,11,255,228,254,193,254,191,254,211,254,221,254,223,254,205,254,196,254,182,254,171,254,153,254,141,254,135,254,152,254,175,254,203,254,223,254,239,254,16,255,51,255,73,255,94,255,112,
        255,140,255,169,255,174,255,170,255,160,255,133,255,99,255,66,255,27,255,3,255,235,254,234,254,227,254,217,254,218,254,221,254,228,254,241,254,3,255,26,255,45,255,61,255,65,255,75,255,84,255,99,255,122,255,127,255,126,255,124,255,113,255,102,255,73,255,
        57,255,36,255,9,255,225,254,175,254,133,254,102,254,83,254,85,254,98,254,109,254,113,254,121,254,106,254,106,254,95,254,70,254,40,254,26,254,11,254,8,254,0,254,8,254,22,254,58,254,62,254,62,254,52,254,27,254,254,253,222,253,195,253,178,253,177,253,181,
        253,192,253,215,253,228,253,247,253,251,253,249,253,250,253,254,253,4,254,11,254,29,254,62,254,106,254,155,254,202,254,0,255,48,255,81,255,124,255,162,255,213,255,250,255,33,0,53,0,80,0,101,0,127,0,157,0,186,0,205,0,231,0,4,1,25,1,49,1,67,1,75,1,87,1,
        87,1,81,1,68,1,47,1,39,1,27,1,10,1,244,0,223,0,191,0,154,0,127,0,115,0,105,0,101,0,102,0,104,0,94,0,79,0,50,0,14,0,242,255,226,255,221,255,232,255,244,255,24,0,40,0,53,0,51,0,51,0,36,0,18,0,25,0,42,0,65,0,97,0,141,0,175,0,223,0,241,0,21,1,69,1,111,1,
        150,1,168,1,205,1,245,1,25,2,66,2,88,2,107,2,100,2,118,2,117,2,107,2,85,2,70,2,57,2,42,2,34,2,6,2,12,2,31,2,46,2,78,2,93,2,113,2,135,2,154,2,181,2,200,2,223,2,240,2,246,2,255,2,10,3,27,3,60,3,74,3,95,3,121,3,150,3,167,3,177,3,170,3,164,3,168,3,172,3,
        165,3,132,3,96,3,34,3,229,2,174,2,120,2,66,2,23,2,240,1,186,1,140,1,87,1,32,1,241,0,197,0,147,0,120,0,98,0,73,0,37,0,4,0,214,255,181,255,123,255,75,255,8,255,229,254,189,254,153,254,144,254,135,254,127,254,134,254,133,254,119,254,108,254,98,254,102,254,
        110,254,119,254,116,254,109,254,105,254,96,254,108,254,138,254,151,254,169,254,200,254,214,254,211,254,191,254,157,254,133,254,133,254,148,254,175,254,205,254,0,255,61,255,102,255,108,255,115,255,112,255,97,255,80,255,52,255,22,255,8,255,245,254,4,255,
        15,255,32,255,45,255,47,255,66,255,84,255,108,255,136,255,149,255,162,255,162,255,152,255,145,255,133,255,116,255,95,255,75,255,73,255,63,255,68,255,59,255,54,255,66,255,76,255,90,255,109,255,129,255,140,255,141,255,133,255,119,255,126,255,137,255,149,
        255,160,255,173,255,210,255,235,255,246,255,244,255,219,255,185,255,163,255,142,255,126,255,105,255,111,255,105,255,102,255,102,255,87,255,73,255,50,255,25,255,253,254,211,254,203,254,207,254,224,254,241,254,252,254,12,255,14,255,33,255,43,255,45,255,
        40,255,23,255,21,255,18,255,12,255,5,255,247,254,224,254,202,254,162,254,123,254,85,254,62,254,44,254,41,254,56,254,67,254,92,254,106,254,103,254,87,254,59,254,24,254,12,254,1,254,12,254,40,254,81,254,120,254,139,254,152,254,128,254,91,254,69,254,38,
        254,6,254,233,253,236,253,231,253,246,253,16,254,41,254,69,254,85,254,92,254,131,254,142,254,163,254,174,254,213,254,1,255,23,255,51,255,98,255,162,255,227,255,3,0,43,0,79,0,108,0,139,0,173,0,190,0,204,0,229,0,244,0,251,0,6,1,7,1,9,1,25,1,39,1,49,1,68,
        1,90,1,104,1,97,1,87,1,51,1,9,1,224,0,195,0,169,0,147,0,144,0,130,0,127,0,119,0,101,0,80,0,47,0,7,0,231,255,206,255,203,255,210,255,212,255,231,255,235,255,235,255,253,255,1,0,17,0,32,0,58,0,84,0,101,0,114,0,123,0,130,0,148,0,177,0,205,0,244,0,28,1,60,
        1,85,1,97,1,110,1,129,1,166,1,202,1,236,1,0,2,24,2,59,2,81,2,96,2,104,2,96,2,89,2,81,2,95,2,99,2,118,2,128,2,139,2,149,2,128,2,129,2,140,2,142,2,158,2,175,2,172,2,158,2,139,2,121,2,121,2,110,2,103,2,131,2,160,2,192,2,221,2,248,2,28,3,62,3,92,3,116,3,
        116,3,107,3,75,3,46,3,241,2,199,2,153,2,111,2,59,2,250,1,183,1,115,1,60,1,15,1,224,0,184,0,150,0,98,0,69,0,26,0,226,255,180,255,129,255,109,255,95,255,93,255,84,255,66,255,45,255,29,255,7,255,245,254,210,254,198,254,181,254,174,254,171,254,175,254,175,
        254,162,254,149,254,144,254,139,254,138,254,127,254,137,254,126,254,103,254,70,254,40,254,40,254,54,254,67,254,101,254,139,254,184,254,225,254,247,254,249,254,253,254,5,255,8,255,5,255,249,254,249,254,9,255,15,255,12,255,242,254,229,254,210,254,189,254,
        177,254,187,254,195,254,193,254,211,254,228,254,236,254,250,254,19,255,65,255,117,255,156,255,185,255,210,255,223,255,214,255,201,255,181,255,160,255,160,255,170,255,188,255,202,255,219,255,219,255,241,255,245,255,245,255,231,255,227,255,223,255,220,
        255,208,255,203,255,196,255,199,255,206,255,226,255,231,255,238,255,224,255,224,255,224,255,228,255,224,255,235,255,235,255,230,255,231,255,230,255,226,255,205,255,178,255,142,255,108,255,66,255,19,255,241,254,223,254,199,254,181,254,170,254,167,254,
        187,254,187,254,192,254,203,254,223,254,231,254,250,254,252,254,253,254,3,255,254,254,1,255,246,254,235,254,203,254,169,254,130,254,102,254,69,254,51,254,38,254,41,254,38,254,52,254,60,254,69,254,85,254,92,254,108,254,108,254,113,254,109,254,110,254,
        123,254,146,254,159,254,192,254,214,254,216,254,207,254,187,254,156,254,113,254,90,254,56,254,55,254,52,254,65,254,85,254,131,254,180,254,209,254,252,254,16,255,40,255,54,255,83,255,129,255,167,255,224,255,14,0,46,0,86,0,111,0,137,0,165,0,181,0,187,0,
        195,0,208,0,217,0,233,0,240,0,241,0,251,0,253,0,6,1,13,1,17,1,9,1,255,0,238,0,235,0,222,0,217,0,204,0,193,0,180,0,180,0,170,0,166,0,169,0,151,0,134,0,90,0,35,0,250,255,205,255,191,255,188,255,190,255,195,255,199,255,192,255,187,255,188,255,191,255,206,
        255,232,255,15,0,50,0,101,0,150,0,191,0,217,0,247,0,10,1,7,1,2,1,252,0,252,0,20,1,43,1,75,1,105,1,125,1,137,1,151,1,161,1,191,1,227,1,255,1,3,2,14,2,27,2,48,2,84,2,100,2,113,2,131,2,158,2,175,2,169,2,174,2,157,2,162,2,144,2,125,2,78,2,45,2,34,2,5,2,254,
        1,240,1,254,1,2,2,21,2,46,2,45,2,71,2,96,2,115,2,158,2,179,2,192,2,194,2,179,2,176,2,139,2,115,2,86,2,57,2,20,2,230,1,178,1,126,1,82,1,25,1,230,0,173,0,114,0,76,0,33,0,253,255,212,255,160,255,120,255,94,255,63,255,45,255,19,255,15,255,9,255,9,255,14,
        255,21,255,15,255,15,255,0,255,236,254,232,254,228,254,238,254,231,254,221,254,203,254,182,254,146,254,124,254,101,254,103,254,108,254,109,254,119,254,110,254,92,254,74,254,74,254,84,254,110,254,157,254,206,254,7,255,41,255,55,255,62,255,54,255,41,255,
        12,255,236,254,206,254,185,254,184,254,184,254,175,254,175,254,185,254,195,254,200,254,213,254,217,254,228,254,216,254,242,254,1,255,21,255,29,255,61,255,98,255,144,255,188,255,216,255,232,255,244,255,3,0,4,0,246,255,237,255,246,255,3,0,29,0,50,0,55,
        0,53,0,37,0,25,0,22,0,10,0,8,0,1,0,1,0,10,0,7,0,3,0,0,0,252,255,235,255,230,255,228,255,239,255,7,0,28,0,37,0,46,0,39,0,33,0,8,0,252,255,221,255,199,255,180,255,155,255,141,255,120,255,99,255,70,255,43,255,11,255,229,254,207,254,188,254,175,254,167,254,
        169,254,178,254,192,254,213,254,234,254,252,254,247,254,241,254,235,254,227,254,214,254,206,254,189,254,177,254,164,254,144,254,141,254,110,254,90,254,58,254,31,254,20,254,33,254,51,254,90,254,121,254,157,254,193,254,216,254,221,254,218,254,210,254,221,
        254,224,254,223,254,245,254,4,255,22,255,32,255,30,255,9,255,250,254,224,254,207,254,198,254,198,254,199,254,211,254,250,254,43,255,75,255,111,255,152,255,203,255,227,255,253,255,21,0,47,0,72,0,97,0,123,0,145,0,159,0,173,0,183,0,184,0,190,0,183,0,183,
        0,199,0,219,0,251,0,20,1,28,1,20,1,252,0,224,0,198,0,166,0,141,0,129,0,126,0,133,0,154,0,172,0,195,0,206,0,198,0,176,0,133,0,90,0,43,0,14,0,246,255,232,255,223,255,206,255,194,255,187,255,163,255,160,255,159,255,165,255,172,255,196,255,220,255,12,0,62,
        0,120,0,166,0,206,0,249,0,17,1,32,1,36,1,43,1,57,1,64,1,79,1,78,1,76,1,71,1,81,1,86,1,97,1,123,1,135,1,150,1,157,1,176,1,197,1,225,1,245,1,9,2,42,2,75,2,118,2,140,2,143,2,149,2,151,2,157,2,161,2,124,2,100,2,67,2,36,2,3,2,232,1,205,1,194,1,197,1,196,1,
        211,1,215,1,244,1,17,2,42,2,61,2,61,2,61,2,46,2,20,2,10,2,240,1,230,1,211,1,197,1,178,1,164,1,143,1,123,1,96,1,74,1,35,1,253,0,216,0,169,0,104,0,29,0,202,255,136,255,93,255,69,255,50,255,41,255,41,255,33,255,29,255,12,255,239,254,220,254,202,254,205,
        254,214,254,236,254,245,254,7,255,19,255,18,255,12,255,239,254,206,254,170,254,133,254,94,254,78,254,60,254,40,254,37,254,41,254,63,254,98,254,116,254,139,254,157,254,175,254,185,254,191,254,209,254,220,254,229,254,232,254,245,254,246,254,234,254,232,
        254,214,254,193,254,173,254,173,254,171,254,166,254,156,254,156,254,173,254,180,254,210,254,231,254,247,254,15,255,33,255,52,255,76,255,101,255,117,255,141,255,177,255,196,255,227,255,245,255,14,0,36,0,46,0,48,0,57,0,65,0,80,0,86,0,84,0,82,0,80,0,80,
        0,73,0,64,0,60,0,53,0,53,0,57,0,53,0,44,0,50,0,53,0,61,0,44,0,37,0,35,0,39,0,50,0,57,0,75,0,89,0,91,0,68,0,36,0,0,0,228,255,201,255,190,255,181,255,173,255,174,255,170,255,154,255,129,255,104,255,76,255,48,255,27,255,253,254,238,254,234,254,234,254,229,
        254,234,254,236,254,234,254,229,254,217,254,203,254,198,254,189,254,199,254,202,254,207,254,207,254,192,254,162,254,139,254,121,254,123,254,112,254,108,254,117,254,119,254,121,254,145,254,157,254,173,254,202,254,234,254,4,255,29,255,44,255,48,255,62,
        255,65,255,51,255,43,255,44,255,43,255,48,255,51,255,55,255,57,255,51,255,45,255,51,255,63,255,70,255,102,255,138,255,177,255,220,255,252,255,28,0,46,0,65,0,89,0,98,0,111,0,122,0,132,0,147,0,162,0,176,0,190,0,195,0,201,0,199,0,205,0,212,0,209,0,206,0,
        209,0,204,0,183,0,172,0,145,0,123,0,109,0,97,0,108,0,114,0,144,0,159,0,162,0,162,0,137,0,115,0,75,0,51,0,29,0,17,0,255,255,234,255,212,255,180,255,167,255,162,255,156,255,160,255,173,255,181,255,206,255,238,255,24,0,65,0,104,0,133,0,166,0,188,0,217,0,
        224,0,249,0,15,1,47,1,78,1,100,1,108,1,119,1,126,1,123,1,125,1,119,1,107,1,107,1,122,1,144,1,155,1,179,1,191,1,226,1,233,1,248,1,0,2,5,2,17,2,38,2,64,2,81,2,81,2,81,2,64,2,54,2,23,2,252,1,250,1,247,1,241,1,227,1,214,1,208,1,198,1,190,1,187,1,191,1,202,
        1,208,1,205,1,189,1,176,1,151,1,114,1,89,1,61,1,45,1,42,1,21,1,18,1,24,1,25,1,15,1,11,1,255,0,231,0,186,0,130,0,68,0,252,255,195,255,159,255,129,255,102,255,72,255,51,255,37,255,21,255,18,255,11,255,247,254,238,254,232,254,234,254,223,254,223,254,220,
        254,216,254,221,254,220,254,218,254,210,254,187,254,188,254,157,254,126,254,83,254,59,254,29,254,23,254,19,254,23,254,54,254,87,254,126,254,148,254,160,254,175,254,180,254,185,254,187,254,187,254,196,254,203,254,207,254,217,254,228,254,242,254,236,254,
        236,254,238,254,249,254,247,254,245,254,228,254,220,254,217,254,224,254,231,254,0,255,23,255,62,255,94,255,120,255,144,255,180,255,199,255,232,255,6,0,21,0,48,0,62,0,73,0,90,0,87,0,91,0,98,0,101,0,107,0,109,0,109,0,108,0,101,0,87,0,86,0,87,0,86,0,90,
        0,83,0,78,0,80,0,84,0,93,0,109,0,111,0,109,0,97,0,87,0,79,0,72,0,55,0,57,0,57,0,44,0,43,0,37,0,17,0,248,255,228,255,205,255,198,255,187,255,173,255,166,255,162,255,154,255,145,255,131,255,117,255,106,255,87,255,72,255,45,255,9,255,245,254,224,254,213,
        254,211,254,203,254,189,254,187,254,188,254,196,254,193,254,202,254,209,254,207,254,191,254,187,254,181,254,180,254,174,254,162,254,171,254,177,254,181,254,175,254,175,254,170,254,175,254,203,254,235,254,21,255,57,255,91,255,120,255,124,255,124,255,104,
        255,80,255,69,255,58,255,55,255,63,255,73,255,106,255,126,255,151,255,167,255,176,255,174,255,188,255,212,255,246,255,11,0,36,0,47,0,60,0,66,0,71,0,71,0,72,0,83,0,86,0,94,0,118,0,133,0,139,0,161,0,175,0,180,0,188,0,180,0,166,0,148,0,139,0,129,0,120,0,
        132,0,129,0,140,0,143,0,151,0,151,0,136,0,127,0,115,0,104,0,87,0,79,0,66,0,64,0,55,0,50,0,30,0,17,0,245,255,216,255,191,255,180,255,184,255,188,255,208,255,228,255,250,255,19,0,42,0,64,0,76,0,93,0,116,0,130,0,145,0,173,0,208,0,231,0,10,1,36,1,53,1,81,
        1,107,1,126,1,139,1,151,1,165,1,169,1,164,1,162,1,146,1,141,1,140,1,154,1,165,1,171,1,176,1,179,1,168,1,165,1,168,1,171,1,182,1,183,1,196,1,205,1,211,1,200,1,193,1,183,1,178,1,173,1,191,1,209,1,208,1,200,1,205,1,194,1,171,1,148,1,123,1,99,1,64,1,35,1,
        7,1,241,0,222,0,206,0,199,0,184,0,177,0,165,0,159,0,163,0,155,0,157,0,155,0,137,0,105,0,73,0,32,0,3,0,230,255,209,255,188,255,173,255,154,255,123,255,79,255,47,255,25,255,11,255,15,255,25,255,37,255,41,255,30,255,14,255,249,254,218,254,187,254,177,254,
        166,254,163,254,162,254,163,254,151,254,139,254,130,254,105,254,91,254,76,254,66,254,67,254,76,254,96,254,117,254,108,254,102,254,92,254,95,254,102,254,120,254,141,254,159,254,189,254,206,254,217,254,216,254,231,254,245,254,12,255,18,255,27,255,37,255,
        48,255,39,255,26,255,3,255,246,254,3,255,5,255,29,255,50,255,83,255,122,255,159,255,196,255,230,255,0,0,32,0,58,0,84,0,109,0,134,0,145,0,145,0,143,0,130,0,136,0,136,0,136,0,144,0,162,0,159,0,150,0,143,0,134,0,134,0,122,0,118,0,133,0,139,0,130,0,130,0,
        130,0,137,0,140,0,133,0,129,0,125,0,107,0,97,0,66,0,43,0,21,0,250,255,231,255,245,255,241,255,250,255,10,0,12,0,18,0,250,255,230,255,202,255,183,255,170,255,156,255,155,255,148,255,147,255,134,255,112,255,91,255,65,255,41,255,1,255,220,254,184,254,159,
        254,139,254,144,254,169,254,188,254,210,254,224,254,224,254,216,254,200,254,205,254,205,254,209,254,211,254,217,254,232,254,232,254,235,254,238,254,249,254,254,254,7,255,14,255,25,255,33,255,57,255,81,255,102,255,123,255,126,255,133,255,140,255,129,255,
        124,255,126,255,137,255,141,255,156,255,181,255,196,255,220,255,244,255,7,0,29,0,39,0,30,0,25,0,15,0,22,0,22,0,19,0,29,0,42,0,47,0,65,0,75,0,82,0,87,0,87,0,94,0,96,0,102,0,109,0,115,0,120,0,126,0,118,0,111,0,104,0,109,0,107,0,109,0,122,0,122,0,127,0,
        137,0,133,0,126,0,102,0,68,0,58,0,43,0,33,0,26,0,26,0,22,0,14,0,252,255,238,255,238,255,246,255,3,0,8,0,15,0,32,0,42,0,47,0,55,0,61,0,66,0,79,0,86,0,104,0,116,0,139,0,163,0,201,0,222,0,3,1,42,1,71,1,87,1,108,1,128,1,158,1,179,1,196,1,205,1,211,1,215,
        1,204,1,200,1,175,1,153,1,122,1,97,1,83,1,75,1,83,1,90,1,105,1,107,1,119,1,122,1,123,1,126,1,118,1,129,1,137,1,157,1,175,1,172,1,182,1,178,1,173,1,173,1,166,1,161,1,150,1,126,1,82,1,36,1,238,0,188,0,154,0,122,0,118,0,108,0,107,0,107,0,108,0,105,0,105,
        0,89,0,66,0,54,0,47,0,33,0,32,0,24,0,25,0,24,0,10,0,8,0,249,255,221,255,185,255,141,255,106,255,79,255,65,255,54,255,58,255,55,255,45,255,30,255,21,255,14,255,234,254,209,254,184,254,169,254,138,254,123,254,117,254,113,254,116,254,115,254,119,254,119,
        254,121,254,121,254,108,254,101,254,96,254,92,254,80,254,60,254,54,254,41,254,30,254,45,254,70,254,108,254,137,254,167,254,193,254,220,254,246,254,7,255,25,255,41,255,57,255,63,255,59,255,51,255,45,255,40,255,58,255,73,255,97,255,117,255,140,255,169,
        255,188,255,198,255,206,255,224,255,249,255,24,0,55,0,89,0,125,0,151,0,169,0,175,0,162,0,157,0,144,0,150,0,155,0,172,0,186,0,184,0,195,0,195,0,190,0,198,0,193,0,202,0,199,0,191,0,177,0,170,0,166,0,158,0,155,0,151,0,137,0,129,0,118,0,101,0,82,0,60,0,40,
        0,26,0,14,0,7,0,253,255,244,255,248,255,3,0,19,0,33,0,37,0,35,0,29,0,11,0,246,255,231,255,213,255,199,255,188,255,167,255,149,255,131,255,102,255,79,255,55,255,27,255,0,255,225,254,193,254,199,254,193,254,203,254,217,254,216,254,223,254,225,254,228,254,
        228,254,229,254,243,254,246,254,253,254,15,255,23,255,43,255,62,255,73,255,84,255,99,255,95,255,104,255,98,255,98,255,93,255,97,255,93,255,109,255,116,255,138,255,152,255,167,255,181,255,187,255,195,255,188,255,192,255,199,255,210,255,226,255,235,255,
        249,255,0,0,8,0,17,0,6,0,253,255,253,255,255,255,252,255,7,0,11,0,26,0,30,0,50,0,61,0,68,0,72,0,72,0,68,0,64,0,53,0,62,0,60,0,71,0,80,0,98,0,104,0,101,0,109,0,108,0,102,0,94,0,76,0,60,0,55,0,35,0,35,0,32,0,22,0,21,0,11,0,252,255,241,255,231,255,237,255,
        239,255,255,255,19,0,36,0,48,0,47,0,50,0,46,0,32,0,24,0,29,0,36,0,46,0,55,0,82,0,108,0,139,0,162,0,191,0,215,0,238,0,10,1,36,1,76,1,112,1,150,1,176,1,184,1,187,1,194,1,193,1,191,1,193,1,189,1,178,1,157,1,135,1,103,1,71,1,54,1,43,1,45,1,36,1,46,1,47,1,
        45,1,46,1,49,1,65,1,93,1,94,1,105,1,126,1,140,1,132,1,128,1,136,1,130,1,118,1,92,1,74,1,36,1,0,1,220,0,188,0,150,0,112,0,79,0,51,0,42,0,28,0,24,0,8,0,1,0,252,255,244,255,234,255,235,255,224,255,219,255,223,255,238,255,245,255,3,0,7,0,3,0,231,255,210,
        255,191,255,178,255,154,255,137,255,122,255,98,255,83,255,68,255,43,255,22,255,253,254,234,254,211,254,195,254,192,254,170,254,167,254,151,254,139,254,128,254,117,254,108,254,109,254,120,254,123,254,135,254,127,254,108,254,102,254,94,254,81,254,87,254,
        77,254,77,254,80,254,92,254,92,254,115,254,127,254,155,254,185,254,216,254,246,254,19,255,37,255,51,255,52,255,54,255,58,255,51,255,61,255,83,255,119,255,163,255,203,255,235,255,250,255,7,0,0,0,241,255,238,255,245,255,6,0,17,0,36,0,72,0,102,0,126,0,145,
        0,151,0,159,0,162,0,169,0,170,0,170,0,170,0,183,0,199,0,220,0,216,0,227,0,222,0,229,0,238,0,229,0,216,0,204,0,181,0,172,0,165,0,150,0,148,0,130,0,111,0,93,0,76,0,62,0,46,0,36,0,30,0,26,0,22,0,19,0,14,0,26,0,25,0,26,0,21,0,21,0,22,0,33,0,36,0,30,0,15,
        0,253,255,224,255,194,255,163,255,133,255,116,255,95,255,88,255,77,255,65,255,54,255,44,255,33,255,18,255,1,255,243,254,220,254,214,254,213,254,214,254,227,254,239,254,4,255,16,255,32,255,50,255,63,255,76,255,98,255,119,255,147,255,159,255,170,255,190,
        255,187,255,188,255,159,255,160,255,159,255,155,255,158,255,165,255,169,255,185,255,206,255,234,255,242,255,0,0,252,255,234,255,223,255,212,255,202,255,201,255,194,255,212,255,230,255,249,255,11,0,24,0,18,0,11,0,1,0,248,255,250,255,10,0,17,0,30,0,57,
        0,66,0,71,0,79,0,66,0,64,0,54,0,40,0,40,0,36,0,48,0,75,0,87,0,97,0,97,0,98,0,82,0,62,0,36,0,11,0,253,255,249,255,10,0,29,0,36,0,29,0,19,0,3,0,245,255,235,255,237,255,241,255,250,255,6,0,19,0,28,0,25,0,32,0,35,0,22,0,29,0,28,0,28,0,42,0,53,0,73,0,93,0,
        112,0,145,0,163,0,188,0,220,0,252,0,29,1,51,1,68,1,82,1,100,1,114,1,121,1,132,1,139,1,146,1,137,1,130,1,119,1,110,1,101,1,90,1,76,1,50,1,36,1,7,1,249,0,249,0,242,0,242,0,255,0,252,0,11,1,13,1,31,1,32,1,49,1,54,1,53,1,56,1,57,1,56,1,38,1,24,1,6,1,242,
        0,226,0,198,0,177,0,150,0,134,0,109,0,73,0,35,0,7,0,241,255,224,255,210,255,202,255,195,255,196,255,192,255,202,255,212,255,213,255,231,255,230,255,231,255,220,255,214,255,209,255,203,255,201,255,191,255,180,255,180,255,172,255,154,255,133,255,102,255,
        73,255,39,255,5,255,235,254,207,254,191,254,185,254,188,254,195,254,181,254,171,254,170,254,153,254,135,254,113,254,91,254,87,254,87,254,92,254,106,254,124,254,133,254,127,254,128,254,113,254,103,254,94,254,95,254,112,254,127,254,151,254,169,254,191,
        254,210,254,223,254,246,254,4,255,37,255,44,255,51,255,61,255,81,255,97,255,133,255,173,255,210,255,248,255,24,0,40,0,43,0,40,0,25,0,14,0,12,0,14,0,24,0,35,0,62,0,87,0,101,0,111,0,136,0,150,0,151,0,154,0,155,0,165,0,177,0,193,0,206,0,230,0,229,0,230,
        0,235,0,234,0,227,0,222,0,209,0,215,0,206,0,194,0,179,0,166,0,141,0,122,0,104,0,96,0,84,0,76,0,64,0,58,0,53,0,50,0,48,0,42,0,29,0,21,0,11,0,1,0,246,255,235,255,232,255,242,255,244,255,248,255,253,255,242,255,221,255,212,255,184,255,165,255,149,255,138,
        255,129,255,126,255,126,255,134,255,129,255,113,255,99,255,75,255,54,255,40,255,22,255,12,255,254,254,253,254,14,255,23,255,25,255,47,255,69,255,99,255,131,255,147,255,176,255,202,255,214,255,230,255,232,255,241,255,248,255,249,255,11,0,11,0,14,0,12,
        0,245,255,238,255,237,255,232,255,224,255,221,255,228,255,245,255,248,255,235,255,231,255,227,255,223,255,213,255,209,255,195,255,201,255,208,255,216,255,237,255,246,255,0,0,6,0,4,0,4,0,7,0,10,0,22,0,22,0,36,0,51,0,54,0,57,0,61,0,64,0,61,0,55,0,54,0,
        62,0,61,0,60,0,54,0,47,0,33,0,24,0,7,0,6,0,15,0,15,0,15,0,26,0,29,0,24,0,18,0,12,0,3,0,4,0,249,255,253,255,3,0,7,0,10,0,8,0,6,0,6,0,10,0,17,0,35,0,44,0,61,0,89,0,93,0,101,0,114,0,126,0,145,0,169,0,183,0,206,0,244,0,14,1,29,1,45,1,45,1,38,1,43,1,54,1,
        58,1,63,1,57,1,63,1,69,1,74,1,86,1,79,1,81,1,74,1,61,1,36,1,17,1,244,0,231,0,219,0,220,0,226,0,238,0,235,0,234,0,233,0,229,0,231,0,241,0,241,0,253,0,245,0,240,0,240,0,230,0,226,0,206,0,198,0,183,0,169,0,145,0,126,0,107,0,73,0,48,0,26,0,242,255,213,255,
        201,255,190,255,185,255,184,255,185,255,180,255,190,255,206,255,209,255,201,255,187,255,166,255,165,255,156,255,165,255,162,255,165,255,180,255,178,255,167,255,159,255,142,255,112,255,91,255,61,255,29,255,1,255,229,254,214,254,188,254,171,254,169,254,
        153,254,151,254,146,254,135,254,126,254,113,254,112,254,109,254,102,254,96,254,102,254,126,254,135,254,145,254,148,254,145,254,144,254,149,254,151,254,138,254,146,254,155,254,167,254,188,254,196,254,207,254,224,254,235,254,7,255,34,255,61,255,91,255,
        129,255,165,255,201,255,226,255,241,255,0,0,8,0,19,0,17,0,17,0,33,0,37,0,47,0,53,0,62,0,69,0,79,0,97,0,91,0,105,0,116,0,118,0,129,0,137,0,147,0,162,0,177,0,186,0,195,0,195,0,195,0,205,0,201,0,208,0,205,0,204,0,206,0,198,0,191,0,186,0,187,0,175,0,168,
        0,152,0,133,0,111,0,107,0,104,0,100,0,84,0,76,0,55,0,43,0,28,0,17,0,7,0,10,0,248,255,248,255,241,255,242,255,234,255,219,255,212,255,202,255,191,255,185,255,185,255,169,255,170,255,174,255,177,255,181,255,188,255,187,255,174,255,160,255,151,255,134,255,
        124,255,105,255,104,255,94,255,73,255,61,255,50,255,45,255,44,255,79,255,81,255,93,255,113,255,133,255,160,255,187,255,209,255,234,255,244,255,252,255,15,0,40,0,55,0,71,0,89,0,80,0,83,0,64,0,44,0,17,0,253,255,239,255,228,255,221,255,216,255,221,255,219,
        255,234,255,237,255,234,255,230,255,220,255,220,255,212,255,198,255,191,255,194,255,206,255,212,255,238,255,250,255,12,0,24,0,25,0,22,0,19,0,14,0,11,0,15,0,25,0,35,0,43,0,51,0,61,0,61,0,53,0,40,0,21,0,12,0,3,0,253,255,6,0,14,0,32,0,26,0,25,0,22,0,22,
        0,6,0,255,255,252,255,0,0,0,0,3,0,7,0,7,0,11,0,6,0,1,0,255,255,250,255,249,255,255,255,18,0,39,0,53,0,84,0,107,0,120,0,127,0,137,0,150,0,158,0,173,0,186,0,197,0,217,0,229,0,244,0,255,0,4,1,17,1,27,1,27,1,28,1,15,1,15,1,25,1,38,1,42,1,57,1,58,1,63,1,51,
        1,40,1,29,1,17,1,255,0,237,0,226,0,216,0,209,0,205,0,201,0,193,0,187,0,175,0,169,0,166,0,162,0,162,0,162,0,165,0,168,0,168,0,173,0,172,0,158,0,150,0,139,0,133,0,129,0,123,0,114,0,100,0,72,0,47,0,3,0,227,255,203,255,192,255,195,255,185,255,187,255,180,
        255,173,255,170,255,162,255,152,255,145,255,138,255,134,255,133,255,129,255,144,255,148,255,156,255,147,255,145,255,141,255,136,255,115,255,81,255,57,255,33,255,4,255,231,254,203,254,182,254,171,254,170,254,174,254,173,254,171,254,166,254,163,254,151,
        254,134,254,127,254,127,254,133,254,139,254,141,254,151,254,151,254,156,254,152,254,156,254,167,254,177,254,184,254,196,254,207,254,205,254,216,254,227,254,241,254,4,255,30,255,57,255,97,255,130,255,169,255,191,255,206,255,210,255,213,255,210,255,213,
        255,216,255,220,255,234,255,11,0,37,0,68,0,90,0,102,0,102,0,119,0,120,0,118,0,122,0,109,0,104,0,120,0,127,0,143,0,151,0,162,0,175,0,186,0,191,0,188,0,191,0,198,0,206,0,212,0,220,0,226,0,216,0,205,0,195,0,180,0,172,0,162,0,157,0,151,0,148,0,133,0,119,
        0,107,0,89,0,73,0,65,0,47,0,33,0,15,0,4,0,255,255,246,255,241,255,235,255,223,255,208,255,210,255,202,255,195,255,184,255,181,255,177,255,173,255,174,255,177,255,173,255,188,255,191,255,192,255,198,255,194,255,184,255,173,255,166,255,158,255,154,255,
        154,255,142,255,134,255,127,255,123,255,126,255,116,255,129,255,129,255,144,255,158,255,177,255,195,255,206,255,228,255,249,255,14,0,36,0,51,0,69,0,83,0,79,0,82,0,79,0,66,0,58,0,51,0,42,0,24,0,4,0,244,255,238,255,234,255,227,255,227,255,228,255,231,255,
        228,255,221,255,216,255,216,255,205,255,196,255,191,255,192,255,192,255,203,255,214,255,235,255,245,255,3,0,17,0,14,0,14,0,253,255,253,255,252,255,244,255,250,255,7,0,17,0,26,0,24,0,26,0,18,0,14,0,8,0,4,0,0,0,255,255,3,0,0,0,1,0,0,0,255,255,1,0,1,0,0,
        0,7,0,8,0,11,0,14,0,17,0,11,0,3,0,7,0,1,0,1,0,11,0,17,0,37,0,51,0,60,0,68,0,78,0,87,0,116,0,137,0,150,0,163,0,173,0,179,0,186,0,197,0,195,0,199,0,209,0,220,0,219,0,229,0,222,0,231,0,231,0,241,0,242,0,247,0,0,1,3,1,253,0,4,1,13,1,13,1,13,1,3,1,4,1,0,1,
        235,0,216,0,202,0,197,0,180,0,172,0,162,0,147,0,133,0,114,0,105,0,89,0,80,0,78,0,72,0,84,0,101,0,114,0,118,0,122,0,130,0,125,0,118,0,107,0,93,0,71,0,57,0,40,0,21,0,252,255,220,255,198,255,181,255,173,255,159,255,163,255,156,255,148,255,140,255,129,255,
        120,255,109,255,105,255,104,255,102,255,104,255,112,255,109,255,112,255,111,255,101,255,90,255,68,255,47,255,29,255,8,255,254,254,252,254,239,254,235,254,227,254,223,254,216,254,213,254,216,254,210,254,196,254,184,254,167,254,149,254,137,254,130,254,
        128,254,138,254,135,254,135,254,139,254,149,254,160,254,178,254,195,254,209,254,220,254,232,254,241,254,253,254,1,255,14,255,39,255,59,255,94,255,120,255,138,255,166,255,183,255,194,255,199,255,202,255,201,255,212,255,216,255,221,255,245,255,7,0,26,0,
        55,0,97,0,134,0,157,0,172,0,172,0,161,0,145,0,123,0,108,0,105,0,111,0,129,0,152,0,172,0,206,0,223,0,230,0,224,0,223,0,209,0,197,0,194,0,188,0,197,0,191,0,197,0,195,0,201,0,184,0,188,0,184,0,179,0,166,0,150,0,137,0,129,0,108,0,91,0,78,0,66,0,57,0,46,0,
        33,0,22,0,7,0,249,255,237,255,216,255,202,255,192,255,192,255,192,255,201,255,212,255,210,255,209,255,209,255,206,255,203,255,201,255,205,255,210,255,213,255,223,255,227,255,230,255,221,255,213,255,208,255,196,255,199,255,212,255,208,255,217,255,216,
        255,219,255,220,255,209,255,208,255,212,255,206,255,205,255,219,255,235,255,253,255,11,0,28,0,35,0,44,0,46,0,54,0,61,0,60,0,60,0,53,0,40,0,39,0,32,0,26,0,18,0,17,0,11,0,3,0,253,255,239,255,237,255,231,255,231,255,220,255,216,255,209,255,208,255,206,255,
        214,255,216,255,216,255,223,255,221,255,230,255,232,255,234,255,244,255,250,255,3,0,252,255,255,255,246,255,248,255,255,255,0,0,4,0,11,0,17,0,19,0,28,0,22,0,21,0,22,0,24,0,14,0,19,0,22,0,24,0,25,0,17,0,28,0,21,0,28,0,36,0,33,0,32,0,35,0,32,0,32,0,30,
        0,29,0,32,0,42,0,53,0,53,0,57,0,69,0,75,0,86,0,93,0,109,0,123,0,139,0,158,0,169,0,175,0,193,0,198,0,201,0,204,0,201,0,206,0,201,0,206,0,202,0,217,0,223,0,222,0,217,0,216,0,220,0,217,0,222,0,222,0,216,0,220,0,229,0,226,0,231,0,219,0,206,0,201,0,170,0,
        152,0,133,0,107,0,91,0,80,0,64,0,58,0,53,0,46,0,53,0,57,0,61,0,60,0,60,0,71,0,71,0,69,0,62,0,60,0,54,0,50,0,43,0,28,0,18,0,10,0,255,255,239,255,216,255,202,255,183,255,176,255,176,255,166,255,162,255,155,255,147,255,144,255,136,255,120,255,99,255,83,
        255,66,255,63,255,62,255,58,255,55,255,54,255,58,255,57,255,55,255,39,255,32,255,26,255,23,255,5,255,7,255,253,254,14,255,8,255,9,255,4,255,253,254,239,254,223,254,205,254,191,254,180,254,166,254,169,254,169,254,166,254,167,254,173,254,174,254,170,254,
        175,254,177,254,184,254,199,254,210,254,223,254,239,254,4,255,15,255,40,255,62,255,77,255,91,255,101,255,115,255,126,255,140,255,159,255,176,255,190,255,210,255,223,255,235,255,244,255,0,0,11,0,26,0,46,0,60,0,84,0,102,0,125,0,141,0,162,0,159,0,155,0,
        141,0,133,0,143,0,151,0,166,0,190,0,205,0,219,0,224,0,226,0,216,0,206,0,199,0,191,0,186,0,180,0,172,0,172,0,169,0,180,0,179,0,177,0,180,0,175,0,165,0,154,0,144,0,132,0,111,0,96,0,86,0,80,0,79,0,72,0,57,0,40,0,19,0,253,255,244,255,224,255,220,255,217,
        255,203,255,212,255,220,255,217,255,223,255,227,255,232,255,232,255,228,255,230,255,224,255,234,255,230,255,234,255,237,255,244,255,252,255,3,0,12,0,14,0,12,0,6,0,255,255,0,0,246,255,245,255,248,255,239,255,235,255,245,255,248,255,0,0,6,0,11,0,6,0,14,
        0,6,0,17,0,28,0,29,0,29,0,36,0,32,0,26,0,26,0,17,0,18,0,21,0,14,0,15,0,12,0,3,0,7,0,253,255,237,255,223,255,210,255,213,255,213,255,208,255,210,255,199,255,194,255,194,255,194,255,190,255,194,255,201,255,210,255,217,255,230,255,230,255,231,255,234,255,
        235,255,235,255,232,255,235,255,238,255,245,255,255,255,8,0,18,0,25,0,21,0,19,0,19,0,28,0,33,0,47,0,47,0,55,0,66,0,62,0,60,0,55,0,54,0,53,0,50,0,51,0,51,0,54,0,55,0,61,0,54,0,42,0,30,0,18,0,28,0,26,0,43,0,55,0,73,0,83,0,94,0,108,0,116,0,137,0,144,0,161,
        0,175,0,186,0,198,0,197,0,194,0,197,0,193,0,197,0,197,0,205,0,201,0,205,0,204,0,199,0,193,0,186,0,198,0,204,0,193,0,193,0,187,0,187,0,180,0,173,0,170,0,168,0,155,0,140,0,112,0,91,0,60,0,39,0,28,0,32,0,33,0,43,0,47,0,47,0,44,0,32,0,29,0,12,0,10,0,255,
        255,0,0,1,0,245,255,246,255,241,255,244,255,244,255,237,255,227,255,227,255,216,255,208,255,199,255,185,255,173,255,165,255,160,255,163,255,156,255,154,255,142,255,129,255,106,255,83,255,62,255,41,255,36,255,27,255,30,255,32,255,41,255,41,255,50,255,
        50,255,62,255,52,255,44,255,51,255,40,255,41,255,36,255,34,255,27,255,18,255,12,255,4,255,253,254,250,254,243,254,234,254,223,254,210,254,206,254,210,254,216,254,217,254,207,254,203,254,193,254,198,254,195,254,200,254,223,254,231,254,250,254,15,255,33,
        255,40,255,52,255,65,255,73,255,73,255,81,255,98,255,116,255,144,255,170,255,184,255,209,255,228,255,245,255,252,255,14,0,19,0,29,0,30,0,42,0,57,0,73,0,101,0,122,0,147,0,175,0,193,0,208,0,209,0,205,0,194,0,191,0,194,0,201,0,205,0,217,0,220,0,227,0,224,
        0,213,0,202,0,191,0,188,0,190,0,191,0,195,0,194,0,186,0,180,0,181,0,177,0,176,0,162,0,166,0,151,0,143,0,127,0,122,0,109,0,97,0,82,0,66,0,58,0,47,0,25,0,15,0,6,0,0,0,252,255,242,255,228,255,216,255,212,255,219,255,228,255,230,255,238,255,244,255,0,0,248,
        255,241,255,242,255,238,255,248,255,250,255,3,0,17,0,25,0,21,0,30,0,29,0,35,0,32,0,35,0,29,0,21,0,14,0,10,0,11,0,15,0,25,0,21,0,24,0,26,0,22,0,24,0,18,0,14,0,8,0,8,0,255,255,3,0,7,0,15,0,21,0,18,0,25,0,35,0,36,0,35,0,28,0,19,0,10,0,249,255,238,255,221,
        255,219,255,214,255,209,255,195,255,190,255,178,255,169,255,169,255,167,255,167,255,184,255,201,255,216,255,227,255,239,255,235,255,230,255,220,255,217,255,217,255,223,255,242,255,250,255,0,0,14,0,22,0,26,0,25,0,26,0,25,0,24,0,28,0,37,0,50,0,65,0,69,
        0,90,0,93,0,98,0,98,0,89,0,80,0,78,0,75,0,69,0,72,0,65,0,61,0,48,0,37,0,30,0,25,0,28,0,26,0,36,0,47,0,57,0,71,0,79,0,91,0,107,0,122,0,140,0,143,0,145,0,152,0,155,0,158,0,172,0,181,0,172,0,176,0,170,0,175,0,169,0,180,0,176,0,175,0,179,0,168,0,173,0,175,
        0,168,0,158,0,150,0,145,0,133,0,119,0,102,0,93,0,76,0,72,0,57,0,54,0,53,0,39,0,36,0,32,0,22,0,15,0,11,0,10,0,1,0,241,255,227,255,209,255,191,255,183,255,174,255,169,255,176,255,183,255,185,255,183,255,185,255,195,255,198,255,190,255,190,255,187,255,178,
        255,177,255,173,255,176,255,177,255,158,255,144,255,119,255,94,255,75,255,59,255,55,255,51,255,48,255,50,255,32,255,26,255,18,255,26,255,33,255,47,255,55,255,59,255,59,255,73,255,69,255,59,255,50,255,45,255,41,255,37,255,33,255,27,255,21,255,23,255,23,
        255,19,255,22,255,12,255,9,255,8,255,0,255,247,254,236,254,234,254,223,254,220,254,214,254,218,254,227,254,229,254,247,254,0,255,12,255,23,255,25,255,26,255,40,255,48,255,61,255,70,255,91,255,116,255,142,255,167,255,191,255,208,255,224,255,238,255,252,
        255,14,0,22,0,36,0,46,0,60,0,79,0,96,0,104,0,123,0,141,0,173,0,186,0,201,0,213,0,230,0,237,0,231,0,229,0,215,0,202,0,194,0,187,0,193,0,202,0,215,0,215,0,212,0,201,0,201,0,204,0,201,0,191,0,188,0,186,0,190,0,183,0,175,0,169,0,166,0,159,0,148,0,139,0,140,
        0,120,0,105,0,86,0,76,0,62,0,57,0,46,0,44,0,35,0,28,0,12,0,6,0,252,255,252,255,241,255,239,255,244,255,244,255,248,255,0,0,10,0,12,0,25,0,37,0,42,0,46,0,48,0,37,0,30,0,30,0,35,0,35,0,40,0,44,0,47,0,58,0,72,0,71,0,72,0,65,0,58,0,51,0,42,0,39,0,37,0,29,
        0,30,0,35,0,28,0,15,0,17,0,12,0,12,0,14,0,17,0,24,0,29,0,33,0,37,0,36,0,28,0,24,0,24,0,12,0,4,0,253,255,239,255,226,255,219,255,202,255,198,255,198,255,199,255,194,255,190,255,187,255,190,255,190,255,191,255,187,255,198,255,195,255,201,255,209,255,217,
        255,227,255,237,255,239,255,244,255,250,255,255,255,0,0,10,0,17,0,29,0,39,0,47,0,43,0,54,0,57,0,62,0,78,0,82,0,94,0,100,0,104,0,98,0,102,0,97,0,96,0,84,0,82,0,76,0,68,0,64,0,60,0,54,0,57,0,50,0,51,0,46,0,42,0,40,0,37,0,47,0,51,0,65,0,79,0,83,0,101,0,
        111,0,125,0,139,0,148,0,148,0,152,0,155,0,157,0,161,0,162,0,165,0,169,0,173,0,176,0,175,0,172,0,165,0,161,0,152,0,148,0,139,0,134,0,126,0,114,0,94,0,76,0,61,0,54,0,39,0,39,0,46,0,39,0,40,0,33,0,29,0,22,0,4,0,241,255,216,255,203,255,188,255,173,255,172,
        255,159,255,165,255,159,255,145,255,145,255,145,255,142,255,134,255,136,255,144,255,151,255,165,255,163,255,162,255,158,255,154,255,156,255,144,255,136,255,123,255,119,255,108,255,95,255,86,255,79,255,81,255,73,255,63,255,61,255,57,255,54,255,48,255,
        51,255,54,255,44,255,43,255,47,255,44,255,48,255,57,255,62,255,68,255,63,255,59,255,54,255,54,255,55,255,57,255,58,255,62,255,54,255,61,255,65,255,58,255,50,255,36,255,23,255,9,255,8,255,3,255,1,255,7,255,7,255,3,255,0,255,243,254,242,254,239,254,1,255,
        14,255,26,255,43,255,57,255,70,255,84,255,81,255,101,255,112,255,133,255,147,255,165,255,177,255,194,255,201,255,224,255,234,255,6,0,30,0,51,0,72,0,93,0,109,0,123,0,125,0,141,0,151,0,175,0,186,0,205,0,219,0,234,0,235,0,230,0,220,0,213,0,209,0,202,0,204,
        0,201,0,209,0,216,0,216,0,213,0,204,0,206,0,194,0,183,0,184,0,179,0,186,0,181,0,173,0,165,0,155,0,147,0,130,0,125,0,129,0,122,0,120,0,119,0,105,0,107,0,89,0,78,0,66,0,51,0,46,0,40,0,35,0,33,0,25,0,17,0,15,0,14,0,11,0,4,0,15,0,25,0,22,0,21,0,28,0,26,0,
        35,0,33,0,39,0,36,0,35,0,29,0,32,0,37,0,40,0,53,0,65,0,78,0,79,0,79,0,83,0,86,0,72,0,68,0,62,0,46,0,36,0,40,0,37,0,42,0,37,0,42,0,62,0,55,0,57,0,60,0,55,0,55,0,46,0,43,0,33,0,28,0,19,0,17,0,253,255,245,255,244,255,234,255,220,255,208,255,208,255,201,
        255,201,255,205,255,205,255,216,255,216,255,212,255,203,255,191,255,187,255,178,255,177,255,183,255,196,255,208,255,227,255,237,255,252,255,255,255,253,255,252,255,4,0,11,0,15,0,26,0,28,0,47,0,61,0,69,0,80,0,83,0,80,0,75,0,79,0,78,0,68,0,75,0,75,0,80,
        0,86,0,83,0,89,0,86,0,84,0,84,0,78,0,71,0,69,0,61,0,58,0,54,0,46,0,50,0,51,0,48,0,50,0,58,0,61,0,72,0,75,0,87,0,94,0,100,0,114,0,119,0,122,0,126,0,136,0,137,0,152,0,151,0,158,0,154,0,155,0,158,0,157,0,155,0,151,0,147,0,139,0,122,0,97,0,91,0,68,0,65,0,
        53,0,43,0,32,0,30,0,26,0,30,0,30,0,22,0,30,0,17,0,253,255,237,255,214,255,194,255,172,255,165,255,155,255,149,255,145,255,144,255,148,255,138,255,133,255,124,255,120,255,123,255,126,255,117,255,122,255,115,255,117,255,117,255,117,255,116,255,115,255,
        115,255,117,255,120,255,126,255,133,255,127,255,123,255,115,255,104,255,95,255,81,255,91,255,90,255,80,255,81,255,75,255,70,255,63,255,61,255,55,255,47,255,43,255,52,255,54,255,65,255,66,255,81,255,81,255,80,255,75,255,81,255,83,255,75,255,79,255,73,
        255,68,255,55,255,47,255,39,255,26,255,25,255,15,255,19,255,12,255,22,255,23,255,21,255,9,255,3,255,252,254,252,254,250,254,1,255,14,255,23,255,36,255,43,255,55,255,69,255,88,255,104,255,116,255,126,255,130,255,155,255,177,255,198,255,221,255,238,255,
        7,0,29,0,50,0,69,0,97,0,114,0,126,0,139,0,140,0,145,0,161,0,170,0,179,0,195,0,201,0,212,0,219,0,222,0,217,0,213,0,206,0,211,0,211,0,206,0,205,0,215,0,223,0,220,0,223,0,205,0,198,0,187,0,184,0,175,0,169,0,162,0,159,0,154,0,143,0,134,0,129,0,127,0,126,
        0,122,0,130,0,122,0,119,0,104,0,94,0,80,0,75,0,61,0,51,0,51,0,42,0,37,0,37,0,35,0,30,0,25,0,30,0,24,0,33,0,30,0,25,0,25,0,28,0,29,0,36,0,37,0,44,0,58,0,71,0,73,0,75,0,78,0,75,0,71,0,76,0,71,0,76,0,82,0,72,0,76,0,64,0,51,0,48,0,43,0,42,0,39,0,50,0,58,
        0,64,0,73,0,79,0,80,0,71,0,55,0,50,0,46,0,28,0,24,0,19,0,12,0,7,0,253,255,245,255,231,255,223,255,223,255,216,255,219,255,216,255,223,255,224,255,221,255,213,255,202,255,195,255,187,255,178,255,183,255,184,255,181,255,185,255,191,255,202,255,212,255,
        226,255,232,255,252,255,1,0,8,0,11,0,18,0,25,0,29,0,46,0,53,0,60,0,58,0,54,0,54,0,54,0,57,0,51,0,60,0,60,0,60,0,68,0,61,0,62,0,73,0,76,0,76,0,82,0,83,0,75,0,71,0,60,0,55,0,46,0,47,0,47,0,51,0,54,0,60,0,47,0,51,0,53,0,55,0,58,0,55,0,68,0,79,0,89,0,97,
        0,104,0,120,0,132,0,134,0,141,0,139,0,140,0,139,0,148,0,148,0,140,0,136,0,125,0,116,0,96,0,91,0,78,0,66,0,51,0,46,0,40,0,32,0,26,0,14,0,10,0,6,0,0,0,245,255,232,255,219,255,205,255,184,255,170,255,165,255,156,255,158,255,154,255,141,255,142,255,138,255,
        129,255,129,255,130,255,124,255,117,255,101,255,91,255,97,255,91,255,95,255,98,255,99,255,108,255,115,255,116,255,124,255,129,255,129,255,129,255,138,255,140,255,140,255,136,255,140,255,141,255,142,255,126,255,119,255,117,255,109,255,109,255,94,255,83,
        255,81,255,75,255,72,255,65,255,69,255,66,255,68,255,70,255,76,255,76,255,87,255,97,255,94,255,91,255,80,255,77,255,77,255,70,255,73,255,66,255,63,255,63,255,50,255,40,255,30,255,25,255,19,255,26,255,32,255,18,255,21,255,14,255,8,255,9,255,14,255,21,
        255,32,255,47,255,57,255,73,255,90,255,106,255,116,255,123,255,136,255,147,255,163,255,183,255,202,255,234,255,6,0,18,0,33,0,54,0,65,0,87,0,97,0,116,0,134,0,144,0,154,0,152,0,161,0,169,0,175,0,191,0,212,0,217,0,227,0,219,0,222,0,212,0,204,0,213,0,209,
        0,213,0,212,0,212,0,211,0,208,0,204,0,193,0,181,0,173,0,166,0,154,0,148,0,141,0,140,0,133,0,125,0,129,0,123,0,132,0,118,0,127,0,120,0,120,0,118,0,108,0,105,0,94,0,84,0,71,0,61,0,60,0,50,0,61,0,47,0,46,0,40,0,40,0,32,0,40,0,39,0,30,0,32,0,28,0,29,0,35,
        0,48,0,61,0,61,0,66,0,79,0,83,0,83,0,78,0,75,0,60,0,62,0,58,0,61,0,64,0,66,0,68,0,68,0,66,0,68,0,72,0,73,0,73,0,76,0,79,0,83,0,83,0,82,0,72,0,66,0,58,0,57,0,48,0,35,0,32,0,15,0,18,0,10,0,248,255,253,255,245,255,239,255,235,255,230,255,231,255,228,255,
        226,255,216,255,206,255,198,255,187,255,181,255,184,255,187,255,190,255,194,255,198,255,206,255,216,255,227,255,241,255,245,255,253,255,8,0,19,0,29,0,33,0,33,0,29,0,26,0,17,0,17,0,22,0,15,0,22,0,36,0,33,0,35,0,44,0,50,0,61,0,64,0,75,0,78,0,83,0,79,0,
        79,0,76,0,80,0,79,0,79,0,82,0,73,0,68,0,65,0,55,0,57,0,44,0,48,0,43,0,48,0,51,0,57,0,53,0,51,0,48,0,55,0,72,0,84,0,96,0,107,0,115,0,119,0,119,0,130,0,130,0,133,0,136,0,136,0,127,0,123,0,104,0,93,0,83,0,73,0,62,0,48,0,47,0,33,0,26,0,14,0,255,255,250,255,
        246,255,226,255,206,255,180,255,180,255,170,255,170,255,170,255,162,255,159,255,155,255,147,255,148,255,136,255,124,255,116,255,111,255,104,255,97,255,91,255,97,255,95,255,97,255,97,255,91,255,87,255,88,255,97,255,98,255,101,255,112,255,116,255,116,255,
        120,255,126,255,129,255,148,255,160,255,173,255,176,255,178,255,172,255,170,255,162,255,152,255,136,255,120,255,111,255,116,255,111,255,106,255,111,255,108,255,97,255,94,255,88,255,84,255,87,255,86,255,87,255,93,255,97,255,98,255,87,255,88,255,86,255,
        87,255,88,255,91,255,91,255,80,255,80,255,70,255,55,255,36,255,37,255,48,255,40,255,44,255,41,255,40,255,41,255,44,255,36,255,40,255,52,255,63,255,73,255,90,255,98,255,106,255,123,255,129,255,149,255,160,255,178,255,198,255,221,255,237,255,6,0,21,0,25,
        0,43,0,54,0,75,0,98,0,107,0,115,0,125,0,126,0,143,0,155,0,163,0,175,0,191,0,209,0,213,0,206,0,211,0,213,0,205,0,208,0,209,0,215,0,212,0,215,0,215,0,213,0,198,0,188,0,177,0,163,0,150,0,150,0,139,0,144,0,145,0,144,0,141,0,145,0,139,0,139,0,136,0,133,0,
        123,0,126,0,111,0,104,0,105,0,98,0,91,0,84,0,75,0,62,0,55,0,48,0,51,0,48,0,42,0,44,0,40,0,37,0,42,0,35,0,28,0,29,0,32,0,39,0,39,0,51,0,72,0,69,0,79,0,86,0,82,0,84,0,87,0,83,0,72,0,66,0,55,0,58,0,61,0,62,0,62,0,84,0,83,0,80,0,82,0,72,0,65,0,65,0,69,0,
        71,0,69,0,71,0,68,0,64,0,66,0,66,0,66,0,65,0,57,0,44,0,28,0,17,0,8,0,8,0,7,0,252,255,248,255,239,255,232,255,221,255,213,255,205,255,198,255,202,255,202,255,201,255,202,255,208,255,209,255,203,255,206,255,210,255,223,255,234,255,237,255,242,255,245,255,
        252,255,6,0,253,255,1,0,12,0,1,0,8,0,4,0,4,0,3,0,11,0,17,0,21,0,26,0,30,0,35,0,48,0,61,0,64,0,58,0,58,0,54,0,54,0,57,0,58,0,54,0,60,0,65,0,62,0,65,0,64,0,65,0,55,0,55,0,47,0,43,0,37,0,35,0,26,0,28,0,29,0,26,0,26,0,37,0,46,0,53,0,69,0,82,0,86,0,93,0,100,
        0,111,0,118,0,109,0,105,0,101,0,96,0,84,0,72,0,72,0,62,0,51,0,36,0,26,0,21,0,11,0,6,0,248,255,232,255,220,255,203,255,183,255,177,255,160,255,154,255,148,255,149,255,151,255,154,255,141,255,133,255,123,255,115,255,120,255,113,255,112,255,112,255,104,
        255,101,255,95,255,94,255,88,255,90,255,90,255,86,255,90,255,88,255,95,255,102,255,104,255,108,255,115,255,111,255,122,255,131,255,149,255,170,255,181,255,187,255,192,255,196,255,195,255,202,255,194,255,192,255,180,255,169,255,162,255,155,255,147,255,
        142,255,134,255,133,255,123,255,120,255,109,255,98,255,91,255,93,255,93,255,80,255,80,255,84,255,93,255,98,255,102,255,111,255,116,255,112,255,109,255,97,255,87,255,70,255,58,255,47,255,37,255,34,255,39,255,50,255,63,255,66,255,70,255,76,255,79,255,87,
        255,86,255,95,255,102,255,111,255,127,255,144,255,156,255,167,255,176,255,195,255,216,255,227,255,244,255,250,255,4,0,14,0,22,0,35,0,42,0,48,0,65,0,75,0,79,0,94,0,111,0,119,0,144,0,144,0,163,0,175,0,180,0,186,0,194,0,195,0,197,0,198,0,202,0,208,0,206,
        0,209,0,206,0,204,0,199,0,190,0,180,0,161,0,150,0,139,0,139,0,143,0,145,0,150,0,147,0,150,0,148,0,147,0,136,0,114,0,112,0,119,0,112,0,107,0,105,0,100,0,91,0,84,0,72,0,69,0,60,0,68,0,62,0,58,0,58,0,54,0,43,0,43,0,43,0,43,0,36,0,39,0,36,0,42,0,48,0,55,
        0,53,0,65,0,62,0,72,0,78,0,76,0,71,0,73,0,80,0,72,0,66,0,64,0,65,0,64,0,65,0,65,0,61,0,62,0,73,0,69,0,71,0,80,0,86,0,84,0,91,0,93,0,97,0,96,0,93,0,83,0,86,0,76,0,66,0,64,0,57,0,42,0,36,0,25,0,25,0,15,0,12,0,4,0,245,255,242,255,235,255,230,255,228,255,
        226,255,226,255,230,255,227,255,234,255,227,255,220,255,231,255,235,255,230,255,231,255,238,255,235,255,238,255,241,255,238,255,242,255,248,255,241,255,242,255,237,255,237,255,244,255,250,255,253,255,1,0,14,0,22,0,25,0,30,0,37,0,47,0,55,0,39,0,43,0,47,
        0,47,0,54,0,64,0,58,0,61,0,65,0,64,0,53,0,57,0,54,0,57,0,55,0,42,0,37,0,43,0,32,0,22,0,12,0,10,0,17,0,26,0,40,0,42,0,48,0,46,0,61,0,61,0,65,0,76,0,75,0,82,0,75,0,73,0,79,0,71,0,61,0,58,0,47,0,44,0,40,0,32,0,25,0,14,0,6,0,245,255,226,255,214,255,203,255,
        190,255,174,255,165,255,148,255,142,255,137,255,137,255,126,255,123,255,120,255,113,255,104,255,102,255,95,255,98,255,95,255,97,255,99,255,109,255,102,255,104,255,95,255,97,255,94,255,95,255,94,255,97,255,98,255,104,255,106,255,115,255,119,255,119,255,
        133,255,144,255,149,255,165,255,174,255,191,255,198,255,209,255,208,255,205,255,205,255,203,255,195,255,194,255,194,255,190,255,178,255,167,255,156,255,154,255,136,255,130,255,123,255,120,255,117,255,105,255,98,255,91,255,88,255,87,255,90,255,99,255,
        105,255,113,255,122,255,126,255,120,255,122,255,115,255,105,255,95,255,86,255,77,255,72,255,72,255,61,255,68,255,68,255,80,255,87,255,104,255,98,255,109,255,109,255,115,255,127,255,134,255,149,255,169,255,183,255,199,255,214,255,223,255,227,255,239,255,
        239,255,253,255,7,0,11,0,17,0,29,0,35,0,40,0,43,0,55,0,68,0,72,0,82,0,93,0,115,0,127,0,136,0,150,0,161,0,172,0,181,0,184,0,193,0,187,0,184,0,184,0,176,0,181,0,183,0,183,0,181,0,173,0,170,0,159,0,147,0,130,0,130,0,133,0,136,0,134,0,141,0,134,0,130,0,129,
        0,120,0,107,0,107,0,100,0,100,0,96,0,94,0,80,0,80,0,76,0,69,0,65,0,62,0,65,0,71,0,72,0,68,0,69,0,66,0,66,0,53,0,47,0,44,0,55,0,54,0,53,0,51,0,54,0,51,0,53,0,64,0,71,0,64,0,57,0,64,0,69,0,76,0,80,0,73,0,69,0,64,0,62,0,53,0,53,0,50,0,58,0,66,0,66,0,80,
        0,89,0,100,0,100,0,109,0,116,0,118,0,114,0,107,0,108,0,96,0,101,0,78,0,78,0,66,0,71,0,69,0,55,0,46,0,39,0,22,0,10,0,253,255,245,255,237,255,235,255,237,255,241,255,244,255,242,255,249,255,242,255,238,255,237,255,234,255,237,255,238,255,224,255,228,255,
        230,255,232,255,231,255,231,255,230,255,230,255,242,255,237,255,241,255,239,255,245,255,255,255,1,0,3,0,14,0,22,0,25,0,29,0,28,0,35,0,33,0,40,0,37,0,36,0,29,0,29,0,26,0,37,0,43,0,46,0,53,0,61,0,60,0,64,0,55,0,48,0,44,0,39,0,29,0,24,0,29,0,29,0,33,0,26,
        0,36,0,36,0,37,0,48,0,50,0,50,0,50,0,51,0,54,0,51,0,53,0,57,0,51,0,48,0,43,0,42,0,35,0,26,0,26,0,24,0,18,0,8,0,0,0,238,255,227,255,217,255,199,255,192,255,184,255,173,255,169,255,155,255,141,255,130,255,120,255,119,255,115,255,117,255,115,255,116,255,
        112,255,113,255,106,255,109,255,109,255,109,255,119,255,123,255,129,255,131,255,130,255,126,255,124,255,108,255,111,255,119,255,120,255,123,255,133,255,144,255,147,255,149,255,156,255,163,255,178,255,180,255,185,255,191,255,201,255,209,255,217,255,217,
        255,221,255,221,255,223,255,219,255,212,255,202,255,188,255,183,255,169,255,154,255,140,255,129,255,130,255,123,255,123,255,122,255,117,255,115,255,111,255,117,255,117,255,116,255,111,255,120,255,119,255,126,255,117,255,113,255,113,255,116,255,106,255,
        106,255,94,255,93,255,98,255,94,255,102,255,102,255,120,255,117,255,117,255,131,255,137,255,142,255,151,255,159,255,173,255,185,255,194,255,199,255,217,255,226,255,227,255,242,255,252,255,3,0,12,0,8,0,18,0,22,0,32,0,37,0,43,0,53,0,55,0,68,0,72,0,82,0,
        93,0,100,0,109,0,120,0,141,0,144,0,169,0,168,0,168,0,165,0,168,0,159,0,154,0,148,0,158,0,162,0,170,0,161,0,165,0,158,0,148,0,141,0,134,0,132,0,125,0,127,0,115,0,116,0,120,0,108,0,94,0,86,0,89,0,80,0,76,0,71,0,64,0,64,0,60,0,62,0,65,0,58,0,58,0,65,0,65,
        0,66,0,75,0,69,0,72,0,79,0,73,0,66,0,61,0,60,0,54,0,62,0,61,0,65,0,54,0,55,0,50,0,53,0,50,0,44,0,42,0,43,0,48,0,51,0,50,0,54,0,53,0,57,0,58,0,50,0,46,0,51,0,51,0,55,0,68,0,80,0,89,0,100,0,107,0,118,0,127,0,120,0,108,0,105,0,97,0,80,0,71,0,64,0,66,0,76,
        0,69,0,75,0,69,0,60,0,46,0,30,0,19,0,1,0,248,255,244,255,242,255,241,255,249,255,246,255,246,255,248,255,238,255,235,255,227,255,220,255,214,255,213,255,224,255,224,255,228,255,235,255,228,255,234,255,228,255,234,255,234,255,232,255,246,255,250,255,249,
        255,255,255,0,0,0,0,4,0,3,0,0,0,0,0,7,0,10,0,8,0,17,0,19,0,24,0,22,0,29,0,28,0,32,0,32,0,43,0,48,0,48,0,43,0,40,0,36,0,33,0,29,0,37,0,36,0,33,0,36,0,32,0,35,0,32,0,33,0,29,0,26,0,28,0,25,0,36,0,46,0,42,0,33,0,28,0,30,0,29,0,25,0,25,0,14,0,18,0,17,0,6,
        0,1,0,253,255,245,255,239,255,230,255,224,255,216,255,203,255,201,255,190,255,181,255,162,255,147,255,129,255,126,255,126,255,124,255,119,255,120,255,127,255,119,255,119,255,126,255,124,255,124,255,129,255,131,255,137,255,138,255,138,255,142,255,145,
        255,138,255,138,255,136,255,140,255,145,255,144,255,147,255,149,255,152,255,159,255,165,255,166,255,174,255,180,255,174,255,184,255,190,255,196,255,208,255,216,255,221,255,223,255,231,255,227,255,227,255,216,255,212,255,203,255,191,255,184,255,172,255,
        169,255,159,255,145,255,138,255,137,255,133,255,130,255,127,255,122,255,123,255,123,255,123,255,116,255,123,255,113,255,119,255,120,255,124,255,117,255,131,255,129,255,123,255,127,255,130,255,129,255,137,255,136,255,144,255,141,255,145,255,142,255,145,
        255,151,255,163,255,178,255,191,255,198,255,214,255,216,255,220,255,223,255,230,255,232,255,244,255,1,0,11,0,18,0,28,0,22,0,29,0,29,0,35,0,39,0,44,0,54,0,62,0,69,0,83,0,86,0,91,0,97,0,104,0,109,0,132,0,133,0,143,0,140,0,141,0,141,0,137,0,145,0,143,0,
        141,0,144,0,141,0,139,0,148,0,144,0,140,0,126,0,126,0,130,0,119,0,119,0,114,0,105,0,100,0,96,0,90,0,83,0,80,0,83,0,76,0,71,0,62,0,58,0,50,0,46,0,44,0,48,0,48,0,55,0,58,0,64,0,75,0,78,0,78,0,76,0,76,0,73,0,66,0,65,0,61,0,58,0,55,0,60,0,68,0,69,0,64,0,
        55,0,47,0,44,0,42,0,35,0,36,0,33,0,37,0,44,0,50,0,51,0,53,0,55,0,46,0,60,0,61,0,72,0,71,0,73,0,82,0,97,0,104,0,105,0,109,0,112,0,115,0,111,0,115,0,105,0,101,0,86,0,73,0,68,0,73,0,68,0,58,0,55,0,53,0,39,0,36,0,24,0,17,0,12,0,7,0,255,255,239,255,231,255,
        231,255,228,255,224,255,220,255,220,255,220,255,217,255,219,255,228,255,224,255,224,255,230,255,232,255,235,255,237,255,238,255,244,255,239,255,241,255,230,255,244,255,244,255,241,255,239,255,242,255,242,255,245,255,246,255,6,0,0,0,12,0,7,0,8,0,3,0,11,
        0,8,0,17,0,18,0,19,0,24,0,26,0,22,0,24,0,26,0,25,0,26,0,29,0,42,0,40,0,42,0,37,0,44,0,44,0,33,0,37,0,28,0,26,0,22,0,12,0,19,0,18,0,22,0,18,0,14,0,12,0,14,0,19,0,17,0,12,0,10,0,4,0,3,0,0,0,253,255,237,255,231,255,227,255,216,255,206,255,198,255,185,255,
        177,255,166,255,158,255,149,255,151,255,138,255,140,255,140,255,144,255,136,255,136,255,134,255,134,255,137,255,137,255,137,255,142,255,154,255,149,255,152,255,149,255,155,255,154,255,156,255,155,255,155,255,156,255,151,255,152,255,158,255,158,255,162,
        255,169,255,172,255,176,255,170,255,169,255,174,255,178,255,183,255,184,255,190,255,196,255,209,255,216,255,217,255,209,255,213,255,214,255,214,255,214,255,199,255,201,255,201,255,191,255,180,255,173,255,160,255,155,255,145,255,136,255,133,255,134,255,
        133,255,130,255,130,255,133,255,133,255,129,255,126,255,127,255,127,255,131,255,127,255,131,255,136,255,141,255,149,255,155,255,166,255,173,255,170,255,178,255,170,255,176,255,184,255,184,255,185,255,198,255,199,255,206,255,220,255,231,255,232,255,241,
        255,237,255,245,255,249,255,253,255,3,0,10,0,19,0,29,0,35,0,32,0,30,0,42,0,44,0,54,0,54,0,55,0,65,0,71,0,82,0,87,0,98,0,107,0,108,0,116,0,130,0,130,0,130,0,129,0,134,0,127,0,127,0,127,0,118,0,127,0,126,0,126,0,126,0,123,0,118,0,118,0,115,0,105,0,107,
        0,100,0,86,0,86,0,89,0,82,0,79,0,75,0,73,0,75,0,76,0,61,0,60,0,43,0,39,0,35,0,42,0,50,0,51,0,57,0,66,0,71,0,73,0,68,0,71,0,73,0,66,0,65,0,53,0,61,0,61,0,60,0,54,0,57,0,57,0,51,0,50,0,40,0,42,0,39,0,35,0,35,0,33,0,29,0,32,0,42,0,40,0,48,0,46,0,55,0,57,
        0,69,0,76,0,80,0,79,0,86,0,93,0,101,0,105,0,101,0,108,0,115,0,111,0,108,0,112,0,112,0,111,0,104,0,104,0,86,0,75,0,71,0,64,0,61,0,54,0,40,0,35,0,18,0,12,0,3,0,252,255,238,255,237,255,228,255,219,255,212,255,199,255,202,255,203,255,209,255,216,255,217,
        255,220,255,221,255,224,255,228,255,226,255,231,255,239,255,242,255,239,255,242,255,242,255,246,255,230,255,234,255,234,255,228,255,228,255,231,255,237,255,246,255,1,0,3,0,6,0,11,0,3,0,3,0,250,255,252,255,253,255,1,0,3,0,6,0,11,0,12,0,14,0,28,0,24,0,
        35,0,32,0,24,0,15,0,21,0,15,0,10,0,12,0,18,0,22,0,18,0,19,0,19,0,10,0,7,0,10,0,15,0,8,0,1,0,12,0,4,0,0,0,252,255,255,255,244,255,246,255,238,255,234,255,227,255,221,255,213,255,208,255,198,255,191,255,188,255,181,255,174,255,177,255,167,255,163,255,162,
        255,159,255,162,255,158,255,156,255,145,255,145,255,144,255,142,255,144,255,145,255,141,255,151,255,160,255,170,255,170,255,167,255,167,255,173,255,181,255,187,255,183,255,187,255,178,255,173,255,169,255,169,255,170,255,176,255,183,255,181,255,190,255,
        188,255,196,255,203,255,209,255,205,255,205,255,209,255,209,255,214,255,214,255,206,255,210,255,206,255,203,255,209,255,206,255,196,255,199,255,199,255,198,255,183,255,174,255,165,255,156,255,152,255,148,255,144,255,149,255,148,255,152,255,147,255,142,
        255,133,255,138,255,138,255,136,255,142,255,144,255,140,255,148,255,142,255,154,255,160,255,170,255,177,255,176,255,183,255,183,255,190,255,194,255,198,255,212,255,217,255,223,255,224,255,238,255,238,255,242,255,248,255,250,255,248,255,253,255,6,0,12,
        0,17,0,21,0,21,0,32,0,37,0,43,0,48,0,53,0,58,0,62,0,68,0,75,0,84,0,94,0,100,0,105,0,115,0,112,0,112,0,119,0,120,0,122,0,125,0,120,0,129,0,127,0,123,0,120,0,109,0,114,0,109,0,112,0,109,0,111,0,107,0,102,0,90,0,91,0,83,0,82,0,75,0,69,0,68,0,72,0,75,0,75,
        0,80,0,76,0,76,0,64,0,61,0,50,0,55,0,54,0,61,0,61,0,69,0,66,0,66,0,55,0,65,0,65,0,55,0,57,0,55,0,50,0,43,0,43,0,50,0,51,0,47,0,48,0,51,0,54,0,53,0,53,0,54,0,51,0,48,0,43,0,46,0,39,0,37,0,37,0,50,0,54,0,65,0,66,0,72,0,69,0,71,0,79,0,86,0,82,0,89,0,84,
        0,94,0,94,0,94,0,93,0,94,0,93,0,96,0,93,0,89,0,91,0,90,0,78,0,69,0,61,0,50,0,37,0,24,0,11,0,6,0,249,255,246,255,230,255,231,255,226,255,212,255,212,255,208,255,198,255,199,255,201,255,203,255,205,255,210,255,216,255,217,255,220,255,216,255,217,255,226,
        255,216,255,216,255,232,255,228,255,227,255,231,255,232,255,234,255,230,255,235,255,232,255,230,255,237,255,238,255,244,255,245,255,244,255,248,255,246,255,249,255,1,0,4,0,14,0,6,0,7,0,8,0,8,0,10,0,17,0,21,0,17,0,21,0,11,0,6,0,7,0,3,0,8,0,4,0,7,0,17,
        0,19,0,19,0,19,0,15,0,19,0,10,0,6,0,7,0,7,0,6,0,4,0,250,255,241,255,244,255,227,255,224,255,219,255,217,255,210,255,210,255,210,255,206,255,202,255,209,255,210,255,209,255,205,255,199,255,203,255,195,255,192,255,184,255,176,255,170,255,172,255,166,255,
        170,255,163,255,158,255,162,255,169,255,172,255,183,255,177,255,180,255,185,255,185,255,185,255,190,255,184,255,185,255,194,255,191,255,184,255,181,255,181,255,174,255,177,255,181,255,185,255,194,255,188,255,201,255,210,255,209,255,210,255,205,255,209,
        255,201,255,206,255,212,255,210,255,210,255,209,255,206,255,205,255,196,255,196,255,195,255,201,255,196,255,192,255,196,255,178,255,176,255,177,255,170,255,167,255,166,255,166,255,163,255,165,255,167,255,162,255,159,255,155,255,148,255,142,255,147,255,
        147,255,151,255,147,255,158,255,165,255,173,255,173,255,178,255,184,255,190,255,195,255,199,255,201,255,210,255,217,255,220,255,226,255,228,255,235,255,238,255,241,255,249,255,0,0,7,0,8,0,12,0,17,0,18,0,28,0,25,0,24,0,28,0,29,0,35,0,47,0,58,0,58,0,65,
        0,76,0,84,0,84,0,91,0,94,0,91,0,96,0,102,0,107,0,107,0,111,0,112,0,122,0,120,0,123,0,126,0,118,0,108,0,101,0,89,0,97,0,91,0,90,0,91,0,97,0,91,0,82,0,73,0,64,0,61,0,65,0,57,0,62,0,64,0,66,0,79,0,80,0,80,0,78,0,75,0,75,0,71,0,64,0,64,0,60,0,62,0,62,0,58,
        0,57,0,60,0,60,0,54,0,50,0,46,0,47,0,47,0,37,0,39,0,39,0,39,0,43,0,50,0,47,0,46,0,48,0,54,0,51,0,43,0,40,0,30,0,37,0,42,0,48,0,53,0,61,0,55,0,62,0,66,0,75,0,78,0,80,0,82,0,93,0,96,0,93,0,94,0,93,0,96,0,93,0,89,0,87,0,87,0,87,0,79,0,78,0,80,0,68,0,60,
        0,50,0,36,0,26,0,12,0,12,0,3,0,249,255,242,255,239,255,239,255,228,255,226,255,219,255,210,255,208,255,206,255,196,255,194,255,196,255,201,255,208,255,205,255,208,255,205,255,205,255,209,255,209,255,208,255,214,255,221,255,226,255,227,255,234,255,237,
        255,241,255,246,255,246,255,242,255,249,255,248,255,252,255,1,0,255,255,0,0,253,255,6,0,3,0,7,0,6,0,0,0,10,0,12,0,17,0,11,0,8,0,4,0,3,0,255,255,252,255,6,0,4,0,7,0,4,0,0,0,6,0,8,0,11,0,14,0,21,0,19,0,15,0,14,0,14,0,10,0,1,0,249,255,245,255,234,255,230,
        255,228,255,232,255,220,255,223,255,216,255,217,255,208,255,206,255,209,255,206,255,205,255,205,255,205,255,201,255,199,255,201,255,198,255,201,255,198,255,194,255,190,255,184,255,191,255,191,255,187,255,187,255,187,255,188,255,194,255,192,255,192,255,
        192,255,187,255,192,255,191,255,191,255,194,255,192,255,185,255,188,255,191,255,194,255,191,255,187,255,187,255,192,255,191,255,195,255,199,255,202,255,212,255,219,255,214,255,216,255,206,255,206,255,210,255,212,255,209,255,205,255,206,255,203,255,205,
        255,198,255,196,255,201,255,198,255,191,255,192,255,191,255,195,255,191,255,187,255,181,255,183,255,184,255,187,255,188,255,190,255,184,255,183,255,173,255,178,255,170,255,167,255,159,255,165,255,165,255,167,255,174,255,181,255,188,255,184,255,198,255,
        196,255,195,255,196,255,203,255,205,255,214,255,220,255,223,255,234,255,237,255,235,255,244,255,250,255,255,255,4,0,18,0,12,0,11,0,15,0,25,0,37,0,35,0,39,0,48,0,43,0,43,0,44,0,53,0,57,0,62,0,61,0,68,0,71,0,72,0,76,0,79,0,87,0,90,0,96,0,102,0,97,0,98,
        0,101,0,115,0,120,0,125,0,119,0,107,0,105,0,105,0,90,0,87,0,93,0,90,0,94,0,97,0,93,0,80,0,80,0,73,0,65,0,60,0,64,0,61,0,66,0,73,0,76,0,78,0,84,0,82,0,80,0,75,0,73,0,71,0,65,0,65,0,62,0,51,0,48,0,48,0,46,0,44,0,40,0,47,0,44,0,44,0,43,0,50,0,44,0,40,0,
        39,0,42,0,42,0,46,0,40,0,40,0,30,0,37,0,36,0,37,0,39,0,48,0,47,0,51,0,55,0,58,0,65,0,66,0,73,0,80,0,84,0,91,0,84,0,94,0,93,0,83,0,83,0,82,0,82,0,75,0,71,0,66,0,60,0,60,0,62,0,50,0,36,0,28,0,18,0,11,0,4,0,1,0,0,0,0,0,253,255,246,255,237,255,228,255,224,
        255,220,255,214,255,206,255,198,255,201,255,199,255,201,255,194,255,194,255,196,255,195,255,202,255,196,255,199,255,201,255,208,255,206,255,214,255,221,255,224,255,220,255,221,255,228,255,227,255,234,255,239,255,252,255,249,255,1,0,1,0,252,255,253,255,
        249,255,255,255,3,0,253,255,0,0,1,0,7,0,4,0,12,0,7,0,7,0,7,0,7,0,3,0,244,255,253,255,248,255,249,255,245,255,0,0,0,0,3,0,11,0,12,0,12,0,15,0,6,0,3,0,6,0,253,255,249,255,245,255,245,255,244,255,244,255,241,255,234,255,230,255,226,255,216,255,217,255,208,
        255,208,255,206,255,216,255,219,255,210,255,216,255,205,255,210,255,213,255,213,255,208,255,206,255,202,255,203,255,196,255,187,255,185,255,190,255,181,255,181,255,192,255,187,255,190,255,195,255,199,255,203,255,202,255,190,255,199,255,203,255,198,255,
        194,255,187,255,188,255,187,255,191,255,187,255,188,255,180,255,191,255,184,255,190,255,187,255,195,255,198,255,202,255,202,255,209,255,208,255,210,255,213,255,212,255,212,255,214,255,216,255,206,255,213,255,210,255,208,255,205,255,202,255,198,255,203,
        255,199,255,195,255,194,255,191,255,192,255,190,255,183,255,191,255,187,255,187,255,180,255,177,255,173,255,167,255,169,255,166,255,173,255,178,255,180,255,181,255,188,255,194,255,198,255,196,255,203,255,203,255,199,255,199,255,206,255,213,255,213,255,
        219,255,230,255,231,255,242,255,242,255,242,255,250,255,252,255,7,0,15,0,19,0,30,0,30,0,26,0,30,0,30,0,30,0,42,0,47,0,55,0,54,0,51,0,61,0,55,0,64,0,62,0,62,0,65,0,76,0,75,0,79,0,82,0,82,0,82,0,86,0,84,0,83,0,89,0,97,0,90,0,101,0,100,0,97,0,101,0,102,
        0,97,0,97,0,94,0,84,0,90,0,76,0,72,0,80,0,72,0,76,0,64,0,61,0,60,0,61,0,58,0,75,0,75,0,75,0,79,0,69,0,71,0,66,0,57,0,53,0,48,0,50,0,51,0,46,0,42,0,46,0,53,0,47,0,47,0,42,0,37,0,36,0,32,0,32,0,33,0,35,0,36,0,39,0,29,0,28,0,30,0,32,0,28,0,29,0,32,0,33,
        0,35,0,44,0,51,0,55,0,64,0,68,0,69,0,80,0,83,0,86,0,82,0,79,0,87,0,83,0,72,0,62,0,64,0,57,0,58,0,61,0,64,0,66,0,62,0,53,0,44,0,30,0,25,0,21,0,18,0,255,255,255,255,250,255,245,255,245,255,244,255,234,255,231,255,228,255,217,255,213,255,210,255,213,255,
        212,255,210,255,214,255,206,255,209,255,209,255,205,255,206,255,206,255,198,255,205,255,206,255,206,255,205,255,212,255,216,255,217,255,227,255,231,255,235,255,242,255,244,255,0,0,3,0,0,0,4,0,6,0,4,0,7,0,255,255,0,0,255,255,6,0,8,0,14,0,15,0,17,0,18,
        0,8,0,3,0,0,0,1,0,4,0,7,0,7,0,4,0,3,0,4,0,1,0,250,255,11,0,12,0,10,0,7,0,255,255,3,0,250,255,241,255,234,255,237,255,230,255,238,255,237,255,241,255,237,255,235,255,238,255,228,255,228,255,226,255,219,255,212,255,217,255,221,255,208,255,212,255,212,255,
        209,255,201,255,210,255,210,255,206,255,210,255,205,255,203,255,194,255,196,255,190,255,192,255,194,255,196,255,198,255,199,255,196,255,192,255,194,255,190,255,195,255,198,255,188,255,184,255,180,255,181,255,187,255,195,255,196,255,191,255,195,255,195,
        255,188,255,188,255,187,255,185,255,194,255,191,255,195,255,196,255,199,255,195,255,201,255,201,255,205,255,201,255,202,255,203,255,213,255,216,255,213,255,212,255,208,255,199,255,198,255,209,255,209,255,209,255,210,255,208,255,206,255,199,255,191,255,
        176,255,172,255,172,255,176,255,187,255,178,255,185,255,187,255,184,255,185,255,191,255,195,255,195,255,202,255,203,255,210,255,210,255,208,255,212,255,213,255,219,255,226,255,230,255,231,255,232,255,234,255,238,255,234,255,237,255,245,255,255,255,4,
        0,7,0,12,0,12,0,24,0,24,0,26,0,28,0,30,0,33,0,33,0,33,0,39,0,47,0,43,0,48,0,55,0,60,0,61,0,61,0,57,0,57,0,57,0,55,0,57,0,68,0,69,0,71,0,71,0,72,0,78,0,72,0,82,0,83,0,93,0,96,0,93,0,93,0,89,0,87,0,89,0,91,0,82,0,83,0,84,0,78,0,76,0,71,0,69,0,75,0,68,0,
        69,0,71,0,68,0,78,0,69,0,65,0,69,0,61,0,64,0,64,0,53,0,57,0,58,0,47,0,54,0,51,0,55,0,46,0,43,0,43,0,43,0,33,0,36,0,32,0,30,0,29,0,29,0,22,0,14,0,14,0,21,0,36,0,39,0,36,0,35,0,40,0,43,0,42,0,42,0,48,0,60,0,48,0,54,0,66,0,55,0,62,0,62,0,62,0,69,0,64,0,
        66,0,65,0,64,0,62,0,64,0,64,0,58,0,46,0,51,0,47,0,40,0,35,0,30,0,19,0,15,0,8,0,1,0,255,255,238,255,238,255,238,255,237,255,239,255,235,255,232,255,226,255,228,255,221,255,213,255,219,255,213,255,212,255,208,255,205,255,213,255,212,255,212,255,216,255,
        213,255,209,255,206,255,206,255,214,255,216,255,219,255,220,255,228,255,234,255,244,255,246,255,255,255,1,0,1,0,8,0,4,0,4,0,12,0,14,0,12,0,10,0,15,0,22,0,18,0,28,0,24,0,21,0,15,0,18,0,12,0,14,0,17,0,17,0,19,0,19,0,17,0,12,0,4,0,1,0,0,0,249,255,252,255,
        1,0,3,0,250,255,248,255,248,255,242,255,237,255,237,255,227,255,230,255,231,255,235,255,235,255,238,255,230,255,235,255,219,255,221,255,227,255,228,255,221,255,210,255,209,255,210,255,210,255,210,255,212,255,216,255,208,255,209,255,195,255,195,255,195,
        255,196,255,196,255,191,255,187,255,195,255,198,255,201,255,203,255,206,255,196,255,195,255,205,255,201,255,195,255,194,255,191,255,195,255,192,255,190,255,190,255,187,255,195,255,198,255,196,255,196,255,192,255,192,255,187,255,195,255,195,255,195,255,
        202,255,206,255,210,255,208,255,212,255,206,255,205,255,213,255,213,255,217,255,223,255,223,255,223,255,228,255,227,255,220,255,221,255,216,255,217,255,223,255,212,255,203,255,201,255,195,255,196,255,190,255,192,255,191,255,191,255,188,255,192,255,194,
        255,201,255,201,255,205,255,206,255,209,255,209,255,216,255,210,255,212,255,217,255,221,255,220,255,226,255,230,255,235,255,241,255,242,255,245,255,250,255,237,255,241,255,250,255,252,255,7,0,11,0,18,0,18,0,29,0,24,0,24,0,33,0,32,0,36,0,35,0,33,0,39,
        0,48,0,46,0,43,0,44,0,60,0,64,0,57,0,62,0,68,0,62,0,60,0,64,0,66,0,65,0,61,0,73,0,75,0,78,0,72,0,79,0,75,0,83,0,76,0,83,0,86,0,80,0,86,0,89,0,90,0,90,0,87,0,89,0,84,0,87,0,83,0,82,0,82,0,71,0,65,0,61,0,60,0,54,0,55,0,60,0,60,0,66,0,64,0,69,0,61,0,57,
        0,61,0,48,0,42,0,32,0,37,0,36,0,39,0,39,0,39,0,37,0,36,0,36,0,24,0,28,0,25,0,24,0,25,0,22,0,28,0,33,0,28,0,30,0,33,0,33,0,35,0,35,0,37,0,39,0,36,0,33,0,30,0,33,0,33,0,33,0,30,0,33,0,32,0,43,0,40,0,40,0,44,0,44,0,40,0,42,0,33,0,32,0,28,0,28,0,22,0,30,
        0,28,0,24,0,15,0,6,0,3,0,253,255,244,255,237,255,226,255,221,255,226,255,226,255,223,255,234,255,226,255,230,255,228,255,220,255,212,255,212,255,212,255,208,255,203,255,203,255,206,255,210,255,213,255,220,255,230,255,224,255,223,255,227,255,226,255,226,
        255,226,255,231,255,238,255,237,255,238,255,241,255,245,255,250,255,0,0,250,255,255,255,7,0,11,0,15,0,15,0,15,0,17,0,14,0,15,0,30,0,25,0,24,0,21,0,22,0,24,0,24,0,12,0,18,0,15,0,15,0,7,0,8,0,7,0,0,0,252,255,246,255,245,255,244,255,252,255,248,255,237,
        255,239,255,244,255,238,255,237,255,237,255,235,255,230,255,231,255,221,255,228,255,232,255,234,255,232,255,231,255,226,255,228,255,219,255,214,255,216,255,216,255,212,255,212,255,212,255,205,255,210,255,206,255,195,255,199,255,187,255,190,255,190,255,
        187,255,188,255,196,255,201,255,205,255,209,255,194,255,190,255,199,255,192,255,191,255,181,255,181,255,183,255,181,255,183,255,185,255,191,255,199,255,195,255,190,255,188,255,185,255,190,255,194,255,192,255,192,255,195,255,202,255,205,255,205,255,206,
        255,213,255,212,255,210,255,208,255,209,255,210,255,217,255,213,255,213,255,219,255,221,255,219,255,219,255,217,255,213,255,208,255,203,255,201,255,198,255,196,255,195,255,192,255,194,255,196,255,192,255,192,255,198,255,203,255,205,255,213,255,213,255,
        220,255,227,255,234,255,231,255,227,255,235,255,237,255,238,255,245,255,245,255,249,255,248,255,248,255,253,255,255,255,4,0,4,0,3,0,0,0,3,0,8,0,10,0,17,0,15,0,26,0,32,0,33,0,32,0,30,0,29,0,39,0,40,0,37,0,39,0,37,0,50,0,53,0,62,0,60,0,54,0,61,0,65,0,66,
        0,66,0,69,0,64,0,60,0,64,0,71,0,76,0,73,0,78,0,83,0,80,0,76,0,78,0,83,0,82,0,79,0,87,0,80,0,76,0,78,0,82,0,86,0,90,0,83,0,76,0,72,0,61,0,61,0,57,0,61,0,53,0,55,0,68,0,72,0,69,0,66,0,66,0,60,0,48,0,51,0,47,0,39,0,42,0,42,0,46,0,47,0,42,0,37,0,35,0,39,
        0,32,0,28,0,28,0,19,0,19,0,29,0,28,0,35,0,28,0,32,0,32,0,37,0,36,0,39,0,32,0,35,0,25,0,14,0,18,0,17,0,24,0,24,0,35,0,30,0,29,0,33,0,33,0,30,0,32,0,30,0,24,0,21,0,21,0,21,0,22,0,18,0,14,0,12,0,8,0,14,0,10,0,8,0,8,0,1,0,1,0,250,255,241,255,237,255,234,
        255,231,255,234,255,237,255,231,255,221,255,217,255,220,255,223,255,224,255,226,255,228,255,224,255,221,255,219,255,214,255,220,255,220,255,227,255,226,255,228,255,227,255,230,255,228,255,232,255,232,255,234,255,235,255,239,255,245,255,249,255,255,255,
        3,0,6,0,11,0,18,0,24,0,19,0,18,0,19,0,26,0,24,0,26,0,24,0,24,0,29,0,36,0,26,0,29,0,25,0,19,0,18,0,18,0,12,0,17,0,11,0,10,0,8,0,12,0,7,0,255,255,246,255,241,255,242,255,244,255,246,255,246,255,245,255,242,255,235,255,228,255,230,255,223,255,226,255,227,
        255,230,255,237,255,232,255,230,255,231,255,231,255,232,255,231,255,213,255,212,255,206,255,196,255,196,255,194,255,192,255,195,255,194,255,195,255,192,255,188,255,195,255,195,255,190,255,187,255,183,255,191,255,187,255,190,255,190,255,187,255,190,255,
        187,255,188,255,187,255,188,255,190,255,188,255,192,255,194,255,191,255,194,255,190,255,191,255,191,255,192,255,187,255,190,255,190,255,190,255,201,255,203,255,198,255,203,255,206,255,209,255,212,255,210,255,206,255,201,255,209,255,210,255,213,255,217,
        255,217,255,214,255,214,255,217,255,219,255,216,255,210,255,208,255,212,255,213,255,208,255,213,255,210,255,209,255,208,255,206,255,209,255,212,255,210,255,214,255,219,255,221,255,220,255,228,255,231,255,238,255,238,255,234,255,239,255,238,255,246,255,
        244,255,244,255,250,255,4,0,255,255,253,255,1,0,4,0,3,0,14,0,14,0,18,0,19,0,19,0,29,0,24,0,24,0,21,0,29,0,33,0,39,0,39,0,42,0,46,0,48,0,46,0,47,0,51,0,39,0,50,0,50,0,61,0,57,0,66,0,61,0,62,0,64,0,62,0,61,0,61,0,66,0,68,0,71,0,79,0,79,0,80,0,75,0,76,0,
        75,0,73,0,83,0,91,0,86,0,93,0,83,0,73,0,83,0,84,0,80,0,79,0,79,0,73,0,71,0,69,0,68,0,58,0,57,0,54,0,62,0,61,0,58,0,57,0,55,0,57,0,54,0,51,0,48,0,39,0,36,0,32,0,33,0,33,0,33,0,36,0,32,0,37,0,32,0,32,0,29,0,25,0,19,0,21,0,19,0,22,0,22,0,21,0,18,0,24,0,
        21,0,21,0,19,0,18,0,17,0,15,0,15,0,11,0,14,0,19,0,10,0,11,0,10,0,15,0,8,0,14,0,11,0,14,0,14,0,11,0,6,0,8,0,10,0,8,0,4,0,6,0,1,0,3,0,1,0,253,255,255,255,249,255,241,255,231,255,232,255,232,255,231,255,230,255,231,255,228,255,230,255,228,255,241,255,232,
        255,232,255,230,255,231,255,235,255,242,255,238,255,232,255,227,255,231,255,238,255,239,255,235,255,237,255,235,255,238,255,241,255,244,255,249,255,246,255,253,255,4,0,14,0,14,0,12,0,17,0,14,0,24,0,24,0,22,0,25,0,22,0,25,0,21,0,21,0,24,0,30,0,30,0,30,
        0,36,0,24,0,19,0,24,0,21,0,21,0,11,0,12,0,10,0,8,0,8,0,1,0,3,0,1,0,3,0,255,255,249,255,249,255,241,255,238,255,237,255,239,255,238,255,230,255,231,255,230,255,232,255,228,255,234,255,234,255,231,255,235,255,237,255,228,255,224,255,213,255,217,255,210,
        255,210,255,214,255,212,255,214,255,206,255,205,255,198,255,198,255,196,255,191,255,184,255,174,255,170,255,173,255,187,255,184,255,180,255,178,255,177,255,178,255,187,255,198,255,194,255,195,255,192,255,195,255,203,255,198,255,192,255,196,255,198,255,
        198,255,188,255,198,255,196,255,203,255,206,255,209,255,199,255,198,255,199,255,206,255,214,255,210,255,212,255,213,255,213,255,220,255,217,255,217,255,214,255,214,255,206,255,209,255,208,255,210,255,217,255,216,255,216,255,219,255,220,255,223,255,223,
        255,221,255,223,255,220,255,220,255,216,255,219,255,216,255,217,255,219,255,226,255,232,255,228,255,231,255,234,255,244,255,237,255,234,255,235,255,248,255,248,255,250,255,255,255,6,0,0,0,14,0,17,0,12,0,14,0,18,0,15,0,18,0,19,0,19,0,18,0,14,0,22,0,19,
        0,26,0,24,0,29,0,33,0,35,0,37,0,37,0,42,0,44,0,43,0,47,0,46,0,50,0,48,0,47,0,51,0,57,0,60,0,57,0,65,0,62,0,65,0,66,0,58,0,61,0,64,0,66,0,68,0,65,0,66,0,68,0,66,0,73,0,76,0,76,0,78,0,76,0,69,0,69,0,78,0,72,0,69,0,66,0,66,0,61,0,61,0,58,0,69,0,57,0,57,
        0,53,0,54,0,46,0,46,0,46,0,47,0,48,0,40,0,47,0,42,0,46,0,40,0,39,0,46,0,47,0,46,0,40,0,37,0,33,0,30,0,35,0,25,0,26,0,24,0,12,0,14,0,10,0,12,0,14,0,8,0,14,0,10,0,10,0,12,0,15,0,12,0,11,0,14,0,7,0,11,0,1,0,1,0,0,0,255,255,252,255,8,0,3,0,3,0,4,0,250,255,
        252,255,252,255,252,255,249,255,252,255,253,255,252,255,249,255,250,255,252,255,245,255,248,255,248,255,239,255,242,255,238,255,235,255,230,255,235,255,232,255,230,255,228,255,228,255,235,255,226,255,238,255,231,255,227,255,226,255,230,255,230,255,235,
        255,230,255,231,255,230,255,239,255,232,255,239,255,237,255,238,255,241,255,238,255,250,255,4,0,3,0,10,0,8,0,7,0,10,0,12,0,17,0,22,0,19,0,19,0,29,0,21,0,26,0,25,0,21,0,24,0,28,0,28,0,33,0,25,0,28,0,24,0,24,0,18,0,14,0,14,0,10,0,6,0,3,0,3,0,3,0,252,255,
        250,255,246,255,246,255,248,255,242,255,238,255,241,255,242,255,244,255,242,255,237,255,237,255,230,255,232,255,234,255,228,255,224,255,223,255,226,255,220,255,221,255,226,255,224,255,221,255,219,255,212,255,205,255,208,255,201,255,199,255,198,255,201,
        255,192,255,192,255,185,255,188,255,181,255,180,255,177,255,181,255,180,255,184,255,181,255,178,255,195,255,201,255,202,255,195,255,187,255,191,255,199,255,195,255,202,255,203,255,205,255,202,255,208,255,213,255,203,255,210,255,206,255,209,255,209,255,
        208,255,203,255,203,255,202,255,212,255,213,255,224,255,227,255,214,255,221,255,219,255,223,255,219,255,212,255,220,255,216,255,219,255,219,255,224,255,228,255,216,255,223,255,220,255,228,255,227,255,230,255,228,255,227,255,224,255,219,255,224,255,223,
        255,221,255,226,255,227,255,234,255,239,255,241,255,245,255,249,255,255,255,6,0,255,255,0,0,1,0,253,255,0,0,0,0,4,0,15,0,11,0,7,0,14,0,14,0,24,0,26,0,28,0,18,0,26,0,22,0,21,0,22,0,18,0,22,0,28,0,26,0,25,0,29,0,37,0,37,0,37,0,44,0,40,0,47,0,48,0,55,0,
        55,0,55,0,54,0,62,0,62,0,60,0,66,0,68,0,64,0,60,0,65,0,64,0,57,0,60,0,57,0,60,0,64,0,64,0,66,0,73,0,76,0,76,0,75,0,73,0,76,0,75,0,75,0,75,0,73,0,71,0,78,0,72,0,68,0,64,0,64,0,57,0,48,0,47,0,48,0,46,0,46,0,42,0,40,0,42,0,43,0,47,0,47,0,42,0,33,0,30,0,
        29,0,32,0,29,0,29,0,29,0,29,0,24,0,28,0,22,0,21,0,19,0,22,0,17,0,19,0,12,0,15,0,10,0,17,0,11,0,7,0,6,0,6,0,6,0,4,0,11,0,10,0,8,0,3,0,255,255,253,255,248,255,249,255,248,255,249,255,246,255,252,255,249,255,245,255,250,255,249,255,246,255,248,255,245,255,
        241,255,246,255,248,255,246,255,246,255,252,255,250,255,252,255,252,255,237,255,242,255,235,255,241,255,241,255,239,255,231,255,232,255,234,255,238,255,228,255,231,255,227,255,239,255,242,255,238,255,234,255,230,255,238,255,239,255,234,255,237,255,238,
        255,248,255,255,255,248,255,250,255,250,255,3,0,4,0,10,0,3,0,8,0,252,255,7,0,10,0,12,0,11,0,15,0,24,0,19,0,28,0,30,0,32,0,33,0,30,0,25,0,21,0,25,0,25,0,21,0,18,0,18,0,14,0,12,0,21,0,17,0,8,0,7,0,6,0,252,255,250,255,246,255,249,255,252,255,249,255,249,
        255,242,255,235,255,238,255,244,255,242,255,241,255,234,255,235,255,232,255,231,255,221,255,214,255,224,255,224,255,223,255,228,255,226,255,227,255,221,255,220,255,226,255,219,255,208,255,206,255,202,255,199,255,195,255,191,255,187,255,187,255,177,255,
        180,255,181,255,184,255,185,255,184,255,190,255,195,255,195,255,201,255,203,255,202,255,201,255,199,255,205,255,205,255,209,255,209,255,216,255,216,255,209,255,208,255,214,255,210,255,216,255,214,255,214,255,208,255,210,255,216,255,217,255,221,255,220,
        255,223,255,221,255,219,255,221,255,221,255,227,255,214,255,217,255,216,255,216,255,221,255,224,255,226,255,228,255,228,255,226,255,234,255,231,255,220,255,227,255,230,255,230,255,235,255,234,255,235,255,238,255,244,255,238,255,239,255,239,255,245,255,
        241,255,246,255,244,255,250,255,253,255,0,0,0,0,253,255,3,0,8,0,6,0,15,0,10,0,12,0,11,0,15,0,11,0,15,0,21,0,24,0,15,0,15,0,18,0,25,0,24,0,30,0,22,0,32,0,28,0,29,0,37,0,36,0,36,0,36,0,35,0,35,0,43,0,43,0,46,0,43,0,43,0,46,0,37,0,44,0,47,0,46,0,48,0,53,
        0,53,0,54,0,57,0,53,0,54,0,48,0,53,0,53,0,58,0,51,0,54,0,55,0,55,0,62,0,61,0,61,0,62,0,57,0,55,0,51,0,57,0,50,0,51,0,53,0,51,0,54,0,48,0,48,0,42,0,43,0,46,0,44,0,39,0,32,0,33,0,35,0,32,0,36,0,29,0,28,0,28,0,24,0,28,0,28,0,25,0,24,0,22,0,24,0,17,0,15,
        0,17,0,18,0,17,0,8,0,6,0,4,0,4,0,6,0,6,0,12,0,11,0,11,0,6,0,255,255,252,255,246,255,252,255,249,255,239,255,239,255,234,255,244,255,238,255,241,255,242,255,245,255,246,255,244,255,248,255,249,255,245,255,245,255,241,255,242,255,241,255,249,255,250,255,
        248,255,248,255,249,255,250,255,246,255,237,255,241,255,239,255,238,255,234,255,238,255,237,255,237,255,238,255,231,255,237,255,231,255,231,255,234,255,235,255,231,255,239,255,244,255,245,255,245,255,246,255,249,255,253,255,249,255,252,255,250,255,1,
        0,3,0,252,255,252,255,0,0,1,0,7,0,15,0,15,0,10,0,17,0,17,0,28,0,22,0,30,0,30,0,32,0,37,0,30,0,36,0,30,0,25,0,21,0,18,0,19,0,24,0,24,0,26,0,21,0,25,0,11,0,10,0,8,0,255,255,0,0,253,255,249,255,253,255,246,255,248,255,245,255,237,255,239,255,237,255,234,
        255,232,255,239,255,238,255,231,255,234,255,226,255,231,255,231,255,237,255,237,255,234,255,231,255,226,255,210,255,209,255,214,255,213,255,214,255,214,255,217,255,213,255,206,255,201,255,196,255,190,255,183,255,183,255,184,255,187,255,191,255,185,255,
        194,255,199,255,202,255,205,255,195,255,202,255,203,255,209,255,208,255,214,255,219,255,223,255,214,255,216,255,210,255,216,255,219,255,219,255,223,255,219,255,223,255,216,255,217,255,220,255,223,255,221,255,221,255,227,255,228,255,217,255,220,255,223,
        255,221,255,228,255,226,255,230,255,226,255,232,255,231,255,228,255,238,255,232,255,234,255,238,255,245,255,246,255,249,255,244,255,249,255,245,255,248,255,241,255,245,255,248,255,248,255,249,255,246,255,248,255,246,255,249,255,252,255,245,255,3,0,255,
        255,0,0,255,255,1,0,4,0,4,0,4,0,6,0,7,0,6,0,8,0,18,0,21,0,24,0,19,0,26,0,22,0,15,0,22,0,22,0,19,0,25,0,25,0,36,0,26,0,22,0,24,0,30,0,22,0,32,0,30,0,35,0,32,0,39,0,36,0,35,0,39,0,46,0,43,0,43,0,46,0,43,0,46,0,53,0,48,0,48,0,58,0,60,0,50,0,50,0,50,0,55,
        0,53,0,44,0,46,0,50,0,60,0,62,0,60,0,60,0,61,0,53,0,48,0,48,0,50,0,48,0,46,0,48,0,50,0,48,0,50,0,47,0,44,0,43,0,39,0,32,0,36,0,26,0,29,0,25,0,21,0,17,0,21,0,18,0,26,0,19,0,18,0,15,0,18,0,17,0,15,0,12,0,11,0,10,0,14,0,15,0,11,0,15,0,19,0,12,0,7,0,4,0,
        1,0,8,0,8,0,8,0,3,0,4,0,253,255,250,255,246,255,252,255,248,255,244,255,238,255,238,255,235,255,232,255,235,255,237,255,237,255,231,255,234,255,239,255,238,255,249,255,252,255,252,255,0,0,250,255,255,255,0,0,1,0,252,255,248,255,241,255,238,255,237,255,
        235,255,230,255,237,255,237,255,239,255,241,255,237,255,237,255,226,255,234,255,234,255,238,255,238,255,242,255,239,255,241,255,246,255,244,255,244,255,249,255,0,0,248,255,3,0,3,0,7,0,8,0,7,0,10,0,0,0,4,0,8,0,6,0,7,0,12,0,15,0,24,0,19,0,22,0,24,0,22,
        0,25,0,26,0,22,0,24,0,25,0,28,0,22,0,18,0,18,0,11,0,15,0,21,0,21,0,18,0,15,0,8,0,12,0,4,0,7,0,10,0,1,0,250,255,246,255,244,255,235,255,235,255,235,255,232,255,239,255,234,255,231,255,239,255,248,255,241,255,241,255,238,255,230,255,227,255,226,255,217,
        255,223,255,220,255,223,255,221,255,216,255,212,255,209,255,199,255,199,255,205,255,206,255,202,255,201,255,212,255,205,255,202,255,203,255,205,255,210,255,208,255,206,255,208,255,208,255,209,255,203,255,206,255,213,255,208,255,212,255,214,255,213,255,
        217,255,214,255,220,255,213,255,214,255,213,255,219,255,221,255,220,255,223,255,230,255,224,255,224,255,231,255,230,255,226,255,230,255,227,255,234,255,231,255,230,255,235,255,228,255,224,255,227,255,231,255,231,255,232,255,234,255,238,255,241,255,231,
        255,231,255,235,255,242,255,237,255,238,255,241,255,248,255,246,255,244,255,250,255,249,255,248,255,248,255,245,255,250,255,253,255,246,255,1,0,0,0,255,255,253,255,248,255,245,255,249,255,250,255,248,255,248,255,255,255,255,255,4,0,1,0,6,0,8,0,8,0,10,
        0,10,0,12,0,18,0,25,0,26,0,22,0,30,0,25,0,28,0,24,0,28,0,29,0,33,0,28,0,29,0,32,0,22,0,25,0,33,0,32,0,35,0,37,0,36,0,29,0,28,0,28,0,32,0,37,0,37,0,43,0,43,0,48,0,46,0,44,0,36,0,33,0,32,0,33,0,36,0,46,0,42,0,47,0,48,0,48,0,46,0,36,0,42,0,32,0,37,0,33,
        0,29,0,40,0,39,0,39,0,33,0,37,0,36,0,32,0,30,0,28,0,28,0,29,0,29,0,25,0,18,0,14,0,15,0,18,0,17,0,10,0,15,0,8,0,4,0,17,0,12,0,19,0,15,0,11,0,17,0,14,0,19,0,17,0,14,0,7,0,8,0,14,0,10,0,12,0,14,0,11,0,8,0,11,0,0,0,4,0,248,255,245,255,241,255,238,255,239,
        255,238,255,237,255,245,255,246,255,238,255,238,255,238,255,241,255,242,255,244,255,250,255,0,0,246,255,1,0,252,255,0,0,253,255,252,255,255,255,255,255,252,255,248,255,250,255,250,255,246,255,250,255,244,255,244,255,245,255,242,255,237,255,234,255,241,
        255,239,255,239,255,239,255,239,255,237,255,249,255,248,255,255,255,248,255,249,255,244,255,249,255,252,255,252,255,0,0,3,0,7,0,4,0,7,0,14,0,8,0,14,0,18,0,12,0,15,0,17,0,26,0,19,0,19,0,19,0,24,0,22,0,25,0,32,0,26,0,30,0,24,0,22,0,22,0,19,0,22,0,22,0,
        18,0,18,0,25,0,12,0,7,0,10,0,11,0,11,0,7,0,255,255,252,255,253,255,250,255,250,255,245,255,244,255,245,255,244,255,242,255,237,255,241,255,231,255,235,255,230,255,221,255,224,255,223,255,224,255,232,255,227,255,221,255,217,255,219,255,213,255,221,255,
        214,255,216,255,210,255,210,255,202,255,202,255,196,255,202,255,209,255,210,255,210,255,217,255,214,255,219,255,212,255,213,255,213,255,214,255,214,255,209,255,209,255,209,255,212,255,210,255,216,255,216,255,216,255,217,255,227,255,226,255,228,255,228,
        255,221,255,224,255,227,255,235,255,228,255,238,255,237,255,231,255,237,255,241,255,242,255,235,255,235,255,238,255,244,255,235,255,242,255,242,255,241,255,242,255,239,255,245,255,246,255,250,255,244,255,244,255,245,255,244,255,239,255,242,255,239,255,
        245,255,246,255,245,255,252,255,255,255,253,255,0,0,3,0,6,0,8,0,3,0,0,0,252,255,0,0,255,255,255,255,252,255,253,255,253,255,252,255,246,255,248,255,0,0,253,255,0,0,0,0,255,255,253,255,252,255,1,0,8,0,18,0,22,0,22,0,24,0,26,0,18,0,26,0,24,0,19,0,19,0,
        18,0,19,0,15,0,18,0,28,0,22,0,22,0,17,0,25,0,30,0,33,0,29,0,24,0,32,0,35,0,35,0,36,0,35,0,33,0,35,0,48,0,37,0,35,0,32,0,39,0,43,0,37,0,40,0,39,0,40,0,39,0,43,0,37,0,30,0,33,0,35,0,33,0,26,0,29,0,30,0,33,0,35,0,32,0,33,0,24,0,19,0,25,0,21,0,17,0,12,0,
        4,0,14,0,11,0,10,0,12,0,15,0,11,0,8,0,4,0,3,0,6,0,6,0,10,0,6,0,6,0,10,0,4,0,3,0,8,0,3,0,6,0,1,0,3,0,0,0,253,255,253,255,4,0,7,0,6,0,7,0,4,0,0,0,0,0,248,255,245,255,242,255,235,255,239,255,239,255,238,255,241,255,237,255,232,255,231,255,241,255,234,255,
        252,255,252,255,252,255,249,255,248,255,252,255,253,255,0,0,1,0,252,255,3,0,4,0,1,0,250,255,253,255,252,255,250,255,248,255,242,255,246,255,249,255,249,255,248,255,248,255,245,255,248,255,245,255,252,255,244,255,242,255,241,255,245,255,3,0,252,255,3,
        0,4,0,0,0,255,255,255,255,0,0,11,0,18,0,17,0,15,0,17,0,17,0,22,0,29,0,22,0,24,0,22,0,26,0,30,0,33,0,35,0,35,0,39,0,32,0,32,0,32,0,25,0,22,0,21,0,17,0,15,0,14,0,11,0,12,0,11,0,7,0,14,0,14,0,8,0,255,255,1,0,1,0,252,255,249,255,248,255,246,255,242,255,238,
        255,242,255,242,255,242,255,232,255,235,255,235,255,235,255,234,255,237,255,234,255,239,255,228,255,226,255,227,255,223,255,220,255,221,255,216,255,212,255,212,255,220,255,216,255,223,255,221,255,212,255,210,255,208,255,212,255,213,255,213,255,217,255,
        214,255,212,255,210,255,224,255,226,255,224,255,221,255,221,255,219,255,220,255,226,255,214,255,219,255,223,255,226,255,223,255,227,255,220,255,226,255,227,255,232,255,235,255,237,255,235,255,239,255,238,255,242,255,244,255,245,255,248,255,252,255,249,
        255,0,0,1,0,1,0,252,255,255,255,250,255,244,255,245,255,248,255,245,255,244,255,246,255,245,255,246,255,246,255,248,255,246,255,249,255,245,255,242,255,245,255,250,255,255,255,252,255,255,255,3,0,252,255,249,255,252,255,4,0,7,0,4,0,0,0,0,0,0,0,253,255,
        0,0,253,255,250,255,4,0,255,255,4,0,4,0,1,0,8,0,3,0,0,0,0,0,6,0,3,0,6,0,1,0,17,0,18,0,26,0,21,0,24,0,25,0,24,0,22,0,22,0,22,0,19,0,18,0,22,0,22,0,28,0,25,0,22,0,25,0,25,0,18,0,24,0,21,0,21,0,30,0,30,0,29,0,33,0,42,0,37,0,42,0,37,0,36,0,40,0,39,0,43,0,
        37,0,35,0,37,0,24,0,25,0,25,0,25,0,22,0,19,0,25,0,26,0,19,0,17,0,21,0,17,0,15,0,11,0,19,0,19,0,19,0,15,0,17,0,12,0,15,0,15,0,19,0,12,0,6,0,1,0,252,255,0,0,7,0,1,0,3,0,8,0,6,0,7,0,1,0,14,0,14,0,6,0,8,0,8,0,1,0,3,0,255,255,252,255,252,255,250,255,255,255,
        250,255,253,255,0,0,6,0,0,0,249,255,255,255,255,255,249,255,250,255,244,255,248,255,245,255,239,255,231,255,232,255,237,255,241,255,244,255,252,255,248,255,245,255,239,255,245,255,241,255,250,255,250,255,1,0,246,255,255,255,255,255,255,255,255,255,253,
        255,250,255,255,255,1,0,253,255,250,255,248,255,246,255,246,255,246,255,252,255,249,255,252,255,250,255,255,255,249,255,250,255,250,255,249,255,248,255,252,255,249,255,6,0,3,0,11,0,8,0,15,0,17,0,14,0,17,0,17,0,21,0,26,0,26,0,29,0,29,0,29,0,24,0,19,0,
        17,0,19,0,19,0,14,0,21,0,19,0,24,0,19,0,21,0,21,0,19,0,14,0,21,0,6,0,10,0,15,0,6,0,3,0,4,0,3,0,4,0,255,255,253,255,245,255,244,255,244,255,241,255,241,255,238,255,245,255,241,255,238,255,235,255,235,255,232,255,232,255,231,255,232,255,235,255,235,255,
        231,255,228,255,221,255,221,255,223,255,223,255,230,255,213,255,213,255,216,255,210,255,208,255,203,255,212,255,212,255,206,255,210,255,212,255,208,255,216,255,210,255,209,255,213,255,206,255,213,255,223,255,221,255,227,255,230,255,232,255,228,255,226,
        255,230,255,224,255,226,255,224,255,226,255,230,255,234,255,228,255,230,255,232,255,231,255,227,255,232,255,235,255,239,255,244,255,237,255,252,255,249,255,249,255,250,255,252,255,252,255,0,0,248,255,255,255,252,255,250,255,3,0,1,0,252,255,248,255,246,
        255,245,255,245,255,245,255,246,255,242,255,249,255,249,255,245,255,250,255,249,255,252,255,253,255,1,0,1,0,4,0,0,0,3,0,6,0,3,0,8,0,7,0,7,0,250,255,4,0,252,255,246,255,253,255,252,255,3,0,4,0,250,255,249,255,239,255,250,255,249,255,4,0,0,0,11,0,11,0,
        15,0,17,0,21,0,14,0,17,0,21,0,21,0,25,0,22,0,22,0,28,0,35,0,25,0,26,0,24,0,26,0,25,0,24,0,18,0,26,0,24,0,26,0,28,0,19,0,29,0,30,0,25,0,26,0,35,0,35,0,28,0,22,0,28,0,28,0,33,0,30,0,30,0,37,0,37,0,39,0,28,0,22,0,19,0,18,0,10,0,12,0,14,0,15,0,11,0,12,0,
        14,0,17,0,8,0,8,0,8,0,10,0,19,0,4,0,6,0,8,0,11,0,8,0,6,0,6,0,8,0,1,0,252,255,253,255,1,0,253,255,1,0,1,0,8,0,7,0,7,0,7,0,4,0,6,0,255,255,252,255,253,255,255,255,252,255,250,255,6,0,6,0,0,0,6,0,3,0,0,0,249,255,249,255,255,255,246,255,249,255,250,255,248,
        255,244,255,245,255,249,255,244,255,252,255,245,255,244,255,242,255,245,255,237,255,237,255,244,255,244,255,250,255,255,255,1,0,253,255,255,255,255,255,6,0,4,0,4,0,253,255,255,255,0,0,1,0,252,255,241,255,250,255,252,255,255,255,0,0,255,255,1,0,253,255,
        244,255,245,255,239,255,244,255,248,255,3,0,8,0,7,0,11,0,3,0,10,0,14,0,14,0,14,0,15,0,22,0,26,0,24,0,26,0,26,0,22,0,26,0,32,0,33,0,33,0,33,0,29,0,30,0,30,0,36,0,29,0,21,0,19,0,18,0,19,0,21,0,17,0,17,0,18,0,12,0,7,0,4,0,1,0,252,255,255,255,4,0,3,0,248,
        255,242,255,242,255,238,255,241,255,238,255,239,255,237,255,239,255,241,255,235,255,232,255,231,255,230,255,234,255,231,255,232,255,230,255,234,255,235,255,234,255,227,255,230,255,231,255,220,255,217,255,210,255,213,255,217,255,217,255,219,255,220,255,
        219,255,226,255,223,255,212,255,220,255,220,255,220,255,210,255,223,255,230,255,224,255,227,255,231,255,231,255,231,255,234,255,226,255,232,255,237,255,241,255,235,255,232,255,231,255,232,255,237,255,235,255,234,255,0,0
    };

    static const int cello_wavSize = numElementsInArray (cello_wav);
}

class WaveAudioFormatUnitTests : public AudioFormatUnitTests<WavAudioFormat>
{
public:
    WaveAudioFormatUnitTests() {}

    InputStream* getValidTestFileData() override
    {
        return new MemoryInputStream (WaveUnitTestData::cello_wav, WaveUnitTestData::cello_wavSize, false);
    }

    StringPairArray createTestMetadata() override
    {
        StringPairArray metadataValues (WavAudioFormat::createBWAVMetadata ("description",
                                                                            "originator",
                                                                            "originatorRef",
                                                                            Time::getCurrentTime(),
                                                                            defaultNumTestAudioBufferSamples,
                                                                            "codingHistory"));

        for (int i = numElementsInArray (WavFileHelpers::ListInfoChunk::types); --i >= 0;)
            metadataValues.set (WavFileHelpers::ListInfoChunk::types[i],
                                WavFileHelpers::ListInfoChunk::types[i]);

        if (metadataValues.size() > 0)
            metadataValues.set ("MetaDataSource", "WAV");

        return metadataValues;
    }

private:
    MemoryBlock noData;

    JUCE_DECLARE_NON_COPYABLE (WaveAudioFormatUnitTests)
};

static const WaveAudioFormatUnitTests waveAudioFormatUnitTests;

#endif
