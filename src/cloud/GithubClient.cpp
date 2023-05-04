/*
    Copyright (c) 2022 Soundspear.
    Distributed under the Boost Software License, Version 1.0.
    See http://www.boost.org/LICENSE_1_0.txt
*/

#include "GithubClient.hpp"

formula::cloud::GithubClient::GithubClient(const std::shared_ptr<formula::events::EventHub>& eventHubRef)
: eventHub(eventHubRef), client(WIDE("https://api.github.com/"))  { }

void formula::cloud::GithubClient::checkForUpdates() {
    client.request(web::http::methods::GET, WIDE("/repos/soundspear/formula/releases"), destructorCts.get_token())
        .then([this](const web::http::http_response& response) mutable {
            if (response.status_code() != 200) {
                return;
            }
            try {
                auto jsonResponse = response.extract_json().get();
                auto releases = jsonResponse.as_array();

                for (auto& release : releases) {
                    if (release[WIDE("prerelease")].as_bool()) {
                        continue;
                    }
                    auto releaseVersion = NARROW(release[WIDE("tag_name")].as_string());
                    if (isGreaterThanCurrentVersion(releaseVersion)) {
                        eventHub->publish(EventType::newVersionReleased, releaseVersion);
                    }
                    break; // First release will always be the most recent
                }
            }
            catch (const std::exception&) { /* Silence check for update errors */ }
        })
        .then([] (const pplx::task<void>& previous_task) {
            try {
                previous_task.wait();
            } catch (std::exception& e) { /* ignore */ }
        });
}

void formula::cloud::GithubClient::parseVersion(const std::string& input, int result[3]) {
    std::istringstream parser(input);
    parser >> result[0];
    for(int idx = 1; idx < 3; idx++)
    {
        parser.get();
        parser >> result[idx];
    }
}

bool formula::cloud::GithubClient::isGreaterThanCurrentVersion(std::string semverStr) {
    int currentVersion[3], semver[3];
    parseVersion(FORMULA_VERSION, currentVersion);
    parseVersion(semverStr, semver);
    return std::lexicographical_compare(currentVersion, currentVersion + 3, semver, semver + 3);
}
