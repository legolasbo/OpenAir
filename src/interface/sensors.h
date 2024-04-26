#pragma once

#include "api.h"
#include "../repositories/SensorRepository.hpp"
#include "../configuration/Configuration.h"
#include "../configuration/SensorConfiguration.h"

class SensorApi : public API {
    public:

    SensorApi(AsyncWebServer & server) : API(server) {

        server.on("/api/sensors/types", HTTP_GET, [this](AsyncWebServerRequest * request) {
            this->respondJson(SensorRepository::knownSensorTypesJson(), request);
        });

        server.on("/api/sensors/add", HTTP_POST, [this](AsyncWebServerRequest * request){
            this->addSensor(request);
        });
        server.on("/api/sensors/get", HTTP_GET, [this](AsyncWebServerRequest * request){
            this->sensorJson(request);
        });
        server.on("/api/sensors/edit", HTTP_POST, [this](AsyncWebServerRequest * request){
            this->editSensor(request);
        });
        server.on("/api/sensors/delete", HTTP_POST, [this](AsyncWebServerRequest * request){
            this->deleteSensor(request);
        });

        server.on("/api/sensors/options", HTTP_GET, [this](AsyncWebServerRequest * request){
           this->options(request);
        });
        server.on("/api/sensors/details", HTTP_GET, [this](AsyncWebServerRequest * request) {
            this->details(request);
        });

        server.on("/api/sensors", HTTP_GET, [this](AsyncWebServerRequest * request){
            this->listSensors(request);
        });
    }

    void options(AsyncWebServerRequest * request);
    void details(AsyncWebServerRequest * request);
    void listSensors(AsyncWebServerRequest * request);
    void addSensor(AsyncWebServerRequest * request);
    void deleteSensor(AsyncWebServerRequest * request);
    void sensorJson(AsyncWebServerRequest * request);
    void editSensor(AsyncWebServerRequest * request);
};

void SensorApi::options(AsyncWebServerRequest * request) {
    auto configs = DI::GetContainer()->resolve<SensorConfigurations>();
    std::string uuid = this->extractUuid(request);
    SensorConfiguration * sensor = configs->get(uuid);

    if (uuid != "" && sensor == nullptr) {
        return internalServerErrorResponse(request, "Unknown uuid was given");
    }

    if (sensor != nullptr) {
        return this->respondJson(sensor->getConfigurationOptions(), request);
    }

    if (!request->hasParam("type")) {
        return internalServerErrorResponse(request, "Unable to determine sensor type");
    }

    SensorType type = SensorTypeFromMachineName(request->getParam("type")->value().c_str());
    if (type == SensorType::UNKNOWN_SENSOR_TYPE) {
        return internalServerErrorResponse(request, "Unknown sensor type");
    }

    sensor = configs->create(type);
    JsonDocument options = sensor->getConfigurationOptions();
    delete(sensor);

    this->respondJson(options, request);
}

void SensorApi::details(AsyncWebServerRequest * request) {
    std::string uuid = this->extractUuid(request);
    if (uuid == "") {
        return internalServerErrorResponse(request, "Unable to determine the uuid");
    }

    SensorConfiguration * sensor = DI::GetContainer()->resolve<SensorConfigurations>()->get(uuid);
    if (sensor == nullptr) {
        return internalServerErrorResponse(request, "Unknown uuid");
    }

    this->respondJson(sensor->toDetails(), request);
}

void SensorApi::listSensors(AsyncWebServerRequest * request) {
    Log.infoln("Listing sensors");
    JsonDocument doc;
    auto sensors = DI::GetContainer()->resolve<SensorConfigurations>();

    for (auto uuid : sensors->getUuids()) {
        SensorConfiguration* sensor = sensors->get(uuid);
        doc[uuid] = sensor->toJson();
    }

    this->respondJson(doc, request);
}

void SensorApi::addSensor(AsyncWebServerRequest * request) {
    if (!request->hasArg("type")) {
        return internalServerErrorResponse(request, "missing type parameter");
    }
    
    SensorType sensType = SensorType(SensorTypeFromMachineName(request->arg("type").c_str()));
    ConnectionType connType = ConnectionType(ConnectionTypeFromMachineName(request->arg("connection").c_str()));
    SensorConnector connector = SensorConnector(SensorConnectorFromMachineName(request->arg("connector").c_str()));

    request->redirect("/sensors");
}

void SensorApi::sensorJson(AsyncWebServerRequest * request) {
    std::string uuid = this->extractUuid(request);
    if (uuid == "") {
        return internalServerErrorResponse(request, "Unable to get json without a valid uuid");
    }

    SensorConfiguration * sensor = DI::GetContainer()->resolve<SensorConfigurations>()->get(uuid);
    if (sensor == nullptr) {
        return internalServerErrorResponse(request, "Unable to get json without a valid uuid");
    }

    this->respondJson(sensor->toJson(), request);
}

void SensorApi::deleteSensor(AsyncWebServerRequest * request) {
    if (!request->hasArg("uuid")) {
        Serial.println("No uuid param present");
        return request->redirect("/");
    }

    auto uuid = request->arg("uuid").c_str();

    DI::GetContainer()->resolve<SensorRepository>()->remove(uuid);
    DI::GetContainer()->resolve<Configuration>()->save();
    request->redirect("/sensors");
}

void SensorApi::editSensor(AsyncWebServerRequest * request) {
    std::string uuid = this->extractUuid(request);
    if (uuid == "") {
        return internalServerErrorResponse(request, "Unable to edit a sensor without a uuid");
    }

    String errors;
    SensorConfiguration * sensor = DI::GetContainer()->resolve<SensorConfigurations>()->get(uuid);

    this->processFormValues(sensor, request);

    DI::GetContainer()->resolve<Configuration>()->save();
    request->redirect("/sensors");
}

