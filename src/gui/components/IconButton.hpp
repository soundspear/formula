#ifndef FORMULA_GUI_ICONBUTTON_INCLUDED
#define FORMULA_GUI_ICONBUTTON_INCLUDED

#include <memory>

#include <JuceHeader.h>

namespace formula::gui
{
	class IconButton : public DrawableButton {
	public:
		IconButton()
			: DrawableButton("", DrawableButton::ButtonStyle::ImageFitted) {}

        enum ColourIds : unsigned int
        {
            idleColourId = 0xf0f00100,
            highlightedColourId = 0xf0f00101,
            downColourId = 0xf0f00102,
            toggledColourId = 0xf0f00103
        };

		void setImage(const char* normalImageSvgData, Colour colour) {
			normalImageSvgSource = XmlDocument::parse(normalImageSvgData);
			normalImageSvgDrawable = juce::Drawable::createFromSVG(*normalImageSvgSource);
            normalImageSvgDrawable->replaceColour(Colours::black, colour);

			DrawableButton::setImages(normalImageSvgDrawable.get());
		}

		void paintButton(Graphics& g,
			bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override {
            const auto& laf = getLookAndFeel();
            const auto idleColour = laf.findColour(ColourIds::idleColourId);
            const auto highlightedColour = laf.findColour(ColourIds::highlightedColourId);
            const auto downColour = laf.findColour(ColourIds::downColourId);
            const auto toggledColour = laf.findColour(ColourIds::toggledColourId);

			auto localBounds = getLocalBounds().toFloat();
            g.fillAll(idleColour);

            if (shouldDrawButtonAsDown) {
                g.setColour(downColour);
                g.fillRoundedRectangle(localBounds, 3.f);
            }
            else if (getToggleState()) {
                g.setColour(toggledColour);
                g.fillRoundedRectangle(localBounds, 3.f);
            }
            else if (shouldDrawButtonAsHighlighted) {
                g.setColour(highlightedColour);
                g.fillRoundedRectangle(localBounds, 3.f);
            }
		}
	private:
		std::unique_ptr<XmlElement> normalImageSvgSource;
		std::unique_ptr<juce::Drawable> normalImageSvgDrawable;
	};
}

#endif // FORMULA_GUI_ICONBUTTON_INCLUDED