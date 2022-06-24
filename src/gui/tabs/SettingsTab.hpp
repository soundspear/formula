#ifndef FORMULA_GUI_SETTINGSTAB_INCLUDED
#define FORMULA_GUI_SETTINGSTAB_INCLUDED

#include <JuceHeader.h>
#include "events/EventHub.hpp"
#include "cloud/FormulaCloudClient.hpp"
#include "gui/components/popups/FormulaCloudTosPopup.hpp"

namespace formula::gui {
class SettingsTab : public juce::Component, juce::ComboBox::Listener {
    public:
        explicit SettingsTab(
            const std::shared_ptr<formula::events::EventHub>& eventHubRef,
            const std::shared_ptr<formula::cloud::FormulaCloudClient>& cloudRef
        );
        void visibilityChanged() override;
        void resized() override;
    private:
        void refreshLoginSettings();
        void setPossibleWindowSizes();
        void comboBoxChanged(ComboBox *comboBoxThatHasChanged) override;

        std::shared_ptr<formula::events::EventHub> eventHub;
        std::shared_ptr<formula::cloud::FormulaCloudClient> cloud;

        juce::Label loginLabel;
        juce::TextButton loginButton;

        juce::TextButton formulaCloudTosButton;
        formula::gui::FormulaCloudTosPopup tosPopup;

        juce::Label windowSizeLabel;
        juce::ComboBox windowSizeComboBox;
        juce::StringArray windowSizeList;
    };
}


#endif //FORMULA_GUI_SETTINGSTAB_INCLUDED
