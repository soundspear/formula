#ifndef FORMULA_GUI_SETUSERNAMEPOPUP_INCLUDED
#define FORMULA_GUI_SETUSERNAMEPOPUP_INCLUDED

#include <JuceHeader.h>
#include <gui/components/popups/FormulaPopup.hpp>
#include "src/cloud/FormulaCloudClient.hpp"

namespace formula::gui {
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
