/*
  ==============================================================================

   This file is part of the JUCE framework examples.
   Copyright (c) Raw Material Software Limited

   The code included in this file is provided under the terms of the ISC license
   http://www.isc.org/downloads/software-support-policy/isc-license. Permission
   to use, copy, modify, and/or distribute this software for any purpose with or
   without fee is hereby granted provided that the above copyright notice and
   this permission notice appear in all copies.

   THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
   REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
   AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
   INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
   LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
   OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
   PERFORMANCE OF THIS SOFTWARE.

  ==============================================================================
*/

/*******************************************************************************
 The block below describes the properties of this PIP. A PIP is a short snippet
 of code that can be read by the Projucer and used to generate a JUCE project.

 BEGIN_JUCE_PIP_METADATA

 name:             AudioAppDemo
 version:          1.0.0
 vendor:           JUCE
 website:          http://juce.com
 description:      Simple audio application.

 dependencies:     juce_audio_basics, juce_audio_devices, juce_audio_formats,
                   juce_audio_processors, juce_audio_utils, juce_core,
                   juce_data_structures, juce_events, juce_graphics,
                   juce_gui_basics, juce_gui_extra, juce_audio_processors_headless
 exporters:        xcode_mac, vs2022, vs2026, linux_make, androidstudio,
                   xcode_iphone, xcode_tvos

 moduleFlags:      JUCE_STRICT_REFCOUNTEDPOINTER=1

 type:             Component
 mainClass:        AudioAppDemo

 useLocalCopy:     1

 END_JUCE_PIP_METADATA

*******************************************************************************/

#pragma once


//==============================================================================
class AudioAppDemo final : public AudioAppComponent
{
public:
    //==============================================================================
    AudioAppDemo()
       #ifdef JUCE_DEMO_RUNNER
        : AudioAppComponent (getSharedAudioDeviceManager (0, 2))
       #endif
    {
        addAndMakeVisible (frequencySlider);
        frequencySlider.setName ("Frequency");
        frequencySlider.setRange (20.0, 10000.0, 1.0);
        frequencySlider.setValue (frequency.load());
        frequencySlider.setTextValueSuffix (" Hz");
        frequencySlider.onValueChange = [this]
        {
            frequency = (float) frequencySlider.getValue();
            repaint();
        };

        addAndMakeVisible (amplitudeSlider);
        amplitudeSlider.setName ("Amplitude");
        amplitudeSlider.setRange (0.0, 0.9, 0.01);
        amplitudeSlider.setValue (amplitude.load());
        amplitudeSlider.onValueChange = [this]
        {
            amplitude = (float) amplitudeSlider.getValue();
            repaint();
        };

        addAndMakeVisible (playButton);
        playButton.onClick = [this] { isTonePlaying = playButton.getToggleState(); };

        setAudioChannels (0, 2);

        setSize (800, 600);
    }

    ~AudioAppDemo() override
    {
        shutdownAudio();
    }

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double newSampleRate) override
    {
        sampleRate = newSampleRate;
        expectedSamplesPerBlock = samplesPerBlockExpected;
    }

    /*  This method generates the actual audio samples.
        In this example the buffer is filled with a sine wave whose frequency and
        amplitude are controlled by the mouse position.
     */
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override
    {
        bufferToFill.clearActiveBufferRegion();
        auto originalPhase = phase;
        const auto blockAmplitude = isTonePlaying.load() ? amplitude.load() : 0.0f;
        const auto phaseDelta = (float) (MathConstants<double>::twoPi * frequency.load() / sampleRate);

        for (auto chan = 0; chan < bufferToFill.buffer->getNumChannels(); ++chan)
        {
            phase = originalPhase;

            auto* channelData = bufferToFill.buffer->getWritePointer (chan, bufferToFill.startSample);

            for (auto i = 0; i < bufferToFill.numSamples ; ++i)
            {
                channelData[i] = blockAmplitude * std::sin (phase);

                // increment the phase step for the next sample
                phase = std::fmod (phase + phaseDelta, MathConstants<float>::twoPi);
            }
        }
    }

    void releaseResources() override
    {
        // This gets automatically called when audio device parameters change
        // or device is restarted.
    }


    //==============================================================================
    void paint (Graphics& g) override
    {
        // (Our component is opaque, so we must completely fill the background with a solid colour)
        g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

        auto centreY = (float) getHeight() / 2.0f;
        auto radius = amplitude.load() * 200.0f;

        if (radius >= 0.0f)
        {
            // Draw an ellipse based on the mouse position and audio volume
            g.setColour (Colours::lightgreen);

            g.fillEllipse (jmax (0.0f, lastMousePosition.x) - radius / 2.0f,
                           jmax (0.0f, lastMousePosition.y) - radius / 2.0f,
                           radius, radius);
        }

        // Draw a representative sine wave.
        Path wavePath;
        wavePath.startNewSubPath (0, centreY);

        for (auto x = 1.0f; x < (float) getWidth(); ++x)
            wavePath.lineTo (x, centreY + amplitude.load() * (float) getHeight() * 2.0f
                                            * std::sin (x * frequency.load() * 0.0001f));

        g.setColour (getLookAndFeel().findColour (Slider::thumbColourId));
        g.strokePath (wavePath, PathStrokeType (2.0f));
    }

    // Mouse handling..
    void mouseDown (const MouseEvent& e) override
    {
        isTonePlaying = true;
        mouseDrag (e);
    }

    void mouseDrag (const MouseEvent& e) override
    {
        lastMousePosition = e.position;

        frequency = jlimit (20.0f, 10000.0f, (float) (getHeight() - e.y) * 10.0f);
        amplitude = jmin (0.9f, 0.2f * e.position.x / (float) getWidth());
        frequencySlider.setValue (frequency.load(), dontSendNotification);
        amplitudeSlider.setValue (amplitude.load(), dontSendNotification);

        repaint();
    }

    void mouseUp (const MouseEvent&) override
    {
        isTonePlaying = playButton.getToggleState();
        repaint();
    }

    void resized() override
    {
        auto controls = getLocalBounds().reduced (20).removeFromBottom (90);
        playButton.setBounds (controls.removeFromLeft (120).reduced (5));
        frequencySlider.setBounds (controls.removeFromLeft (controls.getWidth() / 2).reduced (5));
        amplitudeSlider.setBounds (controls.reduced (5));
    }


private:
    //==============================================================================
    float phase       = 0.0f;
    std::atomic<float> frequency { 5000.0f };
    std::atomic<float> amplitude { 0.2f };
    std::atomic<bool> isTonePlaying { false };

    double sampleRate = 0.0;
    int expectedSamplesPerBlock = 0;
    Point<float> lastMousePosition;

    Slider frequencySlider, amplitudeSlider;
    ToggleButton playButton { "Play tone" };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioAppDemo)
};
