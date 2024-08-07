/*
    Copyright (c) 2022 Soundspear.
    Distributed under the Boost Software License, Version 1.0.
    See http://www.boost.org/LICENSE_1_0.txt
*/

#include "FormulaCodeEditor.hpp"

formula::gui::FormulaCodeTokenizer formula::gui::FormulaCodeEditor::tokenizer;

formula::gui::FormulaCodeEditor::FormulaCodeEditor(const std::shared_ptr<events::EventHub>& eventHub, juce::CodeDocument& documentRef)
    : CodeEditorComponent(documentRef, &tokenizer), searchBar(eventHub)
{
    addChildComponent(searchBar);

    searchBar.setSearchEventType(EventType::codeSearch);
    setCodeEditorComponentColourScheme();
}

bool formula::gui::FormulaCodeEditor::keyPressed (const KeyPress& key)
{
    if (CodeEditorComponent::keyPressed(key)) return true;

    if (key == KeyPress ('f', ModifierKeys::commandModifier, 0))
    {
        toggleSearch();
        return true;
    }

    return false;
}

void formula::gui::FormulaCodeEditor::resized()
{
    CodeEditorComponent::resized();

    constexpr auto searchBarHeight = 24;
    constexpr auto margin = 8;

    auto area = getLocalBounds()
            .withTrimmedTop(margin)
            .withTrimmedLeft(margin);

    searchBar.setBounds(area.removeFromTop(searchBarHeight));
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

void formula::gui::FormulaCodeEditor::toggleSearch()
{
    if (!searchBar.isVisible())
    {
        searchBar.setVisible(true);
        searchBar.grabKeyboardFocus();
    }
    else
    {
        searchBar.setVisible(false);
    }
}
