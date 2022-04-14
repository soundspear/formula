#ifndef FORMULA_PROCESSOR_KNOBSPANELLISTENER_INCLUDED
#define FORMULA_PROCESSOR_KNOBSPANELLISTENER_INCLUDED

#include <boost/lexical_cast.hpp>

#include <JuceHeader.h>

#include "processor/PluginState.hpp"

namespace formula::processor {
    class KnobsPanelListener : public juce::AudioProcessorValueTreeState::Listener {
    public:
        KnobsPanelListener();
        void parameterChanged(const String &parameterID, float newValue) override;
    protected:
        float dryWet, inGain, outGain;
        float userKnobs[FORMULA_NUM_USER_KNOBS], userSwitches[FORMULA_NUM_USER_SWITCHES];
    private:
        void dryWetChanged(float newValue);
        void inGainChanged(float newValue);
        void outGainChanged(float newValue);
        void userKnobChanged(float newValue, int knobId);
        void userSwitchChanged(float newValue, int switchId);
    };
}

#endif //FORMULA_PROCESSOR_KNOBSPANELLISTENER_INCLUDED
