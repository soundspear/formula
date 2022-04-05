#include "FormulaLoader.hpp"

formula::processor::FormulaLoader::FormulaLoader()
    : isMono(false), isLoaded(false)
{
}

formula::processor::FormulaLoader::~FormulaLoader()
{
    unloadLibrary();
}

void formula::processor::FormulaLoader::loadLibrary(std::string compilationId)
{
    unloadLibrary();

    formula::storage::CompilerStorage storage;
    const auto libraryPath = storage.getLibraryPath(compilationId);

    if (!boost::filesystem::exists(libraryPath.singleChannelLibrariesPaths[0])
        && !boost::filesystem::exists(libraryPath.twoChannelsLibraryPath)) {
        return;
    }

    boost::dll::shared_library formula;

    isMono = boost::filesystem::exists(libraryPath.singleChannelLibrariesPaths[0]);

    if (isMono) {
        for (int channel = 0; channel < 2; channel++) {
            singleChannelLibraries[channel] = boost::dll::shared_library();
            singleChannelLibraries[channel].load(libraryPath.singleChannelLibrariesPaths[channel]);
            singleChannelEntrypoints[channel] = singleChannelLibraries[channel].get<ProcessBlockMono>(processBlockMonoSymbolName);
        }
    }
    else {
        twoChannelsLibrary = boost::dll::shared_library();
        twoChannelsLibrary.load(libraryPath.twoChannelsLibraryPath);
        twoChannelsEntrypoint = twoChannelsLibrary.get<ProcessBlockStereo>(processBlockStereoSymbolName);
    }
    isLoaded = true;
}

void formula::processor::FormulaLoader::unloadLibrary()
{
    if (!isLoaded) return;
    if (isMono) {
        for (int channel = 0; channel < 2; channel++) {
            if (singleChannelLibraries[channel].is_loaded()) {
                singleChannelLibraries[channel].unload();
            }
        }
    }
    else {
        if (twoChannelsLibrary.is_loaded()) {
            twoChannelsLibrary.unload();
        }
    }
    isLoaded = false;
}

void formula::processor::FormulaLoader::formulaProcessBlock(
    AudioBuffer<float>& buffer, 
    std::shared_ptr<PluginState> pluginState,
    double sampleRate
) {
    if (buffer.getNumChannels() > 2) return;
    if (!isLoaded) return;

    float** writePointers = buffer.getArrayOfWritePointers();
    int numSamples = buffer.getNumSamples();
    int numChannels = buffer.getNumChannels();

    float knobs[FORMULA_NUM_USER_KNOBS];
    float switches[FORMULA_NUM_USER_SWITCHES];
    for (auto i = 0; i < FORMULA_NUM_USER_KNOBS; i++) {
        knobs[i] = pluginState->getKnobValue(i);
    }
    for (auto i = 0; i < FORMULA_NUM_USER_SWITCHES; i++) {
        switches[i] = pluginState->getSwitchValue(i);
    }

    const auto wet = pluginState->getDryWet();
    const auto inGain = powf(10, 0.05f*pluginState->getInGain());
    const auto outGain = powf(10, 0.05f*pluginState->getOutGain());
    if (isMono) {
        for (int channel = 0; channel < numChannels; channel++) {
            singleChannelEntrypoints[channel](
                writePointers[channel], 
                numSamples, 
                static_cast<float>(sampleRate), 
                knobs, switches,
                wet, inGain, outGain
            );
        }
    }
    else {
        if (buffer.getNumChannels() != 2) return;
        twoChannelsEntrypoint(
            writePointers, 
            numSamples, 
            static_cast<float>(sampleRate), 
            knobs, switches,
            wet, inGain, outGain
        );
    }
}