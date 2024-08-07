/*
    Copyright (c) 2022 Soundspear.
    Distributed under the Boost Software License, Version 1.0.
    See http://www.boost.org/LICENSE_1_0.txt
*/

#include <regex>
#include "FormulaCodeEditor.hpp"

formula::gui::FormulaCodeTokenizer formula::gui::FormulaCodeEditor::tokenizer;

formula::gui::FormulaCodeEditor::FormulaCodeEditor(const std::shared_ptr<events::EventHub>& eventHub, juce::CodeDocument& documentRef)
    : CodeEditorComponent(documentRef, &tokenizer), searchBar(eventHub)
{
    addChildComponent(searchBar);

    searchBar.setSearchEventType(EventType::codeSearch);
    setCodeEditorComponentColourScheme();

    eventHub->subscribeOnUiThread<FormulaCodeEditor>(
            EventType::codeSearch, []([[maybe_unused]] boost::any arg, FormulaCodeEditor* thisPtr) {
        auto actionType = boost::any_cast<formula::gui::SearchAction>(arg);
        switch (actionType)
        {
        case SearchAction::QueryChanged:
            thisPtr->runSearchQuery();
            thisPtr->goToNextSearchResult();
            break;
        case SearchAction::SearchValidated:
            thisPtr->goToNextSearchResult();
            break;
        case SearchAction::SearchCancelled:
            thisPtr->toggleSearchBar();
            thisPtr->unselect();
            break;
        }
    }, this);
}

bool formula::gui::FormulaCodeEditor::keyPressed (const KeyPress& key)
{
    if (key == KeyPress ('f', ModifierKeys::commandModifier, 0))
    {
        toggleSearchBar();
        return true;
    }

    if (key == KeyPress::tabKey && isHighlightActive())
    {
        indentSelection();
        return true;
    }
    if (key == KeyPress (KeyPress::tabKey, ModifierKeys::shiftModifier, 0))
    {
        unindentSelection();
        return true;
    }

    return CodeEditorComponent::keyPressed(key);
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

void formula::gui::FormulaCodeEditor::unselect()
{
    selectRegion(getCaretPos(), getCaretPos());
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

void formula::gui::FormulaCodeEditor::toggleSearchBar()
{
    if (!searchBar.isVisible())
    {
        searchBar.setVisible(true);
        if (isShowing())
        {
            searchBar.grabKeyboardFocus();
        }
    }
    else
    {
        searchBar.setVisible(false);
    }
}

void formula::gui::FormulaCodeEditor::runSearchQuery()
{
    const auto& query = searchBar.getQuery();
    if (query.empty()) return;

    const auto& content = getDocument().getAllContent().toStdString();
    searchMatches.clear();
    currentSearchMatch = 0;

    const std::regex pattern(query, std::regex_constants::icase);
    std::smatch match;
    std::string::const_iterator searchStart(content.cbegin());
    int charactersFromStartOfDocument = 0;
    while (std::regex_search(searchStart, content.cend(), match, pattern))
    {
        charactersFromStartOfDocument += static_cast<int>(match.position());
        const auto position = CodeDocument::Position(getDocument(), charactersFromStartOfDocument);
        searchMatches.emplace_back(std::make_pair(position, match.length()));
        charactersFromStartOfDocument += static_cast<int>(match.length());
        searchStart = match.suffix().first;
    }
}

void formula::gui::FormulaCodeEditor::goToNextSearchResult()
{
    if (searchMatches.empty())
    {
        return;
    }

    if (++currentSearchMatch >= static_cast<int>(searchMatches.size()))
    {
        currentSearchMatch = 0;
    }

    const auto& [position, size] = searchMatches[currentSearchMatch];
    moveCaretTo(position, false);
    const Range highlight (position.getPosition(), position.getPosition() + size);
    setHighlightedRegion(highlight);
}
