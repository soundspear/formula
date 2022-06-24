#ifndef FORMULA_COMPILER_NOINLINEASSEMBLY_HPP
#define FORMULA_COMPILER_NOINLINEASSEMBLY_HPP

#include <src/compiler/SecurityGuard.hpp>

namespace formula::compiler {
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

#endif //FORMULA_COMPILER_NOINLINEASSEMBLY_HPP
