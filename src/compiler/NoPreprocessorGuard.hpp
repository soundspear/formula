#ifndef FORMULA_COMPILER_NOINCLUDEGUARD_INCLUDED
#define FORMULA_COMPILER_NOINCLUDEGUARD_INCLUDED

#include <regex>

#include <src/compiler/SecurityGuard.hpp>

namespace formula::compiler {
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

#endif //FORMULA_COMPILER_NOINCLUDEGUARD_INCLUDED
