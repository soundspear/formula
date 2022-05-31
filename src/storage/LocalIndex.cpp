#include "LocalIndex.hpp"

using namespace formula::processor;

formula::storage::LocalIndex::LocalIndex(
    const std::shared_ptr<formula::processor::PluginState>& pluginStateRef
)
    : formula::storage::LocalStorage(),
    pluginState(pluginStateRef)
{
    indexPath = storageFolder / "index.json";

    refreshIndex();
}

void formula::storage::LocalIndex::refreshIndex()
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

void formula::storage::LocalIndex::saveIndex()
{
    boost::property_tree::write_json(indexPath.string(), index);
}

void formula::storage::LocalIndex::saveCurrentFormulaToIndex()
{
    auto metadata = pluginState->getActiveFormulaMetadata();

    auto dateNow = boost::posix_time::second_clock::local_time();
    const auto dateNowStr = boost::posix_time::to_simple_string(dateNow);

    if (index.get_child_optional(metadata[FormulaMetadataKeys::name]) == boost::none) {
        metadata[FormulaMetadataKeys::created] = dateNowStr;
    }
    metadata[formula::processor::FormulaMetadataKeys::lastModified] = dateNowStr;

    addFormulaToIndex(metadata, true);

    this->pluginState->setActiveFormulaMetadata(metadata);

    saveIndex();
}

void formula::storage::LocalIndex::addFormulaToIndex(formula::processor::FormulaMetadata& metadata, bool overrideExisting)
{
    if (index.get_child_optional(metadata[FormulaMetadataKeys::name]) != boost::none) {
        if (overrideExisting) {
            index.erase(metadata[FormulaMetadataKeys::name]);
        }
        else {
            std::string newName;
            auto i = 1;
            do {
                newName = metadata[FormulaMetadataKeys::name] + " (" + std::to_string(i) + ")";
                i++;
            } while (index.get_child_optional(newName) != boost::none);
            metadata[FormulaMetadataKeys::name] = newName;
        }
    }

    boost::property_tree::ptree newFormula;
    for (auto keyPair : metadata) {
        newFormula.add(std::string(keyPair.first), keyPair.second);
    }

    index.add_child(metadata[FormulaMetadataKeys::name], newFormula);

    saveIndex();
}

void formula::storage::LocalIndex::deleteFormula(std::string name)
{
    auto formula = index.get_child_optional(name);

    if (!formula) return;

    index.erase(name);
    saveIndex();
}

formula::storage::LocalIndexIterator formula::storage::LocalIndex::begin()
{
    return { index.begin() };
}

formula::storage::LocalIndexIterator formula::storage::LocalIndex::end()
{
    return { index.end() };
}

std::string formula::storage::LocalIndex::serializeMetadata(const formula::processor::FormulaMetadata& metadata) {
    boost::property_tree::ptree exportedProperties;
    for (const auto& keyPair : metadata) {
        exportedProperties.add(std::string(keyPair.first), keyPair.second);
    }
    std::stringstream ss;
    boost::property_tree::write_json(ss, exportedProperties);

    return ss.str();
}

formula::processor::FormulaMetadata formula::storage::LocalIndex::deserializeMetadata(const std::string& metadataStr) {
    std::stringstream ss;
    ss.str(metadataStr);
    boost::property_tree::ptree importedProperties;
    boost::property_tree::read_json(ss, importedProperties);
    formula::processor::FormulaMetadata metadata;

    for (auto it = importedProperties.begin(); it != importedProperties.end(); ++it) {
        metadata[it->first] = it->second.data();
    }

    return metadata;
}
