#ifndef FORMULA_GUI_SETTINGSTAB_INCLUDED
#define FORMULA_GUI_SETTINGSTAB_INCLUDED

#include <JuceHeader.h>
#include <events/EventHub.hpp>

namespace formula::gui {
    /**
     * Application tab that displays application settings
     */
class SettingsTab : public juce::Component, juce::ComboBox::Listener {
    public:
        explicit SettingsTab(
            const std::shared_ptr<formula::events::EventHub>& eventHubRef
        );
        void visibilityChanged() override;
        void resized() override;
    private:
        void setPossibleWindowSizes();
        void comboBoxChanged(ComboBox *comboBoxThatHasChanged) override;

        std::shared_ptr<formula::events::EventHub> eventHub;

        juce::Label windowSizeLabel;
        juce::ComboBox windowSizeComboBox;
        juce::StringArray windowSizeList;
    };
}


#endif //FORMULA_GUI_SETTINGSTAB_INCLUDED
