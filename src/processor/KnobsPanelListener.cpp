#include "KnobsPanelListener.hpp"

formula::processor::KnobsPanelListener::KnobsPanelListener()
: juce::AudioProcessorValueTreeState::Listener() {
    dryWetChanged(1.f);
    inGainChanged(0.f);
    outGainChanged(0.f);
    for (auto i = 1; i < FORMULA_NUM_USER_KNOBS + 1; i++) {
        userKnobChanged(.5f, i);
    }
    for (auto i = 1; i < FORMULA_NUM_USER_SWITCHES + 1; i++) {
        userSwitchChanged(0.f, i);
    }
}

void formula::processor::KnobsPanelListener::parameterChanged(const String &parameterID, float newValue) {
    if (parameterID == "Dry Wet") {
        dryWetChanged(newValue);
    } else if (parameterID == "In Gain") {
        inGainChanged(newValue);
    } else if (parameterID == "Out Gain") {
        outGainChanged(newValue);
    } else if (parameterID.startsWith("Knob")) {
        auto knobIdStr = parameterID.fromFirstOccurrenceOf("Knob ", false, false);
        auto knobId = boost::lexical_cast<int>(knobIdStr);
        userKnobChanged(newValue, knobId);
    } else if (parameterID.startsWith("Switch")) {
        auto switchIdStr = parameterID.fromFirstOccurrenceOf("Switch ", false, false);
        auto switchId = boost::lexical_cast<int>(switchIdStr);
        userSwitchChanged(newValue, switchId);
    }
}

void formula::processor::KnobsPanelListener::dryWetChanged(float newValue) {
    dryWet = newValue;
}

void formula::processor::KnobsPanelListener::inGainChanged(float newValue) {
    inGain = powf(10, 0.05f*newValue);
}

void formula::processor::KnobsPanelListener::outGainChanged(float newValue) {
    outGain = powf(10, 0.05f*newValue);
}

void formula::processor::KnobsPanelListener::userKnobChanged(float newValue, int knobId) {
    userKnobs[knobId - 1] = newValue;
}

void formula::processor::KnobsPanelListener::userSwitchChanged(float newValue, int switchId) {
    userSwitches[switchId - 1] = newValue;
}