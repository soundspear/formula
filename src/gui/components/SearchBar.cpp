/*
    Copyright (c) 2022 Soundspear.
    Distributed under the Boost Software License, Version 1.0.
    See http://www.boost.org/LICENSE_1_0.txt
*/

#include "SearchBar.hpp"

formula::gui::SearchBar::SearchBar(const std::shared_ptr<formula::events::EventHub>& eventHubRef)
: eventHub(eventHubRef), searchEventType(EventType::undefined) {
    queryEditor.setMultiLine(false, false);
    addAndMakeVisible(queryEditor);

    searchButton.setImage(formula::binary::search_svg, Colour::fromRGB(0xaf,0xb1,0xb3));
    queryEditor.onTextChange = [this]() {
        this->eventHub->publish(searchEventType, SearchAction::QueryChanged);
    };
    queryEditor.onEscapeKey = [this]() {
        this->eventHub->publish(searchEventType, SearchAction::SearchCancelled);
    };
    queryEditor.onReturnKey = [this]() {
        this->eventHub->publish(searchEventType, SearchAction::SearchValidated);
    };
    searchButton.onClick = [this]() {
        this->eventHub->publish(searchEventType, SearchAction::SearchValidated);
    };
    addAndMakeVisible(searchButton);
}

void formula::gui::SearchBar::resized() {
    Component::resized();

    constexpr auto searchQueryWidth = 200;
    constexpr auto buttonSize = 24;
    constexpr auto searchQueryButtonMargin = 6;

    auto area = getLocalBounds();

    queryEditor.setBounds(area.removeFromLeft(searchQueryWidth));
    area.removeFromLeft(searchQueryButtonMargin);

    searchButton.setBounds(area.removeFromLeft(buttonSize));
}

void formula::gui::SearchBar::setSearchEventType(EventType newSearchEventType)
{
    this->searchEventType = newSearchEventType;
}
