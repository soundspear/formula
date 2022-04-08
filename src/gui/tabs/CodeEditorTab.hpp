#ifndef FORMULA_GUI_CODE_EDITOR_TAB_INCLUDED
#define FORMULA_GUI_CODE_EDITOR_TAB_INCLUDED

#include <boost/any.hpp>

#include "Formula_artefacts/JuceLibraryCode/JuceHeader.h"

#include "src/gui/components/SvgButton.hpp"
#include "src/gui/components/SaveLocalFormulaPopup.hpp"
#include "src/gui/components/KnobsPanel.hpp"
#include "src/events/EventHub.hpp"
#include "src/processor/PluginState.hpp"
#include "src/processor/FormulaMetadata.hpp"

namespace formula::gui
{
class CodeEditorTab : public juce::Component, public juce::CodeDocument::Listener, public juce::Timer
    {
    public:
        CodeEditorTab(
            const std::shared_ptr<formula::events::EventHub>& eventHub,
            const std::shared_ptr<formula::processor::PluginState>& pluginState);
        void paint(Graphics& g) override;
        void resized() override;

        void codeDocumentTextInserted(
            const String& newText,
            int 	insertIndex
        ) override;
        void codeDocumentTextDeleted(
            int startIndex,
            int endIndex
        ) override;

        void timerCallback() override;
    private:
        CodeDocument codeDocument;
        CPlusPlusCodeTokeniser cppTokeniser;
        std::unique_ptr<CodeEditorComponent> editor;
        formula::gui::FormulaButton compileButton;
        formula::gui::FormulaButton muteButton;
        formula::gui::FormulaButton saveLocalButton;
        formula::gui::FormulaButton showDebugButton;
        formula::gui::FormulaButton showKnobsButton;
        formula::gui::FormulaButton zoomInButton;
        formula::gui::FormulaButton zoomOutButton;

        formula::gui::SaveLocalFormulaPopup savePopup;

        formula::gui::KnobsPanel<FORMULA_NUM_USER_KNOBS, FORMULA_NUM_USER_SWITCHES> knobsPanel;

        juce::TextEditor compilerErrors;
        juce::TextEditor debugSymbols;

        std::shared_ptr<formula::events::EventHub> eventHub;
        std::shared_ptr<formula::processor::PluginState> pluginState;

        void setCodeEditorComponentColourScheme();

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CodeEditorTab)
    };
}

#endif // FORMULA_GUI_CODE_EDITOR_TAB_INCLUDED