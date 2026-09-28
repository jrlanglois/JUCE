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

#include "Box2DSamples.h"
#include "HostControlsBridge.h"
#include "host_controls.h"

namespace Box2DSamples
{

namespace
{

String getDisplayText (const char* label)
{
    return translate (String::fromUTF8 (label != nullptr ? label : "").upToFirstOccurrenceOf ("##", false, false));
}

String formatText (const char* format, va_list arguments)
{
    if (format == nullptr)
        return {};

    const auto translatedFormat = translate (String::fromUTF8 (format));
    std::array<char, 2048> buffer = {};
    va_list argumentsCopy;
    va_copy (argumentsCopy, arguments);
    std::vsnprintf (buffer.data(), buffer.size(), translatedFormat.toRawUTF8(), argumentsCopy);
    va_end (argumentsCopy);
    buffer.back() = 0;
    return String::fromUTF8 (buffer.data());
}

int getNumDecimalPlaces (const char* format)
{
    const String formatString = String::fromUTF8 (format != nullptr ? format : "");
    const int decimalPoint = formatString.indexOfChar ('.');

    if (decimalPoint < 0)
        return 3;

    int numDecimalPlaces = 0;

    for (int index = decimalPoint + 1; index < formatString.length(); ++index)
    {
        if (! CharacterFunctions::isDigit (formatString[index]))
            break;

        ++numDecimalPlaces;
    }

    return numDecimalPlaces;
}

} // namespace

struct HostControlState final
{
    bool boolValue = false,
         changed = false;
    float floatValue = 0.0f;
    int intValue = 0;
    b2Vec2 pairValue = {};
};

class HostControlSession final
{
public:
    void beginFrame (ControlPanel& newControlPanel)
    {
        controlPanel = &newControlPanel;
        occurrences.clear();
    }

    void endFrame()
    {
        controlPanel = nullptr;
    }

    bool showButton (const char* label)
    {
        auto [identifier, state] = getState ("button", label);
        const bool wasActivated = std::exchange (state.changed, false);

        if (controlPanel != nullptr)
            controlPanel->showButton (identifier, getDisplayText (label), [statePointer = &state] { statePointer->changed = true; });

        return wasActivated;
    }

    bool showToggle (const char* label, bool* value)
    {
        if (value == nullptr)
            return false;

        auto [identifier, state] = getState ("toggle", label);
        const bool wasEdited = std::exchange (state.changed, false);

        if (wasEdited)
            *value = state.boolValue;
        else
            state.boolValue = *value;

        if (controlPanel != nullptr)
            controlPanel->showToggle (identifier, getDisplayText (label), state.boolValue, [statePointer = &state] { statePointer->changed = true; });

        return wasEdited;
    }

    bool showRadioButton (const char* label, bool selected)
    {
        auto [identifier, state] = getState ("radio", label);
        const bool wasActivated = std::exchange (state.changed, false);

        if (controlPanel != nullptr)
            controlPanel->showRadioButton (identifier, getDisplayText (label), selected, [statePointer = &state] { statePointer->changed = true; });

        return wasActivated;
    }

    bool showFloatSlider (const char* label, float* value, float minValue, float maxValue, const char* format)
    {
        if (value == nullptr)
            return false;

        auto [identifier, state] = getState ("float", label);
        const bool wasEdited = std::exchange (state.changed, false);

        if (wasEdited)
            *value = state.floatValue;
        else
            state.floatValue = *value;

        if (controlPanel != nullptr)
            controlPanel->showFloatSlider (identifier, getDisplayText (label), state.floatValue, minValue, maxValue, getNumDecimalPlaces (format), [statePointer = &state] { statePointer->changed = true; });

        return wasEdited;
    }

    bool showFloatPairSliders (const char* label, float* values, float minValue, float maxValue, const char* format)
    {
        if (values == nullptr)
            return false;

        auto [identifier, state] = getState ("floatPair", label);
        const bool wasEdited = std::exchange (state.changed, false);

        if (wasEdited)
        {
            values[0] = state.pairValue.x;
            values[1] = state.pairValue.y;
        }
        else
        {
            state.pairValue = { values[0], values[1] };
        }

        if (controlPanel != nullptr)
            controlPanel->showFloatPairSliders (identifier, getDisplayText (label), state.pairValue, minValue, maxValue, getNumDecimalPlaces (format), [statePointer = &state] { statePointer->changed = true; });

        return wasEdited;
    }

    bool showIntegerSlider (const char* label, int* value, int minValue, int maxValue)
    {
        if (value == nullptr)
            return false;

        auto [identifier, state] = getState ("integer", label);
        const bool wasEdited = std::exchange (state.changed, false);

        if (wasEdited)
            *value = state.intValue;
        else
            state.intValue = *value;

        if (controlPanel != nullptr)
            controlPanel->showIntegerSlider (identifier, getDisplayText (label), state.intValue, minValue, maxValue, [statePointer = &state] { statePointer->changed = true; });

        return wasEdited;
    }

    bool showChoice (const char* label, int* index, const char* const* items, int itemCount)
    {
        if (index == nullptr)
            return false;

        auto [identifier, state] = getState ("choice", label);
        const bool wasEdited = std::exchange (state.changed, false);

        if (wasEdited)
            *index = state.intValue;
        else
            state.intValue = *index;

        StringArray itemNames;

        for (int itemIndex = 0; itemIndex < itemCount; ++itemIndex)
            itemNames.add (translate (String::fromUTF8 (items[itemIndex])));

        if (controlPanel != nullptr)
            controlPanel->showChoice (identifier, getDisplayText (label), state.intValue, itemNames, [statePointer = &state] { statePointer->changed = true; });

        return wasEdited;
    }

    void showText (const String& text, ControlPanel::TextTone tone = ControlPanel::TextTone::standard)
    {
        auto [identifier, state] = getState ("text", text.toRawUTF8());
        ignoreUnused (state);

        if (controlPanel != nullptr)
            controlPanel->showText (identifier, text, tone);
    }

    void showSeparator()
    {
        auto [identifier, state] = getState ("separator", "");
        ignoreUnused (state);

        if (controlPanel != nullptr)
            controlPanel->showSeparator (identifier);
    }

    void showProgress (float fraction, const char* overlay)
    {
        auto [identifier, state] = getState ("progress", overlay);
        ignoreUnused (state);

        if (controlPanel != nullptr)
            controlPanel->showProgress (identifier, translate (String::fromUTF8 (overlay != nullptr ? overlay : "")), fraction);
    }

private:
    std::pair<String, HostControlState&> getState (const String& kind, const char* label)
    {
        const String sourceLabel = String::fromUTF8 (label != nullptr ? label : "");
        const String occurrenceKey = kind + "\n" + sourceLabel;
        const int occurrence = occurrences[occurrenceKey]++;
        const String identifier = kind + "." + sourceLabel + "." + String (occurrence);
        return { identifier, states[identifier] };
    }

    ControlPanel* controlPanel = nullptr;
    std::map<String, int> occurrences;
    std::map<String, HostControlState> states;
};

thread_local HostControlSession* activeHostControlSession = nullptr;

HostControlsBridge::HostControlsBridge()
    : session (std::make_unique<HostControlSession>())
{
}

HostControlsBridge::~HostControlsBridge()
{
    if (activeHostControlSession == session.get())
        activeHostControlSession = nullptr;
}

void HostControlsBridge::beginFrame (ControlPanel& controlPanel)
{
    jassert (activeHostControlSession == nullptr);
    session->beginFrame (controlPanel);
    activeHostControlSession = session.get();
}

void HostControlsBridge::endFrame()
{
    jassert (activeHostControlSession == session.get());
    activeHostControlSession = nullptr;
    session->endFrame();
}

} // namespace Box2DSamples

namespace HostControls
{

bool beginPanel (const char* name)
{
    if (Box2DSamples::activeHostControlSession != nullptr)
        Box2DSamples::activeHostControlSession->showText (Box2DSamples::getDisplayText (name), Box2DSamples::ControlPanel::TextTone::heading);

    return true;
}

void endPanel() {}

bool button (const char* label)
{
    return Box2DSamples::activeHostControlSession != nullptr && Box2DSamples::activeHostControlSession->showButton (label);
}

bool checkbox (const char* label, bool* value)
{
    return Box2DSamples::activeHostControlSession != nullptr && Box2DSamples::activeHostControlSession->showToggle (label, value);
}

bool radioButton (const char* label, bool selected)
{
    return Box2DSamples::activeHostControlSession != nullptr && Box2DSamples::activeHostControlSession->showRadioButton (label, selected);
}

bool sliderFloat (const char* label, float* value, float minValue, float maxValue, const char* format)
{
    return Box2DSamples::activeHostControlSession != nullptr && Box2DSamples::activeHostControlSession->showFloatSlider (label, value, minValue, maxValue, format);
}

bool sliderFloat2 (const char* label, float* values, float minValue, float maxValue, const char* format)
{
    return Box2DSamples::activeHostControlSession != nullptr && Box2DSamples::activeHostControlSession->showFloatPairSliders (label, values, minValue, maxValue, format);
}

bool sliderInt (const char* label, int* value, int minValue, int maxValue, const char*)
{
    return Box2DSamples::activeHostControlSession != nullptr && Box2DSamples::activeHostControlSession->showIntegerSlider (label, value, minValue, maxValue);
}

void textV (const char* format, va_list arguments)
{
    if (Box2DSamples::activeHostControlSession != nullptr)
        Box2DSamples::activeHostControlSession->showText (Box2DSamples::formatText (format, arguments));
}

void textDisabledV (const char* format, va_list arguments)
{
    if (Box2DSamples::activeHostControlSession != nullptr)
        Box2DSamples::activeHostControlSession->showText (Box2DSamples::formatText (format, arguments), Box2DSamples::ControlPanel::TextTone::secondary);
}

void textUnformatted (const char* text)
{
    if (Box2DSamples::activeHostControlSession != nullptr)
        Box2DSamples::activeHostControlSession->showText (translate (String::fromUTF8 (text != nullptr ? text : "")));
}

void textColoredV (HostVec4, const char* format, va_list arguments)
{
    if (Box2DSamples::activeHostControlSession != nullptr)
        Box2DSamples::activeHostControlSession->showText (Box2DSamples::formatText (format, arguments), Box2DSamples::ControlPanel::TextTone::warning);
}

void separator()
{
    if (Box2DSamples::activeHostControlSession != nullptr)
        Box2DSamples::activeHostControlSession->showSeparator();
}

void spacing() {}
void dummy (HostVec2) {}
void pushItemWidth (float) {}
void popItemWidth() {}
void pushStyleColor (int, HostVec4) {}
void popStyleColor (int) {}
void sameLine (float, float) {}

bool collapsingHeader (const char* label, int)
{
    if (Box2DSamples::activeHostControlSession != nullptr)
        Box2DSamples::activeHostControlSession->showText (Box2DSamples::getDisplayText (label), Box2DSamples::ControlPanel::TextTone::heading);

    return true;
}

bool beginTabBar (const char*, int) { return true; }
void endTabBar() {}

bool beginTabItem (const char* label, bool* open, int)
{
    if (open != nullptr && ! *open)
        return false;

    return collapsingHeader (label, 0);
}

void endTabItem() {}

bool combo (const char* label, int* index, const char* const* items, int itemCount)
{
    return Box2DSamples::activeHostControlSession != nullptr && Box2DSamples::activeHostControlSession->showChoice (label, index, items, itemCount);
}

bool inputText (const char*, char*, int) { return false; }

void progressBar (float fraction, HostVec2, const char* overlay)
{
    if (Box2DSamples::activeHostControlSession != nullptr)
        Box2DSamples::activeHostControlSession->showProgress (fraction, overlay);
}

void openPopup (const char*) {}
bool beginPopupModal (const char* name, bool* open, int) { return beginTabItem (name, open, 0); }
void closeCurrentPopup() {}
bool selectable (const char* label, bool selected, int) { return radioButton (label, selected); }
bool isKeyPressed (int, bool) { return false; }
float getFontSize() { return 16.0f; }
float getFrameHeight() { return 20.0f; }
void setItemTooltipV (const char*, va_list) {}
bool begin (const char* name, bool* open, int) { return beginTabItem (name, open, 0); }
void end() {}
bool beginTable (const char*, int, int, float, HostVec2) { return true; }
void endTable() {}
void tableNextRow (int, float) {}
bool tableNextColumn() { return true; }
void tableSetupColumn (const char*, int, float) {}
void tableHeadersRow() {}
void setCursorPosX (float) {}
HostVec2 getCursorScreenPos() { return {}; }
HostVec2 getItemRectMin() { return {}; }
HostVec2 getItemRectMax() { return {}; }
float getContentRegionAvailX() { return 0.0f; }
HostVec2 getContentRegionAvail() { return {}; }
float getTextLineHeight() { return 16.0f; }

HostDrawList* getWindowDrawList()
{
    static HostDrawList drawList;
    return &drawList;
}

void setNextWindowPos (HostVec2, int, HostVec2) {}
void setNextWindowSize (HostVec2, int) {}
void setNextWindowBgAlpha (float) {}

bool treeNodeExV (const char*, int, const char* label, va_list arguments)
{
    textV (label, arguments);
    return true;
}

void treePop() {}

bool menuItem (const char* label, const char*, bool* selected, bool enabled)
{
    return enabled && checkbox (label, selected);
}

bool beginMenu (const char* label, bool enabled)
{
    return enabled && collapsingHeader (label, 0);
}

void endMenu() {}
bool beginMainMenuBar() { return true; }
void endMainMenuBar() {}
bool beginChild (const char*, HostVec2, int, int) { return true; }
void endChild() {}
bool beginListBox (const char*, HostVec2) { return true; }
void endListBox() {}
void setScrollHereY (float) {}
bool isItemHovered (int) { return false; }
uint32_t getColorU32 (int, float) { return 0; }
HostVec4 getStyleColorVec4 (int) { return {}; }
float getTextLineHeightWithSpacing() { return 20.0f; }

} // namespace HostControls
