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

enum
{
    standardControlRowHeight = 32,
    interactionControlRowHeight = 56,
    separatorRowHeight = 8
};

class ModelProperty
{
public:
    ModelProperty (ControlModel& newControlModel, String newIdentifier) :
        controlModel (newControlModel),
        identifier (std::move (newIdentifier))
    {
    }

protected:
    [[nodiscard]] const ControlItem* getItem() const noexcept { return controlModel.findItem (identifier); }

    ControlModel& controlModel;
    const String identifier;
};

class ActionProperty final : public PropertyComponent,
                             private ModelProperty
{
public:
    ActionProperty (ControlModel& presentedControlModel, const ControlItem& item) :
        PropertyComponent (item.labelText, standardControlRowHeight),
        ModelProperty (presentedControlModel, item.identifier)
    {
        addAndMakeVisible (button);
        button.onClick = [this] { this->controlModel.queueAction (identifier); };
        refresh();
    }

    void refresh() override
    {
        if (const auto* item = getItem())
        {
            setTitle (item->labelText);
            setDescription (item->helpText);
            button.setButtonText (item->labelText);
            button.setTitle (item->labelText);
            button.setDescription (item->helpText);
            button.setEnabled (item->enabled);
        }
    }

    void resized() override { button.setBounds (getLocalBounds().reduced (4, 2)); }

private:
    TextButton button;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ActionProperty)
};

class BooleanProperty final : public BooleanPropertyComponent,
                              private ModelProperty
{
public:
    BooleanProperty (ControlModel& presentedControlModel, const ControlItem& item) :
        BooleanPropertyComponent (item.labelText, {}, {}),
        ModelProperty (presentedControlModel, item.identifier)
    {
        setPreferredHeight (standardControlRowHeight);
        refresh();
    }

    void setState (bool newState) override
    {
        if (const auto* item = getItem())
        {
            if (item->kind == ControlItem::Kind::radioButton)
                controlModel.queueAction (identifier);
            else
                controlModel.queueBooleanEdit (identifier, newState);
        }
    }

    bool getState() const override
    {
        if (const auto* item = getItem())
            return item->booleanValue;

        return false;
    }

    void refresh() override
    {
        if (const auto* item = getItem())
        {
            setName (item->labelText);
            setTitle (item->labelText);
            setDescription (item->helpText);
            setEnabled (item->enabled);
        }

        BooleanPropertyComponent::refresh();

        if (const auto* item = getItem(); item != nullptr)
        {
            if (auto* buttonComponent = getChildComponent (0))
            {
                buttonComponent->setTitle (item->labelText);
                buttonComponent->setDescription (item->helpText);
            }
        }
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BooleanProperty)
};

class NumberProperty final : public SliderPropertyComponent,
                             private ModelProperty
{
public:
    NumberProperty (ControlModel& presentedControlModel, const ControlItem& item) :
        SliderPropertyComponent (item.labelText, item.minValue, item.maxValue, item.interval),
        ModelProperty (presentedControlModel, item.identifier)
    {
        setPreferredHeight (standardControlRowHeight);
        refresh();
    }

    void setValue (double newValue) override { controlModel.queueNumberEdit (identifier, newValue); }

    double getValue() const override
    {
        if (const auto* item = getItem())
            return item->value;

        return 0.0;
    }

    void refresh() override
    {
        if (const auto* item = getItem())
        {
            setName (item->labelText);
            setTitle (item->labelText);
            setDescription (item->helpText);
            setEnabled (item->enabled);
            slider.setRange (item->minValue, item->maxValue, item->interval);
            slider.setNumDecimalPlacesToDisplay (item->numDecimalPlaces);
            slider.setTitle (item->labelText);
            slider.setDescription (item->helpText);
        }

        SliderPropertyComponent::refresh();
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NumberProperty)
};

class ChoiceProperty final : public ChoicePropertyComponent,
                             private ModelProperty
{
public:
    ChoiceProperty (ControlModel& presentedControlModel, const ControlItem& item) :
        ChoicePropertyComponent (item.labelText),
        ModelProperty (presentedControlModel, item.identifier)
    {
        setPreferredHeight (standardControlRowHeight);
        refresh();
    }

    void setIndex (int newIndex) override
    {
        if (const auto* item = getItem(); item != nullptr && isPositiveAndBelow (newIndex, (int) item->choices.size()))
        {
            controlModel.queueChoiceEdit (identifier, item->choices[(size_t) newIndex].identifier);
            queuedIndex = newIndex;
        }
    }

    int getIndex() const override
    {
        const auto* item = getItem();

        if (item == nullptr || ! item->selectedChoiceIdentifier.has_value())
            return -1;

        const auto iterator = std::find_if (item->choices.begin(), item->choices.end(), [&item] (const auto& choice)
        {
            return choice.identifier == *item->selectedChoiceIdentifier;
        });
        return iterator != item->choices.end() ? (int) std::distance (item->choices.begin(), iterator) : -1;
    }

    void refresh() override
    {
        if (choiceControl != nullptr && hasChoiceSnapshot)
        {
            const int modelIndex = getIndex();
            const int controlIndex = choiceControl->getSelectedItemIndex();
            const bool modelChanged = modelIndex != previousModelIndex;
            const bool controlChanged = controlIndex != previousControlIndex;

            if (queuedIndex.has_value() && modelIndex == *queuedIndex)
                queuedIndex.reset();
            else if (! queuedIndex.has_value() && controlChanged && ! modelChanged)
                setIndex (controlIndex);
        }

        const auto* item = getItem();

        if (item == nullptr)
            return;

        setName (item->labelText);
        setTitle (item->labelText);
        setDescription (item->helpText);
        setEnabled (item->enabled);
        choices.clear();

        for (const auto& choice : item->choices)
            choices.add (choice.labelText);

        ChoicePropertyComponent::refresh();

        if (choiceControl == nullptr)
        {
            for (auto* child : getChildren())
            {
                if (auto* childChoiceControl = dynamic_cast<ComboBox*> (child))
                {
                    choiceControl = childChoiceControl;
                    break;
                }
            }
        }

        if (choiceControl != nullptr)
        {
            choiceControl->clear (dontSendNotification);
            choiceControl->addItemList (choices, 1);
            choiceControl->setSelectedItemIndex (getIndex(), dontSendNotification);
            choiceControl->setTitle (item->labelText);
            choiceControl->setDescription (item->helpText);
            captureChoiceSnapshot();
        }
    }

private:
    ComboBox* choiceControl = nullptr;
    int previousModelIndex = -1,
        previousControlIndex = -1;
    bool hasChoiceSnapshot = false;
    std::optional<int> queuedIndex;

    void captureChoiceSnapshot()
    {
        if (choiceControl == nullptr)
            return;

        previousModelIndex = getIndex();
        previousControlIndex = choiceControl->getSelectedItemIndex();
        hasChoiceSnapshot = true;
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChoiceProperty)
};

class TextInputProperty final : public TextPropertyComponent,
                                private ModelProperty
{
public:
    TextInputProperty (ControlModel& presentedControlModel, const ControlItem& item) :
        TextPropertyComponent (item.labelText, 0, false),
        ModelProperty (presentedControlModel, item.identifier)
    {
        setPreferredHeight (standardControlRowHeight);
        refresh();
    }

    void setText (const String& newText) override { controlModel.queueTextEdit (identifier, newText); }

    String getText() const override
    {
        if (const auto* item = getItem())
            return item->stringValue;

        return {};
    }

    void refresh() override
    {
        if (const auto* item = getItem())
        {
            setName (item->labelText);
            setTitle (item->labelText);
            setDescription (item->helpText);
            setEnabled (item->enabled);
            setEditable (item->enabled);
        }

        TextPropertyComponent::refresh();

        if (const auto* item = getItem(); item != nullptr)
        {
            if (auto* editor = getChildComponent (0))
            {
                editor->setTitle (item->labelText);
                editor->setDescription (item->helpText);
            }
        }
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TextInputProperty)
};

class ReadOnlyProperty final : public PropertyComponent,
                               private ModelProperty
{
public:
    ReadOnlyProperty (ControlModel& presentedControlModel, const ControlItem& item) :
        PropertyComponent (item.labelText, standardControlRowHeight),
        ModelProperty (presentedControlModel, item.identifier)
    {
        label.setJustificationType (Justification::centredLeft);
        label.setMinimumHorizontalScale (0.65f);
        label.setInterceptsMouseClicks (false, false);
        addAndMakeVisible (label);
        refresh();
    }

    void refresh() override
    {
        if (const auto* item = getItem())
        {
            setTitle (item->labelText);
            setDescription (item->helpText);
            label.setTitle (item->labelText);
            label.setDescription (item->helpText);
            label.setText (item->labelText, dontSendNotification);
            const auto standardColour = findColour (PropertyComponent::labelTextColourId);
            const auto standardFont = getLookAndFeel().getLabelFont (label);
            auto textColour = standardColour;
            if (item->textTone == ControlItem::TextTone::secondary) { textColour = Colours::grey; }
            else if (item->textTone == ControlItem::TextTone::warning) { textColour = Colours::orange; }
            label.setColour (Label::textColourId, textColour);
            label.setFont (item->textTone == ControlItem::TextTone::subheading ? standardFont.boldened() : standardFont);
        }
    }

    void resized() override { label.setBounds (getLocalBounds().reduced (6, 2)); }

private:
    Label label;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ReadOnlyProperty)
};

class SeparatorProperty final : public PropertyComponent
{
public:
    SeparatorProperty() : PropertyComponent ("separator", separatorRowHeight) { setAccessible (false); }

    void refresh() override {}

    void paint (Graphics& graphics) override
    {
        graphics.setColour (findColour (PropertyComponent::labelTextColourId).withAlpha (0.25f));
        graphics.fillRect (getLocalBounds().reduced (6, 3));
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SeparatorProperty)
};

class ProgressProperty final : public PropertyComponent,
                               private ModelProperty
{
public:
    ProgressProperty (ControlModel& presentedControlModel, const ControlItem& item) :
        PropertyComponent (item.labelText, standardControlRowHeight),
        ModelProperty (presentedControlModel, item.identifier),
        progressBar (progressValue, ProgressBar::Style::linear)
    {
        progressBar.setPercentageDisplay (true);
        addAndMakeVisible (progressBar);
        refresh();
    }

    void refresh() override
    {
        if (const auto* item = getItem())
        {
            progressValue = item->value;
            setName (item->labelText);
            setTitle (item->labelText);
            setDescription (item->helpText);
            progressBar.setTitle (item->labelText);
            progressBar.setDescription (item->helpText);
            progressBar.repaint();
        }
    }

private:
    double progressValue = 0.0;
    ProgressBar progressBar;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ProgressProperty)
};

class WrappedText final : public Component
{
public:
    WrappedText() = default;

    void setText (String newText)
    {
        if (text != newText)
        {
            text = std::move (newText);
            repaint();
        }
    }

    void paint (Graphics& graphics) override
    {
        graphics.setColour (findColour (PropertyComponent::labelTextColourId));
        graphics.drawFittedText (text, getLocalBounds(), Justification::centredLeft, 3, 0.8f);
    }

private:
    String text;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WrappedText)
};

class InteractionProperty final : public PropertyComponent,
                                  private ModelProperty
{
public:
    InteractionProperty (ControlModel& presentedControlModel, const ControlItem& item) :
        PropertyComponent (item.labelText, interactionControlRowHeight),
        ModelProperty (presentedControlModel, item.identifier)
    {
        actionText.setAccessible (false);
        actionText.setInterceptsMouseClicks (false, false);
        addAndMakeVisible (actionText);
        setWantsKeyboardFocus (false);
        refresh();
    }

    void refresh() override
    {
        if (const auto* item = getItem())
        {
            setName (item->labelText);
            setTitle (item->labelText);
            setDescription (item->helpText);
            actionText.setText (item->helpText);
        }
    }

    void resized() override { actionText.setBounds (getLookAndFeel().getPropertyComponentContentPosition (*this).reduced (4, 2)); }

    std::unique_ptr<AccessibilityHandler> createAccessibilityHandler() override
    {
        return std::make_unique<AccessibilityHandler> (*this, AccessibilityRole::staticText);
    }

private:
    WrappedText actionText;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InteractionProperty)
};

PropertyComponent* createProperty (ControlModel& controlModel, const ControlItem& item)
{
    switch (item.kind)
    {
        case ControlItem::Kind::button:       return new ActionProperty (controlModel, item);
        case ControlItem::Kind::radioButton:  return new BooleanProperty (controlModel, item);
        case ControlItem::Kind::toggle:       return new BooleanProperty (controlModel, item);
        case ControlItem::Kind::number:       return new NumberProperty (controlModel, item);
        case ControlItem::Kind::choice:       return new ChoiceProperty (controlModel, item);
        case ControlItem::Kind::textInput:    return new TextInputProperty (controlModel, item);
        case ControlItem::Kind::text:         return new ReadOnlyProperty (controlModel, item);
        case ControlItem::Kind::separator:    return new SeparatorProperty();
        case ControlItem::Kind::progress:     return new ProgressProperty (controlModel, item);
        case ControlItem::Kind::interaction:  return new InteractionProperty (controlModel, item);
    }

    jassertfalse;
    return nullptr;
}

struct Section final
{
    String title;
    Array<PropertyComponent*> properties;
    bool initiallyOpen = false;
};

int getSectionIndex (ControlItem::Group group)
{
    switch (group)
    {
        case ControlItem::Group::catalogue:        return 0;
        case ControlItem::Group::primaryActions:
        case ControlItem::Group::interactionHelp:
        case ControlItem::Group::sampleControls:   return 1;
        case ControlItem::Group::simulation:       return 2;
        case ControlItem::Group::solver:           return 3;
        case ControlItem::Group::drawing:          return 4;
        case ControlItem::Group::recording:        return 5;
        case ControlItem::Group::viewCalibration:  return 6;
    }

    jassertfalse;
    return 1;
}

} // namespace

void appendControlProperties (ControlModel& controlModel, PropertyPanel& propertyPanel)
{
    std::array<Section, 7> sections =
    {
        Section { TRANS ("Catalogue"), {}, true },
        Section { TRANS ("Controls"), {}, true },
        Section { TRANS ("Simulation"), {}, false },
        Section { TRANS ("Solver"), {}, false },
        Section { TRANS ("Drawing"), {}, false },
        Section { TRANS ("Recording"), {}, false },
        Section { TRANS ("View calibration"), {}, false }
    };

    for (const auto& item : controlModel.getItems())
    {
        if (auto* property = createProperty (controlModel, item))
            sections[(size_t) getSectionIndex (item.group)].properties.add (property);
    }

    for (auto& section : sections)
    {
        if (! section.properties.isEmpty())
            propertyPanel.addSection (section.title, section.properties, section.initiallyOpen);
    }
}

} // namespace Box2DSamples
