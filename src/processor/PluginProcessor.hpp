#ifndef FORMULA_PROCESSOR_PLUGIN_PROCESSOR_INCLUDED
#define FORMULA_PROCESSOR_PLUGIN_PROCESSOR_INCLUDED

#include <boost/any.hpp>

#include <JuceHeader.h>

#include <events/EventHub.hpp>
#include <compiler/CompilerWrapper.hpp>
#include <processor/FormulaLoader.hpp>
#include <processor/PluginState.hpp>
#include <storage/LocalIndex.hpp>
#include <storage/CompilerStorage.hpp>
#include <storage/LocalSettings.hpp>
#include <cloud/FormulaCloudClient.hpp>

namespace formula::processor
{
    class PluginProcessor : public juce::AudioProcessor
    {
    public:
        PluginProcessor();
        ~PluginProcessor() override;
        void prepareToPlay (double sampleRate, int samplesPerBlock) override;
        void releaseResources() override;

        bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

        void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
        using AudioProcessor::processBlock;
        juce::AudioProcessorEditor* createEditor() override;
        bool hasEditor() const override;
        const juce::String getName() const override;

        bool acceptsMidi() const override;
        bool producesMidi() const override;
        bool isMidiEffect() const override;
        double getTailLengthSeconds() const override;
        int getNumPrograms() override;
        int getCurrentProgram() override;
        void setCurrentProgram (int index) override;
        const juce::String getProgramName (int index) override;
        void changeProgramName (int index, const juce::String& newName) override;
        void getStateInformation (juce::MemoryBlock& destData) override;
        void setStateInformation (const void* data, int sizeInBytes) override;

    private:
        std::shared_ptr<formula::events::EventHub> eventHub;
        std::shared_ptr<formula::processor::PluginState> pluginState;
        std::shared_ptr<formula::storage::LocalSettings> settings;
        std::shared_ptr<formula::storage::LocalIndex> localIndex;
        std::shared_ptr<formula::cloud::FormulaCloudClient> cloud;
        formula::compiler::CompilerWrapper clangWrapper;
        formula::processor::FormulaLoader formulaLoader;

        juce::Random random;
        std::string previousCompilationId;
        bool recompiled;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginProcessor)
    };
}

#endif // FORMULA_PROCESSOR_PLUGIN_PROCESSOR_INCLUDED