#ifndef FORMULA_GUI_RATINGCOMPONENT_INCLUDED
#define FORMULA_GUI_RATINGCOMPONENT_INCLUDED

#include <JuceHeader.h>

namespace formula::gui {
    class RatingComponent : public juce::Component {
    public:
        explicit RatingComponent(bool isReadOnly);
        void paint(Graphics& g) override;

        void setReadOnly(bool isReadOnly);
        void setRating(double rating);
    private:
        static std::unique_ptr<juce::DrawableButton> designButton();

        std::vector<std::unique_ptr<juce::DrawableButton>> stars;
        bool isReadOnly;
    };
}

#endif //FORMULA_GUI_RATINGCOMPONENT_INCLUDED
