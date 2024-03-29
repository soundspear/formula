﻿/*
    Copyright (c) 2022 Soundspear.
    Distributed under the Boost Software License, Version 1.0.
    See http://www.boost.org/LICENSE_1_0.txt
*/

#include <compiler/CompilerWrapper.hpp>

#include <boost/algorithm/string.hpp>

using namespace boost::assign;

formula::compiler::CompilerWrapper::CompilerWrapper(const std::shared_ptr<formula::events::EventHub>& eventHubRef)
    : eventHub(eventHubRef)
{
    formulaBaseCodeMono = std::string(formula::binary::FormulaBaseMono_c, formula::binary::FormulaBaseMono_cSize);
    boost::replace_all(formulaBaseCodeMono, "\r\n", "\n");
    formulaBaseCodeStereo = std::string(formula::binary::FormulaBaseStereo_c, formula::binary::FormulaBaseStereo_cSize);
    boost::replace_all(formulaBaseCodeStereo, "\r\n", "\n");

    securityGuards += std::make_unique<NoDynamicAllocationGuard>();
    securityGuards += std::make_unique<NoInlineAssemblyGuard>();
    securityGuards += std::make_unique<NoPreprocessorGuard>();
    securityGuards += std::make_unique<NoSystemCallGuard>();

    eventHub->subscribe(EventType::compilationRequest, [this](boost::any arg) {
        std::lock_guard<std::mutex> lock { compileMutex };
        if (compileThread.joinable()) {
            compileThread.join();
        }
        auto sourceCode = boost::any_cast<std::string>(arg);
        compileThread = std::thread{&formula::compiler::CompilerWrapper::compileFormula, this, sourceCode };
    }, this);
}

formula::compiler::CompilerWrapper::~CompilerWrapper() {
    if (compileThread.joinable())
        compileThread.join();
    eventHub->unsubscribe<CompilerWrapper>(this);
}

bool formula::compiler::CompilerWrapper::checkSecurityGuards(const std::string& sourceCode) {
    std::string errStr;
    std::istringstream iss(sourceCode);
    std::string line;
    for (int i = 0; std::getline(iss, line); i++)
    {
        for (auto& guard : securityGuards)
        {
            if (guard->checkLine(line)) {
                errStr = "Line " + boost::lexical_cast<std::string>(i+1) + ": " + guard->getErrorMessage();
                eventHub->publish(EventType::compilationFail, errStr);
                return true;
            }
        }
    }
    return false;
}

void formula::compiler::CompilerWrapper::compileFormula(const std::string& sourceCode)
{
    if (checkSecurityGuards(sourceCode)) {
        return;
    }

    formula::storage::CompilerStorage storage;
    storage.copyLibFormula();
    const auto compilationId = storage.createCompilationId();

    const auto compiledLibPath = storage.getLibraryPath(compilationId);

    const auto hasMonoEntrypoint =
        std::regex_search(sourceCode,
            std::regex(monoFormulaEntrypoint + std::string(R"(\s*\{)")));
    const auto hasStereoEntrypoint =
        std::regex_search(sourceCode,
            std::regex(stereoFormulaEntrypoint + std::string(R"(\s*\{)")));

    std::string sourcePath, outPath;
    if (hasMonoEntrypoint) {
        outPath = compiledLibPath.singleChannelLibrariesPaths[0];
        sourcePath = storage.createSourceFile(compilationId, formulaBaseCodeMono + sourceCode);
    }
    else if (hasStereoEntrypoint) {
        outPath = compiledLibPath.twoChannelsLibraryPath;
        sourcePath = storage.createSourceFile(compilationId, formulaBaseCodeStereo + sourceCode);
    }
    else {
        eventHub->publish(EventType::compilationFail,
            std::string("Missing formula entry point: ")
            + std::string(monoFormulaEntrypoint) + std::string(" or ")
            + std::string(stereoFormulaEntrypoint)
        );
        return;
    }

    std::string errStr;
    auto compileArgs = getCompilerArgs(sourcePath, outPath, hasMonoEntrypoint);
    auto compilerPath = getCompilerPath();
    bool success = launchCompiler(compilerPath, compileArgs, errStr);

    storage.deleteSourceFile(compilationId);

    if (success) {
        if (hasMonoEntrypoint) {
            boost::filesystem::copy_file(
                compiledLibPath.singleChannelLibrariesPaths[0], 
                compiledLibPath.singleChannelLibrariesPaths[1]
            );
        }

        eventHub->publish(EventType::compilationSuccess, compilationId);
    }
    else {
        try {
            sanitizeErrorString(errStr, hasMonoEntrypoint);
        } catch (std::exception&) {
            errStr = "[WARNING] Failed to sanitize error string\r\n" + errStr;
        }
        eventHub->publish(EventType::compilationFail, errStr);
    }
}

bool formula::compiler::CompilerWrapper::launchCompiler
(const std::string& compilerPath, const std::vector<std::string>& compileArgs, std::string& errStr) {

    boost::process::ipstream errReader;
    std::thread reader([&errReader, &errStr] {
        std::string line;
        while (std::getline(errReader, line)) {
            errStr += line + "\n";
        }
    });

#ifdef _WIN32
    boost::process::child c(compilerPath, compileArgs,
        boost::process::std_err > errReader, boost::process::windows::create_no_window);
#else
    boost::process::child c(compilerPath, compileArgs,
                            boost::process::std_err > errReader);
#endif

    while (c.running()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    c.wait();

    errReader.pipe().close();
    reader.join();

    return c.exit_code() == 0;
}

std::string formula::compiler::CompilerWrapper::replaceMacros(std::string str) {
    boost::replace_all(str, "__time", "TIME");
    boost::replace_all(str, "__sample_rate", "SAMPLE_RATE");
    boost::replace_all(str, "__knobs", "KNOB_i");
    boost::replace_all(str, "__switches", "SWITCH_i");

    return str;
}