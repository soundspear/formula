/*
    Copyright (c) 2022 Soundspear.
    Distributed under the Boost Software License, Version 1.0.
    See http://www.boost.org/LICENSE_1_0.txt
*/

#ifndef FORMULA_GUI_FORMULACODEEDITOR_INCLUDED
#define FORMULA_GUI_FORMULACODEEDITOR_INCLUDED

#include <JuceHeader.h>
#include <gui/FormulaCodeTokenizer.hpp>

namespace formula::gui {
    class FormulaCodeEditor : public CodeEditorComponent {
    public:
        FormulaCodeEditor(juce::CodeDocument& document);
    private:
        void setCodeEditorComponentColourScheme();

        static formula::gui::FormulaCodeTokenizer tokenizer;
    };
}


#endif //FORMULA_GUI_FORMULACODEEDITOR_INCLUDED
