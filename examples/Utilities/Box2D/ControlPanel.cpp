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

namespace Box2DSamples
{

namespace
{

constexpr int standardControlRowHeight = 32;
constexpr int separatorRowHeight = 8;

struct ControlRow final
{
    enum class Kind
    {
        action,
        toggle,
        slider,
        choice,
        text,
        separator,
        progress
    };

    Kind kind = Kind::text;
    ControlPanel::TextTone textTone = ControlPanel::TextTone::standard;
    String identifier,
           text;
    StringArray choices;
    std::function<void()> action,
                          editAction;
    bool* boolTarget = nullptr;
    float* floatTarget = nullptr;
    int* intTarget = nullptr;
    double minValue = 0.0,
           maxValue = 1.0,
           interval = 0.0,
           progressValue = 0.0;
    int numDecimalPlaces = 0;
    bool enabled = true,
         displayBool = false,
         queuedBool = false,
         hasQueuedBool = false,
         hasQueuedFloat = false,
         hasQueuedInt = false,
         isButtonQueued = false;
    float queuedFloat = 0.0f;
    int queuedInt = 0;
};

class ActionProperty final : public PropertyComponent
{
public:
    explicit ActionProperty (std::shared_ptr<ControlRow> rowIn) :
        PropertyComponent ({}, standardControlRowHeight),
        row (std::move (rowIn))
    {
        addAndMakeVisible (button);
        button.onClick = [this] { row->isButtonQueued = true; };
        refresh();
    }

    void refresh() override
    {
        setTitle (row->text);
        button.setTitle (row->text);
        button.setButtonText (row->text);
        button.setEnabled (row->enabled);
    }

    void resized() override { button.setBounds (getLocalBounds().reduced (4, 2)); }

private:
    std::shared_ptr<ControlRow> row;
    TextButton button;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ActionProperty)
};

class ToggleProperty final : public BooleanPropertyComponent
{
public:
    explicit ToggleProperty (std::shared_ptr<ControlRow> rowIn) :
        BooleanPropertyComponent (rowIn->text, {}, {}),
        row (std::move (rowIn))
    {
        setPreferredHeight (standardControlRowHeight);
        refresh();
    }

    void setState (bool newState) override
    {
        if (row->boolTarget != nullptr)
        {
            row->queuedBool = newState;
            row->hasQueuedBool = true;
        }
        else
        {
            row->isButtonQueued = true;
        }
    }

    bool getState() const override { return row->boolTarget != nullptr ? *row->boolTarget : row->displayBool; }

    void refresh() override
    {
        setName (row->text);
        setTitle (row->text);
        setEnabled (row->enabled);
        BooleanPropertyComponent::refresh();
    }

private:
    std::shared_ptr<ControlRow> row;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ToggleProperty)
};

class SliderProperty final : public SliderPropertyComponent
{
public:
    explicit SliderProperty (std::shared_ptr<ControlRow> rowIn) :
        SliderPropertyComponent (rowIn->text, rowIn->minValue, rowIn->maxValue, rowIn->interval),
        row (std::move (rowIn))
    {
        setPreferredHeight (standardControlRowHeight);
        refresh();
    }

    void setValue (double newValue) override
    {
        if (row->floatTarget != nullptr)
        {
            row->queuedFloat = (float) newValue;
            row->hasQueuedFloat = true;
        }
        else if (row->intTarget != nullptr)
        {
            row->queuedInt = roundToInt (newValue);
            row->hasQueuedInt = true;
        }
    }

    double getValue() const override
    {
        if (row->floatTarget != nullptr)
            return *row->floatTarget;

        return row->intTarget != nullptr ? *row->intTarget : 0.0;
    }

    void refresh() override
    {
        setName (row->text);
        setTitle (row->text);
        setEnabled (row->enabled);
        slider.setRange (row->minValue, row->maxValue, row->interval);
        slider.setNumDecimalPlacesToDisplay (row->numDecimalPlaces);
        SliderPropertyComponent::refresh();
    }

private:
    std::shared_ptr<ControlRow> row;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SliderProperty)
};

class ChoiceProperty final : public ChoicePropertyComponent
{
public:
    explicit ChoiceProperty (std::shared_ptr<ControlRow> rowIn) :
        ChoicePropertyComponent (rowIn->text),
        row (std::move (rowIn))
    {
        choices = row->choices;
        setPreferredHeight (standardControlRowHeight);
        refresh();
    }

    void setIndex (int newIndex) override
    {
        row->queuedInt = newIndex;
        row->hasQueuedInt = true;
    }

    int getIndex() const override { return row->intTarget != nullptr ? *row->intTarget : -1; }

    void refresh() override
    {
        setName (row->text);
        setTitle (row->text);
        setEnabled (row->enabled);
        ChoicePropertyComponent::refresh();
    }

private:
    std::shared_ptr<ControlRow> row;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChoiceProperty)
};

class ReadOnlyProperty final : public PropertyComponent
{
public:
    explicit ReadOnlyProperty (std::shared_ptr<ControlRow> rowIn) :
        PropertyComponent ({}, standardControlRowHeight),
        row (std::move (rowIn))
    {
        label.setJustificationType (Justification::centredLeft);
        label.setMinimumHorizontalScale (0.65f);
        addAndMakeVisible (label);
        refresh();
    }

    void refresh() override
    {
        setTitle (row->text);
        label.setTitle (row->text);
        label.setText (row->text, dontSendNotification);
        label.setColour (Label::textColourId,
                         row->textTone == ControlPanel::TextTone::secondary
                             ? Colours::grey
                             : findColour (PropertyComponent::labelTextColourId));
    }

    void resized() override { label.setBounds (getLocalBounds().reduced (6, 2)); }

private:
    std::shared_ptr<ControlRow> row;
    Label label;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ReadOnlyProperty)
};

class SeparatorProperty final : public PropertyComponent
{
public:
    explicit SeparatorProperty (std::shared_ptr<ControlRow> rowIn) :
        PropertyComponent ({}, separatorRowHeight),
        row (std::move (rowIn))
    {
        setAccessible (false);
    }

    void refresh() override {}

    void paint (Graphics& graphics) override
    {
        graphics.setColour (findColour (PropertyComponent::labelTextColourId).withAlpha (0.25f));
        graphics.fillRect (getLocalBounds().reduced (6, 3));
    }

private:
    std::shared_ptr<ControlRow> row;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SeparatorProperty)
};

class ProgressProperty final : public PropertyComponent
{
public:
    explicit ProgressProperty (std::shared_ptr<ControlRow> rowIn) :
        PropertyComponent (rowIn->text, standardControlRowHeight),
        row (std::move (rowIn)),
        progressBar (row->progressValue, ProgressBar::Style::linear)
    {
        progressBar.setPercentageDisplay (true);
        addAndMakeVisible (progressBar);
        refresh();
    }

    void refresh() override
    {
        setName (row->text);
        setTitle (row->text);
        progressBar.setTitle (row->text);
        progressBar.repaint();
    }

private:
    std::shared_ptr<ControlRow> row;
    ProgressBar progressBar;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ProgressProperty)
};

PropertyComponent* createProperty (const std::shared_ptr<ControlRow>& row)
{
    switch (row->kind)
    {
        case ControlRow::Kind::action:    return new ActionProperty (row);
        case ControlRow::Kind::toggle:    return new ToggleProperty (row);
        case ControlRow::Kind::slider:    return new SliderProperty (row);
        case ControlRow::Kind::choice:    return new ChoiceProperty (row);
        case ControlRow::Kind::text:      return new ReadOnlyProperty (row);
        case ControlRow::Kind::separator: return new SeparatorProperty (row);
        case ControlRow::Kind::progress:  return new ProgressProperty (row);
    }

    jassertfalse;
    return nullptr;
}

} // namespace

class ControlPanel::Pimpl
{
public:
    ControlRow& prepareRow (const String& identifier, ControlRow::Kind kind)
    {
        auto& row = rows[identifier];

        if (row == nullptr || row->kind != kind)
        {
            row = std::make_shared<ControlRow>();
            row->identifier = identifier;
            row->kind = kind;
            ++revision;
        }

        liveIdentifiers.push_back (identifier);
        return *row;
    }

    void setRowText (ControlRow& row, const String& text)
    {
        if (row.textTone == TextTone::heading && row.text != text)
            ++revision;

        row.text = text;
    }

    std::map<String, std::shared_ptr<ControlRow>> rows;
    std::vector<String> liveIdentifiers,
                        previousIdentifiers;
    uint64 revision = 0;
    bool isInFrame = false;
};

ControlPanel::ControlPanel() : pimpl (std::make_unique<Pimpl>()) {}

ControlPanel::~ControlPanel() = default;

void ControlPanel::beginFrame()
{
    pimpl->previousIdentifiers = pimpl->liveIdentifiers;
    pimpl->liveIdentifiers.clear();
    pimpl->isInFrame = true;
}

void ControlPanel::applyQueuedEdits()
{
    for (auto& pair : pimpl->rows)
    {
        auto& row = *pair.second;

        if (row.hasQueuedBool && row.boolTarget != nullptr)
        {
            *row.boolTarget = row.queuedBool;
            row.hasQueuedBool = false;

            if (row.editAction != nullptr)
                row.editAction();
        }

        if (row.hasQueuedFloat && row.floatTarget != nullptr)
        {
            *row.floatTarget = row.queuedFloat;
            row.hasQueuedFloat = false;

            if (row.editAction != nullptr)
                row.editAction();
        }

        if (row.hasQueuedInt && row.intTarget != nullptr)
        {
            *row.intTarget = row.queuedInt;
            row.hasQueuedInt = false;

            if (row.editAction != nullptr)
                row.editAction();
        }

        if (row.isButtonQueued && row.action != nullptr)
        {
            row.isButtonQueued = false;
            row.action();
        }
    }
}

void ControlPanel::clear()
{
    if (! pimpl->rows.empty() || ! pimpl->liveIdentifiers.empty())
        ++pimpl->revision;

    pimpl->rows.clear();
    pimpl->liveIdentifiers.clear();
    pimpl->previousIdentifiers.clear();
}

void ControlPanel::showButton (const String& identifier, const String& text, std::function<void()> action, bool shouldEnable)
{
    jassert (pimpl->isInFrame);
    auto& row = pimpl->prepareRow (identifier, ControlRow::Kind::action);
    pimpl->setRowText (row, text);
    row.action = std::move (action);
    row.editAction = {};
    row.boolTarget = nullptr;
    row.floatTarget = nullptr;
    row.intTarget = nullptr;
    row.enabled = shouldEnable;
}

void ControlPanel::showToggle (const String& identifier, const String& text, bool& value, std::function<void()> editAction, bool shouldEnable)
{
    jassert (pimpl->isInFrame);
    auto& row = pimpl->prepareRow (identifier, ControlRow::Kind::toggle);
    pimpl->setRowText (row, text);
    row.action = {};
    row.boolTarget = &value;
    row.floatTarget = nullptr;
    row.intTarget = nullptr;
    row.editAction = std::move (editAction);
    row.enabled = shouldEnable;
}

void ControlPanel::showFloatSlider (const String& identifier, const String& text, float& value, float minValue, float maxValue, int numDecimalPlaces, std::function<void()> editAction, bool shouldEnable)
{
    jassert (pimpl->isInFrame);
    auto& row = pimpl->prepareRow (identifier, ControlRow::Kind::slider);
    pimpl->setRowText (row, text);
    row.boolTarget = nullptr;
    row.floatTarget = &value;
    row.intTarget = nullptr;
    row.editAction = std::move (editAction);
    row.minValue = minValue;
    row.maxValue = maxValue;
    row.interval = 0.0;
    row.numDecimalPlaces = numDecimalPlaces;
    row.enabled = shouldEnable;
}

void ControlPanel::showFloatPairSliders (const String& identifier, const String& text, b2Vec2& value, float minValue, float maxValue, int numDecimalPlaces, std::function<void()> editAction, bool shouldEnable)
{
    const auto xText = TRANS ("{controlName}, X").replace ("{controlName}", text);
    const auto yText = TRANS ("{controlName}, Y").replace ("{controlName}", text);
    showFloatSlider (identifier + ".x", xText, value.x, minValue, maxValue, numDecimalPlaces, editAction, shouldEnable);
    showFloatSlider (identifier + ".y", yText, value.y, minValue, maxValue, numDecimalPlaces, editAction, shouldEnable);
}

void ControlPanel::showIntegerSlider (const String& identifier, const String& text, int& value, int minValue, int maxValue, std::function<void()> editAction, bool shouldEnable)
{
    jassert (pimpl->isInFrame);
    auto& row = pimpl->prepareRow (identifier, ControlRow::Kind::slider);
    pimpl->setRowText (row, text);
    row.boolTarget = nullptr;
    row.floatTarget = nullptr;
    row.intTarget = &value;
    row.editAction = std::move (editAction);
    row.minValue = minValue;
    row.maxValue = maxValue;
    row.interval = 1.0;
    row.numDecimalPlaces = 0;
    row.enabled = shouldEnable;
}

void ControlPanel::showChoice (const String& identifier, const String& text, int& selectedIndex, const StringArray& itemNames, std::function<void()> editAction, bool shouldEnable)
{
    jassert (pimpl->isInFrame);
    auto& row = pimpl->prepareRow (identifier, ControlRow::Kind::choice);
    pimpl->setRowText (row, text);
    row.boolTarget = nullptr;
    row.floatTarget = nullptr;
    row.intTarget = &selectedIndex;
    row.editAction = std::move (editAction);
    row.enabled = shouldEnable;

    if (row.choices != itemNames)
    {
        row.choices = itemNames;
        ++pimpl->revision;
    }
}

void ControlPanel::showRadioButton (const String& identifier, const String& text, bool isSelected, std::function<void()> action, bool shouldEnable)
{
    jassert (pimpl->isInFrame);
    auto& row = pimpl->prepareRow (identifier, ControlRow::Kind::toggle);
    pimpl->setRowText (row, text);
    row.action = std::move (action);
    row.editAction = {};
    row.boolTarget = nullptr;
    row.floatTarget = nullptr;
    row.intTarget = nullptr;
    row.displayBool = isSelected;
    row.enabled = shouldEnable;
}

void ControlPanel::showList (const String& identifier, int& selectedIndex, const StringArray& itemNames, int numVisibleRows, std::function<void()> editAction, bool shouldEnable)
{
    ignoreUnused (numVisibleRows);
    showChoice (identifier, {}, selectedIndex, itemNames, std::move (editAction), shouldEnable);
}

void ControlPanel::showText (const String& identifier, const String& text, TextTone tone)
{
    jassert (pimpl->isInFrame);
    auto& row = pimpl->prepareRow (identifier, ControlRow::Kind::text);

    if (row.textTone != tone && (row.textTone == TextTone::heading || tone == TextTone::heading))
        ++pimpl->revision;

    row.textTone = tone;
    pimpl->setRowText (row, text);
}

void ControlPanel::showSeparator (const String& identifier)
{
    jassert (pimpl->isInFrame);
    pimpl->prepareRow (identifier, ControlRow::Kind::separator);
}

void ControlPanel::showProgress (const String& identifier, const String& text, double progress)
{
    jassert (pimpl->isInFrame);
    auto& row = pimpl->prepareRow (identifier, ControlRow::Kind::progress);
    pimpl->setRowText (row, text);
    row.progressValue = std::clamp (progress, 0.0, 1.0);
}

void ControlPanel::endFrame()
{
    jassert (pimpl->isInFrame);
    pimpl->isInFrame = false;

    for (auto iterator = pimpl->rows.begin(); iterator != pimpl->rows.end();)
    {
        if (std::find (pimpl->liveIdentifiers.begin(), pimpl->liveIdentifiers.end(), iterator->first) == pimpl->liveIdentifiers.end())
        {
            iterator = pimpl->rows.erase (iterator);
            ++pimpl->revision;
        }
        else
        {
            ++iterator;
        }
    }

    if (pimpl->previousIdentifiers != pimpl->liveIdentifiers)
        ++pimpl->revision;
}

void ControlPanel::appendPropertiesTo (PropertyPanel& propertyPanel, const String& defaultSectionTitle) const
{
    Array<PropertyComponent*> properties;
    auto sectionTitle = defaultSectionTitle;

    const auto addSection = [&propertyPanel, &properties, &sectionTitle]
    {
        if (! properties.isEmpty())
        {
            propertyPanel.addSection (sectionTitle, properties);
            properties.clear();
        }
    };

    for (const auto& identifier : pimpl->liveIdentifiers)
    {
        const auto iterator = pimpl->rows.find (identifier);

        if (iterator == pimpl->rows.end())
            continue;

        const auto& row = iterator->second;

        if (row->kind == ControlRow::Kind::text && row->textTone == TextTone::heading)
        {
            addSection();
            sectionTitle = row->text;
            continue;
        }

        properties.add (createProperty (row));
    }

    addSection();
}

uint64 ControlPanel::getRevision() const noexcept { return pimpl->revision; }

} // namespace Box2DSamples
