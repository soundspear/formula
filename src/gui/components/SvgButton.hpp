#ifndef FORMULA_GUI_SVG_BUTTON_INCLUDED
#define FORMULA_GUI_SVG_BUTTON_INCLUDED

#include <memory>

#include <JuceHeader.h>

namespace formula::gui
{
	class FormulaButton : public DrawableButton {
	public:
		FormulaButton()
			: DrawableButton("", DrawableButton::ButtonStyle::ImageFitted) {}

		FormulaButton(const String& name, const DrawableButton::ButtonStyle buttonStyle) 
		: DrawableButton(name, buttonStyle) {}

		void setImages(const char* normalImageSvgData, const char* overImageSvgData = nullptr,
			const char* downImageSvgData = nullptr, const char* disabledImageSvgData = nullptr,
			const char* normalImageOnSvgData = nullptr, const char* overImageOnSvgData = nullptr,
			const char* downImageOnSvgData = nullptr, const char* disabledImageOnSvgData = nullptr) {
			normalImageSvgSource = XmlDocument::parse(normalImageSvgData);
			normalImageSvgDrawable = juce::Drawable::createFromSVG(*normalImageSvgSource);
			if (overImageSvgData) {
				overImageSvgSource = XmlDocument::parse(overImageSvgData);
				overImageSvgDrawable = juce::Drawable::createFromSVG(*overImageSvgSource);
			}
			if (downImageSvgData) {
				downImageSvgSource = XmlDocument::parse(downImageSvgData);
				downImageSvgDrawable = juce::Drawable::createFromSVG(*downImageSvgSource);
			}
			if (disabledImageSvgData) {
				disabledImageSvgSource = XmlDocument::parse(disabledImageSvgData);
				disabledImageSvgDrawable = juce::Drawable::createFromSVG(*disabledImageSvgSource);
			}
			if (normalImageOnSvgData) {
				normalImageOnSvgSource = XmlDocument::parse(normalImageOnSvgData);
				normalImageOnSvgDrawable = juce::Drawable::createFromSVG(*normalImageOnSvgSource);
			}
			if (overImageOnSvgData) {
				overImageOnSvgSource = XmlDocument::parse(overImageOnSvgData);
				overImageOnSvgDrawable = juce::Drawable::createFromSVG(*overImageOnSvgSource);
			}
			if (downImageOnSvgData) {
				downImageOnSvgSource = XmlDocument::parse(downImageOnSvgData);
				downImageOnSvgDrawable = juce::Drawable::createFromSVG(*downImageOnSvgSource);
			}
			if (disabledImageOnSvgData) {
				disabledImageOnSvgSource = XmlDocument::parse(disabledImageOnSvgData);
				disabledImageOnSvgDrawable = juce::Drawable::createFromSVG(*disabledImageOnSvgSource);
			}
			DrawableButton::setImages(normalImageSvgDrawable.get(),
				overImageOnSvgDrawable.get(),
				downImageOnSvgDrawable.get(),
				disabledImageOnSvgDrawable.get(),
				normalImageOnSvgDrawable.get(),
				overImageOnSvgDrawable.get(),
				downImageOnSvgDrawable.get(),
				disabledImageOnSvgDrawable.get()
			);
		}
		
		void paintButton(Graphics& g,
			bool /*shouldDrawButtonAsHighlighted*/, bool /*shouldDrawButtonAsDown*/ ) override {
			auto localBounds = getLocalBounds().toFloat();
			g.fillAll(Colours::red);
			g.drawRoundedRectangle(localBounds, 3.f, 1.f);
		}
	private:
		std::unique_ptr<XmlElement> normalImageSvgSource;
		std::unique_ptr<juce::Drawable> normalImageSvgDrawable;
		std::unique_ptr<XmlElement> overImageSvgSource;
		std::unique_ptr<juce::Drawable> overImageSvgDrawable;
		std::unique_ptr<XmlElement> downImageSvgSource;
		std::unique_ptr<juce::Drawable> downImageSvgDrawable;
		std::unique_ptr<XmlElement> disabledImageSvgSource;
		std::unique_ptr<juce::Drawable> disabledImageSvgDrawable;
		std::unique_ptr<XmlElement> normalImageOnSvgSource;
		std::unique_ptr<juce::Drawable> normalImageOnSvgDrawable;
		std::unique_ptr<XmlElement> overImageOnSvgSource;
		std::unique_ptr<juce::Drawable> overImageOnSvgDrawable;
		std::unique_ptr<XmlElement> downImageOnSvgSource;
		std::unique_ptr<juce::Drawable> downImageOnSvgDrawable;
		std::unique_ptr<XmlElement> disabledImageOnSvgSource;
		std::unique_ptr<juce::Drawable> disabledImageOnSvgDrawable;
	};
}

#endif // FORMULA_GUI_SVG_BUTTON_INCLUDED