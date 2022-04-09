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

    editor.reset(new CodeEditorComponent(codeDocument, &cppTokeniser));
    addAndMakeVisible(editor.get());
    auto metadata = pluginState->getActiveFormulaMetadata();
    auto formulaSource = metadata[formula::processor::FormulaMetadataKeys::source];
    if (formulaSource.empty()) {
        editor->loadContent(R"""(
formula_main {
    float output = input;
    return output;
}
)""");
    }
    else {
        editor->loadContent(formulaSource);
    }
    metadata[formula::processor::FormulaMetadataKeys::source] = editor->getDocument().getAllContent().toStdString();
    pluginState->setActiveFormulaMetadata(metadata);
    setCodeEditorComponentColourScheme();
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
            EventType::compilationSuccess,[] (boost::any _, CodeEditorTab* editor) {
         editor->compilerErrors.setText("");
         editor->compilerErrors.setVisible(false);
         editor->compileButton.setEnabled(true);
         editor->resized();
    }, this);

    eventHub->subscribeOnUiThread<CodeEditorTab>(
            EventType::compilationFail, [](boost::any errorText, CodeEditorTab* editor) {
        auto errorStr = boost::any_cast<std::string>(errorText);
        editor->compilerErrors.setVisible(true);
        editor->compilerErrors.setText(boost::any_cast<std::string>(errorStr));
        editor->compileButton.setEnabled(true);
        editor->resized();
    }, this);

    eventHub->subscribeOnUiThread<CodeEditorTab>(
            EventType::loadFormulaRequest, [](boost::any metadata, CodeEditorTab* editor) {
        auto formulaMetadata = boost::any_cast<formula::processor::FormulaMetadata>(metadata);
        editor->editor->loadContent(formulaMetadata[formula::processor::FormulaMetadataKeys::source]);
        editor->knobsPanel.restoreFromState(formulaMetadata);
        editor->pluginState->setActiveFormulaMetadata(formulaMetadata);
        editor->resized();
    }, this);

    startTimer(500);
}

void formula::gui::CodeEditorTab::paint(Graphics& g)
{
    const auto backgroundColour = Colour::fromRGB(0x3c, 0x3f, 0x41);

    g.fillAll(backgroundColour);
}

void formula::gui::CodeEditorTab::codeDocumentTextInserted(const String& newText, int insertIndex)
{
    /* -- Leads to insert/delete de-synchronisations
    auto activeFormula = pluginState->getPropertyAsString(formula::processor::FormulaMetadataKeys::source);
    activeFormula.insert(insertIndex, newText.toStdString());
    pluginState->setActiveFormulaMetadataField(formula::processor::FormulaMetadataKeys::source, activeFormula);
    */
    auto source = codeDocument.getAllContent();
    pluginState->setActiveFormulaMetadataField(formula::processor::FormulaMetadataKeys::source, source.toStdString());
}

void formula::gui::CodeEditorTab::codeDocumentTextDeleted(int startIndex, int endIndex)
{
    /* -- Leads to insert/delete de-synchronisations
    auto activeFormula = pluginState->getPropertyAsString(formula::processor::FormulaMetadataKeys::source);
    activeFormula.erase(startIndex, static_cast<size_t>(endIndex) - startIndex);
    pluginState->setActiveFormulaMetadataField(formula::processor::FormulaMetadataKeys::source, activeFormula);
    */
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
    savePopup.setBounds(Rectangle<int>(
        editorCenter.getX() - 200,
        editorCenter.getY() - 175,
        400,
        350
        ));

    auto toolbarArea = area.removeFromLeft(toolbarSizePixels);
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

void formula::gui::CodeEditorTab::setCodeEditorComponentColourScheme()
{
    struct Type
    {
        const char* name;
        juce::uint32 colour;
    };

    const Type types[] =
    {
        { "Error",              0xffe60000 },
        { "Comment",            0xff72d20c },
        { "Keyword",            0xffee6f6f },
        { "Operator",           0xffc4eb19 },
        { "Identifier",         0xffcfcfcf },
        { "Integer",            0xff42c8c4 },
        { "Float",              0xff885500 },
        { "String",             0xffbc45dd },
        { "Bracket",            0xff058202 },
        { "Punctuation",        0xffcfbeff },
        { "Preprocessor Text",  0xfff8f631 }
    };

    CodeEditorComponent::ColourScheme cs;

    for (auto& t : types)
        cs.set(t.name, Colour(t.colour));

    editor->setColourScheme(cs);
}

void formula::gui::CodeEditorTab::timerCallback() {
    debugSymbols.setText(pluginState->getDebugString());
}