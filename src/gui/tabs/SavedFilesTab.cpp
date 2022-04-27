#include "SavedFilesTab.hpp"

using namespace boost::assign;
using FormulaMetadata = formula::processor::FormulaMetadata;
using FormulaMetadataKeys = formula::processor::FormulaMetadataKeys;

formula::gui::SavedFilesTab::SavedFilesTab(
    const std::shared_ptr<formula::events::EventHub>& eventHubRef,
    const std::shared_ptr<formula::processor::PluginState>& pluginStateRef,
    const std::shared_ptr<formula::storage::LocalIndex>& localIndexRef
)
    : eventHub(eventHubRef), pluginState(pluginStateRef), localIndex(localIndexRef)
{
    importButton.setButtonText("Import formula from file");
    importButton.onClick = [this] {
        importFormulaFromFile();
        this->resized();
    };
    addAndMakeVisible(importButton);

    addAndMakeVisible(table);
    table.setModel(this);

    table.setColour(ListBox::outlineColourId, Colours::grey);
    table.setOutlineThickness(1);

    table.getHeader().addColumn("Source", SavedFileColumnsIds::source, 1);
    table.getHeader().addColumn("Name", SavedFileColumnsIds::name, 300);
    table.getHeader().addColumn("Created", SavedFileColumnsIds::created, 175);
    table.getHeader().addColumn("Last Modified", SavedFileColumnsIds::lastModified, 175);
    table.getHeader().addColumn("Description", SavedFileColumnsIds::description, 600);

    table.getHeader().setSortColumnId(4, true);
    table.getHeader().setColumnVisible(1, false);

    loadButton.setButtonText("Load in the editor");
    loadButton.setHelpText("Load formula in the editor");
    addChildComponent(loadButton);
    loadButton.onClick = [this] {
        changeBottomBarVisibility(false);
        const auto metadata = this->data[this->table.getSelectedRow()];
        this->eventHub->publish(EventType::loadFormulaRequest, metadata);
    };

    deleteButton.setButtonText("Delete");
    deleteButton.setHelpText("Delete this Formula");
    addChildComponent(deleteButton);
    deleteButton.onClick = [this] {
        changeBottomBarVisibility(false);
        deleteFormula();
    };

    publishButton.setButtonText("Publish online");
    publishButton.setHelpText("Publish publicly or privately in Formula Cloud");
    addChildComponent(publishButton);
    publishButton.onClick = [this] {
        changeBottomBarVisibility(false);
    };

    exportButton.setButtonText("Export to file");
    exportButton.setHelpText("Save this formula and its configuration to a local file that you can import back");
    addChildComponent(exportButton);
    exportButton.onClick = [this] {
        changeBottomBarVisibility(false);
        exportFormulaToFile();
    };
}

void formula::gui::SavedFilesTab::refreshData()
{
    data.clear();
    for (auto it = localIndex->begin(); it != localIndex->end(); ++it) {
        auto metadata = static_cast<FormulaMetadata>(it);
        data += metadata;
    }
    table.updateContent();
}

void formula::gui::SavedFilesTab::changeBottomBarVisibility(bool visible)
{
    this->loadButton.setVisible(visible);
    this->publishButton.setVisible(visible);
    this->exportButton.setVisible(visible);
    this->deleteButton.setVisible(visible);
}

void formula::gui::SavedFilesTab::exportFormulaToFile()
{
    const auto metadata = this->data[this->table.getSelectedRow()];

    FileChooser chooser("Select the destination file...",
        File::getSpecialLocation(File::userHomeDirectory),
        "", true);

    auto fileChosen = chooser.browseForFileToSave(true);

    if (!fileChosen) return;

    auto fileInfo = chooser.getResult();

    auto serialized = formula::storage::LocalIndex::serializeMetadata(metadata);

    try {
        boost::filesystem::save_string_file(fileInfo.getFullPathName().toStdString(), serialized);
    } catch (std::exception&) {
        eventHub->publish(EventType::unexpectedError, ErrorCodes::cannotExportFile);
    }
}

void formula::gui::SavedFilesTab::importFormulaFromFile()
{
    FileChooser chooser("Select the file to import...",
        File::getSpecialLocation(File::userHomeDirectory),
        "", true);

    auto fileChosen = chooser.browseForFileToOpen();

    if (!fileChosen) return;

    auto fileInfo = chooser.getResult();
    std::string content;
    boost::filesystem::load_string_file(fileInfo.getFullPathName().toStdString(), content);

    try {
        FormulaMetadata metadata = formula::storage::LocalIndex::deserializeMetadata(content);
        localIndex->addFormulaToIndex(metadata, false);
    } catch (std::exception&) {
        eventHub->publish(EventType::unexpectedError, ErrorCodes::cannotImportFile);
    }

    refreshData();
}

void formula::gui::SavedFilesTab::deleteFormula()
{
    AlertWindow confirmAlert("Confirmation", "Are you sure you want to delete this formula?", MessageBoxIconType::WarningIcon);
    confirmAlert.addButton("Yes", 0);
    confirmAlert.addButton("No", 1);
    auto result = AlertWindow::showYesNoCancelBox(
        MessageBoxIconType::WarningIcon,
        "Confirmation",
        "Are you sure you want to delete this formula?",
        "Yes",
        "No",
        "Cancel"
    );

    if (result != 1) return;

    auto &metadata = this->data[this->table.getSelectedRow()];
    localIndex->deleteFormula(metadata[FormulaMetadataKeys::name]);
    refreshData();
}

int formula::gui::SavedFilesTab::getNumRows()
{
    return static_cast<int>(data.size());
}

void formula::gui::SavedFilesTab::paintRowBackground(Graphics& g, int rowNumber, int /*width*/, int /*height*/, bool rowIsSelected)
{
    auto alternateColour = getLookAndFeel().findColour(ListBox::backgroundColourId)
        .interpolatedWith(getLookAndFeel().findColour(ListBox::textColourId), 0.03f);
    if (rowIsSelected)
        g.fillAll(Colours::lightblue);
    else if (rowNumber % 2)
        g.fillAll(alternateColour);
}

void formula::gui::SavedFilesTab::paintCell(Graphics& g, int rowNumber, int columnId,
    int width, int height, bool /*rowIsSelected*/)
{
    if (rowNumber >= data.size()) {
        return;
    }
    g.setColour(getLookAndFeel().findColour(ListBox::textColourId));
    g.setFont(Font());

    auto row = data[rowNumber];
    String text;

    switch (columnId) {
    case SavedFileColumnsIds::name:
        text = row[FormulaMetadataKeys::name];
        break;
    case SavedFileColumnsIds::created:
        text = row[FormulaMetadataKeys::created];
        break;
    case SavedFileColumnsIds::lastModified:
        text = row[FormulaMetadataKeys::lastModified];
        break;
    case SavedFileColumnsIds::description:
        text = row[FormulaMetadataKeys::description];
        break;
    default:
        return;
    }

    g.drawText(text, 2, 0, width - 4, height, Justification::centredLeft, true);

    g.setColour(getLookAndFeel().findColour(ListBox::backgroundColourId));
    g.fillRect(width - 1, 0, 1, height);
}

void formula::gui::SavedFilesTab::sortOrderChanged(int newSortColumnId, bool isForwards)
{
    std::function<bool(FormulaMetadata, FormulaMetadata)> predicate;
    switch (newSortColumnId) {
        case SavedFileColumnsIds::name:
            predicate = [](FormulaMetadata a, FormulaMetadata b) { 
                return a[FormulaMetadataKeys::name] < b[FormulaMetadataKeys::name];
            };
            break;
            /* /!\ A PARSER EN DATE */
        case SavedFileColumnsIds::created:
            predicate = [](FormulaMetadata a, FormulaMetadata b) {
                return a[FormulaMetadataKeys::created] < b[FormulaMetadataKeys::created];
            };
            break;
        case SavedFileColumnsIds::lastModified:
            predicate = [](FormulaMetadata a, FormulaMetadata b) {
                return a[FormulaMetadataKeys::lastModified] < b[FormulaMetadataKeys::lastModified];
            };
            break;
        case SavedFileColumnsIds::description:
            predicate = [](FormulaMetadata a, FormulaMetadata b) {
                return a[FormulaMetadataKeys::description] < b[FormulaMetadataKeys::description];
            };
            break;
        default:
            return;
    }
    if (isForwards) {
        std::sort(data.begin(), data.end(), predicate);
    }
    else {
        std::sort(data.rbegin(), data.rend(), predicate);
    }

    table.updateContent();
}
// This is overloaded from TableListBoxModel, and must update any custom components that we're using
Component* formula::gui::SavedFilesTab::refreshComponentForCell(int /*rowNumber*/ , int /*columnId*/ , bool /*isRowSelected*/,
    Component* /*existingComponentToUpdate*/)
{
    return nullptr;
}

void formula::gui::SavedFilesTab::selectedRowsChanged(int /*lastRowSelected*/) {
    changeBottomBarVisibility(true);
    repaint();
}

void formula::gui::SavedFilesTab::resized()
{
    constexpr auto topBarHeight = 32;
    constexpr auto topBarMargin = 4;
    constexpr auto buttonImportWidthPixels = 256;
    constexpr auto bottomBarHeight = 64;
    constexpr auto buttonWidthPixels = 175;
    constexpr auto buttonHeightPixels = 36;
    constexpr auto bottomButtonsMargin = 16;
    constexpr auto tableMargin = 8;

    auto area = getLocalBounds();

    auto topBarArea = area.removeFromTop(topBarHeight);
    importButton.setBounds(
        topBarArea.removeFromLeft(buttonImportWidthPixels)
        .withTrimmedTop(topBarMargin).withTrimmedBottom(topBarMargin).withTrimmedLeft(tableMargin)
    );

    deleteButton.setBounds(
        topBarArea.removeFromLeft(buttonImportWidthPixels)
        .withTrimmedTop(topBarMargin).withTrimmedBottom(topBarMargin).withTrimmedLeft(tableMargin)
    );

    table.setBounds(area.withTrimmedLeft(tableMargin).withTrimmedRight(tableMargin));

    auto buttonsArea = area.removeFromBottom(bottomBarHeight);
    const auto buttonsCenter = buttonsArea.getCentre();
    loadButton.setBounds(Rectangle<int> {
        buttonsCenter.getX() - buttonWidthPixels * 3 / 2 - bottomButtonsMargin, buttonsArea.getY(),
            buttonWidthPixels, buttonHeightPixels
    });
    publishButton.setBounds(Rectangle<int> {
        buttonsCenter.getX() - buttonWidthPixels / 2, buttonsArea.getY(),
            buttonWidthPixels, buttonHeightPixels
    });
    exportButton.setBounds(Rectangle<int> {
        buttonsCenter.getX() + buttonWidthPixels / 2 + bottomButtonsMargin, buttonsArea.getY(),
            buttonWidthPixels, buttonHeightPixels
    });
}

void formula::gui::SavedFilesTab::visibilityChanged()
{
    if (isVisible()) {
        localIndex->refreshIndex();
        refreshData();
        table.updateContent();
    }
}
