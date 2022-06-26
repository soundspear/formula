/*
    Copyright (c) 2022 Soundspear.
    Distributed under the Boost Software License, Version 1.0.
    See http://www.boost.org/LICENSE_1_0.txt
*/

#include <cpprest/json.h>
#include <boost/format.hpp>
#include <iostream>

#include "cloud/FormulaCloudClient.hpp"
#include "cloud/ListFormulaDto.hpp"
#include "processor/FormulaMetadata.hpp"
#include "GetFormulaDto.hpp"
#include "storage/LocalIndex.hpp"

using formula::processor::FormulaMetadataKeys;

formula::cloud::FormulaCloudClient::FormulaCloudClient(const std::shared_ptr<formula::storage::LocalSettings>& settings,
                                                       const std::shared_ptr<formula::events::EventHub>& eventHub)
: AuthenticatedClient(settings, eventHub, baseUrl){

}

void formula::cloud::FormulaCloudClient::listFormulas(int skip, int take, std::string sort, std::string order, bool self, std::string searchQuery) {
    auto uri = (boost::format("/api/Formula/list?skip=%1%&take=%2%&sort=%3%&order=%4%&self=%5%")
                                 % skip % take % sort % order % (self ? "true" : "false") ).str();

    if (!searchQuery.empty()) {
        uri = (boost::format("%1%&searchQuery=%2%") % uri % searchQuery).str();
    }

    const auto requestFunction = [this, uri]() {
        const auto req = forgeAuthenticatedRequest(web::http::methods::GET, uri);
        return client.request(req ,destructorCts.get_token());
    };

    const auto successCallback = [this](web::json::value response) {
        std::vector<formula::cloud::ListFormulaDto> formulaList;
        for (auto formula : response.as_array()) {
            ListFormulaDto dto;
            dto.id = NARROW(formula[WIDE("id")].as_string());
            if (!formula[WIDE("author")].is_null()) {
                dto.author = NARROW(formula[WIDE("author")].as_string());
            }
            dto.name = NARROW(formula[WIDE("name")].as_string());
            dto.description = NARROW(formula[WIDE("description")].as_string());
            if (!formula[WIDE("rating")].is_null()) {
                dto.rating = formula[WIDE("rating")].as_double();
            }
            if (!formula[WIDE("created")].is_null()) {
                dto.created = boost::posix_time::from_iso_extended_string(NARROW(formula[WIDE("created")].as_string()));
            }
            if (!formula[WIDE("lastModified")].is_null()) {
                dto.lastModified = boost::posix_time::from_iso_extended_string(NARROW(formula[WIDE("lastModified")].as_string()));
            }
            formulaList.push_back(dto);
        }

        eventHub->publish(EventType::listFormulaResponse, formulaList);
    };

    requestWrapper(requestFunction, successCallback);
}

void formula::cloud::FormulaCloudClient::getFormula(std::string formulaId) {
    if (!isLoggedIn()) {
        eventHub->publish(EventType::needLogin);
        return;
    }

    const auto uri = (boost::format("/api/Formula/%1%")
                      % formulaId ).str();

    const auto requestFunction = [this, uri]() {
        const auto req = forgeAuthenticatedRequest(web::http::methods::GET, uri);
        return client.request(req ,destructorCts.get_token());
    };

    const auto successCallback = [this](web::json::value response) {
        auto resp = response.serialize();
        formula::cloud::GetFormulaDto dto;
        dto.author = NARROW(response[WIDE("author")].as_string());
        dto.name = NARROW(response[WIDE("name")].as_string());
        dto.description = NARROW(response[WIDE("description")].as_string());
        dto.numDownloads = response[WIDE("numDownloads")].as_integer();
        if (!response[WIDE("rating")].is_null()) {
            dto.rating = response[WIDE("rating")].as_double();
        }
        dto.numRatings = response[WIDE("numRatings")].as_integer();
        auto metadataStr = NARROW(response[WIDE("metadata")].as_string());
        auto metadata = formula::storage::LocalIndex::deserializeMetadata(metadataStr);
        metadata[formula::processor::FormulaMetadataKeys::name] = dto.name;
        dto.metadata = metadata;

        const auto userRatingsList = response[WIDE("userRatings")].as_array();
        for (auto userRating : userRatingsList) {
            GetFormulaDto::GetUserRatingDto ratingDto;
            ratingDto.rating = userRating[WIDE("rating")].as_double();
            ratingDto.comment = NARROW(userRating[WIDE("comment")].as_string());

            dto.ratings.push_back(ratingDto);
        }

        eventHub->publish(EventType::getFormulaResponse, dto);
    };

    requestWrapper(requestFunction, successCallback);
}

void formula::cloud::FormulaCloudClient::createFormula(formula::processor::FormulaMetadata metadata) {
    if (!isLoggedIn()) {
        eventHub->publish(EventType::needLogin);
        return;
    }
    if (!(getUserName().has_value())) {
        eventHub->publish(EventType::needSetUsername);
        return;
    }
    const auto uri = std::string("/api/Formula");

    web::json::value body;

    using formula::processor::FormulaMetadataKeys;
    body[WIDE("name")] = web::json::value::string(WIDE(metadata[FormulaMetadataKeys::name]));
    body[WIDE("description")] = web::json::value::string(WIDE(metadata[FormulaMetadataKeys::description]));
    auto serializedMetadata = formula::storage::LocalIndex::serializeMetadata(metadata);
    body[WIDE("metadata")] = web::json::value::string(WIDE(serializedMetadata));

    const auto requestFunction = [this, uri, body, metadata]() {
        auto req = forgeAuthenticatedRequest(web::http::methods::POST, uri);
        req.set_body(body);
        return client.request(req, destructorCts.get_token()).then([this, metadata](
            const web::http::http_response& response) {
                if (response.status_code() == 409) {
                    auto body = response.extract_json().get();
                    auto conflictingFormulaId = NARROW(body[WIDE("conflictingFormulaId")].as_string());
                    this->eventHub->publish(EventType::formulaAlreadyExists, std::pair(conflictingFormulaId, metadata));
                    response.set_status_code(web::http::status_codes::NonAuthInfo);
                }
                return response;
        });
    };

    const auto successCallback = [this](web::json::value response) {
        eventHub->publish(EventType::createFormulaSuccess);
    };

    requestWrapper(requestFunction, successCallback);
}

void formula::cloud::FormulaCloudClient::updateFormula(std::string formulaId, formula::processor::FormulaMetadata metadata) {
    if (!isLoggedIn()) {
        eventHub->publish(EventType::needLogin);
        return;
    }
    if (!(getUserName().has_value())) {
        eventHub->publish(EventType::needSetUsername);
        return;
    }

    const auto uri = (boost::format("/api/Formula/%1%")
                      % formulaId ).str();

    web::json::value body;
    using formula::processor::FormulaMetadataKeys;
    body[WIDE("description")] = web::json::value::string(WIDE(metadata[FormulaMetadataKeys::description]));
    auto serializedMetadata = formula::storage::LocalIndex::serializeMetadata(metadata);
    body[WIDE("metadata")] = web::json::value::string(WIDE(serializedMetadata));

    const auto requestFunction = [this, uri, body]() {
        auto req = forgeAuthenticatedRequest(web::http::methods::PATCH, uri);
        req.set_body(body);
        return client.request(req ,destructorCts.get_token());
    };

    const auto successCallback = [this](web::json::value response) {
        eventHub->publish(EventType::createFormulaSuccess);
    };

    requestWrapper(requestFunction, successCallback);
}

void formula::cloud::FormulaCloudClient::submitRating(std::string formulaId, double rating, std::string comment) {
    if (!isLoggedIn()) {
        eventHub->publish(EventType::needLogin);
        return;
    }
    if (!(getUserName().has_value())) {
        eventHub->publish(EventType::needSetUsername);
        return;
    }
    const auto uri = (boost::format("/api/Formula/%1%/rating")
                      % formulaId ).str();

    web::json::value body ;
    body[WIDE("rating")] = web::json::value::number(rating);
    body[WIDE("comment")] = web::json::value::string(WIDE(comment));

    const auto requestFunction = [this, uri, body]() {
        auto req = forgeAuthenticatedRequest(web::http::methods::POST, uri);
        req.set_body(body);
        return client.request(req ,destructorCts.get_token());
    };

    const auto successCallback = [this](web::json::value response) {
        eventHub->publish(EventType::ratingSubmitted);
    };

    requestWrapper(requestFunction, successCallback);
}

void formula::cloud::FormulaCloudClient::requestWrapper(RequestFunction requestFunction, SuccessCallback successCallback, int numTries) {
    eventHub->publish(EventType::webRequestSent);

    requestFunction()
    .then(
        [this, successCallback, requestFunction, numTries](const web::http::http_response& response) {
            if (response.status_code() == web::http::status_codes::NonAuthInfo) {
                // We use HTTP 203 (Non authoritative information) to mark that the response
                // has already been processor through a middleware
                eventHub->publish(EventType::webRequestFinished);
                return;
            }

            if (response.status_code() < 299) {
                try {
                    auto jsonResponse = response.extract_json().get();
                    successCallback(jsonResponse);
                }
                catch (const std::exception&) {
                    eventHub->publish(EventType::unexpectedError, formula::gui::ErrorCodes::cannotParseJson);
                }
                eventHub->publish(EventType::webRequestFinished);
            }
            else {
                if (numTries > 0) {
                    if (response.status_code() == 403) {
                        eventHub->publish(EventType::subscriptionExpired);
                    }
                    else if (response.status_code() == 401) {
                        eventHub->publish(EventType::needLogin);
                    }
                    else {
                        eventHub->publish(EventType::unexpectedError, formula::gui::ErrorCodes::webResponseError);
                    }
                    eventHub->publish(EventType::webRequestFinished);
                }
                else {
                    refreshAccessToken().wait();
                    requestWrapper(requestFunction, successCallback, numTries + 1);
                }
            }
        }
    )
    .then([this, successCallback, requestFunction, numTries](const pplx::task<void>& previousTask) {
        try {
            previousTask.wait();
            eventHub->publish(EventType::webRequestFinished);
        } catch (std::exception& ex) {
            if (numTries > 0) {
                eventHub->publish(EventType::webRequestFinished);
                eventHub->publish(EventType::unexpectedError, formula::gui::ErrorCodes::networkError);
            }
            else {
                std::this_thread::sleep_for(std::chrono::seconds(10));
                requestWrapper(requestFunction, successCallback, numTries + 1);
            }
        }
    });
}


