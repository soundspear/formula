/*
    Copyright (c) 2022 Soundspear.
    Distributed under the Boost Software License, Version 1.0.
    See http://www.boost.org/LICENSE_1_0.txt
*/

#ifndef FORMULA_GUI_NOCOMPILERFOUNDPOPUP_INCLUDED
#define FORMULA_GUI_NOCOMPILERFOUNDPOPUP_INCLUDED

#include <JuceHeader.h>
#include <gui/components/popups/FormulaPopup.hpp>

namespace formula::gui {
    class NoCompilerFoundPopup : public formula::gui::FormulaPopup {
    public:
        NoCompilerFoundPopup();

        juce::Point<int> getPopupSize() override;
        void resized() override;
    private:
        juce::Label title;
        juce::Label topDescriptionLabel;
        juce::Label bottomDescriptionLabel;
        juce::HyperlinkButton downloadLink;

        juce::Font titleFont;
    };
}


#endif //FORMULA_GUI_NOCOMPILERFOUNDPOPUP_INCLUDED
