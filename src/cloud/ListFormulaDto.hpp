/*
    Copyright (c) 2022 Soundspear.
    Distributed under the Boost Software License, Version 1.0.
    See http://www.boost.org/LICENSE_1_0.txt
*/

#ifndef FORMULA_CLOUD_LISTFORMULADTO_INCLUDED
#define FORMULA_CLOUD_LISTFORMULADTO_INCLUDED

#include <string>
#include <optional>
#include "boost/date_time/posix_time/posix_time.hpp"

namespace formula::cloud {
    /**
     * Data object for listing the available formulas
     */
    struct ListFormulaDto {
        std::string id;
        std::string author;
        std::string name;
        std::string description;
        std::optional<double> rating;
        boost::posix_time::ptime created;
        boost::posix_time::ptime lastModified;
    };
}

#endif //FORMULA_CLOUD_LISTFORMULADTO_INCLUDED
