//
// Created by antoi on 23/03/2022.
//

#ifndef FORMULA_CLOUD_FORMULACLOUDCLIENT_INCLUDED
#define FORMULA_CLOUD_FORMULACLOUDCLIENT_INCLUDED

#include <chrono>
#include <thread>

#include <cpprest/http_client.h>

#include "storage/LocalSettings.hpp"
#include "events/EventHub.hpp"
#include "cloud/AuthenticatedClient.hpp"

namespace formula::cloud {

    using RequestFunction = std::function<pplx::task<web::http::http_response>()>;
    using SuccessCallback = std::function<void(web::json::value)>;

    class FormulaCloudClient : public AuthenticatedClient {
    public:
        explicit FormulaCloudClient(const std::shared_ptr<formula::storage::LocalSettings>& settings,
                                    const std::shared_ptr<formula::events::EventHub>& eventHub);
        ~FormulaCloudClient() override = default;

        void listFormulas(int skip, int take, std::string sort, std::string order, bool self, std::string searchQuery);
        void getFormula(std::string formulaId);
        void createFormula(std::string name, std::string description, std::string source, std::string author);
        void updateFormula(std::string formulaId, std::string name, std::string description, std::string source, std::string author);
        void submitRating(std::string formulaId, double rating, std::string comment);

    private:
        void requestWrapper(RequestFunction requestFunction, SuccessCallback successCallback, int numTries = 0);

        inline static const utility::string_t baseUrl = W("https://formula-staging.soundspear.com");
    };
}

#endif //FORMULA_CLOUD_FORMULACLOUDCLIENT_INCLUDED
