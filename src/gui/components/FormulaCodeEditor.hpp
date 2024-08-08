/*
    Copyright (c) 2022 Soundspear.
    Distributed under the Boost Software License, Version 1.0.
    See http://www.boost.org/LICENSE_1_0.txt
*/

#ifndef FORMULA_GUI_FORMULACODEEDITOR_INCLUDED
#define FORMULA_GUI_FORMULACODEEDITOR_INCLUDED

#include <JuceHeader.h>
#include <gui/FormulaCodeTokenizer.hpp>

#include <gui/components/SearchBar.hpp>

namespace formula::gui {
    /**
     * Formula code editor component
     */
    class FormulaCodeEditor : public CodeEditorComponent {
    public:
        FormulaCodeEditor(const std::shared_ptr<events::EventHub>& eventHub, juce::CodeDocument& document);
        bool keyPressed (const KeyPress& key) override;
        void resized() override;
        void unselect();
    private:
        void setCodeEditorComponentColourScheme();
        void toggleSearchBar();
        void runSearchQuery();
        void goToNextSearchResult();

        static formula::gui::FormulaCodeTokenizer tokenizer;
        formula::gui::SearchBar searchBar;
        std::vector<std::pair<CodeDocument::Position, int>> searchMatches;
        unsigned int currentSearchMatch = 0;
    };
}


#endif //FORMULA_GUI_FORMULACODEEDITOR_INCLUDED
