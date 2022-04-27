#include "AuthenticatedClient.hpp"

formula::cloud::AuthenticatedClient::AuthenticatedClient(const std::shared_ptr<formula::storage::LocalSettings>& settings,
                                                         const std::shared_ptr<formula::events::EventHub>& eventHub,
                                                         utility::string_t baseUrl)
                                                         : settings(settings), eventHub(eventHub), client(baseUrl) {
    accessToken = settings->find<std::string>(storage::SettingKey::accessToken);
    refreshToken = settings->find<std::string>(storage::SettingKey::refreshToken);
    expiresAt = settings->find<unsigned long long>(storage::SettingKey::expiresAt);
}

void formula::cloud::AuthenticatedClient::login(std::string user, std::string password) {
    web::json::value body;
    body[W("username")] = web::json::value::string(W(user));
    body[W("password")] = web::json::value::string(W(password));

    web::json::value jsonResponse;
    client.request(web::http::methods::POST, U("/api/auth/login"), body, destructorCts.get_token())
            .then([this](const web::http::http_response& response) {
                if (response.status_code() == 400) {
                    eventHub->publish(EventType::loginFail);
                    return;
                }
                try {
                    auto jsonResponse = response.extract_json().get();
                    if (jsonResponse.is_null()) return;
                    processLoginResponse(jsonResponse);
                    eventHub->publish(EventType::loginSuccess);
                }
                catch (const std::exception&) {
                    eventHub->publish(EventType::unexpectedError);
                }
            });
}

pplx::task<void> formula::cloud::AuthenticatedClient::refreshAccessToken() {
    web::json::value body;
    body[W("refreshToken")] = web::json::value::string(W(refreshToken.value_or("")));

    web::json::value jsonResponse;
    return client.request(web::http::methods::POST, U("/api/auth/refreshAccessToken"), body, destructorCts.get_token())
            .then([this](const web::http::http_response& response) {
                if (response.status_code() == 400) {
                    eventHub->publish(EventType::needLogin);
                    return;
                }
                try {
                    auto jsonResponse = response.extract_json().get();
                    if (jsonResponse.is_null()) return;
                    processRefreshedTokenResponse(jsonResponse);
                }
                catch (const std::exception&) {
                    eventHub->publish(EventType::unexpectedError);
                }
            });
}

void formula::cloud::AuthenticatedClient::processLoginResponse(web::json::value jsonResponse) {
    accessToken = utility::conversions::to_utf8string(jsonResponse[W("accessToken")].as_string());
    refreshToken = utility::conversions::to_utf8string(jsonResponse[W("refreshToken")].as_string());
    expiresAt = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()).count()
                + jsonResponse[W("expiresIn")].as_integer();

    settings->add(storage::SettingKey::accessToken, accessToken);
    settings->add(storage::SettingKey::refreshToken, refreshToken);
    settings->add(storage::SettingKey::expiresAt, expiresAt);
}

void formula::cloud::AuthenticatedClient::processRefreshedTokenResponse(web::json::value jsonResponse) {
    accessToken = utility::conversions::to_utf8string(jsonResponse[W("accessToken")].as_string());
    expiresAt = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()).count()
                + jsonResponse[W("expiresIn")].as_integer();

    settings->add(storage::SettingKey::accessToken, accessToken);
    settings->add(storage::SettingKey::expiresAt, expiresAt);
}

bool formula::cloud::AuthenticatedClient::isTokenValid() {
    if (!accessToken || !refreshToken || !expiresAt)
        return false;
    const auto now = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
    return now < expiresAt;
}

web::http::http_request
formula::cloud::AuthenticatedClient::forgeAuthenticatedRequest(web::http::method method, std::string uri) {
    web::http::http_request req(method);
    req.headers().add(W("Authorization"), W("Bearer " + accessToken.value_or("")));
    req.set_request_uri(W(uri));

    return req;
}

formula::cloud::AuthenticatedClient::~AuthenticatedClient() {
    destructorCts.cancel();
}
