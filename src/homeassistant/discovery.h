#pragma once

#include <ArduinoJson.h>
#include <WiFi.h>
#include "device.h"
#include "constants.h"

typedef std::function<std::string(void)> HaSensorCallback;

class HaSensor {
    private:
        std::string discoveryTopicName;
        std::string sensorName;
        std::string sensorMachineName;
        std::shared_ptr<Device> device;

    public:
        virtual ~HaSensor() {};
        virtual const char * icon() = 0;
        virtual const char * category() = 0;
        virtual const char * deviceClass() = 0;
        virtual const char * unit() = 0;
        virtual const std::set<std::string> options() = 0;
        virtual const int suggestedDisplayPrecision() = 0;

        HaSensor(std::string machineName, std::string sensorName) {
            this->device = DI::GetContainer()->resolve<Device>();
            this->sensorName = sensorName;
            
            this->sensorMachineName = machineName;
            std::replace(this->sensorMachineName.begin(), this->sensorMachineName.end(), ' ', '-');

            int size = strlen(SENSOR_DISCOVERY_TEMPLATE) + sensorMachineName.size() + device->getName().size();
            char buff[size];
            sprintf(buff, SENSOR_DISCOVERY_TEMPLATE, device->getName().c_str(), sensorMachineName.c_str());
            this->discoveryTopicName = std::string(buff);
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
            data["state_topic"] = this->stateTopic();
            data["device"] = device->toJson();
            data["availability_topic"] = "~/availability";
            data["payload_available"] = HA_ONLINE;
            data["payload_not_available"] = HA_OFFLINE;

            if (this->suggestedDisplayPrecision() >= 0)
                data["suggested_display_precision"] = this->suggestedDisplayPrecision();
            
            if(strlen(this->icon()))
                data["icon"] = this->icon();

            if (strlen(this->category()))
                data["entity_category"] = this->category();

            if(strlen(this->deviceClass())) {
                data["state_class"] = "measurement";
                data["device_class"] = this->deviceClass();
                if (this->deviceClass() == "enum")
                    data["state_class"] = "None";

                if (this->options().size()) {
                    auto options = data.add<JsonArray>();
                    data["options"] = options;
                    for (auto option : this->options()) {
                        options.add(option);
                    }
                }
            }

            if (strlen(this->unit()))
                data["unit_of_measurement"] = this->unit();

            return data;
        }

        virtual std::string toValue() = 0;
};

class IpHaSensor : public HaSensor {
    public:
        IpHaSensor(std::string machineName, std::string sensorName) : HaSensor(machineName, sensorName) {}

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

        const std::set<std::string> options() override {
            return {};
        }

        const int suggestedDisplayPrecision() override {
            return -1;
        }

        std::string toValue() override {
            return WiFi.localIP().toString().c_str();
        }
};

class FreeMemoryHaSensor : public HaSensor {
    public:
        FreeMemoryHaSensor(std::string machineName, std::string sensorName) : HaSensor(machineName, sensorName) {}


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

        const std::set<std::string> options() override {
            return {};
        }

        const int suggestedDisplayPrecision() override {
            return -1;
        }
};

class GenericHaSensor : public HaSensor {
    private:
        HaSensorCallback callback;
    public:
        GenericHaSensor(std::string machineName, std::string sensorName, HaSensorCallback valueCallback) : HaSensor(machineName, sensorName) {
            this->callback = valueCallback;
        }

        const char * icon() override {
            return "";
        }

        const char * category() override {
            return "";
        }

        std::string toValue() override {
            return this->callback();
        }

        const char * unit() override {
            return "";
        }
        
        const char * deviceClass() override {
            return "";
        }

        const std::set<std::string> options() override {
            return {};
        }

        const int suggestedDisplayPrecision() override {
            return -1;
        }
};

class HumidityHaSensor : public HaSensor {
    private:
        HaSensorCallback callback;
    public:
        HumidityHaSensor(std::string machineName, std::string sensorName, HaSensorCallback valueCallback) : HaSensor(machineName, sensorName) {
            this->callback = valueCallback;
        }

        const char * icon() override {
            return "mdi:water-percent";
        }

        const char * category() override {
            return "";
        }

        std::string toValue() override {
            return this->callback();
        }

        const char * unit() override {
            return "%";
        }
        
        const char * deviceClass() override {
            return "humidity";
        }

        const std::set<std::string> options() override {
            return {};
        }

        const int suggestedDisplayPrecision() override {
            return 2;
        }
};

class TemperatureHaSensor : public HaSensor {
    private:
        HaSensorCallback callback;
    public:
        TemperatureHaSensor(std::string machineName, std::string sensorName, HaSensorCallback valueCallback) : HaSensor(machineName, sensorName) {
            this->callback = valueCallback;
        }

        const char * icon() override {
            return "mdi:thermometer";
        }

        const char * category() override {
            return "";
        }

        std::string toValue() override {
            return this->callback();
        }

        const char * unit() override {
            return "°C";
        }
        
        const char * deviceClass() override {
            return "temperature";
        }

        const std::set<std::string> options() override {
            return {};
        }

        const int suggestedDisplayPrecision() override {
            return 2;
        }
};
