/*
    Copyright (c) 2022 Soundspear.
    Distributed under the Boost Software License, Version 1.0.
    See http://www.boost.org/LICENSE_1_0.txt
*/

#include "FormulaLoader.hpp"

formula::processor::library::FormulaLoader::FormulaLoader()
    : isLoaded(false)
{
    formulaLibraryVersions.emplace_back(std::make_shared<FormulaVersion_1_1>());
    formulaLibraryVersions.emplace_back(std::make_shared<FormulaVersion_1_0>());

    debugStackIdx = new int;
    debugStack = new char* [debugStackSize];
    for (int i = 0; i < debugStackSize; i++) {
        debugStack[i] = new char[2048];
    }
}

formula::processor::library::FormulaLoader::~FormulaLoader()
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

void formula::processor::library::FormulaLoader::loadCompiledFormulaAsync(
    std::string compilationId,
    std::optional<std::string> oldFormulaToDeleteCompilationId
)
{
    if (loaderThread.joinable()) {
        loaderThread.join();
    }

    loaderThread = std::thread  {
        &formula::processor::library::FormulaLoader::loadLibrary,
        this,
        compilationId, oldFormulaToDeleteCompilationId
    };
}

void formula::processor::library::FormulaLoader::unloadCompiledFormulaAsync()
{
    if (loaderThread.joinable()) {
        loaderThread.join();
    }
    loaderThread = std::thread{ &formula::processor::library::FormulaLoader::unloadLibrary, this };
}

void formula::processor::library::FormulaLoader::formulaProcessBlock(
    AudioBuffer<float>& buffer,
    float dryWet, float inGain, float outGain,
    double bpm, double bar,
    float* userKnobs, float* userSwitches,
    double sampleRate
) {
    if (buffer.getNumChannels() > 2) return;
    if (!isLoaded) return;

    auto locked = libraryUsedMutex.try_lock();
    if (!locked) {
        return;
    }

    const auto& formula = formulaLibraryVersions[loadedFormulaLibraryVersionIdx];
    formula->processBlock(buffer,
                          dryWet, inGain, outGain,
                          bpm, bar,
                          userKnobs, userSwitches,
                          sampleRate,
                          debugStackIdx, debugStack, debugStackSize);

    formatDebugString();

    libraryUsedMutex.unlock();
}

void formula::processor::library::FormulaLoader::loadLibrary(
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

    bool success = false;
    for (size_t i = 0; i < formulaLibraryVersions.size(); ++i) {
        success = formulaLibraryVersions[i]->tryLoad(path);
        if (success) {
            loadedFormulaLibraryVersionIdx = i;
            break;
        }
    }

    if (!success) {
        isLoaded = false;
        loadedFormulaLibraryVersionIdx = 0;
        return;
    }

    try {
        if (oldFormulaToDeleteCompilationId.has_value()) {
            storage.deleteLibrary(oldFormulaToDeleteCompilationId.value());
        }
    } catch (std::exception&) {}

    isLoaded = true;
    libraryUsedMutex.unlock();
}

void formula::processor::library::FormulaLoader::unloadLibrary() {
    if (!isLoaded) return;

    libraryUsedMutex.lock();

    const auto& formula = formulaLibraryVersions[loadedFormulaLibraryVersionIdx];
    formula->unload();

    isLoaded = false;
    lastDebugString.clear();

    libraryUsedMutex.unlock();
}

void formula::processor::library::FormulaLoader::formatDebugString() {
    lastDebugString.clear();
    for (auto i = 0; i < *debugStackIdx; i++) {
        lastDebugString += debugStack[i] + std::string("\r\n");
    }
}