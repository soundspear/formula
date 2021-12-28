#include "PluginState.hpp"

formula::processor::PluginState::PluginState(AudioProcessor& audioProcessor, String name)
    : AudioProcessorValueTreeState(audioProcessor, nullptr, juce::Identifier(name),
    {
        std::make_unique<AudioParameterFloat>("Dry Wet", "Dry Wet", 0.f, 1.f, 1.f),
        std::make_unique<AudioParameterFloat>("In Gain", "In Gain", -12.f, 12.f, .0f),
        std::make_unique<AudioParameterFloat>("Out Gain", "Out Gain", -12.f, 12.f, .0f),
        BOOST_PP_REPEAT(FORMULA_NUM_USER_KNOBS, FORMULA_CREATE_USER_KNOB_PARAMETERS, _)
        BOOST_PP_REPEAT(FORMULA_NUM_USER_SWITCHES, FORMULA_CREATE_USER_SWITCH_PARAMETERS, _)
    })
{
}

float formula::processor::PluginState::getKnobValue(int knobId)
{
    return getParameterAsValue("Knob " + std::to_string(knobId + 1)).getValue();
}

float formula::processor::PluginState::getSwitchValue(int switchId)
{
    return getParameterAsValue("Switch " + std::to_string(switchId)).getValue();
}

void formula::processor::PluginState::setActiveFormulaMetadata(formula::processor::FormulaMetadata metadata)
{
    for (auto keyPair : metadata) {
        setPropertyAsString(
            std::string(keyPair.first),
            keyPair.second
        );
    }
}

void formula::processor::PluginState::setActiveFormulaMetadataField(std::string key, std::string value)
{
    setPropertyAsString(
        std::string(key), value
    );
}

formula::processor::FormulaMetadata formula::processor::PluginState::getActiveFormulaMetadata()
{
    FormulaMetadata metadata;
    const auto numProperties = state.getNumProperties();
    for (auto i = 0; i < numProperties; i++) {
        auto propName = state.getPropertyName(i);
        auto &stateVar = state.getProperty(propName);
        if (stateVar.isString()) {
            metadata[propName.toString().toStdString()] = VariantConverter<String>::fromVar(stateVar).toStdString();
        }
    }
    return metadata;
}

void formula::processor::PluginState::serialize(juce::MemoryBlock& outData)
{
    auto currentState = copyState();
    std::unique_ptr<juce::XmlElement> xml(currentState.createXml());

    MemoryOutputStream out(outData, false);
    const auto textXml = XmlElement::TextFormat().singleLine();
    xml->writeTo(out, XmlElement::TextFormat().singleLine());
}

void formula::processor::PluginState::deserialize(const void* data, int sizeInBytes)
{
    const auto xmlString = String::fromUTF8(static_cast<const char*>(data), sizeInBytes);
    std::unique_ptr<juce::XmlElement> xmlState(juce::parseXML(xmlString));

    if (xmlState != nullptr)
        replaceState(juce::ValueTree::fromXml(*xmlState));
}

AudioProcessorValueTreeState::SliderAttachment* formula::processor::PluginState::attachSlider(std::string name, Slider& slider)
{
    return new AudioProcessorValueTreeState::SliderAttachment(*this, juce::String(name), slider);
}

AudioProcessorValueTreeState::ButtonAttachment* formula::processor::PluginState::attachButton(std::string name, Button& button)
{
    return new AudioProcessorValueTreeState::ButtonAttachment(*this, juce::String(name), button);
}

void formula::processor::PluginState::setPropertyAsString(std::string key, std::string value)
{
    auto identifier = juce::Identifier(juce::String(key));
    state.setProperty(identifier, juce::var(String(value)), nullptr);
}

std::string formula::processor::PluginState::getPropertyAsString(std::string key)
{
    auto identifier = juce::Identifier(juce::String(key));
    if (!state.hasProperty(identifier)) {
        return "";
    }
    const auto& stateVar = state.getProperty(identifier);
    if (stateVar.isString()) {
        return VariantConverter<String>::fromVar(stateVar).toStdString();
    }
    else {
        return "";
    }
}
