#include "SavedFilesTab.hpp"

using namespace boost::assign;
using FormulaMetadata = formula::processor::FormulaMetadata;
using FormulaMetadataKeys = formula::processor::FormulaMetadataKeys;

formula::gui::SavedFilesTab::SavedFilesTab(
    const std::shared_ptr<formula::events::EventHub>& eventHubRef,
    const std::shared_ptr<formula::processor::PluginState>& pluginStateRef,
    const std::shared_ptr<formula::storage::UserIndex>& localIndexRef
)
    : FormulaListTabBase(localIndexRef), eventHub(eventHubRef), pluginState(pluginStateRef)
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
    table.setOutlineThickness(2);
    table.setRowHeight(30);

    auto & header = table.getHeader();
    header.addColumn("Source", ColumnsIds::source, 1);
    header.addColumn("Name", ColumnsIds::name, 300);
    header.addColumn("Created", ColumnsIds::created, 175);
    header.addColumn("Last Modified", ColumnsIds::lastModified, 175);
    header.addColumn("Description", ColumnsIds::description, 600);

    header.setSortColumnId(4, true);
    header.setColumnVisible(1, false);

    loadButton.setButtonText("Load in the editor");
    loadButton.setHelpText("Load formula in the editor");
    addChildComponent(loadButton);
    loadButton.onClick = [this] {
        changeBottomBarVisibility(false);
        const auto metadata = this->data[static_cast<unsigned int>(this->table.getSelectedRow())];
        this->eventHub->publish(EventType::loadFormulaRequest, metadata);
        this->table.deselectAllRows();
    };

    deleteButton.setButtonText("Delete");
    deleteButton.setHelpText("Delete this Formula");
    addChildComponent(deleteButton);
    deleteButton.onClick = [this] {
        changeBottomBarVisibility(false);
        deleteFormula();
        this->table.deselectAllRows();
    };

    exportButton.setButtonText("Export to file");
    exportButton.setHelpText("Save this formula and its configuration to a local file that you can import back");
    addChildComponent(exportButton);
    exportButton.onClick = [this] {
        exportFormulaToFile();
        this->table.deselectAllRows();
    };
}

void formula::gui::SavedFilesTab::refreshData()
{
    data.clear();
    for (auto it = index->begin(); it != index->end(); ++it) {
        auto metadata = static_cast<FormulaMetadata>(it);
        data += metadata;
    }
    table.updateContent();
}

void formula::gui::SavedFilesTab::changeBottomBarVisibility(bool visible)
{
    this->loadButton.setVisible(visible);
    this->exportButton.setVisible(visible);
    this->deleteButton.setVisible(visible);
}

void formula::gui::SavedFilesTab::exportFormulaToFile()
{
    const auto metadata = this->data[static_cast<unsigned int>(this->table.getSelectedRow())];

    FileChooser chooser("Select the destination file...",
        File::getSpecialLocation(File::userHomeDirectory),
        "", true);

    auto fileChosen = chooser.browseForFileToSave(true);

    if (!fileChosen) return;

    auto fileInfo = chooser.getResult();

    auto serialized = formula::storage::LocalIndex::serializeMetadata(metadata);

    try {
        std::ofstream file(fileInfo.getFullPathName().toStdString());
        file << serialized;
        file.close();
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
    try {
        std::ifstream file(fileInfo.getFullPathName().toStdString());
        std::stringstream buffer;
        buffer << file.rdbuf();
        file.close();

        std::string content = buffer.str();

        FormulaMetadata metadata = formula::storage::LocalIndex::deserializeMetadata(content);
        index->addFormulaToIndex(metadata, false);
    } catch (std::exception&) {
        eventHub->publish(EventType::unexpectedError, ErrorCodes::cannotImportFile);
    }

    refreshData();
}

void formula::gui::SavedFilesTab::deleteFormula()
{
    auto result = AlertWindow::showYesNoCancelBox(
        MessageBoxIconType::WarningIcon,
        "Confirmation",
        "Are you sure you want to delete this formula?",
        "Yes",
        "No",
        "Cancel"
    );

    if (result != 1) return;

    auto &metadata = this->data[static_cast<unsigned int>(this->table.getSelectedRow())];
    index->deleteFormula(metadata[FormulaMetadataKeys::name]);
    refreshData();
}

void formula::gui::SavedFilesTab::selectedRowsChanged(int /*lastRowSelected*/) {
    auto bottomBarVisible = table.getSelectedRow() != -1;
    changeBottomBarVisibility(bottomBarVisible);

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
    exportButton.setBounds(Rectangle<int> {
        buttonsCenter.getX() + buttonWidthPixels / 2 + bottomButtonsMargin, buttonsArea.getY(),
            buttonWidthPixels, buttonHeightPixels
    });
}