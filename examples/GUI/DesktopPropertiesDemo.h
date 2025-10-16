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

 name:             DesktopPropertiesDemo
 version:          1.0.0
 vendor:           JUCE
 website:          http://juce.com
 description:      Displays desktop properties, system colours and accent colours.

 dependencies:     juce_core, juce_data_structures, juce_events, juce_graphics,
                   juce_gui_basics
 exporters:        xcode_mac, vs2022, vs2026, linux_make, androidstudio,
                   xcode_iphone

 moduleFlags:      JUCE_STRICT_REFCOUNTEDPOINTER=1

 type:             Component
 mainClass:        DesktopPropertiesDemo

 useLocalCopy:     1

 END_JUCE_PIP_METADATA

*******************************************************************************/

#pragma once

#include "../Assets/DemoUtilities.h"

//==============================================================================
class DesktopPropertiesDemo final : public Component,
                                    public DarkModeSettingListener,
                                    public AccentColourListener,
                                    private Timer
{
public:
    static inline constexpr int margin = 6;

    struct PropertyItem
    {
        enum class Type { text, colour };

        String name;
        Type type = Type::text;
        std::function<String()> getTextValue;
        std::function<Colour()> getColourValue;

        static PropertyItem makeText (String n, std::function<String()> getter)
        {
            return { n, Type::text, std::move (getter), nullptr };
        }

        static PropertyItem makeColour (String n, std::function<Colour()> getter)
        {
            return { n, Type::colour, nullptr, std::move (getter) };
        }
    };

    //==============================================================================
    class PropertiesListModel final : public ListBoxModel
    {
    public:
        PropertiesListModel()
        {
            refreshItems();
        }

        void refreshItems()
        {
            items.clear();

            auto addSeparator = [this] (const String& title)
            {
                items.add (PropertyItem::makeText (title, [] { return ""; }));
            };

            auto addText = [this] (const String& name, std::function<String()> getter)
            {
                items.add (PropertyItem::makeText (name, std::move (getter)));
            };

            auto addColour = [this] (const String& name, std::function<Colour()> getter)
            {
                items.add (PropertyItem::makeColour (name, std::move (getter)));
            };

            // Desktop state
            addSeparator ("--- Desktop State ---");
            addText ("Dark Mode Active", [] { return Desktop::getInstance().isDarkModeActive() ? "Yes" : "No"; });
            addText ("Transparency Effects", [] { return Desktop::getInstance().areTransparencyEffectsEnabled() ? "Enabled" : "Disabled"; });
            addText ("Screen Saver Enabled", [] { return Desktop::isScreenSaverEnabled() ? "Yes" : "No"; });
            addText ("Headless", [] { return Desktop::getInstance().isHeadless() ? "Yes" : "No"; });
            addText ("Kiosk Mode", [] { return Desktop::getInstance().getKioskModeComponent() != nullptr ? "Active" : "Inactive"; });

            // Statistics
            addSeparator ("--- Statistics ---");
            addText ("Num Components", [] { return String (Desktop::getInstance().getNumComponents()); });
            addText ("Num Mouse Sources", [] { return String (Desktop::getInstance().getNumMouseSources()); });
            addText ("Num Dragging Mouse Sources", [] { return String (Desktop::getInstance().getNumDraggingMouseSources()); });
            addText ("Mouse Click Counter", [] { return String (Desktop::getInstance().getMouseButtonClickCounter()); });
            addText ("Mouse Wheel Counter", [] { return String (Desktop::getInstance().getMouseWheelMoveCounter()); });
            addText ("Num Displays", [] { return String (Desktop::getInstance().getDisplays().displays.size()); });

            // Display
            addSeparator ("--- Display ---");
            addText ("Global Scale Factor", [] { return String (Desktop::getInstance().getGlobalScaleFactor(), 2); });
            addText ("Current Orientation", []
            {
                switch (Desktop::getInstance().getCurrentOrientation())
                {
                    case Desktop::upright:              return "Upright";
                    case Desktop::upsideDown:           return "Upside Down";
                    case Desktop::rotatedClockwise:     return "Rotated Clockwise";
                    case Desktop::rotatedAntiClockwise: return "Rotated Anti-Clockwise";
                    default:                            return "Unknown";
                }
            });

            // System colours
            addSeparator ("--- System Colours ---");
            addColour ("Background", [] { return Desktop::getInstance().getSystemColour (Desktop::SystemColourType::background); });
            addColour ("Foreground", [] { return Desktop::getInstance().getSystemColour (Desktop::SystemColourType::foreground); });
            addColour ("Complement", [] { return Desktop::getInstance().getSystemColour (Desktop::SystemColourType::complement); });

            // Accent colour
            addSeparator ("--- Accent Colours ---");
            addColour ("Accent", [] { return Desktop::getInstance().getAccentColour(); });
            addColour ("Accent Dark 1", [] { return Desktop::getInstance().getSystemColour (Desktop::SystemColourType::accentDark1); });
            addColour ("Accent Dark 2", [] { return Desktop::getInstance().getSystemColour (Desktop::SystemColourType::accentDark2); });
            addColour ("Accent Dark 3", [] { return Desktop::getInstance().getSystemColour (Desktop::SystemColourType::accentDark3); });
            addColour ("Accent Light 1", [] { return Desktop::getInstance().getSystemColour (Desktop::SystemColourType::accentLight1); });
            addColour ("Accent Light 2", [] { return Desktop::getInstance().getSystemColour (Desktop::SystemColourType::accentLight2); });
            addColour ("Accent Light 3", [] { return Desktop::getInstance().getSystemColour (Desktop::SystemColourType::accentLight3); });
        }

        int getNumRows() override { return items.size(); }

        void paintListBoxItem (int rowNumber, Graphics& g, int width, int height, bool rowIsSelected) override
        {
            if (! isPositiveAndBelow (rowNumber, items.size()))
                return;

            auto& item = items.getReference (rowNumber);
            Rectangle<int> bounds (width, height);
            const FontOptions fontOptions (16.0f);

            if (item.type == PropertyItem::Type::colour && item.getColourValue)
            {
                // For colour items, fill the background with the colour
                auto colour = item.getColourValue();

                if (colour.isTransparent())
                {
                    // Show a checkerboard pattern for invalid/transparent colours
                    g.fillCheckerBoard (bounds.toFloat(), (float) margin, (float) margin, Colour (0xff444444), Colour (0xff666666));

                    g.setColour (Colours::white);
                    g.setFont (Font (fontOptions).withStyle (Font::bold));
                    g.drawText (item.name, bounds.reduced (margin, 0), Justification::centredLeft, true);

                    g.setColour (Colours::white);
                    g.setFont (fontOptions);
                    g.drawText ("N/A", bounds.reduced (margin, 0), Justification::centredRight, true);
                }
                else
                {
                    g.fillAll (colour);

                    // Draw the colour name with contrasting text
                    auto textColour = colour.contrasting();
                    g.setColour (textColour);
                    g.setFont (Font (fontOptions).boldened());
                    g.drawText (item.name, bounds.reduced (margin, 0), Justification::centredLeft, true);

                    // Draw RGB values
                    g.setFont (fontOptions.withName (Font::getDefaultMonospacedFontName()));
                    g.drawText (colour.toString(), bounds.reduced (margin, 0), Justification::centredRight, true);
                }
            }
            else if (item.type == PropertyItem::Type::text)
            {
                // For text items, use standard list box styling
                if (rowIsSelected)
                    g.fillAll (Colour (0xff4f94cd));

                g.setColour (Desktop::getInstance().getDefaultLookAndFeel().findColour (ListBox::textColourId));
                g.setFont (FontOptions (15.0f, item.name.contains ("---") ? Font::plain : Font::bold));

                auto textBounds = bounds.reduced (margin, 0);
                g.drawText (item.name, textBounds.removeFromLeft (width / 2), Justification::centredLeft, true);

                if (item.getTextValue != nullptr)
                {
                    g.setFont (fontOptions);
                    g.drawText (item.getTextValue(), textBounds, Justification::centredRight, true);
                }
            }
        }

        Component* refreshComponentForRow (int, bool, Component* existingComponentToUpdate) override
        {
            deleteAndZero (existingComponentToUpdate);
            return nullptr;
        }

    private:
        Array<PropertyItem> items;
    };

    //==============================================================================
    DesktopPropertiesDemo()
    {
        setOpaque (true);

        addAndMakeVisible (listBox);
        listBox.setModel (&listModel);
        listBox.setRowHeight (48);

        addAndMakeVisible (infoLabel);
        infoLabel.setText ("Desktop properties, statistics, system colours and accent colours. "
                           "The statistics update in real-time. On Windows, try changing the accent colour!",
                           dontSendNotification);
        infoLabel.setJustificationType (Justification::centred);
        infoLabel.setFont (FontOptions (14.0f));

        addAndMakeVisible (lastChangeLabel);
        updateLastChangeLabel (ChangeType::none);
        lastChangeLabel.setJustificationType (Justification::centred);
        lastChangeLabel.setFont (FontOptions (12.0f, Font::italic));

        Desktop::getInstance().addDarkModeSettingListener (this);
        Desktop::getInstance().addAccentColourListener (this);

        startTimer (100);

        setSize (600, 600);
    }

    ~DesktopPropertiesDemo() override
    {
        Desktop::getInstance().removeAccentColourListener (this);
        Desktop::getInstance().removeDarkModeSettingListener (this);
    }

    void paint (Graphics& g) override
    {
        g.fillAll (getUIColourIfAvailable (LookAndFeel_V4::ColourScheme::UIColour::windowBackground));
    }

    void resized() override
    {
        auto bounds = getLocalBounds().reduced (margin);

        infoLabel.setBounds (bounds.removeFromTop (listBox.getRowHeight()));
        bounds.removeFromTop (margin);

        lastChangeLabel.setBounds (bounds.removeFromTop (listBox.getRowHeight()));
        bounds.removeFromTop (margin);

        listBox.setBounds (bounds);
    }

    void darkModeSettingChanged() override
    {
        listModel.refreshItems();
        listBox.repaint();
        updateLastChangeLabel (ChangeType::darkMode);
    }

    void accentColourChanged() override
    {
        listModel.refreshItems();
        listBox.repaint();
        updateLastChangeLabel (ChangeType::accentColour);
    }

    void timerCallback() override
    {
        listBox.repaint();
    }

private:
    //==============================================================================
    enum class ChangeType { none, darkMode, accentColour };

    //==============================================================================
    PropertiesListModel listModel;
    ListBox listBox { "Desktop Properties", &listModel };
    Label infoLabel, lastChangeLabel;

    //==============================================================================
    void updateLastChangeLabel (ChangeType changeType)
    {
        if (changeType == ChangeType::darkMode)
        {
            lastChangeLabel.setText ("Dark mode setting changed at " + Time::getCurrentTime().toString (true, true), dontSendNotification);
            lastChangeLabel.setColour (Label::textColourId, Colours::lightblue);
        }
        else if (changeType == ChangeType::accentColour)
        {
            lastChangeLabel.setText ("Accent colour updated at " + Time::getCurrentTime().toString (true, true), dontSendNotification);
            lastChangeLabel.setColour (Label::textColourId, Colours::lightgreen);
        }
        else
        {
            lastChangeLabel.setText ("Listening for system changes...", dontSendNotification);
            lastChangeLabel.setColour (Label::textColourId, Colours::grey);
        }
    }

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DesktopPropertiesDemo)
};


