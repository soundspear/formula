//
// Created by antoi on 25/03/2022.
//

#ifndef FORMULA_CLOUD_GETFORMULADTO_HPP
#define FORMULA_CLOUD_GETFORMULADTO_HPP

#include <string>

namespace formula::cloud {
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
        std::string source;
        std::vector<GetUserRatingDto> ratings;
    };
}

#endif //FORMULA_CLOUD_GETFORMULADTO_HPP
