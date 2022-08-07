/*
    Copyright (c) 2022 Soundspear.
    Distributed under the Boost Software License, Version 1.0.
    See http://www.boost.org/LICENSE_1_0.txt
*/

#ifndef FORMULA_CLOUD_GITHUBCLIENT_INCLUDED
#define FORMULA_CLOUD_GITHUBCLIENT_INCLUDED

#include <cpprest/json.h>
#include <cpprest/http_msg.h>
#include <cpprest/http_client.h>

#include "events/EventHub.hpp"
#include "cloud/StringHelpers.hpp"

namespace formula::cloud {
    /**
     * Client to check for updates using the Github API
     */
    class GithubClient {
    public:
        explicit GithubClient(const std::shared_ptr<formula::events::EventHub>& eventHub);
        void checkForUpdates();
    private:
        static void parseVersion(const std::string& input, int result[3]);
        static bool isGreaterThanCurrentVersion(std::string semverStr);
        std::shared_ptr<formula::events::EventHub> eventHub;
        web::http::client::http_client client;
        pplx::cancellation_token_source destructorCts;
        std::shared_ptr<std::mutex> mutex;
    };
}

#endif //FORMULA_CLOUD_GITHUBCLIENT_INCLUDED
