#pragma once

#include <ArduinoJson.h>
#include <WiFi.h>
#include "device.h"
#include "constants.h"

class HaSensor {
    private:
        std::string discoveryTopicName;
        std::string sensorName;
        std::string sensorMachineName;

        Device * device;
    public:
        virtual ~HaSensor() {};
        virtual const char * icon() = 0;
        virtual const char * category() = 0;
        virtual const char * deviceClass() = 0;
        virtual const char * unit() = 0;

        HaSensor(Device * device, std::string sensorName) {
            this->device = device;
            this->sensorName = sensorName;

            int size = strlen(SENSOR_DISCOVERY_TEMPLATE) + sensorName.size() + device->getName().size();
            char buff[size];
            sprintf(buff, SENSOR_DISCOVERY_TEMPLATE, device->getName().c_str(), sensorName.c_str());
            this->discoveryTopicName = std::string(buff);

            this->sensorMachineName = sensorName;
            std::replace(this->sensorMachineName.begin(), this->sensorMachineName.end(), ' ', '-');
        }

        virtual std::string discoveryTopic() {
            return this->discoveryTopicName;
        }

        virtual std::string stateTopic() {
            return device->getName() + "/" + sensorMachineName;
        }

        virtual JsonDocument toDiscovery() {
            JsonDocument data;

            data["~"] = device->getName();
            data["unique_id"] = device->getName() + "-" + sensorMachineName;
            data["object_id"] = device->getName() + "_" + sensorMachineName;
            data["name"] = sensorName;
            data["icon"] = this->icon();
            data["state_topic"] = this->stateTopic();
            data["state_class"] = "measurement";
            data["entity_category"] = this->category();
            data["device"] = device->toJson();
            data["availability_topic"] = "~/availability";
            data["payload_available"] = HA_ONLINE;
            data["payload_not_available"] = HA_OFFLINE;

            if(strlen(this->deviceClass()))
                data["device_class"] = this->deviceClass();

            if (strlen(this->unit()))
                data["unit"] = this->unit();

            return data;
        }

        virtual std::string toValue() = 0;
};

class IpHaSensor : public HaSensor {
    public:
        IpHaSensor(Device & device, std::string sensorName) : HaSensor(&device, sensorName) {}

        const char * icon() override {
            return "mdi:network-outline";
        }

        const char * category() override {
            return "diagnostic";
        }

        const char * unit() override {
            return "";
        }
        
        const char * deviceClass() override {
            return "";
        }

        std::string toValue() override {
            return WiFi.localIP().toString().c_str();
        }
};

class FreeMemorySensor : public HaSensor {
    public:
        FreeMemorySensor(Device & device, std::string sensorName) : HaSensor(&device, sensorName) {}


        const char * icon() override {
            return "mdi:memory";
        }

        const char * category() override {
            return "diagnostic";
        }

        std::string toValue() override {
            return std::to_string(esp_get_free_heap_size());
        }

        const char * unit() override {
            return "B";
        }
        
        const char * deviceClass() override {
            return "";
        }
};
