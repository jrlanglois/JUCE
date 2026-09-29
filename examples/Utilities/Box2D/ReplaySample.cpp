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

#include "ReplaySample.h"

namespace Box2DSamples
{

namespace
{

String getBodyTypeName (b2BodyType type)
{
    switch (type)
    {
        case b2_staticBody:    return TRANS ("static");
        case b2_kinematicBody: return TRANS ("kinematic");
        case b2_dynamicBody:   return TRANS ("dynamic");
        case b2_bodyTypeCount: break;
    }

    jassertfalse;
    return TRANS ("unknown");
}

String getShapeTypeName (b2ShapeType type)
{
    switch (type)
    {
        case b2_circleShape:       return TRANS ("circle");
        case b2_capsuleShape:      return TRANS ("capsule");
        case b2_segmentShape:      return TRANS ("segment");
        case b2_polygonShape:      return TRANS ("polygon");
        case b2_chainSegmentShape: return TRANS ("chain segment");
        case b2_shapeTypeCount:    break;
    }

    jassertfalse;
    return TRANS ("unknown");
}

String getJointTypeName (b2JointType type)
{
    switch (type)
    {
        case b2_distanceJoint:  return TRANS ("distance");
        case b2_filterJoint:    return TRANS ("filter");
        case b2_motorJoint:     return TRANS ("motor");
        case b2_moverJoint:     return TRANS ("mover");
        case b2_pogoJoint:      return TRANS ("pogo");
        case b2_prismaticJoint: return TRANS ("prismatic");
        case b2_revoluteJoint:  return TRANS ("revolute");
        case b2_weldJoint:      return TRANS ("weld");
        case b2_wheelJoint:     return TRANS ("wheel");
    }

    jassertfalse;
    return TRANS ("unknown");
}

String getQueryTypeName (b2ReplayQueryType type)
{
    switch (type)
    {
        case b2_replayQueryOverlapAABB:    return TRANS ("overlap AABB");
        case b2_replayQueryOverlapShape:   return TRANS ("overlap shape");
        case b2_replayQueryCastRay:        return TRANS ("cast ray");
        case b2_replayQueryCastShape:      return TRANS ("cast shape");
        case b2_replayQueryCollideMover:   return TRANS ("collide mover");
        case b2_replayQueryCastRayClosest: return TRANS ("cast ray closest");
        case b2_replayQueryCastMover:      return TRANS ("cast mover");
        case b2_replayQueryShapeTestPoint: return TRANS ("shape test point");
        case b2_replayQueryShapeRayCast:   return TRANS ("shape ray cast");
    }

    jassertfalse;
    return TRANS ("unknown");
}

template<typename PositionType>
String getCoordinateText (PositionType value) { return String (value.x, 3) + ", " + String (value.y, 3); }

class ReplayRefreshableComponent : public Component
{
public:
    virtual void refresh() = 0;
};

enum class SelectionKind
{
    none,
    body,
    shape,
    joint,
    query
};

struct Selection final
{
    SelectionKind kind = SelectionKind::none;
    int bodyOrdinal = -1,
        slot = -1,
        queryIndex = -1;
};

struct PickContext final
{
    b2Pos point;
    b2ShapeId shapeId;
};

} // namespace

class ReplaySample final : public Sample
{
public:
    explicit ReplaySample (Context& newContext) :
        Sample (newContext, false),
        creationResult (Result::ok())
    {
        if (context.replayData.isEmpty())
        {
            context.settings.isPaused = true;
            return;
        }

        if (context.replayData.getSize() > (size_t) std::numeric_limits<int>::max())
        {
            creationResult = Result::fail (TRANS ("The recording is too large to open."));
            return;
        }

        player = b2CreateReplay (context.replayData.getData(),
                                 (int) context.replayData.getSize(),
                                 context.settings.numWorkers);

        if (player == nullptr)
        {
            creationResult = Result::fail (TRANS ("The recording is malformed or incompatible with this Box2D build."));
            return;
        }

        worldId = b2Replay_GetWorldId (player);
        info = b2Replay_GetInfo (player);
        b2Replay_SetKeyframePolicy (player, (size_t) keyframeBudgetMB * 1024 * 1024, keyframeMinInterval);
        context.settings.isPaused = true;
        resetCamera();
    }

    ~ReplaySample() override
    {
        worldId = b2_nullWorldId;

        if (player != nullptr)
            b2DestroyReplay (player);
    }

    void applyPendingChanges()
    {
        if (pendingPlaybackSpeed.has_value())
        {
            playbackSpeed = std::clamp (*pendingPlaybackSpeed, 0.25f, 4.0f);
            pendingPlaybackSpeed.reset();
        }

        if (pendingLoopState.has_value())
        {
            shouldLoop = *pendingLoopState;
            pendingLoopState.reset();
        }

        if (pendingSelection.has_value())
        {
            selectionKind = pendingSelection->kind;
            selectedBodyOrdinal = pendingSelection->bodyOrdinal;
            selectedSlot = pendingSelection->slot;
            selectedQueryIndex = pendingSelection->queryIndex;
            pendingSelection.reset();
        }

        if (player != nullptr && shouldApplyKeyframePolicy)
        {
            b2Replay_SetKeyframePolicy (player, (size_t) keyframeBudgetMB * 1024 * 1024, keyframeMinInterval);
            b2Replay_Restart (player);
            worldId = b2Replay_GetWorldId (player);
            context.settings.isPaused = true;
            shouldApplyKeyframePolicy = false;
        }

        if (player != nullptr && pendingFrame.has_value())
        {
            b2Replay_SeekFrame (player, std::clamp (*pendingFrame, 0, info.frameCount));
            worldId = b2Replay_GetWorldId (player);
            context.settings.isPaused = true;
            pendingFrame.reset();
        }

        if (pendingPlaying.has_value())
        {
            context.settings.isPaused = ! *pendingPlaying;
            pendingPlaying.reset();
        }
    }

    void queueSeek (int frame) { pendingFrame = frame; }

    void queuePlaying (bool shouldPlay) { pendingPlaying = shouldPlay; }

    void queueSpeed (float speedMultiplier) { pendingPlaybackSpeed = speedMultiplier; }

    void queueLoopState (bool shouldEnableLooping) { pendingLoopState = shouldEnableLooping; }

    int getFrame() const noexcept { return player != nullptr ? b2Replay_GetFrame (player) : 0; }

    int getNumFrames() const noexcept { return info.frameCount; }

    int getNumQueries() const noexcept { return player != nullptr ? b2Replay_GetFrameQueryCount (player) : 0; }

    float getPlaybackSpeed() const noexcept { return playbackSpeed; }

    bool isLooping() const noexcept { return shouldLoop; }

    bool isPlaying() const noexcept { return ! context.settings.isPaused; }

    bool isAtEnd() const noexcept { return player != nullptr && b2Replay_IsAtEnd (player); }

    bool hasDiverged() const noexcept { return player != nullptr && b2Replay_HasDiverged (player); }

    size_t getKeyframeBytes() const noexcept { return player != nullptr ? b2Replay_GetKeyframeBytes (player) : 0; }

    int getKeyframeInterval() const noexcept { return player != nullptr ? b2Replay_GetKeyframeInterval (player) : 0; }

    Result getCreationResult() const { return creationResult; }

    int getNumBodySlots() const noexcept { return player != nullptr ? b2Replay_GetBodyCount (player) : 0; }

    b2BodyId getBodyId (int bodyOrdinal) const noexcept { return player != nullptr ? b2Replay_GetBodyId (player, bodyOrdinal) : b2_nullBodyId; }

    String getBodyLabel (int bodyOrdinal) const
    {
        const b2BodyId bodyId = getBodyId (bodyOrdinal);

        if (! b2Body_IsValid (bodyId))
            return {};

        String label = TRANS ("Body {bodyOrdinal}").replace ("{bodyOrdinal}", String (bodyOrdinal));
        const char* name = b2Body_GetName (bodyId);

        if (name != nullptr && name[0] != '\0')
            label += " " + String::fromUTF8 ("\xe2\x80\x94") + " " + String::fromUTF8 (name);

        label += " (" + getBodyTypeName (b2Body_GetType (bodyId)) + ")";
        return label;
    }

    int getNumShapes (int bodyOrdinal) const noexcept
    {
        const b2BodyId bodyId = getBodyId (bodyOrdinal);
        return b2Body_IsValid (bodyId) ? b2Body_GetShapeCount (bodyId) : 0;
    }

    int getNumJoints (int bodyOrdinal) const noexcept
    {
        const b2BodyId bodyId = getBodyId (bodyOrdinal);
        return b2Body_IsValid (bodyId) ? b2Body_GetJointCount (bodyId) : 0;
    }

    String getShapeLabel (int bodyOrdinal, int shapeIndex) const
    {
        const b2ShapeId shapeId = getShapeId (bodyOrdinal, shapeIndex);

        if (! b2Shape_IsValid (shapeId))
            return {};

        return TRANS ("Shape {shapeIndex}: {shapeType}")
            .replace ("{shapeIndex}", String (shapeIndex))
            .replace ("{shapeType}", getShapeTypeName (b2Shape_GetType (shapeId)));
    }

    String getJointLabel (int bodyOrdinal, int jointIndex) const
    {
        const b2JointId jointId = getJointId (bodyOrdinal, jointIndex);

        if (! b2Joint_IsValid (jointId))
            return {};

        return TRANS ("Joint {jointIndex}: {jointType}")
            .replace ("{jointIndex}", String (jointIndex))
            .replace ("{jointType}", getJointTypeName (b2Joint_GetType (jointId)));
    }

    String getQueryLabel (int queryIndex) const
    {
        if (! isPositiveAndBelow (queryIndex, getNumQueries()))
            return {};

        const b2ReplayQueryInfo query = b2Replay_GetFrameQuery (player, queryIndex);
        return TRANS ("Query {queryIndex}: {queryType}")
            .replace ("{queryIndex}", String (queryIndex))
            .replace ("{queryType}", getQueryTypeName (query.type));
    }

    String getSelectionDetail() const
    {
        if (player == nullptr)
            return TRANS ("No recording is loaded.");

        if (selectionKind == SelectionKind::query && isPositiveAndBelow (selectedQueryIndex, getNumQueries()))
        {
            const b2ReplayQueryInfo query = b2Replay_GetFrameQuery (player, selectedQueryIndex);
            return TRANS ("Query {queryIndex}\nType: {queryType}\nOrigin: {origin}\nTranslation: {translation}\nHits: {numHits}")
                .replace ("{queryIndex}", String (selectedQueryIndex))
                .replace ("{queryType}", getQueryTypeName (query.type))
                .replace ("{origin}", getCoordinateText (query.origin))
                .replace ("{translation}", getCoordinateText (query.translation))
                .replace ("{numHits}", String (query.hitCount));
        }

        const b2BodyId bodyId = getSelectedBody();

        if (! b2Body_IsValid (bodyId))
            return TRANS ("Select a body, shape, joint, or recorded query.");

        if (selectionKind == SelectionKind::shape)
        {
            const b2ShapeId shapeId = getShapeId (selectedBodyOrdinal, selectedSlot);

            if (b2Shape_IsValid (shapeId))
            {
                const b2AABB bounds = b2Shape_GetAABB (shapeId);
                return TRANS ("Shape {shapeIndex}\nType: {shapeType}\nLower bound: {lowerBound}\nUpper bound: {upperBound}")
                    .replace ("{shapeIndex}", String (selectedSlot))
                    .replace ("{shapeType}", getShapeTypeName (b2Shape_GetType (shapeId)))
                    .replace ("{lowerBound}", getCoordinateText (bounds.lowerBound))
                    .replace ("{upperBound}", getCoordinateText (bounds.upperBound));
            }
        }

        if (selectionKind == SelectionKind::joint)
        {
            const b2JointId jointId = getJointId (selectedBodyOrdinal, selectedSlot);

            if (b2Joint_IsValid (jointId))
            {
                return TRANS ("Joint {jointIndex}\nType: {jointType}")
                    .replace ("{jointIndex}", String (selectedSlot))
                    .replace ("{jointType}", getJointTypeName (b2Joint_GetType (jointId)));
            }
        }

        return TRANS ("Body {bodyOrdinal}\nType: {bodyType}\nPosition: {position}\nLinear velocity: {linearVelocity}\nAngular velocity: {angularVelocity}\nMass: {mass}\nAwake: {isAwake}")
            .replace ("{bodyOrdinal}", String (selectedBodyOrdinal))
            .replace ("{bodyType}", getBodyTypeName (b2Body_GetType (bodyId)))
            .replace ("{position}", getCoordinateText (b2Body_GetPosition (bodyId)))
            .replace ("{linearVelocity}", getCoordinateText (b2Body_GetLinearVelocity (bodyId)))
            .replace ("{angularVelocity}", String (b2Body_GetAngularVelocity (bodyId), 3))
            .replace ("{mass}", String (b2Body_GetMass (bodyId), 3))
            .replace ("{isAwake}", b2Body_IsAwake (bodyId) ? TRANS ("yes") : TRANS ("no"));
    }

    void queueBodySelection (int bodyOrdinal)
    {
        Selection selection;
        selection.kind = SelectionKind::body;
        selection.bodyOrdinal = bodyOrdinal;
        queueSelection (selection);
    }

    void queueShapeSelection (int bodyOrdinal, int shapeIndex)
    {
        Selection selection;
        selection.kind = SelectionKind::shape;
        selection.bodyOrdinal = bodyOrdinal;
        selection.slot = shapeIndex;
        queueSelection (selection);
    }

    void queueJointSelection (int bodyOrdinal, int jointIndex)
    {
        Selection selection;
        selection.kind = SelectionKind::joint;
        selection.bodyOrdinal = bodyOrdinal;
        selection.slot = jointIndex;
        queueSelection (selection);
    }

    void queueQuerySelection (int queryIndex)
    {
        Selection selection;
        selection.kind = SelectionKind::query;
        selection.queryIndex = queryIndex;
        queueSelection (selection);
    }

    double getStepIntervalSeconds() const noexcept override
    {
        if (player == nullptr || info.timeStep <= 0.0f || playbackSpeed <= 0.0f)
            return 0.0;

        return (double) info.timeStep / (double) playbackSpeed;
    }

    void advanceSimulation() override
    {
        if (player == nullptr)
            return;

        if (b2Replay_IsAtEnd (player))
        {
            if (! shouldLoop)
            {
                context.settings.isPaused = true;
                return;
            }

            b2Replay_Restart (player);
            worldId = b2Replay_GetWorldId (player);
        }

        if (b2Replay_StepFrame (player))
        {
            worldId = b2Replay_GetWorldId (player);
            recordSimulationStep();
        }
    }

    void prepareFrame() override
    {
        if (player == nullptr)
        {
            addScreenTextLine (TRANS ("Open a Box2D recording to use Replay Viewer."));
            return;
        }

        context.drawList.getDebugDraw().drawingBounds = context.camera.getVisibleBounds();
        const int queryIndex = selectionKind == SelectionKind::query ? selectedQueryIndex : -1;
        b2Replay_DrawFrameQueries (player, &context.drawList.getDebugDraw(), queryIndex);

        const b2BodyId selectedBody = getSelectedBody();

        if (b2Body_IsValid (selectedBody))
            context.drawList.addBounds (b2Body_ComputeAABB (selectedBody), b2_colorOrange);

        String status = TRANS ("Frame {currentFrame} of {numFrames}.")
                            .replace ("{currentFrame}", String (getFrame()))
                            .replace ("{numFrames}", String (info.frameCount));

        if (b2Replay_HasDiverged (player))
        {
            status += " ";
            status += TRANS ("Replay diverged at frame {frame}.")
                          .replace ("{frame}", String (b2Replay_GetDivergeFrame (player)));
        }

        addScreenTextLine (status);
    }

    void updateControls (ControlModel& controls) override
    {
        if (player == nullptr)
        {
            controls.showText ("replayStatus", TRANS ("No recording is loaded."), ControlItem::TextTone::secondary);
            return;
        }

        controls.showText ("replayName",
                           TRANS ("Recording: {recordingName}").replace ("{recordingName}", context.replayName),
                           ControlItem::TextTone::subheading);
        controls.showButton ("replayFirst", TRANS ("First frame"), [this] { queueSeek (0); });
        controls.showButton ("replayPrevious", TRANS ("Previous frame"), [this] { queueSeek (getFrame() - 1); });
        controls.showButton ("replayPlayPause",
                             context.settings.isPaused ? TRANS ("Play") : TRANS ("Pause"),
                             [this] { queuePlaying (context.settings.isPaused); });
        controls.showButton ("replayNext", TRANS ("Next frame"), [this] { queueSeek (getFrame() + 1); });
        controls.showButton ("replayLast", TRANS ("Last frame"), [this] { queueSeek (info.frameCount); });
        timelineFrame = getFrame();
        controls.showIntegerSlider ("replayTimeline",
                                    TRANS ("Replay timeline"),
                                    timelineFrame,
                                    0,
                                    std::max (1, info.frameCount),
                                    [this] { queueSeek (timelineFrame); });
        controls.setHelpText ("replayTimeline", TRANS ("Selects the current replay frame."));
        controls.showFloatSlider ("replaySpeed", TRANS ("Playback speed"), playbackSpeed, 0.25f, 4.0f, 2);
        controls.showToggle ("replayLoop", TRANS ("Loop playback"), shouldLoop);
        controls.showIntegerSlider ("replayKeyframeBudget",
                                    TRANS ("Keyframe memory, MB"),
                                    keyframeBudgetMB,
                                    16,
                                    1024,
                                    [this] { shouldApplyKeyframePolicy = true; });
        controls.showIntegerSlider ("replayKeyframeInterval",
                                    TRANS ("Minimum keyframe interval"),
                                    keyframeMinInterval,
                                    1,
                                    240,
                                    [this] { shouldApplyKeyframePolicy = true; });
    }

    bool handleKeyPress (const KeyPress& key) override
    {
        if (player == nullptr)
            return false;

        if (key == KeyPress::escapeKey)
        {
            queueSelection ({});
            return true;
        }

        if (key.isKeyCode (','))
        {
            queueSeek (getFrame() - (key.getModifiers().isShiftDown() ? 5 : 1));
            return true;
        }

        return false;
    }

    void handleMouseDown (b2Pos position, const ModifierKeys& modifiers) override
    {
        if (player == nullptr || ! modifiers.isLeftButtonDown() || ! b2World_IsValid (worldId))
            return;

        const b2Vec2 delta = { 0.001f, 0.001f };
        const b2AABB bounds = { b2Neg (delta), delta };
        PickContext pickContext { position, b2_nullShapeId };
        b2World_OverlapAABB (worldId, position, bounds, b2DefaultQueryFilter(), pickShape, &pickContext);

        Selection selection;

        if (b2Shape_IsValid (pickContext.shapeId))
        {
            const b2BodyId bodyId = b2Shape_GetBody (pickContext.shapeId);
            selection.kind = SelectionKind::shape;
            selection.bodyOrdinal = findBodyOrdinal (bodyId);

            const int numShapes = b2Body_GetShapeCount (bodyId);
            std::vector<b2ShapeId> shapeIds ((size_t) numShapes);
            b2Body_GetShapes (bodyId, shapeIds.data(), numShapes);

            for (int shapeIndex = 0; shapeIndex < numShapes; ++shapeIndex)
            {
                if (B2_ID_EQUALS (shapeIds[(size_t) shapeIndex], pickContext.shapeId))
                {
                    selection.slot = shapeIndex;
                    break;
                }
            }
        }

        queueSelection (selection);
    }

    void resetCamera() override
    {
        if (player == nullptr || ! b2World_IsValid (worldId))
        {
            Sample::resetCamera();
            return;
        }

        b2AABB bounds = info.bounds;
        const b2Vec2 extents = b2AABB_Extents (bounds);

        if (extents.x <= 0.0f && extents.y <= 0.0f)
            bounds = b2World_GetBounds (worldId);

        context.camera.setViewToBounds (bounds);
        context.camera.zoom *= 1.5f;
    }

    bool hasSolverControls() const noexcept override { return false; }

    bool hasProfile() const noexcept override { return false; }

    std::unique_ptr<Component> createInspectorComponent() override;

    std::unique_ptr<Component> createMetricsComponent() override;


private:
    b2Replay* player = nullptr;
    b2ReplayInfo info = {};
    Result creationResult;
    float playbackSpeed = 1.0f;
    int keyframeBudgetMB = 64,
        keyframeMinInterval = 16,
        timelineFrame = 0,
        selectedBodyOrdinal = -1,
        selectedSlot = -1,
        selectedQueryIndex = -1;
    SelectionKind selectionKind = SelectionKind::none;
    bool shouldLoop = false,
         shouldApplyKeyframePolicy = false;
    std::optional<float> pendingPlaybackSpeed;
    std::optional<int> pendingFrame;
    std::optional<bool> pendingPlaying,
                        pendingLoopState;
    std::optional<Selection> pendingSelection;

    static bool pickShape (b2ShapeId shapeId, void* userData)
    {
        auto* pickContext = static_cast<PickContext*> (userData);

        if (b2Shape_TestPoint (shapeId, pickContext->point))
        {
            pickContext->shapeId = shapeId;
            return false;
        }

        return true;
    }

    int findBodyOrdinal (b2BodyId bodyId) const noexcept
    {
        for (int bodyOrdinal = 0; bodyOrdinal < getNumBodySlots(); ++bodyOrdinal)
        {
            if (B2_ID_EQUALS (getBodyId (bodyOrdinal), bodyId))
                return bodyOrdinal;
        }

        return -1;
    }

    b2ShapeId getShapeId (int bodyOrdinal, int shapeIndex) const
    {
        const b2BodyId bodyId = getBodyId (bodyOrdinal);

        if (! b2Body_IsValid (bodyId))
            return b2_nullShapeId;

        const int numShapes = b2Body_GetShapeCount (bodyId);

        if (! isPositiveAndBelow (shapeIndex, numShapes))
            return b2_nullShapeId;

        std::vector<b2ShapeId> shapeIds ((size_t) numShapes);
        b2Body_GetShapes (bodyId, shapeIds.data(), numShapes);
        return shapeIds[(size_t) shapeIndex];
    }

    b2JointId getJointId (int bodyOrdinal, int jointIndex) const
    {
        const b2BodyId bodyId = getBodyId (bodyOrdinal);

        if (! b2Body_IsValid (bodyId))
            return b2_nullJointId;

        const int numJoints = b2Body_GetJointCount (bodyId);

        if (! isPositiveAndBelow (jointIndex, numJoints))
            return b2_nullJointId;

        std::vector<b2JointId> jointIds ((size_t) numJoints);
        b2Body_GetJoints (bodyId, jointIds.data(), numJoints);
        return jointIds[(size_t) jointIndex];
    }

    b2BodyId getSelectedBody() const noexcept { return selectionKind == SelectionKind::query ? b2_nullBodyId : getBodyId (selectedBodyOrdinal); }

    void queueSelection (Selection selection) { pendingSelection = selection; }
};

namespace
{

class ReplayTreeItem final : public TreeViewItem
{
public:
    ReplayTreeItem (String newText, std::function<void()> newSelectionAction) : text (std::move (newText)), selectionAction (std::move (newSelectionAction)) {}

    bool mightContainSubItems() override { return getNumSubItems() > 0; }

    String getUniqueName() const override { return text; }

    void paintItem (Graphics& graphics, int width, int height) override
    {
        if (isSelected())
        {
            graphics.setColour (Colours::cornflowerblue.withAlpha (0.35f));
            graphics.fillRect (0, 0, width, height);
        }

        graphics.setColour (Colours::white);
        graphics.drawText (text, 2, 0, width - 4, height, Justification::centredLeft, true);
    }

    void itemSelectionChanged (bool isNowSelected) override
    {
        TreeViewItem::itemSelectionChanged (isNowSelected);

        if (isNowSelected && selectionAction != nullptr)
            selectionAction();
    }

private:
    String text;
    std::function<void()> selectionAction;
};

class ReplayInspectorComponent final : public ReplayRefreshableComponent
{
public:
    explicit ReplayInspectorComponent (ReplaySample& newSample) : sample (newSample)
    {
        setTitle (TRANS ("Replay outline and detail"));
        tree.setTitle (TRANS ("Replay scene outline"));
        tree.setRootItemVisible (false);
        detail.setTitle (TRANS ("Replay selection detail"));
        detail.setMultiLine (true);
        detail.setReadOnly (true);
        detail.setCaretVisible (false);
        addAndMakeVisible (tree);
        addAndMakeVisible (detail);
        refresh();
    }

    ~ReplayInspectorComponent() override { tree.setRootItem (nullptr); }

    void refresh() override
    {
        rebuildTree();
        detail.setText (sample.getSelectionDetail(), false);
    }

    void resized() override
    {
        auto bounds = getLocalBounds().reduced (4);
        tree.setBounds (bounds.removeFromTop (bounds.getHeight() / 2));
        detail.setBounds (bounds);
    }

private:
    ReplaySample& sample;
    TreeView tree;
    TextEditor detail;
    std::unique_ptr<ReplayTreeItem> rootItem;

    void rebuildTree()
    {
        tree.setRootItem (nullptr);
        rootItem = std::make_unique<ReplayTreeItem> (TRANS ("Replay"), std::function<void()>());

        auto bodiesItem = std::make_unique<ReplayTreeItem> (TRANS ("Bodies"), std::function<void()>());

        for (int bodyOrdinal = 0; bodyOrdinal < sample.getNumBodySlots(); ++bodyOrdinal)
        {
            const String bodyLabel = sample.getBodyLabel (bodyOrdinal);

            if (bodyLabel.isEmpty())
                continue;

            auto bodyItem = std::make_unique<ReplayTreeItem> (bodyLabel, [this, bodyOrdinal]
            {
                sample.queueBodySelection (bodyOrdinal);
            });

            for (int shapeIndex = 0; shapeIndex < sample.getNumShapes (bodyOrdinal); ++shapeIndex)
            {
                auto shapeItem = std::make_unique<ReplayTreeItem> (sample.getShapeLabel (bodyOrdinal, shapeIndex),
                                                                  [this, bodyOrdinal, shapeIndex]
                                                                  {
                                                                      sample.queueShapeSelection (bodyOrdinal, shapeIndex);
                                                                  });
                bodyItem->addSubItem (shapeItem.release());
            }

            for (int jointIndex = 0; jointIndex < sample.getNumJoints (bodyOrdinal); ++jointIndex)
            {
                auto jointItem = std::make_unique<ReplayTreeItem> (sample.getJointLabel (bodyOrdinal, jointIndex),
                                                                  [this, bodyOrdinal, jointIndex]
                                                                  {
                                                                      sample.queueJointSelection (bodyOrdinal, jointIndex);
                                                                  });
                bodyItem->addSubItem (jointItem.release());
            }

            bodiesItem->addSubItem (bodyItem.release());
        }

        auto queriesItem = std::make_unique<ReplayTreeItem> (TRANS ("Recorded queries"), std::function<void()>());

        for (int queryIndex = 0; queryIndex < sample.getNumQueries(); ++queryIndex)
        {
            auto queryItem = std::make_unique<ReplayTreeItem> (sample.getQueryLabel (queryIndex), [this, queryIndex]
            {
                sample.queueQuerySelection (queryIndex);
            });
            queriesItem->addSubItem (queryItem.release());
        }

        bodiesItem->setOpen (true);
        queriesItem->setOpen (true);
        rootItem->addSubItem (bodiesItem.release());
        rootItem->addSubItem (queriesItem.release());
        tree.setRootItem (rootItem.get());
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ReplayInspectorComponent)
};

class ReplayMetricsComponent final : public ReplayRefreshableComponent
{
public:
    explicit ReplayMetricsComponent (ReplaySample& newSample) :
        sample (newSample),
        progressBar (progress)
    {
        setTitle (TRANS ("Replay status and keyframes"));
        statusLabel.setTitle (TRANS ("Replay status"));
        keyframeLabel.setTitle (TRANS ("Replay keyframe status"));
        progressBar.setTitle (TRANS ("Replay progress"));

        for (auto* component : { static_cast<Component*> (&statusLabel),
                                 static_cast<Component*> (&progressBar),
                                 static_cast<Component*> (&keyframeLabel) })
            addAndMakeVisible (component);

        refresh();
    }

    void refresh() override
    {
        const int numFrames = sample.getNumFrames(),
                  frame = sample.getFrame();
        progress = numFrames > 0 ? (double) frame / (double) numFrames : 0.0;

        String status = TRANS ("Frame {currentFrame} of {numFrames}.")
                            .replace ("{currentFrame}", String (frame))
                            .replace ("{numFrames}", String (numFrames));

        if (sample.isAtEnd())
            status += " " + TRANS ("End of recording.");

        if (sample.hasDiverged())
            status += " " + TRANS ("Replay has diverged.");

        statusLabel.setText (status, dontSendNotification);

        if (sample.hasDiverged() && ! divergenceWasAnnounced)
        {
            AccessibilityHandler::postAnnouncement (TRANS ("Replay has diverged."), AccessibilityHandler::AnnouncementPriority::medium);
            divergenceWasAnnounced = true;
        }
        else if (! sample.hasDiverged())
        {
            divergenceWasAnnounced = false;
        }

        keyframeLabel.setText (TRANS ("Keyframes: {numKilobytes} KB at {interval}-frame spacing.")
                                   .replace ("{numKilobytes}", String (sample.getKeyframeBytes() / 1024))
                                   .replace ("{interval}", String (sample.getKeyframeInterval())),
                               dontSendNotification);
    }

    void resized() override
    {
        auto bounds = getLocalBounds().reduced (4);
        statusLabel.setBounds (bounds.removeFromTop (22));
        progressBar.setBounds (bounds.removeFromTop (18));
        keyframeLabel.setBounds (bounds.removeFromTop (22));
    }

private:
    ReplaySample& sample;
    double progress = 0.0;
    bool divergenceWasAnnounced = false;
    Label statusLabel,
          keyframeLabel;
    ProgressBar progressBar;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ReplayMetricsComponent)
};

} // namespace

std::unique_ptr<Component> ReplaySample::createInspectorComponent() { return std::make_unique<ReplayInspectorComponent> (*this); }

std::unique_ptr<Component> ReplaySample::createMetricsComponent() { return std::make_unique<ReplayMetricsComponent> (*this); }

std::unique_ptr<Sample> createReplaySample (Context& context) { return std::make_unique<ReplaySample> (context); }

Result getReplaySampleCreationResult (const Sample& sample)
{
    if (const auto* replay = dynamic_cast<const ReplaySample*> (&sample))
        return replay->getCreationResult();

    return Result::ok();
}

void applyReplayPendingChanges (Sample& sample)
{
    if (auto* replay = dynamic_cast<ReplaySample*> (&sample))
        replay->applyPendingChanges();
}

void refreshReplayComponent (Component& component)
{
    if (auto* replayComponent = dynamic_cast<ReplayRefreshableComponent*> (&component))
        replayComponent->refresh();
}

bool isReplaySample (const Sample& sample) noexcept { return dynamic_cast<const ReplaySample*> (&sample) != nullptr; }

int getReplayFrame (const Sample& sample) noexcept
{
    if (const auto* replay = dynamic_cast<const ReplaySample*> (&sample))
        return replay->getFrame();

    return 0;
}

int getReplayNumFrames (const Sample& sample) noexcept
{
    if (const auto* replay = dynamic_cast<const ReplaySample*> (&sample))
        return replay->getNumFrames();

    return 0;
}

int getReplayNumQueries (const Sample& sample) noexcept
{
    if (const auto* replay = dynamic_cast<const ReplaySample*> (&sample))
        return replay->getNumQueries();

    return 0;
}

void queueReplaySeek (Sample& sample, int frame)
{
    if (auto* replay = dynamic_cast<ReplaySample*> (&sample))
        replay->queueSeek (frame);
}

void queueReplayPlaying (Sample& sample, bool shouldPlay)
{
    if (auto* replay = dynamic_cast<ReplaySample*> (&sample))
        replay->queuePlaying (shouldPlay);
}

void queueReplaySpeed (Sample& sample, float speedMultiplier)
{
    if (auto* replay = dynamic_cast<ReplaySample*> (&sample))
        replay->queueSpeed (speedMultiplier);
}

} // namespace Box2DSamples
