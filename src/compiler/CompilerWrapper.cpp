#include <compiler/CompilerWrapper.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

using namespace boost::assign;

formula::compiler::CompilerWrapper::CompilerWrapper(const std::shared_ptr<formula::events::EventHub>& eventHub)
    : eventHub(eventHub)
{
#if defined (_WIN32)
    clangPath = boost::filesystem::path(R"(C:\Program Files\tcc\tcc.exe)");
    clangArgs += "-shared";
#elif defined(__APPLE__ )
    clangPath = boost::process::search_path("clang");
    clangArgs += "-dynamiclib";
#endif
    clangArgs += "-fvisibility=hidden";

    eventHub->subscribe(EventType::compilationRequest, [this](boost::any arg) {
        std::lock_guard<std::mutex> lock { compileMutex };
        if (compileThread.joinable()) {
            compileThread.join();
        }
        auto sourceCode = boost::any_cast<std::string>(arg);
        compileThread = std::thread{&formula::compiler::CompilerWrapper::compileFormula, this, sourceCode };
    });

    formulaBaseCodeMono = std::string(formula::binary::FormulaBaseMono_c, formula::binary::FormulaBaseMono_cSize);
    formulaBaseCodeStereo = std::string(formula::binary::FormulaBaseStereo_c, formula::binary::FormulaBaseStereo_cSize);
}
void formula::compiler::CompilerWrapper::sanitizeErrorString(std::string& errStr, bool isMono) {
    const auto& sourceBase = isMono ? formulaBaseCodeMono : formulaBaseCodeStereo;
    int lineNumberInBaseFile = std::count(sourceBase.begin(), sourceBase.end(), '\n');

    boost::replace_all(errStr, "\r\n", "\n");
    std::vector<std::string> strs;
    boost::split(strs,errStr,boost::is_any_of("\n"));

    std::regex lineNumberError(R"(^.*\.c:(\d+)(.*)$)");
    std::regex compilerError(R"(tcc:\s+)");
    errStr.clear();
    for (auto& line : strs) {
        line = std::regex_replace(line, compilerError, "");

        std::smatch matches;
        std::regex_search(line, matches, lineNumberError);
        if (matches.empty()) {
            errStr += line + "\r\n";
            continue;
        }

        auto lineNumber = boost::lexical_cast<int>(matches[1]);
        lineNumber -= lineNumberInBaseFile;
        std::string errorMessage = matches[2];
        errStr += "Line " + std::to_string(lineNumber) + errorMessage + "\r\n";
    }
}

void formula::compiler::CompilerWrapper::compileFormula(const std::string& sourceCode)
{
    formula::storage::CompilerStorage storage;
    const auto compilationId = storage.createCompilationId();

    const auto compiledLibPath = storage.getLibraryPath(compilationId);

    std::string errStr;
    auto compileArgs = std::vector(clangArgs);
    const auto hasMonoEntrypoint =
        std::regex_search(sourceCode,
            std::regex(monoFormulaEntrypoint + std::string(R"(\s*\{)")));
    const auto hasStereoEntrypoint =
        std::regex_search(sourceCode,
            std::regex(stereoFormulaEntrypoint + std::string(R"(\s*\{)")));

    std::string sourcePath;
    if (hasMonoEntrypoint) {
        compileArgs += "-o", compiledLibPath.singleChannelLibrariesPaths[0];
        sourcePath = storage.createSourceFile(compilationId, formulaBaseCodeMono + sourceCode);
    }
    else if (hasStereoEntrypoint) {
        compileArgs += "-o", compiledLibPath.twoChannelsLibraryPath;
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

    compileArgs += sourcePath;
    bool success = launchClang(compileArgs, errStr);

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
        } catch (std::exception& ex) {
            errStr = "[WARNING] Failed to sanitize error string\r\n" + errStr;
        }
        eventHub->publish(EventType::compilationFail, errStr);
    }
}

bool formula::compiler::CompilerWrapper::launchClang
(std::vector<std::string> compileArgs, std::string& errStr) {

    boost::process::ipstream errReader;
    std::thread reader([&errReader, &errStr] {
        std::string line;
        while (std::getline(errReader, line)) {
            errStr += line + "\n";
        }
    });

#ifdef _WIN32
    boost::process::child c(clangPath, compileArgs,
        boost::process::std_err > errReader, boost::process::windows::create_no_window);
#else
    boost::process::child c(clangPath, compileArgs,
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

formula::compiler::CompilerWrapper::~CompilerWrapper() {
    if (compileThread.joinable()) 
        compileThread.join();
}
