#include "LocalIndex.hpp"

using namespace formula::processor;

formula::storage::LocalIndex::LocalIndex(
    const std::shared_ptr<formula::events::EventHub>& eventHub,
    const std::shared_ptr<formula::processor::PluginState>& pluginState
)
    : formula::storage::LocalStorage(),
    eventHub(eventHub), pluginState(pluginState)
{
    indexPath = storageFolder / "index.json";

    eventHub->subscribe(EventType::saveLocalFormulaRequest, [this](boost::any _) {
        this->saveCurrentFormulaToIndex();
    });

    refreshIndex();
}

void formula::storage::LocalIndex::refreshIndex()
{
    if (!boost::filesystem::exists(indexPath))
    {
        index = boost::property_tree::ptree();
    }
    else 
    {
        boost::property_tree::read_json(indexPath.string(), index);
    }
}

void formula::storage::LocalIndex::saveIndex()
{
    boost::property_tree::write_json(indexPath.string(), index);
}

void formula::storage::LocalIndex::saveCurrentFormulaToIndex()
{
    auto metadata = pluginState->getActiveFormulaMetadata();

    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%d-%m-%Y %H:%M");
    const auto dateNow = oss.str();

    if (index.get_child_optional(metadata[FormulaMetadataKeys::name]) == boost::none) {
        metadata[FormulaMetadataKeys::created] = dateNow;
    }
    metadata[formula::processor::FormulaMetadataKeys::lastModified] = dateNow;

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
    return LocalIndexIterator(index.begin());
}

formula::storage::LocalIndexIterator formula::storage::LocalIndex::end()
{
    return LocalIndexIterator(index.end());
}
