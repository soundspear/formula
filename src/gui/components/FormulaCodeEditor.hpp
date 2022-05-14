#ifndef FORMULA_GUI_FORMULACODEEDITOR_INCLUDED
#define FORMULA_GUI_FORMULACODEEDITOR_INCLUDED

#include <JuceHeader.h>

namespace formula::gui {
    class FormulaCodeEditor : public CodeEditorComponent {
    public:
        FormulaCodeEditor(juce::CodeDocument& document);
    private:
        void setCodeEditorComponentColourScheme();

        static CPlusPlusCodeTokeniser cppTokeniser;
    };
}


#endif //FORMULA_GUI_FORMULACODEEDITOR_INCLUDED
