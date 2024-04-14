#ifndef INTERFACE_SENSORS_H
#define INTERFACE_SENSORS_H

#include "api.h"
#include "../factories/SensorFactory.h"
#include "../configuration/Configuration.h"
#include "../configuration/SensorConfiguration.h"

class SensorApi : public API {
    public:

    void initialize(DI * container, AsyncWebServer * server, Configuration * config) {
        API::initialize(container, server, config);
        
        server->on("/api/sensors/configurable", HTTP_GET, [this](AsyncWebServerRequest * request) {
            this->respondJson(SensorFactory::knownSensorTypesJson(), request);
        });

        server->on("/api/sensors/add", HTTP_POST, [this](AsyncWebServerRequest * request){
            this->addSensor(request);
        });
        server->on("/api/sensors/delete", HTTP_POST, [this](AsyncWebServerRequest * request){
            this->deleteSensor(request);
        });

        server->on("/api/sensors/get", HTTP_GET, [this](AsyncWebServerRequest * request){
            this->sensorJson(request);
        });
        server->on("/api/sensors/options", HTTP_GET, [this](AsyncWebServerRequest * request){
           this->respondJson(SensorConfiguration::getConfigurationOptions(), request);
        });

        server->on("/api/sensors/edit", HTTP_POST, [this](AsyncWebServerRequest * request){
            this->editSensor(request);
        });

        server->on("/api/sensors", HTTP_GET, [this](AsyncWebServerRequest * request){
            this->listSensors(request);
        });
    }

    void listSensors(AsyncWebServerRequest * request);
    void addSensor(AsyncWebServerRequest * request);
    void deleteSensor(AsyncWebServerRequest * request);
    void sensorJson(AsyncWebServerRequest * request);
    void editSensor(AsyncWebServerRequest * request);
};

void SensorApi::listSensors(AsyncWebServerRequest * request) {
    JsonDocument doc;
    SensorConfigurations * sensors = this->config->getSensors();

    for (auto uuid : sensors->getUuids()) {
        SensorConfiguration* sensor = sensors->get(uuid);
        doc[uuid]["name"] = sensor->getName();
        doc[uuid]["type"] = ToString(sensor->getSensorType());
        doc[uuid]["connector"] = ToString(sensor->getSensorConnector());
        doc[uuid]["connection"] = ToString(sensor->getConnectionType());
        doc[uuid]["uuid"] = uuid;
    }

    this->respondJson(doc, request);
}

void SensorApi::addSensor(AsyncWebServerRequest * request) {
    Serial.println("Add sensor handler");
    if (!request->hasArg("connection")) {
        return internalServerErrorResponse(request, "missing connection parameter");
    };
    if (!request->hasArg("type")) {
        return internalServerErrorResponse(request, "missing type parameter");
    }
    if (!request->hasArg("connector")) {
        return internalServerErrorResponse(request, "missing connector parameter");
    }

    SensorType sensType = SensorType(SensorTypeFromMachineName(request->arg("type").c_str()));
    ConnectionType connType = ConnectionType(ConnectionTypeFromMachineName(request->arg("connection").c_str()));
    SensorConnector connector = SensorConnector(SensorConnectorFromMachineName(request->arg("connector").c_str()));

    SensorConfiguration * config = new SensorConfiguration(this->container, connector, connType, sensType);

    if (this->config->getSensors()->identicalConfigExists(config)) {
        return internalServerErrorResponse(request, "Unable to add this configuration. it is already present");
    }

    if (request->hasArg("name")) {
        config->oldSetOption("name", request->arg("name").c_str());
    }

    this->config->getSensors()->add(config);
    request->redirect("/sensors");
    this->config->save();
}

void SensorApi::sensorJson(AsyncWebServerRequest * request) {
    std::string uuid = this->extractValidUuid(request);
    if (uuid == "") {
        return;
    }

    this->respondJson(this->config->getSensors()->get(uuid)->toJson(), request);
}

void SensorApi::deleteSensor(AsyncWebServerRequest * request) {
    if (!request->hasArg("uuid")) {
        Serial.println("No uuid param present");
        return request->redirect("/");
    }

    auto uuid = request->arg("uuid").c_str();

    for (auto calcId : this->config->getCalculators()->getUuids()) {
        if (this->config->getCalculators()->get(calcId)->dependsOn(uuid)) {
            this->config->getCalculators()->erase(calcId);
        }
    }

    this->config->getSensors()->erase(uuid);
    this->config->save();
    request->redirect("/sensors");
}

void SensorApi::editSensor(AsyncWebServerRequest * request) {
    std::string uuid = this->extractValidUuid(request);
    if (uuid == "") {
        return;
    }

    String errors;
    SensorConfiguration * sensor = this->config->getSensors()->get(uuid);
    for (size_t i = 0; i < request->args(); i++) {
        String argName = request->argName(i);
        if (argName == "uuid") {
            continue;
        }

        if (!sensor->hasOption(argName.c_str())) {
            errors += "Unknown option: ";
            errors += argName.c_str();
            errors += "<br>";
            continue;
        }

        String value = request->arg(argName);
        if (!sensor->oldSetOption(argName.c_str(), value.c_str())) {
            errors += "Unknown value: ";
            errors += value.c_str();
            errors += " for option ";
            errors += value.c_str();
            errors += "<br>";
            continue;
        }
    }

    this->config->save();
    request->redirect("/sensors");
}

#endif