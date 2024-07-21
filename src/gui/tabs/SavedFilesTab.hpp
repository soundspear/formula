#ifndef FORMULA_GUI_SAVEDFILESTAB_INCLUDED
#define FORMULA_GUI_SAVEDFILESTAB_INCLUDED

#include <vector>
#include <algorithm>
#include <fstream>

#include <boost/any.hpp>
#include <boost/assign/std/vector.hpp>

#include <JuceHeader.h>

#include <gui/components/IconButton.hpp>
#include <gui/ErrorCodes.hpp>
#include <events/EventHub.hpp>
#include <processor/PluginState.hpp>
#include <processor/FormulaMetadata.hpp>
#include <storage/LocalIndex.hpp>
#include <gui/tabs/FormulaListTabBase.hpp>
#include <storage/UserIndex.hpp>

namespace formula::gui
{
    /**
     * Application tab that displays locally saved formulas
     */
    class SavedFilesTab : public FormulaListTabBase
    {
    public:
        SavedFilesTab(
            const std::shared_ptr<formula::events::EventHub>& eventHubRef,
            const std::shared_ptr<formula::processor::PluginState>& pluginStateRef,
            const std::shared_ptr<formula::storage::UserIndex>& localIndexRef);

        void refreshData() override;
        void changeBottomBarVisibility(bool visible);
        void exportFormulaToFile();
        void importFormulaFromFile();
        void deleteFormula();
        void publishFormula();
        void askOverwriteFormula(std::string formulaId, formula::processor::FormulaMetadata metadata);

        void selectedRowsChanged(int lastRowSelected) override;
        void resized() override;

    private:
        TextButton importButton;
        TextButton loadButton;
        TextButton exportButton;
        TextButton deleteButton;

        std::shared_ptr<formula::events::EventHub> eventHub;
        std::shared_ptr<formula::processor::PluginState> pluginState;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SavedFilesTab)
    };
}

#endif // FORMULA_GUI_SAVEDFILESTAB_INCLUDED