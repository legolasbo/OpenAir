#ifndef INTERFACE_CALCULATORS_H
#define INTERFACE_CALCULATORS_H

#include "api.h"
#include "../speedCalculators/CalculatorFactory.h"

class CalculatorApi : public API {
    public:

    void initialize(AsyncWebServer * server, Configuration * config) {
        API::initialize(server, config);

        server->on("/api/calculators/configurable", HTTP_GET, [this](AsyncWebServerRequest * request) {
            this->respondJson(CalculatorFactory::knownCalculatorTypesJson(), request);
        });

        server->on("/api/calculators/add", HTTP_POST, [this](AsyncWebServerRequest * request) {
            this->add(request);
        });
        server->on("/api/calculators/delete", HTTP_POST, [this](AsyncWebServerRequest * request) {
            this->remove(request);
        });
        server->on("/api/calculators", HTTP_GET, [this](AsyncWebServerRequest * request) {
            this->list(request);
        });
    }

    void add(AsyncWebServerRequest * request);
    void list(AsyncWebServerRequest * request);
    void remove(AsyncWebServerRequest * request);
};

void CalculatorApi::list(AsyncWebServerRequest * request) {
    JsonDocument doc;
    CalculatorConfigurations * calculators = this->config->getCalculators();

    for (auto uuid : calculators->getUuids()) {
        CalculatorConfiguration* calculator = calculators->get(uuid);
        doc[uuid]["name"] = calculator->getName();
        doc[uuid]["type"] = ToString(calculator->type());
        doc[uuid]["valid"] = calculator->isValid();

        doc[uuid]["uuid"] = uuid;
    }

    this->respondJson(doc, request);
}

void CalculatorApi::remove(AsyncWebServerRequest * request) {
    if (!request->hasArg("uuid")) {
        Serial.println("No uuid param present");
        return request->redirect("/");
    }

    this->config->getCalculators()->erase(request->arg("uuid").c_str());
    request->redirect("/");
}

void CalculatorApi::add(AsyncWebServerRequest * request) {
    if (!request->hasArg("type")) {
        return internalServerErrorResponse(request, "Missing 'type' parameter");
    }

    auto type = CalculatorTypeFromMachineName(request->arg("type").c_str());
    if (type == UNKNOWN_CALCULATOR_TYPE) {
        return internalServerErrorResponse(request, "Unknown type");
    }

    this->config->getCalculators()->addNew(type);
    request->redirect("/");
}

#endif