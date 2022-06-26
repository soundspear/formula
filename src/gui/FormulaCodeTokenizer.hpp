/*
    Copyright (c) 2022 Soundspear.
    Distributed under the Boost Software License, Version 1.0.
    See http://www.boost.org/LICENSE_1_0.txt
*/

#ifndef FORMULA_GUI_FORMULACODETOKENIZER_INCLUDED
#define FORMULA_GUI_FORMULACODETOKENIZER_INCLUDED

#include <JuceHeader.h>

namespace formula::gui {
    /**
     * Editor tokenizer for the C language enhanced with Formula features and macros
     */
    class FormulaCodeTokenizer : public juce::CPlusPlusCodeTokeniser {
    public:
        FormulaCodeTokenizer();
        CodeEditorComponent::ColourScheme getDefaultColourScheme() override;
        int readNextToken(CodeDocument::Iterator &) override;

        enum TokenType
        {
            tokenType_error = 0,
            tokenType_comment,
            tokenType_keyword,
            tokenType_operator,
            tokenType_identifier,
            tokenType_integer,
            tokenType_float,
            tokenType_string,
            tokenType_bracket,
            tokenType_punctuation,
            tokenType_preprocessor,
            tokenType_formulaMacro
        };
    private:
        int parseFormulaIdentifier (CodeDocument::Iterator& source);
        bool isFormulaMacro(const String &token);
        bool isFormulaKeyword(const String &token);

        std::unordered_set<juce::String> formulaMacros;
        std::unordered_set<juce::String> formulaKeywords;
    };
}


#endif //FORMULA_GUI_FORMULACODETOKENIZER_INCLUDED
