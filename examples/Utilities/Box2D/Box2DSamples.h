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

#include <JuceHeader.h>

namespace Box2DSamples
{

class UpstreamSampleAdapter;
class ReplaySample;

enum class MouseButton
{
    primary,
    secondary,
    middle
};

/** Describes one ordinary sample's authored home framing. */
struct HomeView final
{
    /** Chooses the bounds for a drawable aspect ratio.

        @param aspectRatio Drawable width divided by drawable height.

        @returns the portrait override for a portrait ratio when present; otherwise, the default bounds.
    */
    [[nodiscard]] b2AABB getBoundsForAspectRatio (float aspectRatio) const noexcept;

    //==============================================================================
    /** Static upstream category key. */
    const char* category = nullptr;

    /** Static upstream sample-name key. */
    const char* sampleName = nullptr;

    /** Aspect-independent default home bounds. */
    b2AABB defaultBounds = {};

    /** Optional bounds used when the drawable area is portrait. */
    std::optional<b2AABB> portraitBounds;
};

namespace HomeViewCatalog
{
    /** @returns every authored ordinary-sample home view. */
    [[nodiscard]] Span<const HomeView> getEntries() noexcept;

    /** Finds the home view for one stable upstream identity.

        @param category     Static upstream category key.
        @param sampleName   Static upstream sample-name key.

        @returns the matching static definition, or `nullptr` when none exists.
    */
    [[nodiscard]] const HomeView* find (const char* category, const char* sampleName) noexcept;
}

/** Describes one input and the action it performs. */
struct InteractionHint final
{
    /** Stable untranslated merge and row key. */
    Identifier identifier;

    /** Static translated-at-display-time input and action descriptions. */
    String inputDescription,
           actionDescription;
};

/** Associates interaction hints with one stable sample identity. */
struct SampleInteractionHelp final
{
    /** Static unrestricted upstream category and sample-name keys. */
    String category,
           sampleName;

    /** Ordered interaction hints shown after the global controls. */
    Span<const InteractionHint> hints;
};

namespace SampleInteractionCatalog
{
    /** @returns the generic interactions shared by ordinary live samples. */
    [[nodiscard]] Span<const InteractionHint> getGenericLiveHints() noexcept;

    /** @returns every authored sample interaction definition. */
    [[nodiscard]] Span<const SampleInteractionHelp> getEntries() noexcept;

    /** Finds interaction help for one stable upstream identity without allocating temporary strings.

        @param category    Upstream category key.
        @param sampleName  Upstream sample-name key.

        @returns the matching static definition, or `nullptr` when the sample has no additional interactions.
    */
    [[nodiscard]] const SampleInteractionHelp* find (StringRef category, StringRef sampleName) noexcept;
}

/** Stores and transforms the sample viewport. */
struct Camera final
{
    /** Restores the default centre and zoom. */
    void reset() noexcept;

    /** Updates the drawable dimensions used by viewport transforms. */
    void setDrawableSize (float newWidth, float newHeight) noexcept;

    /** Sets the viewport to frame the supplied world bounds. */
    void setViewToBounds (b2AABB bounds) noexcept;

    /** @returns the world position corresponding to a component position. */
    [[nodiscard]] b2Pos convertComponentToWorld (juce::Point<float> componentPosition, const juce::Rectangle<float>& targetArea) const noexcept;

    /** @returns the component position corresponding to a world position. */
    [[nodiscard]] juce::Point<float> convertWorldToComponent (b2Pos worldPosition, const juce::Rectangle<float>& targetArea) const noexcept;

    /** @returns the drawable aspect ratio, or the upstream baseline before layout. */
    [[nodiscard]] float getAspectRatio() const noexcept;

    /** @returns the visible world bounds. */
    [[nodiscard]] b2AABB getVisibleBounds() const noexcept;

    /** @returns the visible world dimensions. */
    [[nodiscard]] b2Vec2 getViewSize() const noexcept;

    //==============================================================================
    /** World-space centre of the viewport. */
    b2Pos centre = { 0.0f, 20.0f };

    /** Visible vertical half-extent and drawable dimensions, in that order. */
    float zoom = 1.0f,
          drawableWidth = 0.0f,
          drawableHeight = 0.0f;
};

/** Buffers sample-specific drawing commands for native JUCE painting. */
class DrawList final
{
public:
    /** Constructs an empty drawing command list. */
    DrawList();

    /** Destroys the drawing command list. */
    ~DrawList();

    //==============================================================================
    /** Removes commands from the previous frame. */
    void clear();

    /** Adds one world-space point. */
    void addPoint (b2Pos position, float diameter, b2HexColor colour);

    /** Adds one world-space line. */
    void addLine (b2Pos start, b2Pos end, b2HexColor colour);

    /** Adds one world-space circle outline. */
    void addCircle (b2Pos centre, float radius, b2HexColor colour);

    /** Adds one world-space capsule outline. */
    void addCapsule (b2Pos start, b2Pos end, float radius, b2HexColor colour);

    /** Adds one transformed polygon outline. */
    void addPolygon (b2WorldTransform transform, const b2Vec2* vertices, int numVertices, b2HexColor colour);

    /** Adds one transformed solid circle. */
    void addSolidCircle (b2WorldTransform transform, b2Vec2 centre, float radius, b2HexColor colour);

    /** Adds one transformed solid polygon. */
    void addSolidPolygon (b2WorldTransform transform, const b2Vec2* vertices, int numVertices, float radius, b2HexColor colour);

    /** Adds one world transform marker. */
    void addTransform (b2WorldTransform transform, float scale);

    /** Adds one world-space bounds outline. */
    void addBounds (b2AABB bounds, b2HexColor colour);

    /** Adds one world-space text label. */
    void addWorldText (b2Pos position, b2HexColor colour, const String& text);

    /** Adds one component-space text label. */
    void addScreenText (juce::Point<float> position, b2HexColor colour, const String& text);

    /** Paints all buffered commands into the target area. */
    void render (Graphics& graphics, const Camera& camera, const juce::Rectangle<float>& targetArea) const;

    /** @returns debug-draw callbacks that append to this command list. */
    [[nodiscard]] b2DebugDraw& getDebugDraw() noexcept;

private:
    //==============================================================================
    class Pimpl;

    //==============================================================================
    std::unique_ptr<Pimpl> pimpl;

    //==============================================================================
    void append (const DrawList& other);

    friend class UpstreamSampleAdapter;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DrawList)
};

/** Describes one stable choice without prescribing its presentation. */
struct ControlChoice final
{
    /** Stable untranslated identifier and translated visible label. */
    String identifier,
           labelText;
};

/** Describes one Box2D demo control without prescribing its presentation. */
struct ControlItem final
{
    /** Identifies a semantic control group. */
    enum class Group
    {
        catalogue,
        primaryActions,
        interactionHelp,
        sampleControls,
        simulation,
        solver,
        drawing,
        recording,
        viewCalibration
    };

    /** Identifies a control's interaction shape. */
    enum class Kind
    {
        button,
        radioButton,
        toggle,
        number,
        choice,
        textInput,
        text,
        separator,
        progress,
        interaction
    };

    /** Identifies a non-interactive text row's emphasis. */
    enum class TextTone
    {
        standard,
        secondary,
        warning,
        subheading
    };

    //==============================================================================
    /** Stable identifier retained across descriptions. */
    String identifier;

    /** Semantic group selected by the producer. */
    Group group = Group::sampleControls;

    /** Interaction shape presented by a view. */
    Kind kind = Kind::text;

    /** Emphasis applied when `kind` is `text`. */
    TextTone textTone = TextTone::standard;

    /** Translated visible label, current editable text, and reusable help description. */
    String labelText,
           stringValue,
           helpText;

    /** Stable choices presented when `kind` is `choice`. */
    std::vector<ControlChoice> choices;

    /** Current scalar value for a number or progress item. */
    double value = 0.0,
           minValue = 0.0,
           maxValue = 1.0,
           interval = 0.0;

    /** Selected stable choice, or an empty value when selection is invalid. */
    std::optional<String> selectedChoiceIdentifier;

    /** Decimal precision requested for a number item. */
    int numDecimalPlaces = 0;

    /** Whether the control accepts edits. */
    bool enabled = true,
         booleanValue = false;
};

/** Retains one ordered description of Box2D demo controls. */
class ControlModel final
{
public:
    /** Creates an empty control model. */
    ControlModel();

    /** Releases retained controls and callbacks. */
    ~ControlModel();

    //==============================================================================
    /** Applies edits queued through stable control identifiers. */
    void applyQueuedEdits();

    /** Starts one control-description pass and invalidates outstanding item views. */
    void beginFrame();

    /** Selects the semantic group assigned to subsequent controls. */
    void setGroup (ControlItem::Group group);

    /** Finishes one control-description pass and publishes its items. */
    void endFrame();

    /** Removes controls, callbacks, and queued edits before their owners are destroyed. */
    void clear();

    /** @returns a view of the published ordered items, valid until the next `beginFrame()` or `clear()`. */
    [[nodiscard]] Span<const ControlItem> getItems() const noexcept;

    /** Finds one published item. The pointer remains valid only while the current item view remains valid. */
    [[nodiscard]] const ControlItem* findItem (const String& identifier) const noexcept;

    /** @returns the revision of the published identifiers, groups, and kinds. */
    [[nodiscard]] uint64 getStructureRevision() const noexcept;

    //==============================================================================
    /** Queues one action for the next edit application pass. */
    void queueAction (const String& identifier);

    /** Queues one Boolean edit for the next edit application pass. */
    void queueBooleanEdit (const String& identifier, bool value);

    /** Queues one number edit for the next edit application pass. */
    void queueNumberEdit (const String& identifier, double value);

    /** Queues one choice edit for the next edit application pass. */
    void queueChoiceEdit (const String& identifier, const String& choiceIdentifier);

    /** Queues one text edit for the next edit application pass. */
    void queueTextEdit (const String& identifier, const String& value);

    //==============================================================================
    /** Describes a translated button control. */
    void showButton (const String& identifier, const String& displayText, std::function<void()> action, bool shouldEnable = true);

    /** Describes a translated radio-button control whose activation queues an action. */
    void showRadioButton (const String& identifier, const String& displayText, bool isSelected, std::function<void()> action, bool shouldEnable = true);

    /** Describes a translated Boolean control. */
    void showToggle (const String& identifier, const String& displayText, bool& value, std::function<void()> editAction = {}, bool shouldEnable = true);

    /** Describes a translated numeric control. */
    void showFloatSlider (const String& identifier, const String& displayText, float& value, float minValue, float maxValue, int numDecimalPlaces, std::function<void()> editAction = {}, bool shouldEnable = true);

    /** Describes translated X and Y controls using stable `.x` and `.y` identifier suffixes. */
    void showFloatPairSliders (const String& identifier, const String& displayText, b2Vec2& value, float minValue, float maxValue, int numDecimalPlaces, std::function<void()> editAction = {}, bool shouldEnable = true);

    /** Describes a translated integer control. */
    void showIntegerSlider (const String& identifier, const String& displayText, int& value, int minValue, int maxValue, std::function<void()> editAction = {}, bool shouldEnable = true);

    /** Describes a translated choice control. */
    void showChoice (const String& identifier, const String& displayText, int& selectedIndex, const StringArray& itemNames, std::function<void()> editAction = {}, bool shouldEnable = true);

    /** Describes a translated choice control with stable producer-owned choices. */
    void showChoice (const String& identifier, const String& displayText, String& selectedChoiceIdentifier, Span<const ControlChoice> choices, std::function<void()> editAction = {}, bool shouldEnable = true);

    /** Describes a translated text-input control. */
    void showTextInput (const String& identifier, const String& displayText, String& value, std::function<void()> editAction = {}, bool shouldEnable = true);

    /** Describes translated read-only text. */
    void showText (const String& identifier, const String& text, ControlItem::TextTone tone = ControlItem::TextTone::standard);

    /** Describes a visual separator. */
    void showSeparator (const String& identifier);

    /** Describes a translated progress control. */
    void showProgress (const String& identifier, const String& displayText, double fraction);

    /** Describes one translated interaction, mapping its input to the label and its action to help text. */
    void showInteraction (const String& identifier, const String& inputDescription, const String& actionDescription);

    /** Assigns translated help text to an item already described in the current pass. */
    void setHelpText (const String& identifier, const String& helpText);

private:
    //==============================================================================
    struct Pimpl;

    //==============================================================================
    std::unique_ptr<Pimpl> pimpl;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ControlModel)
};

/** Appends the desktop PropertyPanel presentation of one published control model.

    The model must outlive the appended properties, or the panel must be cleared before the model is destroyed.
    The caller clears and appends again when `getStructureRevision()` changes, and calls `PropertyPanel::refreshAll()` when only published values or presentation metadata change.

    @param controlModel   Model supplying item state and accepting queued edits.
    @param propertyPanel  Panel that takes ownership of the generated desktop properties.
*/
void appendControlProperties (ControlModel& controlModel, PropertyPanel& propertyPanel);

/** Holds one sample's simulation settings. */
struct SimulationSettings final
{
    /** Timing and contact distances, in Hertz, metres, and simulation units. */
    float hertz = 60.0f,
          recycleDistance = B2_CONTACT_RECYCLE_DISTANCE;

    /** Sub-step, restitution, worker, and queued single-step quantities. */
    int numSubSteps = 4,
        numRestitutionIterations = 2,
        numWorkers = 1,
        numSingleSteps = 0;

    /** Positive simulation feature flags. */
    bool isPaused = false,
         isWarmStartingEnabled = true,
         isContinuousCollisionEnabled = true,
         isRestitutionPropagationEnabled = false,
         isSleepingEnabled = true;
};

/** Supplies host state to one adapted upstream sample. */
class Context final
{
public:
    /** Constructs a context that appends custom drawing to `newDrawList`. */
    explicit Context (DrawList& newDrawList);

    //==============================================================================
    /** @returns whether the supplied JUCE key code is currently held. */
    [[nodiscard]] bool isKeyDown (int keyCode) const;

    /** Installs the held-key query used by interactive and headless hosts. */
    void setKeyStateQuery (std::function<bool (int)> newKeyStateQuery);

    //==============================================================================
    /** Camera, simulation settings, and world capacity used for the active sample. */
    Camera camera;
    SimulationSettings settings;
    b2Capacity capacity = {};

    /** Static authored home view for the active ordinary sample. */
    const HomeView* homeView = nullptr;

    /** Camera values derived from the active home bounds for the current drawable aspect. */
    b2Pos homeCameraCentre = { 0.0f, 20.0f };
    float homeCameraZoom = 1.0f;

    /** Debug-draw configuration and command destination. */
    b2DebugDraw debugDraw = b2DefaultDebugDraw();
    DrawList& drawList;

    /** Replay bytes and display name supplied before selecting the replay viewer. */
    MemoryBlock replayData;
    String replayName;

    /** Positive lifecycle and bounded-test flags. */
    bool shouldRestart = false,
         shouldUseReducedWorkload = false;

private:
    //==============================================================================
    std::function<bool (int)> keyStateQuery;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Context)
};

/** Base class shared by every adapted upstream Box2D sample. */
class Sample
{
public:
    /** Constructs a sample and optionally creates its world. */
    explicit Sample (Context& newContext, bool shouldCreateWorld = true);

    /** Destroys the sample and its owned world or replay player. */
    virtual ~Sample();

    //==============================================================================
    /** @returns the presentation-time interval represented by one fixed simulation advance. */
    [[nodiscard]] virtual double getStepIntervalSeconds() const noexcept;

    /** Advances simulation or replay state by one fixed step without preparing presentation data. */
    virtual void advanceSimulation();

    /** Rebuilds query results and sample-specific drawing commands for one presented frame. */
    virtual void prepareFrame();

    /** Describes or refreshes the sample's controls during an open pass. */
    virtual void updateControls (ControlModel& controls);

    /** Handles one focused key press. */
    virtual bool handleKeyPress (const KeyPress& key);

    /** Handles a pointer press in world coordinates. */
    virtual void handleMouseDown (b2Pos position, MouseButton button, const ModifierKeys& modifiers);

    /** Handles a pointer release in world coordinates. */
    virtual void handleMouseUp (b2Pos position, MouseButton button);

    /** Handles pointer movement in world coordinates. */
    virtual void handleMouseMove (b2Pos position);

    /** Restores the sample's home camera. */
    virtual void resetCamera();

    /** @returns whether ordinary solver controls affect this sample. */
    [[nodiscard]] virtual bool hasSolverControls() const noexcept;

    /** @returns whether this sample supplies step profiles. */
    [[nodiscard]] virtual bool hasProfile() const noexcept;

    /** Creates an optional inspector whose lifetime must end before this sample. */
    [[nodiscard]] virtual std::unique_ptr<Component> createInspectorComponent();

    /** Creates an optional metrics view whose lifetime must end before this sample. */
    [[nodiscard]] virtual std::unique_ptr<Component> createMetricsComponent();

    /** @returns the current world, which may be null before replay data is loaded. */
    [[nodiscard]] b2WorldId getWorldId() const noexcept;

    /** @returns chronological copies of the retained step profiles. */
    [[nodiscard]] std::vector<b2Profile> getProfileHistory() const;

    /** @returns the number of simulated or replayed steps. */
    [[nodiscard]] int getNumSteps() const noexcept;

    /** Begins recording the current world at a step boundary. */
    void startRecording();

    /** Stops recording and returns its bytes when recording was active. */
    [[nodiscard]] std::optional<MemoryBlock> stopRecording();

protected:
    //==============================================================================
    /** Recreates the owned world from current settings and capacity. */
    void createWorld();

    /** Appends one translated diagnostic line to the draw list. */
    void addScreenTextLine (const String& text);

    //==============================================================================
    Context& context;
    b2WorldId worldId = b2_nullWorldId;

private:
    //==============================================================================
    class Pimpl;

    //==============================================================================
    std::unique_ptr<Pimpl> pimpl;
    bool ownsWorld = true;

    //==============================================================================
    void recordSimulationStep();

    friend class UpstreamSampleAdapter;
    friend class ReplaySample;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Sample)
};

/** Creates one owned sample. */
using SampleCreateFunction = std::unique_ptr<Sample> (*) (Context&);

/** Supplies an optional world-capacity override. */
using SampleCapacityFunction = b2Capacity (*) ();

/** Describes one entry imported from the upstream sample registry. */
struct Entry final
{
    /** Static translated-at-display-time category and sample names. */
    const char* category = nullptr;
    const char* name = nullptr;

    /** Factory and optional capacity function. */
    SampleCreateFunction createSample = nullptr;
    SampleCapacityFunction getCapacity = nullptr;

    /** Whether this entry is the replay viewer. */
    bool isReplayViewer = false;
};

namespace Catalog
{
    /** Registers one static upstream sample entry. */
    int registerSample (const char* category, const char* name, SampleCreateFunction createSample, SampleCapacityFunction getCapacity = nullptr, bool isReplayViewer = false);

    /** Sorts and validates the complete registry once. */
    void initialise();

    /** @returns every registered entry in category and name order. */
    [[nodiscard]] const std::vector<Entry>& getEntries();

    /** @returns the replay viewer's index, or an empty value before it is registered. */
    [[nodiscard]] std::optional<int> getReplayIndex();
}

/** Owns the active sample and all non-visual sample state. */
class Runtime final
{
public:
    /** Constructs the runtime and initialises the complete catalogue. */
    Runtime();

    /** Destroys any auxiliary sample state and the active sample. */
    ~Runtime();

    //==============================================================================
    /** Selects one catalogue entry and reports invalid input or construction failure. */
    [[nodiscard]] Result selectSample (int sampleIndex, bool shouldRestart = false);

    /** Supplies recording bytes and selects the replay viewer. */
    [[nodiscard]] Result selectReplay (const MemoryBlock& recordingData, const String& displayName);

    /** Recreates the active sample using current settings. */
    [[nodiscard]] Result restartSample();

    /** Applies elapsed presentation time as bounded fixed simulation steps, then prepares one stable frame. */
    void updateForPresentation (double presentationTimeSeconds);

    /** Discards the preceding presentation timestamp and any unconsumed elapsed time. */
    void resetPresentationClock() noexcept;

    /** Appends the active sample's items during an open control-description pass.

        @param controls  Receives items during an existing `beginFrame()` / `endFrame()` pass.
    */
    void appendCurrentSampleControls (ControlModel& controls);

    /** Begins recording the active ordinary sample. */
    void startRecording();

    /** Stops recording and returns the resulting bytes when available. */
    [[nodiscard]] std::optional<MemoryBlock> stopRecording();

    /** @returns the active sample, or `nullptr` before selection. */
    [[nodiscard]] Sample* getCurrentSample() noexcept;

    /** @returns the active sample, or `nullptr` before selection. */
    [[nodiscard]] const Sample* getCurrentSample() const noexcept;

    /** @returns the mutable host context. */
    [[nodiscard]] Context& getContext() noexcept;

    /** @returns the custom drawing command list. */
    [[nodiscard]] DrawList& getDrawList() noexcept;

private:
    //==============================================================================
    class Pimpl;

    //==============================================================================
    std::unique_ptr<Pimpl> pimpl;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Runtime)
};

/** Displays the active world and routes camera, pointer, and keyboard input. */
class Canvas final : public Component
{
public:
    /** Constructs an empty sample canvas. */
    Canvas();

    //==============================================================================
    /** Points the canvas at a non-owned runtime. */
    void setRuntime (Runtime* newRuntime) noexcept;

    /** Restores Home and enables responsive refitting until the user pans or zooms. */
    void resetView();

    /** Applies queued keyboard, pointer, pan, zoom, and home input at a presentation boundary. */
    void applyPendingInput();

    /** Refreshes the accessible title, description, and state for a sample change. */
    void updateAccessibility (const String& sampleName, const String& instructions);

    //==============================================================================
    /** @internal */
    void paint (Graphics& graphics) override;
    /** @internal */
    void resized() override;
    /** @internal */
    bool keyPressed (const KeyPress& key) override;
    /** @internal */
    void mouseDown (const MouseEvent& event) override;
    /** @internal */
    void mouseDrag (const MouseEvent& event) override;
    /** @internal */
    void mouseUp (const MouseEvent& event) override;
    /** @internal */
    void mouseMove (const MouseEvent& event) override;
    /** @internal */
    void mouseWheelMove (const MouseEvent& event, const MouseWheelDetails& wheel) override;
    /** @internal */
    std::unique_ptr<AccessibilityHandler> createAccessibilityHandler() override;

private:
    //==============================================================================
    enum class PointerEventType
    {
        down,
        up,
        move
    };

    struct PendingPointerEvent final
    {
        PointerEventType type = PointerEventType::move;
        juce::Point<float> position;
        MouseButton button = MouseButton::primary;
        ModifierKeys modifiers;
    };

    //==============================================================================
    Runtime* runtime = nullptr;
    Box2DRenderer renderer;
    MouseButton activePointerButton = MouseButton::primary;
    bool panActive = false,
         homeRequested = false,
         shouldRefitHomeViewOnResize = true;
    float pendingZoomFactor = 1.0f;
    juce::Point<float> lastPanPosition,
                       pendingPanDelta;
    std::vector<KeyPress> pendingKeyPresses;
    std::vector<PendingPointerEvent> pendingPointerEvents;

    //==============================================================================
    [[nodiscard]] juce::Rectangle<float> getDrawableArea() const noexcept;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Canvas)
};

/** Displays native profile, counter, and sample-supplied metrics. */
class MetricsComponent final : public Component
{
public:
    /** Constructs an empty metrics component. */
    MetricsComponent();

    /** Destroys the metrics component. */
    ~MetricsComponent() override;

    //==============================================================================
    /** Points the metrics view at a non-owned runtime and replaces sample-supplied content. */
    void setRuntime (Runtime* newRuntime);

    /** Shows or hides the built-in profiler and counters without hiding sample-supplied metrics. */
    void setProfilerVisible (bool shouldShowProfiler);

    /** @returns whether the built-in profiler and counters are visible. */
    [[nodiscard]] bool isProfilerVisible() const noexcept;

    /** Refreshes live profile, counter, and replay values. */
    void refresh();

    //==============================================================================
    /** @internal */
    void paint (Graphics& graphics) override;
    /** @internal */
    void resized() override;
    /** @internal */
    std::unique_ptr<AccessibilityHandler> createAccessibilityHandler() override;

private:
    //==============================================================================
    class Pimpl;

    //==============================================================================
    std::unique_ptr<Pimpl> pimpl;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MetricsComponent)
};

} // namespace Box2DSamples

/** Demonstrates every sample registered by the pinned Box2D source tree. */
class Box2DDemo final : public Component
{
public:
    /** Constructs the native Box2D sample browser. */
    Box2DDemo();

    /** Cancels file work and destroys sample-owned views before their sample. */
    ~Box2DDemo() override;

    //==============================================================================
    /** @internal */
    void paint (Graphics& graphics) override;
    /** @internal */
    void resized() override;
    /** @internal */
    void lookAndFeelChanged() override;

private:
    //==============================================================================
    class Pimpl;

    //==============================================================================
    std::unique_ptr<Pimpl> pimpl;

    //==============================================================================
    void updateForPresentation (double presentationTimeSeconds);

   #if JUCE_UNIT_TESTS
    friend struct Box2DDemoTestAccess;
   #endif

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Box2DDemo)
};
