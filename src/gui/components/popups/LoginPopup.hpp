/*
    Copyright (c) 2022 Soundspear.
    Distributed under the Boost Software License, Version 1.0.
    See http://www.boost.org/LICENSE_1_0.txt
*/

#ifndef FORMULA_GUI_LOGINPOPUP_INCLUDED
#define FORMULA_GUI_LOGINPOPUP_INCLUDED

#include <iomanip>
#include <ctime>

#include <boost/any.hpp>

#include "JuceHeader.h"

#include "src/gui/components/popups/FormulaPopup.hpp"
#include "src/events/EventHub.hpp"
#include "src/cloud/FormulaCloudClient.hpp"

namespace formula::gui
{
    enum LoginPopupType {
        FirstLogin, LoginFailed, MissingSubscription
    };

    /**
     * Displays a popup that asks for Formula Cloud login information
     */
    class LoginPopup : public formula::gui::FormulaPopup
    {
    public:
        explicit LoginPopup(const std::shared_ptr<formula::cloud::FormulaCloudClient>& cloudRef);
        void setType(LoginPopupType popupType);

        juce::Point<int> getPopupSize() override;
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

#endif // FORMULA_GUI_LOGINPOPUP_INCLUDED