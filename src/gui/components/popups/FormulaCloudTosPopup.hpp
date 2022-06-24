/*
    Copyright (c) 2022 Soundspear.
    Distributed under the Boost Software License, Version 1.0.
    See http://www.boost.org/LICENSE_1_0.txt
*/

#ifndef FORMULA_GUI_FORMULACLOUDTOSPOPUP_INCLUDED
#define FORMULA_GUI_FORMULACLOUDTOSPOPUP_INCLUDED

#include "JuceHeader.h"

#include "src/gui/components/popups/FormulaPopup.hpp"

namespace formula::gui {
    class FormulaCloudTosPopup : public formula::gui::FormulaPopup {
    public:
        FormulaCloudTosPopup() {
            auto tosContent = juce::String(formula::binary::formula_cloud_tos_txt,
                                           formula::binary::formula_cloud_tos_txtSize);
            tosContentEditor.setReadOnly(true);
            tosContentEditor.setMultiLine(true);
            tosContentEditor.setText(tosContent);
            addAndMakeVisible(tosContentEditor);

            closeButton = std::unique_ptr<Button>(getLookAndFeel().createDocumentWindowButton(DocumentWindow::TitleBarButtons::closeButton));
            addAndMakeVisible(closeButton.get());
            closeButton->onClick = [this]() {
                this->setVisible(false);
            };
        }

        juce::Point<int> getPopupSize() override {
            return { 600, 350 };
        }

        void resized() override {
            constexpr auto closeButtonSize = 32;
            constexpr auto pad = 8;
            constexpr auto borderSize = 1;

            auto area = getLocalBounds();

            closeButton->setBounds(area.getX() + area.getWidth() - closeButtonSize - borderSize, borderSize,
                                   closeButtonSize, closeButtonSize);

            area = area.withTrimmedLeft(pad).withTrimmedRight(pad)
                    .withTrimmedTop(pad).withTrimmedBottom(pad);

            tosContentEditor.setBounds(area);
        };
    private:
        std::unique_ptr<Button> closeButton;
        juce::TextEditor tosContentEditor;
    };
}

#endif //FORMULA_GUI_FORMULACLOUDTOSPOPUP_INCLUDED
