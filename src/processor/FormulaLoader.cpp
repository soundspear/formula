#include "FormulaLoader.hpp"

formula::processor::FormulaLoader::FormulaLoader()
    : isMono(false), isLoaded(false), twoChannelsEntrypoint(nullptr), singleChannelEntrypoints()
{
    debugStackIdx = new int;
    debugStack = new char* [debugStackSize];
    for (int i = 0; i < debugStackSize; i++) {
        debugStack[i] = new char[2048];
    }
}

formula::processor::FormulaLoader::~FormulaLoader()
{
    if (loaderThread.joinable()) {
        loaderThread.join();
    }

    unloadLibrary();

    delete debugStackIdx;
    for (int i = 0; i < debugStackSize; i++) {
        delete debugStack[i];
    }
    delete debugStack;
}

void formula::processor::FormulaLoader::loadCompiledFormulaAsync(
    std::string compilationId,
    std::optional<std::string> oldFormulaToDeleteCompilationId
)
{
    if (loaderThread.joinable()) {
        loaderThread.join();
    }

    loaderThread = std::thread  {
        &formula::processor::FormulaLoader::loadLibrary,
        this,
        compilationId, oldFormulaToDeleteCompilationId
    };
}

void formula::processor::FormulaLoader::unloadCompiledFormulaAsync()
{
    if (loaderThread.joinable()) {
        loaderThread.join();
    }
    loaderThread = std::thread{ &formula::processor::FormulaLoader::unloadLibrary, this };
}

void formula::processor::FormulaLoader::formulaProcessBlock(
    AudioBuffer<float>& buffer,
    float dryWet, float inGain, float outGain,
    float* userKnobs, float* userSwitches,
    double sampleRate
) {
    if (buffer.getNumChannels() > 2) return;
    if (!isLoaded) return;

    auto locked = libraryUsedMutex.try_lock();
    if (!locked) {
        return;
    }

    float** writePointers = buffer.getArrayOfWritePointers();
    int numSamples = buffer.getNumSamples();
    int numChannels = buffer.getNumChannels();

    if (isMono) {
        for (int channel = 0; channel < numChannels; channel++) {
            singleChannelEntrypoints[channel](
                    writePointers[channel],
                    numSamples,
                    static_cast<float>(sampleRate),
                    userKnobs, userSwitches,
                    dryWet, inGain, outGain,
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
            userKnobs, userSwitches,
            dryWet, inGain, outGain,
            debugStackIdx, debugStack, debugStackSize
        );
    }
    formatDebugString();

    libraryUsedMutex.unlock();
}

void formula::processor::FormulaLoader::loadLibrary(
        std::string compilationId,
        std::optional<std::string> oldFormulaToDeleteCompilationId
    ) {
    unloadLibrary();

    formula::storage::CompilerStorage storage;
    const auto path = storage.getLibraryPath(compilationId);

    if (!boost::filesystem::exists(path.singleChannelLibrariesPaths[0])
        && !boost::filesystem::exists(path.twoChannelsLibraryPath)) {
        return;
    }

    libraryUsedMutex.lock();

    boost::dll::shared_library formula;

    isMono = boost::filesystem::exists(path.singleChannelLibrariesPaths[0]);

    if (isMono) {
        for (int channel = 0; channel < 2; channel++) {
            singleChannelLibraries[channel] = boost::dll::shared_library();
            singleChannelLibraries[channel].load(path.singleChannelLibrariesPaths[channel]);
            singleChannelEntrypoints[channel] = singleChannelLibraries[channel].get<ProcessBlockMono>(processBlockMonoSymbolName);
        }
    }
    else {
        twoChannelsLibrary = boost::dll::shared_library();
        twoChannelsLibrary.load(path.twoChannelsLibraryPath);
        twoChannelsEntrypoint = twoChannelsLibrary.get<ProcessBlockStereo>(processBlockStereoSymbolName);
    }
    isLoaded = true;

    try {
        if (!oldFormulaToDeleteCompilationId.has_value()) {
            formula::storage::CompilerStorage storage;
            storage.deleteLibrary(oldFormulaToDeleteCompilationId.value());
        }
    } catch (std::exception&) {}

    libraryUsedMutex.unlock();
}

void formula::processor::FormulaLoader::unloadLibrary() {
    if (!isLoaded) return;

    libraryUsedMutex.lock();

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

    libraryUsedMutex.unlock();
}

void formula::processor::FormulaLoader::formatDebugString() {
    lastDebugString.clear();
    for (auto i = 0; i < *debugStackIdx; i++) {
        lastDebugString += debugStack[i] + std::string("\r\n");
    }
}