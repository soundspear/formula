/*
    Copyright (c) 2022 Soundspear.
    Distributed under the Boost Software License, Version 1.0.
    See http://www.boost.org/LICENSE_1_0.txt
*/

#ifndef FORMULA_PROCESSOR_KNOBSPANELLISTENER_INCLUDED
#define FORMULA_PROCESSOR_KNOBSPANELLISTENER_INCLUDED

#include <boost/lexical_cast.hpp>

#include <JuceHeader.h>

#include "processor/PluginState.hpp"

namespace formula::processor {
    /**
     * Listener to the knobs and switches panel events
     */
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
