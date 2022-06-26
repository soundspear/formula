/*
    Copyright (c) 2022 Soundspear.
    Distributed under the Boost Software License, Version 1.0.
    See http://www.boost.org/LICENSE_1_0.txt
*/

#ifndef FORMULA_COMPILER_CLANGWRAPPER_INCLUDED
#define FORMULA_COMPILER_CLANGWRAPPER_INCLUDED

#include <boost/lexical_cast.hpp>

#include <compiler/CompilerWrapper.hpp>

namespace formula::compiler{
    /**
     * Wrapper to the Clang compiler
     */
    class ClangWrapper : public CompilerWrapper {
    public:
        explicit ClangWrapper(const std::shared_ptr<formula::events::EventHub>& eventHubRef);

    protected:
        void sanitizeErrorString(std::string& errStr, bool isMono) override;
        std::string getCompilerPath() override;
        std::vector<std::string> getCompilerArgs(std::string sourcePath, std::string outPath, bool isMono) override;
    };
}

#endif //FORMULA_COMPILER_CLANGWRAPPER_INCLUDED
