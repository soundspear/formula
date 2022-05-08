#ifndef FORMULA_GUI_CODE_EDITOR_TAB_INCLUDED
#define FORMULA_GUI_CODE_EDITOR_TAB_INCLUDED

#include <boost/any.hpp>

#include "Formula_artefacts/JuceLibraryCode/JuceHeader.h"

#include "src/gui/components/IconButton.hpp"
#include "src/gui/components/popups/SaveLocalFormulaPopup.hpp"
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
            const std::shared_ptr<formula::events::EventHub>& eventHubRef,
            const std::shared_ptr<formula::processor::PluginState>& pluginStateRef,
            const std::shared_ptr<formula::storage::LocalIndex>& localIndexRef);
        ~CodeEditorTab();

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

        juce::String findAutoTabulation();
    private:
        CodeDocument codeDocument;
        CPlusPlusCodeTokeniser cppTokeniser;
        std::unique_ptr<CodeEditorComponent> editor;
        formula::gui::IconButton newButton;
        formula::gui::IconButton compileButton;
        formula::gui::IconButton muteButton;
        formula::gui::IconButton saveLocalButton;
        formula::gui::IconButton showDebugButton;
        formula::gui::IconButton showKnobsButton;
        formula::gui::IconButton zoomInButton;
        formula::gui::IconButton zoomOutButton;

        formula::gui::SaveLocalFormulaPopup savePopup;

        formula::gui::KnobsPanel<FORMULA_NUM_USER_KNOBS, FORMULA_NUM_USER_SWITCHES> knobsPanel;

        juce::TextEditor compilerErrors;
        juce::TextEditor debugSymbols;

        std::shared_ptr<formula::events::EventHub> eventHub;
        std::shared_ptr<formula::processor::PluginState> pluginState;

        void setCodeEditorComponentColourScheme();

        std::string defaultEditorContent = R"""(
formula_main {
    float output = input;
    return output;
}
)""";

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CodeEditorTab)
    };
}

#endif // FORMULA_GUI_CODE_EDITOR_TAB_INCLUDED