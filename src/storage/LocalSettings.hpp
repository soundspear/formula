/*
    Copyright (c) 2022 Soundspear.
    Distributed under the Boost Software License, Version 1.0.
    See http://www.boost.org/LICENSE_1_0.txt
*/

#ifndef FORMULA_STORAGE_LOCALSETTINGS_INCLUDED
#define FORMULA_STORAGE_LOCALSETTINGS_INCLUDED

#include <string>
#include <iostream>
#include <optional>

#include <boost/property_tree/ptree.hpp>
#include "LocalStorage.hpp"

namespace formula::storage {
    struct SettingKey {
        inline static const std::string accessToken = "accessToken";
        inline static const std::string refreshToken = "refreshToken";
        inline static const std::string expiresAt = "expiresAt";
        inline static const std::string username = "username";
        inline static const std::string email = "email";
        inline static const std::string windowResolution = "windowResolution";
    };

    /**
     * Manages the application persistent settings
     */
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
            remove(key);
            settings.add(key, value.value());
            saveSettings();
        }

        void remove(std::string key) {
            if (settings.get_child_optional(key)) {
                settings.erase(key);
            }
        }

    private:
        void loadSettings();
        void saveSettings();
        boost::property_tree::ptree settings;
        boost::filesystem::path settingsPath;
    };
}


#endif //FORMULA_STORAGE_LOCALSETTINGS_INCLUDED
