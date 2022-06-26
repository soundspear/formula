/*
    Copyright (c) 2022 Soundspear.
    Distributed under the Boost Software License, Version 1.0.
    See http://www.boost.org/LICENSE_1_0.txt
*/

#ifndef FORMULA_GUI_SETUSERNAMEPOPUP_INCLUDED
#define FORMULA_GUI_SETUSERNAMEPOPUP_INCLUDED

#include <JuceHeader.h>
#include <gui/components/popups/FormulaPopup.hpp>
#include "src/cloud/FormulaCloudClient.hpp"

namespace formula::gui {
    /**
     * Ask the user to choose an username
     */
    class SetUserNamePopup : public formula::gui::FormulaPopup {
    public:
        explicit SetUserNamePopup(const std::shared_ptr<formula::cloud::FormulaCloudClient>& cloudRef);

        void setUserNameAlreadyExists(bool userNameAlreadyExists);

        juce::Point<int> getPopupSize() override;
        void resized() override;
    private:
        std::shared_ptr<formula::cloud::FormulaCloudClient> cloud;

        juce::Label titleLabel;
        juce::Label descriptionLabel;
        TextEditor nameEditor;
        TextButton okButton;

        juce::Font titleFont;
    };
}


#endif //FORMULA_GUI_SETUSERNAMEPOPUP_INCLUDED
