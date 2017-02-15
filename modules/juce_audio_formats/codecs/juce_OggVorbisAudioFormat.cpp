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

#if JUCE_USE_OGGVORBIS

#if JUCE_MAC && ! defined (__MACOSX__)
 #define __MACOSX__ 1
#endif

namespace OggVorbisNamespace
{
#if JUCE_INCLUDE_OGGVORBIS_CODE || ! defined (JUCE_INCLUDE_OGGVORBIS_CODE)
 #if JUCE_MSVC
  #pragma warning (push)
  #pragma warning (disable: 4267 4127 4244 4996 4100 4701 4702 4013 4133 4206 4305 4189 4706 4995 4365 4456 4457 4459)
 #endif

 #if JUCE_CLANG
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wconversion"
  #pragma clang diagnostic ignored "-Wshadow"
  #pragma clang diagnostic ignored "-Wdeprecated-register"
 #endif

 #include "oggvorbis/vorbisenc.h"
 #include "oggvorbis/codec.h"
 #include "oggvorbis/vorbisfile.h"

 #include "oggvorbis/bitwise.c"
 #include "oggvorbis/framing.c"
 #include "oggvorbis/libvorbis-1.3.2/lib/analysis.c"
 #include "oggvorbis/libvorbis-1.3.2/lib/bitrate.c"
 #include "oggvorbis/libvorbis-1.3.2/lib/block.c"
 #include "oggvorbis/libvorbis-1.3.2/lib/codebook.c"
 #include "oggvorbis/libvorbis-1.3.2/lib/envelope.c"
 #include "oggvorbis/libvorbis-1.3.2/lib/floor0.c"
 #include "oggvorbis/libvorbis-1.3.2/lib/floor1.c"
 #include "oggvorbis/libvorbis-1.3.2/lib/info.c"
 #include "oggvorbis/libvorbis-1.3.2/lib/lpc.c"
 #include "oggvorbis/libvorbis-1.3.2/lib/lsp.c"
 #include "oggvorbis/libvorbis-1.3.2/lib/mapping0.c"
 #include "oggvorbis/libvorbis-1.3.2/lib/mdct.c"
 #include "oggvorbis/libvorbis-1.3.2/lib/psy.c"
 #include "oggvorbis/libvorbis-1.3.2/lib/registry.c"
 #include "oggvorbis/libvorbis-1.3.2/lib/res0.c"
 #include "oggvorbis/libvorbis-1.3.2/lib/sharedbook.c"
 #include "oggvorbis/libvorbis-1.3.2/lib/smallft.c"
 #include "oggvorbis/libvorbis-1.3.2/lib/synthesis.c"
 #include "oggvorbis/libvorbis-1.3.2/lib/vorbisenc.c"
 #include "oggvorbis/libvorbis-1.3.2/lib/vorbisfile.c"
 #include "oggvorbis/libvorbis-1.3.2/lib/window.c"

 #if JUCE_MSVC
  #pragma warning (pop)
 #endif

 #if JUCE_CLANG
  #pragma clang diagnostic pop
 #endif
#else
 #include <vorbis/vorbisenc.h>
 #include <vorbis/codec.h>
 #include <vorbis/vorbisfile.h>
#endif
}

#undef max
#undef min

//==============================================================================
static const char* const oggFormatName = "Ogg-Vorbis file";

const char* const OggVorbisAudioFormat::encoderName = "encoder";
const char* const OggVorbisAudioFormat::id3title = "id3title";
const char* const OggVorbisAudioFormat::id3artist = "id3artist";
const char* const OggVorbisAudioFormat::id3album = "id3album";
const char* const OggVorbisAudioFormat::id3comment = "id3comment";
const char* const OggVorbisAudioFormat::id3date = "id3date";
const char* const OggVorbisAudioFormat::id3genre = "id3genre";
const char* const OggVorbisAudioFormat::id3trackNumber = "id3trackNumber";


//==============================================================================
class OggReader : public AudioFormatReader
{
public:
    OggReader (InputStream* const inp)
        : AudioFormatReader (inp, oggFormatName),
          reservoirStart (0),
          samplesInReservoir (0)
    {
        using namespace OggVorbisNamespace;
        sampleRate = 0;
        usesFloatingPointData = true;

        callbacks.read_func  = &oggReadCallback;
        callbacks.seek_func  = &oggSeekCallback;
        callbacks.close_func = &oggCloseCallback;
        callbacks.tell_func  = &oggTellCallback;

        const int err = ov_open_callbacks (input, &ovFile, 0, 0, callbacks);

        if (err == 0)
        {
            vorbis_info* info = ov_info (&ovFile, -1);

            vorbis_comment* const comment = ov_comment (&ovFile, -1);
            addMetadataItem (comment, "ENCODER",     OggVorbisAudioFormat::encoderName);
            addMetadataItem (comment, "TITLE",       OggVorbisAudioFormat::id3title);
            addMetadataItem (comment, "ARTIST",      OggVorbisAudioFormat::id3artist);
            addMetadataItem (comment, "ALBUM",       OggVorbisAudioFormat::id3album);
            addMetadataItem (comment, "COMMENT",     OggVorbisAudioFormat::id3comment);
            addMetadataItem (comment, "DATE",        OggVorbisAudioFormat::id3date);
            addMetadataItem (comment, "GENRE",       OggVorbisAudioFormat::id3genre);
            addMetadataItem (comment, "TRACKNUMBER", OggVorbisAudioFormat::id3trackNumber);

            lengthInSamples = (uint32) ov_pcm_total (&ovFile, -1);
            numChannels = (unsigned int) info->channels;
            bitsPerSample = 16;
            sampleRate = info->rate;

            reservoir.setSize ((int) numChannels, (int) jmin (lengthInSamples, (int64) 4096));
        }
    }

    ~OggReader()
    {
        OggVorbisNamespace::ov_clear (&ovFile);
    }

    void addMetadataItem (OggVorbisNamespace::vorbis_comment* comment, const char* name, const char* metadataName)
    {
        if (const char* value = vorbis_comment_query (comment, name, 0))
            metadataValues.set (metadataName, value);
    }

    //==============================================================================
    bool readSamples (int** destSamples, int numDestChannels, int startOffsetInDestBuffer,
                      int64 startSampleInFile, int numSamples) override
    {
        while (numSamples > 0)
        {
            const int numAvailable = (int) (reservoirStart + samplesInReservoir - startSampleInFile);

            if (startSampleInFile >= reservoirStart && numAvailable > 0)
            {
                // got a few samples overlapping, so use them before seeking..

                const int numToUse = jmin (numSamples, numAvailable);

                for (int i = jmin (numDestChannels, reservoir.getNumChannels()); --i >= 0;)
                    if (destSamples[i] != nullptr)
                        memcpy (destSamples[i] + startOffsetInDestBuffer,
                                reservoir.getReadPointer (i, (int) (startSampleInFile - reservoirStart)),
                                sizeof (float) * (size_t) numToUse);

                startSampleInFile += numToUse;
                numSamples -= numToUse;
                startOffsetInDestBuffer += numToUse;

                if (numSamples == 0)
                    break;
            }

            if (startSampleInFile < reservoirStart
                || startSampleInFile + numSamples > reservoirStart + samplesInReservoir)
            {
                // buffer miss, so refill the reservoir
                int bitStream = 0;

                reservoirStart = jmax (0, (int) startSampleInFile);
                samplesInReservoir = reservoir.getNumSamples();

                if (reservoirStart != (int) OggVorbisNamespace::ov_pcm_tell (&ovFile))
                    OggVorbisNamespace::ov_pcm_seek (&ovFile, reservoirStart);

                int offset = 0;
                int numToRead = samplesInReservoir;

                while (numToRead > 0)
                {
                    float** dataIn = nullptr;

                    const long samps = OggVorbisNamespace::ov_read_float (&ovFile, &dataIn, numToRead, &bitStream);
                    if (samps <= 0)
                        break;

                    jassert (samps <= numToRead);

                    for (int i = jmin ((int) numChannels, reservoir.getNumChannels()); --i >= 0;)
                        memcpy (reservoir.getWritePointer (i, offset), dataIn[i], sizeof (float) * (size_t) samps);

                    numToRead -= samps;
                    offset += samps;
                }

                if (numToRead > 0)
                    reservoir.clear (offset, numToRead);
            }
        }

        if (numSamples > 0)
        {
            for (int i = numDestChannels; --i >= 0;)
                if (destSamples[i] != nullptr)
                    zeromem (destSamples[i] + startOffsetInDestBuffer, sizeof (int) * (size_t) numSamples);
        }

        return true;
    }

    //==============================================================================
    static size_t oggReadCallback (void* ptr, size_t size, size_t nmemb, void* datasource)
    {
        return (size_t) (static_cast<InputStream*> (datasource)->read (ptr, (int) (size * nmemb))) / size;
    }

    static int oggSeekCallback (void* datasource, OggVorbisNamespace::ogg_int64_t offset, int whence)
    {
        InputStream* const in = static_cast<InputStream*> (datasource);

        if (whence == SEEK_CUR)
            offset += in->getPosition();
        else if (whence == SEEK_END)
            offset += in->getTotalLength();

        in->setPosition (offset);
        return 0;
    }

    static int oggCloseCallback (void*)
    {
        return 0;
    }

    static long oggTellCallback (void* datasource)
    {
        return (long) static_cast<InputStream*> (datasource)->getPosition();
    }

private:
    OggVorbisNamespace::OggVorbis_File ovFile;
    OggVorbisNamespace::ov_callbacks callbacks;
    AudioSampleBuffer reservoir;
    int reservoirStart, samplesInReservoir;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OggReader)
};

//==============================================================================
class OggWriter  : public AudioFormatWriter
{
public:
    OggWriter (OutputStream* const out,
               const double sampleRate_,
               const unsigned int numChannels_,
               const unsigned int bitsPerSample_,
               const int qualityIndex,
               const StringPairArray& metadata)
        : AudioFormatWriter (out, oggFormatName, sampleRate_, numChannels_, bitsPerSample_),
          ok (false)
    {
        using namespace OggVorbisNamespace;

        vorbis_info_init (&vi);

        if (vorbis_encode_init_vbr (&vi, (int) numChannels_, (int) sampleRate_,
                                    jlimit (0.0f, 1.0f, qualityIndex * 0.1f)) == 0)
        {
            vorbis_comment_init (&vc);

            addMetadata (metadata, OggVorbisAudioFormat::encoderName,    "ENCODER");
            addMetadata (metadata, OggVorbisAudioFormat::id3title,       "TITLE");
            addMetadata (metadata, OggVorbisAudioFormat::id3artist,      "ARTIST");
            addMetadata (metadata, OggVorbisAudioFormat::id3album,       "ALBUM");
            addMetadata (metadata, OggVorbisAudioFormat::id3comment,     "COMMENT");
            addMetadata (metadata, OggVorbisAudioFormat::id3date,        "DATE");
            addMetadata (metadata, OggVorbisAudioFormat::id3genre,       "GENRE");
            addMetadata (metadata, OggVorbisAudioFormat::id3trackNumber, "TRACKNUMBER");

            vorbis_analysis_init (&vd, &vi);
            vorbis_block_init (&vd, &vb);

            ogg_stream_init (&os, Random::getSystemRandom().nextInt());

            ogg_packet header;
            ogg_packet header_comm;
            ogg_packet header_code;

            vorbis_analysis_headerout (&vd, &vc, &header, &header_comm, &header_code);

            ogg_stream_packetin (&os, &header);
            ogg_stream_packetin (&os, &header_comm);
            ogg_stream_packetin (&os, &header_code);

            for (;;)
            {
                if (ogg_stream_flush (&os, &og) == 0)
                    break;

                output->write (og.header, (size_t) og.header_len);
                output->write (og.body,   (size_t) og.body_len);
            }

            ok = true;
        }
    }

    ~OggWriter()
    {
        using namespace OggVorbisNamespace;
        if (ok)
        {
            // write a zero-length packet to show ogg that we're finished..
            writeSamples (0);

            ogg_stream_clear (&os);
            vorbis_block_clear (&vb);
            vorbis_dsp_clear (&vd);
            vorbis_comment_clear (&vc);

            vorbis_info_clear (&vi);
            output->flush();
        }
        else
        {
            vorbis_info_clear (&vi);
            output = nullptr; // to stop the base class deleting this, as it needs to be returned
                              // to the caller of createWriter()
        }
    }

    //==============================================================================
    bool write (const int** samplesToWrite, int numSamples) override
    {
        if (ok)
        {
            using namespace OggVorbisNamespace;

            if (numSamples > 0)
            {
                const double gain = 1.0 / 0x80000000u;
                float** const vorbisBuffer = vorbis_analysis_buffer (&vd, numSamples);

                for (int i = (int) numChannels; --i >= 0;)
                {
                    float* const dst = vorbisBuffer[i];
                    const int* const src = samplesToWrite [i];

                    if (src != nullptr && dst != nullptr)
                    {
                        for (int j = 0; j < numSamples; ++j)
                            dst[j] = (float) (src[j] * gain);
                    }
                }
            }

            writeSamples (numSamples);
        }

        return ok;
    }

    void writeSamples (int numSamples)
    {
        using namespace OggVorbisNamespace;

        vorbis_analysis_wrote (&vd, numSamples);

        while (vorbis_analysis_blockout (&vd, &vb) == 1)
        {
            vorbis_analysis (&vb, 0);
            vorbis_bitrate_addblock (&vb);

            while (vorbis_bitrate_flushpacket (&vd, &op))
            {
                ogg_stream_packetin (&os, &op);

                for (;;)
                {
                    if (ogg_stream_pageout (&os, &og) == 0)
                        break;

                    output->write (og.header, (size_t) og.header_len);
                    output->write (og.body,   (size_t) og.body_len);

                    if (ogg_page_eos (&og))
                        break;
                }
            }
        }
    }

    bool ok;

private:
    OggVorbisNamespace::ogg_stream_state os;
    OggVorbisNamespace::ogg_page og;
    OggVorbisNamespace::ogg_packet op;
    OggVorbisNamespace::vorbis_info vi;
    OggVorbisNamespace::vorbis_comment vc;
    OggVorbisNamespace::vorbis_dsp_state vd;
    OggVorbisNamespace::vorbis_block vb;

    void addMetadata (const StringPairArray& metadata, const char* name, const char* vorbisName)
    {
        const String s (metadata [name]);

        if (s.isNotEmpty())
            vorbis_comment_add_tag (&vc, vorbisName, const_cast<char*> (s.toRawUTF8()));
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OggWriter)
};


//==============================================================================
OggVorbisAudioFormat::OggVorbisAudioFormat()  : AudioFormat (oggFormatName, ".ogg")
{
}

OggVorbisAudioFormat::~OggVorbisAudioFormat()
{
}

Array<int> OggVorbisAudioFormat::getPossibleSampleRates()
{
    const int rates[] = { 8000, 11025, 12000, 16000, 22050, 32000,
                          44100, 48000, 88200, 96000, 176400, 192000 };

    return Array<int> (rates, numElementsInArray (rates));
}

Array<int> OggVorbisAudioFormat::getPossibleBitDepths()
{
    const int depths[] = { 32 };

    return Array<int> (depths, numElementsInArray (depths));
}

bool OggVorbisAudioFormat::canDoStereo()    { return true; }
bool OggVorbisAudioFormat::canDoMono()      { return true; }
bool OggVorbisAudioFormat::isCompressed()   { return true; }

AudioFormatReader* OggVorbisAudioFormat::createReaderFor (InputStream* in, const bool deleteStreamIfOpeningFails)
{
    ScopedPointer<OggReader> r (new OggReader (in));

    if (r->sampleRate > 0)
        return r.release();

    if (! deleteStreamIfOpeningFails)
        r->input = nullptr;

    return nullptr;
}

AudioFormatWriter* OggVorbisAudioFormat::createWriterFor (OutputStream* out,
                                                          double sampleRate,
                                                          unsigned int numChannels,
                                                          int bitsPerSample,
                                                          const StringPairArray& metadataValues,
                                                          int qualityOptionIndex)
{
    if (out == nullptr)
        return nullptr;

    ScopedPointer<OggWriter> w (new OggWriter (out, sampleRate, numChannels,
                                               (unsigned int) bitsPerSample,
                                               qualityOptionIndex, metadataValues));

    return w->ok ? w.release() : nullptr;
}

StringArray OggVorbisAudioFormat::getQualityOptions()
{
    static const char* options[] = { "64 kbps", "80 kbps", "96 kbps", "112 kbps", "128 kbps", "160 kbps",
                                     "192 kbps", "224 kbps", "256 kbps", "320 kbps", "500 kbps", 0 };
    return StringArray (options);
}

int OggVorbisAudioFormat::estimateOggFileQuality (const File& source)
{
    if (FileInputStream* const in = source.createInputStream())
    {
        ScopedPointer<AudioFormatReader> r (createReaderFor (in, true));

        if (r != nullptr)
        {
            const double lengthSecs = r->lengthInSamples / r->sampleRate;
            const int approxBitsPerSecond = (int) (source.getSize() * 8 / lengthSecs);

            const StringArray qualities (getQualityOptions());
            int bestIndex = 0;
            int bestDiff = 10000;

            for (int i = qualities.size(); --i >= 0;)
            {
                const int diff = std::abs (qualities[i].getIntValue() - approxBitsPerSecond);

                if (diff < bestDiff)
                {
                    bestDiff = diff;
                    bestIndex = i;
                }
            }

            return bestIndex;
        }
    }

    return 0;
}

//==============================================================================
#if JUCE_UNIT_TESTS

namespace OggUnitTestData
{
    static const uint8 cello_ogg[] =
    {
        79,103,103,83,0,2,0,0,0,0,0,0,0,0,231,83,55,223,0,0,0,0,137,64,210,123,1,30,1,118,111,114,98,105,115,0,0,0,0,1,34,86,0,0,0,0,0,0,80,195,0,0,0,0,0,0,169,1,79,103,103,83,0,0,0,0,0,0,0,0,0,0,231,83,55,223,1,0,0,0,196,81,145,52,15,146,255,255,255,255,255,
        255,255,255,255,255,255,255,255,224,3,118,111,114,98,105,115,13,0,0,0,76,97,118,102,53,54,46,50,53,46,49,48,49,5,0,0,0,31,0,0,0,101,110,99,111,100,101,114,61,76,97,118,99,53,54,46,50,56,46,49,48,48,32,108,105,98,118,111,114,98,105,115,20,0,0,0,101,110,
        99,111,100,101,100,95,98,121,61,116,114,97,99,107,116,105,111,110,15,0,0,0,100,97,116,101,61,50,48,48,54,45,49,48,45,48,52,15,0,0,0,99,111,100,105,110,103,95,104,105,115,116,111,114,121,61,16,0,0,0,116,105,109,101,95,114,101,102,101,114,101,110,99,101,
        61,48,1,5,118,111,114,98,105,115,36,66,67,86,1,0,64,0,0,24,66,16,42,5,173,99,142,58,200,21,33,140,25,162,160,66,202,41,199,29,66,208,33,163,36,67,136,58,198,53,199,24,99,71,185,100,138,66,201,129,208,144,85,0,0,64,0,0,164,28,87,80,114,73,45,231,156,115,
        163,24,87,204,113,232,32,231,156,115,229,32,103,204,113,9,37,231,156,115,142,57,231,146,114,142,49,231,156,115,163,24,87,14,114,41,45,231,156,115,129,20,71,138,113,167,24,231,156,115,164,28,71,138,113,168,24,231,156,115,109,49,183,146,114,206,57,231,
        156,115,230,32,135,82,114,174,53,231,156,115,164,24,103,14,114,11,37,231,156,115,198,32,103,204,113,235,32,231,156,115,140,53,183,212,114,206,57,231,156,115,206,57,231,156,115,206,57,231,156,115,140,49,231,156,115,206,57,231,156,115,110,49,231,22,115,
        174,57,231,156,115,206,57,231,28,115,206,57,231,156,115,32,52,100,21,0,144,0,0,160,161,40,138,226,40,14,16,26,178,10,0,200,0,0,16,64,113,20,71,145,20,75,177,28,203,209,36,13,8,13,89,5,0,0,1,0,8,0,0,160,72,134,164,72,138,165,88,142,102,105,158,38,122,
        162,40,154,162,42,171,178,105,202,178,44,203,178,235,186,46,16,26,178,10,0,72,0,0,80,81,20,197,112,20,7,8,13,89,5,0,100,0,0,8,96,40,138,163,56,142,228,88,146,165,89,158,7,132,134,172,2,0,128,0,0,4,0,0,80,12,71,177,20,77,241,36,207,242,60,207,243,60,207,
        243,60,207,243,60,207,243,60,207,243,60,207,243,60,13,8,13,89,5,0,32,0,0,0,130,40,100,24,3,66,67,86,1,0,64,0,0,8,33,26,25,67,157,82,18,92,10,22,66,28,17,67,29,66,206,67,169,165,131,224,41,133,37,99,210,83,172,65,8,33,124,239,61,247,222,123,239,129,208,
        144,85,0,0,16,0,0,97,20,56,136,129,199,36,8,33,132,98,20,39,68,113,166,32,8,33,132,229,36,88,202,121,232,36,8,221,131,16,66,184,156,123,203,185,247,222,123,32,52,100,21,0,0,8,0,192,32,132,16,66,8,33,132,16,66,8,41,164,148,82,72,41,166,152,98,138,41,199,
        28,115,204,49,199,32,131,12,50,232,160,147,78,58,201,164,146,78,58,202,36,163,142,82,107,41,181,20,83,76,177,229,22,99,173,181,214,156,115,175,65,41,99,140,49,198,24,99,140,49,198,24,99,140,49,198,24,35,8,13,89,5,0,128,0,0,16,6,25,100,144,65,8,33,132,
        20,82,72,41,166,152,114,204,49,199,28,3,66,67,86,1,0,128,0,0,2,0,0,0,28,69,82,36,71,114,36,71,146,36,201,146,44,73,147,60,203,179,60,203,179,60,77,212,68,77,21,85,213,85,109,215,246,109,95,246,109,223,213,101,223,246,101,219,213,101,93,150,101,221,181,
        109,93,214,93,93,215,117,93,215,117,93,215,117,93,215,117,93,215,117,93,215,129,208,144,85,0,128,4,0,128,142,228,56,142,228,56,142,228,72,142,164,72,10,16,26,178,10,0,144,1,0,16,0,128,163,56,138,227,72,142,228,88,142,37,89,146,38,105,150,103,121,150,
        167,121,154,168,137,30,16,26,178,10,0,0,4,0,16,0,0,0,0,0,128,162,40,138,163,56,142,36,89,150,166,105,158,167,122,162,40,154,170,170,138,166,169,170,170,106,154,166,105,154,166,105,154,166,105,154,166,105,154,166,105,154,166,105,154,166,105,154,166,105,
        154,166,105,154,166,105,154,166,105,2,161,33,171,0,0,9,0,0,29,199,113,28,71,113,28,199,113,36,71,146,36,32,52,100,21,0,32,3,0,32,0,0,67,81,28,69,114,44,199,146,52,75,179,60,203,211,68,207,244,92,81,54,117,83,87,109,32,52,100,21,0,0,8,0,32,0,0,0,0,0,0,
        199,115,60,199,115,60,201,147,60,203,115,60,199,147,60,73,211,52,77,211,52,77,211,52,77,211,52,77,211,52,77,211,52,77,211,52,77,211,52,77,211,52,77,211,52,77,211,52,77,211,52,77,211,52,77,211,52,77,3,66,67,86,2,0,100,0,0,28,197,152,123,82,74,169,206,
        65,72,49,39,103,59,198,28,180,152,155,14,21,66,76,90,45,54,100,136,24,38,173,199,210,41,66,144,163,154,74,200,144,49,138,106,41,165,83,8,41,169,165,148,208,49,198,164,166,214,90,42,165,180,30,8,13,89,17,0,68,1,0,0,8,33,198,16,99,136,49,6,33,131,16,49,
        198,32,116,16,34,198,28,132,12,66,6,33,148,20,74,201,32,132,18,66,73,145,99,12,66,7,33,131,16,82,9,161,100,16,66,41,33,149,2,0,0,2,28,0,0,2,44,132,66,67,86,4,0,113,2,0,8,66,206,33,198,32,68,140,65,8,37,164,20,66,72,41,98,12,66,230,156,148,204,57,41,165,
        148,214,66,41,169,69,140,65,200,156,147,146,57,39,37,148,210,82,41,165,181,80,74,107,165,148,214,66,41,173,181,214,106,77,173,197,26,74,105,45,148,210,90,41,165,181,212,90,141,173,181,26,35,198,32,100,206,73,201,156,147,82,74,105,173,148,210,90,230,28,
        149,14,66,74,29,132,148,74,74,45,150,148,90,204,156,147,210,65,71,165,131,144,82,73,37,182,146,82,140,37,149,216,74,74,49,150,148,98,108,45,198,218,98,172,53,148,210,90,73,37,182,146,82,140,45,182,26,91,140,53,71,140,65,201,156,147,146,57,39,165,148,
        210,90,41,169,181,204,57,41,29,132,148,58,7,37,149,148,98,44,37,181,152,57,39,165,131,144,82,7,33,165,146,82,108,37,165,216,66,41,173,149,148,98,44,37,181,216,98,204,181,181,216,106,40,169,197,146,82,140,37,165,24,91,140,181,182,216,106,236,164,180,22,
        82,137,45,148,210,98,139,177,214,214,90,173,161,148,24,75,74,49,150,148,98,140,49,214,220,98,172,57,148,210,98,73,37,198,146,82,139,45,182,92,91,140,53,167,214,114,109,45,214,220,98,204,53,198,92,123,173,181,231,212,90,173,169,181,90,91,140,53,199,26,
        115,172,181,230,222,65,105,45,148,18,91,40,169,197,214,90,173,45,198,90,67,41,177,149,148,98,44,37,197,216,98,204,181,181,88,115,40,37,198,146,82,140,37,165,24,91,140,181,198,24,115,78,173,213,216,98,204,53,181,86,107,173,181,231,24,107,236,169,181,90,
        91,140,53,183,216,106,173,181,246,94,115,236,181,0,0,128,1,7,0,128,0,19,202,64,161,33,43,1,128,40,0,0,194,24,165,24,131,208,32,164,148,99,16,26,132,148,98,14,66,165,20,99,206,73,169,148,98,204,57,41,153,99,206,65,72,37,99,206,57,8,37,133,16,74,73,37,
        165,16,66,41,37,165,84,0,0,64,129,3,0,64,128,13,154,18,139,3,20,26,178,18,0,8,9,0,32,16,82,138,49,231,32,148,146,82,74,17,66,76,57,6,33,132,82,82,106,45,66,72,41,230,28,132,80,74,74,173,85,76,49,230,28,132,16,74,73,169,181,74,49,198,156,131,16,66,41,
        41,181,150,57,231,28,132,16,74,73,41,165,214,50,230,156,131,16,66,41,41,165,212,90,7,33,132,16,74,41,37,165,214,90,235,32,132,16,66,41,165,164,212,90,107,33,132,16,74,41,165,164,148,90,139,49,132,16,66,41,165,164,146,82,107,49,150,82,74,73,41,165,148,
        82,107,45,198,82,74,41,41,165,148,82,75,173,197,152,82,74,41,165,214,90,107,45,198,24,83,74,41,165,212,90,107,177,197,24,99,106,173,181,214,90,139,49,198,24,107,77,173,181,214,90,139,49,198,24,99,173,5,0,0,28,56,0,0,4,24,65,39,25,85,22,97,163,9,23,30,
        128,66,67,86,4,0,81,0,0,128,49,136,49,196,24,114,142,65,200,160,68,206,49,9,153,132,200,57,71,165,147,146,73,9,161,149,214,50,41,161,149,146,90,228,156,147,210,81,202,168,148,150,66,105,153,164,210,90,104,161,0,0,176,3,7,0,176,3,11,161,208,144,149,0,
        64,30,0,0,129,144,82,140,57,231,28,82,138,49,198,156,115,14,41,165,24,99,206,57,167,24,99,204,57,231,156,83,140,49,230,156,115,206,49,198,156,115,206,57,231,24,99,206,57,231,156,115,206,57,231,156,115,14,66,231,156,115,206,57,7,161,115,206,57,231,32,
        132,208,57,231,156,115,16,66,40,0,0,168,192,1,0,32,192,70,145,205,9,70,130,10,13,89,9,0,164,2,0,0,200,48,230,156,115,82,82,106,148,98,12,66,8,165,164,212,40,197,24,132,16,74,73,41,115,14,66,8,165,164,212,90,198,24,116,18,74,73,169,181,14,66,40,165,164,
        212,90,140,29,132,18,74,73,169,181,24,59,8,165,164,148,82,107,49,118,16,74,73,169,165,214,98,44,165,164,212,90,107,49,214,90,74,73,169,181,214,98,172,53,165,212,90,140,49,214,90,107,74,169,181,24,99,172,181,214,2,0,192,19,28,0,128,10,108,88,29,225,164,
        104,44,176,208,144,149,0,64,6,0,192,16,0,192,1,0,0,3,14,0,0,1,38,148,129,66,67,86,2,0,169,0,0,128,49,140,57,231,28,132,82,26,165,156,131,16,66,41,169,52,74,57,7,33,132,82,82,202,156,147,80,74,41,41,181,150,57,39,165,148,82,82,106,173,131,80,74,74,41,
        181,22,99,7,161,148,148,82,106,45,198,14,66,42,41,181,22,99,141,29,132,82,82,106,45,198,24,67,41,41,181,22,99,140,181,134,82,82,106,45,198,24,107,45,41,181,22,99,141,181,230,90,82,106,45,198,26,107,205,181,0,0,132,6,7,0,176,3,27,86,71,56,41,26,11,44,
        52,100,37,0,144,7,0,64,32,196,24,99,140,57,135,148,98,140,49,231,156,67,74,49,198,152,115,206,49,198,24,115,206,57,231,24,99,140,57,231,156,115,140,49,231,156,115,206,57,198,152,115,206,57,231,28,115,206,57,231,156,115,142,57,231,156,115,206,57,231,156,
        115,206,57,231,156,115,206,57,231,156,115,206,9,0,0,42,112,0,0,8,176,81,100,115,130,145,160,66,67,86,2,0,225,0,0,128,49,140,57,199,24,116,18,82,106,152,130,14,66,8,37,164,208,66,163,152,115,16,66,40,165,164,212,50,232,164,164,84,74,74,173,197,150,57,
        39,165,164,82,82,74,173,197,14,66,74,41,165,212,90,140,49,118,16,82,74,41,165,214,98,140,181,131,80,74,74,45,197,88,99,173,29,132,82,82,106,173,181,24,107,13,165,164,212,90,108,49,214,154,115,40,37,165,214,90,140,177,214,154,75,74,173,197,88,99,173,185,
        230,92,82,106,45,182,88,107,173,53,231,212,90,140,49,214,154,107,206,189,167,214,98,140,177,214,154,115,238,189,0,0,147,7,7,0,168,4,27,103,88,73,58,43,28,13,46,52,100,37,0,144,27,0,128,32,196,152,115,206,65,8,33,132,16,66,8,33,82,138,49,231,32,132,16,
        66,8,33,148,82,74,164,20,99,206,65,8,33,132,16,66,8,33,132,140,49,231,160,131,16,66,8,165,148,82,74,41,25,99,206,65,8,33,132,16,74,40,165,132,18,58,231,160,131,16,66,9,165,148,82,74,41,165,116,206,57,8,33,132,16,74,41,165,148,82,74,233,32,132,16,66,8,
        165,148,82,74,41,165,148,210,65,8,33,132,80,74,41,165,148,82,74,41,37,132,16,66,8,165,148,82,74,41,165,148,82,74,8,33,132,16,74,41,165,148,82,74,41,165,148,16,66,8,165,148,82,74,41,165,148,82,74,41,33,132,16,74,41,165,148,82,74,41,165,148,82,66,8,165,
        148,82,74,41,165,148,82,74,41,165,132,16,74,41,165,148,82,74,41,165,148,82,74,9,161,148,82,74,41,165,148,82,74,41,165,148,18,74,41,165,148,82,74,41,165,148,82,74,41,37,148,82,74,41,165,148,82,74,41,165,148,82,74,40,165,148,82,74,41,165,148,82,74,41,165,
        148,80,74,41,165,148,82,74,41,165,148,82,74,41,161,148,82,74,41,165,148,82,74,41,165,148,82,10,0,0,58,112,0,0,8,48,162,210,66,236,52,227,202,35,112,68,33,195,4,84,104,200,74,0,32,28,0,0,64,4,58,8,33,132,16,66,8,17,115,16,66,8,33,132,16,66,136,152,131,
        16,66,8,33,132,16,66,8,33,132,16,66,8,165,148,82,74,41,165,148,82,74,41,165,148,82,74,41,165,148,82,74,41,165,148,82,74,41,165,148,82,74,41,165,148,82,74,41,165,148,82,74,41,165,148,82,74,41,165,148,82,74,41,165,148,82,74,41,165,148,82,74,41,165,148,
        82,74,41,165,148,82,74,41,165,148,82,74,41,165,148,82,74,41,165,148,82,74,41,165,148,82,74,41,165,148,82,74,41,165,148,82,74,41,165,148,82,74,41,165,20,0,117,153,225,0,24,61,97,227,12,43,73,103,133,163,193,133,134,172,4,0,210,2,0,0,99,24,99,140,41,200,
        164,179,22,99,173,13,99,16,66,7,157,132,20,106,168,37,166,134,49,8,33,116,80,74,74,45,182,88,115,6,161,164,82,74,73,45,198,88,131,205,61,131,80,74,41,165,164,22,99,173,57,23,227,65,72,37,165,212,98,171,181,231,28,140,238,32,148,146,82,74,49,214,154,115,
        238,189,104,208,73,73,169,181,90,115,238,61,7,95,60,8,165,164,214,90,140,61,7,31,140,48,162,148,150,98,172,177,214,28,124,17,70,24,81,74,75,45,198,154,123,205,189,24,99,132,74,41,198,90,123,206,185,231,92,140,17,62,165,22,99,174,185,247,30,124,46,194,
        248,226,98,204,57,247,226,131,15,62,8,97,140,144,49,230,216,115,240,189,23,99,140,15,194,200,92,115,46,194,24,227,139,48,194,248,32,108,173,185,7,95,140,17,70,24,99,124,239,53,248,160,123,49,194,8,35,140,49,194,8,221,115,209,69,248,98,140,49,70,24,95,
        132,1,0,185,17,14,0,136,11,70,18,82,103,25,86,26,113,227,9,24,34,144,66,67,86,1,0,49,0,0,4,49,198,32,164,144,82,74,41,197,24,99,140,49,198,24,99,140,49,198,24,99,140,49,198,156,99,206,57,231,156,0,0,192,4,7,0,128,0,43,216,149,89,90,181,81,220,212,73,
        94,244,65,224,19,58,98,51,50,228,82,42,102,114,34,232,145,26,106,177,18,236,208,10,110,240,2,176,208,144,149,0,0,25,0,0,228,164,148,148,90,45,26,66,202,65,105,53,136,200,32,229,36,197,36,34,99,144,130,210,130,167,144,49,136,73,202,29,99,10,33,5,169,118,
        208,49,133,20,163,26,82,10,153,82,10,106,170,57,134,142,49,168,49,39,225,82,9,165,6,0,0,64,16,0,32,32,36,0,192,0,65,193,12,0,48,56,64,24,57,16,232,8,32,112,104,3,0,12,68,200,76,96,80,8,13,14,50,1,224,1,34,66,42,0,72,76,80,148,46,116,65,8,17,164,139,32,
        139,7,46,156,184,241,196,13,39,116,104,131,0,0,0,0,0,128,0,128,15,0,128,132,2,136,136,102,102,174,194,226,2,35,67,99,131,163,195,227,3,36,68,100,36,0,0,0,0,0,64,0,224,3,0,32,33,1,34,162,153,153,171,176,184,192,200,208,216,224,232,240,248,0,9,17,25,9,
        0,0,0,0,0,0,0,0,0,2,2,2,0,0,0,0,0,1,0,0,0,2,2,79,103,103,83,0,0,128,86,0,0,0,0,0,0,231,83,55,223,2,0,0,0,145,203,205,69,47,79,116,114,112,110,114,129,129,135,135,133,138,130,138,132,132,132,131,130,126,124,130,121,127,122,125,128,123,122,122,132,127,
        125,120,132,122,119,125,124,113,120,122,125,124,117,115,117,228,164,154,5,222,153,66,64,129,227,226,146,178,13,202,30,159,174,235,55,15,138,250,216,215,250,183,175,145,117,169,94,131,31,67,7,57,108,254,219,95,183,251,69,207,153,64,167,83,72,87,225,222,
        176,247,243,31,125,90,88,153,209,230,249,216,91,50,94,231,183,223,233,106,115,46,15,204,2,19,228,158,244,135,68,177,168,123,255,231,156,63,26,107,246,190,126,212,213,50,15,182,101,206,43,70,127,22,146,36,222,62,241,152,63,215,214,150,60,89,126,180,63,
        134,177,226,209,246,172,174,109,122,95,20,197,254,233,74,21,76,181,122,191,17,222,203,233,116,235,142,95,17,21,188,210,146,123,175,172,19,204,178,103,70,61,165,70,122,171,222,247,228,33,174,222,205,29,93,163,196,138,78,68,13,181,172,165,231,224,63,94,
        247,166,183,166,154,44,161,89,63,139,40,69,63,90,247,232,198,126,21,172,210,119,161,118,197,235,80,187,70,153,22,226,85,208,139,200,43,215,181,245,186,21,141,106,61,133,151,170,20,57,103,14,242,233,219,250,199,210,255,78,225,226,203,240,227,244,228,233,
        118,189,126,55,211,151,162,225,239,233,141,211,131,178,161,241,158,194,145,169,55,182,99,111,161,45,91,59,236,162,34,215,122,88,27,47,143,215,134,213,48,9,109,51,124,217,91,31,230,187,2,68,159,133,146,146,40,158,243,180,250,54,21,222,235,104,198,67,7,
        93,251,218,146,183,13,188,157,62,155,94,197,53,226,168,10,100,113,245,54,155,150,74,183,220,116,37,173,165,177,240,118,155,110,35,173,148,142,106,76,79,94,139,122,10,239,98,142,221,159,117,221,170,85,241,105,39,21,166,142,209,171,94,225,85,122,26,150,
        52,217,210,221,199,17,226,52,47,125,163,227,137,109,116,174,97,132,201,190,111,209,163,76,67,5,60,159,105,101,135,255,90,202,164,59,132,17,186,133,144,205,185,153,100,203,109,67,27,189,21,39,230,204,234,66,143,231,157,135,190,108,11,55,38,105,151,86,
        27,219,113,54,214,166,189,127,97,105,155,13,87,168,86,135,215,53,209,173,232,241,232,229,123,136,230,223,97,232,83,163,6,121,92,46,166,14,158,234,136,121,62,159,249,126,17,242,226,126,214,190,233,162,83,248,173,226,134,60,164,239,233,163,197,213,57,7,
        92,161,5,57,40,179,75,136,35,0,211,175,129,63,246,246,164,75,65,214,142,179,248,120,81,41,124,39,34,209,178,10,193,225,155,190,54,91,62,198,213,71,230,29,189,93,76,66,231,125,98,227,67,35,115,95,198,253,250,73,185,179,167,70,241,45,235,163,219,235,211,
        50,174,60,184,94,95,133,167,144,82,49,154,43,122,107,87,225,120,76,179,247,233,93,103,155,162,86,77,89,74,99,188,134,233,249,213,118,233,65,126,124,142,236,186,106,50,37,40,96,0,54,175,247,50,249,114,74,221,70,136,140,40,66,200,86,230,80,41,106,159,191,
        26,2,235,47,215,219,242,206,163,15,195,25,51,136,239,92,108,194,186,232,11,107,156,186,164,124,42,91,225,247,253,209,69,4,138,143,89,119,120,57,41,16,61,148,77,115,250,166,223,180,216,182,98,12,173,41,45,123,199,3,71,45,172,20,82,95,209,204,172,137,113,
        105,136,106,210,20,175,247,132,146,246,213,164,78,160,172,219,13,249,178,232,62,202,90,239,191,179,169,203,154,185,158,122,130,6,0,11,208,95,23,44,180,36,80,136,17,194,244,106,210,223,181,81,60,209,190,101,43,190,140,150,158,89,154,164,35,155,197,136,
        119,249,30,244,65,88,97,137,68,57,248,209,200,251,103,168,247,207,114,1,247,42,107,71,1,173,239,185,49,190,201,182,203,79,127,153,55,166,238,194,225,37,173,115,51,3,50,43,90,170,169,24,231,239,195,121,81,212,178,34,158,43,42,94,199,3,177,31,198,200,219,
        245,155,161,100,186,227,160,88,95,164,121,188,86,79,97,112,47,200,3,0,222,202,66,35,0,20,16,223,124,81,175,19,128,128,153,98,138,14,117,62,127,150,131,111,253,119,229,110,76,135,200,54,197,75,117,38,185,103,106,173,196,165,148,233,27,217,186,86,152,44,
        7,42,10,69,238,118,249,16,72,186,16,157,176,20,231,26,229,180,85,210,213,57,225,239,176,137,53,171,242,234,66,59,181,14,191,166,239,14,146,194,167,47,61,54,175,211,211,11,113,196,195,135,211,172,177,48,92,108,151,207,167,78,15,40,0,130,50,220,169,199,
        221,225,201,19,161,229,23,130,187,55,29,215,243,25,35,30,219,98,29,0,20,144,222,236,25,123,61,59,132,97,2,193,216,152,64,212,190,181,76,95,54,111,43,221,70,18,56,173,246,72,202,190,58,27,253,119,197,48,220,184,169,82,65,54,189,94,207,136,112,255,36,82,
        88,247,242,72,93,21,130,158,198,152,68,181,255,64,183,227,74,147,10,104,169,123,244,136,228,39,149,35,193,142,40,149,20,85,93,210,130,138,126,128,2,76,205,114,133,1,103,210,96,241,222,17,81,200,164,241,252,255,168,153,59,107,179,114,227,133,21,204,249,
        224,128,136,119,236,219,180,117,14,62,219,18,29,0,12,192,205,163,232,53,71,179,96,108,140,16,106,223,23,142,151,30,207,209,135,221,146,22,153,116,53,234,106,21,61,120,142,251,94,120,254,44,26,38,199,220,218,18,196,72,119,236,72,84,122,14,138,194,9,100,
        143,184,203,225,17,130,41,153,58,226,255,204,111,136,189,23,64,167,129,175,58,133,151,168,95,161,187,98,219,226,25,247,134,8,222,137,208,74,35,248,153,91,215,33,183,164,176,41,125,26,147,87,84,159,253,251,95,106,214,206,42,210,141,214,113,45,153,201,
        123,34,254,3,30,235,98,13,0,26,72,222,188,28,43,158,97,193,4,51,102,52,241,38,221,191,37,150,141,167,214,108,70,27,69,158,81,59,49,135,140,114,87,57,100,148,247,52,217,102,162,144,135,186,50,85,16,105,36,32,12,80,200,36,99,160,160,69,212,198,6,4,0,110,
        142,177,107,171,46,51,175,180,28,52,22,13,27,179,107,130,66,166,204,89,231,11,74,88,195,183,192,24,22,189,20,159,165,58,16,197,78,114,45,123,42,165,67,243,90,19,112,86,186,138,181,208,130,177,16,211,125,167,255,169,30,235,115,55,125,157,207,1,30,219,
        34,13,0,10,96,237,207,99,83,100,19,75,78,18,140,77,33,132,207,187,68,169,110,199,122,221,227,42,115,234,247,9,51,107,76,221,180,97,217,22,55,17,211,53,149,141,44,197,46,40,4,0,148,148,59,150,149,81,116,221,28,42,107,109,128,60,230,154,188,165,168,160,
        114,14,61,228,233,245,15,217,125,89,82,197,244,88,206,167,72,34,209,251,183,225,127,90,253,187,176,194,240,88,19,14,149,125,45,243,126,40,222,189,85,30,35,195,26,254,143,152,2,193,186,179,4,179,214,84,222,234,66,29,0,56,72,222,124,79,226,33,33,97,102,
        10,51,90,14,109,220,174,184,39,229,202,101,123,67,17,179,26,162,182,245,204,53,124,42,14,161,189,170,211,42,54,92,95,229,174,27,247,169,161,149,210,242,65,0,255,33,82,122,247,46,56,32,2,24,93,103,11,190,247,94,134,104,23,160,164,137,199,66,117,40,119,
        248,190,8,61,10,8,246,40,85,77,48,253,185,77,11,82,57,208,102,109,180,59,234,157,190,210,229,237,161,23,221,158,182,194,51,142,50,239,104,97,194,142,103,63,170,177,113,118,213,84,211,22,126,202,130,25,0,0,108,93,79,92,164,125,16,112,136,146,198,20,33,
        196,57,218,156,44,184,109,177,209,136,168,221,179,65,235,60,145,77,233,62,171,56,154,1,115,171,134,190,218,22,134,160,248,95,217,95,100,34,81,102,250,24,87,175,237,219,215,74,183,202,87,254,157,23,247,175,168,62,143,201,104,100,156,117,230,211,153,85,
        122,123,30,50,231,165,21,135,55,33,90,99,123,86,210,203,244,13,159,93,103,44,229,192,208,157,150,203,6,135,188,185,84,190,213,108,19,137,209,16,98,21,126,213,3,254,185,130,17,0,0,172,231,63,145,91,50,35,132,67,130,133,162,24,35,251,248,65,189,147,226,
        146,166,237,22,98,149,165,167,115,198,84,78,127,178,73,201,186,163,119,11,221,116,63,30,81,110,88,48,66,125,86,139,79,59,18,0,193,29,17,117,69,51,106,156,154,117,195,189,23,49,70,64,107,92,173,182,122,239,65,189,130,68,248,159,76,103,241,200,186,154,
        47,103,70,44,184,229,45,161,195,26,23,213,174,166,198,249,174,103,79,246,224,2,142,220,71,247,112,183,93,51,133,151,131,243,221,0,158,169,130,1,0,0,172,61,187,254,40,39,82,10,7,41,48,161,16,166,104,139,160,157,50,58,183,120,229,139,117,74,68,237,187,
        95,173,77,226,102,4,97,215,217,152,238,86,156,97,239,244,226,169,196,157,124,103,255,140,155,89,143,189,77,168,208,193,165,251,244,67,101,75,95,26,169,221,67,44,129,242,151,48,170,152,205,164,47,55,132,72,154,189,141,70,246,154,182,203,252,47,231,78,
        179,221,104,129,252,65,120,161,22,49,131,250,241,214,67,231,83,214,137,143,161,31,74,93,100,155,35,23,1,222,169,82,35,0,0,88,219,175,190,65,88,101,11,48,65,17,66,212,156,218,143,254,66,152,146,197,146,178,27,41,23,155,168,155,244,23,166,236,187,181,152,
        137,247,54,127,255,178,52,132,183,91,20,50,51,3,209,83,221,246,45,164,149,34,136,196,109,242,139,189,137,167,62,31,228,200,157,84,187,238,239,64,168,213,242,97,229,99,77,246,126,206,75,231,177,255,18,71,57,176,241,241,156,124,71,241,190,67,172,57,243,
        181,225,79,251,109,182,46,148,232,119,2,20,226,187,244,188,62,61,62,1,126,137,82,13,0,0,172,253,20,30,43,159,135,18,33,5,39,20,66,8,183,188,92,86,62,22,189,130,246,42,252,150,12,113,113,220,210,90,216,84,42,169,202,26,251,21,29,108,212,26,177,138,23,
        213,136,24,5,209,205,13,131,184,65,163,1,161,132,160,101,189,154,169,111,207,150,205,125,81,215,69,21,228,177,101,26,147,135,214,46,41,199,88,11,233,147,183,151,151,131,75,92,87,209,62,134,207,168,38,183,221,209,127,158,211,222,115,213,190,213,126,117,
        78,148,26,82,79,65,42,95,1,126,137,50,41,0,20,192,230,101,100,231,248,146,201,24,24,81,138,17,138,123,95,35,86,116,42,154,210,251,30,229,35,54,55,138,172,137,228,40,106,140,255,194,109,109,59,242,178,52,122,76,227,251,158,163,120,103,166,153,235,174,
        209,44,101,117,207,253,30,108,84,39,133,104,2,71,24,165,210,171,232,95,165,15,216,168,12,199,177,23,226,162,119,240,100,57,48,5,65,214,150,74,36,167,101,99,108,175,127,198,252,243,183,126,156,78,103,185,54,135,7,66,195,184,213,137,0,62,137,82,3,0,0,88,
        251,38,26,59,33,68,23,6,233,172,16,98,180,11,193,152,164,117,214,86,166,64,48,223,21,39,179,73,3,232,104,172,160,199,115,156,38,227,219,225,53,117,251,55,182,94,218,189,160,171,207,189,173,170,150,119,173,90,89,239,93,143,243,44,206,111,165,171,58,18,
        212,65,166,89,78,191,221,176,206,108,82,132,68,77,47,195,194,65,161,19,174,156,8,32,109,20,232,126,89,146,37,105,74,10,73,155,92,126,251,123,74,153,118,75,83,231,2,254,120,82,3,0,0,216,252,45,90,20,121,23,109,65,182,100,172,16,69,168,61,159,172,187,118,
        196,37,212,221,243,46,119,77,249,66,56,155,165,140,162,142,175,191,246,117,253,86,238,229,105,230,213,73,214,210,91,238,158,10,34,27,126,255,204,240,89,170,192,60,205,46,6,244,170,46,17,80,109,166,116,103,217,126,239,227,198,180,106,119,55,130,210,137,
        167,15,4,80,176,29,42,210,242,146,26,106,210,74,217,178,10,255,118,206,79,127,90,55,14,171,138,193,148,147,59,231,63,196,91,0,254,120,114,29,0,0,216,156,136,172,29,222,140,208,32,8,133,16,70,195,187,32,234,247,218,230,20,56,194,247,22,209,219,166,193,
        99,84,97,5,177,251,92,38,62,178,154,173,15,94,148,108,140,35,51,148,250,210,59,90,43,191,61,130,227,229,189,129,213,75,218,141,42,0,82,14,34,21,199,219,120,52,10,245,230,55,139,239,41,140,9,155,198,13,206,61,171,218,193,171,155,80,98,217,38,215,145,25,
        15,140,232,229,218,119,130,26,142,57,95,58,67,4,254,120,74,2,112,181,52,32,185,249,109,41,118,86,145,150,13,198,136,98,132,246,88,210,253,152,223,184,153,191,213,231,122,78,162,139,2,157,101,137,147,130,176,245,47,146,139,187,24,81,180,144,222,176,4,
        38,177,123,142,66,146,133,49,231,128,99,145,104,215,115,200,100,186,85,216,188,234,49,56,161,95,75,199,134,184,105,82,13,231,36,187,201,48,124,127,172,244,70,226,45,110,130,228,108,231,196,180,51,199,155,30,185,115,157,19,79,62,123,54,237,29,235,116,
        175,182,24,0,158,88,82,13,0,0,244,253,39,115,12,134,17,66,8,9,71,161,79,230,129,182,53,59,26,42,14,226,63,108,221,40,176,204,34,104,65,48,111,188,22,208,124,152,91,73,195,60,121,239,121,17,99,236,69,113,124,250,141,209,147,244,107,125,174,15,124,111,
        59,161,166,13,115,187,151,70,124,153,158,165,124,8,74,37,164,201,84,86,109,29,10,81,86,43,87,40,55,58,59,222,126,176,249,123,127,50,173,215,83,170,143,179,188,71,28,181,62,67,169,27,187,11,94,104,18,29,3,0,224,248,187,187,231,194,231,96,34,2,20,192,152,
        34,132,80,139,162,184,47,117,232,1,153,141,182,180,13,147,68,246,21,33,34,212,38,53,133,22,164,159,140,200,243,217,105,41,243,157,178,101,145,233,46,164,84,208,218,9,167,131,46,45,58,217,161,172,245,155,174,202,148,138,126,111,230,214,220,207,172,255,
        34,119,102,14,173,167,140,177,134,23,130,202,151,158,146,15,147,132,249,156,109,251,237,110,222,106,134,155,70,40,135,206,250,254,207,143,227,26,254,87,34,3,0,0,216,220,42,126,249,236,51,130,0,198,20,97,140,114,121,181,153,46,79,187,59,27,168,125,189,
        213,50,170,78,76,147,233,171,81,214,134,177,242,169,118,191,223,204,202,115,221,37,188,72,211,207,230,111,48,26,109,126,17,198,198,97,100,119,229,99,25,126,144,191,182,231,72,150,201,221,100,109,205,70,110,181,213,42,154,221,53,139,70,90,245,109,130,
        238,138,70,153,112,164,172,183,90,222,165,177,100,158,176,50,6,107,73,195,119,150,186,71,144,140,216,201,103,0,62,72,82,29,0,0,232,223,117,12,4,194,8,35,20,254,253,102,243,145,37,117,152,51,83,26,50,53,133,11,89,191,162,244,157,122,41,186,48,40,117,95,
        149,159,155,96,34,143,123,51,113,122,211,136,113,156,177,194,51,30,75,141,235,239,115,115,60,223,59,169,253,187,215,149,136,66,254,70,153,236,212,59,61,230,113,173,58,198,163,35,164,99,164,26,174,200,61,45,7,17,133,242,216,29,7,117,133,77,141,73,231,
        82,186,82,218,144,111,101,207,186,173,193,52,222,103,98,29,0,0,88,219,175,78,57,5,132,28,70,99,24,33,132,144,233,91,244,250,133,109,183,9,246,88,108,216,79,179,138,171,163,101,136,138,27,125,214,73,94,123,191,26,165,135,209,191,142,117,137,201,194,30,
        231,33,51,61,57,107,39,5,53,152,181,250,167,215,155,110,147,86,55,221,153,21,210,223,211,219,226,177,161,208,74,253,91,42,193,93,105,52,134,54,242,241,155,34,21,169,221,230,198,195,124,163,233,148,171,59,152,173,221,200,56,36,164,45,222,87,18,13,0,0,
        108,142,186,192,174,31,178,229,24,12,81,132,17,50,230,236,214,150,168,48,200,71,245,245,236,39,47,155,163,139,82,70,1,196,77,221,138,212,201,208,20,245,179,126,72,245,250,42,57,53,27,238,191,50,106,38,238,191,35,61,229,96,37,108,196,248,190,169,147,52,
        210,99,220,208,74,94,255,223,149,240,168,225,247,201,251,151,230,181,38,173,175,38,195,146,133,55,197,20,58,198,184,235,184,143,146,15,163,87,68,251,246,152,76,119,100,23,222,71,50,13,0,0,108,174,203,35,247,203,1,70,11,17,83,68,17,138,222,150,29,142,
        123,187,117,158,148,207,66,213,236,112,11,209,74,64,72,65,217,179,190,27,210,48,136,109,201,98,139,154,70,63,205,1,231,235,158,220,216,200,253,142,108,47,145,123,0,61,199,180,230,213,189,133,205,113,175,177,191,212,182,119,112,8,127,51,43,51,90,175,239,
        151,196,132,112,169,229,217,69,254,53,79,147,97,247,193,202,63,234,241,150,239,43,249,182,187,122,211,35,12,223,31,238,60,150,94,235,227,32,1,190,87,114,37,0,0,88,113,169,190,30,162,71,33,5,6,67,20,33,132,98,77,230,150,39,187,168,73,7,19,253,246,165,
        195,175,48,159,21,50,93,125,9,216,150,216,110,54,42,129,161,133,149,129,239,44,97,123,160,51,219,252,161,85,198,120,63,118,191,109,199,224,149,76,38,152,153,210,187,250,253,100,255,107,117,210,124,98,58,179,156,235,151,209,56,134,23,229,108,109,86,66,
        115,66,109,174,41,169,148,151,84,178,215,8,173,95,228,84,42,253,48,196,104,98,26,221,50,0,222,71,10,37,0,0,88,43,215,7,29,93,67,160,53,140,48,66,8,153,209,150,84,157,237,196,145,197,100,244,71,243,50,110,250,132,246,136,84,124,156,128,61,131,97,90,43,
        150,181,117,101,211,75,9,214,155,223,93,135,231,185,106,55,154,105,227,212,179,15,234,241,83,53,181,159,157,0,75,134,118,99,182,49,145,231,58,61,143,73,167,255,140,247,251,184,227,79,102,5,166,83,83,103,29,103,161,81,115,19,6,78,127,244,120,220,175,225,
        77,63,196,48,164,205,141,1,158,87,10,41,0,0,88,123,80,190,74,62,169,209,6,166,148,34,138,12,165,37,57,234,235,218,176,162,118,254,54,163,42,90,93,140,45,1,51,204,168,132,13,43,252,8,183,180,28,38,185,157,193,41,73,54,232,53,253,163,107,160,172,123,61,
        226,227,6,97,240,74,23,13,177,30,84,12,157,214,36,14,142,24,16,81,166,172,155,70,25,214,197,205,255,109,93,206,86,177,14,111,212,81,43,145,72,31,154,137,191,197,51,223,134,239,202,59,221,20,30,126,71,10,9,0,0,216,60,15,5,202,37,186,200,14,96,204,8,97,
        20,107,110,209,26,165,178,208,122,70,33,237,157,6,140,213,149,34,71,77,202,70,153,92,234,183,133,142,31,140,185,110,150,241,196,52,237,80,205,108,152,122,187,207,108,17,211,145,162,187,233,121,227,149,206,61,100,45,170,235,52,21,16,79,165,234,153,96,
        180,181,107,180,166,107,105,126,38,222,92,110,242,85,76,101,114,86,179,215,120,63,39,55,164,191,41,113,36,23,8,214,173,159,239,45,7,50,232,189,216,197,195,27,6,158,55,74,28,0,0,232,191,103,5,24,83,70,8,5,221,219,63,161,52,230,253,182,206,35,133,32,202,
        246,177,229,140,44,56,234,134,215,181,60,92,98,197,82,76,199,236,42,62,26,132,138,186,230,95,214,166,58,83,232,147,25,125,136,186,34,41,217,179,231,77,170,185,56,50,47,154,8,222,197,52,213,19,128,215,195,54,175,199,117,210,251,229,86,53,155,205,180,74,
        216,184,78,183,14,146,239,214,85,155,5,147,173,20,158,53,208,218,215,235,180,166,110,0,126,87,74,18,224,250,0,176,121,87,93,57,10,35,12,48,98,68,17,54,85,184,61,58,126,99,146,20,34,106,94,166,83,70,14,54,131,9,37,207,40,26,249,204,135,244,44,76,245,156,
        70,163,192,218,208,236,206,114,239,103,240,62,174,213,138,149,194,86,13,103,204,236,233,237,45,109,150,41,14,221,158,155,155,82,43,250,150,202,179,42,120,82,147,80,106,189,53,149,112,75,94,145,83,243,123,168,25,218,191,100,195,168,59,37,77,89,132,0,13,
        62,71,74,28,0,0,232,231,129,49,40,140,49,66,72,110,117,166,80,222,70,253,66,199,135,92,146,241,179,113,245,15,87,145,166,9,24,180,214,216,120,215,126,8,33,138,244,62,79,39,109,26,41,6,103,235,24,210,102,37,14,222,49,61,104,8,113,191,241,139,235,235,153,
        215,36,251,193,208,61,70,238,156,210,208,57,13,15,169,108,172,219,100,127,195,190,56,11,254,244,163,142,151,97,22,14,156,110,213,248,210,249,147,38,175,234,62,149,233,201,30,123,89,231,86,14,158,86,82,1,0,0,92,241,108,226,10,52,204,152,82,132,70,95,222,
        1,171,105,236,149,43,213,3,245,116,90,249,7,146,24,205,222,187,30,21,209,167,109,4,217,244,81,149,255,42,13,77,196,124,205,112,213,154,215,8,104,139,212,142,25,137,161,134,51,204,26,155,10,159,149,54,237,130,218,28,134,88,13,200,102,179,195,81,158,147,
        138,91,110,168,91,133,15,136,245,189,152,60,175,215,90,198,236,165,132,204,135,87,55,102,11,66,82,38,215,212,208,116,146,145,1,62,39,106,32,1,0,232,207,139,86,146,143,33,132,16,197,124,59,162,66,99,90,156,216,93,181,251,201,183,110,232,198,228,46,86,
        239,81,3,162,177,79,33,27,216,182,16,171,58,52,164,14,62,192,189,173,231,186,239,42,185,117,197,160,10,187,245,8,149,98,159,114,183,74,16,164,148,13,98,125,84,166,195,235,16,58,206,242,185,33,171,128,20,148,66,112,248,9,43,51,186,255,205,165,201,78,94,
        147,232,118,229,50,181,249,62,87,42,141,2,0,64,191,187,37,48,69,148,98,100,222,122,188,228,170,203,117,55,155,70,58,108,63,138,135,60,248,98,167,18,77,80,60,118,17,71,36,166,155,148,115,133,144,41,78,71,243,134,22,62,176,182,149,238,106,163,247,181,158,
        95,199,199,248,54,164,179,24,190,104,19,173,49,114,30,124,63,206,214,115,121,159,77,215,168,31,86,134,130,62,29,154,124,21,243,15,83,214,93,237,129,1,240,136,60,26,126,27,173,198,202,118,82,73,142,0,158,55,90,131,2,0,38,244,239,175,128,66,8,33,140,130,
        174,94,70,148,123,58,62,78,43,44,203,54,206,103,54,195,140,178,199,196,27,46,189,112,121,173,137,198,102,59,23,9,237,79,221,239,155,210,202,38,47,189,110,188,147,239,164,237,35,110,10,143,233,76,43,205,198,154,183,182,121,194,89,202,206,59,101,250,203,
        21,119,89,107,143,235,142,201,208,72,225,119,95,233,213,184,250,217,128,48,67,59,51,236,50,11,106,136,222,115,254,207,146,249,113,119,1,158,22,42,1,0,0,244,223,67,192,24,97,132,80,104,92,14,140,71,103,198,14,25,40,162,193,111,201,153,38,153,120,136,66,
        220,162,84,190,180,17,150,56,212,133,87,108,220,251,177,66,143,91,237,182,117,80,247,188,132,235,79,236,253,80,154,23,22,72,61,10,185,254,196,191,150,70,99,115,216,147,94,86,114,252,61,8,56,100,76,221,98,209,242,158,242,60,198,219,124,191,39,220,183,
        243,37,57,181,103,27,198,107,170,236,170,165,109,175,49,255,208,146,254,199,67,59,126,102,42,17,0,56,96,237,199,234,5,225,12,2,20,69,8,35,220,180,90,61,53,245,204,205,245,198,242,108,46,45,96,9,225,46,3,248,28,143,116,73,44,226,83,225,62,104,181,59,174,
        32,148,70,5,200,24,134,49,93,242,122,125,91,63,172,91,219,245,243,203,89,25,73,103,122,204,248,204,44,197,216,253,210,200,175,205,243,176,78,219,116,99,13,34,76,93,183,38,128,136,33,78,151,144,223,87,238,217,187,49,217,124,175,164,121,29,27,112,164,24,
        56,85,17,126,55,90,192,2,0,11,214,50,60,122,8,147,21,163,97,132,16,66,8,226,143,127,170,103,181,83,197,164,205,90,121,73,86,210,114,29,52,176,168,17,138,21,230,215,169,216,243,171,4,168,198,61,57,15,39,117,118,163,25,83,218,28,215,112,247,156,40,175,
        154,233,51,215,116,99,4,123,98,55,198,75,26,233,149,218,98,132,146,134,26,245,74,85,163,243,162,235,249,111,243,244,100,138,154,159,164,183,63,245,143,198,240,234,120,114,163,1,94,55,154,193,2,0,15,107,79,179,219,33,59,49,18,70,8,81,132,152,61,144,189,
        120,34,183,232,187,152,236,95,18,247,106,43,132,160,123,88,117,238,73,218,203,197,156,33,217,125,125,57,59,196,234,238,227,171,191,123,188,145,111,161,110,8,234,78,44,91,62,175,189,229,204,217,48,79,253,221,27,11,208,106,148,141,49,62,139,253,181,11,
        215,201,203,216,99,189,172,176,175,208,27,221,216,157,143,213,57,247,210,171,227,247,170,0,94,55,154,193,2,0,11,122,115,22,160,148,34,132,48,143,149,55,185,92,86,10,210,110,104,151,114,25,195,8,85,221,230,101,118,73,48,154,62,240,30,235,102,129,137,109,
        216,212,239,77,28,75,8,187,149,195,142,68,204,73,163,97,213,122,177,2,70,49,98,178,161,182,31,82,107,53,229,62,172,91,98,239,247,8,22,238,51,189,188,227,29,171,17,217,245,95,98,207,245,184,207,110,83,251,199,75,102,140,70,46,37,7,166,130,176,119,114,
        79,103,103,83,0,4,62,89,0,0,0,0,0,0,231,83,55,223,3,0,0,0,69,62,165,153,2,126,98,126,55,218,128,13,0,14,134,243,236,96,8,33,68,145,28,111,45,154,89,79,253,102,176,252,212,142,80,179,120,194,36,140,41,26,92,50,81,178,196,24,50,163,232,60,80,22,213,202,
        141,223,38,42,144,140,178,166,183,29,125,100,28,179,241,238,61,251,60,206,238,122,164,126,126,54,185,249,96,243,117,252,218,63,163,28,119,15,173,111,140,179,165,87,166,163,206,121,235,168,28,122,107,4,231,146,67,9,45,150,111,247,217,76,101,27,147,173,
        159,122,114,103,197,155,19,79,0,126,119,249,0,127,0,112,16,25,33,132,96,48,82,90,126,130,118,98,253,40,220,16,182,254,103,51,122,0,198,218,101,201,139,255,33,225,177,81,219,43,127,115,75,214,107,135,100,114,246,49,76,212,57,1,217,118,253,227,59,55,209,
        13,167,190,108,154,248,37,55,177,3,253,47,203,93,217,118,39,62,230,100,219,29,163,21,59,0,172,71,142,128,48,241,109,226,13,89,0,0
    };

    static const int cello_oggSize = numElementsInArray (cello_ogg);
}

class OggAudioFormatUnitTests : public AudioFormatUnitTests<OggVorbisAudioFormat>
{
public:
    OggAudioFormatUnitTests() {}

    InputStream* getValidTestFileData() override
    {
        return new MemoryInputStream (OggUnitTestData::cello_ogg, OggUnitTestData::cello_oggSize, false);
    }

    StringPairArray createTestMetadata() override
    {
        StringPairArray metadataValues;

        static const char* const props[] =
        {
            OggVorbisAudioFormat::id3title,
            OggVorbisAudioFormat::id3artist,
            OggVorbisAudioFormat::id3album,
            OggVorbisAudioFormat::id3comment,
            OggVorbisAudioFormat::id3date,
            OggVorbisAudioFormat::id3genre,
            OggVorbisAudioFormat::id3trackNumber
        };

        for (int i = numElementsInArray (props); --i >= 0;)
            metadataValues.set (props[i], props[i]);

        return metadataValues;
    }

private:
    JUCE_DECLARE_NON_COPYABLE (OggAudioFormatUnitTests)
};

static const OggAudioFormatUnitTests oggAudioFormatUnitTests;

#endif

#endif
