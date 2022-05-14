#ifndef FORMULA_GUI_FORMULALOOKANDFEEL_INCLUDED
#define FORMULA_GUI_FORMULALOOKANDFEEL_INCLUDED

#include <JuceHeader.h>

namespace formula::gui {
    class FormulaLookAndFeel : public juce::LookAndFeel_V4
    {
    public:
        FormulaLookAndFeel()
        : LookAndFeel_V4(createColourScheme()) {
            std::vector<std::pair<int, juce::uint32>> colorMapping {
                    {juce::CodeEditorComponent::ColourIds::backgroundColourId,     0x2b2b2b },
                    {juce::CodeEditorComponent::ColourIds::lineNumberBackgroundId, 0x313335 },
                    {juce::TabbedComponent::ColourIds::backgroundColourId,         backgroundColor },
                    {juce::TextButton::ColourIds::buttonColourId,                  backgroundColor },
                    {juce::TextButton::ColourIds::buttonOnColourId,                darker(backgroundColor, 0.5f) },
                    {juce::ListBox::ColourIds::backgroundColourId,                 backgroundColor },
                    {IconButton::ColourIds::idleColourId,                          backgroundColor },
                    {IconButton::ColourIds::highlightedColourId,                   brighter(backgroundColor, 0.2f) },
                    {IconButton::ColourIds::downColourId,                          brighter(backgroundColor, 0.3f) },
                    {IconButton::ColourIds::toggledColourId,                       darker(backgroundColor, 0.5f) },
                    {AlertWindow::ColourIds::backgroundColourId,                   darker(backgroundColor, 0.3f) },
            };
            this->mapColors(colorMapping);
        }

        void drawTabButton(TabBarButton& button,
            Graphics& g,
            bool 	isMouseOver,
            bool 	isMouseDown
        ) override {
            const juce::Rectangle<int> activeArea(button.getActiveArea());

            const TabbedButtonBar::Orientation o = button.getTabbedButtonBar().getOrientation();

            // Draw background
            Colour bkg(getCurrentColourScheme().getUIColour(ColourScheme::widgetBackground));

            if (button.getToggleState())
            {
                g.setColour(bkg.brighter(0.25f));
            }
            else
            {
                g.setColour(bkg.brighter(0.10f));
            }

            g.fillRect(activeArea);

            // Draw text

            const float alpha = button.isEnabled() ? ((isMouseOver || isMouseDown) ? 1.0f : 0.8f) : 0.3f;

            Colour col(bkg.contrasting().withMultipliedAlpha(alpha));
            

            const juce::Rectangle<float> area(button.getTextArea().toFloat());

            float length = area.getWidth();
            float depth = area.getHeight();

            if (button.getTabbedButtonBar().isVertical())
                std::swap(length, depth);

            TextLayout textLayout;
            createTabTextLayout(button, length, depth, col, textLayout);

            AffineTransform t;

            switch (o)
            {
            case TabbedButtonBar::TabsAtLeft:   t = t.rotated(MathConstants<float>::pi * -0.5f).translated(area.getX(), area.getBottom()); break;
            case TabbedButtonBar::TabsAtRight:  t = t.rotated(MathConstants<float>::pi * 0.5f).translated(area.getRight(), area.getY()); break;
            case TabbedButtonBar::TabsAtTop:
            case TabbedButtonBar::TabsAtBottom: t = t.translated(area.getX(), area.getY()); break;
            default:                            jassertfalse; break;
            }

            g.addTransform(t);
            textLayout.draw(g, juce::Rectangle<float>(length, depth));
        }

        static ColourScheme createColourScheme() {
            return { 0xff000000 | backgroundColor, 0xff000000 | darker(backgroundColor, 0.3f), 0xff000000 | brighter(backgroundColor, 0.3f),
             0xff666666, 0xffffffff, 0xffba7272,
             0xff000000, 0xffffffff, 0xffffffff };
        }

    private:
        static constexpr juce::uint32 brighter(juce::uint32 color, float amount) {
            juce::uint32 result = 0;
            amount = 1.0f / (1.0f + amount);
            for (int i = 0; i < 3; i++) {
                auto offset = i * 8;
                auto mask = 0xff << offset;
                auto channel = 255 - ((color & mask) >> offset);
                channel = static_cast<int>(amount * channel);
                result |= (255 - channel) << offset;
            }
            return result;
        }

        static constexpr juce::uint32 darker(juce::uint32 color, float amount) {
            juce::uint32 result = 0;
            amount = 1.0f / (1.0f + amount);
            for (int i = 0; i < 3; i++) {
                auto offset = i * 8;
                auto mask = 0xff << offset;
                auto channel = (color & mask) >> offset;
                channel = static_cast<int>(amount * channel);
                result |= channel << offset;
            }
            return result;
        }

        void mapColors(std::vector<std::pair<int, juce::uint32>> colorMapping) {
            for (auto& pair : colorMapping) {
                juce::uint32 colourHex = (0xff << 24) | std::get<1>(pair);
                setColour(std::get<0>(pair), juce::Colour(colourHex));
            }
        }

        static constexpr juce::uint32 backgroundColor = 0x3c3f41;
    };
}

#endif //FORMULA_GUI_FORMULALOOKANDFEEL_INCLUDED
