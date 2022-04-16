#ifndef FORMULA_GUI_FORMULADETAILSPANEL_INCLUDED
#define FORMULA_GUI_FORMULADETAILSPANEL_INCLUDED

#include <JuceHeader.h>
#include <events/EventHub.hpp>
#include <cloud/GetFormulaDto.hpp>
#include <gui/components/RatingComponent.hpp>

namespace formula::gui {
    class FormulaDetailsPanel : public juce::Component {
    public:
        explicit FormulaDetailsPanel();
        void setFormulaDto(formula::cloud::GetFormulaDto dto);

        void paint(Graphics& g) override;
        void resized() override;
    private:
        cloud::GetFormulaDto dto;

        Font nameFont, authorFont, descriptionFont;

        std::unique_ptr<Button> closeButton;
        Label nameLabel;
        Label authorLabel;
        CodeDocument codePreview;
        std::unique_ptr<CodeEditorComponent> codePreviewEditor;
        CPlusPlusCodeTokeniser cppTokeniser;
        Label descriptionLabel;
        Label ratingsHeadingLabel;
        TextButton loadFormulaButton;
        std::vector<std::unique_ptr<RatingComponent>> ratingComponents;
        std::vector<std::unique_ptr<Label>> commentLabels;
    };
}

#endif //FORMULA_GUI_FORMULADETAILSPANEL_INCLUDED
