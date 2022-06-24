#ifndef FORMULA_CLOUD_GETFORMULADTO_INCLUDED
#define FORMULA_CLOUD_GETFORMULADTO_INCLUDED

#include <processor/FormulaMetadata.hpp>
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
        formula::processor::FormulaMetadata metadata;
        std::vector<GetUserRatingDto> ratings;
    };
}

#endif //FORMULA_CLOUD_GETFORMULADTO_INCLUDED
