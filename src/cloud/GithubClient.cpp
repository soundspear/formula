/*
    Copyright (c) 2022 Soundspear.
    Distributed under the Boost Software License, Version 1.0.
    See http://www.boost.org/LICENSE_1_0.txt
*/

#include "GithubClient.hpp"

formula::cloud::GithubClient::GithubClient(const std::shared_ptr<formula::events::EventHub>& eventHub)
: eventHub(eventHub), client(WIDE("https://api.github.com/"))  { }

void formula::cloud::GithubClient::checkForUpdates() {
    web::json::value jsonResponse;
    client.request(web::http::methods::GET, WIDE("/repos/soundspear/formula/releases"), destructorCts.get_token())
            .then([this](const web::http::http_response& response) {
                if (response.status_code() != 200) {
                    return;
                }
                try {
                    auto jsonResponse = response.extract_json().get();
                    auto releases = jsonResponse.as_array();

                    std::optional<std::string> newerVersion;
                    for (auto& release : releases) {
                        if (release[WIDE("prerelease")].as_bool()) {
                            continue;
                        }
                        auto releaseVersion = NARROW(release[WIDE("tag_name")].as_string());
                        if (releaseVersion != FORMULA_VERSION) {
                            newerVersion = releaseVersion;
                        }
                        break; // First release will always be the most recent
                    }

                    if (newerVersion.has_value()) {
                        eventHub->publish(EventType::newVersionReleased, newerVersion.value());
                    }
                }
                catch (const std::exception&) { /* Silence check for update errors */ }
            });
}
