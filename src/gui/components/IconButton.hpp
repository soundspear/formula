/*
    Copyright (c) 2022 Soundspear.
    Distributed under the Boost Software License, Version 1.0.
    See http://www.boost.org/LICENSE_1_0.txt
*/

#ifndef FORMULA_GUI_ICONBUTTON_INCLUDED
#define FORMULA_GUI_ICONBUTTON_INCLUDED

#include <memory>

#include <JuceHeader.h>

namespace formula::gui
{
    /**
     * A square button with a coloured SVG icon and a tooltip
     */
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
            const auto idleColour = laf.findColour(static_cast<int>(ColourIds::idleColourId));
            const auto highlightedColour = laf.findColour(static_cast<int>(ColourIds::highlightedColourId));
            const auto downColour = laf.findColour(static_cast<int>(ColourIds::downColourId));
            const auto toggledColour = laf.findColour(static_cast<int>(ColourIds::toggledColourId));

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