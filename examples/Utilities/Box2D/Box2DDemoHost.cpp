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
#include "ReplayFileIO.h"
#include "ReplaySample.h"

namespace
{

[[nodiscard]] String formatCalibrationValue (float value)
{
    const float normalisedValue = std::abs (value) < 0.00005f ? 0.0f : value;
    return String (normalisedValue, 4);
}

[[nodiscard]] String formatBoundsInitializer (b2AABB bounds)
{
    return String ("{ { ") + formatCalibrationValue (bounds.lowerBound.x) + "f, "
         + formatCalibrationValue (bounds.lowerBound.y) + "f }, { "
         + formatCalibrationValue (bounds.upperBound.x) + "f, "
         + formatCalibrationValue (bounds.upperBound.y) + "f } }";
}

class EmbeddedComponentProperty final : public PropertyComponent
{
public:
    EmbeddedComponentProperty (Component& componentIn, int preferredComponentHeight) :
        PropertyComponent (TRANS ("Metrics"), preferredComponentHeight),
        component (componentIn)
    {
        addAndMakeVisible (component);
    }

    ~EmbeddedComponentProperty() override { removeChildComponent (&component); }

    void refresh() override {}

    void resized() override { component.setBounds (getLocalBounds()); }

private:
    Component& component;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EmbeddedComponentProperty)
};

} // namespace

class Box2DDemo::Pimpl final
{
public:
    explicit Pimpl (Box2DDemo& ownerIn) :
        owner (ownerIn),
        vblankAttachment (&owner,
                          [safeOwner = SafePointer<Box2DDemo> (&owner)] (double presentationTimeSeconds)
                          {
                              if (safeOwner != nullptr)
                                  safeOwner->updateForPresentation (presentationTimeSeconds);
                          })
    {
        canvas.setRuntime (&runtime);
        propertyPanel.setExplicitFocusOrder (1);
        canvas.setExplicitFocusOrder (2);
        metricsContent.addAndMakeVisible (metrics);
        refreshTranslations();

        if (! filteredSampleIndices.empty())
        {
            pendingCatalogueIndex = filteredSampleIndices.front();

            if (const auto identifier = getCatalogueIdentifier (*pendingCatalogueIndex))
                selectedSampleIdentifier = *identifier;
        }

        refreshControlModel();
        rebuildPropertiesIfNeeded();
        hasFinishedConstruction = true;
    }

    ~Pimpl() { destroyAuxiliaryViews(); }

    void attachToOwner()
    {
        owner.addAndMakeVisible (propertyPanel);
        owner.addAndMakeVisible (canvas);
    }

    void updatePresentation (double presentationTimeSeconds)
    {
        if (! owner.isShowing())
        {
            runtime.resetPresentationClock();
            return;
        }

        if (activeLanguageSignature != getCurrentLanguageSignature())
            refreshTranslations();

        controlModel.applyQueuedEdits();
        applyPendingCatalogueSelection();
        applyPendingReplayRead();
        canvas.applyPendingInput();
        runtime.updateForPresentation (presentationTimeSeconds);
        refreshControlModel();
        metrics.refresh();

        if (inspector != nullptr)
            Box2DSamples::refreshReplayComponent (*inspector);

        rebuildPropertiesIfNeeded();
        propertyPanel.refreshAll();
        layoutMetricsContent();
        canvas.repaint();
    }

    void layoutComponents (juce::Rectangle<int> bounds)
    {
        const int maxPanelWidth = std::max (140, bounds.getWidth() - 160);
        const int panelWidth = std::min (std::clamp (bounds.getWidth() / 3, 220, 360), maxPanelWidth);
        propertyPanel.setBounds (bounds.removeFromLeft (panelWidth));
        bounds.removeFromLeft (4);
        canvas.setBounds (bounds);
        layoutMetricsContent();
    }

    void refreshTranslations()
    {
        activeLanguageSignature = getCurrentLanguageSignature();
        const auto selectedCategory = getSelectedCategory();
        propertyPanel.setTitle (TRANS ("Box2D controls"));
        propertyPanel.setDescription (TRANS ("Selects a Box2D sample and configures its simulation and presentation."));
        rebuildCategoryChoices (selectedCategory);
        refreshCatalogueFilter();
        refreshControlModel();
        metrics.refresh();
        propertiesNeedRebuild = true;
        shouldRestorePropertyState = false;

        for (auto* component : { static_cast<Component*> (&propertyPanel),
                                 static_cast<Component*> (&canvas),
                                 static_cast<Component*> (&metrics) })
            component->invalidateAccessibilityHandler();

        rebuildPropertiesIfNeeded();
    }

    Box2DDemo& owner;
    Box2DSamples::Runtime runtime;
    Box2DSamples::Canvas canvas;
    Box2DSamples::ControlModel controlModel;
    Box2DSamples::MetricsComponent metrics;
    Component metricsContent;
    PropertyPanel propertyPanel;
    std::unique_ptr<Component> inspector;
    std::unique_ptr<FileChooser> openReplayChooser,
                                 saveRecordingChooser;
    ThreadPool fileIOThreadPool { 1 };
    std::optional<int> activeCatalogueIndex,
                       pendingCatalogueIndex;
    std::optional<Box2DSamples::ReplayFileReadResult> pendingReplayRead;
    std::optional<MemoryBlock> lastRecording;
    std::optional<String> fileStatus;
    std::vector<int> filteredSampleIndices;
    StringArray categoryKeys;
    String activeLanguageSignature,
           catalogueSearch,
           selectedCategoryIdentifier = "catalogue.allCategories",
           selectedSampleIdentifier;
    uint64 renderedControlStructureRevision = 0;
    bool recordingActive = false,
         fileIOBusy = false,
         hasFinishedConstruction = false,
         propertiesNeedRebuild = true,
         renderedMetricsVisible = false,
         profilerVisible = false,
         shouldResetPropertyScrollPosition = false,
         shouldRestorePropertyState = true;
    VBlankAttachment vblankAttachment;

private:
    static String getCurrentLanguageSignature()
    {
        if (const auto* mappings = LocalisedStrings::getCurrentMappings())
            return mappings->getLanguageName() + "\n" + mappings->getCountryCodes().joinIntoString (" ") + "\n" + String (mappings->getMappings().size());

        return {};
    }

    std::optional<String> getCatalogueIdentifier (int catalogueIndex) const
    {
        if (! isPositiveAndBelow (catalogueIndex, (int) Box2DSamples::Catalog::getEntries().size()))
            return std::nullopt;

        const auto& entry = Box2DSamples::Catalog::getEntries()[(size_t) catalogueIndex];
        return String::fromUTF8 (entry.category) + "\n" + String::fromUTF8 (entry.name);
    }

    std::optional<int> getCatalogueIndex (const String& identifier) const
    {
        for (const int catalogueIndex : filteredSampleIndices)
        {
            if (const auto catalogueIdentifier = getCatalogueIdentifier (catalogueIndex);
                catalogueIdentifier.has_value() && *catalogueIdentifier == identifier)
                return std::optional<int> (catalogueIndex);
        }

        return std::nullopt;
    }

    std::optional<String> getSelectedCategory() const
    {
        if (selectedCategoryIdentifier == "catalogue.allCategories")
            return std::nullopt;

        return selectedCategoryIdentifier;
    }

    void rebuildCategoryChoices (const std::optional<String>& selectedCategory)
    {
        categoryKeys.clear();

        for (const auto& entry : Box2DSamples::Catalog::getEntries())
        {
            const String category = String::fromUTF8 (entry.category);

            if (! categoryKeys.contains (category))
                categoryKeys.add (category);
        }

        selectedCategoryIdentifier = "catalogue.allCategories";

        if (selectedCategory.has_value() && categoryKeys.contains (*selectedCategory))
            selectedCategoryIdentifier = *selectedCategory;
        propertiesNeedRebuild = true;
    }

    void refreshCatalogueFilter()
    {
        const String searchText = catalogueSearch.trim();
        const auto selectedCategory = getSelectedCategory();
        const auto& entries = Box2DSamples::Catalog::getEntries();
        filteredSampleIndices.clear();

        for (int catalogueIndex = 0; catalogueIndex < (int) entries.size(); ++catalogueIndex)
        {
            const auto& entry = entries[(size_t) catalogueIndex];
            const auto catalogueLabel = getCatalogueLabel (catalogueIndex);
            const bool categoryMatches = ! selectedCategory.has_value() || *selectedCategory == entry.category;
            const bool searchMatches = searchText.isEmpty() || (catalogueLabel.has_value() && catalogueLabel->containsIgnoreCase (searchText));

            if (categoryMatches && searchMatches)
                filteredSampleIndices.push_back (catalogueIndex);
        }
    }

    void revealCatalogueIndex (int catalogueIndex)
    {
        auto iterator = std::find (filteredSampleIndices.begin(), filteredSampleIndices.end(), catalogueIndex);

        if (iterator == filteredSampleIndices.end())
        {
            catalogueSearch.clear();
            selectedCategoryIdentifier = "catalogue.allCategories";
            refreshCatalogueFilter();
        }
    }

    [[nodiscard]] bool isMetricsPanelVisible() const noexcept { return profilerVisible || inspector != nullptr; }

    std::vector<Box2DSamples::ControlChoice> getCategoryChoices() const
    {
        std::vector<Box2DSamples::ControlChoice> choices;
        choices.push_back ({ "catalogue.allCategories", TRANS ("All categories") });

        for (const auto& category : categoryKeys)
            choices.push_back ({ category, TRANS (category) });

        return choices;
    }

    std::vector<Box2DSamples::ControlChoice> getSampleChoices() const
    {
        std::vector<Box2DSamples::ControlChoice> choices;

        for (const int catalogueIndex : filteredSampleIndices)
        {
            const auto identifier = getCatalogueIdentifier (catalogueIndex);
            const auto label = getCatalogueLabel (catalogueIndex);
            jassert (identifier.has_value() && label.has_value());

            if (identifier.has_value() && label.has_value())
                choices.push_back ({ *identifier, *label });
        }

        return choices;
    }

    void updateMetricsPresentation()
    {
        metrics.setProfilerVisible (profilerVisible);
        propertiesNeedRebuild = true;

        if (hasFinishedConstruction)
            owner.resized();
    }

    void layoutMetricsContent()
    {
        auto metricsBounds = metricsContent.getLocalBounds();

        if (inspector != nullptr)
            inspector->setBounds (metricsBounds.removeFromTop (metricsBounds.getHeight() * 2 / 5));

        metrics.setBounds (metricsBounds);
    }

    void rebuildPropertiesIfNeeded()
    {
        if (! propertiesNeedRebuild
            && renderedControlStructureRevision == controlModel.getStructureRevision()
            && renderedMetricsVisible == isMetricsPanelVisible())
            return;

        auto opennessState = shouldRestorePropertyState ? propertyPanel.getOpennessState() : nullptr;
        const int scrollPosition = propertyPanel.getViewport().getViewPositionY();
        propertyPanel.clear();
        Box2DSamples::appendControlProperties (controlModel, propertyPanel);

        if (isMetricsPanelVisible())
        {
            Array<PropertyComponent*> metricsProperties;
            metricsProperties.add (new EmbeddedComponentProperty (metricsContent, inspector != nullptr ? 560 : 360));
            propertyPanel.addSection (TRANS ("Metrics"), metricsProperties, true);
        }

        if (opennessState != nullptr)
            propertyPanel.restoreOpennessState (*opennessState);

        propertyPanel.getViewport().setViewPosition (0, shouldResetPropertyScrollPosition ? 0 : scrollPosition);
        renderedControlStructureRevision = controlModel.getStructureRevision();
        renderedMetricsVisible = isMetricsPanelVisible();
        propertiesNeedRebuild = false;
        shouldResetPropertyScrollPosition = false;
        shouldRestorePropertyState = true;
        layoutMetricsContent();
    }

    [[nodiscard]] String formatCalibrationInitializer()
    {
        return formatBoundsInitializer (runtime.getContext().camera.getVisibleBounds());
    }

    void setFileStatus (String newStatus, bool shouldAnnounce = true)
    {
        fileStatus = std::move (newStatus);

        if (shouldAnnounce)
            AccessibilityHandler::postAnnouncement (*fileStatus, AccessibilityHandler::AnnouncementPriority::medium);
    }

    void destroyAuxiliaryViews()
    {
        metrics.setRuntime (nullptr);
        inspector.reset();
    }

    void rebuildAuxiliaryViews()
    {
        if (auto* sample = runtime.getCurrentSample())
        {
            inspector = sample->createInspectorComponent();

            if (inspector != nullptr)
                metricsContent.addAndMakeVisible (*inspector);
        }

        metrics.setRuntime (&runtime);
        updateMetricsPresentation();
    }

    void stopRecordingBeforeSampleChange()
    {
        if (! recordingActive)
            return;

        if (auto recording = runtime.stopRecording())
            lastRecording = std::move (*recording);

        recordingActive = false;
    }

    void selectCatalogueSample (int sampleIndex)
    {
        if (! isPositiveAndBelow (sampleIndex, (int) Box2DSamples::Catalog::getEntries().size()))
            return;

        stopRecordingBeforeSampleChange();
        destroyAuxiliaryViews();
        controlModel.clear();
        const Result selectResult = runtime.selectSample (sampleIndex);
        jassert (selectResult.wasOk());

        if (selectResult.failed())
        {
            setFileStatus (selectResult.getErrorMessage());
            rebuildAuxiliaryViews();
            return;
        }

        activeCatalogueIndex = sampleIndex;
        const auto identifier = getCatalogueIdentifier (sampleIndex);
        const auto label = getCatalogueLabel (sampleIndex);
        jassert (identifier.has_value() && label.has_value());

        if (identifier.has_value())
            selectedSampleIdentifier = *identifier;

        canvas.resetView();

        if (label.has_value())
            setFileStatus (TRANS ("Selected sample: {sampleName}.").replace ("{sampleName}", *label));

        rebuildAuxiliaryViews();
        propertiesNeedRebuild = true;
        shouldResetPropertyScrollPosition = true;
    }

    void applyPendingCatalogueSelection()
    {
        if (! pendingCatalogueIndex.has_value())
            return;

        const int sampleIndex = *pendingCatalogueIndex;
        pendingCatalogueIndex.reset();
        selectCatalogueSample (sampleIndex);
    }

    void selectReplayData (const MemoryBlock& recordingData, const String& displayName)
    {
        stopRecordingBeforeSampleChange();
        destroyAuxiliaryViews();
        controlModel.clear();
        const Result selectResult = runtime.selectReplay (recordingData, displayName);

        if (selectResult.failed())
        {
            setFileStatus (selectResult.getErrorMessage());
            rebuildAuxiliaryViews();
            return;
        }

        if (const auto replayIndex = Box2DSamples::Catalog::getReplayIndex())
        {
            activeCatalogueIndex = *replayIndex;
            revealCatalogueIndex (*replayIndex);

            if (const auto identifier = getCatalogueIdentifier (*replayIndex))
                selectedSampleIdentifier = *identifier;
        }

        canvas.resetView();
        setFileStatus (TRANS ("Loaded recording: {recordingName}.").replace ("{recordingName}", displayName));
        rebuildAuxiliaryViews();
        propertiesNeedRebuild = true;
        shouldResetPropertyScrollPosition = true;
    }

    void restartActiveSample()
    {
        destroyAuxiliaryViews();
        controlModel.clear();
        const Result restartResult = runtime.restartSample();
        jassert (restartResult.wasOk());

        if (restartResult.failed())
        {
            setFileStatus (restartResult.getErrorMessage());
        }
        else if (activeCatalogueIndex.has_value())
        {
            if (const auto label = getCatalogueLabel (*activeCatalogueIndex))
                setFileStatus (TRANS ("Restarted sample: {sampleName}.").replace ("{sampleName}", *label));
        }
        else
        {
            jassertfalse;
        }

        rebuildAuxiliaryViews();
        propertiesNeedRebuild = true;
        shouldResetPropertyScrollPosition = true;
    }

    void beginRecording()
    {
        runtime.startRecording();
        recordingActive = true;
        setFileStatus (TRANS ("Recording started."));
    }

    void finishRecording()
    {
        auto recording = runtime.stopRecording();
        recordingActive = false;

        if (! recording.has_value())
        {
            setFileStatus (TRANS ("No recording data was produced."));
            return;
        }

        lastRecording = std::move (*recording);
        setFileStatus (TRANS ("Recording stopped."));
        chooseRecordingDestination();
    }

    void chooseReplayFile()
    {
        openReplayChooser = std::make_unique<FileChooser> (TRANS ("Open a Box2D recording"),
                                                           File::getSpecialLocation (File::userDocumentsDirectory),
                                                           "*.b2rec",
                                                           true,
                                                           false,
                                                           &owner);
        openReplayChooser->launchAsync (FileBrowserComponent::openMode | FileBrowserComponent::canSelectFiles,
                                        [safeOwner = SafePointer<Box2DDemo> (&owner)] (const FileChooser& chooser)
                                        {
                                            if (safeOwner == nullptr)
                                                return;

                                            const URL url = chooser.getURLResult();

                                            if (! url.isEmpty())
                                                safeOwner->pimpl->readReplayAsync (url);
                                        });
    }

    void chooseRecordingDestination()
    {
        if (! lastRecording.has_value())
            return;

        const File initialFile = File::getSpecialLocation (File::userDocumentsDirectory)
                                     .getChildFile ("Box2D-recording.b2rec");
        saveRecordingChooser = std::make_unique<FileChooser> (TRANS ("Save the Box2D recording"),
                                                              initialFile,
                                                              "*.b2rec",
                                                              true,
                                                              false,
                                                              &owner);
        saveRecordingChooser->launchAsync (FileBrowserComponent::saveMode
                                               | FileBrowserComponent::canSelectFiles
                                               | FileBrowserComponent::warnAboutOverwriting,
                                           [safeOwner = SafePointer<Box2DDemo> (&owner)] (const FileChooser& chooser)
                                           {
                                               if (safeOwner == nullptr)
                                                   return;

                                               const URL url = chooser.getURLResult();

                                               if (! url.isEmpty())
                                                   safeOwner->pimpl->writeRecordingAsync (url);
                                           });
    }

    void readReplayAsync (URL url)
    {
        fileIOBusy = true;
        setFileStatus (TRANS ("Loading recording..."));
        fileIOThreadPool.addJob ([safeOwner = SafePointer<Box2DDemo> (&owner), urlToRead = std::move (url)]
        {
            auto result = Box2DSamples::readReplayFile (urlToRead);
            MessageManager::callAsync ([safeOwner, readResult = std::move (result)] () mutable
            {
                if (safeOwner == nullptr)
                    return;

                safeOwner->pimpl->fileIOBusy = false;
                safeOwner->pimpl->pendingReplayRead = std::move (readResult);
            });
        });
    }

    void writeRecordingAsync (URL url)
    {
        if (! lastRecording.has_value())
            return;

        fileIOBusy = true;
        setFileStatus (TRANS ("Saving recording..."));
        MemoryBlock recordingData = *lastRecording;
        fileIOThreadPool.addJob ([safeOwner = SafePointer<Box2DDemo> (&owner),
                                 urlToWrite = std::move (url),
                                 recordingDataToWrite = std::move (recordingData)]
        {
            const auto error = Box2DSamples::writeReplayFile (urlToWrite, recordingDataToWrite);
            MessageManager::callAsync ([safeOwner, error]
            {
                if (safeOwner == nullptr)
                    return;

                safeOwner->pimpl->fileIOBusy = false;

                if (! error.has_value())
                    safeOwner->pimpl->setFileStatus (TRANS ("Recording saved."));
                else
                    safeOwner->pimpl->setFileStatus (Box2DSamples::getReplayFileErrorMessage (*error));
            });
        });
    }

    void applyPendingReplayRead()
    {
        if (! pendingReplayRead.has_value())
            return;

        auto result = std::move (*pendingReplayRead);
        pendingReplayRead.reset();

        if (result.error.has_value())
        {
            setFileStatus (Box2DSamples::getReplayFileErrorMessage (*result.error));
            return;
        }

        selectReplayData (result.data, result.displayName);
    }

    void refreshControlModel()
    {
        auto& settings = runtime.getContext().settings;
        const auto* sample = runtime.getCurrentSample();
        const bool hasSample = sample != nullptr,
                   isReplayViewer = hasSample && Box2DSamples::isReplaySample (*sample);
        controlModel.beginFrame();
        controlModel.setGroup (Box2DSamples::ControlItem::Group::catalogue);
        controlModel.showTextInput ("catalogue.search", TRANS ("Search"), catalogueSearch, [this]
        {
            refreshCatalogueFilter();
        });
        controlModel.setHelpText ("catalogue.search", TRANS ("Filters the Box2D sample catalogue by category or sample name."));

        const auto categoryChoices = getCategoryChoices();
        controlModel.showChoice ("catalogue.category", TRANS ("Category"), selectedCategoryIdentifier, categoryChoices, [this]
        {
            refreshCatalogueFilter();
        });
        controlModel.setHelpText ("catalogue.category", TRANS ("Filters the Box2D sample catalogue to one category."));

        if (activeCatalogueIndex.has_value())
        {
            if (const auto identifier = getCatalogueIdentifier (*activeCatalogueIndex))
                selectedSampleIdentifier = *identifier;
        }

        const auto sampleChoices = getSampleChoices();
        controlModel.showChoice ("catalogue.sample", TRANS ("Sample"), selectedSampleIdentifier, sampleChoices, [this]
        {
            const auto catalogueIndex = getCatalogueIndex (selectedSampleIdentifier);

            if (catalogueIndex.has_value())
                pendingCatalogueIndex = *catalogueIndex;
        });
        controlModel.setHelpText ("catalogue.sample", TRANS ("Selects the Box2D sample to run."));

        controlModel.setGroup (Box2DSamples::ControlItem::Group::primaryActions);
        controlModel.showToggle ("host.pause", TRANS ("Pause"), settings.isPaused, {}, hasSample);
        controlModel.showButton ("host.step", TRANS ("Step"), [this]
        {
            ++runtime.getContext().settings.numSingleSteps;
        }, hasSample);
        controlModel.showButton ("host.restart", TRANS ("Restart"), [this]
        {
            restartActiveSample();
        }, hasSample);
        controlModel.showButton ("host.resetView", TRANS ("Reset view"), [this]
        {
            canvas.resetView();
        }, hasSample);
        controlModel.showToggle ("host.profiler", TRANS ("Profiler"), profilerVisible, [this]
        {
            updateMetricsPresentation();
        });

        controlModel.setGroup (Box2DSamples::ControlItem::Group::interactionHelp);
        std::vector<Box2DSamples::InteractionHint> interactionHints;

        if (activeCatalogueIndex.has_value() && isPositiveAndBelow (*activeCatalogueIndex, (int) Box2DSamples::Catalog::getEntries().size()))
        {
            const auto& entry = Box2DSamples::Catalog::getEntries()[(size_t) *activeCatalogueIndex];
            const auto* sampleHelp = Box2DSamples::SampleInteractionCatalog::find (entry.category, entry.name);

            if (entry.isReplayViewer)
            {
                if (sampleHelp != nullptr)
                    interactionHints.assign (sampleHelp->hints.begin(), sampleHelp->hints.end());
            }
            else
            {
                const auto genericHints = Box2DSamples::SampleInteractionCatalog::getGenericLiveHints();
                interactionHints.assign (genericHints.begin(), genericHints.end());

                if (sampleHelp != nullptr)
                {
                    for (const auto& sampleHint : sampleHelp->hints)
                    {
                        const auto iterator = std::find_if (interactionHints.begin(), interactionHints.end(), [&sampleHint] (const auto& hint)
                        {
                            return hint.identifier == sampleHint.identifier;
                        });

                        if (iterator != interactionHints.end())
                            *iterator = sampleHint;
                        else
                            interactionHints.push_back (sampleHint);
                    }
                }
            }
        }

        StringArray accessibleInteractions;

        for (const auto& hint : interactionHints)
        {
            const String inputDescription = TRANS (hint.inputDescription);
            const String actionDescription = TRANS (hint.actionDescription);
            controlModel.showInteraction ("interaction." + hint.identifier.toString(), inputDescription, actionDescription);
            accessibleInteractions.add (TRANS ("{input}: {action}")
                                            .replace ("{input}", inputDescription)
                                            .replace ("{action}", actionDescription));
        }

        std::optional<String> activeCatalogueLabel;

        if (activeCatalogueIndex.has_value())
            activeCatalogueLabel = getCatalogueLabel (*activeCatalogueIndex);

        if (hasSample && activeCatalogueLabel.has_value())
            canvas.updateAccessibility (*activeCatalogueLabel, accessibleInteractions.joinIntoString (" "));
        else
            canvas.updateAccessibility (TRANS ("Box2D sample canvas"), TRANS ("Select a sample from the catalogue."));

        controlModel.setGroup (Box2DSamples::ControlItem::Group::sampleControls);
        runtime.appendCurrentSampleControls (controlModel);

        controlModel.setGroup (Box2DSamples::ControlItem::Group::simulation);

       #if JUCE_WINDOWS || JUCE_MAC || JUCE_IOS || JUCE_LINUX || JUCE_ANDROID
        controlModel.showIntegerSlider ("simulation.workers",
                                        TRANS ("Workers"),
                                        settings.numWorkers,
                                        1,
                                        B2_MAX_WORKERS,
                                        [this] { applyWorkerCount(); },
                                        hasSample);
       #endif

        controlModel.setGroup (Box2DSamples::ControlItem::Group::solver);

        if (hasSample && sample->hasSolverControls())
        {
            controlModel.showFloatSlider ("solver.simulationHertz", TRANS ("Simulation frequency (Hz)"), settings.hertz, 5.0f, 240.0f, 0);
            controlModel.showIntegerSlider ("solver.numSubSteps", TRANS ("Sub-steps"), settings.numSubSteps, 1, 32);
            controlModel.showIntegerSlider ("solver.numRestitutionIterations",
                                            TRANS ("Restitution iterations"),
                                            settings.numRestitutionIterations,
                                            0,
                                            8);
            controlModel.showFloatSlider ("solver.contactRecycleDistance",
                                          TRANS ("Contact recycle distance (m)"),
                                          settings.recycleDistance,
                                          0.0f,
                                          0.1f,
                                          3,
                                          [this] { applyContactRecycleDistance(); });
            controlModel.showToggle ("solver.sleep", TRANS ("Sleeping"), settings.isSleepingEnabled);
            controlModel.showToggle ("solver.warmStarting", TRANS ("Warm starting"), settings.isWarmStartingEnabled);
            controlModel.showToggle ("solver.continuousCollision", TRANS ("Continuous collision"), settings.isContinuousCollisionEnabled);
            controlModel.showToggle ("solver.restitutionPropagation", TRANS ("Restitution propagation"), settings.isRestitutionPropagationEnabled);
        }

        controlModel.setGroup (Box2DSamples::ControlItem::Group::drawing);

        if (hasSample)
        {
            auto& debugDraw = runtime.getContext().debugDraw;
            controlModel.showToggle ("drawing.shapes", TRANS ("Shapes"), debugDraw.drawShapes);
            controlModel.showToggle ("drawing.chainNormals", TRANS ("Chain normals"), debugDraw.drawChainNormals);
            controlModel.showToggle ("drawing.joints", TRANS ("Joints"), debugDraw.drawJoints);
            controlModel.showToggle ("drawing.jointExtras", TRANS ("Joint extras"), debugDraw.drawJointExtras);
            controlModel.showToggle ("drawing.bounds", TRANS ("Bounds"), debugDraw.drawBounds);
            controlModel.showToggle ("drawing.mass", TRANS ("Mass"), debugDraw.drawMass);
            controlModel.showToggle ("drawing.bodyNames", TRANS ("Body names"), debugDraw.drawBodyNames);
            controlModel.showToggle ("drawing.graphColours", TRANS ("Graph colours"), debugDraw.drawGraphColors);
            controlModel.showToggle ("drawing.islands", TRANS ("Islands"), debugDraw.drawIslands);
            controlModel.showToggle ("drawing.contacts", TRANS ("Contact points"), debugDraw.drawContacts);
            controlModel.showToggle ("drawing.contactNormals", TRANS ("Contact normals"), debugDraw.drawContactNormals);
            controlModel.showToggle ("drawing.contactFeatures", TRANS ("Contact features"), debugDraw.drawContactFeatures);
            controlModel.showToggle ("drawing.contactForces", TRANS ("Contact forces"), debugDraw.drawContactForces);
            controlModel.showToggle ("drawing.frictionForces", TRANS ("Friction forces"), debugDraw.drawFrictionForces);
            controlModel.showToggle ("drawing.anchorA", TRANS ("Use contact anchor A"), debugDraw.drawAnchorA);
        }

        controlModel.setGroup (Box2DSamples::ControlItem::Group::recording);
        controlModel.showButton ("recording.record",
                                 recordingActive ? TRANS ("Stop and save recording") : TRANS ("Start recording"),
                                 [this]
                                 {
                                     if (recordingActive)
                                         finishRecording();
                                     else
                                         beginRecording();
                                 },
                                 hasSample && ! isReplayViewer && ! fileIOBusy);
        controlModel.showButton ("recording.openReplay", TRANS ("Open recording..."), [this] { chooseReplayFile(); }, ! fileIOBusy);
        controlModel.showButton ("recording.playLast", TRANS ("Play last recording"), [this]
        {
            if (lastRecording.has_value())
                selectReplayData (*lastRecording, TRANS ("Last recording"));
        }, lastRecording.has_value() && ! fileIOBusy);
        controlModel.showButton ("recording.saveLast", TRANS ("Save last recording..."), [this]
        {
            chooseRecordingDestination();
        }, lastRecording.has_value() && ! fileIOBusy);

        if (fileStatus.has_value())
            controlModel.showText ("recording.fileStatus", *fileStatus, Box2DSamples::ControlItem::TextTone::secondary);

        controlModel.setGroup (Box2DSamples::ControlItem::Group::viewCalibration);
        const auto& context = runtime.getContext();

        if (hasSample && ! isReplayViewer && context.homeView != nullptr)
        {
            const auto bounds = context.camera.getVisibleBounds();
            controlModel.showText ("viewCalibration.aspect",
                                   TRANS ("Canvas aspect: {aspectRatio}.")
                                       .replace ("{aspectRatio}", formatCalibrationValue (context.camera.getAspectRatio())));
            controlModel.showText ("viewCalibration.lowerBound",
                                   TRANS ("Lower bound: {x}, {y}.")
                                       .replace ("{x}", formatCalibrationValue (bounds.lowerBound.x))
                                       .replace ("{y}", formatCalibrationValue (bounds.lowerBound.y)));
            controlModel.showText ("viewCalibration.upperBound",
                                   TRANS ("Upper bound: {x}, {y}.")
                                       .replace ("{x}", formatCalibrationValue (bounds.upperBound.x))
                                       .replace ("{y}", formatCalibrationValue (bounds.upperBound.y)));
            controlModel.showButton ("viewCalibration.copyCurrentBounds", TRANS ("Copy current bounds"), [this]
            {
                SystemClipboard::copyTextToClipboard (formatCalibrationInitializer());
            });
        }

        controlModel.endFrame();
    }

    void applyWorkerCount()
    {
        auto& settings = runtime.getContext().settings;
        settings.numWorkers = std::clamp (settings.numWorkers, 1, (int) B2_MAX_WORKERS);

        if (const auto* sample = runtime.getCurrentSample())
        {
            const b2WorldId worldId = sample->getWorldId();

            if (b2World_IsValid (worldId))
                b2World_SetWorkerCount (worldId, settings.numWorkers);
        }
    }

    void applyContactRecycleDistance()
    {
        if (const auto* sample = runtime.getCurrentSample())
        {
            const b2WorldId worldId = sample->getWorldId();

            if (b2World_IsValid (worldId))
                b2World_SetContactRecycleDistance (worldId, runtime.getContext().settings.recycleDistance);
        }
    }

    std::optional<String> getCatalogueLabel (int catalogueIndex) const
    {
        if (! isPositiveAndBelow (catalogueIndex, (int) Box2DSamples::Catalog::getEntries().size()))
            return std::nullopt;

        const auto& entry = Box2DSamples::Catalog::getEntries()[(size_t) catalogueIndex];
        return TRANS (entry.category) + " " + String::fromUTF8 ("\xe2\x80\x94") + " " + TRANS (entry.name);
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Pimpl)
};

Box2DDemo::Box2DDemo() : pimpl (std::make_unique<Pimpl> (*this))
{
    setOpaque (true);
    setFocusContainerType (FocusContainerType::keyboardFocusContainer);
    pimpl->attachToOwner();
    setSize (960, 640);
}

Box2DDemo::~Box2DDemo() = default;

void Box2DDemo::paint (Graphics& graphics) { graphics.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId)); }

void Box2DDemo::resized()
{
    pimpl->layoutComponents (getLocalBounds());
}

void Box2DDemo::lookAndFeelChanged()
{
    pimpl->refreshTranslations();
    resized();
    repaint();
}

void Box2DDemo::updateForPresentation (double presentationTimeSeconds) { pimpl->updatePresentation (presentationTimeSeconds); }
