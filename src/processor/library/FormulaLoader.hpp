/*
    Copyright (c) 2022 Soundspear.
    Distributed under the Boost Software License, Version 1.0.
    See http://www.boost.org/LICENSE_1_0.txt
*/

#ifndef FORMULA_PROCESSOR_FORMULALOADER_INCLUDED
#define FORMULA_PROCESSOR_FORMULALOADER_INCLUDED

#include <optional>
#include <atomic>
#include <mutex>
#include <thread>

#include <boost/dll/import.hpp>
#include <boost/assign/std/vector.hpp>
#include <JuceHeader.h>

#include "src/storage/CompilerStorage.hpp"
#include "src/processor/PluginState.hpp"
#include "FormulaVersion.hpp"
#include "FormulaVersion_1_0.hpp"
#include "FormulaVersion_1_1.hpp"

namespace formula::processor::library {
    /**
     * Loads a compiled formula and calls it
     */
	class FormulaLoader
	{
	public:
		FormulaLoader();
		~FormulaLoader();

		void loadCompiledFormulaAsync(std::string compilationId,
                                      std::optional<std::string> oldFormulaToDeleteCompilationId);
		void unloadCompiledFormulaAsync();
		void formulaProcessBlock(
                AudioBuffer<float>& buffer,
                float dryWet, float inGain, float outGain,
                double bpm, double bar,
                float* userKnobs, float* userSwitches,
                double sampleRate
        );
		bool isReady() const { return isLoaded; };
        std::string getDebugString() { return lastDebugString; }
	private:
        void loadLibrary(std::string compilationId,
                         std::optional<std::string> oldFormulaToDeleteCompilationId);
        void unloadLibrary();

        void formatDebugString();

        int *debugStackIdx;
        char **debugStack;
        int debugStackSize = 512;

        std::string lastDebugString;

		std::atomic<bool> isLoaded;

        std::vector<std::shared_ptr<FormulaVersion>> formulaLibraryVersions;
        size_t loadedFormulaLibraryVersionIdx;

        std::mutex libraryUsedMutex;
        std::thread loaderThread;
	};
}

#endif // FORMULA_PROCESSOR_FORMULALOADER_INCLUDED