/*
    Copyright (c) 2022 Soundspear.
    Distributed under the Boost Software License, Version 1.0.
    See http://www.boost.org/LICENSE_1_0.txt
*/

#include "FormulaCodeEditor.hpp"

formula::gui::FormulaCodeTokenizer formula::gui::FormulaCodeEditor::tokenizer;

formula::gui::FormulaCodeEditor::FormulaCodeEditor(juce::CodeDocument& documentRef)
: CodeEditorComponent(documentRef, &tokenizer) {
    setCodeEditorComponentColourScheme();
}

void formula::gui::FormulaCodeEditor::setCodeEditorComponentColourScheme()
{
    struct Type
    {
        const char* name;
        juce::uint32 colour;
    };

    const Type types[] =
            {
                    { "Error",              0xffe60000 },
                    { "Comment",            0xff9fbc80 },
                    { "Keyword",            0xffee6f6f },
                    { "Operator",           0xffc4eb19 },
                    { "Identifier",         0xffbdbdbd },
                    { "Integer",            0xff639dbb },
                    { "Float",              0xff639dbb },
                    { "String",             0xffbc45dd },
                    { "Bracket",            0xff058202 },
                    { "Punctuation",        0xffcfbeff },
                    { "Preprocessor Text",  0xffcfcfcf },
                    { "Formula Macro",      0xffcd9898 },
            };

    CodeEditorComponent::ColourScheme cs;

    for (auto& t : types)
        cs.set(t.name, Colour(t.colour));

    setColourScheme(cs);
}
