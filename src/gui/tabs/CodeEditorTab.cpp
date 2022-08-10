#include <gui/tabs/CodeEditorTab.hpp>

formula::gui::CodeEditorTab::CodeEditorTab(
    const std::shared_ptr<formula::events::EventHub>& eventHub,
    const std::shared_ptr<formula::processor::PluginState>& pluginState,
    const std::shared_ptr<formula::storage::LocalIndex>& localIndex
) : eventHub(eventHub), pluginState(pluginState),
    savePopup(localIndex, pluginState),
    knobsPanel(pluginState)
{
    setOpaque(true);

    editor = std::make_unique<formula::gui::FormulaCodeEditor>(codeDocument);
    addAndMakeVisible(editor.get());
    auto metadata = pluginState->getActiveFormulaMetadata();
    auto formulaSource = metadata[formula::processor::FormulaMetadataKeys::source];
    if (formulaSource.empty()) {
        editor->loadContent(defaultEditorContent);
    }
    else {
        editor->loadContent(formulaSource);
    }
    metadata[formula::processor::FormulaMetadataKeys::source] = editor->getDocument().getAllContent().toStdString();
    pluginState->setActiveFormulaMetadata(metadata);
    codeDocument.addListener(this);

    const auto green = Colour::fromRGB(0x49,0x9c,0x54);
    const auto grey = Colour::fromRGB(0xaf,0xb1,0xb3);
    const auto orange = Colour::fromRGB(0xc2,0x93,0x44);
    const auto blue = Colour::fromRGB(0x35,0x92,0xc4);

    compileButton.setImage(formula::binary::compile_svg, green);
    compileButton.setTooltip("Launch the current formula");
    compileButton.onClick = [&] {
        auto metadata = pluginState->getActiveFormulaMetadata();
        auto formulaSource = metadata[formula::processor::FormulaMetadataKeys::source];
        eventHub->publish(EventType::compilationRequest, formulaSource);
        compileButton.setEnabled(false);
    };
    addAndMakeVisible(compileButton);

    muteButton.setImage(formula::binary::mute_svg, grey);
    muteButton.setTooltip("Mute the plugin output");
    muteButton.setClickingTogglesState(true);
    muteButton.onClick = [this] {
        this->pluginState->setBypassed(this->muteButton.getToggleState());
    };
    addAndMakeVisible(muteButton);

    newButton.setImage(formula::binary::new_svg, grey);
    newButton.setTooltip("Create a new formula");
    newButton.onClick = [&] {
        auto result = AlertWindow::showYesNoCancelBox(MessageBoxIconType::WarningIcon, "New formula",
                    "All unsaved change will be lost. Do you want to create a new formula?");
        if (result != 1) return;
        formula::processor::FormulaMetadata newMetadata;
        newMetadata[formula::processor::FormulaMetadataKeys::name] = "";
        newMetadata[formula::processor::FormulaMetadataKeys::description] = "";
        newMetadata[formula::processor::FormulaMetadataKeys::source] = defaultEditorContent;
        pluginState->setActiveFormulaMetadata(newMetadata);
        editor->loadContent(defaultEditorContent);
    };
    addAndMakeVisible(newButton);

    saveLocalButton.setImage(formula::binary::save_local_svg, orange);
    saveLocalButton.setTooltip("Save formula to local storage");
    saveLocalButton.onClick = [this] {
        this->savePopup.resetContent();
        this->savePopup.setVisible(true);
        this->resized();
    };
    addAndMakeVisible(saveLocalButton);

    showDebugButton.setImage(formula::binary::show_debug_svg, grey);
    showDebugButton.setTooltip("Show the debug symbols");
    showDebugButton.setClickingTogglesState(true);
    showDebugButton.onClick = [this] {
        this->debugSymbols.setVisible(!this->debugSymbols.isVisible());
        this->resized();
    };
    addAndMakeVisible(showDebugButton);

    showKnobsButton.setImage(formula::binary::show_knobs_svg, blue);
    showKnobsButton.setTooltip("Show the knobs panel");
    showKnobsButton.setClickingTogglesState(true);
    showKnobsButton.onClick = [this] {
        this->knobsPanel.setVisible(!this->knobsPanel.isVisible());
        this->resized();
    };
    addAndMakeVisible(showKnobsButton);

    zoomInButton.setImage(formula::binary::zoom_in_svg, grey);
    zoomInButton.setTooltip("Zoom in");
    zoomInButton.onClick = [this] {
        this->eventHub->publish(EventType::scaleUp);
    };
    addAndMakeVisible(zoomInButton);

    zoomOutButton.setImage(formula::binary::zoom_out_svg, grey);
    zoomOutButton.setTooltip("Zoom out");
    zoomOutButton.onClick = [this] {
        this->eventHub->publish(EventType::scaleDown);
    };
    addAndMakeVisible(zoomOutButton);

    compilerErrors.setReadOnly(true);
    compilerErrors.setMultiLine(true);
    addChildComponent(compilerErrors);

    debugSymbols.setReadOnly(true);
    debugSymbols.setMultiLine(true);
    addChildComponent(debugSymbols);

    addChildComponent(knobsPanel);

    addChildComponent(savePopup);

    eventHub->subscribeOnUiThread<CodeEditorTab>(
            EventType::compilationSuccess,[] ([[maybe_unused]] boost::any _, CodeEditorTab* thisPtr) {
        thisPtr->compilerErrors.setText("");
        thisPtr->compilerErrors.setVisible(false);
        thisPtr->compileButton.setEnabled(true);
        thisPtr->resized();
    }, this);

    eventHub->subscribeOnUiThread<CodeEditorTab>(
            EventType::compilationFail, [](boost::any errorText, CodeEditorTab* thisPtr) {
        auto errorStr = boost::any_cast<std::string>(errorText);
        thisPtr->compilerErrors.setVisible(true);
        thisPtr->compilerErrors.setText(boost::any_cast<std::string>(errorStr));
        thisPtr->compileButton.setEnabled(true);
        thisPtr->resized();
    }, this);

    eventHub->subscribeOnUiThread<CodeEditorTab>(
            EventType::loadFormulaRequest, [](boost::any metadata, CodeEditorTab* thisPtr) {
        auto formulaMetadata = boost::any_cast<formula::processor::FormulaMetadata>(metadata);
        auto formulaSource = formulaMetadata[formula::processor::FormulaMetadataKeys::source];
        thisPtr->editor->loadContent(formulaSource);
        thisPtr->knobsPanel.restoreFromState(formulaMetadata);
        thisPtr->pluginState->setActiveFormulaMetadata(formulaMetadata);
        thisPtr->resized();
        thisPtr->eventHub->publish(EventType::compilationRequest, formulaSource);
    }, this);

    startTimer(100);
}

formula::gui::CodeEditorTab::~CodeEditorTab() {
    eventHub->unsubscribe(this);
}

void formula::gui::CodeEditorTab::paint(Graphics& g)
{
    const auto backgroundColour = Colour::fromRGB(0x3c, 0x3f, 0x41);

    g.fillAll(backgroundColour);
}

void formula::gui::CodeEditorTab::codeDocumentTextInserted(const String& newText, int insertIndex)
{
    if (newText.endsWithChar('\n')) {
        const auto autoTab = findAutoTabulation();
        codeDocument.insertText(editor->getCaretPos(), autoTab);
    }
    auto source = codeDocument.getAllContent();
    pluginState->setActiveFormulaMetadataField(formula::processor::FormulaMetadataKeys::source, source.toStdString());
}

void formula::gui::CodeEditorTab::codeDocumentTextDeleted(int startIndex, int endIndex)
{
    auto source = codeDocument.getAllContent();
    pluginState->setActiveFormulaMetadataField(formula::processor::FormulaMetadataKeys::source, source.toStdString());
}

void formula::gui::CodeEditorTab::resized()
{
    constexpr auto buttonSizePixels = 24;
    constexpr auto toolbarSizePixels = 32;
    constexpr auto buttonMarginSidesPixels = (toolbarSizePixels - buttonSizePixels) / 2;
    constexpr auto buttonMarginBottomPixels = 15;
    constexpr auto compilerErrorsHeightPixels = 140;
    constexpr auto debugSymbolsHeightPixels = 200;
    constexpr auto knobsPanelHeightPixels = 325;

    auto area = getLocalBounds();

    const auto editorCenter = area.getCentre();
    savePopup.setBounds(savePopup.getAreaToFit(editorCenter));

    auto toolbarArea = area.removeFromLeft(toolbarSizePixels);
    toolbarArea.removeFromTop(buttonMarginBottomPixels);

    newButton.setBounds(
            toolbarArea.removeFromTop(buttonSizePixels)
            .withTrimmedLeft(buttonMarginSidesPixels)
            .withTrimmedRight(buttonMarginSidesPixels));
    toolbarArea.removeFromTop(buttonMarginBottomPixels);

    compileButton.setBounds(
            toolbarArea.removeFromTop(buttonSizePixels)
            .withTrimmedLeft(buttonMarginSidesPixels)
            .withTrimmedRight(buttonMarginSidesPixels));
    toolbarArea.removeFromTop(buttonMarginBottomPixels);

    muteButton.setBounds(toolbarArea.removeFromTop(buttonSizePixels)
                                 .withTrimmedLeft(buttonMarginSidesPixels)
                                 .withTrimmedRight(buttonMarginSidesPixels));
    toolbarArea.removeFromTop(buttonMarginBottomPixels);

    saveLocalButton.setBounds(toolbarArea.removeFromTop(buttonSizePixels)
                                      .withTrimmedLeft(buttonMarginSidesPixels)
                                      .withTrimmedRight(buttonMarginSidesPixels));
    toolbarArea.removeFromTop(buttonMarginBottomPixels);

    zoomInButton.setBounds(toolbarArea.removeFromTop(buttonSizePixels)
                                   .withTrimmedLeft(buttonMarginSidesPixels)
                                   .withTrimmedRight(buttonMarginSidesPixels));
    toolbarArea.removeFromTop(buttonMarginBottomPixels);

    zoomOutButton.setBounds(toolbarArea.removeFromTop(buttonSizePixels)
                                    .withTrimmedLeft(buttonMarginSidesPixels)
                                    .withTrimmedRight(buttonMarginSidesPixels));
    toolbarArea.removeFromTop(buttonMarginBottomPixels);

    showDebugButton.setBounds(toolbarArea.removeFromTop(buttonSizePixels)
                                      .withTrimmedLeft(buttonMarginSidesPixels)
                                      .withTrimmedRight(buttonMarginSidesPixels));
    toolbarArea.removeFromTop(buttonMarginBottomPixels);

    showKnobsButton.setBounds(toolbarArea.removeFromTop(buttonSizePixels)
                                      .withTrimmedLeft(buttonMarginSidesPixels)
                                      .withTrimmedRight(buttonMarginSidesPixels));
    toolbarArea.removeFromTop(buttonMarginBottomPixels);

    if (knobsPanel.isVisible()) {
        knobsPanel.setBounds(area.removeFromBottom(knobsPanelHeightPixels));
    }

    if (compilerErrors.isVisible()) {
        compilerErrors.setBounds(area.removeFromBottom(compilerErrorsHeightPixels));
    }

    if (debugSymbols.isVisible()) {
        debugSymbols.setBounds(area.removeFromBottom(debugSymbolsHeightPixels));
    }

    editor->setBounds(area);

}

void formula::gui::CodeEditorTab::timerCallback() {
    debugSymbols.setText(pluginState->getDebugString());
}

juce::String formula::gui::CodeEditorTab::findAutoTabulation() {
    auto previousLineBreakPos = editor->getCaretPos();
    previousLineBreakPos.setLineAndIndex(previousLineBreakPos.getLineNumber() - 1, 0);

    auto whiteSpaceStartPos = juce::CodeDocument::Position(previousLineBreakPos);
    auto whiteSpaceEndPos = juce::CodeDocument::Position(previousLineBreakPos);

    auto curChar = whiteSpaceEndPos.getCharacter();
    while (curChar == '\t' || curChar == ' ') {
        whiteSpaceEndPos.moveBy(1);
        curChar = whiteSpaceEndPos.getCharacter();
    }
    return codeDocument.getTextBetween(whiteSpaceStartPos, whiteSpaceEndPos);;
}
