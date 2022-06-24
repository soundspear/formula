#ifndef FORMULA_PROCESSOR_FORMULALOADER_INCLUDED
#define FORMULA_PROCESSOR_FORMULALOADER_INCLUDED

#include <optional>
#include <atomic>
#include <mutex>
#include <thread>

#include <boost/dll/import.hpp>

#include <JuceHeader.h>

#include <storage/CompilerStorage.hpp>
#include <processor/PluginState.hpp>

typedef void (ProcessBlockMono)(float* in, int numSamples, float sampleRate,
                                const float* knobs, const float* switches,
                                float wet, float inGain, float outGain,
                                int* debug_idx, char** debug_stack, int debug_stack_size);

typedef void (ProcessBlockStereo)(float** in, int numSamples, float sampleRate,
                                  const float* knobs, const float* switches,
                                  float wet, float inGain, float outGain,
                                  int* debug_idx, char** debug_stack, int debug_stack_size);

namespace formula::processor {
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

		boost::dll::shared_library singleChannelLibraries[2];
		boost::dll::shared_library twoChannelsLibrary;
		ProcessBlockMono* singleChannelEntrypoints[2];
		ProcessBlockStereo* twoChannelsEntrypoint;

        int *debugStackIdx;
        char **debugStack;
        int debugStackSize = 512;

        std::string lastDebugString;

		std::atomic<bool> isMono;
		std::atomic<bool> isLoaded;

        std::mutex libraryUsedMutex;
        std::thread loaderThread;

		static constexpr char processBlockMonoSymbolName[] = "process_block_mono";
		static constexpr char processBlockStereoSymbolName[] = "process_block_stereo";
	};
}

#endif // FORMULA_PROCESSOR_FORMULALOADER_INCLUDED