#ifndef FORMULA_GUI_ONLINEFORMULASTAB_INCLUDED
#define FORMULA_GUI_ONLINEFORMULASTAB_INCLUDED

#include <mutex>
#include <boost/assign/std/vector.hpp>
#include <boost/any.hpp>

#include <gui/components/SearchBar.hpp>
#include <events/EventHub.hpp>
#include <http/SearchParameters.hpp>
#include <storage/CommunityIndex.hpp>
#include <gui/components/FormulaDetailsPanel.hpp>
#include <gui/tabs/FormulaListTabBase.hpp>

namespace formula::gui {
    /**
     * Application tab that displays stock Formulas from the Community
     */
    class OnlineFormulasTab : public FormulaListTabBase {
    public:
        OnlineFormulasTab(
            const std::shared_ptr<formula::events::EventHub>& eventHubRef,
            const std::shared_ptr<formula::storage::CommunityIndex>& communityIndexRef
        );
        ~OnlineFormulasTab() override;

        void refreshData() override;
        void selectedRowsChanged(int lastRowSelected) override;
        void resized() override;
    private:
        std::shared_ptr<formula::events::EventHub> eventHub;

        std::string sortColumn, sortDirection;

        formula::gui::FormulaDetailsPanel detailsPanel;
        formula::gui::SearchBar searchBar;
    };
}

#endif //FORMULA_GUI_ONLINEFORMULASTAB_INCLUDED
