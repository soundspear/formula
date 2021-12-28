#ifndef FORMULA_CLOUD_STRINGHELPERS_INCLUDED
#define FORMULA_CLOUD_STRINGHELPERS_INCLUDED

#include <string>
#include <utility>
#include <codecvt>

namespace formula::cloud {
    namespace {
        constexpr auto S = [](utility::string_t w) {
            return utility::conversions::to_utf8string(w);
        };

        constexpr auto W = [](std::string s) {
            return utility::conversions::to_string_t(s);
        };
    }
}

#endif //FORMULA_CLOUD_STRINGHELPERS_INCLUDED
