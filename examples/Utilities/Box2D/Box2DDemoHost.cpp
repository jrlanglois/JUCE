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

class CallbackTextProperty final : public TextPropertyComponent
{
public:
    CallbackTextProperty (const String& propertyName, std::function<String()> getterIn, std::function<void (const String&)> setterIn) :
        TextPropertyComponent (propertyName, 0, false),
        getter (std::move (getterIn)),
        setter (std::move (setterIn))
    {
    }

    void setText (const String& newText) override { setter (newText); }

    String getText() const override { return getter(); }

private:
    std::function<String()> getter;
    std::function<void (const String&)> setter;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CallbackTextProperty)
};

class CallbackChoiceProperty final : public ChoicePropertyComponent
{
public:
    CallbackChoiceProperty (const String& propertyName,
                            StringArray choicesIn,
                            std::function<int()> getterIn,
                            std::function<void (int)> setterIn) :
        ChoicePropertyComponent (propertyName),
        getter (std::move (getterIn)),
        setter (std::move (setterIn))
    {
        choices = std::move (choicesIn);
    }

    void setIndex (int newIndex) override { setter (newIndex); }

    int getIndex() const override { return getter(); }

    void refresh() override
    {
        if (choiceControl != nullptr && hasChoiceSnapshot)
        {
            const int modelIndex = getIndex();
            const int controlIndex = choiceControl->getSelectedItemIndex();
            const bool modelChanged = modelIndex != previousModelIndex;
            const bool controlChanged = controlIndex != previousControlIndex;

            if (controlChanged && ! modelChanged)
                setIndex (controlIndex);
        }

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

        captureChoiceSnapshot();
    }

    void setChoices (StringArray newChoices)
    {
        choices = std::move (newChoices);

        if (choiceControl != nullptr)
        {
            choiceControl->clear (dontSendNotification);
            choiceControl->addItemList (choices, 1);
            choiceControl->setSelectedItemIndex (getIndex(), dontSendNotification);
            captureChoiceSnapshot();
        }
    }

private:
    std::function<int()> getter;
    std::function<void (int)> setter;
    ComboBox* choiceControl = nullptr;
    int previousModelIndex = -1,
        previousControlIndex = -1;
    bool hasChoiceSnapshot = false;

    void captureChoiceSnapshot()
    {
        if (choiceControl == nullptr)
            return;

        previousModelIndex = getIndex();
        previousControlIndex = choiceControl->getSelectedItemIndex();
        hasChoiceSnapshot = true;
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CallbackChoiceProperty)
};

class EmbeddedComponentProperty final : public PropertyComponent
{
public:
    EmbeddedComponentProperty (Component& componentIn, int preferredHeight) :
        PropertyComponent ({}, preferredHeight),
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
        canvasInputHelp.setComponentID ("box2dCanvasInputHelp");
        canvasInputHelp.setAccessible (false);
        canvasInputHelp.setInterceptsMouseClicks (false, false);
        canvasInputHelp.setJustificationType (Justification::centredLeft);
        canvasInputHelp.setMinimumHorizontalScale (0.65f);
        metricsContent.addAndMakeVisible (metrics);
        refreshTranslations();

        if (! filteredSampleIndices.empty())
            pendingCatalogueIndex = filteredSampleIndices.front();

        refreshGlobalControls();
        rebuildPropertiesIfNeeded();
        hasFinishedConstruction = true;
    }

    ~Pimpl() { destroyAuxiliaryViews(); }

    void attachToOwner()
    {
        owner.addAndMakeVisible (propertyPanel);
        owner.addAndMakeVisible (canvasInputHelp);
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

        applyPendingCatalogueSelection();
        applyPendingReplayRead();
        sampleControlPanel.applyQueuedEdits();
        globalControlPanel.applyQueuedEdits();
        canvas.applyPendingInput();
        runtime.updateForPresentation (presentationTimeSeconds);
        runtime.updateControls (sampleControlPanel);
        refreshGlobalControls();
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
        canvasInputHelp.setBounds (bounds.removeFromTop (24));
        canvas.setBounds (bounds);
        layoutMetricsContent();
    }

    void refreshTranslations()
    {
        activeLanguageSignature = getCurrentLanguageSignature();
        const String selectedCategory = getSelectedCategory();
        propertyPanel.setTitle (TRANS ("Box2D controls"));
        propertyPanel.setDescription (TRANS ("Selects a Box2D sample and configures its simulation and presentation."));
        canvasInputHelp.setText (TRANS ("Mouse wheel zooms. Middle-drag pans. Primary-drag interacts. Home resets the view."), dontSendNotification);
        rebuildCategoryChoices (selectedCategory);
        refreshCatalogueFilter();
        updateCanvasAccessibility();
        runtime.updateControls (sampleControlPanel);
        refreshGlobalControls();
        metrics.refresh();
        propertiesNeedRebuild = true;

        for (auto* component : { static_cast<Component*> (&propertyPanel),
                                 static_cast<Component*> (&canvas),
                                 static_cast<Component*> (&metrics) })
            component->invalidateAccessibilityHandler();

        rebuildPropertiesIfNeeded();
    }

    Box2DDemo& owner;
    Box2DSamples::Runtime runtime;
    Box2DSamples::Canvas canvas;
    Box2DSamples::ControlPanel globalControlPanel,
                               sampleControlPanel;
    Box2DSamples::MetricsComponent metrics;
    Component metricsContent;
    PropertyPanel propertyPanel;
    CallbackChoiceProperty* catalogueSampleProperty = nullptr;
    Label canvasInputHelp;
    std::unique_ptr<Component> inspector;
    std::unique_ptr<FileChooser> openReplayChooser,
                                 saveRecordingChooser;
    ThreadPool fileIOThreadPool { 1 };
    std::optional<int> pendingCatalogueIndex;
    std::optional<Box2DSamples::ReplayFileReadResult> pendingReplayRead;
    std::vector<int> filteredSampleIndices;
    StringArray categoryKeys;
    MemoryBlock lastRecording;
    String activeLanguageSignature,
           catalogueSearch,
           fileStatus;
    uint64 renderedGlobalControlRevision = 0,
           renderedSampleControlRevision = 0;
    int activeCatalogueIndex = -1,
        selectedCategoryIndex = 0;
    bool recordingActive = false,
         fileIOBusy = false,
         hasFinishedConstruction = false,
         propertiesNeedRebuild = true,
         renderedProfilerVisible = false,
         profilerVisible = false;
    VBlankAttachment vblankAttachment;

private:
    static String getCurrentLanguageSignature()
    {
        if (const auto* mappings = LocalisedStrings::getCurrentMappings())
            return mappings->getLanguageName() + "\n" + mappings->getCountryCodes().joinIntoString (" ") + "\n" + String (mappings->getMappings().size());

        return {};
    }

    int getCatalogueIndexForRow (int row) const
    {
        return isPositiveAndBelow (row, (int) filteredSampleIndices.size()) ? filteredSampleIndices[(size_t) row] : -1;
    }

    String getSelectedCategory() const
    {
        return isPositiveAndBelow (selectedCategoryIndex - 1, categoryKeys.size()) ? categoryKeys[selectedCategoryIndex - 1] : String();
    }

    void rebuildCategoryChoices (const String& selectedCategory)
    {
        categoryKeys.clear();

        for (const auto& entry : Box2DSamples::Catalog::getEntries())
        {
            const String category = String::fromUTF8 (entry.category);

            if (! categoryKeys.contains (category))
                categoryKeys.add (category);
        }

        const int selectedIndex = categoryKeys.indexOf (selectedCategory);
        selectedCategoryIndex = selectedIndex >= 0 ? selectedIndex + 1 : 0;
        propertiesNeedRebuild = true;
    }

    void refreshCatalogueFilter()
    {
        const String searchText = catalogueSearch.trim();
        const String selectedCategory = getSelectedCategory();
        const auto& entries = Box2DSamples::Catalog::getEntries();
        filteredSampleIndices.clear();

        for (int catalogueIndex = 0; catalogueIndex < (int) entries.size(); ++catalogueIndex)
        {
            const auto& entry = entries[(size_t) catalogueIndex];
            const bool categoryMatches = selectedCategory.isEmpty() || selectedCategory == entry.category;
            const bool searchMatches = searchText.isEmpty() || getCatalogueLabel (catalogueIndex).containsIgnoreCase (searchText);

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
            selectedCategoryIndex = 0;
            refreshCatalogueFilter();

            if (catalogueSampleProperty != nullptr)
                catalogueSampleProperty->setChoices (getSampleChoices());
        }
    }

    [[nodiscard]] bool isMetricsPanelVisible() const noexcept { return profilerVisible || inspector != nullptr; }

    int getSelectedFilteredSampleIndex() const
    {
        const auto iterator = std::find (filteredSampleIndices.begin(), filteredSampleIndices.end(), activeCatalogueIndex);
        return iterator != filteredSampleIndices.end() ? (int) std::distance (filteredSampleIndices.begin(), iterator) : -1;
    }

    StringArray getCategoryChoices() const
    {
        StringArray choices { TRANS ("All categories") };

        for (const auto& category : categoryKeys)
            choices.add (TRANS (category));

        return choices;
    }

    StringArray getSampleChoices() const
    {
        StringArray choices;

        for (const int catalogueIndex : filteredSampleIndices)
            choices.add (getCatalogueLabel (catalogueIndex));

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
            && renderedGlobalControlRevision == globalControlPanel.getRevision()
            && renderedSampleControlRevision == sampleControlPanel.getRevision()
            && renderedProfilerVisible == isMetricsPanelVisible())
            return;

        auto opennessState = propertyPanel.getOpennessState();
        const int scrollPosition = propertyPanel.getViewport().getViewPositionY();
        catalogueSampleProperty = nullptr;
        propertyPanel.clear();

        Array<PropertyComponent*> catalogueProperties;
        auto* searchProperty = new CallbackTextProperty (TRANS ("Search"),
                                                         [this] { return catalogueSearch; },
                                                         [this] (const String& newSearch)
                                                         {
                                                             catalogueSearch = newSearch;
                                                             refreshCatalogueFilter();

                                                             if (catalogueSampleProperty != nullptr)
                                                                 catalogueSampleProperty->setChoices (getSampleChoices());
                                                         });
        searchProperty->setDescription (TRANS ("Filters the Box2D sample catalogue by category or sample name."));
        catalogueProperties.add (searchProperty);
        auto* categoryProperty = new CallbackChoiceProperty (TRANS ("Category"),
                                                             getCategoryChoices(),
                                                             [this] { return selectedCategoryIndex; },
                                                             [this] (int newIndex)
                                                             {
                                                                 selectedCategoryIndex = std::clamp (newIndex, 0, categoryKeys.size());
                                                                 refreshCatalogueFilter();

                                                                 if (catalogueSampleProperty != nullptr)
                                                                     catalogueSampleProperty->setChoices (getSampleChoices());
                                                             });
        categoryProperty->setDescription (TRANS ("Filters the Box2D sample catalogue to one category."));
        catalogueProperties.add (categoryProperty);
        catalogueSampleProperty = new CallbackChoiceProperty (TRANS ("Sample"),
                                                              getSampleChoices(),
                                                              [this] { return getSelectedFilteredSampleIndex(); },
                                                              [this] (int newIndex)
                                                              {
                                                                  const int catalogueIndex = getCatalogueIndexForRow (newIndex);

                                                                  if (catalogueIndex >= 0)
                                                                      pendingCatalogueIndex = catalogueIndex;
                                                              });
        catalogueSampleProperty->setDescription (TRANS ("Selects the Box2D sample to run."));
        catalogueProperties.add (catalogueSampleProperty);
        propertyPanel.addSection (TRANS ("Catalogue"), catalogueProperties);
        globalControlPanel.appendPropertiesTo (propertyPanel, TRANS ("Simulation"));
        sampleControlPanel.appendPropertiesTo (propertyPanel, TRANS ("Sample"));

        if (isMetricsPanelVisible())
        {
            Array<PropertyComponent*> metricsProperties;
            metricsProperties.add (new EmbeddedComponentProperty (metricsContent, inspector != nullptr ? 560 : 360));
            propertyPanel.addSection (TRANS ("Metrics"), metricsProperties);
        }

        if (opennessState != nullptr)
            propertyPanel.restoreOpennessState (*opennessState);

        propertyPanel.getViewport().setViewPosition (0, scrollPosition);
        renderedGlobalControlRevision = globalControlPanel.getRevision();
        renderedSampleControlRevision = sampleControlPanel.getRevision();
        renderedProfilerVisible = isMetricsPanelVisible();
        propertiesNeedRebuild = false;
        layoutMetricsContent();
    }

    void updateCanvasAccessibility()
    {
        if (! isPositiveAndBelow (activeCatalogueIndex, (int) Box2DSamples::Catalog::getEntries().size()))
        {
            canvas.updateAccessibility (TRANS ("Box2D sample canvas"), TRANS ("Select a sample from the catalogue."));
            return;
        }

        const auto& entry = Box2DSamples::Catalog::getEntries()[(size_t) activeCatalogueIndex];
        const String instructions = entry.isReplayViewer
                                        ? TRANS ("Mouse wheel zooms. Middle-drag pans. Home resets the view. Inspect the recorded world or use the replay transport and timeline.")
                                        : TRANS ("Mouse wheel zooms. Middle-drag pans. Primary-drag interacts. Home resets the view.");
        canvas.updateAccessibility (getCatalogueLabel (activeCatalogueIndex), instructions);
    }

    [[nodiscard]] String formatCalibrationInitializer()
    {
        return formatBoundsInitializer (runtime.getContext().camera.getVisibleBounds());
    }

    void setFileStatus (String newStatus, bool shouldAnnounce = true)
    {
        fileStatus = std::move (newStatus);

        if (shouldAnnounce && fileStatus.isNotEmpty())
            AccessibilityHandler::postAnnouncement (fileStatus, AccessibilityHandler::AnnouncementPriority::medium);
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
        sampleControlPanel.clear();
        const Result selectResult = runtime.selectSample (sampleIndex);
        jassert (selectResult.wasOk());

        if (selectResult.failed())
        {
            setFileStatus (selectResult.getErrorMessage());
            rebuildAuxiliaryViews();
            return;
        }

        activeCatalogueIndex = sampleIndex;
        canvas.resetView();
        updateCanvasAccessibility();
        setFileStatus (TRANS ("Selected sample: {sampleName}.").replace ("{sampleName}", getCatalogueLabel (sampleIndex)));
        rebuildAuxiliaryViews();
        refreshGlobalControls();
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
        sampleControlPanel.clear();
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
            updateCanvasAccessibility();
        }

        canvas.resetView();
        setFileStatus (TRANS ("Loaded recording: {recordingName}.").replace ("{recordingName}", displayName));
        rebuildAuxiliaryViews();
        refreshGlobalControls();
    }

    void restartActiveSample()
    {
        destroyAuxiliaryViews();
        sampleControlPanel.clear();
        const Result restartResult = runtime.restartSample();
        jassert (restartResult.wasOk());

        if (restartResult.failed())
            setFileStatus (restartResult.getErrorMessage());
        else
            setFileStatus (TRANS ("Restarted sample: {sampleName}.").replace ("{sampleName}", getCatalogueLabel (activeCatalogueIndex)));

        rebuildAuxiliaryViews();
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
        if (lastRecording.isEmpty())
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
        fileIOBusy = true;
        setFileStatus (TRANS ("Saving recording..."));
        MemoryBlock recordingData = lastRecording;
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

                if (error == Box2DSamples::ReplayFileError::none)
                    safeOwner->pimpl->setFileStatus (TRANS ("Recording saved."));
                else
                    safeOwner->pimpl->setFileStatus (Box2DSamples::getReplayFileErrorMessage (error));
            });
        });
    }

    void applyPendingReplayRead()
    {
        if (! pendingReplayRead.has_value())
            return;

        auto result = std::move (*pendingReplayRead);
        pendingReplayRead.reset();

        if (result.error != Box2DSamples::ReplayFileError::none)
        {
            setFileStatus (Box2DSamples::getReplayFileErrorMessage (result.error));
            return;
        }

        selectReplayData (result.data, result.displayName);
    }

    void refreshGlobalControls()
    {
        auto& settings = runtime.getContext().settings;
        const auto* sample = runtime.getCurrentSample();
        const bool hasSample = sample != nullptr,
                   isReplayViewer = hasSample && Box2DSamples::isReplaySample (*sample);
        globalControlPanel.beginFrame();
        globalControlPanel.showToggle ("pause", TRANS ("Pause"), settings.isPaused, {}, hasSample);
        globalControlPanel.showButton ("step", TRANS ("Step"), [this]
        {
            ++runtime.getContext().settings.numSingleSteps;
        }, hasSample);
        globalControlPanel.showButton ("restart", TRANS ("Restart"), [this]
        {
            restartActiveSample();
        }, hasSample);
        globalControlPanel.showButton ("resetView", TRANS ("Reset view"), [this]
        {
            canvas.resetView();
        }, hasSample);
        globalControlPanel.showToggle ("profiler", TRANS ("Profiler"), profilerVisible, [this]
        {
            updateMetricsPresentation();
        });
        globalControlPanel.showButton ("record",
                                       recordingActive ? TRANS ("Stop and save recording") : TRANS ("Start recording"),
                                       [this]
                                       {
                                           if (recordingActive)
                                               finishRecording();
                                           else
                                               beginRecording();
                                       },
                                       hasSample && ! isReplayViewer && ! fileIOBusy);
        globalControlPanel.showButton ("openReplay",
                                       TRANS ("Open recording..."),
                                       [this] { chooseReplayFile(); },
                                       ! fileIOBusy);
        globalControlPanel.showButton ("playLastRecording",
                                       TRANS ("Play last recording"),
                                       [this] { selectReplayData (lastRecording, TRANS ("Last recording")); },
                                       ! lastRecording.isEmpty() && ! fileIOBusy);
        globalControlPanel.showButton ("saveLastRecording",
                                       TRANS ("Save last recording..."),
                                       [this] { chooseRecordingDestination(); },
                                       ! lastRecording.isEmpty() && ! fileIOBusy);

       #if JUCE_WINDOWS || JUCE_MAC || JUCE_IOS || JUCE_LINUX || JUCE_ANDROID
        globalControlPanel.showIntegerSlider ("workerCount",
                                              TRANS ("Workers"),
                                              settings.numWorkers,
                                              1,
                                              B2_MAX_WORKERS,
                                              [this] { applyWorkerCount(); },
                                              hasSample);
       #endif

        if (hasSample && sample->hasSolverControls())
        {
            globalControlPanel.showText ("solverHeading", TRANS ("Solver"), Box2DSamples::ControlPanel::TextTone::heading);
            globalControlPanel.showFloatSlider ("simulationHertz", TRANS ("Simulation frequency (Hz)"), settings.hertz, 5.0f, 240.0f, 0);
            globalControlPanel.showIntegerSlider ("numSubSteps", TRANS ("Sub-steps"), settings.numSubSteps, 1, 32);
            globalControlPanel.showIntegerSlider ("numRestitutionIterations",
                                                  TRANS ("Restitution iterations"),
                                                  settings.numRestitutionIterations,
                                                  0,
                                                  8);
            globalControlPanel.showFloatSlider ("contactRecycleDistance",
                                                TRANS ("Contact recycle distance (m)"),
                                                settings.recycleDistance,
                                                0.0f,
                                                0.1f,
                                                3,
                                                [this] { applyContactRecycleDistance(); });
            globalControlPanel.showToggle ("sleep", TRANS ("Sleeping"), settings.isSleepingEnabled);
            globalControlPanel.showToggle ("warmStarting", TRANS ("Warm starting"), settings.isWarmStartingEnabled);
            globalControlPanel.showToggle ("continuousCollision",
                                           TRANS ("Continuous collision"),
                                           settings.isContinuousCollisionEnabled);
            globalControlPanel.showToggle ("restitutionPropagation",
                                           TRANS ("Restitution propagation"),
                                           settings.isRestitutionPropagationEnabled);
        }

        if (hasSample)
        {
            auto& debugDraw = runtime.getContext().debugDraw;
            globalControlPanel.showText ("drawingHeading", TRANS ("Drawing"), Box2DSamples::ControlPanel::TextTone::heading);
            globalControlPanel.showToggle ("drawShapes", TRANS ("Shapes"), debugDraw.drawShapes);
            globalControlPanel.showToggle ("drawChainNormals", TRANS ("Chain normals"), debugDraw.drawChainNormals);
            globalControlPanel.showToggle ("drawJoints", TRANS ("Joints"), debugDraw.drawJoints);
            globalControlPanel.showToggle ("drawJointExtras", TRANS ("Joint extras"), debugDraw.drawJointExtras);
            globalControlPanel.showToggle ("drawBounds", TRANS ("Bounds"), debugDraw.drawBounds);
            globalControlPanel.showToggle ("drawMass", TRANS ("Mass"), debugDraw.drawMass);
            globalControlPanel.showToggle ("drawBodyNames", TRANS ("Body names"), debugDraw.drawBodyNames);
            globalControlPanel.showToggle ("drawGraphColours", TRANS ("Graph colours"), debugDraw.drawGraphColors);
            globalControlPanel.showToggle ("drawIslands", TRANS ("Islands"), debugDraw.drawIslands);
            globalControlPanel.showToggle ("drawContacts", TRANS ("Contact points"), debugDraw.drawContacts);
            globalControlPanel.showToggle ("drawContactNormals", TRANS ("Contact normals"), debugDraw.drawContactNormals);
            globalControlPanel.showToggle ("drawContactFeatures", TRANS ("Contact features"), debugDraw.drawContactFeatures);
            globalControlPanel.showToggle ("drawContactForces", TRANS ("Contact forces"), debugDraw.drawContactForces);
            globalControlPanel.showToggle ("drawFrictionForces", TRANS ("Friction forces"), debugDraw.drawFrictionForces);
            globalControlPanel.showToggle ("drawAnchorA", TRANS ("Use contact anchor A"), debugDraw.drawAnchorA);
        }

        const auto& context = runtime.getContext();

        if (hasSample && ! isReplayViewer && context.homeView != nullptr)
        {
            const auto bounds = context.camera.getVisibleBounds();
            globalControlPanel.showText ("viewCalibrationHeading", TRANS ("View calibration"), Box2DSamples::ControlPanel::TextTone::heading);
            globalControlPanel.showText ("viewCalibrationAspect",
                                         TRANS ("Canvas aspect: {aspectRatio}.")
                                             .replace ("{aspectRatio}", formatCalibrationValue (context.camera.getAspectRatio())));
            globalControlPanel.showText ("viewCalibrationLowerBound",
                                         TRANS ("Lower bound: {x}, {y}.")
                                             .replace ("{x}", formatCalibrationValue (bounds.lowerBound.x))
                                             .replace ("{y}", formatCalibrationValue (bounds.lowerBound.y)));
            globalControlPanel.showText ("viewCalibrationUpperBound",
                                         TRANS ("Upper bound: {x}, {y}.")
                                             .replace ("{x}", formatCalibrationValue (bounds.upperBound.x))
                                             .replace ("{y}", formatCalibrationValue (bounds.upperBound.y)));
            globalControlPanel.showButton ("copyCurrentBounds", TRANS ("Copy current bounds"), [this]
            {
                SystemClipboard::copyTextToClipboard (formatCalibrationInitializer());
            });
        }

        if (fileStatus.isNotEmpty())
            globalControlPanel.showText ("fileStatus", fileStatus, Box2DSamples::ControlPanel::TextTone::secondary);

        globalControlPanel.endFrame();
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

    String getCatalogueLabel (int catalogueIndex) const
    {
        if (! isPositiveAndBelow (catalogueIndex, (int) Box2DSamples::Catalog::getEntries().size()))
            return {};

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
