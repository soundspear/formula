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
