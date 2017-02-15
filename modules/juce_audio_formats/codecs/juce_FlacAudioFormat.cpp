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

#if JUCE_USE_FLAC

}

#if defined _WIN32 && !defined __CYGWIN__
 #include <io.h>
#else
 #include <unistd.h>
#endif

#if defined _MSC_VER || defined __BORLANDC__ || defined __MINGW32__
 #include <sys/types.h> /* for off_t */
#endif

#if HAVE_INTTYPES_H
 #define __STDC_FORMAT_MACROS
 #include <inttypes.h>
#endif

#if defined _MSC_VER || defined __MINGW32__ || defined __CYGWIN__ || defined __EMX__
 #include <io.h> /* for _setmode(), chmod() */
 #include <fcntl.h> /* for _O_BINARY */
#else
 #include <unistd.h> /* for chown(), unlink() */
#endif

#if defined _MSC_VER || defined __BORLANDC__ || defined __MINGW32__
 #if defined __BORLANDC__
  #include <utime.h> /* for utime() */
 #else
  #include <sys/utime.h> /* for utime() */
 #endif
#else
 #include <sys/types.h> /* some flavors of BSD (like OS X) require this to get time_t */
 #include <utime.h> /* for utime() */
#endif

#if defined _MSC_VER
 #if _MSC_VER >= 1600
  #include <stdint.h>
 #else
  #include <limits.h>
 #endif
#endif

#ifdef _WIN32
 #include <stdio.h>
 #include <sys/stat.h>
 #include <stdarg.h>
 #include <windows.h>
#endif

#ifdef DEBUG
 #include <assert.h>
#endif

#include <stdlib.h>
#include <stdio.h>

namespace juce
{

namespace FlacNamespace
{
#if JUCE_INCLUDE_FLAC_CODE || ! defined (JUCE_INCLUDE_FLAC_CODE)

 #undef VERSION
 #define VERSION "1.3.1"

 #define FLAC__NO_DLL 1

 #if JUCE_MSVC
  #pragma warning (disable: 4267 4127 4244 4996 4100 4701 4702 4013 4133 4206 4312 4505 4365 4005 4334 181 111)
 #else
  #define HAVE_LROUND 1
 #endif

 #if JUCE_MAC
  #define FLAC__SYS_DARWIN 1
 #endif

 #ifndef SIZE_MAX
  #define SIZE_MAX 0xffffffff
 #endif

 #if JUCE_CLANG
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wconversion"
  #pragma clang diagnostic ignored "-Wshadow"
  #pragma clang diagnostic ignored "-Wdeprecated-register"
 #endif

 #if JUCE_INTEL
  #if JUCE_32BIT
   #define FLAC__CPU_IA32 1
  #endif
  #if JUCE_64BIT
   #define FLAC__CPU_X86_64 1
  #endif
  #define FLAC__HAS_X86INTRIN 1
 #endif

 #undef __STDC_LIMIT_MACROS
 #define __STDC_LIMIT_MACROS 1
 #define flac_max jmax
 #define flac_min jmin
 #undef DEBUG // (some flac code dumps debug trace if the app defines this macro)
 #include "flac/all.h"
 #include "flac/libFLAC/bitmath.c"
 #include "flac/libFLAC/bitreader.c"
 #include "flac/libFLAC/bitwriter.c"
 #include "flac/libFLAC/cpu.c"
 #include "flac/libFLAC/crc.c"
 #include "flac/libFLAC/fixed.c"
 #include "flac/libFLAC/float.c"
 #include "flac/libFLAC/format.c"
 #include "flac/libFLAC/lpc_flac.c"
 #include "flac/libFLAC/md5.c"
 #include "flac/libFLAC/memory.c"
 #include "flac/libFLAC/stream_decoder.c"
 #include "flac/libFLAC/stream_encoder.c"
 #include "flac/libFLAC/stream_encoder_framing.c"
 #include "flac/libFLAC/window_flac.c"
 #undef VERSION
#else
 #include <FLAC/all.h>
#endif

 #if JUCE_CLANG
  #pragma clang diagnostic pop
 #endif
}

#undef max
#undef min

//==============================================================================
static const char* const flacFormatName = "FLAC file";


//==============================================================================
class FlacReader  : public AudioFormatReader
{
public:
    FlacReader (InputStream* const in)
        : AudioFormatReader (in, flacFormatName),
          reservoirStart (0),
          samplesInReservoir (0),
          scanningForLength (false)
    {
        using namespace FlacNamespace;
        lengthInSamples = 0;

        decoder = FLAC__stream_decoder_new();

        ok = FLAC__stream_decoder_init_stream (decoder,
                                               readCallback_, seekCallback_, tellCallback_, lengthCallback_,
                                               eofCallback_, writeCallback_, metadataCallback_, errorCallback_,
                                               this) == FLAC__STREAM_DECODER_INIT_STATUS_OK;

        if (ok)
        {
            FLAC__stream_decoder_process_until_end_of_metadata (decoder);

            if (lengthInSamples == 0 && sampleRate > 0)
            {
                // the length hasn't been stored in the metadata, so we'll need to
                // work it out the length the hard way, by scanning the whole file..
                scanningForLength = true;
                FLAC__stream_decoder_process_until_end_of_stream (decoder);
                scanningForLength = false;
                const int64 tempLength = lengthInSamples;

                FLAC__stream_decoder_reset (decoder);
                FLAC__stream_decoder_process_until_end_of_metadata (decoder);
                lengthInSamples = tempLength;
            }
        }
    }

    ~FlacReader()
    {
        FlacNamespace::FLAC__stream_decoder_delete (decoder);
    }

    void useMetadata (const FlacNamespace::FLAC__StreamMetadata_StreamInfo& info)
    {
        sampleRate = info.sample_rate;
        bitsPerSample = info.bits_per_sample;
        lengthInSamples = (unsigned int) info.total_samples;
        numChannels = info.channels;

        reservoir.setSize ((int) numChannels, 2 * (int) info.max_blocksize, false, false, true);
    }

    // returns the number of samples read
    bool readSamples (int** destSamples, int numDestChannels, int startOffsetInDestBuffer,
                      int64 startSampleInFile, int numSamples) override
    {
        using namespace FlacNamespace;

        if (! ok)
            return false;

        while (numSamples > 0)
        {
            if (startSampleInFile >= reservoirStart
                 && startSampleInFile < reservoirStart + samplesInReservoir)
            {
                const int num = (int) jmin ((int64) numSamples,
                                            reservoirStart + samplesInReservoir - startSampleInFile);

                jassert (num > 0);

                for (int i = jmin (numDestChannels, reservoir.getNumChannels()); --i >= 0;)
                    if (destSamples[i] != nullptr)
                        memcpy (destSamples[i] + startOffsetInDestBuffer,
                                reservoir.getReadPointer (i, (int) (startSampleInFile - reservoirStart)),
                                sizeof (int) * (size_t) num);

                startOffsetInDestBuffer += num;
                startSampleInFile += num;
                numSamples -= num;
            }
            else
            {
                if (startSampleInFile >= (int) lengthInSamples)
                {
                    samplesInReservoir = 0;
                }
                else if (startSampleInFile < reservoirStart
                          || startSampleInFile > reservoirStart + jmax (samplesInReservoir, 511))
                {
                    // had some problems with flac crashing if the read pos is aligned more
                    // accurately than this. Probably fixed in newer versions of the library, though.
                    reservoirStart = (int) (startSampleInFile & ~511);
                    samplesInReservoir = 0;
                    FLAC__stream_decoder_seek_absolute (decoder, (FLAC__uint64) reservoirStart);
                }
                else
                {
                    reservoirStart += samplesInReservoir;
                    samplesInReservoir = 0;
                    FLAC__stream_decoder_process_single (decoder);
                }

                if (samplesInReservoir == 0)
                    break;
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

    void useSamples (const FlacNamespace::FLAC__int32* const buffer[], int numSamples)
    {
        if (scanningForLength)
        {
            lengthInSamples += numSamples;
        }
        else
        {
            if (numSamples > reservoir.getNumSamples())
                reservoir.setSize ((int) numChannels, numSamples, false, false, true);

            const unsigned int bitsToShift = 32 - bitsPerSample;

            for (int i = 0; i < (int) numChannels; ++i)
            {
                const FlacNamespace::FLAC__int32* src = buffer[i];

                int n = i;
                while (src == 0 && n > 0)
                    src = buffer [--n];

                if (src != nullptr)
                {
                    int* const dest = reinterpret_cast<int*> (reservoir.getWritePointer(i));

                    for (int j = 0; j < numSamples; ++j)
                        dest[j] = src[j] << bitsToShift;
                }
            }

            samplesInReservoir = numSamples;
        }
    }

    //==============================================================================
    static FlacNamespace::FLAC__StreamDecoderReadStatus readCallback_ (const FlacNamespace::FLAC__StreamDecoder*, FlacNamespace::FLAC__byte buffer[], size_t* bytes, void* client_data)
    {
        using namespace FlacNamespace;
        *bytes = (size_t) static_cast<const FlacReader*> (client_data)->input->read (buffer, (int) *bytes);
        return FLAC__STREAM_DECODER_READ_STATUS_CONTINUE;
    }

    static FlacNamespace::FLAC__StreamDecoderSeekStatus seekCallback_ (const FlacNamespace::FLAC__StreamDecoder*, FlacNamespace::FLAC__uint64 absolute_byte_offset, void* client_data)
    {
        using namespace FlacNamespace;
        static_cast<const FlacReader*> (client_data)->input->setPosition ((int) absolute_byte_offset);
        return FLAC__STREAM_DECODER_SEEK_STATUS_OK;
    }

    static FlacNamespace::FLAC__StreamDecoderTellStatus tellCallback_ (const FlacNamespace::FLAC__StreamDecoder*, FlacNamespace::FLAC__uint64* absolute_byte_offset, void* client_data)
    {
        using namespace FlacNamespace;
        *absolute_byte_offset = (uint64) static_cast<const FlacReader*> (client_data)->input->getPosition();
        return FLAC__STREAM_DECODER_TELL_STATUS_OK;
    }

    static FlacNamespace::FLAC__StreamDecoderLengthStatus lengthCallback_ (const FlacNamespace::FLAC__StreamDecoder*, FlacNamespace::FLAC__uint64* stream_length, void* client_data)
    {
        using namespace FlacNamespace;
        *stream_length = (uint64) static_cast<const FlacReader*> (client_data)->input->getTotalLength();
        return FLAC__STREAM_DECODER_LENGTH_STATUS_OK;
    }

    static FlacNamespace::FLAC__bool eofCallback_ (const FlacNamespace::FLAC__StreamDecoder*, void* client_data)
    {
        return static_cast<const FlacReader*> (client_data)->input->isExhausted();
    }

    static FlacNamespace::FLAC__StreamDecoderWriteStatus writeCallback_ (const FlacNamespace::FLAC__StreamDecoder*,
                                                                         const FlacNamespace::FLAC__Frame* frame,
                                                                         const FlacNamespace::FLAC__int32* const buffer[],
                                                                         void* client_data)
    {
        using namespace FlacNamespace;
        static_cast<FlacReader*> (client_data)->useSamples (buffer, (int) frame->header.blocksize);
        return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
    }

    static void metadataCallback_ (const FlacNamespace::FLAC__StreamDecoder*,
                                   const FlacNamespace::FLAC__StreamMetadata* metadata,
                                   void* client_data)
    {
        static_cast<FlacReader*> (client_data)->useMetadata (metadata->data.stream_info);
    }

    static void errorCallback_ (const FlacNamespace::FLAC__StreamDecoder*, FlacNamespace::FLAC__StreamDecoderErrorStatus, void*)
    {
    }

private:
    FlacNamespace::FLAC__StreamDecoder* decoder;
    AudioSampleBuffer reservoir;
    int reservoirStart, samplesInReservoir;
    bool ok, scanningForLength;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FlacReader)
};


//==============================================================================
class FlacWriter  : public AudioFormatWriter
{
public:
    FlacWriter (OutputStream* const out, double rate, uint32 numChans, uint32 bits, int qualityOptionIndex)
        : AudioFormatWriter (out, flacFormatName, rate, numChans, bits),
          streamStartPos (output != nullptr ? jmax (output->getPosition(), 0ll) : 0ll)
    {
        using namespace FlacNamespace;
        encoder = FLAC__stream_encoder_new();

        if (qualityOptionIndex > 0)
            FLAC__stream_encoder_set_compression_level (encoder, (uint32) jmin (8, qualityOptionIndex));

        FLAC__stream_encoder_set_do_mid_side_stereo (encoder, numChannels == 2);
        FLAC__stream_encoder_set_loose_mid_side_stereo (encoder, numChannels == 2);
        FLAC__stream_encoder_set_channels (encoder, numChannels);
        FLAC__stream_encoder_set_bits_per_sample (encoder, jmin ((unsigned int) 24, bitsPerSample));
        FLAC__stream_encoder_set_sample_rate (encoder, (unsigned int) sampleRate);
        FLAC__stream_encoder_set_blocksize (encoder, 0);
        FLAC__stream_encoder_set_do_escape_coding (encoder, true);

        ok = FLAC__stream_encoder_init_stream (encoder,
                                               encodeWriteCallback, encodeSeekCallback,
                                               encodeTellCallback, encodeMetadataCallback,
                                               this) == FLAC__STREAM_ENCODER_INIT_STATUS_OK;
    }

    ~FlacWriter()
    {
        if (ok)
        {
            FlacNamespace::FLAC__stream_encoder_finish (encoder);
            output->flush();
        }
        else
        {
            output = nullptr; // to stop the base class deleting this, as it needs to be returned
                              // to the caller of createWriter()
        }

        FlacNamespace::FLAC__stream_encoder_delete (encoder);
    }

    //==============================================================================
    bool write (const int** samplesToWrite, int numSamples) override
    {
        using namespace FlacNamespace;
        if (! ok)
            return false;

        HeapBlock<int*> channels;
        HeapBlock<int> temp;
        const int bitsToShift = 32 - (int) bitsPerSample;

        if (bitsToShift > 0)
        {
            temp.malloc (numChannels * (size_t) numSamples);
            channels.calloc (numChannels + 1);

            for (unsigned int i = 0; i < numChannels; ++i)
            {
                if (samplesToWrite[i] == nullptr)
                    break;

                int* const destData = temp.getData() + i * (size_t) numSamples;
                channels[i] = destData;

                for (int j = 0; j < numSamples; ++j)
                    destData[j] = (samplesToWrite[i][j] >> bitsToShift);
            }

            samplesToWrite = const_cast<const int**> (channels.getData());
        }

        return FLAC__stream_encoder_process (encoder, (const FLAC__int32**) samplesToWrite, (unsigned) numSamples) != 0;
    }

    bool writeData (const void* const data, const int size) const
    {
        return output->write (data, (size_t) size);
    }

    static void packUint32 (FlacNamespace::FLAC__uint32 val, FlacNamespace::FLAC__byte* b, const int bytes)
    {
        b += bytes;

        for (int i = 0; i < bytes; ++i)
        {
            *(--b) = (FlacNamespace::FLAC__byte) (val & 0xff);
            val >>= 8;
        }
    }

    void writeMetaData (const FlacNamespace::FLAC__StreamMetadata* metadata)
    {
        using namespace FlacNamespace;
        const FLAC__StreamMetadata_StreamInfo& info = metadata->data.stream_info;

        unsigned char buffer [FLAC__STREAM_METADATA_STREAMINFO_LENGTH];
        const unsigned int channelsMinus1 = info.channels - 1;
        const unsigned int bitsMinus1 = info.bits_per_sample - 1;

        packUint32 (info.min_blocksize, buffer, 2);
        packUint32 (info.max_blocksize, buffer + 2, 2);
        packUint32 (info.min_framesize, buffer + 4, 3);
        packUint32 (info.max_framesize, buffer + 7, 3);
        buffer[10] = (uint8) ((info.sample_rate >> 12) & 0xff);
        buffer[11] = (uint8) ((info.sample_rate >> 4) & 0xff);
        buffer[12] = (uint8) (((info.sample_rate & 0x0f) << 4) | (channelsMinus1 << 1) | (bitsMinus1 >> 4));
        buffer[13] = (FLAC__byte) (((bitsMinus1 & 0x0f) << 4) | (unsigned int) ((info.total_samples >> 32) & 0x0f));
        packUint32 ((FLAC__uint32) info.total_samples, buffer + 14, 4);
        memcpy (buffer + 18, info.md5sum, 16);

        const bool seekOk = output->setPosition (streamStartPos + 4);
        ignoreUnused (seekOk);

        // if this fails, you've given it an output stream that can't seek! It needs
        // to be able to seek back to write the header
        jassert (seekOk);

        output->writeIntBigEndian (FLAC__STREAM_METADATA_STREAMINFO_LENGTH);
        output->write (buffer, FLAC__STREAM_METADATA_STREAMINFO_LENGTH);
    }

    //==============================================================================
    static FlacNamespace::FLAC__StreamEncoderWriteStatus encodeWriteCallback (const FlacNamespace::FLAC__StreamEncoder*,
                                                                              const FlacNamespace::FLAC__byte buffer[],
                                                                              size_t bytes,
                                                                              unsigned int /*samples*/,
                                                                              unsigned int /*current_frame*/,
                                                                              void* client_data)
    {
        using namespace FlacNamespace;
        return static_cast<FlacWriter*> (client_data)->writeData (buffer, (int) bytes)
                ? FLAC__STREAM_ENCODER_WRITE_STATUS_OK
                : FLAC__STREAM_ENCODER_WRITE_STATUS_FATAL_ERROR;
    }

    static FlacNamespace::FLAC__StreamEncoderSeekStatus encodeSeekCallback (const FlacNamespace::FLAC__StreamEncoder*, FlacNamespace::FLAC__uint64, void*)
    {
        using namespace FlacNamespace;
        return FLAC__STREAM_ENCODER_SEEK_STATUS_UNSUPPORTED;
    }

    static FlacNamespace::FLAC__StreamEncoderTellStatus encodeTellCallback (const FlacNamespace::FLAC__StreamEncoder*, FlacNamespace::FLAC__uint64* absolute_byte_offset, void* client_data)
    {
        using namespace FlacNamespace;
        if (client_data == nullptr)
            return FLAC__STREAM_ENCODER_TELL_STATUS_UNSUPPORTED;

        *absolute_byte_offset = (FLAC__uint64) static_cast<FlacWriter*> (client_data)->output->getPosition();
        return FLAC__STREAM_ENCODER_TELL_STATUS_OK;
    }

    static void encodeMetadataCallback (const FlacNamespace::FLAC__StreamEncoder*, const FlacNamespace::FLAC__StreamMetadata* metadata, void* client_data)
    {
        static_cast<FlacWriter*> (client_data)->writeMetaData (metadata);
    }

    bool ok;

private:
    FlacNamespace::FLAC__StreamEncoder* encoder;
    int64 streamStartPos;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FlacWriter)
};


//==============================================================================
FlacAudioFormat::FlacAudioFormat()
    : AudioFormat (flacFormatName, ".flac")
{
}

FlacAudioFormat::~FlacAudioFormat()
{
}

Array<int> FlacAudioFormat::getPossibleSampleRates()
{
    const int rates[] = { 8000, 11025, 12000, 16000, 22050, 32000, 44100, 48000,
                          88200, 96000, 176400, 192000, 352800, 384000 };

    return Array<int> (rates, numElementsInArray (rates));
}

Array<int> FlacAudioFormat::getPossibleBitDepths()
{
    const int depths[] = { 16, 24 };

    return Array<int> (depths, numElementsInArray (depths));
}

bool FlacAudioFormat::canDoStereo()     { return true; }
bool FlacAudioFormat::canDoMono()       { return true; }
bool FlacAudioFormat::isCompressed()    { return true; }

AudioFormatReader* FlacAudioFormat::createReaderFor (InputStream* in, const bool deleteStreamIfOpeningFails)
{
    ScopedPointer<FlacReader> r (new FlacReader (in));

    if (r->sampleRate > 0)
        return r.release();

    if (! deleteStreamIfOpeningFails)
        r->input = nullptr;

    return nullptr;
}

AudioFormatWriter* FlacAudioFormat::createWriterFor (OutputStream* out,
                                                     double sampleRate,
                                                     unsigned int numberOfChannels,
                                                     int bitsPerSample,
                                                     const StringPairArray& /*metadataValues*/,
                                                     int qualityOptionIndex)
{
    if (out != nullptr
        && getPossibleBitDepths().contains (bitsPerSample)
        && getPossibleSampleRates().contains ((int) sampleRate)
        && isPositiveAndBelow (qualityOptionIndex, getQualityOptions().size())
    {
        ScopedPointer<FlacWriter> w (new FlacWriter (out, sampleRate, numberOfChannels,
                                                     (uint32) bitsPerSample, qualityOptionIndex));
        if (w->ok)
            return w.release();
    }

    return nullptr;
}

StringArray FlacAudioFormat::getQualityOptions()
{
    static const char* options[] = { "0 (Fastest)", "1", "2", "3", "4", "5 (Default)","6", "7", "8 (Highest quality)", 0 };
    return StringArray (options);
}

//==============================================================================
#if JUCE_UNIT_TESTS

namespace FlacUnitTestData
{
    static const uint8 cello_flac[] =
    {
        102,76,97,67,0,0,0,34,2,64,2,64,0,0,189,0,3,38,1,244,0,240,0,0,32,158,109,205,38,240,130,146,30,234,248,27,23,95,161,16,195,115,4,0,0,42,11,0,0,0,76,97,118,102,53,50,46,57,51,46,48,1,0,0,0,19,0,0,0,101,110,99,111,100,101,114,61,76,97,118,102,53,50,46,
        57,51,46,48,129,0,32,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,248,36,8,0,202,78,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,229,151,185,183,216,157,94,242,161,114,94,
        107,1,76,255,91,6,31,255,255,255,255,232,68,10,25,228,64,130,1,24,16,64,44,156,70,78,94,69,69,103,177,61,110,45,200,107,146,169,148,38,195,3,74,244,86,185,118,68,52,81,248,48,22,12,47,56,145,194,78,122,170,220,52,43,205,8,110,152,219,56,207,116,92,148,
        121,156,52,33,180,162,69,18,138,213,71,227,62,200,92,149,233,42,222,40,152,33,236,130,199,38,200,149,225,195,68,97,42,114,96,141,72,163,72,221,34,98,82,19,89,190,18,50,66,157,230,210,74,252,71,137,223,217,229,50,174,123,156,238,41,185,27,92,159,61,148,
        236,191,70,207,160,218,131,130,33,74,128,136,98,134,211,17,2,66,160,68,212,110,6,73,128,6,14,44,3,205,242,203,201,95,17,26,30,151,119,29,154,63,166,80,57,168,188,55,86,204,140,196,213,225,50,116,75,246,216,55,107,163,207,121,201,82,123,68,79,5,3,196,
        196,186,31,104,163,197,94,232,84,112,100,110,185,74,84,6,93,110,226,27,77,91,139,26,44,164,190,124,100,141,100,119,220,18,197,35,189,182,68,69,205,242,62,122,57,152,53,84,38,12,29,49,51,109,203,229,114,40,228,170,63,190,234,131,161,215,144,228,44,216,
        66,68,209,37,22,174,42,193,38,225,139,100,27,108,20,151,238,228,26,174,222,24,128,112,90,229,59,51,250,16,14,234,4,11,145,156,136,30,85,214,15,60,142,203,129,185,105,216,201,25,186,133,101,161,195,144,184,236,250,237,238,103,196,212,226,130,208,171,191,
        119,225,72,166,89,32,132,185,118,151,28,11,246,81,68,89,74,42,194,194,79,240,150,34,233,143,219,131,150,51,105,159,112,125,141,116,29,74,134,144,43,59,8,38,224,90,124,181,6,155,105,108,79,155,213,216,125,0,75,233,52,8,146,8,13,113,22,154,55,119,4,239,
        163,40,104,150,194,121,93,75,242,202,17,130,176,78,59,21,216,198,9,108,228,219,212,54,119,111,209,40,130,136,111,39,185,5,210,104,128,232,138,119,36,230,48,36,151,217,137,129,176,114,20,149,69,152,200,9,255,248,36,8,1,205,78,7,69,3,249,1,121,255,216,
        253,253,251,84,248,68,245,184,229,151,67,189,174,133,87,42,25,44,238,143,193,76,127,84,132,226,81,85,201,25,70,186,175,236,180,239,150,34,100,57,12,114,138,179,148,170,108,202,64,142,91,88,119,124,178,55,191,189,10,224,57,194,117,217,59,251,54,140,1,
        28,87,104,4,32,135,8,240,30,224,88,132,213,234,2,32,126,221,12,155,215,227,188,103,100,208,133,19,207,90,80,87,2,27,142,154,12,136,112,80,136,166,113,202,137,230,57,32,13,146,220,177,173,136,17,192,158,46,44,108,107,135,117,118,48,232,64,121,17,164,42,
        211,85,155,104,160,154,98,209,167,72,217,114,192,71,64,9,19,228,133,33,116,40,25,211,42,19,121,199,252,134,241,83,46,74,108,208,177,23,155,162,188,22,69,60,35,232,80,200,254,105,9,76,244,41,236,233,38,156,169,80,179,253,160,59,162,1,138,86,72,88,173,
        158,198,113,77,229,11,13,125,230,48,108,173,221,207,75,249,100,16,65,47,253,51,76,42,138,211,128,184,112,115,207,210,40,223,82,214,56,18,15,131,94,58,165,14,141,132,129,40,138,1,94,110,176,53,169,147,2,219,57,150,130,41,217,161,38,68,237,1,228,123,176,
        213,154,159,156,152,163,106,135,34,14,66,68,191,211,110,118,25,238,148,159,66,109,252,118,238,153,184,132,87,175,98,211,176,74,17,113,46,82,15,253,4,99,200,157,249,211,176,31,243,95,116,168,103,15,89,204,16,163,234,72,52,148,100,36,39,231,152,40,198,
        134,140,177,134,46,55,179,170,62,232,30,92,142,192,37,210,207,170,120,75,128,129,46,18,50,20,201,112,230,228,128,218,33,61,247,194,25,162,232,189,119,136,76,151,212,216,8,180,205,31,76,16,24,167,62,240,102,178,142,101,149,22,104,144,185,30,118,171,140,
        189,67,138,89,197,88,228,221,173,154,23,163,111,105,140,148,12,209,106,203,45,68,238,225,204,141,144,91,38,91,104,99,98,155,18,0,170,137,117,224,61,189,32,226,229,149,112,56,232,218,123,142,86,112,64,118,137,235,254,62,145,227,160,40,161,111,6,65,30,
        53,198,220,6,198,102,102,211,59,197,229,246,47,245,224,89,42,109,28,136,46,129,50,228,149,225,228,93,18,115,22,26,64,150,56,169,178,28,130,33,63,221,116,130,114,133,136,33,232,188,222,116,255,230,32,215,177,107,116,250,22,231,128,179,19,202,149,194,115,
        177,161,175,240,9,248,203,217,240,45,63,120,161,203,226,38,141,58,236,13,209,32,26,26,168,42,101,71,138,43,35,141,113,238,8,192,134,248,204,128,233,200,3,44,65,107,102,212,76,212,112,237,148,27,76,157,77,121,64,149,17,162,63,218,175,151,2,242,13,219,
        216,181,51,200,128,240,184,127,66,198,20,60,161,116,207,176,97,86,4,161,249,15,187,116,34,15,112,108,5,232,70,167,141,250,148,53,129,10,35,27,166,245,59,101,58,251,231,241,189,205,42,16,243,116,191,86,45,254,90,66,38,179,188,68,242,20,40,7,121,67,99,
        210,114,45,208,82,97,139,223,221,253,82,200,213,216,78,204,169,45,126,87,12,241,48,209,152,58,48,188,209,138,11,154,117,12,251,196,81,128,100,168,255,248,36,8,2,196,78,238,115,237,235,238,167,238,48,236,40,237,38,240,181,244,43,229,152,217,180,4,164,
        14,225,57,181,205,199,130,26,126,241,1,34,93,78,139,52,232,233,61,180,128,159,95,74,23,182,251,108,104,157,105,218,14,164,103,227,116,186,73,33,12,5,90,158,57,67,253,67,107,97,40,79,235,213,166,21,17,3,148,43,0,25,147,28,74,200,195,30,160,173,21,48,98,
        156,70,209,224,171,153,6,89,193,243,27,186,28,205,125,78,209,60,67,92,158,193,218,248,185,164,59,162,104,222,60,176,190,16,204,121,244,116,157,195,147,163,173,198,7,197,219,122,133,60,74,164,11,220,205,215,214,108,88,216,106,115,225,125,84,224,230,234,
        180,252,215,107,164,100,218,211,20,114,127,228,157,52,200,94,65,249,192,140,84,207,228,244,154,242,162,134,26,19,166,39,147,108,28,121,111,13,124,192,73,242,160,57,46,36,254,224,91,74,211,117,179,114,164,80,120,42,82,59,168,12,106,251,16,129,4,192,219,
        39,132,9,108,113,191,145,140,18,85,240,149,91,170,153,190,170,15,189,104,103,125,250,144,173,148,229,21,55,209,89,234,131,205,52,157,37,67,110,244,50,66,225,170,136,223,154,71,215,190,147,69,208,36,139,114,170,36,170,132,76,46,87,160,171,86,136,149,175,
        4,218,136,228,155,233,113,167,92,107,21,141,54,206,110,230,153,223,60,22,158,35,158,225,5,157,203,77,106,245,63,243,136,91,182,168,181,40,135,209,181,99,167,176,83,35,27,71,141,27,238,177,5,154,159,78,129,245,137,31,18,212,53,123,87,166,8,99,9,17,187,
        68,112,135,148,102,214,53,249,166,80,153,211,200,117,72,129,11,188,182,250,128,191,166,155,51,86,148,234,120,133,253,44,147,95,198,29,205,38,151,166,125,150,235,188,190,13,10,22,228,43,13,145,1,86,55,31,18,150,80,45,231,114,201,211,249,209,217,243,12,
        134,156,189,133,145,184,167,20,99,233,75,37,75,67,81,181,186,106,107,183,242,24,27,250,192,67,137,29,111,18,251,55,83,165,222,204,231,12,233,217,251,130,88,214,19,241,138,32,23,129,74,135,214,65,145,26,164,89,65,12,206,40,66,78,46,52,39,148,34,222,138,
        62,50,231,200,32,90,108,142,158,33,47,24,166,249,141,213,189,48,136,18,87,41,204,43,214,191,146,161,107,127,89,185,53,254,42,98,133,124,172,46,250,206,121,245,213,101,162,152,159,242,235,177,140,146,140,222,171,164,196,153,152,218,131,205,213,89,12,211,
        25,246,238,185,74,198,166,212,230,51,170,209,19,92,51,75,252,91,250,65,132,148,124,154,128,180,216,26,122,55,181,89,90,168,215,176,110,113,121,181,184,52,74,65,36,44,228,251,48,146,60,102,148,79,202,92,75,136,80,17,255,5,131,168,130,78,27,143,119,53,
        53,66,121,62,83,70,91,184,199,74,236,131,180,29,91,77,37,140,168,100,196,217,32,218,152,202,182,50,199,192,236,46,11,76,225,105,244,196,195,237,191,59,25,181,225,148,73,18,112,105,106,119,104,48,197,193,82,65,26,2,88,178,82,2,20,216,92,66,134,172,162,
        243,43,255,119,210,109,148,167,140,53,83,144,204,130,134,224,118,8,146,166,186,37,49,242,204,72,152,101,123,189,69,167,206,133,66,188,205,116,148,247,40,254,120,83,100,207,25,154,169,26,224,113,46,212,82,46,72,127,212,147,160,83,10,88,136,135,26,214,
        73,161,50,184,74,196,80,32,127,20,255,248,36,8,3,195,78,13,22,9,85,9,41,6,233,1,247,253,172,250,201,249,251,229,152,108,184,38,147,95,7,97,123,78,16,193,194,63,49,5,55,176,207,23,24,79,46,136,174,27,209,188,115,21,92,35,209,25,145,59,45,39,37,193,191,
        189,181,66,77,104,181,85,86,52,140,55,5,242,210,134,57,219,163,185,23,140,193,163,107,2,218,64,150,100,137,90,81,79,187,90,214,87,202,27,132,142,143,81,213,54,189,252,210,231,123,173,187,170,98,250,235,90,138,202,155,44,69,254,71,106,105,22,236,40,227,
        215,168,118,134,215,128,144,242,39,107,60,170,227,65,161,3,203,123,247,47,89,149,184,17,19,182,80,161,21,105,236,83,27,5,110,246,30,6,148,71,2,11,44,178,94,156,5,143,143,56,134,27,108,195,197,52,192,131,129,179,196,105,210,50,193,18,111,159,123,36,82,
        5,184,105,156,98,12,96,36,163,121,35,70,131,41,168,199,59,42,127,146,137,164,78,45,15,108,239,55,238,83,218,93,64,170,49,74,114,236,220,51,204,163,110,170,224,150,46,206,127,114,59,195,160,79,182,227,33,245,103,65,201,15,130,254,40,245,217,22,33,84,160,
        108,114,175,180,191,212,231,222,211,92,227,53,111,230,161,68,176,93,61,185,209,194,11,37,180,20,171,245,34,27,115,64,175,27,121,81,122,211,9,172,6,66,213,68,13,135,136,67,123,13,189,221,21,49,221,162,26,146,130,166,20,249,76,51,163,89,20,106,210,61,70,
        104,246,57,77,99,60,198,158,240,170,23,4,65,23,148,28,167,234,126,49,204,35,58,213,52,178,170,184,164,155,43,31,216,111,123,211,18,156,17,182,157,11,204,92,37,45,36,42,14,186,155,232,202,53,125,226,46,63,168,129,157,202,219,112,182,164,129,44,21,130,
        39,157,214,192,223,81,113,113,208,236,152,141,231,232,167,99,75,204,79,85,112,157,78,251,9,160,85,110,198,221,15,143,95,231,90,30,138,23,195,186,223,211,39,52,161,139,92,98,167,192,141,20,180,42,44,253,145,100,36,23,39,136,94,177,252,12,193,141,80,230,
        207,247,114,103,68,188,197,16,167,69,213,71,75,112,177,26,70,153,74,54,182,154,254,179,55,71,147,72,23,180,237,54,33,11,110,78,229,24,186,121,97,146,21,99,223,105,88,132,115,15,121,18,119,109,207,72,51,235,205,45,85,127,169,78,53,255,66,177,211,237,222,
        218,43,212,89,145,35,89,177,73,160,72,157,160,199,222,124,117,220,33,89,143,22,118,252,7,240,168,212,163,69,98,178,107,243,170,19,94,5,253,114,39,105,154,69,142,139,175,231,144,89,179,192,102,167,27,193,110,2,110,148,13,129,211,21,218,57,194,38,241,1,
        50,63,2,134,11,42,28,42,4,226,224,92,142,114,79,167,126,2,48,155,35,218,146,103,254,224,185,254,21,218,147,185,81,56,95,75,85,250,14,174,230,224,145,117,75,185,29,236,112,41,116,109,247,57,98,77,110,59,19,74,78,111,169,3,123,243,76,250,48,84,84,159,34,
        30,24,101,33,97,202,100,84,89,163,187,120,103,136,108,66,208,87,126,138,167,63,112,131,241,83,243,11,236,158,105,113,30,161,24,243,69,24,108,150,121,117,28,242,225,248,153,153,97,81,162,132,233,211,46,62,91,243,136,82,116,3,100,228,133,46,10,124,154,
        136,54,241,194,114,96,46,194,240,71,54,58,37,60,165,215,32,105,120,183,79,255,248,36,8,4,214,78,5,70,3,135,2,117,2,25,2,166,3,226,5,69,6,84,229,150,105,192,76,139,190,251,9,159,157,208,98,82,126,152,2,235,35,6,130,179,6,97,36,166,140,186,106,35,163,143,
        2,97,190,147,56,73,83,28,52,213,86,65,255,126,245,213,172,154,4,66,36,249,39,164,83,69,120,89,169,137,156,174,144,199,34,184,198,104,104,74,116,156,65,67,70,129,54,79,36,127,61,127,112,58,10,194,145,8,180,59,4,8,131,46,244,70,208,140,22,60,49,27,98,137,
        124,94,170,229,19,214,134,137,115,51,60,27,128,204,238,119,139,116,120,81,5,55,205,44,249,154,94,233,244,87,217,152,170,50,192,254,45,160,150,140,165,154,5,233,116,94,223,160,42,19,149,146,218,13,11,44,123,180,233,106,251,222,40,219,42,35,101,130,205,
        244,104,113,42,26,171,23,2,192,119,245,172,97,178,15,88,173,185,159,133,37,36,179,185,255,182,223,252,39,18,107,140,57,16,238,34,45,189,48,24,209,198,90,6,44,172,248,93,213,67,232,197,35,133,225,86,77,186,16,237,181,235,212,194,4,35,146,58,252,94,237,
        18,150,165,241,94,39,126,100,82,198,58,62,194,95,1,97,148,90,51,175,220,209,26,53,55,138,82,142,130,142,217,9,74,232,128,151,51,10,179,54,34,99,54,77,11,219,88,24,55,152,210,102,130,173,92,80,169,88,216,250,53,250,174,210,5,69,204,215,186,38,156,17,149,
        20,244,142,89,164,230,217,148,147,88,9,45,49,209,70,204,103,165,146,12,160,205,69,183,200,142,156,47,76,169,40,104,8,120,155,3,24,69,28,64,184,3,135,245,174,154,202,185,26,129,5,212,174,8,242,17,55,143,177,194,16,98,144,190,37,52,58,55,50,151,28,195,
        46,102,114,43,28,148,48,156,212,38,132,9,101,95,169,41,87,146,94,42,131,205,7,50,11,99,182,1,63,77,253,255,202,217,67,104,139,177,10,77,153,225,48,212,104,114,199,87,115,59,137,137,56,204,58,88,66,58,19,190,74,37,196,244,35,43,42,143,162,88,220,68,102,
        160,21,209,65,40,184,69,198,43,154,69,131,18,22,137,218,227,229,17,88,197,180,88,64,21,87,68,69,11,170,3,226,243,52,73,164,106,37,38,253,233,32,143,107,144,154,17,72,132,231,50,196,98,12,235,13,18,240,224,112,232,75,25,114,198,183,197,15,241,182,119,
        6,254,207,110,176,188,51,199,98,112,126,80,27,142,11,138,127,109,173,70,66,128,50,28,187,133,14,228,6,198,149,7,248,32,133,92,30,82,199,12,222,160,155,227,206,11,197,163,169,13,49,190,10,196,25,166,21,230,151,221,137,34,168,123,105,53,192,21,135,170,
        229,48,100,45,73,32,21,250,57,47,139,173,108,130,169,71,91,220,21,84,133,7,22,136,85,77,23,248,176,52,0,46,23,255,248,36,8,5,209,78,0,55,0,73,0,33,255,161,254,224,254,164,254,145,254,32,229,150,105,193,152,136,230,251,137,160,221,187,194,127,254,137,
        0,214,107,123,238,248,89,17,242,136,148,139,169,203,139,134,108,95,141,89,91,35,18,76,15,82,243,218,42,10,155,82,136,149,50,225,37,209,52,86,53,198,253,93,131,62,136,47,229,224,224,142,42,161,78,73,130,150,179,188,175,128,72,152,67,2,24,215,0,157,58,
        83,195,211,54,218,102,106,159,96,213,123,131,5,158,64,138,65,140,163,248,191,249,123,75,165,74,112,147,146,49,70,109,171,92,202,210,84,109,86,203,133,48,79,42,22,202,228,237,121,164,145,15,211,4,109,223,227,32,5,238,122,58,87,153,204,141,248,228,164,
        229,237,239,138,122,195,219,37,4,119,134,60,220,178,122,251,104,217,90,223,7,125,203,163,61,51,254,183,230,46,71,176,218,218,134,220,56,34,138,226,112,124,190,69,165,6,19,39,215,246,24,158,134,152,55,66,74,165,21,89,168,36,231,173,19,231,182,48,95,231,
        79,73,93,185,220,150,43,113,146,155,27,225,93,33,87,175,26,133,2,35,241,19,20,165,185,195,86,129,146,151,253,152,39,70,72,122,58,96,234,44,164,185,48,172,19,109,55,34,204,138,225,54,11,20,86,77,85,253,77,210,178,111,12,121,9,180,15,196,26,8,164,217,188,
        187,85,64,157,96,70,218,223,246,49,98,49,60,183,238,98,139,171,34,137,119,206,174,197,53,162,212,137,212,186,46,78,77,166,134,199,179,82,254,81,32,65,195,17,41,136,115,88,204,31,219,4,207,71,72,34,7,150,183,130,132,230,65,10,47,90,196,40,42,84,145,79,
        151,82,212,199,177,120,128,18,228,47,77,104,153,234,35,74,65,90,141,170,114,105,122,77,26,90,74,176,207,113,4,172,41,108,84,152,106,123,84,106,75,226,153,153,29,204,136,72,50,158,124,211,222,151,18,46,81,119,33,224,171,84,132,177,51,9,193,24,147,38,38,
        248,126,212,167,183,236,192,87,172,100,128,73,134,232,65,105,49,23,178,80,37,53,55,196,61,105,152,112,52,81,211,248,141,4,77,98,195,56,204,198,45,221,8,79,229,245,68,130,243,26,229,12,232,83,170,210,104,210,69,157,65,189,61,196,254,167,218,30,100,232,
        189,189,89,142,65,225,241,134,92,110,208,106,46,220,238,101,164,175,182,244,220,237,183,171,99,145,171,228,12,12,77,202,168,183,44,202,153,2,223,87,89,37,76,61,180,195,133,97,208,65,190,106,134,160,108,46,34,243,70,173,139,144,180,180,255,248,36,8,6,
        216,78,4,121,5,45,5,204,6,47,6,106,6,115,6,31,5,99,229,150,75,193,246,136,250,249,49,161,13,208,226,75,254,181,128,177,122,185,246,32,167,244,73,123,19,62,98,17,54,254,2,20,90,130,133,225,218,146,179,128,212,180,252,157,81,205,63,170,4,130,72,21,153,
        150,231,43,100,168,76,40,59,75,155,103,161,61,232,241,237,241,138,209,49,43,109,164,19,116,185,141,118,16,60,212,142,246,102,175,66,1,96,215,69,139,63,32,88,13,113,28,6,3,233,132,200,53,24,23,174,15,3,106,176,128,180,208,47,231,49,248,192,80,74,9,208,
        149,17,33,148,76,65,132,220,146,252,44,142,115,17,95,35,222,242,233,168,229,209,222,182,60,2,239,255,80,51,95,76,102,168,52,39,206,115,102,164,241,11,125,231,148,84,49,78,42,96,229,209,197,51,111,59,132,135,70,100,121,175,159,62,210,84,232,194,146,226,
        155,147,13,206,129,6,238,54,11,139,191,121,91,234,131,228,131,115,35,24,197,235,249,90,19,78,63,191,132,8,75,22,100,116,122,191,129,39,144,64,94,13,75,90,106,102,217,177,9,25,2,88,169,136,170,157,14,8,243,47,20,171,121,177,223,221,79,249,236,236,247,
        32,24,190,72,122,64,193,35,130,145,68,205,221,91,81,177,41,44,47,213,32,122,37,52,25,96,58,166,216,166,42,136,172,72,170,224,203,65,0,209,154,154,89,5,152,59,24,252,237,234,31,109,92,181,11,10,203,162,82,194,233,209,174,50,134,218,206,68,237,112,162,
        44,74,82,38,113,193,223,248,177,232,128,89,57,63,237,205,85,232,210,87,254,220,144,252,237,247,56,61,146,254,12,158,72,149,23,201,44,150,146,242,9,130,27,62,137,110,30,178,25,80,100,195,106,164,179,156,88,13,145,66,17,16,95,111,8,47,93,72,165,143,74,
        134,164,50,209,39,20,90,185,68,47,94,146,19,81,116,36,110,72,154,5,103,165,214,173,138,199,159,145,84,35,43,163,136,94,111,191,68,112,39,29,190,251,125,196,191,42,252,183,58,144,115,143,197,80,17,92,173,245,174,36,37,129,27,94,25,55,99,177,231,40,201,
        9,186,48,249,148,73,254,98,216,218,194,200,45,20,36,73,149,129,45,23,163,147,92,137,93,238,73,102,100,132,177,136,146,183,172,96,176,156,165,43,42,64,183,84,255,248,36,8,7,223,78,255,78,255,142,255,167,255,131,255,87,255,66,255,62,255,97,229,150,61,192,
        122,141,66,247,105,161,157,193,66,155,190,67,130,173,251,225,70,131,117,9,237,86,213,226,213,16,173,192,111,12,12,11,68,102,159,57,87,220,8,37,34,98,200,250,98,15,107,135,238,134,164,117,193,169,31,216,82,112,243,22,67,42,22,22,66,76,110,112,136,172,
        117,139,210,71,71,198,80,16,218,237,126,141,72,214,147,115,148,202,46,252,64,49,204,105,166,12,228,180,167,4,69,228,4,137,223,208,251,90,63,107,52,211,248,191,6,230,179,239,198,147,123,2,247,168,51,186,22,119,50,94,67,172,198,16,206,109,217,54,146,228,
        205,35,156,251,237,160,221,216,145,234,98,135,133,17,90,50,155,1,225,91,133,19,146,20,243,83,175,10,136,21,209,79,190,144,214,83,125,11,190,252,195,146,18,11,11,208,12,179,90,71,4,112,117,239,45,154,91,218,235,90,248,142,214,235,26,20,202,150,181,235,
        230,181,134,183,223,237,238,223,58,87,157,40,187,87,100,75,144,97,148,123,98,251,193,159,119,162,112,19,199,154,85,137,26,30,125,195,44,116,44,143,173,16,91,154,216,84,74,192,176,128,156,249,162,76,159,0,47,129,207,237,97,105,25,153,62,2,95,175,215,239,
        236,249,100,124,146,197,19,112,154,61,237,114,92,53,240,125,43,54,68,204,51,103,196,27,14,138,51,227,222,192,236,216,110,225,107,106,208,41,94,104,76,77,48,89,112,77,99,228,194,218,7,9,96,52,80,132,108,144,181,124,60,153,177,169,228,227,12,137,146,32,
        231,0,50,78,76,135,133,216,71,4,7,74,144,30,131,51,87,212,192,103,2,86,49,140,199,26,51,130,180,120,38,164,23,87,54,126,100,207,172,217,169,159,228,231,48,37,161,211,62,16,250,131,65,156,121,198,182,85,26,106,132,115,3,42,138,16,32,225,1,99,2,199,153,
        84,178,78,154,196,254,8,81,90,157,107,62,26,19,12,150,168,235,131,195,34,170,121,183,202,56,4,198,133,132,202,90,97,222,11,60,182,152,31,224,240,78,87,102,213,101,246,65,135,207,211,45,162,63,176,88,79,72,19,120,252,34,255,248,36,8,8,242,78,254,253,255,
        15,255,54,255,112,255,134,255,125,255,138,255,186,230,42,145,140,122,255,94,29,98,253,171,152,37,97,124,143,0,133,9,175,190,20,158,145,116,162,8,15,252,144,232,129,202,169,19,131,3,207,28,142,217,58,22,121,39,26,213,66,65,80,76,129,65,134,229,84,88,14,
        146,186,53,29,188,208,147,225,120,129,137,56,152,93,209,66,214,184,197,194,175,56,116,75,37,96,190,27,202,3,83,51,81,57,182,84,67,53,105,118,181,56,149,40,147,54,140,83,135,168,146,57,90,8,46,87,206,232,99,204,221,163,195,132,1,69,14,154,232,254,141,
        104,193,205,83,175,174,84,82,72,101,35,130,112,201,185,25,104,100,136,163,230,248,84,131,199,174,27,192,45,51,233,69,171,74,31,209,129,61,236,185,133,200,82,98,1,115,112,206,17,119,222,96,210,159,59,29,22,64,34,172,197,17,169,68,236,154,204,100,170,227,
        195,118,30,102,217,44,67,129,128,163,165,34,171,71,110,81,1,6,171,99,190,168,159,16,144,73,152,231,50,10,175,60,58,79,42,50,217,136,68,73,186,132,180,68,250,72,20,141,170,138,86,84,250,85,66,191,155,28,39,250,112,174,127,34,65,103,136,239,11,253,237,
        174,142,78,110,143,127,221,113,19,228,189,158,217,38,75,8,69,67,130,215,201,166,79,108,220,73,96,179,204,217,30,17,17,53,1,43,137,138,39,2,35,46,245,14,187,30,39,56,210,135,15,69,180,241,210,147,149,171,143,47,237,147,48,94,170,42,225,84,162,213,75,240,
        117,1,139,5,29,64,224,87,237,209,228,234,93,227,18,189,181,2,100,32,158,173,60,204,161,164,52,66,74,61,181,24,252,159,229,44,67,28,188,136,2,127,45,25,75,45,47,97,80,156,200,92,172,83,218,138,63,97,33,39,201,228,90,158,92,187,214,122,208,59,175,47,63,
        214,136,177,152,185,175,238,171,168,129,170,22,139,127,109,22,39,50,48,36,245,209,66,215,196,119,151,48,158,202,53,0,74,136,255,248,36,8,9,245,78,0,109,0,157,0,181,0,196,0,215,0,224,0,234,0,250,230,39,206,153,214,223,102,95,242,133,252,118,227,234,61,
        156,130,156,214,89,134,239,98,130,210,218,230,223,41,90,194,201,17,51,150,98,128,69,58,236,246,43,55,11,85,250,200,44,80,50,117,1,197,29,151,230,230,93,78,16,70,98,151,67,158,234,37,191,59,144,37,118,29,111,13,227,189,83,16,44,189,185,77,126,210,187,
        95,135,180,211,79,37,44,165,105,181,119,10,174,156,158,32,196,115,126,187,137,200,159,208,189,29,140,25,42,252,111,53,32,162,83,183,47,202,16,64,119,66,29,138,123,66,222,109,151,12,210,10,57,109,75,142,43,31,217,196,72,101,29,111,70,233,199,72,97,140,
        213,68,59,229,191,47,173,178,98,74,44,97,169,245,136,229,206,211,6,203,148,122,145,148,241,106,187,72,9,167,36,189,188,178,253,143,235,230,74,64,151,234,75,156,162,238,24,64,173,210,71,45,102,62,205,127,187,71,103,140,201,65,200,102,154,170,170,52,68,
        138,44,19,69,249,64,194,3,196,57,215,197,20,223,179,201,28,96,224,206,65,7,134,53,90,111,150,115,72,144,78,10,224,161,253,96,15,235,30,48,205,36,55,13,178,68,132,200,235,200,10,29,78,252,28,115,130,192,144,69,216,195,220,73,7,172,194,72,225,135,6,19,
        8,112,192,218,255,161,74,77,146,39,164,140,160,136,226,149,98,187,214,77,242,72,16,32,16,141,42,84,252,60,52,118,40,120,242,154,65,6,123,252,63,9,154,61,216,18,64,158,13,5,156,18,120,82,169,130,60,224,163,22,224,128,146,109,214,177,143,107,12,215,242,
        146,40,69,10,94,2,49,6,137,49,70,89,47,110,216,87,151,1,194,33,212,43,110,47,41,127,101,8,20,199,59,172,192,19,70,224,82,194,70,209,16,172,127,65,192,181,192,216,78,98,255,248,36,8,10,252,78,255,50,255,51,255,71,255,80,255,79,255,74,255,57,255,30,230,
        37,250,170,120,165,110,210,17,213,77,106,130,224,126,80,128,104,224,76,29,131,188,80,230,1,15,243,145,15,225,132,189,219,227,144,107,217,138,118,64,193,77,164,44,179,131,164,47,229,242,99,130,90,82,1,14,182,113,42,140,67,132,188,86,115,5,10,170,227,201,
        76,230,232,45,85,12,11,135,33,205,198,158,147,83,222,48,121,176,88,75,60,242,156,24,89,64,229,96,83,116,97,129,100,86,27,93,34,17,198,249,84,232,128,66,28,19,197,67,145,69,250,226,100,128,153,49,129,88,113,231,157,196,8,245,161,179,225,9,112,21,104,225,
        100,166,248,105,165,77,172,178,151,142,155,21,131,193,37,138,31,149,34,122,193,171,48,185,48,140,96,50,176,114,84,58,27,68,120,190,17,142,159,166,169,212,139,6,50,108,89,129,79,165,44,165,132,152,77,249,14,120,34,42,154,84,138,32,122,161,90,152,179,3,
        8,82,196,217,185,225,47,51,133,185,107,173,20,74,1,132,249,225,248,74,245,101,40,144,104,88,225,31,220,66,135,37,28,225,202,82,113,94,126,135,43,218,40,252,50,99,24,119,51,119,177,206,118,165,43,212,30,101,118,87,68,216,170,20,249,116,239,45,118,5,89,
        25,233,44,160,146,214,75,45,48,205,127,71,32,230,13,39,199,147,13,105,197,18,60,255,40,83,200,57,158,200,165,8,21,204,47,78,45,162,156,187,68,233,130,9,147,44,68,169,129,38,48,113,82,145,57,70,188,162,166,234,209,202,199,85,90,196,59,77,214,113,211,20,
        220,178,70,209,45,83,139,83,44,44,167,66,179,52,146,195,86,171,169,151,173,40,116,149,138,197,44,138,41,71,12,9,210,185,224,67,158,255,248,36,8,11,251,78,0,134,0,125,0,114,0,95,0,78,0,67,0,60,0,60,230,34,134,187,40,130,171,15,89,120,221,201,162,52,63,
        43,128,108,99,210,237,154,33,137,129,131,138,22,31,70,52,93,156,166,233,84,147,58,19,206,168,16,183,162,152,241,88,78,38,222,155,15,105,212,136,229,147,210,174,131,51,194,77,53,106,68,204,211,221,254,56,98,96,236,121,3,57,129,73,250,69,36,130,243,182,
        220,48,132,175,204,95,160,236,227,28,168,208,184,67,121,183,88,100,177,62,134,140,207,100,231,157,22,221,119,152,185,44,214,147,76,137,68,166,255,200,184,226,37,254,104,150,161,147,91,197,110,210,4,53,132,79,32,131,20,66,82,142,154,220,204,18,96,199,
        91,106,239,33,5,48,232,34,103,143,110,27,58,183,8,236,139,37,39,12,246,245,57,197,250,166,106,147,186,246,198,161,59,37,114,76,5,194,20,67,84,74,121,98,90,101,164,132,169,218,202,100,217,150,103,32,86,74,43,124,67,103,72,16,124,25,83,141,100,59,19,163,
        25,200,170,213,36,209,59,17,4,17,241,25,186,68,209,75,228,23,10,35,90,64,212,207,103,121,45,140,17,51,111,90,12,81,121,39,230,19,186,67,218,238,118,103,106,149,92,145,6,51,41,29,194,248,133,219,133,115,107,81,125,40,103,63,4,81,136,207,252,179,95,51,
        201,146,162,49,8,220,171,170,32,252,86,153,86,72,151,108,171,107,254,173,19,113,249,238,32,153,253,185,230,247,61,114,227,18,165,66,21,44,82,196,74,12,151,20,122,216,197,232,163,182,18,152,81,73,102,219,156,166,59,245,181,102,193,0,166,11,255,248,36,
        8,12,238,76,255,247,255,227,255,210,255,199,255,195,255,195,255,197,230,188,172,148,68,204,158,185,177,208,125,22,98,26,128,33,4,41,131,31,220,157,212,193,179,138,238,23,13,90,7,41,65,153,56,122,86,8,200,170,43,172,84,11,200,32,34,228,18,6,12,234,12,
        172,193,164,13,9,197,202,160,16,33,249,73,187,24,166,192,128,193,12,160,227,132,112,234,42,29,155,106,140,32,193,32,162,153,130,153,5,12,133,5,66,63,138,83,64,168,206,199,80,133,177,173,50,1,205,129,64,162,17,2,6,62,49,88,49,179,82,25,14,9,152,24,133,
        67,216,17,174,169,235,30,0,50,132,54,73,169,84,81,79,130,21,21,24,162,24,96,206,65,205,69,26,28,162,114,184,35,185,128,163,41,242,141,23,7,72,133,8,142,134,114,240,226,162,3,33,45,27,28,8,76,154,48,200,162,33,24,205,242,133,96,129,20,86,210,25,144,136,
        243,6,138,236,56,225,80,25,208,184,50,102,168,65,62,66,245,87,201,34,178,11,87,149,196,8,122,22,132,20,78,140,233,115,142,91,3,6,12,169,28,71,48,204,156,67,65,142,33,168,150,12,20,172,8,194,198,118,81,153,131,3,240,144,98,199,56,202,29,185,64,203,81,
        54,48,198,74,168,165,146,133,25,7,65,19,133,11,105,166,207,118,123,77,16,226,23,31,82,141,134,106,218,144,77,68,55,130,53,65,6,20,24,131,136,189,70,104,244,137,42,100,147,56,222,79,69,42,97,168,195,147,176,129,26,160,230,66,199,84,235,149,141,0,157,60,
        255,248,36,8,13,233,78,255,220,255,211,255,212,255,205,255,197,255,181,255,176,255,180,230,184,51,160,132,196,198,165,18,21,156,178,35,242,125,9,128,66,2,57,2,226,10,50,167,10,212,149,221,134,216,130,51,61,80,72,38,204,53,77,132,39,97,73,21,17,12,207,
        143,89,132,159,154,41,62,175,249,46,104,74,12,108,107,57,156,165,94,173,50,199,24,231,85,44,129,140,81,62,165,113,25,203,167,24,45,129,6,24,177,176,71,241,6,96,144,77,125,24,108,146,148,177,3,60,104,221,39,102,47,68,75,9,139,127,99,46,9,132,156,204,16,
        170,238,193,254,82,98,185,154,138,40,174,65,2,72,98,229,115,118,30,17,61,205,115,34,25,91,81,40,206,102,83,33,149,177,9,230,168,179,83,120,199,97,159,150,152,130,21,9,249,117,168,33,33,138,67,49,17,3,214,113,214,47,17,200,48,20,166,113,206,66,4,161,21,
        81,87,117,68,152,72,187,196,68,216,38,118,69,98,98,33,17,191,8,66,212,72,189,198,201,88,67,182,62,45,76,4,66,112,45,77,204,140,65,177,113,155,106,41,33,97,197,132,4,81,231,63,50,155,90,201,234,37,145,212,69,200,200,185,139,33,9,130,52,216,166,103,96,
        76,43,13,189,70,236,174,220,70,37,228,238,58,140,162,8,219,63,235,168,205,196,198,141,184,157,247,245,214,104,56,157,72,119,64,75,31,161,8,83,73,236,164,85,196,51,56,5,63,255,248,116,8,14,1,29,255,78,255,254,255,254,255,250,255,250,255,255,255,253,0,
        4,0,21,230,182,56,161,88,193,74,155,50,90,44,152,4,39,124,95,0,81,10,39,17,24,140,136,218,41,147,169,68,202,76,137,241,16,115,161,88,53,34,110,118,114,28,193,28,33,136,74,18,119,61,202,175,243,147,170,138,49,93,138,27,251,98,52,98,48,171,123,10,18,97,
        47,172,150,8,25,39,102,86,96,74,106,253,47,36,100,21,25,137,215,227,60,227,144,103,35,53,69,41,173,162,12,68,9,87,162,122,246,139,175,172,95,130,222,223,134,72,242,58,17,181,208,89,114,19,18,138,101,40,65,132,196,19,229,39,26,102,25,71,141,50,68,245,
        141,139,13,158,172,69,230,27,50,175,24,226,134,27,24,72,102,202,80,240,210,0,0
    };

    static const int cello_flacSize = numElementsInArray (cello_flac);
}

class FlacAudioFormatUnitTests : public AudioFormatUnitTests<FlacAudioFormat>
{
public:
    FlacAudioFormatUnitTests() { }

    InputStream* getValidTestFileData() override
    {
        return new MemoryInputStream (FlacUnitTestData::cello_flac, FlacUnitTestData::cello_flacSize, false);
    }

    StringPairArray createTestMetadata() override
    {
        return StringPairArray();
    }

private:
    JUCE_DECLARE_NON_COPYABLE (FlacAudioFormatUnitTests)
};

static const FlacAudioFormatUnitTests flacAudioFormatUnitTests;

#endif

#endif
