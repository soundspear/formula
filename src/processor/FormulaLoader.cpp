#include "FormulaLoader.hpp"

formula::processor::FormulaLoader::FormulaLoader()
    : isMono(false), isLoaded(false)
{
    debugStackIdx = new int;
    debugStack = new char* [debugStackSize];
    for (int i = 0; i < debugStackSize; i++) {
        debugStack[i] = new char[2048];
    }
}

formula::processor::FormulaLoader::~FormulaLoader()
{
    unloadLibrary();

    delete debugStackIdx;
    for (int i = 0; i < debugStackSize; i++) {
        delete debugStack[i];
    }
    delete debugStack;
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
        for (auto & singleChannelLibrary : singleChannelLibraries) {
            if (singleChannelLibrary.is_loaded()) {
                singleChannelLibrary.unload();
            }
        }
    }
    else {
        if (twoChannelsLibrary.is_loaded()) {
            twoChannelsLibrary.unload();
        }
    }
    isLoaded = false;
    lastDebugString.clear();
}

void formula::processor::FormulaLoader::formulaProcessBlock(
    AudioBuffer<float>& buffer, 
    const std::shared_ptr<PluginState>& pluginState,
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
                    wet, inGain, outGain,
                    debugStackIdx, debugStack, debugStackSize
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
            wet, inGain, outGain,
            debugStackIdx, debugStack, debugStackSize
        );
    }
    formatDebugString();
}

void formula::processor::FormulaLoader::formatDebugString() {
    lastDebugString.clear();
    for (auto i = 0; i < *debugStackIdx; i++) {
        lastDebugString += debugStack[i] + std::string("\r\n");
    }
}
