/*
    Copyright (c) 2022 Soundspear.
    Distributed under the Boost Software License, Version 1.0.
    See http://www.boost.org/LICENSE_1_0.txt
*/

#ifndef FORMULA_CLOUD_GETFORMULADTO_INCLUDED
#define FORMULA_CLOUD_GETFORMULADTO_INCLUDED

#include <processor/FormulaMetadata.hpp>
#include <string>

namespace formula::cloud {
    /**
     * Data Object for retrieving a formula
     */
    struct GetFormulaDto {
        struct GetUserRatingDto {
            std::string comment;
            double rating;
        };

        std::string author;
        std::string name;
        std::string description;
        int numDownloads;
        std::optional<double> rating;
        int numRatings;
        formula::processor::FormulaMetadata metadata;
        std::vector<GetUserRatingDto> ratings;
    };
}

#endif //FORMULA_CLOUD_GETFORMULADTO_INCLUDED
