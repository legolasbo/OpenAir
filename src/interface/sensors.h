#ifndef INTERFACE_SENSORS_H
#define INTERFACE_SENSORS_H

#include <ESPAsyncWebServer.h>
#include "../sensors/SensorFactory.h"
#include "../configuration/Configuration.h"
#include "../configuration/SensorConfiguration.h"
#include "responses.h"

class SensorApi {
    private:
    Configuration * config;
    AsyncWebServer * server;
    
    std::string extractValidUuid(AsyncWebServerRequest * request) {
        if (!request->hasParam("uuid") && !request->hasArg("uuid")) {
            internalServerErrorResponse(request, "uuid is required to configure a sensor");
            return "";
        }

        std::string uuid;
        if (request->hasParam("uuid")) {
            uuid = request->getParam("uuid")->value().c_str();
        }
        else {
            uuid = request->arg("uuid").c_str();
        }

        if (!this->config->getSensors()->exists(uuid)) {
            internalServerErrorResponse(request, "Unknown uuid");
            return "";
        }

        return uuid;
    }

    void respondJson(JsonDocument doc, AsyncWebServerRequest * request) {
        String json;
        serializeJsonPretty(doc, json);
        request->send(200, String("text/json"), json);
    }

    public:
    SensorApi() {}

    void initialize(AsyncWebServer * server, Configuration * config) {
        this->server = server;
        this->config = config;

        server->on("/api/sensors/configurable", HTTP_GET, [](AsyncWebServerRequest * request) {
            String json;
            serializeJsonPretty(SensorFactory::knownSensorTypesJson(), json);
            request->send(200, String("text/json"), json);
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
            this->sensorOptions(request);
        });

        server->on("/api/sensors/configure", HTTP_POST, [this](AsyncWebServerRequest * request){
            this->configureSensor(request);
        });

        server->on("/api/sensors", HTTP_GET, [this](AsyncWebServerRequest * request){
            this->listSensors(request);
        });
    }

    Configuration * getConfig() {
        return this->config;
    }

    void listSensors(AsyncWebServerRequest * request);
    void addSensor(AsyncWebServerRequest * request);
    void deleteSensor(AsyncWebServerRequest * request);
    void sensorJson(AsyncWebServerRequest * request);
    void sensorOptions(AsyncWebServerRequest * request);
    void configureSensor(AsyncWebServerRequest * request);
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
    if (!request->hasArg("connectionType")) {
        return internalServerErrorResponse(request, "missing connectionType parameter");
    };
    if (!request->hasArg("sensorType")) {
        return internalServerErrorResponse(request, "missing sensorType parameter");
    }
    if (!request->hasArg("connection")) {
        return internalServerErrorResponse(request, "missing connection parameter");
    }

    SensorType sensType = SensorType(SensorTypeFromMachineName(request->arg("sensorType").c_str()));
    ConnectionType connType = ConnectionType(ConnectionTypeFromMachineName(request->arg("connectionType").c_str()));
    SensorConnector connector = SensorConnector(SensorConnectorFromMachineName(request->arg("connection").c_str()));

    SensorConfiguration * config = new SensorConfiguration(connector, connType, sensType);

    if (this->config->getSensors()->identicalConfigExists(config)) {
        return internalServerErrorResponse(request, "Unable to add this configuration. it is already present");
    }

    this->config->getSensors()->add(config);
    request->redirect("/");
    this->config->save();
}

void SensorApi::sensorJson(AsyncWebServerRequest * request) {
    std::string uuid = this->extractValidUuid(request);
    if (uuid == "") {
        return;
    }

    this->respondJson(this->config->getSensors()->get(uuid)->toJson(), request);
}

void SensorApi::sensorOptions(AsyncWebServerRequest * request) {
    std::string uuid = this->extractValidUuid(request);
    if (uuid == "") {
        return;
    }
    
    this->respondJson(this->config->getSensors()->get(uuid)->getConfigurationOptions(), request);
}

void SensorApi::deleteSensor(AsyncWebServerRequest * request) {
    Serial.println("Delete sensor handler");
    
    if (!request->hasArg("uuid")) {
        Serial.println("No uuid param present");
        return request->redirect("/");
    }

    this->config->getSensors()->erase(request->arg("uuid").c_str());
    request->redirect("/");
}

void SensorApi::configureSensor(AsyncWebServerRequest * request) {
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
        if (!sensor->setOption(argName.c_str(), value.c_str())) {
            errors += "Unknown value: ";
            errors += value.c_str();
            errors += " for option ";
            errors += value.c_str();
            errors += "<br>";
            continue;
        }
    }

    this->config->save();
    request->redirect("/");
}

#endif