#include <processor/PluginProcessor.hpp>
#include <gui/PluginWindow.hpp>
#include "cloud/FormulaCloudClient.hpp"

formula::processor::PluginProcessor::PluginProcessor()
     : AudioProcessor (BusesProperties()
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                       ),
    eventHub(std::make_shared<formula::events::EventHub>()),
    pluginState(std::make_shared<formula::processor::PluginState>(*this, "Formula")),
    settings(std::make_shared<formula::storage::LocalSettings>()),
    cloud(std::make_shared<formula::cloud::FormulaCloudClient>(settings, eventHub)),
    localIndex(std::make_shared<formula::storage::LocalIndex>(pluginState)),
    clangWrapper(eventHub),
    recompiled(false)
{
    eventHub->subscribe(EventType::compilationSuccess, [this](boost::any compilationId) {
        auto formulaMetadata = pluginState->getActiveFormulaMetadata();
        this->previousCompilationId = formulaMetadata[FormulaMetadataKeys::compilationId];
        formulaMetadata[FormulaMetadataKeys::compilationId] = boost::any_cast<std::string>(compilationId);
        pluginState->setActiveFormulaMetadata(formulaMetadata);
        this->recompiled = true;
    });
}

formula::processor::PluginProcessor::~PluginProcessor()
{
}

const juce::String formula::processor::PluginProcessor::getName() const
{
    return JucePlugin_Name;
}

bool formula::processor::PluginProcessor::acceptsMidi() const
{
    return false;
}

bool formula::processor::PluginProcessor::producesMidi() const
{
    return false;
}

bool formula::processor::PluginProcessor::isMidiEffect() const
{
    return false;
}

double formula::processor::PluginProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int formula::processor::PluginProcessor::getNumPrograms()
{
    return 1;
}

int formula::processor::PluginProcessor::getCurrentProgram()
{
    return 0;
}

void formula::processor::PluginProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String formula::processor::PluginProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void formula::processor::PluginProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

void formula::processor::PluginProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::ignoreUnused (sampleRate, samplesPerBlock);
}

void formula::processor::PluginProcessor::releaseResources()
{
}

bool formula::processor::PluginProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;

    return true;
}

void formula::processor::PluginProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                              juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);

    if (pluginState->isBypassed()) {
        return;
    }

    if (recompiled) {
        formulaLoader.unloadLibrary();

        if (!previousCompilationId.empty()) {
            formula::storage::CompilerStorage storage;
            storage.deleteLibrary(previousCompilationId);
        }

        formulaLoader.loadLibrary(pluginState->getActiveFormulaMetadata()[FormulaMetadataKeys::compilationId]);
        recompiled = false;
    }

    if (formulaLoader.isReady()) {
        juce::ScopedNoDenormals noDenormals;

        formulaLoader.formulaProcessBlock(buffer, pluginState, getSampleRate());

        pluginState->setDebugString(formulaLoader.getDebugString());
    }
}

bool formula::processor::PluginProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* formula::processor::PluginProcessor::createEditor()
{
    return new formula::gui::PluginWindow(*this, eventHub, pluginState, cloud, localIndex);
}

void formula::processor::PluginProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    pluginState->serialize(destData);
}

void formula::processor::PluginProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    pluginState->deserialize(data, sizeInBytes);
    const auto compilationId = pluginState->getActiveFormulaMetadata()[FormulaMetadataKeys::compilationId];
    if (!compilationId.empty()) {
        recompiled = true;
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new formula::processor::PluginProcessor();
}
