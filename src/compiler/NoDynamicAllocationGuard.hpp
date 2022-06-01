#ifndef FORMULA_COMPILER_NODYNAMICALLOCATIONGUARD_HPP
#define FORMULA_COMPILER_NODYNAMICALLOCATIONGUARD_HPP

#include <regex>

#include <src/compiler/SecurityGuard.hpp>

namespace formula::compiler {
    class NoDynamicAllocationGuard : public SecurityGuard {
    public:
        NoDynamicAllocationGuard() : noDynamicAllocationRegex(
                R"(\s*(malloc|realloc|calloc|free|new|delete)\s*(?:\(|$))"
        ) { }

        bool checkLine(std::string& line) override {
            auto found = std::regex_search(line, noDynamicAllocationRegex);
            if (found) {
                setErrorMessage(
                        line + "\r\n" +
                        "\tDynamic allocation (malloc, free, ...) is not allowed in formulas for safety reasons."
                );
            }
            return found;
        }

    private:
        std::regex noDynamicAllocationRegex;
    };
}

#endif //FORMULA_COMPILER_NODYNAMICALLOCATIONGUARD_HPP