#pragma once

#include "../enums/SensorConnectors.h"
#include "../enums/SensorTypes.h"
#include "../enums/ConnectionTypes.h"
#include "GenericConfiguration.h"
#include <ArduinoJson.h>
#include <sstream>

class SensorConfiguration : public GenericConfiguration {
    private:
        SensorType          sensorType;

    public:
    SensorConfiguration(DI &container) : GenericConfiguration(container) {}
    SensorConfiguration(DI &container, SensorType sensorType) : GenericConfiguration(container) {
        this->sensorType = sensorType;
    }
    SensorConfiguration(DI &container, SensorType sensorType, const char * uuid) : GenericConfiguration(container, uuid) {
        this->sensorType = sensorType;
    }

    virtual std::unordered_map<std::string, Option> availableOptions() {
        auto options = GenericConfiguration::availableOptions();
        options.emplace("connector", Option(X4));
        options.emplace("connection", Option(I2C));
        return options;
    };

    SensorConnector getSensorConnector() {
        return this->getOption("connector").toConnector();
    }

    SensorType getSensorType() {
        return this->sensorType;
    }

    ConnectionType getConnectionType() {
        return this->getOption("connection").toConnection();
    }

    bool isValid() {
        return this->getSensorConnector() != UNKNOWN_CONNECTOR
            && this->getConnectionType() != UNKNOWN_CONNECTION_TYPE
            && this->sensorType != UNKNOWN_SENSOR_TYPE;
    }

    bool equals(SensorConfiguration *other) {
        if (this->sensorType != other->sensorType) {
            return false;
        }

        if (this->getSensorConnector() != other->getSensorConnector()) {
            return false;
        }

        if (this->getConnectionType() != other->getConnectionType()) {
            return false;
        }

        return true;
    }

    bool equals(SensorConfiguration &other) {
        return this->equals(&other);
    }

    virtual JsonDocument toJson() {
        JsonDocument doc = GenericConfiguration::toJson();
        doc["type"] = ToMachineName(this->sensorType);
        return doc;
    }

    static JsonDocument getConfigurationOptions() {
        JsonDocument doc;

        doc["name"]["type"] = "text";
        doc["name"]["label"] = "Name";

        doc["uuid"]["type"] = "hidden";

        doc["type"]["type"] = "hidden";

        doc["connector"]["type"] = "select";
        doc["connector"]["label"] = "Connector";
        doc["connector"]["options"][ToMachineName(X4)] = ToString(X4);
        doc["connector"]["options"][ToMachineName(X6)] = ToString(X6);
        
        doc["connection"]["type"] = "select";
        doc["connection"]["label"] = "Connection";
        doc["connection"]["options"][ToMachineName(I2C)] = ToString(I2C);
        doc["connection"]["options"][ToMachineName(UART)] = ToString(UART);

        return doc;
    }

    static SensorConfiguration * fromJson(DI &container, JsonObject doc) {
        const char * uuid = doc["uuid"].as<const char *>();
        const char * sensorTypeName = doc["type"].as<const char *>();
        if (sensorTypeName == nullptr || uuid == nullptr) {  
            Serial.println("Skipping sensor");
            return nullptr;
        }

        SensorType sensorType = SensorTypeFromMachineName(sensorTypeName);
        SensorConfiguration * config = new SensorConfiguration(container, sensorType, uuid);

        config->configureFromJson(doc);

        return config;
    }
};

