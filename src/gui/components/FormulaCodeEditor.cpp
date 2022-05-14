#include "FormulaCodeEditor.hpp"

juce::CPlusPlusCodeTokeniser formula::gui::FormulaCodeEditor::cppTokeniser;

formula::gui::FormulaCodeEditor::FormulaCodeEditor(juce::CodeDocument& document)
: CodeEditorComponent(document, &cppTokeniser) {
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

    setColourScheme(cs);
}
