#ifndef FORMULA_GUI_COMPONENTS_KNOB_PANEL_ELEMENT_INCLUDED
#define FORMULA_GUI_COMPONENTS_KNOB_PANEL_ELEMENT_INCLUDED

#include <string>
#include <array>

#include <JuceHeader.h>

#include <processor/PluginState.hpp>
#include <processor/FormulaMetadata.hpp>

namespace formula::gui {
    template < 
        typename ComponentType,
        typename ValueType,
        typename std::enable_if<
            std::is_base_of<juce::Component, ComponentType>::value
        >::type* = nullptr
    >
    class KnobPanelElement : public Component
    {
    public:
        KnobPanelElement(bool isReadOnly = false)
        : isReadOnly(isReadOnly) {
            addAndMakeVisible(innerElement);
            addAndMakeVisible(nameEditor);
            nameEditor.setJustification(juce::Justification::centred);
            nameEditor.setColour(TextEditor::ColourIds::outlineColourId, Colours::transparentBlack);
            nameEditor.setColour(TextEditor::ColourIds::backgroundColourId, Colours::transparentBlack);
            nameEditor.setColour(TextEditor::ColourIds::shadowColourId, Colours::transparentBlack);
            if (isReadOnly) {
                nameEditor.setReadOnly(true);
                nameEditor.setColour(TextEditor::ColourIds::focusedOutlineColourId, Colours::transparentBlack);
            }
        }

        void paint(Graphics& g) override {
            if (!isReadOnly) {
                g.setColour(getLookAndFeel().findColour(ListBox::backgroundColourId).brighter(0.05f));
                g.fillRoundedRectangle(getLocalBounds().toFloat(), 10.f);
            }
            g.setColour(getLookAndFeel().findColour(ListBox::backgroundColourId).brighter(0.1f));
            g.drawRoundedRectangle(getLocalBounds().toFloat(), 10.f, 2.0f);
        }

        void setNameLabel(std::string name) {
            nameEditor.setText(juce::String(name));
        }

        ComponentType& inner() { return innerElement; }
        juce::TextEditor& nameTextEditor() { return nameEditor; }

        std::string getName() const
        {
            return nameEditor.getText().toStdString();
        }

    protected:
        ComponentType innerElement;
        juce::TextEditor nameEditor;
        bool isReadOnly;

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(KnobPanelElement)
    };

    class KnobPanelSlider : public KnobPanelElement<juce::Slider, double>
    {
    public:
        KnobPanelSlider() : KnobPanelElement<juce::Slider, double>() {
            setSliderProperties();
        };
        KnobPanelSlider(bool isReadOnly) : KnobPanelElement<juce::Slider, double>(isReadOnly) {
            setSliderProperties();
        };
        KnobPanelSlider(bool isReadOnly, Slider::SliderStyle sliderStyle) : KnobPanelElement<juce::Slider, double>(isReadOnly) {
            setSliderProperties();
            innerElement.setSliderStyle(sliderStyle);
        };

        void resized() override
        {
            constexpr auto bottomMargin = 8;
            constexpr auto nameMargin = 6;

            auto area = getLocalBounds();
            area.removeFromBottom(bottomMargin);

            nameEditor.setBounds(area.removeFromBottom(18));  
            area.removeFromBottom(nameMargin);

            innerElement.setBounds(area);
        }

    private:
        void setSliderProperties() {
            innerElement.setTextBoxStyle(Slider::TextBoxBelow, true, 60, 9);
            innerElement.setColour(Slider::ColourIds::textBoxOutlineColourId, Colour(0xFF));
            innerElement.setColour(Slider::ColourIds::textBoxTextColourId, Colour(0x80FFFFFF));
            innerElement.setSliderStyle(Slider::SliderStyle::LinearHorizontal);
        }
    };

    class KnobPanelToggle : public KnobPanelElement<juce::ToggleButton, bool>
    {
    public:
        KnobPanelToggle() : KnobPanelElement<juce::ToggleButton, bool>() {};

        void resized() override
        {
            constexpr auto bottomMargin = 8;
            constexpr auto nameMargin = 6;
            constexpr auto buttonSize = 28;

            auto area = getLocalBounds();
            area.removeFromBottom(bottomMargin);

            nameEditor.setBounds(area.removeFromBottom(18));  
            area.removeFromBottom(nameMargin);

            auto centerX = area.getCentreX();
            area.removeFromLeft(centerX - buttonSize / 2);

            innerElement.setBounds(area);
        }
    };
}


#endif // FORMULA_GUI_COMPONENTS_KNOB_PANEL_ELEMENT_INCLUDED