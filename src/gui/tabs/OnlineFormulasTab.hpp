#ifndef FORMULA_GUI_ONLINEFORMULASTAB_INCLUDED
#define FORMULA_GUI_ONLINEFORMULASTAB_INCLUDED

#include <mutex>
#include <boost/assign/std/vector.hpp>
#include <boost/any.hpp>

#include <JuceHeader.h>

#include <gui/components/LoginPopup.hpp>
#include <events/EventHub.hpp>
#include <cloud/FormulaCloudClient.hpp>
#include <cloud/ListFormulaDto.hpp>
#include "cloud/SearchParameters.hpp"
#include "gui/components/FormulaDetailsPanel.hpp"

namespace formula::gui {
    class OnlineFormulasTab : public juce::Component, public juce::TableListBoxModel, public ScrollBar::Listener {
    public:
        OnlineFormulasTab(const std::shared_ptr<formula::events::EventHub>& eventHub,
                          const std::shared_ptr<formula::cloud::FormulaCloudClient>& cloud);
        ~OnlineFormulasTab() = default;;

        int getNumRows() override;
        void paintRowBackground(Graphics& g, int rowNumber, int, int, bool rowIsSelected) override;
        void paintCell(Graphics& g, int rowNumber, int columnId, int width, int height, bool) override;
        void sortOrderChanged(int newSortColumnId, bool isForwards) override;
        Component* refreshComponentForCell(int rowNumber, int columnId, bool, Component* existingComponentToUpdate) override;
        void selectedRowsChanged(int lastRowSelected) override;
        void resized() override;
        void visibilityChanged() override;
    private:
        void makeSearchAsync();

        enum OnlineFormulasColumnsIds {
            name = 1, author, rating, description, created, lastModified
        };

        std::shared_ptr<formula::cloud::FormulaCloudClient> cloud;
        std::shared_ptr<formula::events::EventHub> eventHub;

        formula::cloud::SearchParameters searchParams;
        std::vector<formula::cloud::ListFormulaDto> data;
        TableListBox table;
        formula::gui::FormulaDetailsPanel detailsPanel;

        bool endOfResultsReached;
    };
}

#endif //FORMULA_GUI_ONLINEFORMULASTAB_INCLUDED
