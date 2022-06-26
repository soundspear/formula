/*
    Copyright (c) 2022 Soundspear.
    Distributed under the Boost Software License, Version 1.0.
    See http://www.boost.org/LICENSE_1_0.txt
*/

#include "AuthenticatedClient.hpp"

formula::cloud::AuthenticatedClient::AuthenticatedClient(const std::shared_ptr<formula::storage::LocalSettings>& settings,
                                                         const std::shared_ptr<formula::events::EventHub>& eventHub,
                                                         utility::string_t baseUrl)
                                                         : settings(settings), eventHub(eventHub), client(baseUrl) {
    accessToken = settings->find<std::string>(storage::SettingKey::accessToken);
    refreshToken = settings->find<std::string>(storage::SettingKey::refreshToken);
    expiresAt = settings->find<unsigned long long>(storage::SettingKey::expiresAt);
    userName = settings->find<std::string>(storage::SettingKey::username);
}

void formula::cloud::AuthenticatedClient::login(std::string user, std::string password) {
    web::json::value body;
    body[WIDE("username")] = web::json::value::string(WIDE(user));
    body[WIDE("password")] = web::json::value::string(WIDE(password));

    web::json::value jsonResponse;
    client.request(web::http::methods::POST, WIDE("/api/auth/login"), body, destructorCts.get_token())
            .then([this](const web::http::http_response& response) {
                if (response.status_code() == 400) {
                    eventHub->publish(EventType::loginFail);
                    return;
                }
                try {
                    auto jsonResponse = response.extract_json().get();
                    if (jsonResponse.is_null()) return;
                    processLoginResponse(jsonResponse);
                    bool hasUsername = settings->find<std::string>(storage::SettingKey::username).has_value();
                    if (hasUsername) {
                        eventHub->publish(EventType::loginSuccess);
                    }
                }
                catch (const std::exception&) {
                    eventHub->publish(EventType::unexpectedError);
                }
            });
}

void formula::cloud::AuthenticatedClient::logout() {
    accessToken.reset(); refreshToken.reset(); expiresAt.reset(); userName.reset();
}

void formula::cloud::AuthenticatedClient::setUsername(std::string newUserName) {
    if (!isLoggedIn()) {
        eventHub->publish(EventType::needLogin);
        return;
    }

    web::json::value body;
    body[WIDE("username")] = web::json::value::string(WIDE(newUserName));

    auto request = forgeAuthenticatedRequest(web::http::methods::POST, "/api/auth/username");
    request.set_body(body);
    web::json::value jsonResponse;
    client.request(request, destructorCts.get_token())
            .then([this, newUserName](const web::http::http_response& response) {
                if (response.status_code() == 409) {
                    eventHub->publish(EventType::userNameAlreadyExists);
                } else if (response.status_code() <= 299) {
                    refreshAccessToken();
                } else {
                    eventHub->publish(EventType::unexpectedError, formula::gui::ErrorCodes::webResponseError);
                }
            })
            .then([this](const pplx::task<void>& previousTask) {
                try {
                    previousTask.wait();
                    eventHub->publish(EventType::webRequestFinished);
                    eventHub->publish(EventType::loginSuccess);
                } catch (std::exception& ex) {
                    eventHub->publish(EventType::webRequestFinished);
                    eventHub->publish(EventType::unexpectedError, formula::gui::ErrorCodes::networkError);
                }
            });;
}

pplx::task<void> formula::cloud::AuthenticatedClient::refreshAccessToken() {
    web::json::value body;
    body[WIDE("refreshToken")] = web::json::value::string(WIDE(refreshToken.value_or("")));

    web::json::value jsonResponse;
    return client.request(web::http::methods::POST, WIDE("/api/auth/refreshAccessToken"), body, destructorCts.get_token())
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
    accessToken = NARROW(jsonResponse[WIDE("accessToken")].as_string());
    refreshToken = NARROW(jsonResponse[WIDE("refreshToken")].as_string());
    expiresAt = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()).count()
                + jsonResponse[WIDE("expiresIn")].as_integer();

    settings->add(storage::SettingKey::accessToken, accessToken);
    settings->add(storage::SettingKey::refreshToken, refreshToken);
    settings->add(storage::SettingKey::expiresAt, expiresAt);

    checkUsername(jsonResponse);
}

void formula::cloud::AuthenticatedClient::processRefreshedTokenResponse(web::json::value jsonResponse) {
    accessToken = utility::conversions::to_utf8string(jsonResponse[WIDE("accessToken")].as_string());
    expiresAt = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()).count()
                + jsonResponse[WIDE("expiresIn")].as_integer();

    settings->add(storage::SettingKey::accessToken, accessToken);
    settings->add(storage::SettingKey::expiresAt, expiresAt);

    checkUsername(jsonResponse);
}

bool formula::cloud::AuthenticatedClient::isLoggedIn() {
    return accessToken && refreshToken && expiresAt;
}

std::optional<std::string> formula::cloud::AuthenticatedClient::getUserName() {
    return userName;
}

web::http::http_request
formula::cloud::AuthenticatedClient::forgeAuthenticatedRequest(web::http::method method, std::string uri) {
    web::http::http_request req(method);
    req.headers().add(WIDE("Authorization"), WIDE("Bearer " + accessToken.value_or("")));
    req.set_request_uri(WIDE(uri));

    return req;
}

void formula::cloud::AuthenticatedClient::checkUsername(web::json::value tokenResponse) {
    auto userNameField = tokenResponse[WIDE("username")];
    if (!userNameField.is_null()) {
        userName = NARROW(userNameField.as_string());
        settings->add(storage::SettingKey::username, userName);
    } else {
        eventHub->publish(EventType::needSetUsername);
    }
}

formula::cloud::AuthenticatedClient::~AuthenticatedClient() {
    destructorCts.cancel();
}
