/*
    Copyright (c) 2022 Soundspear.
    Distributed under the Boost Software License, Version 1.0.
    See http://www.boost.org/LICENSE_1_0.txt
*/

#ifndef FORMULA_COMPILER_NOSYSTEMCALLGUARD_INCLUDED
#define FORMULA_COMPILER_NOSYSTEMCALLGUARD_INCLUDED

#include <regex>

#include <src/compiler/SecurityGuard.hpp>

namespace formula::compiler {
    /**
     * Check that there is no exec or system calls in a formula
     */
    class NoSystemCallGuard : public SecurityGuard {
    public:
        NoSystemCallGuard() :
            noSystemRegex(
                    R"(\s*system\s*(?:\(|$))"
            ),
            noExecRegex(
                    R"(\s*(exec|execl|execlp|execv|execvp|execle|execve)\s*(?:\(|$))"
            )
        { }

        bool checkLine(std::string& line) override {
            auto found = std::regex_search(line, noSystemRegex) || std::regex_search(line, noExecRegex) ;
            if (found) {
                setErrorMessage(
                        line + "\r\n" +
                        "\tSystem and exec calls are not allowed in formulas for safety reasons."
                );
            }
            return found;
        }

    private:
        std::regex noSystemRegex;
        std::regex noExecRegex;
    };
}

#endif //FORMULA_COMPILER_NOSYSTEMCALLGUARD_INCLUDED
