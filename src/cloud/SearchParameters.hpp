/*
    Copyright (c) 2022 Soundspear.
    Distributed under the Boost Software License, Version 1.0.
    See http://www.boost.org/LICENSE_1_0.txt
*/

#ifndef FORMULA_CLOUD_SEARCHPARAMETERS_INCLUDED
#define FORMULA_CLOUD_SEARCHPARAMETERS_INCLUDED

#include <string>

namespace formula::cloud {
    struct SearchParameters {
        SearchParameters() {
            skip = 0;
            take = 50;
        }

        int skip;
        int take;
    };
}

#endif //FORMULA_CLOUD_SEARCHPARAMETERS_INCLUDED
