#ifndef FORMULA_GUI_SETTINGSTAB_INCLUDED
#define FORMULA_GUI_SETTINGSTAB_INCLUDED

#include <JuceHeader.h>
#include "events/EventHub.hpp"
#include "cloud/FormulaCloudClient.hpp"

namespace formula::gui {
    class SettingsTab : public juce::Component {
    public:
        explicit SettingsTab(
            const std::shared_ptr<formula::events::EventHub>& eventHubRef,
            const std::shared_ptr<formula::cloud::FormulaCloudClient>& cloudRef
        );
        void visibilityChanged() override;
        void resized() override;
    private:
        void refreshLoginSettings();

        std::shared_ptr<formula::events::EventHub> eventHub;
        std::shared_ptr<formula::cloud::FormulaCloudClient> cloud;

        juce::Label loginLabel;
        juce::TextButton loginButton;
    };
}


#endif //FORMULA_GUI_SETTINGSTAB_INCLUDED