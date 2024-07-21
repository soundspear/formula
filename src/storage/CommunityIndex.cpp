/*
    Copyright (c) 2024 Soundspear.
    Distributed under the Boost Software License, Version 1.0.
    See http://www.boost.org/LICENSE_1_0.txt
 */

#include "CommunityIndex.hpp"

void formula::storage::CommunityIndex::loadIndex()
{
    boost::iostreams::basic_array_source<char> input_source(
        formula::binary::community_index_json,
        formula::binary::community_index_jsonSize
    );
    boost::iostreams::stream input_stream(input_source);
    boost::property_tree::read_json(input_stream, index);
}

void formula::storage::CommunityIndex::saveIndex()
{
    throw std::logic_error("The community index is immutable");
}
