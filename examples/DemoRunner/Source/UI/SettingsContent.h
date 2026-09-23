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

#pragma once

#include "MainComponent.h"

//==============================================================================
class SettingsContent final : public Component
{
public:
    SettingsContent (MainComponent& topLevelComponent)
        : innerContent (topLevelComponent)
    {
        settingsViewport.setViewedComponent (&innerContent, false);
        addAndMakeVisible (settingsViewport);

        setFocusContainerType (FocusContainerType::focusContainer);
        setTitle ("DemoRunner Settings");

        setOpaque (true);
    }

    void paint (Graphics& g) override
    {
        g.fillAll (findColour (ResizableWindow::backgroundColourId));
    }

    void resized() override
    {
        constexpr int minimumWidth  = 350;
        constexpr int minimumHeight = 550;

        auto r = getLocalBounds();
        const auto scrollBarWidth = getLookAndFeel().getDefaultScrollbarWidth();

        innerContent.setSize (jmax (r.getWidth() - scrollBarWidth, minimumWidth),
                              jmax (r.getHeight(), minimumHeight));

        settingsViewport.setBounds (r);
    }

private:
    static constexpr float titleLabelFontHeight = 18.0f;
    static constexpr int itemHeight = 30;
    static constexpr int itemSpacing = 7;

    class GraphicsSettingsGroup final : public Component,
                                        private ComponentMovementWatcher,
                                        private Timer
    {
    public:
        GraphicsSettingsGroup (MainComponent& comp)
            : ComponentMovementWatcher (&comp),
              mainComponent (comp)
        {
            addAndMakeVisible (titleLabel);
            titleLabel.setFont (FontOptions { titleLabelFontHeight });

            addLookAndFeels();

            addAndMakeVisible (lookAndFeelSelector);

            for (int i = 0; i < lookAndFeelNames.size(); ++i)
                lookAndFeelSelector.addItem (lookAndFeelNames.getReference (i), i + 1);

            lookAndFeelSelector.setSelectedItemIndex (lookAndFeelNames.indexOf ("LookAndFeel_V4 (Dark)"));

            lookAndFeelSelector.onChange = [this]
            {
                auto* lf = lookAndFeels.getUnchecked (lookAndFeelSelector.getSelectedItemIndex());
                Desktop::getInstance().setDefaultLookAndFeel (lf);
            };

            addAndMakeVisible (lookAndFeelLabel);
            lookAndFeelLabel.setJustificationType (Justification::centredRight);
            lookAndFeelLabel.attachToComponent (&lookAndFeelSelector, true);

            addAndMakeVisible (rendererSelector);
            rendererSelector.onChange = [this] { mainComponent.setRenderingEngine (rendererSelector.getSelectedItemIndex()); };

            addAndMakeVisible (rendererLabel);
            rendererLabel.setJustificationType (Justification::centredRight);
            rendererLabel.attachToComponent (&rendererSelector, true);

           #if JUCE_TVOS
            addAndMakeVisible (overscanCompensationSelector);
            overscanCompensationSelector.addItemList ({ "System Default", "Scale", "Inset Bounds", "None" }, 1);
            overscanCompensationSelector.setSelectedItemIndex (getOverscanCompensationIndex (
                                                                    Desktop::getInstance().getOverscanCompensation()),
                                                               NotificationType::dontSendNotification);
            overscanCompensationSelector.onChange = [this]
            {
                Desktop::getInstance().setOverscanCompensation (
                    getOverscanCompensation (overscanCompensationSelector.getSelectedItemIndex()));
                refreshOverscanInsets();
            };

            addAndMakeVisible (overscanCompensationLabel);
            overscanCompensationLabel.setJustificationType (Justification::centredRight);
            overscanCompensationLabel.attachToComponent (&overscanCompensationSelector, true);

            addAndMakeVisible (overscanInsetsLabel);
            overscanInsetsLabel.setJustificationType (Justification::centredLeft);

            refreshOverscanInsets();
            startTimerHz (4);
           #endif

            setFocusContainerType (FocusContainerType::focusContainer);
            setTitle ("Graphics Settings");
        }

        void resized() override
        {
            auto bounds = getLocalBounds();

            titleLabel.setBounds (bounds.removeFromTop (itemHeight));
            bounds.removeFromTop (itemSpacing);

            const auto xPos  = roundToInt ((float) bounds.getX() + ((float) bounds.getWidth() * 0.35f));
            const auto width = roundToInt ((float) bounds.getWidth() * 0.6f);

            lookAndFeelSelector.setBounds (bounds.removeFromTop (itemHeight).withWidth (width).withX (xPos));
            bounds.removeFromTop (itemSpacing);

            rendererSelector.setBounds (bounds.removeFromTop (itemHeight).withWidth (width).withX (xPos));

           #if JUCE_TVOS
            bounds.removeFromTop (itemSpacing);
            overscanCompensationSelector.setBounds (bounds.removeFromTop (itemHeight).withWidth (width).withX (xPos));
            bounds.removeFromTop (itemSpacing);
            overscanInsetsLabel.setBounds (bounds.removeFromTop (itemHeight).withWidth (width).withX (xPos));
           #endif
        }

    private:
        void timerCallback() override
        {
           #if JUCE_TVOS
            refreshOverscanInsets();
           #endif
        }

        void componentMovedOrResized (bool, bool) override    {}
        using ComponentListener::componentMovedOrResized;

        void componentVisibilityChanged() override            {}
        using ComponentListener::componentVisibilityChanged;

        void componentPeerChanged() override
        {
            auto* newPeer = mainComponent.getPeer();

            if (peer != newPeer)
            {
                peer = newPeer;

                if (peer != nullptr)
                    refreshRenderingEngineSelector();
            }
        }

        void refreshRenderingEngineSelector()
        {
            rendererSelector.clear (NotificationType::dontSendNotification);

            rendererSelector.addItemList (mainComponent.getRenderingEngines(), 1);
            rendererSelector.setSelectedItemIndex (mainComponent.getCurrentRenderingEngine());
        }

        void addLookAndFeels()
        {
            lookAndFeelNames.addArray ({ "LookAndFeel_V1", "LookAndFeel_V2", "LookAndFeel_V3",
                                         "LookAndFeel_V4 (Dark)", "LookAndFeel_V4 (Midnight)",
                                         "LookAndFeel_V4 (Grey)", "LookAndFeel_V4 (Light)" });

            lookAndFeels.add (new LookAndFeel_V1());
            lookAndFeels.add (new LookAndFeel_V2());
            lookAndFeels.add (new LookAndFeel_V3());
            lookAndFeels.add (new LookAndFeel_V4 (LookAndFeel_V4::getDarkColourScheme()));
            lookAndFeels.add (new LookAndFeel_V4 (LookAndFeel_V4::getMidnightColourScheme()));
            lookAndFeels.add (new LookAndFeel_V4 (LookAndFeel_V4::getGreyColourScheme()));
            lookAndFeels.add (new LookAndFeel_V4 (LookAndFeel_V4::getLightColourScheme()));
        }

       #if JUCE_TVOS
        static int getOverscanCompensationIndex (Desktop::OverscanCompensation policy)
        {
            switch (policy)
            {
                case Desktop::OverscanCompensation::systemDefault: return 0;
                case Desktop::OverscanCompensation::scale:         return 1;
                case Desktop::OverscanCompensation::insetBounds:   return 2;
                case Desktop::OverscanCompensation::none:          return 3;
            }

            jassertfalse;
            return 0;
        }

        static Desktop::OverscanCompensation getOverscanCompensation (int index)
        {
            switch (index)
            {
                case 0:  return Desktop::OverscanCompensation::systemDefault;
                case 1:  return Desktop::OverscanCompensation::scale;
                case 2:  return Desktop::OverscanCompensation::insetBounds;
                case 3:  return Desktop::OverscanCompensation::none;
                default: break;
            }

            jassertfalse;
            return Desktop::OverscanCompensation::systemDefault;
        }

        void refreshOverscanInsets()
        {
            const auto* display = Desktop::getInstance().getDisplays().getPrimaryDisplay();

            if (display == nullptr)
            {
                overscanInsetsLabel.setText ("Insets unavailable", NotificationType::dontSendNotification);
                return;
            }

            const auto insets = display->overscanCompensationInsets;
            overscanInsetsLabel.setText ("Insets: T " + String (insets.getTop())
                                           + " L " + String (insets.getLeft())
                                           + " B " + String (insets.getBottom())
                                           + " R " + String (insets.getRight()),
                                         NotificationType::dontSendNotification);
        }
       #endif

        MainComponent& mainComponent;
        ComponentPeer* peer = nullptr;

        Label titleLabel       { {}, "Graphics" },
              lookAndFeelLabel { {}, "LookAndFeel:" },
              rendererLabel    { {}, "Renderer:" };

        ComboBox lookAndFeelSelector, rendererSelector;
       #if JUCE_TVOS
        Label overscanCompensationLabel { {}, "Overscan:" },
              overscanInsetsLabel;
        ComboBox overscanCompensationSelector;
       #endif
        StringArray lookAndFeelNames;
        OwnedArray<LookAndFeel> lookAndFeels;
    };

    class AudioSettingsGroup final : public Component
    {
    public:
        AudioSettingsGroup()
            : deviceSelectorComp (getSharedAudioDeviceManager(), 0, 256, 0, 256, true, true, true, false)
        {
            addAndMakeVisible (titleLabel);
            titleLabel.setFont (FontOptions { titleLabelFontHeight });

            addAndMakeVisible (deviceSelectorComp);
            deviceSelectorComp.setItemHeight (itemHeight);

            setFocusContainerType (FocusContainerType::focusContainer);
            setTitle ("Audio Settings");
        }

        void resized() override
        {
            auto bounds = getLocalBounds();

            titleLabel.setBounds (bounds.removeFromTop (itemHeight));
            bounds.removeFromTop (itemSpacing);

            deviceSelectorComp.setBounds (bounds);
        }

    private:
        Label titleLabel { {}, "Audio" };
        AudioDeviceSelectorComponent deviceSelectorComp;
    };

    //==============================================================================
    class InnerContent final : public Component
    {
    public:
        InnerContent (MainComponent& mainComponent)
            : graphicsSettings (mainComponent)
        {
            addAndMakeVisible (graphicsSettings);
            addAndMakeVisible (audioSettings);

            setOpaque (true);
        }

        void paint (Graphics& g) override
        {
            g.fillAll (findColour (ResizableWindow::backgroundColourId).contrasting (0.2f));
        }

        void resized() override
        {
            auto bounds = getLocalBounds();

           #if JUCE_TVOS
            graphicsSettings.setBounds (bounds.removeFromTop (190));
           #else
            graphicsSettings.setBounds (bounds.removeFromTop (150));
           #endif
            audioSettings.setBounds (bounds);
        }

        std::unique_ptr<AccessibilityHandler> createAccessibilityHandler() override
        {
            return createIgnoredAccessibilityHandler (*this);
        }

    private:
        GraphicsSettingsGroup graphicsSettings;
        AudioSettingsGroup audioSettings;
    };

    Viewport settingsViewport;
    InnerContent innerContent;
};
