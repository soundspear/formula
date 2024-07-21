/*
    Copyright (c) 2022 Soundspear.
    Distributed under the Boost Software License, Version 1.0.
    See http://www.boost.org/LICENSE_1_0.txt
*/

#ifndef FORMULA_HTTP_SEARCHPARAMETERS_INCLUDED
#define FORMULA_HTTP_SEARCHPARAMETERS_INCLUDED

#include <string>

namespace formula::http {
    /**
     * Stores persistent search parameters
     */
    struct SearchParameters {
        SearchParameters() {
            skip = 0;
            take = 50;
        }

        int skip;
        int take;
    };
}

#endif //FORMULA_HTTP_SEARCHPARAMETERS_INCLUDED
