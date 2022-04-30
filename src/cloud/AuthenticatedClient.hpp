#ifndef FORMULA_CLOUD_CLOUDAUTHENTICATION_INCLUDED
#define FORMULA_CLOUD_CLOUDAUTHENTICATION_INCLUDED

#include <string>
#include <optional>

#include <cpprest/json.h>
#include <cpprest/http_msg.h>
#include <cpprest/http_client.h>

#include "gui/ErrorCodes.hpp"
#include "storage/LocalSettings.hpp"
#include "events/EventHub.hpp"
#include "cloud/StringHelpers.hpp"

namespace formula::cloud {
    class AuthenticatedClient {
    public:
        void login(std::string user, std::string password);
        void setUsername(std::string username);
        virtual ~AuthenticatedClient();

    protected:
        explicit AuthenticatedClient(const std::shared_ptr<formula::storage::LocalSettings>& settings,
                                     const std::shared_ptr<formula::events::EventHub>& eventHub,
                                     utility::string_t baseUrl);

        pplx::task<void> refreshAccessToken();
        void processLoginResponse(web::json::value jsonResponse);
        void processRefreshedTokenResponse(web::json::value jsonResponse);

        bool hasUserName();
        bool tokenExists();
        web::http::http_request forgeAuthenticatedRequest(web::http::method method, std::string uri);

        std::shared_ptr<formula::events::EventHub> eventHub;
        web::http::client::http_client client;
        pplx::cancellation_token_source destructorCts;
        std::shared_ptr<std::mutex> mutex;
    private:
        void checkUsername(web::json::value tokenResponse);

        std::shared_ptr<formula::storage::LocalSettings> settings;

        std::optional<std::string> accessToken;
        std::optional<std::string> refreshToken;
        std::optional<unsigned long long> expiresAt;
        std::optional<std::string> userName;
    };
}


#endif //FORMULA_CLOUD_CLOUDAUTHENTICATION_INCLUDED
