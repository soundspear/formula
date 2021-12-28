#ifndef FORMULA_COMPILER_CLANG_WRAPPER
#define FORMULA_COMPILER_CLANG_WRAPPER

#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <regex>

#include <boost/process.hpp>
#ifdef _WIN32
#include <boost/process/windows.hpp>
#endif
#include <boost/assign/std/vector.hpp>
#include <boost/dll/import.hpp>
#include <boost/filesystem/fstream.hpp>

#include <JuceHeader.h>
#include <BinaryData.h>

#include <events/EventHub.hpp>
#include <processor/PluginState.hpp>
#include <storage/CompilerStorage.hpp>

namespace formula::compiler {
	class ClangWrapper
	{
	public:
		ClangWrapper(const std::shared_ptr<formula::events::EventHub>& eventHub);
		~ClangWrapper();
		void compileFormula(const std::string& sourceCode);

	private:
		bool launchClang(std::vector<std::string> compileArgs, std::string& errStr);

		std::shared_ptr<formula::events::EventHub> eventHub;
		boost::filesystem::path clangPath;
		std::vector<std::string> clangArgs;
		std::thread compileThread;
		std::mutex compileMutex;
		std::string formulaBaseCodeMono;
		std::string formulaBaseCodeStereo;

		static constexpr char monoFormulaEntrypoint[] = "formula_main";
		static constexpr char stereoFormulaEntrypoint[] = "formula_main_stereo";
	};
}
#endif // FORMULA_COMPILER_CLANG_WRAPPER