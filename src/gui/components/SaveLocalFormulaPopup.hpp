#ifndef FORMULA_GUI_SAVE_LOCAL_FORMULA_POPUP_INCLUDED
#define FORMULA_GUI_SAVE_LOCAL_FORMULA_POPUP_INCLUDED

#include <iomanip>
#include <ctime>

#include <boost/any.hpp>

#include <JuceHeader.h>

#include <events/EventHub.hpp>
#include <processor/PluginState.hpp>
#include <processor/FormulaMetadata.hpp>

namespace formula::gui
{
    class SaveLocalFormulaPopup : public juce::Component
    {
    public:
        SaveLocalFormulaPopup(
            const std::shared_ptr<formula::events::EventHub>& eventHub,
            const std::shared_ptr<formula::processor::PluginState>& pluginState);
        void paint(Graphics& g) override;
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

        std::shared_ptr<formula::events::EventHub> eventHub;
        std::shared_ptr<formula::processor::PluginState> pluginState;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SaveLocalFormulaPopup)
    };
}

#endif // FORMULA_GUI_SAVE_LOCAL_FORMULA_POPUP_INCLUDED