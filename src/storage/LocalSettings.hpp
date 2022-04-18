//
// Created by antoi on 23/03/2022.
//

#ifndef FORMULA_STORAGE_LOCALSETTINGS_INCLUDED
#define FORMULA_STORAGE_LOCALSETTINGS_INCLUDED

#include <string>
#include <iostream>

#include <boost/property_tree/ptree.hpp>
#include "LocalStorage.hpp"

namespace formula::storage {
    struct SettingKey {
        inline static const std::string accessToken = "accessToken";
        inline static const std::string refreshToken = "refreshToken";
        inline static const std::string expiresAt = "expiresAt";
        inline static const std::string username = "username";
        inline static const std::string email = "email";
    };

    class LocalSettings : public LocalStorage {
    public:
        LocalSettings();

        template <typename T>
        std::optional<T> find(std::string key){
            loadSettings();
            const auto child = settings.get_child_optional(key);
            return child ? std::optional<T>(child.value().get_value<T>()) : std::nullopt;
        }

        template <typename T>
        void add(std::string key, std::optional<T> value){
            if (settings.get_child_optional(key)) {
                settings.erase(key);
            }
            settings.add(key, value.value());
            saveSettings();
        }

    private:
        void loadSettings();
        void saveSettings();
        boost::property_tree::ptree settings;
        boost::filesystem::path settingsPath;
    };
}


#endif //FORMULA_STORAGE_LOCALSETTINGS_INCLUDED
