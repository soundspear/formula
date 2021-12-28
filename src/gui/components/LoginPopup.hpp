#ifndef FORMULA_GUI_LOGIN_POPUP_INCLUDED
#define FORMULA_GUI_LOGIN_POPUP_INCLUDED

#include <iomanip>
#include <ctime>

#include <boost/any.hpp>

#include <JuceHeader.h>

#include <events/EventHub.hpp>
#include <processor/PluginState.hpp>
#include <processor/FormulaMetadata.hpp>

namespace formula::gui
{
    enum LoginPopupType {
        Synchronize, Download
    };

    class LoginPopup : public juce::Component
    {
    public:
        LoginPopup(
            const std::shared_ptr<formula::events::EventHub>& eventHub,
            const std::shared_ptr<formula::processor::PluginState>& pluginState);
        void setType(LoginPopupType popupType);
        juce::Rectangle<int> getAreaToFit(juce::Point<int> parentCenter);
        void paint(Graphics& g) override;
        void resized() override;
    private:
        Label titleLabel;
        Label descriptionLabel;
        Label loginLabel;
        HyperlinkButton subscribeButton;
        TextButton cancelButton;

        Label emailLabel;
        Label passwordLabel;
        TextEditor emailEditor;
        TextEditor passwordEditor;
        TextButton validateLoginButton;

        int descriptionLabelHeight;

        std::shared_ptr<formula::events::EventHub> eventHub;
        std::shared_ptr<formula::processor::PluginState> pluginState;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LoginPopup)
    };
}

#endif // FORMULA_GUI_LOGIN_POPUP_INCLUDED