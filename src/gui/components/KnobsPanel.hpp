#ifndef FORMULA_GUI_KNOBSPANEL_INCLUDED
#define FORMULA_GUI_KNOBSPANEL_INCLUDED

#include <string>
#include <array>

#include <boost/lexical_cast.hpp>

#include <JuceHeader.h>

#include <processor/PluginState.hpp>
#include <processor/FormulaMetadata.hpp>
#include <gui/components/KnobPanelElement.hpp>
#include <events/EventHub.hpp>

namespace formula::gui {
    template<unsigned int NumKnobs, unsigned int NumSwitches>
	class KnobsPanel : public Component 
    {
    public:
        explicit KnobsPanel(
            const std::shared_ptr<formula::processor::PluginState>& pluginState);
        void restoreFromState(formula::processor::FormulaMetadata& metadata);
        void paint(Graphics& g) override;
        void resized() override;
        void resetContent();
        void visibilityChanged() override;
    private:
        KnobPanelSlider dryWetKnob;
        KnobPanelSlider inGainKnob;
        KnobPanelSlider outGainKnob;
        std::array<KnobPanelSlider, NumKnobs> userKnobs;
        std::array<KnobPanelToggle, NumSwitches> userSwitches;

        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> dryWetKnobAttachment;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> inGainKnobAttachment;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> outGainKnobAttachment;
        std::array<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>, NumKnobs> userKnobsAttachments;
        std::array<std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>, NumSwitches> userSwitchesAttachments;

        std::shared_ptr<formula::processor::PluginState> pluginState;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(KnobsPanel)
	};
}

template<unsigned int NumKnobs, unsigned int NumSwitches>
formula::gui::KnobsPanel<NumKnobs, NumSwitches>::KnobsPanel(
    const std::shared_ptr<formula::processor::PluginState>& pluginState
) : dryWetKnob(true, juce::Slider::SliderStyle::RotaryVerticalDrag), 
    inGainKnob(true, juce::Slider::SliderStyle::RotaryVerticalDrag), 
    outGainKnob(true, juce::Slider::SliderStyle::RotaryVerticalDrag)
{
    addAndMakeVisible(dryWetKnob);
    dryWetKnob.setNameLabel("Dry Wet");
    {
        auto &knob = dryWetKnob.inner();
        knob.setRange(0., 1.);
        knob.setValue(1.);
        dryWetKnobAttachment.reset(pluginState->attachSlider("Dry Wet", knob));
    }

    inGainKnob.setNameLabel("In Gain");
    addAndMakeVisible(inGainKnob);
    {
        auto &knob = inGainKnob.inner();
        knob.setRange(-12., 12.);
        knob.setValue(0.);
        knob.setTextValueSuffix(" dB");
        inGainKnobAttachment.reset(pluginState->attachSlider("In Gain", knob));
    }

    outGainKnob.setNameLabel("Out Gain");
    addAndMakeVisible(outGainKnob);
    {
        auto& knob = outGainKnob.inner();
        knob.setRange(-12., 12.);
        knob.setValue(0., juce::NotificationType::dontSendNotification);
        knob.setTextValueSuffix(" dB");
        outGainKnobAttachment.reset(pluginState->attachSlider("Out Gain", knob));
    }

    for (auto i = 0; i < NumSwitches; i++) {
        addAndMakeVisible(userSwitches[i]);
        userSwitches[i].setNameLabel("...");
        auto &toggle = userSwitches[i].inner();
        toggle.setToggleState(false, juce::NotificationType::dontSendNotification);
        toggle.onStateChange = [&, i]() {
            pluginState->setActiveFormulaMetadataField(
                formula::processor::FormulaMetadataKeys::switchDefaultValue(i),
                std::to_string(toggle.getToggleState())
            );
        };
        auto switchName = std::string("Switch ") + std::to_string(i + 1);
        userSwitchesAttachments[i].reset(pluginState->attachButton(switchName, toggle));
        auto& textEditor = userSwitches[i].nameTextEditor();
        textEditor.onFocusLost = [&, i]() {
            pluginState->setActiveFormulaMetadataField(
                formula::processor::FormulaMetadataKeys::switchName(i),
                textEditor.getText().toStdString()
            );
        };
    }

    for (auto i = 0; i < NumKnobs; i++) {
        addAndMakeVisible(userKnobs[i]);
        userKnobs[i].setNameLabel("...");
        auto &knob = userKnobs[i].inner();
        knob.setRange(0., 1.);
        knob.onValueChange = [&, i]() {
            pluginState->setActiveFormulaMetadataField(
                formula::processor::FormulaMetadataKeys::knobDefaultValue(i),
                std::to_string(knob.getValue())
            );
        };
        auto knobName = std::string("Knob ") + std::to_string(i + 1);
        userKnobsAttachments[i].reset(pluginState->attachSlider(knobName, knob));
        auto& textEditor = userKnobs[i].nameTextEditor();
        textEditor.onFocusLost = [&, i]() {
            pluginState->setActiveFormulaMetadataField(
                formula::processor::FormulaMetadataKeys::knobName(i),
                textEditor.getText().toStdString()
            );
        };
    }

    auto metadata = pluginState->getActiveFormulaMetadata();
    restoreFromState(metadata);
}

template<unsigned int NumKnobs, unsigned int NumSwitches>
inline void formula::gui::KnobsPanel<NumKnobs, NumSwitches>::restoreFromState(formula::processor::FormulaMetadata& metadata)
{
    dryWetKnob.inner().setValue(1.f);
    inGainKnob.inner().setValue(0.f);
    outGainKnob.inner().setValue(0.f);

    for (auto i = 0; i < NumSwitches; i++) {
        auto& toggle = userSwitches[i].inner();
        auto valueStr = metadata[formula::processor::FormulaMetadataKeys::switchDefaultValue(i)];
        try
        {
            toggle.setToggleState(
                static_cast<bool>(boost::lexical_cast<unsigned>(valueStr)), juce::NotificationType::sendNotification
            );
        }
        catch (boost::bad_lexical_cast&)
        {
            // Switch has never been toggled and does not have a default value
            toggle.setToggleState(0, juce::NotificationType::sendNotification);
        }
        auto& textEditor = userSwitches[i].nameTextEditor(); 
        auto name = metadata[formula::processor::FormulaMetadataKeys::switchName(i)];
        if (!name.empty()) {
            textEditor.setText(name);
        }
        else {
            textEditor.setText("...");
        }
    }

    for (auto i = 0; i < NumKnobs; i++) {
        auto& knob = userKnobs[i].inner();
        auto valueStr = metadata[formula::processor::FormulaMetadataKeys::knobDefaultValue(i)];
        try
        {
            knob.setValue(boost::lexical_cast<double>(valueStr));
        }
        catch (boost::bad_lexical_cast&)
        {
            // Knob has never been moved and does not have a default value
            knob.setValue(.5f);
        }
        auto& textEditor = userKnobs[i].nameTextEditor();
        auto name = metadata[formula::processor::FormulaMetadataKeys::knobName(i)];
        if (!name.empty()) {
            textEditor.setText(name);
        }
        else {
            textEditor.setText("...");
        }
    }
}

template<unsigned int NumKnobs, unsigned int NumSwitches>
void formula::gui::KnobsPanel<NumKnobs, NumSwitches>::paint(Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(ListBox::backgroundColourId));
    auto area = getLocalBounds();
    g.setColour(getLookAndFeel().findColour(ListBox::outlineColourId));
    g.drawRect(area);
}

template<unsigned int NumKnobs, unsigned int NumSwitches>
void formula::gui::KnobsPanel<NumKnobs, NumSwitches>::resized()
{
    constexpr auto pad = 12;

    Grid grid;

    grid.rowGap = 8_px;
    grid.columnGap = 8_px;

    using Track = Grid::TrackInfo;

#define ITEMS_PER_ROW 6

    for (auto i = 0; i < ITEMS_PER_ROW; i++) {
        grid.templateColumns.add(Track(1_fr));
    }

    auto numColumns = std::ceil((3 + FORMULA_NUM_USER_KNOBS + FORMULA_NUM_USER_SWITCHES) / ITEMS_PER_ROW);
    for (auto i = 0; i < numColumns; i++) {
        grid.templateRows.add(Track(1_fr));
    }

    grid.autoColumns = Track(1_fr);
    grid.autoRows = Track(1_fr);
    grid.autoFlow = Grid::AutoFlow::row;

    grid.items.add(GridItem(dryWetKnob));
    grid.items.add(GridItem(inGainKnob));
    grid.items.add(GridItem(outGainKnob));
    for (auto i = 0; i < FORMULA_NUM_USER_SWITCHES; i++) {
        grid.items.add(GridItem(userSwitches[i]));
    }
    for (auto i = 0; i < FORMULA_NUM_USER_KNOBS; i++) {
        grid.items.add(GridItem(userKnobs[i]));
    }

    grid.performLayout(
        getLocalBounds()
        .withTrimmedTop(pad).withTrimmedBottom(pad)
        .withTrimmedLeft(pad).withTrimmedRight(pad));
}

template<unsigned int NumKnobs, unsigned int NumSwitches>
void formula::gui::KnobsPanel<NumKnobs, NumSwitches>::resetContent()
{
}


template<unsigned int NumKnobs, unsigned int NumSwitches>
void formula::gui::KnobsPanel<NumKnobs, NumSwitches>::visibilityChanged()
{
}


#endif // FORMULA_GUI_KNOBSPANEL_INCLUDED