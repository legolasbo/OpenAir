#pragma once 

#include "api.h"

class CalculatorApi : public API {
    public:

    void configureCallbacks (AsyncWebServer & server) override {
        server.on("/api/calculators/types", HTTP_GET, [this](AsyncWebServerRequest * request) {
            this->respondJson(DI::GetContainer()->resolve<CalculatorRepository>()->availableCalculatorTypes(), request);
        });

        server.on("/api/calculators/add", HTTP_POST, [this](AsyncWebServerRequest * request) {
            this->create(request);
        });
        server.on("/api/calculators/get", HTTP_GET, [this](AsyncWebServerRequest * request) {
            this->read(request);
        });
        server.on("/api/calculators/edit", HTTP_POST, [this](AsyncWebServerRequest * request) {
            this->update(request);
        });
        server.on("/api/calculators/delete", HTTP_POST, [this](AsyncWebServerRequest * request) {
            this->remove(request);
        });

        server.on("/api/calculators/options", HTTP_GET, [this](AsyncWebServerRequest * request) {
            this->options(request);
        });
        server.on("/api/calculators/details", HTTP_GET, [this](AsyncWebServerRequest * request) {
            this->details(request);
        });

        server.on("/api/calculators", HTTP_GET, [this](AsyncWebServerRequest * request) {
            this->list(request);
        });
    }

    void options(AsyncWebServerRequest * request);
    void details(AsyncWebServerRequest * request);
    void list(AsyncWebServerRequest * request);
    void create(AsyncWebServerRequest * request);
    void read(AsyncWebServerRequest * request);
    void update(AsyncWebServerRequest * request);
    void remove(AsyncWebServerRequest * request);
};

void CalculatorApi::options(AsyncWebServerRequest * request) {
    std::string uuid = this->extractUuid(request);
    auto repository = DI::GetContainer()->resolve<CalculatorRepository>();
    auto calculator = repository->getInstance(uuid);

    if (calculator != nullptr){ 
        return this->respondJson(calculator->toInterfaceOptions(), request);
    }

    if (uuid != "") {
        return internalServerErrorResponse(request, "Unknown uuid was given");
    }

    if (!request->hasParam("type")) {
        return internalServerErrorResponse(request, "Unable to determine calculator type");
    }

    CalculatorType type = CalculatorTypeFromMachineName(request->getParam("type")->value().c_str());
    if (type == CalculatorType::UNKNOWN_CALCULATOR_TYPE) {
        return internalServerErrorResponse(request, "Unknown calculater type");
    }

    calculator = repository->create(type);
    this->respondJson(calculator->toInterfaceOptions(), request);
}

void CalculatorApi::details(AsyncWebServerRequest * request) {
    std::string uuid = this->extractUuid(request);
    if (uuid == "") {
        return internalServerErrorResponse(request, "Unable to determine the uuid");
    }

    auto calc = DI::GetContainer()->resolve<CalculatorRepository>()->getInstance(uuid);
    if (calc == nullptr) {
        return internalServerErrorResponse(request, "Unknown uuid");
    }

    this->respondJson(calc->toDetails(), request);
}

void CalculatorApi::create(AsyncWebServerRequest * request) {
    if (!request->hasArg("type")) {
        return internalServerErrorResponse(request, "Missing 'type' parameter");
    }

    auto repo = DI::GetContainer()->resolve<CalculatorRepository>();
    auto calculator = repo->create(request->arg("type").c_str());
    this->processFormValues(calculator, request);
    repo->addInstance(calculator);

    DI::GetContainer()->resolve<Configuration>()->save();
    request->redirect("/calculators");
}

void CalculatorApi::list(AsyncWebServerRequest * request) {
    JsonDocument doc;
    auto calculators = DI::GetContainer()->resolve<CalculatorRepository>();

    for (auto uuid : calculators->getUuids()) {
        auto calculator = calculators->getInstance(uuid);
        doc[uuid]["name"] = calculator->getOption("name")->toStr();
        doc[uuid]["type"] = ToMachineName(calculator->type());
        doc[uuid]["valid"] = calculator->isValid();
    }

    this->respondJson(doc, request);
}

void CalculatorApi::read(AsyncWebServerRequest * request) {
    std::string uuid = this->extractUuid(request);
    auto calculator = DI::GetContainer()->resolve<CalculatorRepository>()->getInstance(uuid);
    if (calculator == nullptr) {
        return internalServerErrorResponse(request, "Unable to get json without a valid uuid");
    }

    this->respondJson(calculator->toJson(), request);
}

void CalculatorApi::remove(AsyncWebServerRequest * request) {
    std::string uuid = this->extractUuid(request);
    if (uuid == "") {
        return internalServerErrorResponse(request, "Unable to remove without a valid uuid");
    }

    DI::GetContainer()->resolve<CalculatorRepository>()->remove(uuid);
    DI::GetContainer()->resolve<Configuration>()->save();
    request->redirect("/calculators");
}

void CalculatorApi::update(AsyncWebServerRequest * request) {
    std::string uuid = this->extractUuid(request);
    if (uuid == "") {
        return internalServerErrorResponse(request, "Unable to edit a calculator without a valid uuid");
    }

    std::shared_ptr<SpeedCalculator> calculator = DI::GetContainer()->resolve<CalculatorRepository>()->getInstance(uuid);

    this->processFormValues(calculator, request);
    
    DI::GetContainer()->resolve<Configuration>()->save();
    request->redirect("/calculators");
}
