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

enum class BindingKind
{
    action,
    boolean,
    floatingPoint,
    integer,
    indexedChoice,
    identifiedChoice,
    text
};

struct ControlBinding final
{
    BindingKind kind = BindingKind::action;
    std::function<void()> action,
                          editAction;
    bool* boolTarget = nullptr;
    float* floatTarget = nullptr;
    int* intTarget = nullptr;
    String* stringTarget = nullptr;
    std::vector<String> choiceIdentifiers;
};

enum class QueuedEditKind
{
    action,
    boolean,
    number,
    choice,
    text
};

struct QueuedEdit final
{
    String identifier,
           stringValue;
    QueuedEditKind kind = QueuedEditKind::action;
    double numberValue = 0.0;
    bool booleanValue = false;
};

using StructureEntry = std::tuple<String, ControlItem::Group, ControlItem::Kind>;

std::vector<StructureEntry> getStructure (const std::vector<ControlItem>& items)
{
    std::vector<StructureEntry> result;
    result.reserve (items.size());

    for (const auto& item : items)
        result.emplace_back (item.identifier, item.group, item.kind);

    return result;
}

bool containsChoice (const ControlItem& item, const String& identifier)
{
    return std::any_of (item.choices.begin(), item.choices.end(), [&identifier] (const auto& choice)
    {
        return choice.identifier == identifier;
    });
}

bool areChoicesValid (Span<const ControlChoice> choices)
{
    for (size_t choiceIndex = 0; choiceIndex < choices.size(); ++choiceIndex)
    {
        if (choices[choiceIndex].identifier.isEmpty())
            return false;

        for (size_t previousIndex = 0; previousIndex < choiceIndex; ++previousIndex)
        {
            if (choices[previousIndex].identifier == choices[choiceIndex].identifier)
                return false;
        }
    }

    return true;
}

} // namespace

struct ControlModel::Pimpl final
{
    ControlItem* appendItem (const String& identifier, ControlItem::Kind kind)
    {
        jassert (isInFrame);
        jassert (identifier.isNotEmpty());

        const auto isDuplicate = std::any_of (buildingItems.begin(), buildingItems.end(), [&identifier] (const auto& item)
        {
            return item.identifier == identifier;
        });
        jassert (! isDuplicate);

        if (! isInFrame || identifier.isEmpty() || isDuplicate)
            return nullptr;

        auto& item = buildingItems.emplace_back();
        item.identifier = identifier;
        item.group = currentGroup;
        item.kind = kind;
        return &item;
    }

    ControlBinding* appendBinding (const String& identifier, BindingKind kind)
    {
        auto [iterator, inserted] = bindings.emplace (identifier, ControlBinding {});
        jassert (inserted);

        if (! inserted)
            return nullptr;

        iterator->second.kind = kind;
        return &iterator->second;
    }

    std::vector<ControlItem> publishedItems,
                             buildingItems;
    std::map<String, ControlBinding> bindings;
    std::vector<QueuedEdit> queuedEdits;
    ControlItem::Group currentGroup = ControlItem::Group::sampleControls;
    uint64 structureRevision = 0;
    bool isInFrame = false,
         hasSelectedGroup = false;
};

ControlModel::ControlModel() : pimpl (std::make_unique<Pimpl>()) {}

ControlModel::~ControlModel() = default;

void ControlModel::applyQueuedEdits()
{
    auto queuedEdits = std::move (pimpl->queuedEdits);
    pimpl->queuedEdits.clear();

    for (const auto& edit : queuedEdits)
    {
        const auto iterator = pimpl->bindings.find (edit.identifier);

        if (iterator == pimpl->bindings.end())
        {
            jassertfalse;
            continue;
        }

        auto& binding = iterator->second;

        switch (edit.kind)
        {
            case QueuedEditKind::action:
                if (binding.kind == BindingKind::action && binding.action != nullptr)
                    binding.action();
                else
                    jassertfalse;
                break;

            case QueuedEditKind::boolean:
                if (binding.kind == BindingKind::boolean && binding.boolTarget != nullptr)
                {
                    *binding.boolTarget = edit.booleanValue;

                    if (binding.editAction != nullptr)
                        binding.editAction();
                }
                else
                {
                    jassertfalse;
                }
                break;

            case QueuedEditKind::number:
                if (binding.kind == BindingKind::floatingPoint && binding.floatTarget != nullptr)
                {
                    *binding.floatTarget = (float) edit.numberValue;

                    if (binding.editAction != nullptr)
                        binding.editAction();
                }
                else if (binding.kind == BindingKind::integer && binding.intTarget != nullptr)
                {
                    *binding.intTarget = roundToInt (edit.numberValue);

                    if (binding.editAction != nullptr)
                        binding.editAction();
                }
                else
                {
                    jassertfalse;
                }
                break;

            case QueuedEditKind::choice:
                if (binding.kind == BindingKind::indexedChoice && binding.intTarget != nullptr)
                {
                    const auto choiceIterator = std::find (binding.choiceIdentifiers.begin(), binding.choiceIdentifiers.end(), edit.stringValue);

                    if (choiceIterator == binding.choiceIdentifiers.end())
                    {
                        jassertfalse;
                        break;
                    }

                    *binding.intTarget = (int) std::distance (binding.choiceIdentifiers.begin(), choiceIterator);

                    if (binding.editAction != nullptr)
                        binding.editAction();
                }
                else if (binding.kind == BindingKind::identifiedChoice && binding.stringTarget != nullptr)
                {
                    *binding.stringTarget = edit.stringValue;

                    if (binding.editAction != nullptr)
                        binding.editAction();
                }
                else
                {
                    jassertfalse;
                }
                break;

            case QueuedEditKind::text:
                if (binding.kind == BindingKind::text && binding.stringTarget != nullptr)
                {
                    *binding.stringTarget = edit.stringValue;

                    if (binding.editAction != nullptr)
                        binding.editAction();
                }
                else
                {
                    jassertfalse;
                }
                break;
        }
    }
}

void ControlModel::beginFrame()
{
    jassert (! pimpl->isInFrame);
    pimpl->buildingItems.clear();
    pimpl->bindings.clear();
    pimpl->currentGroup = ControlItem::Group::sampleControls;
    pimpl->hasSelectedGroup = false;
    pimpl->isInFrame = true;
}

void ControlModel::setGroup (ControlItem::Group group)
{
    jassert (pimpl->isInFrame);

    if (! pimpl->isInFrame)
        return;

    if (pimpl->hasSelectedGroup)
    {
        jassert ((int) group >= (int) pimpl->currentGroup);

        if ((int) group < (int) pimpl->currentGroup)
            return;
    }

    pimpl->currentGroup = group;
    pimpl->hasSelectedGroup = true;
}

void ControlModel::endFrame()
{
    jassert (pimpl->isInFrame);

    if (! pimpl->isInFrame)
        return;

    pimpl->isInFrame = false;
    const auto previousStructure = getStructure (pimpl->publishedItems);
    const auto nextStructure = getStructure (pimpl->buildingItems);
    pimpl->publishedItems = std::move (pimpl->buildingItems);

    if (previousStructure != nextStructure)
        ++pimpl->structureRevision;
}

void ControlModel::clear()
{
    if (! pimpl->publishedItems.empty())
        ++pimpl->structureRevision;

    pimpl->publishedItems.clear();
    pimpl->buildingItems.clear();
    pimpl->bindings.clear();
    pimpl->queuedEdits.clear();
    pimpl->isInFrame = false;
    pimpl->hasSelectedGroup = false;
}

Span<const ControlItem> ControlModel::getItems() const noexcept { return pimpl->publishedItems; }

const ControlItem* ControlModel::findItem (const String& identifier) const noexcept
{
    const auto iterator = std::find_if (pimpl->publishedItems.begin(), pimpl->publishedItems.end(), [&identifier] (const auto& item)
    {
        return item.identifier == identifier;
    });

    return iterator != pimpl->publishedItems.end() ? &*iterator : nullptr;
}

uint64 ControlModel::getStructureRevision() const noexcept { return pimpl->structureRevision; }

void ControlModel::queueAction (const String& identifier)
{
    const auto* item = findItem (identifier);
    const auto isValid = item != nullptr
                        && item->enabled
                        && (item->kind == ControlItem::Kind::button || item->kind == ControlItem::Kind::radioButton);
    jassert (isValid);

    if (isValid)
        pimpl->queuedEdits.push_back ({ identifier, {}, QueuedEditKind::action });
}

void ControlModel::queueBooleanEdit (const String& identifier, bool value)
{
    const auto* item = findItem (identifier);
    const auto isValid = item != nullptr && item->enabled && item->kind == ControlItem::Kind::toggle;
    jassert (isValid);

    if (isValid)
        pimpl->queuedEdits.push_back ({ identifier, {}, QueuedEditKind::boolean, 0.0, value });
}

void ControlModel::queueNumberEdit (const String& identifier, double value)
{
    const auto* item = findItem (identifier);
    const auto isValid = item != nullptr
                        && item->enabled
                        && item->kind == ControlItem::Kind::number
                        && std::isfinite (value);
    jassert (isValid);

    if (isValid)
        pimpl->queuedEdits.push_back ({ identifier, {}, QueuedEditKind::number, std::clamp (value, item->minValue, item->maxValue) });
}

void ControlModel::queueChoiceEdit (const String& identifier, const String& choiceIdentifier)
{
    const auto* item = findItem (identifier);
    const auto isValid = item != nullptr
                        && item->enabled
                        && item->kind == ControlItem::Kind::choice
                        && containsChoice (*item, choiceIdentifier);
    jassert (isValid);

    if (isValid)
        pimpl->queuedEdits.push_back ({ identifier, choiceIdentifier, QueuedEditKind::choice });
}

void ControlModel::queueTextEdit (const String& identifier, const String& value)
{
    const auto* item = findItem (identifier);
    const auto isValid = item != nullptr && item->enabled && item->kind == ControlItem::Kind::textInput;
    jassert (isValid);

    if (isValid)
        pimpl->queuedEdits.push_back ({ identifier, value, QueuedEditKind::text });
}

void ControlModel::showButton (const String& identifier, const String& displayText, std::function<void()> action, bool shouldEnable)
{
    auto* item = pimpl->appendItem (identifier, ControlItem::Kind::button);
    auto* binding = pimpl->appendBinding (identifier, BindingKind::action);

    if (item == nullptr || binding == nullptr)
        return;

    item->labelText = displayText;
    item->enabled = shouldEnable;
    binding->action = std::move (action);
}

void ControlModel::showRadioButton (const String& identifier, const String& displayText, bool isSelected, std::function<void()> action, bool shouldEnable)
{
    auto* item = pimpl->appendItem (identifier, ControlItem::Kind::radioButton);
    auto* binding = pimpl->appendBinding (identifier, BindingKind::action);

    if (item == nullptr || binding == nullptr)
        return;

    item->labelText = displayText;
    item->booleanValue = isSelected;
    item->enabled = shouldEnable;
    binding->action = std::move (action);
}

void ControlModel::showToggle (const String& identifier, const String& displayText, bool& value, std::function<void()> editAction, bool shouldEnable)
{
    auto* item = pimpl->appendItem (identifier, ControlItem::Kind::toggle);
    auto* binding = pimpl->appendBinding (identifier, BindingKind::boolean);

    if (item == nullptr || binding == nullptr)
        return;

    item->labelText = displayText;
    item->booleanValue = value;
    item->enabled = shouldEnable;
    binding->boolTarget = &value;
    binding->editAction = std::move (editAction);
}

void ControlModel::showFloatSlider (const String& identifier, const String& displayText, float& value, float minValue, float maxValue, int numDecimalPlaces, std::function<void()> editAction, bool shouldEnable)
{
    auto* item = pimpl->appendItem (identifier, ControlItem::Kind::number);
    auto* binding = pimpl->appendBinding (identifier, BindingKind::floatingPoint);

    if (item == nullptr || binding == nullptr)
        return;

    item->labelText = displayText;
    item->value = value;
    item->minValue = minValue;
    item->maxValue = maxValue;
    item->numDecimalPlaces = numDecimalPlaces;
    item->enabled = shouldEnable;
    binding->floatTarget = &value;
    binding->editAction = std::move (editAction);
}

void ControlModel::showFloatPairSliders (const String& identifier, const String& displayText, b2Vec2& value, float minValue, float maxValue, int numDecimalPlaces, std::function<void()> editAction, bool shouldEnable)
{
    const auto xText = TRANS ("{controlName}, X").replace ("{controlName}", displayText);
    const auto yText = TRANS ("{controlName}, Y").replace ("{controlName}", displayText);
    showFloatSlider (identifier + ".x", xText, value.x, minValue, maxValue, numDecimalPlaces, editAction, shouldEnable);
    showFloatSlider (identifier + ".y", yText, value.y, minValue, maxValue, numDecimalPlaces, std::move (editAction), shouldEnable);
}

void ControlModel::showIntegerSlider (const String& identifier, const String& displayText, int& value, int minValue, int maxValue, std::function<void()> editAction, bool shouldEnable)
{
    auto* item = pimpl->appendItem (identifier, ControlItem::Kind::number);
    auto* binding = pimpl->appendBinding (identifier, BindingKind::integer);

    if (item == nullptr || binding == nullptr)
        return;

    item->labelText = displayText;
    item->value = value;
    item->minValue = minValue;
    item->maxValue = maxValue;
    item->interval = 1.0;
    item->enabled = shouldEnable;
    binding->intTarget = &value;
    binding->editAction = std::move (editAction);
}

void ControlModel::showChoice (const String& identifier, const String& displayText, int& selectedIndex, const StringArray& itemNames, std::function<void()> editAction, bool shouldEnable)
{
    auto* item = pimpl->appendItem (identifier, ControlItem::Kind::choice);
    auto* binding = pimpl->appendBinding (identifier, BindingKind::indexedChoice);

    if (item == nullptr || binding == nullptr)
        return;

    item->labelText = displayText;
    item->enabled = shouldEnable;
    item->choices.reserve ((size_t) itemNames.size());
    binding->choiceIdentifiers.reserve ((size_t) itemNames.size());

    for (int itemIndex = 0; itemIndex < itemNames.size(); ++itemIndex)
    {
        const String choiceIdentifier (itemIndex);
        item->choices.push_back ({ choiceIdentifier, itemNames[itemIndex] });
        binding->choiceIdentifiers.push_back (choiceIdentifier);
    }

    if (isPositiveAndBelow (selectedIndex, itemNames.size()))
        item->selectedChoiceIdentifier = String (selectedIndex);

    binding->intTarget = &selectedIndex;
    binding->editAction = std::move (editAction);
}

void ControlModel::showChoice (const String& identifier, const String& displayText, String& selectedChoiceIdentifier, Span<const ControlChoice> choices, std::function<void()> editAction, bool shouldEnable)
{
    const auto choicesAreValid = areChoicesValid (choices);
    jassert (choicesAreValid);

    if (! choicesAreValid)
        return;

    auto* item = pimpl->appendItem (identifier, ControlItem::Kind::choice);
    auto* binding = pimpl->appendBinding (identifier, BindingKind::identifiedChoice);

    if (item == nullptr || binding == nullptr)
        return;

    item->labelText = displayText;
    item->enabled = shouldEnable;
    item->choices.assign (choices.begin(), choices.end());

    if (containsChoice (*item, selectedChoiceIdentifier))
        item->selectedChoiceIdentifier = selectedChoiceIdentifier;

    binding->stringTarget = &selectedChoiceIdentifier;
    binding->editAction = std::move (editAction);
}

void ControlModel::showTextInput (const String& identifier, const String& displayText, String& value, std::function<void()> editAction, bool shouldEnable)
{
    auto* item = pimpl->appendItem (identifier, ControlItem::Kind::textInput);
    auto* binding = pimpl->appendBinding (identifier, BindingKind::text);

    if (item == nullptr || binding == nullptr)
        return;

    item->labelText = displayText;
    item->stringValue = value;
    item->enabled = shouldEnable;
    binding->stringTarget = &value;
    binding->editAction = std::move (editAction);
}

void ControlModel::showText (const String& identifier, const String& text, ControlItem::TextTone tone)
{
    if (auto* item = pimpl->appendItem (identifier, ControlItem::Kind::text))
    {
        item->labelText = text;
        item->textTone = tone;
        item->enabled = false;
    }
}

void ControlModel::showSeparator (const String& identifier)
{
    if (auto* item = pimpl->appendItem (identifier, ControlItem::Kind::separator))
        item->enabled = false;
}

void ControlModel::showProgress (const String& identifier, const String& displayText, double fraction)
{
    if (auto* item = pimpl->appendItem (identifier, ControlItem::Kind::progress))
    {
        item->labelText = displayText;
        item->value = std::clamp (fraction, 0.0, 1.0);
        item->enabled = false;
    }
}

void ControlModel::showInteraction (const String& identifier, const String& inputDescription, const String& actionDescription)
{
    if (auto* item = pimpl->appendItem (identifier, ControlItem::Kind::interaction))
    {
        item->labelText = inputDescription;
        item->helpText = actionDescription;
        item->enabled = false;
    }
}

void ControlModel::setHelpText (const String& identifier, const String& helpText)
{
    jassert (pimpl->isInFrame);
    const auto iterator = std::find_if (pimpl->buildingItems.begin(), pimpl->buildingItems.end(), [&identifier] (const auto& item)
    {
        return item.identifier == identifier;
    });
    jassert (iterator != pimpl->buildingItems.end());

    if (iterator != pimpl->buildingItems.end())
        iterator->helpText = helpText;
}

} // namespace Box2DSamples
