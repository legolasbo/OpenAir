#ifndef SENSOR_CONFIGURATION_H
#define SENSOR_CONFIGURATION_H

#include "../sensors/SensorConnectors.h"
#include "../sensors/SensorTypes.h"
#include "../sensors/ConnectionTypes.h"
#include "GenericConfiguration.h"
#include <ArduinoJson.h>
#include <sstream>

class SensorConfiguration : public GenericConfiguration {
    private:
        SensorConnector     connector;
        SensorType          sensorType;
        ConnectionType      connectionType;
        std::string         name;


    public:
    SensorConfiguration() {}
    SensorConfiguration(SensorConnector connector, ConnectionType connectionType, SensorType sensorType) : GenericConfiguration() {
        this->connector = connector;
        this->connectionType = connectionType;
        this->sensorType = sensorType;
    }
    SensorConfiguration(SensorConnector connector, ConnectionType connectionType, SensorType sensorType, const char * uuid) : GenericConfiguration(uuid) {
        this->connector = connector;
        this->connectionType = connectionType;
        this->sensorType = sensorType;
    }
    SensorConfiguration(SensorConnector connector, ConnectionType connectionType, SensorType sensorType, std::string name) : SensorConfiguration(connector, connectionType, sensorType) {
        this->name = name;
    }
    SensorConfiguration(SensorConnector connector, ConnectionType connectionType, SensorType sensorType, const char * uuid, const char * name) : SensorConfiguration(connector, connectionType, sensorType, uuid) {
        this->name = name;
    }

    SensorConnector getSensorConnector() {
        return this->connector;
    }

    SensorType getSensorType() {
        return this->sensorType;
    }

    std::string getName() {
        return this->name;
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

    const char * machineName() {
        std::ostringstream out;

        out << ToMachineName(this->connector);
        out << "_";
        out << ToMachineName(this->connectionType);
        out << "_";
        out << ToMachineName(this->sensorType);

        return out.str().c_str();
    }

    virtual JsonDocument toJson() {
        JsonDocument doc = GenericConfiguration::toJson();
        doc["name"] = this->name;
        doc["connector"] = ToMachineName(this->connector);
        doc["connection"] = ToMachineName(this->connectionType);
        doc["sensor"] = ToMachineName(this->sensorType);
        return doc;
    }

    virtual bool hasOption(std::string name) {
        if (name == "connector") {
            return true;
        }

        if (name == "connection") {
            return true;
        }

        if (name == "name") {
            return true;
        }

        return false;
    }

    virtual bool setOption(std::string name, std::string value) {
        if (name == "connector") {
            return this->setConnector(value);
        }

        if (name == "connection") {
            return this->setConnection(value);
        }

        if (name == "name") {
            this->name = value;
            return true;
        }

        return false;
    }

    virtual bool setConnection(ConnectionType connection) {
        if (connection == UNKNOWN_CONNECTION_TYPE) {
            return false;
        }

        this->connectionType = connection;
        return true;
    }

    virtual bool setConnection(std::string connection) {
        return this->setConnection(ConnectionTypeFromMachineName(connection.c_str()));
    }

    virtual bool setConnector(SensorConnector connector) {
        if (connector == UNKNOWN_CONNECTOR) {
            return false;
        }
        this->connector = connector;
        return true;
    }

    virtual bool setConnector(std::string value) {
        return this->setConnector(SensorConnectorFromMachineName(value.c_str()));
    }

    virtual JsonDocument getConfigurationOptions() {
        JsonDocument doc;

        doc["name"]["type"] = "text";
        doc["name"]["label"] = "Name";

        doc["uuid"]["type"] = "hidden";

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

    static SensorConfiguration * fromJson(JsonObject doc) {
        const char * uuid = doc["uuid"].as<const char *>();
        const char * name = doc["name"].as<const char *>();
        const char * connectorName = doc["connector"].as<const char *>();
        const char * connectionTypeName = doc["connection"].as<const char *>();
        const char * sensorTypeName = doc["sensor"].as<const char *>();
        if (connectorName == nullptr || connectionTypeName == nullptr || sensorTypeName == nullptr || uuid == nullptr) {  
            Serial.println("Skipping sensor");
            return new SensorConfiguration();
        }

        SensorConnector connector = SensorConnectorFromMachineName(connectorName);
        ConnectionType connectionType = ConnectionTypeFromMachineName(connectionTypeName);
        SensorType sensorType = SensorTypeFromMachineName(sensorTypeName);
        return new SensorConfiguration(connector, connectionType, sensorType, uuid, name);
    }
};


#endif
