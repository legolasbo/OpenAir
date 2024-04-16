#ifndef INTERFACE_CALCULATORS_H
#define INTERFACE_CALCULATORS_H

#include "api.h"

class CalculatorApi : public API {
    private:
    void processFormValues(CalculatorConfiguration * calc, AsyncWebServerRequest * request) {
        for (size_t i = 0; i < request->args(); i++) {
            Serial.printf("Processing option %d: %s -> %s\n", i, request->argName(i).c_str(), request->arg(request->argName(i)).c_str());
            auto argName = request->argName(i).c_str();
            if (calc->hasOption(argName)) {
                calc->oldSetOption(argName, request->arg(argName).c_str());
            }
            else {
                calc->setOption(argName, request->arg(argName).c_str());
            }
        }
    }

    public:

    void initialize(DI * container, AsyncWebServer * server, Configuration * config) {
        API::initialize(container, server, config);

        server->on("/api/calculators/types", HTTP_GET, [this](AsyncWebServerRequest * request) {
            this->respondJson(this->config->getCalculators()->availableCalculatorTypes(), request);
        });

        server->on("/api/calculators/options", HTTP_GET, [this](AsyncWebServerRequest * request) {
            this->options(request);
        });
        server->on("/api/calculators/details", HTTP_GET, [this](AsyncWebServerRequest * request) {
            this->details(request);
        });

        server->on("/api/calculators/add", HTTP_POST, [this](AsyncWebServerRequest * request) {
            this->add(request);
        });
        server->on("/api/calculators/edit", HTTP_POST, [this](AsyncWebServerRequest * request) {
            this->edit(request);
        });
        server->on("/api/calculators/delete", HTTP_POST, [this](AsyncWebServerRequest * request) {
            this->remove(request);
        });
        server->on("/api/calculators/get", HTTP_GET, [this](AsyncWebServerRequest * request) {
            this->get(request);
        });
        server->on("/api/calculators", HTTP_GET, [this](AsyncWebServerRequest * request) {
            this->list(request);
        });
    }

    void options(AsyncWebServerRequest * request);
    void add(AsyncWebServerRequest * request);
    void edit(AsyncWebServerRequest * request);
    void get(AsyncWebServerRequest * request);
    void details(AsyncWebServerRequest * request);
    void list(AsyncWebServerRequest * request);
    void remove(AsyncWebServerRequest * request);
};

void CalculatorApi::options(AsyncWebServerRequest * request) {
    auto configs = container->resolve<CalculatorConfigurations>();
    if (request->hasParam("uuid")) {
        std::string uuid = request->getParam("uuid")->value().c_str();
        CalculatorConfiguration * calculator = configs->get(uuid);
        return this->respondJson(calculator->getConfigurationOptions(), request);
    }

    if (!request->hasParam("type")) {
        return internalServerErrorResponse(request, "Unable to determine calculator type");
    }

    CalculatorType type = CalculatorTypeFromMachineName(request->getParam("type")->value().c_str());
    if (type == CalculatorType::UNKNOWN_CALCULATOR_TYPE) {
        return internalServerErrorResponse(request, "Unknown calculater type");
    }

    CalculatorConfiguration * c = configs->create(type);
    JsonDocument options = c->getConfigurationOptions();
    delete(c);

    return this->respondJson(options, request);
}

void CalculatorApi::list(AsyncWebServerRequest * request) {
    JsonDocument doc;
    CalculatorConfigurations * calculators = this->config->getCalculators();

    for (auto uuid : calculators->getUuids()) {
        CalculatorConfiguration* calculator = calculators->get(uuid);
        doc[uuid]["name"] = calculator->getName();
        doc[uuid]["type"] = ToMachineName(calculator->type());
        doc[uuid]["valid"] = calculator->isValid();
    }

    this->respondJson(doc, request);
}

void CalculatorApi::remove(AsyncWebServerRequest * request) {
    if (!request->hasArg("uuid")) {
        Serial.println("No uuid param present");
        return request->redirect("/");
    }

    this->config->getCalculators()->erase(request->arg("uuid").c_str());
    this->config->save();
    request->redirect("/calculators");
}

void CalculatorApi::get(AsyncWebServerRequest * request) {
    if (!request->hasArg("uuid")) {
        Serial.println("No uuid param present");
        return request->redirect("/");
    }

    this->respondJson(this->config->getCalculators()->get(request->arg("uuid").c_str())->toJson(), request);
}

void CalculatorApi::details(AsyncWebServerRequest * request) {
    if (!request->hasArg("uuid")) {
        Serial.println("No uuid param present");
        return request->redirect("/");
    }

    this->respondJson(this->config->getCalculators()->get(request->arg("uuid").c_str())->toDetails(), request);
}

void CalculatorApi::add(AsyncWebServerRequest * request) {
    if (!request->hasArg("type")) {
        return internalServerErrorResponse(request, "Missing 'type' parameter");
    }

    auto type = CalculatorTypeFromMachineName(request->arg("type").c_str());
    if (type == UNKNOWN_CALCULATOR_TYPE) {
        return internalServerErrorResponse(request, "Unknown type");
    }

    CalculatorConfiguration * calc = this->config->getCalculators()->create(type);
    this->processFormValues(calc, request);

    this->config->getCalculators()->add(calc);
    request->redirect("/calculators");
    this->config->save();
}

void CalculatorApi::edit(AsyncWebServerRequest * request) {
    if (!request->hasArg("uuid")) {
        return internalServerErrorResponse(request, "Missing 'uuid' parameter");
    }
    std::string uuid = request->arg("uuid").c_str();
    CalculatorConfiguration * calc = this->config->getCalculators()->get(uuid);
    
    if (calc == nullptr) {
        return internalServerErrorResponse(request, "Unknown calculator");
    }
    this->processFormValues(calc, request);

    serializeJsonPretty(calc->toJson(), Serial);

    request->redirect("/calculators");
    this->config->save();
}

#endif