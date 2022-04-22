#ifndef FORMULA_GUI_SAVE_LOCAL_FORMULA_POPUP_INCLUDED
#define FORMULA_GUI_SAVE_LOCAL_FORMULA_POPUP_INCLUDED

#include <iomanip>
#include <ctime>

#include <boost/any.hpp>

#include "JuceHeader.h"

#include "src/gui/components/popups/FormulaPopup.hpp"
#include "src/events/EventHub.hpp"
#include "src/storage/LocalIndex.hpp"
#include "src/processor/PluginState.hpp"
#include "src/processor/FormulaMetadata.hpp"

namespace formula::gui
{
    class SaveLocalFormulaPopup : public formula::gui::FormulaPopup
    {
    public:
        SaveLocalFormulaPopup(
            const std::shared_ptr<formula::storage::LocalIndex>& localIndexRef,
            const std::shared_ptr<formula::processor::PluginState>& pluginStateRef);

        juce::Point<int> getPopupSize() override;
        void resized() override;
        void resetContent();
        void visibilityChanged() override;
    private:
        Label titleLabel;
        Label nameLabel;
        Label descriptionLabel;
        TextEditor nameEditor;
        TextEditor descriptionEditor;
        TextButton saveButton;
        TextButton cancelButton;

        std::shared_ptr<formula::storage::LocalIndex> localIndex;
        std::shared_ptr<formula::processor::PluginState> pluginState;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SaveLocalFormulaPopup)
    };
}

#endif // FORMULA_GUI_SAVE_LOCAL_FORMULA_POPUP_INCLUDED