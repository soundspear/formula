/*
    Copyright (c) 2022 Soundspear.
    Distributed under the Boost Software License, Version 1.0.
    See http://www.boost.org/LICENSE_1_0.txt
*/

#ifndef FORMULA_PROCESSOR_PLUGINSTATE_INCLUDED
#define FORMULA_PROCESSOR_PLUGINSTATE_INCLUDED

#include <string>
#include <variant>

#include <boost/lexical_cast.hpp>
#include <boost/preprocessor/repetition/repeat_from_to.hpp>

#include <JuceHeader.h>

#include <processor/FormulaMetadata.hpp>

#define FORMULA_NUM_USER_KNOBS 12
#define FORMULA_NUM_USER_SWITCHES 3
#define FORMULA_CREATE_USER_KNOB_PARAMETERS(_, i, __) std::make_unique<AudioParameterFloat>("Knob " + std::to_string((i)+1), "Knob " + std::to_string((i)+1), 0.f, 1.f, 0.5f),
#define FORMULA_CREATE_USER_SWITCH_PARAMETERS(_, i, __) std::make_unique<AudioParameterBool>("Switch " + std::to_string((i)+1), "Switch " + std::to_string((i)+1), false),


namespace formula::processor {
	class PluginState : public AudioProcessorValueTreeState
	{
	public:
		PluginState(AudioProcessor& processor, String name);
        ~PluginState() override;

        void setupListener(AudioProcessorValueTreeState::Listener* listener);

        std::string getDebugString() { return debugString; };
        void setDebugString(std::string newString) { debugString = newString; }

		void setActiveFormulaMetadata(FormulaMetadata metadata);
		void setActiveFormulaMetadataField(std::string key, std::string value);
		FormulaMetadata getActiveFormulaMetadata();

		void serialize(juce::MemoryBlock& outData);
		void deserialize(const void* data, int sizeInBytes);

		AudioProcessorValueTreeState::SliderAttachment*
			attachSlider(std::string name, Slider& slider);
		AudioProcessorValueTreeState::ButtonAttachment*
			attachButton(std::string name, Button& button);

        bool isBypassed() { return bypass; }
        void setBypassed(bool isBypassed) { bypass = isBypassed; }
	private:
		void setPropertyAsString(std::string key, std::string value);
		std::string getPropertyAsString(std::string key);

        std::string debugString;
        std::atomic<bool> bypass = false;

        std::mutex stateSaveMutex;

		//JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginState)
	};
}

#endif // FORMULA_PROCESSOR_PLUGINSTATE_INCLUDED