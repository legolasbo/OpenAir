#ifndef SENSOR_CONFIGURATION_H
#define SENSOR_CONFIGURATION_H

#include "../sensors/SensorConnectors.h"
#include "../sensors/SensorTypes.h"
#include "../sensors/ConnectionTypes.h"
#include <ArduinoJson.h>
#include <sstream>

class SensorConfiguration {
    private:
        SensorConnector    connector;
        SensorType          sensorType;
        ConnectionType      connectionType;


    public:
    SensorConfiguration() {}
    SensorConfiguration(SensorConnector connector, ConnectionType connectionType, SensorType sensorType) {
        this->connector = connector;
        this->connectionType = connectionType;
        this->sensorType = sensorType;
    }

    SensorConnector getSensorConnector() {
        return this->connector;
    }

    SensorType getSensorType() {
        return this->sensorType;
    }

    ConnectionType getConnectionType() {
        return this->connectionType;
    }

    bool isValid() {
        return this->connector != UNKNOWN_CONNECTOR
            && this->connectionType != UNKNOWN_CONNECTION_TYPE
            && this->sensorType != UNKNOWN_SENSOR_TYPE;
    }

    bool equals(SensorConfiguration *other) {
        if (this->sensorType != other->sensorType) {
            return false;
        }

        if (this->connector != other->connector) {
            return false;
        }

        if (this->connectionType != other->connectionType) {
            return false;
        }

        return true;
    }

    bool equals(SensorConfiguration &other) {
        return this->equals(&other);
    }

    const char * getMachineName() {
        std::ostringstream out;

        out << ToMachineName(this->connector);
        out << "_";
        out << ToMachineName(this->connectionType);
        out << "_";
        out << ToMachineName(this->sensorType);

        return out.str().c_str();
    }

    JsonDocument toJson() {
        JsonDocument doc;
        doc["connector"] = ToMachineName(this->connector);
        doc["connectionType"] = ToMachineName(this->connectionType);
        doc["sensorType"] = ToMachineName(this->sensorType);
        return doc;
    }

    static SensorConfiguration fromJson(JsonObject doc) {
        const char * connectorName = doc["connector"].as<const char *>();
        const char * connectionTypeName = doc["connectionType"].as<const char *>();
        const char * sensorTypeName = doc["sensorType"].as<const char *>();
        if (connectorName == nullptr || connectionTypeName == nullptr || sensorTypeName == nullptr) {  
            return SensorConfiguration();
        }

        SensorConnector connector = SensorConnectorFromMachineName(connectorName);
        ConnectionType connectionType = ConnectionTypeFromMachineName(connectionTypeName);
        SensorType sensorType = SensorTypeFromMachineName(sensorTypeName);
        return SensorConfiguration(connector, connectionType, sensorType);
    }
};


#endif
