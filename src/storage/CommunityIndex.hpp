/*
    Copyright (c) 2024 Soundspear.
    Distributed under the Boost Software License, Version 1.0.
    See http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef FORMULA_STORAGE_COMMUNITYINDEX_INCLUDED
#define FORMULA_STORAGE_COMMUNITYINDEX_INCLUDED

#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/array.hpp>
#include <BinaryData.h>
#include <storage/LocalIndex.hpp>

namespace formula::storage {
    class CommunityIndex : public LocalIndex {
    public:
        void loadIndex() override;
        void saveIndex() override;
    };
}

#endif // FORMULA_STORAGE_COMMUNITYINDEX_INCLUDED
