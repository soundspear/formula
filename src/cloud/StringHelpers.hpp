/*
    Copyright (c) 2022 Soundspear.
    Distributed under the Boost Software License, Version 1.0.
    See http://www.boost.org/LICENSE_1_0.txt
*/

#ifndef FORMULA_CLOUD_STRINGHELPERS_INCLUDED
#define FORMULA_CLOUD_STRINGHELPERS_INCLUDED

#include <string>
#include <utility>
#include <codecvt>

namespace formula::cloud {
    namespace {
        /**
         * Converts a wide string to a narrow string
         */
        constexpr auto NARROW = [](utility::string_t w) {
            return utility::conversions::to_utf8string(w);
        };

        /**
         * Converts a narrow string to a wide string
         */
        constexpr auto WIDE = [](std::string s) {
            return utility::conversions::to_string_t(s);
        };
    }
}

#endif //FORMULA_CLOUD_STRINGHELPERS_INCLUDED
