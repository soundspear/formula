#ifndef FORMULA_CLOUD_LISTFORMULADTO_INCLUDED
#define FORMULA_CLOUD_LISTFORMULADTO_INCLUDED

#include <string>
#include <optional>
#include "boost/date_time/posix_time/posix_time.hpp"

namespace formula::cloud {
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
