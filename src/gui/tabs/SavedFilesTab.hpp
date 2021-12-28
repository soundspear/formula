#ifndef FORMULA_GUI_SAVED_FILES_TAB_INCLUDED
#define FORMULA_GUI_SAVED_FILES_TAB_INCLUDED

#include <vector>
#include <algorithm>

#include <boost/any.hpp>
#include <boost/assign/std/vector.hpp>

#include <JuceHeader.h>

#include <gui/components/SvgButton.hpp>
#include <gui/components/LoginPopup.hpp>
#include <events/EventHub.hpp>
#include <processor/PluginState.hpp>
#include <processor/FormulaMetadata.hpp>
#include <storage/LocalIndex.hpp>

namespace formula::gui
{
    class SavedFilesTab : public juce::Component, public TableListBoxModel
    {
    public:
        SavedFilesTab(
            const std::shared_ptr<formula::events::EventHub>& eventHub,
            const std::shared_ptr<formula::processor::PluginState>& pluginState);

        void refreshData();
        void changeBottomBarVisibility(bool visible);
        void exportFormulaToFile();
        void importFormulaFromFile();
        void deleteFormula();

        int getNumRows() override;
        void paintRowBackground(Graphics& g, int rowNumber, int, int, bool rowIsSelected) override;
        void paintCell(Graphics& g, int rowNumber, int columnId, int width, int height, bool) override;
        void sortOrderChanged(int newSortColumnId, bool isForwards) override;
        Component* refreshComponentForCell(int rowNumber, int columnId, bool, Component* existingComponentToUpdate) override;
        void selectedRowsChanged(int lastRowSelected) override;
        void resized() override;
        void visibilityChanged() override;

    private:
        enum SavedFileColumnsIds {
            source = 1, name = 2, created = 3, lastModified = 4, description = 5
        };

        std::vector<formula::processor::FormulaMetadata> data;
        TableListBox table;

        TextButton importButton;
        TextButton loadButton;
        TextButton publishButton;
        TextButton exportButton;
        TextButton deleteButton;

        formula::gui::LoginPopup loginPopup;

        std::shared_ptr<formula::events::EventHub> eventHub;
        std::shared_ptr<formula::processor::PluginState> pluginState;
        std::shared_ptr<formula::storage::LocalIndex> localIndex;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SavedFilesTab)
    };
}

#endif // FORMULA_GUI_SAVED_FILES_TAB_INCLUDED