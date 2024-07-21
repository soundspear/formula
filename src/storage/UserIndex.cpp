//    Copyright (c) 2024 Soundspear.
//    Distributed under the Boost Software License, Version 1.0.
//    See http://www.boost.org/LICENSE_1_0.txt

#include "UserIndex.hpp"

formula::storage::UserIndex::UserIndex(
    const std::shared_ptr<formula::processor::PluginState>& pluginStateRef
)
    : LocalIndex(pluginStateRef)
{
    indexPath = storageFolder / "index.json";
}

void formula::storage::UserIndex::loadIndex()
{
    if (!boost::filesystem::exists(indexPath))
    {
        auto defaultIndex = std::string(formula::binary::default_index_json, formula::binary::default_index_jsonSize);
        std::ofstream file(indexPath.string());
        file << defaultIndex;
        file.close();
    }
    boost::property_tree::read_json(indexPath.string(), index);
}

void formula::storage::UserIndex::saveIndex()
{
    boost::property_tree::write_json(indexPath.string(), index);
}