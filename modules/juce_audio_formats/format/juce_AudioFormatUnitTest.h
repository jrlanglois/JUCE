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

#pragma once


//==============================================================================
/**
    Subclasses of AudioFormatUnitTests will be hooked up to an automated
    battery of tests that your codec should easily pass.

    @see AudioFormat
*/
template<typename AudioFormatSubclass>
class JUCE_API AudioFormatUnitTests : public UnitTest
{
protected:
    AudioFormatUnitTests (bool supportsWritingAudio = true)
        : UnitTest (AudioFormatSubclass().getFormatName() + " format tests"),
          supportsWriter (supportsWritingAudio)
    {}

    enum
    {
        defaultNumTestAudioBufferSamples = 256
    };

    void runTest() override
    {
        beginTest ("Creating audio format instance.");
        ScopedPointer<AudioFormat> audioFormat (new AudioFormatSubclass());
        expect (audioFormat != nullptr, "Out of memory?");

        if (audioFormat == nullptr)
            return;

        performReadingTests (*audioFormat);

        if (supportsWriter)
            performWritingTests (*audioFormat);
    }

    /** Subclasses must supply all of their supported tags in order to test metadata.
        Obviously the keys must not be blank, but the values can be.
    */
    virtual StringPairArray createTestMetadata() = 0;

    /** Subclasses must supply a valid set of audio data, effectively a blob from a file,
        to test codec reading.

        The stream returned will be managed by the tests.
    */
    virtual InputStream* getValidTestFileData() = 0;

    /** Subclasses may optionally supply a set of corrupted audio data
        to be sure codec reading fails as expected.

        The stream returned will be managed by the tests.

        By default, the stream refers to an empty block of data.
    */
    virtual InputStream* getCorruptedTestFileData()
    {
        return new MemoryInputStream (emptyData, false);
    }

    void performReadingTests (AudioFormat& instance)
    {
        InputStream* validData = getValidTestFileData();
        expect (validData != nullptr);

        ScopedPointer<AudioFormatReader> reader (instance.createReaderFor (validData, true));
        expect (reader != nullptr);

        if (reader->lengthInSamples > 0 && reader->numChannels > 0)
        {
            AudioSampleBuffer buffer (reader->numChannels, (int) reader->lengthInSamples);

            expect (reader->readSamples (nullptr, (int) reader->numChannels, 0, 0, (int) reader->lengthInSamples));
        }

        InputStream* corruptedData = getCorruptedTestFileData();
        expect (corruptedData != nullptr);

        reader = instance.createReaderFor (corruptedData, true);
        expect (reader == nullptr);
    }

    void performWritingTests (AudioFormat& instance)
    {
        beginTest ("Setting up metadata");
        const StringPairArray metadataValues (createTestMetadata());
        expect (metadataValues.size() > 0);

        const Array<int> possibleSampleRates = instance.getPossibleSampleRates();
        const Array<int> possibleBitDepths = instance.getPossibleBitDepths();
        const StringArray possibleQualityOptions = instance.getQualityOptions();
        const int numQualityOptions = possibleQualityOptions.size();

        Array<int> channelsToTest;
        if (instance.canDoMono())   channelsToTest.add (1);
        if (instance.canDoStereo()) channelsToTest.add (2);

        const int defaultSampleRate = possibleSampleRates.getFirst();
        const int defaultBitDepth = possibleBitDepths.getFirst();
        const int defaultQualityOption = 0; //Fixed due to not having any default settings mechanisms present.
/*
        expect (channelsToTest.size() > 0);
        expect (possibleSampleRates.size() > 0);
        expect (possibleBitDepths.size() > 0);
        expect (numQualityOptions > 0);

        if (instance.isCompressed())
        {
            possibleBitDepths.size() > 0;
            numQualityOptions > 0;
        
            for (int qualityOptionIndex = 0; qualityOptionIndex < numQualityOptions; ++qualityOptionIndex)
                for (const int rate : possibleSampleRates)
                    for (const int numChannels : channelsToTest)
                        testWrite (instance, metadataValues, rate, numChannels, 0, qualityOptionIndex);
        }
        else
        {
            for (const int depth : possibleBitDepths)
                for (const int rate : possibleSampleRates)
                    for (const int numChannels : channelsToTest)
                        testWrite (instance, metadataValues, rate, numChannels, depth);
        }
*/
        //Test invalid bit-depth
        testWrite (instance, metadataValues, defaultSampleRate, 2, 1, 0, nullptr, true);

        //Test invalid sample-rate
        testWrite (instance, metadataValues, 1, 2, defaultBitDepth, 0, nullptr, true);

        //Test 0 channels
        testWrite (instance, metadataValues, defaultSampleRate, 0, defaultBitDepth, 0, nullptr, true);

        beginTest ("Metadata write/read test.");

        {
            MemoryBlock testData;
            testWrite (instance, metadataValues, 44100, 2, 16, 0, &testData);

            ScopedPointer<AudioFormatReader> reader (instance.createReaderFor (new MemoryInputStream (testData, false), false));
            expect (reader != nullptr);
            expect (reader->metadataValues == metadataValues, "Somehow, the metadata is different!");
        }
    }

    void testWrite (AudioFormat& format, const StringPairArray& metadata,
                    int rate, int numChannels, int depth,
                    int qualityOptionIndex = 0,
                    MemoryBlock* dataToWriteTo = nullptr,
                    bool expectNullWriter = false)
    {
        String name ("Creating writer: %1 channel(s), %2 bits, %3 sample rate");
        name = name.replace ("%1", String (numChannels), true)
                   .replace ("%2", String (depth), true)
                   .replace ("%3", String (rate), true);

        beginTest (name);

        MemoryOutputStream* stream = nullptr;
        if (dataToWriteTo != nullptr)
            stream = new MemoryOutputStream (*dataToWriteTo, false);
        else
            stream = new MemoryOutputStream();

        ScopedPointer<AudioFormatWriter> writer (format.createWriterFor (stream, (double) rate, numChannels, depth, metadata, qualityOptionIndex));

        if (expectNullWriter)
        {
            expect (writer == nullptr);
            return;
        }

        expect (writer != nullptr);

        AudioSampleBuffer buffer (numChannels, defaultNumTestAudioBufferSamples);
        buffer.clear();

        beginTest ("Writing audio data (float).");

        expect (writer->writeFromAudioSampleBuffer (buffer, 0, defaultNumTestAudioBufferSamples), "Failed writing audio data!");
    }

private:
    const bool supportsWriter;
    const MemoryBlock emptyData;

    JUCE_DECLARE_NON_COPYABLE (AudioFormatUnitTests)
};
