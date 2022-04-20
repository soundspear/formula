#include <compiler/ClangWrapper.hpp>

using namespace boost::assign;

formula::compiler::ClangWrapper::ClangWrapper(const std::shared_ptr<formula::events::EventHub>& eventHubRef)
        : formula::compiler::CompilerWrapper(eventHubRef)
{
}

void formula::compiler::ClangWrapper::sanitizeErrorString(std::string& errStr, bool isMono)
{
    const auto& sourceBase = isMono ? getBaseCodeMono() : getBaseCodeStereo();
    auto lineNumberInBaseFile = std::count(sourceBase.begin(), sourceBase.end(), '\n');

    boost::replace_all(errStr, "\r\n", "\n");
    std::vector<std::string> strs;
    boost::split(strs, errStr, boost::is_any_of("\n"));

    std::regex lineNumberError(R"(^.*\.c:(\d+)(.*)$)");

    errStr.clear();

    for (auto& line : strs) {
        std::smatch matches;
        std::regex_search(line, matches, lineNumberError);
        if (matches.empty()) {
            errStr += line + "\r\n";
            continue;
        }

        auto lineNumber = boost::lexical_cast<long long>(matches[1]);
        lineNumber -= lineNumberInBaseFile;
        std::string errorMessage = matches[2];
        errStr += "Line " + std::to_string(lineNumber) + errorMessage + "\r\n";
    }
}

std::string formula::compiler::ClangWrapper::getCompilerPath()
{
    auto clangPath = boost::process::search_path("clang");
    if (clangPath.empty() || !boost::filesystem::exists(clangPath)) {
#       if defined (_WIN32)
        clangPath = boost::filesystem::path(R"(C:\Program Files\LLVM\bin\clang.exe)");
#       elif defined(__APPLE__)
        clangPath = boost::filesystem::path(R"(/usr/bin/clang)");
        if (!boost::filesystem::exists(clangPath)) {
            clangPath = boost::filesystem::path(R"(/usr/local/bin/clang)");
        }
#       endif
    }

    if (!boost::filesystem::exists(clangPath)) {
        return "";
    }

    return clangPath.string();
}

std::vector<std::string> formula::compiler::ClangWrapper::getCompilerArgs(std::string sourcePath, std::string outPath, bool isMono)
{
    std::vector<std::string> args;

#if defined (_WIN32)
    args += "-shared";
#elif defined(__APPLE__ )
    args += "-dynamiclib";
#endif
    args += "-fvisibility=hidden";

    args += "-o", outPath;
    args += sourcePath;

    return args;
}
