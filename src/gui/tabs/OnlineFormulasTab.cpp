#include <boost/format.hpp>
#include "OnlineFormulasTab.hpp"
#include "gui/components/RatingComponent.hpp"

using namespace boost::assign;

formula::gui::OnlineFormulasTab::OnlineFormulasTab(const std::shared_ptr<formula::events::EventHub>& eventHubRef,
                                                   const std::shared_ptr<formula::cloud::FormulaCloudClient>& cloudRef)
        :   cloud(cloudRef),
            eventHub(eventHubRef),
            sortColumn("last_modified"), sortDirection("desc"),
            detailsPanel(eventHubRef),
            searchBar(eventHubRef),
            endOfResultsReached(false)
{
    addAndMakeVisible(searchBar);

    table.setColour(ListBox::outlineColourId, Colours::grey);
    table.setOutlineThickness(2);
    table.setRowHeight(30);
    auto & header = table.getHeader();
    header.addColumn("Name", OnlineFormulasColumnsIds::name, 200);
    header.addColumn("Author", OnlineFormulasColumnsIds::author, 125);

#ifdef __PREVIEW_SHOW_RATINGS
    header.addColumn("Rating", OnlineFormulasColumnsIds::rating, 75);
#endif

    header.addColumn("Description", OnlineFormulasColumnsIds::description, 600);
    header.addColumn("Created", OnlineFormulasColumnsIds::created, 100);
    header.addColumn("LastModified", OnlineFormulasColumnsIds::lastModified, 100);
    table.setModel(this);
    table.getVerticalScrollBar().addListener(this);
    addAndMakeVisible(table);

    addChildComponent(detailsPanel);

    eventHub->subscribeOnUiThread<OnlineFormulasTab>(
            EventType::listFormulaResponse, [](boost::any arg, OnlineFormulasTab* thisPtr) {
        thisPtr->searchParams.skip += thisPtr->searchParams.take;
        const auto response = boost::any_cast<std::vector<formula::cloud::ListFormulaDto>>(arg);
        if (response.size() < static_cast<unsigned>(thisPtr->searchParams.take)) {
            thisPtr->endOfResultsReached = true;
        }
        thisPtr->data.insert(thisPtr->data.end(), response.begin(), response.end());
        thisPtr->table.updateContent();
    }, this);

    eventHub->subscribeOnUiThread<OnlineFormulasTab>(
            EventType::getFormulaResponse, [](boost::any arg, OnlineFormulasTab* thisPtr) {
        const auto response = boost::any_cast<formula::cloud::GetFormulaDto>(arg);
        thisPtr->detailsPanel.setFormulaDto(response);
        thisPtr->detailsPanel.setVisible(true);
        thisPtr->resized();
    }, this);

    eventHub->subscribeOnUiThread<OnlineFormulasTab>(
            EventType::searchFormulaRequest, []([[maybe_unused]] boost::any arg, OnlineFormulasTab* thisPtr) {
        thisPtr->data.clear();
        thisPtr->endOfResultsReached = false;
        thisPtr->searchParams.skip = 0;
        thisPtr->makeSearchAsync();
    }, this);
}

formula::gui::OnlineFormulasTab::~OnlineFormulasTab() {
    eventHub->unsubscribe(this);
}

int formula::gui::OnlineFormulasTab::getNumRows() {
    return static_cast<int>(data.size());
}

void formula::gui::OnlineFormulasTab::paintRowBackground(Graphics &g, int rowNumber, int, int, bool rowIsSelected) {
    auto alternateColour = getLookAndFeel().findColour(ListBox::backgroundColourId)
            .interpolatedWith(getLookAndFeel().findColour(ListBox::textColourId), 0.03f);
    if (rowIsSelected)
        g.fillAll(Colours::lightblue);
    else if (rowNumber % 2)
        g.fillAll(alternateColour);
}

void formula::gui::OnlineFormulasTab::paintCell(Graphics &g, int rowNumber, int columnId, int width, int height, bool) {
    if (static_cast<unsigned>(rowNumber) >= data.size()) {
        return;
    }

    g.setColour(getLookAndFeel().findColour(ListBox::textColourId));
    g.setFont(Font());

    auto row = data[static_cast<unsigned>(rowNumber)];
    String text;

    switch (columnId) {
        case OnlineFormulasColumnsIds::name:
            text = row.name;
            break;
        case OnlineFormulasColumnsIds::author:
            text = row.author;
            break;
        case OnlineFormulasColumnsIds::description:
            text = row.description;
            break;
        case OnlineFormulasColumnsIds::created:
            text = boost::posix_time::to_simple_string(row.created);
            text = text.dropLastCharacters(text.length() - 11);
            break;
        case OnlineFormulasColumnsIds::lastModified:
            text = boost::posix_time::to_simple_string(row.lastModified);
            text = text.dropLastCharacters(text.length() - 11);
            break;
        default:
            return;
    }

    g.drawText(text, 2, 0, width - 4, height, Justification::centredLeft, true);

    g.setColour(getLookAndFeel().findColour(ListBox::backgroundColourId));
    g.fillRect(width - 1, 0, 1, height);
}

void formula::gui::OnlineFormulasTab::sortOrderChanged(int newSortColumnId, bool isForwards) {
    switch (newSortColumnId) {
        case OnlineFormulasColumnsIds::name:
            sortColumn = "name";
            break;
        case OnlineFormulasColumnsIds::author:
            sortColumn = "author";
            break;
        case OnlineFormulasColumnsIds::lastModified:
            sortColumn = "last_modified";
            break;
        case OnlineFormulasColumnsIds::created:
            sortColumn = "created";
            break;
        default:
            return;
    }
    searchParams.skip = 0;
    data.clear();
    endOfResultsReached = false;
    sortDirection = isForwards ? "asc" : "desc";

    makeSearchAsync();
}

Component *formula::gui::OnlineFormulasTab::refreshComponentForCell(int /*rowNumber*/, int /*columnId*/, bool,
                                                                    Component */*existingComponentToUpdate*/) {

#ifdef __PREVIEW_SHOW_RATINGS
    if (columnId == OnlineFormulasColumnsIds::rating) {
        auto* ratingsBox = dynamic_cast<formula::gui::RatingComponent*> (existingComponentToUpdate);

        if (ratingsBox == nullptr)
            ratingsBox = new RatingComponent(false);

        if (data[rowNumber].rating.has_value())
            ratingsBox->setRating(data[rowNumber].rating.value());
        else
            ratingsBox->setRating(-1);
        return ratingsBox;
    }
#endif
    return nullptr;
}

void formula::gui::OnlineFormulasTab::selectedRowsChanged([[maybe_unused]] int lastRowSelected) {
    const auto selectedRowIdx = table.getSelectedRow();
    if (selectedRowIdx == -1 || static_cast<unsigned>(selectedRowIdx) >= data.size()) {
        return;
    }
    const auto & selectedRowInformation = data[static_cast<unsigned>(selectedRowIdx)];
    cloud->getFormula(selectedRowInformation.id);
    table.deselectAllRows();
}

void formula::gui::OnlineFormulasTab::resized() {
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

void formula::gui::OnlineFormulasTab::visibilityChanged() {
    if (isVisible() && data.size() == 0) {
        makeSearchAsync();
    }
}

void formula::gui::OnlineFormulasTab::scrollBarMoved(ScrollBar *scrollBarThatHasMoved, [[maybe_unused]] double newRangeStart) {
    const auto barRange = scrollBarThatHasMoved->getCurrentRange();
    const auto scrollLimit = scrollBarThatHasMoved->getMaximumRangeLimit();
    if (barRange.getEnd() >= scrollLimit && !this->endOfResultsReached) {
        makeSearchAsync();
    }
}

void formula::gui::OnlineFormulasTab::makeSearchAsync() {
    cloud->listFormulas(searchParams.skip, searchParams.take, sortColumn, sortDirection,
                        searchBar.shouldSearchOnlyUserFormulas(), searchBar.getQuery());
}