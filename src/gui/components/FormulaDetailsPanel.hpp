/*
    Copyright (c) 2022 Soundspear.
    Distributed under the Boost Software License, Version 1.0.
    See http://www.boost.org/LICENSE_1_0.txt
*/

#ifndef FORMULA_GUI_FORMULADETAILSPANEL_INCLUDED
#define FORMULA_GUI_FORMULADETAILSPANEL_INCLUDED

#include <JuceHeader.h>
#include <events/EventHub.hpp>
#include <processor/FormulaMetadata.hpp>
#include <gui/components/FormulaCodeEditor.hpp>

namespace formula::gui {
    /**
     * Collapsible panel to display the preview of a formula
     */
    class FormulaDetailsPanel : public juce::Component {
    public:
        explicit FormulaDetailsPanel(const std::shared_ptr<formula::events::EventHub>& eventHubRef);
        void setFormula(const formula::processor::FormulaMetadata& newMetadata);

        void paint(Graphics& g) override;
        void resized() override;
    private:
        formula::processor::FormulaMetadata metadata;
        std::shared_ptr<formula::events::EventHub> eventHub;

        Font nameFont, authorFont, descriptionFont;

        std::unique_ptr<Button> closeButton;
        Label nameLabel;
        CodeDocument codePreview;
        std::unique_ptr<formula::gui::FormulaCodeEditor> codePreviewEditor;
        Label descriptionLabel;
        Label ratingsHeadingLabel;
        TextButton loadFormulaButton;
    };
}

#endif //FORMULA_GUI_FORMULADETAILSPANEL_INCLUDED
