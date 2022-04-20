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
	class CompilerWrapper
	{
	public:
		explicit CompilerWrapper(const std::shared_ptr<formula::events::EventHub>& eventHubRef);
		virtual ~CompilerWrapper();
		void compileFormula(const std::string& sourceCode);
        bool isCompilerAvailable() { return !(getCompilerPath().empty()); }

	protected:
		virtual void sanitizeErrorString(std::string& errStr, bool isMono) = 0;
		virtual std::string getCompilerPath() = 0;
		virtual std::vector<std::string> getCompilerArgs(std::string sourcePath, std::string outPath, bool isMono) = 0;

		std::string& getBaseCodeMono() { return formulaBaseCodeMono; }
		std::string& getBaseCodeStereo() { return formulaBaseCodeStereo; }

	private:
		bool launchCompiler(const std::string& compilerPath, const std::vector<std::string>& compileArgs, std::string& errStr);

		std::shared_ptr<formula::events::EventHub> eventHub;
		std::thread compileThread;
		std::mutex compileMutex;
		std::string formulaBaseCodeMono;
		std::string formulaBaseCodeStereo;

		static constexpr char monoFormulaEntrypoint[] = "formula_main";
		static constexpr char stereoFormulaEntrypoint[] = "formula_main_stereo";
	};
}
#endif // FORMULA_COMPILER_CLANG_WRAPPER