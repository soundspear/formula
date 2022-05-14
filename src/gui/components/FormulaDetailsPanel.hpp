#ifndef FORMULA_GUI_FORMULADETAILSPANEL_INCLUDED
#define FORMULA_GUI_FORMULADETAILSPANEL_INCLUDED

#include <JuceHeader.h>
#include <events/EventHub.hpp>
#include <cloud/GetFormulaDto.hpp>
#include <processor/FormulaMetadata.hpp>
#include <gui/components/RatingComponent.hpp>
#include <gui/components/FormulaCodeEditor.hpp>

namespace formula::gui {
    class FormulaDetailsPanel : public juce::Component {
    public:
        explicit FormulaDetailsPanel(const std::shared_ptr<formula::events::EventHub>& eventHubRef);
        void setFormulaDto(formula::cloud::GetFormulaDto dto);

        void paint(Graphics& g) override;
        void resized() override;
    private:
        cloud::GetFormulaDto dto;
        std::shared_ptr<formula::events::EventHub> eventHub;

        Font nameFont, authorFont, descriptionFont;

        std::unique_ptr<Button> closeButton;
        Label nameLabel;
        Label authorLabel;
        CodeDocument codePreview;
        std::unique_ptr<formula::gui::FormulaCodeEditor> codePreviewEditor;
        Label descriptionLabel;
        Label ratingsHeadingLabel;
        TextButton loadFormulaButton;
        std::vector<std::unique_ptr<RatingComponent>> ratingComponents;
        std::vector<std::unique_ptr<Label>> commentLabels;
    };
}

#endif //FORMULA_GUI_FORMULADETAILSPANEL_INCLUDED
