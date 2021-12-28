//
// Created by antoi on 23/03/2022.
//

#include <boost/property_tree/json_parser.hpp>
#include "LocalSettings.hpp"

formula::storage::LocalSettings::LocalSettings()
    : formula::storage::LocalStorage() {
    settingsPath = storageFolder / "settings.json";
    loadSettings();
}

void formula::storage::LocalSettings::loadSettings() {
    if (!boost::filesystem::exists(settingsPath))
    {
        settings = boost::property_tree::ptree();
    }
    else
    {
        boost::property_tree::read_json(settingsPath.string(), settings);
    }
}

void formula::storage::LocalSettings::saveSettings() {
    boost::property_tree::write_json(settingsPath.string(), settings);
}
