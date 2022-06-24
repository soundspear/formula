/*
    Copyright (c) 2022 Soundspear.
    Distributed under the Boost Software License, Version 1.0.
    See http://www.boost.org/LICENSE_1_0.txt
*/

#ifndef FORMULA_COMPILER_COMPILERWRAPPER_INCLUDED
#define FORMULA_COMPILER_COMPILERWRAPPER_INCLUDED

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
#include <compiler/NoDynamicAllocationGuard.hpp>
#include <compiler/NoInlineAssemblyGuard.hpp>
#include <compiler/NoPreprocessorGuard.hpp>
#include <compiler/NoSystemCallGuard.hpp>

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

        static std::string replaceMacros(std::string str);
		std::string& getBaseCodeMono() { return formulaBaseCodeMono; }
		std::string& getBaseCodeStereo() { return formulaBaseCodeStereo; }

	private:
        bool checkSecurityGuards(const std::string& sourceCode);
		bool launchCompiler(const std::string& compilerPath, const std::vector<std::string>& compileArgs, std::string& errStr);

		std::shared_ptr<formula::events::EventHub> eventHub;
		std::thread compileThread;
		std::mutex compileMutex;
		std::string formulaBaseCodeMono;
		std::string formulaBaseCodeStereo;
        std::vector<std::unique_ptr<formula::compiler::SecurityGuard>> securityGuards;

		static constexpr char monoFormulaEntrypoint[] = "formula_main";
		static constexpr char stereoFormulaEntrypoint[] = "formula_main_stereo";
	};
}
#endif // FORMULA_COMPILER_COMPILERWRAPPER_INCLUDED