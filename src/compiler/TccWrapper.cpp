#include <compiler/TccWrapper.hpp>

using namespace boost::assign;

formula::compiler::TccWrapper::TccWrapper(const std::shared_ptr<formula::events::EventHub>& eventHubRef)
    : formula::compiler::CompilerWrapper(eventHubRef)
{
}

void formula::compiler::TccWrapper::sanitizeErrorString(std::string& errStr, bool isMono)
{
    const auto& sourceBase = isMono ? getBaseCodeMono() : getBaseCodeStereo();
    auto lineNumberInBaseFile = std::count(sourceBase.begin(), sourceBase.end(), '\n');

    boost::replace_all(errStr, "\r\n", "\n");
    std::vector<std::string> strs;
    boost::split(strs, errStr, boost::is_any_of("\n"));

    std::regex lineNumberError(R"(^.*\.c:(\d+)(.*)$)");
    std::regex compilerError(R"(tcc:\s+)");
    errStr.clear();
    for (auto& line : strs) {
        line = std::regex_replace(line, compilerError, "");

        line = replaceMacros(line);

        std::smatch matches;
        std::regex_search(line, matches, lineNumberError);
        if (matches.empty()) {
            errStr += line + "\r\n";
            continue;
        }

        auto lineNumber = boost::lexical_cast<long long>(matches[1]);
        lineNumber -= lineNumberInBaseFile;
        if (lineNumber < 0) continue;
        std::string errorMessage = matches[2];
        errStr += "Line " + std::to_string(lineNumber) + errorMessage + "\r\n";
    }
}

std::string formula::compiler::TccWrapper::getCompilerPath()
{
    auto tccPath = boost::process::search_path("tcc");
    if (tccPath.empty() || !boost::filesystem::exists(tccPath)) {
#       if defined (_WIN32)
        tccPath = boost::filesystem::path(R"(C:\Program Files\tcc\tcc.exe)");
        if (!boost::filesystem::exists(tccPath)) {
            tccPath = boost::filesystem::path(R"(C:\Program Files (x86)\tcc\tcc.exe)");
        }
        if (!boost::filesystem::exists(tccPath)) {
            tccPath = boost::filesystem::path(R"(C:\tcc\tcc.exe)");
        }
#       elif defined(__APPLE__)
        tccPath = boost::filesystem::path(R"(/usr/bin/tcc)");
        if (!boost::filesystem::exists(tccPath)) {
            tccPath = boost::filesystem::path(R"(/usr/local/bin/tcc)");
        }
#       endif
    }

    if (!boost::filesystem::exists(tccPath)) {
        return "";
    }

    return tccPath.string();
}

std::vector<std::string> formula::compiler::TccWrapper::getCompilerArgs(std::string sourcePath, std::string outPath, bool isMono)
{
    std::vector<std::string> args;

    args += "-shared";
    args += "-o", outPath;
    args += sourcePath;

    return args;
}
