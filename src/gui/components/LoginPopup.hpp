#ifndef FORMULA_GUI_LOGIN_POPUP_INCLUDED
#define FORMULA_GUI_LOGIN_POPUP_INCLUDED

#include <iomanip>
#include <ctime>

#include <boost/any.hpp>

#include <JuceHeader.h>

#include <events/EventHub.hpp>
#include <cloud/FormulaCloudClient.hpp>

namespace formula::gui
{
    enum LoginPopupType {
        FirstLogin, LoginFailed, MissingSubscription
    };

    class LoginPopup : public juce::Component
    {
    public:
        LoginPopup(const std::shared_ptr<formula::cloud::FormulaCloudClient>& cloudRef);
        void setType(LoginPopupType popupType);
        juce::Rectangle<int> getAreaToFit(juce::Point<int> parentCenter);

        void paint(Graphics& g) override;
        void resized() override;
    private:
        LoginPopupType popupType;

        Label titleLabel;
        Label descriptionLabel;
        HyperlinkButton linkButton;
        std::unique_ptr<juce::Button> closeButton;

        Label emailLabel;
        Label passwordLabel;
        TextEditor emailEditor;
        TextEditor passwordEditor;
        TextButton validateButton;

        int descriptionLabelHeight = 0;

        std::shared_ptr<formula::cloud::FormulaCloudClient> cloud;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LoginPopup)
    };
}

#endif // FORMULA_GUI_LOGIN_POPUP_INCLUDED