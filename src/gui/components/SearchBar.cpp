#include "SearchBar.hpp"

formula::gui::SearchBar::SearchBar(const std::shared_ptr<formula::events::EventHub>& eventHubRef)
: eventHub(eventHubRef) {
    queryEditor.setMultiLine(false, false);
    addAndMakeVisible(queryEditor);

    searchButton.setImage(formula::binary::search_svg, Colour::fromRGB(0xaf,0xb1,0xb3));
    searchButton.onClick = [this]() {
        this->eventHub->publish(EventType::searchFormulaRequest);
    };
    addAndMakeVisible(searchButton);

    shouldSearchOnlyUserFormulasToggle.setButtonText("Show only your formulas");
    addAndMakeVisible(shouldSearchOnlyUserFormulasToggle);
    shouldSearchOnlyUserFormulasToggle.onClick = [this]() {
        this->eventHub->publish(EventType::searchFormulaRequest);
    };
}

void formula::gui::SearchBar::resized() {
    Component::resized();

    constexpr auto searchQueryWidth = 200;
    constexpr auto buttonSize = 24;
    constexpr auto checkBoxWidth = 200;
    constexpr auto searchQueryButtonMargin = 6;

    auto area = getLocalBounds();

    queryEditor.setBounds(area.removeFromLeft(searchQueryWidth));
    area.removeFromLeft(searchQueryButtonMargin);

    searchButton.setBounds(area.removeFromLeft(buttonSize));

    area = getLocalBounds();
    shouldSearchOnlyUserFormulasToggle.setBounds(area.removeFromRight(checkBoxWidth));
}
