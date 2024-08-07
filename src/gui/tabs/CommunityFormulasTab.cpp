#include <boost/format.hpp>

#include "CommunityFormulasTab.hpp"

using namespace boost::assign;

formula::gui::CommunityFormulasTab::CommunityFormulasTab(
    const std::shared_ptr<formula::events::EventHub>& eventHubRef,
    const std::shared_ptr<formula::storage::CommunityIndex>& communityIndexRef
)
    :   FormulaListTabBase(communityIndexRef),
        eventHub(eventHubRef),
        sortColumn("last_modified"), sortDirection("desc"),
        detailsPanel(eventHubRef),
        searchBar(eventHubRef)
{
    searchBar.setSearchEventType(EventType::searchFormulaRequest);
    addAndMakeVisible(searchBar);

    table.setColour(ListBox::outlineColourId, Colours::grey);
    table.setOutlineThickness(2);
    table.setRowHeight(30);
    auto & header = table.getHeader();
    header.addColumn("Name", ColumnsIds::name, 200);
    header.addColumn("Description", ColumnsIds::description, 600);
    header.addColumn("Created", ColumnsIds::created, 100);
    header.addColumn("LastModified", ColumnsIds::lastModified, 100);
    table.setModel(this);
    addAndMakeVisible(table);

    addChildComponent(detailsPanel);

    eventHub->subscribeOnUiThread<CommunityFormulasTab>(
            EventType::searchFormulaRequest, []([[maybe_unused]] boost::any arg, CommunityFormulasTab* thisPtr) {
        thisPtr->refreshData();
    }, this);
}

formula::gui::CommunityFormulasTab::~CommunityFormulasTab() {
    eventHub->unsubscribe(this);
}

void formula::gui::CommunityFormulasTab::refreshData()
{
    data.clear();
    for (auto it = index->begin(); it != index->end(); ++it) {
        auto metadata = static_cast<formula::processor::FormulaMetadata>(it);
        const auto& searchQuery = searchBar.getQuery();
        if (searchQuery.empty() || formula::processor::formulaContains(metadata, searchQuery)) {
            data += metadata;
        }
    }

    table.updateContent();
}

void formula::gui::CommunityFormulasTab::selectedRowsChanged([[maybe_unused]] int lastRowSelected) {
    const auto selectedRowIdx = table.getSelectedRow();
    if (selectedRowIdx == -1 || static_cast<unsigned>(selectedRowIdx) >= data.size()) {
        return;
    }
    const auto & selectedRowInformation = data[static_cast<unsigned>(selectedRowIdx)];

    detailsPanel.setFormula(selectedRowInformation);
    detailsPanel.setVisible(true);
    resized();

    table.deselectAllRows();
}

void formula::gui::CommunityFormulasTab::resized() {
    constexpr auto margin = 8;
    constexpr auto searchBarHeight = 24;

    auto area = getLocalBounds()
            .withTrimmedTop(margin)
            .withTrimmedBottom(margin)
            .withTrimmedLeft(margin)
            .withTrimmedRight(margin);

    searchBar.setBounds(area.removeFromTop(searchBarHeight));
    area.removeFromTop(margin);

    table.setBounds(area);

    detailsPanel.setBounds(getLocalBounds().removeFromLeft(getLocalBounds().getWidth() / 3));
}