/*
    Copyright (c) 2022 Soundspear.
    Distributed under the Boost Software License, Version 1.0.
    See http://www.boost.org/LICENSE_1_0.txt
*/

#ifndef FORMULA_COMPILER_NOPREPROCESSORGUARD_INCLUDED
#define FORMULA_COMPILER_NOPREPROCESSORGUARD_INCLUDED

#include <regex>

#include <src/compiler/SecurityGuard.hpp>

namespace formula::compiler {
    /**
     * Check that there are no preprocessor usages in a formula
     */
    class NoPreprocessorGuard : public SecurityGuard {
    public:
        NoPreprocessorGuard() : noPreprocessorRegex(R"(^\s*#.*)") { }

        bool checkLine(std::string& line) override {
            auto found = std::regex_search(line, noPreprocessorRegex);
            if (found) {
                setErrorMessage(
                    line + "\r\n" +
                    "\tPreprocessor definitions (#include, #define, ...) are not allowed in formulas for safety reasons."
                );
            }
            return found;
        }

    private:
        std::regex noPreprocessorRegex;
    };
}

#endif //FORMULA_COMPILER_NOPREPROCESSORGUARD_INCLUDED
