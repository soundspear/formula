/*
    Copyright (c) 2022 Soundspear.
    Distributed under the Boost Software License, Version 1.0.
    See http://www.boost.org/LICENSE_1_0.txt
*/

#ifndef FORMULA_CLOUD_FORMULACLOUDCLIENT_INCLUDED
#define FORMULA_CLOUD_FORMULACLOUDCLIENT_INCLUDED

#include <chrono>
#include <thread>
#include <variant>

#include <cpprest/http_client.h>

#include "storage/LocalSettings.hpp"
#include "events/EventHub.hpp"
#include "cloud/AuthenticatedClient.hpp"
#include "processor/FormulaMetadata.hpp"

namespace formula::cloud {
    using RequestFunction = std::function<pplx::task<web::http::http_response>()>;
    using SuccessCallback = std::function<void(web::json::value)>;

    /**
     * Web client to make requests to Formula Cloud
     */
    class FormulaCloudClient : public AuthenticatedClient {
    public:
        explicit FormulaCloudClient(const std::shared_ptr<formula::storage::LocalSettings>& settings,
                                    const std::shared_ptr<formula::events::EventHub>& eventHub);
        ~FormulaCloudClient() override = default;

        void listFormulas(int skip, int take, std::string sort, std::string order, bool self, std::string searchQuery);
        void getFormula(std::string formulaId);
        void createFormula(formula::processor::FormulaMetadata metadata);
        void updateFormula(std::string formulaId, formula::processor::FormulaMetadata metadata);
        void submitRating(std::string formulaId, double rating, std::string comment);

    private:
        void requestWrapper(RequestFunction requestFunction, SuccessCallback successCallback, int numTries = 0);

        inline static const utility::string_t baseUrl = WIDE(FORMULA_CLOUD_URL);
    };
}

#endif //FORMULA_CLOUD_FORMULACLOUDCLIENT_INCLUDED
