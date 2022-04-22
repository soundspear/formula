#ifndef FORMULA_GUI_FORMULAPOPUP_INCLUDED
#define FORMULA_GUI_FORMULAPOPUP_INCLUDED

#include <JuceHeader.h>

namespace formula::gui {
    class FormulaPopup : public juce::Component {
    public:
        void paint(Graphics& g) override {
            auto area = getLocalBounds();
            const auto backgroundColor = getLookAndFeel().findColour(ListBox::backgroundColourId);
            g.setGradientFill(ColourGradient(backgroundColor,0,0,backgroundColor.brighter(0.3f),area.getRight(), area.getBottom(), false));
            g.fillRect(area);
            g.setColour(getLookAndFeel().findColour(ListBox::outlineColourId));
            g.drawRect(area);
        }

        juce::Rectangle<int> getAreaToFit(juce::Point<int> parentCenter) {
            const auto size = getPopupSize();
            return {
                    parentCenter.getX() - size.x / 2,
                    parentCenter.getY() - size.y / 2,
                    size.x,
                    size.y
            };
        }

    protected:
        virtual juce::Point<int> getPopupSize() = 0;
    };
}

#endif //FORMULA_GUI_FORMULAPOPUP_INCLUDED
