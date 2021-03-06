#ifndef FORMULA_GUI_CODEEDITORTAB_INCLUDED
#define FORMULA_GUI_CODEEDITORTAB_INCLUDED

#include <boost/any.hpp>

#include "Formula_artefacts/JuceLibraryCode/JuceHeader.h"

#include "src/gui/components/IconButton.hpp"
#include "src/gui/components/popups/SaveLocalFormulaPopup.hpp"
#include "src/gui/components/KnobsPanel.hpp"
#include "src/gui/components/FormulaCodeEditor.hpp"
#include "src/events/EventHub.hpp"
#include "src/processor/PluginState.hpp"
#include "src/processor/FormulaMetadata.hpp"

namespace formula::gui
{
    /**
     * Application tab that displays the code editor and the buttons sidebar
     */
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
        std::unique_ptr<formula::gui::FormulaCodeEditor> editor;
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

        std::string defaultEditorContent = R"""(
/*
    Welcome to Formula!

    If this is your first time, be sure to check out the tutorials
    under the 'Saved files' tab.
*/

formula_main {
    float output = input;
    return output;
}
)""";

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CodeEditorTab)
    };
}

#endif // FORMULA_GUI_CODEEDITORTAB_INCLUDED