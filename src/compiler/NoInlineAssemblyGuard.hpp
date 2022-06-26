/*
    Copyright (c) 2022 Soundspear.
    Distributed under the Boost Software License, Version 1.0.
    See http://www.boost.org/LICENSE_1_0.txt
*/

#ifndef FORMULA_COMPILER_NOINLINEASSEMBLYGUARD_INCLUDED
#define FORMULA_COMPILER_NOINLINEASSEMBLYGUARD_INCLUDED

#include <src/compiler/SecurityGuard.hpp>

namespace formula::compiler {
    /**
     * Check that there is no inline assembly in a formula
     */
    class NoInlineAssemblyGuard : public SecurityGuard {
    public:
        bool checkLine(std::string& line) override {
            // __asm__ for clang/gcc/tcc, __asm for msvc
            bool found = line.find("__asm") != std::string::npos;
            if (found) {
                setErrorMessage(
                        line + "\r\n" +
                        "\tInline assembly is not allowed in formulas for safety reasons."
                );
            }
            return found;
        }
    };
}

#endif //FORMULA_COMPILER_NOINLINEASSEMBLYGUARD_INCLUDED
