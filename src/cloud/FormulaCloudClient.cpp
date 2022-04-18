//
// Created by antoi on 23/03/2022.
//

#include <cpprest/json.h>
#include <boost/format.hpp>
#include <iostream>

#include "cloud/FormulaCloudClient.hpp"
#include "cloud/ListFormulaDto.hpp"
#include "processor/FormulaMetadata.hpp"
#include "GetFormulaDto.hpp"

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
            dto.id = S(formula[W("id")].as_string());
            dto.author = S(formula[W("author")].as_string());
            dto.name = S(formula[W("name")].as_string());
            dto.description = S(formula[W("description")].as_string());
            if (!formula[W("rating")].is_null()) {
                dto.rating = formula[W("rating")].as_double();
            }
            dto.created = boost::posix_time::from_iso_extended_string(S(formula[W("created")].as_string()));
            dto.lastModified = boost::posix_time::from_iso_extended_string(S(formula[W("lastModified")].as_string()));
            formulaList.push_back(dto);
        }

        eventHub->publish(EventType::listFormulaResponse, formulaList);
    };

    requestWrapper(requestFunction, successCallback);
}

void formula::cloud::FormulaCloudClient::getFormula(std::string formulaId) {
    const auto uri = (boost::format("/api/Formula/%1%")
                      % formulaId ).str();

    const auto requestFunction = [this, uri]() {
        const auto req = forgeAuthenticatedRequest(web::http::methods::GET, uri);
        return client.request(req ,destructorCts.get_token());
    };

    const auto successCallback = [this](web::json::value response) {
        formula::cloud::GetFormulaDto dto;
        dto.author = S(response[W("author")].as_string());
        dto.name = S(response[W("name")].as_string());
        dto.description = S(response[W("description")].as_string());
        dto.numDownloads = response[W("numDownloads")].as_integer();
        if (!response[W("rating")].is_null()) {
            dto.rating = response[W("rating")].as_double();
        }
        dto.numRatings = response[W("numRatings")].as_integer();
        dto.source = S(response[W("source")].as_string());

        const auto userRatingsList = response[W("userRatings")].as_array();
        for (auto userRating : userRatingsList) {
            GetFormulaDto::GetUserRatingDto ratingDto;
            ratingDto.rating = userRating[W("rating")].as_double();
            ratingDto.comment = S(userRating[W("comment")].as_string());

            dto.ratings.push_back(ratingDto);
        }

        eventHub->publish(EventType::getFormulaResponse, dto);
    };

    requestWrapper(requestFunction, successCallback);
}

void formula::cloud::FormulaCloudClient::createFormula(std::string name, std::string description, std::string source, std::string author) {
    const auto uri = std::string("/api/Formula");

    web::json::value body ;
    body[W("name")] = web::json::value::string(W(name));
    body[W("description")] = web::json::value::string(W(description));
    body[W("source")] = web::json::value::string(W(source));
    body[W("author")] = web::json::value::string(W(author));

    const auto requestFunction = [this, uri, body]() {
        auto req = forgeAuthenticatedRequest(web::http::methods::POST, uri);
        req.set_body(body);
        return client.request(req ,destructorCts.get_token());
    };

    const auto successCallback = [this](web::json::value response) {
        eventHub->publish(EventType::createFormulaSuccess);
    };

    requestWrapper(requestFunction, successCallback);
}

void formula::cloud::FormulaCloudClient::updateFormula(std::string formulaId, std::string name, std::string description,
                                                       std::string source, std::string author) {
    const auto uri = (boost::format("/api/Formula/%1%")
                      % formulaId ).str();

    web::json::value body ;
    body[W("name")] = web::json::value::string(W(name));
    body[W("description")] = web::json::value::string(W(description));
    body[W("source")] = web::json::value::string(W(source));
    body[W("author")] = web::json::value::string(W(author));

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
    const auto uri = (boost::format("/api/Formula/%1%/rating")
                      % formulaId ).str();

    web::json::value body ;
    body[W("rating")] = web::json::value::number(rating);
    body[W("comment")] = web::json::value::string(W(comment));

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
            if (response.status_code() < 299) {
                try {
                    auto jsonResponse = response.extract_json().get();
                    successCallback(jsonResponse);
                }
                catch (const std::exception&) {
                    eventHub->publish(EventType::unknownWebError);
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
                        eventHub->publish(EventType::unknownWebError);
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
                eventHub->publish(EventType::unknownWebError);
            }
            else {
                std::this_thread::sleep_for(std::chrono::seconds(10));
                requestWrapper(requestFunction, successCallback, numTries + 1);
            }
        }
    });
}


