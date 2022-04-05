#ifndef FORMULA_PROCESSOR_FORMULA_LOADER_INCLUDED
#define FORMULA_PROCESSOR_FORMULA_LOADER_INCLUDED

#include <boost/dll/import.hpp>

#include <JuceHeader.h>

#include <storage/CompilerStorage.hpp>
#include <processor/PluginState.hpp>

typedef void (ProcessBlockMono)(float*, int, float, float*, float*, float, float, float);
typedef void (ProcessBlockStereo)(float**, int, float, float*, float*, float, float, float);

namespace formula::processor {
	class FormulaLoader
	{
	public:
		FormulaLoader();
		~FormulaLoader();
		void loadLibrary(std::string compilationId);
		void unloadLibrary();
		void formulaProcessBlock(
			AudioBuffer<float>& buffer, 
			std::shared_ptr<PluginState> pluginState,
			double sampleRate
		);
		bool isReady() { return isLoaded; };
	private:
		boost::dll::shared_library singleChannelLibraries[2];
		boost::dll::shared_library twoChannelsLibrary;
		ProcessBlockMono* singleChannelEntrypoints[2];
		ProcessBlockStereo* twoChannelsEntrypoint;

		bool isMono;
		bool isLoaded;

		static constexpr char processBlockMonoSymbolName[] = "process_block_mono";
		static constexpr char processBlockStereoSymbolName[] = "process_block_stereo";
	};
}

#endif // FORMULA_PROCESSOR_FORMULA_LOADER_INCLUDED