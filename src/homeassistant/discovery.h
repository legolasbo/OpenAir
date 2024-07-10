#pragma once

#include <ArduinoJson.h>
#include <WiFi.h>
#include "device.h"
#include "constants.h"
#include "../outputs/fan.h"

typedef std::function<std::string(void)> HaSensorCallback;
typedef std::function<void(std::string)> HaCommandCallback;

class HaDiscoverable {
    friend class HaSensor;
    friend class HaFan;
    friend class HaFanSpeed;

    private:
        std::string discoveryTopicName;
        std::shared_ptr<Device> device;

    public:
        HaDiscoverable() {
            this->device = DI::GetContainer()->resolve<Device>();
        }

        virtual std::string discoveryTopic() {
            return this->discoveryTopicName;
        }

        virtual JsonDocument toDiscovery() {
            JsonDocument doc;

            doc["~"] = device->getName();
            doc["device"] = device->toJson();
            doc["availability_topic"] = "~/availability";
            doc["payload_available"] = HA_ONLINE;
            doc["payload_not_available"] = HA_OFFLINE;

            return doc;
        };
};

class HaCommandable : public HaDiscoverable {
    private:
        std::map<std::string, HaCommandCallback> commands;
    public:
        virtual std::map<std::string, HaCommandCallback> getCommandTopics() = 0;
};

class HaFan : public HaCommandable {
    public:
        HaFan() : HaCommandable() {
            auto tmp = "homeassistant/fan/%s/hvac/config";
            auto device = DI::GetContainer()->resolve<Device>();
            char buff[strlen(tmp) + device->getName().size()];
            sprintf(buff, tmp, device->getName().c_str());
            this->discoveryTopicName = buff;
        }

        std::map<std::string, HaCommandCallback> getCommandTopics() override {
            return {
                {
                    device->getName() + "/hvac/speed/percentage-command",
                    [this](std::string payload) {
                        int speed = atoi(payload.c_str());
                        DI::GetContainer()->resolve<Fan>()->setManualFanSpeed(speed);
                        Log.traceln("Percentage command: %s", payload.c_str());
                    }
                },
                {
                    device->getName() + "/hvac/on-command",
                    [this](std::string payload) {
                        Log.traceln("Command: %s, resuming auto mode", payload.c_str());
                        DI::GetContainer()->resolve<Fan>()->resumeAuto();
                    }
                },
                {
                    device->getName() + "/hvac/mode/mode-command",
                    [this](std::string payload) {
                        Log.traceln("mode command: %s", payload.c_str());
                        auto fan = DI::GetContainer()->resolve<Fan>();
                        if (payload == "manual") {
                            fan->setManualFanSpeed(fan->currentSpeed());
                        }
                        else {
                            fan->resumeAuto();
                        }
                    }
                },
            };
        }

        JsonDocument  toDiscovery() override {
            JsonDocument doc = HaDiscoverable::toDiscovery();

            doc["unique_id"] = device->getName() + "-hvac";
            doc["object_id"] = device->getName() + "_hvac";
            doc["name"] = "hvac";
            doc["icon"] = "mdi:hvac";
            doc["command_topic"] = "~/hvac/on-command";
            doc["state_topic"] = "~/hvac/on-state";
            doc["percentage_command_topic"] = "~/hvac/speed/percentage-command";
            doc["percentage_state_topic"] = "~/hvac/speed/percentage-state";
            doc["preset_mode_command_topic"] = "~/hvac/mode/mode-command";
            doc["preset_mode_state_topic"] = "~/hvac/mode/mode-state";
            doc["preset_modes"][0] = "auto";
            doc["preset_modes"][1] = "manual";

            return doc;
        }

        std::string percentageStateTopic() {
            return device->getName() + "/hvac/speed/percentage-state";
        }

        std::string toPercentageState() {
            char buff[sizeof(int)*8+1];
            itoa(DI::GetContainer()->resolve<Fan>()->currentSpeed(), buff, 10);
            return buff;
        }

        std::string modeStateTopic() {
            return device->getName() + "/hvac/mode/mode-state";
        }

        std::string toModeState() {
            switch (DI::GetContainer()->resolve<Fan>()->getMode()) {
                case MANUAL: return "manual";
                default: return "auto";
            }
        }

        std::string stateTopic() {
            return device->getName() + "/hvac/on-state";
        }
};

class HaSensor : public HaDiscoverable {
    private:
        std::string sensorName;
        std::string sensorMachineName;

    public:
        virtual ~HaSensor() {};
        virtual const char * icon() = 0;
        virtual const char * category() = 0;
        virtual const char * deviceClass() = 0;
        virtual const char * unit() = 0;
        virtual const std::set<std::string> options() = 0;
        virtual const int suggestedDisplayPrecision() = 0;

        HaSensor(std::string machineName, std::string sensorName) : HaDiscoverable() {
            this->sensorName = sensorName;
            
            this->sensorMachineName = machineName;
            std::replace(this->sensorMachineName.begin(), this->sensorMachineName.end(), ' ', '-');

            int size = strlen(SENSOR_DISCOVERY_TEMPLATE) + sensorMachineName.size() + device->getName().size();
            char buff[size];
            sprintf(buff, SENSOR_DISCOVERY_TEMPLATE, device->getName().c_str(), sensorMachineName.c_str());
            this->discoveryTopicName = std::string(buff);
        }

        virtual std::string stateTopic() {
            return device->getName() + "/" + sensorMachineName;
        }

        JsonDocument toDiscovery() override {
            JsonDocument data = HaDiscoverable::toDiscovery();

            data["unique_id"] = device->getName() + "-" + sensorMachineName;
            data["object_id"] = device->getName() + "_" + sensorMachineName;
            data["name"] = sensorName;
            data["state_topic"] = this->stateTopic();

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

class HaFanSpeed : public HaDiscoverable {
    public:

        HaFanSpeed() : HaDiscoverable() {
            int size = strlen(SENSOR_DISCOVERY_TEMPLATE) + strlen("percentage-state") + device->getName().size();
            char buff[size];
            sprintf(buff, SENSOR_DISCOVERY_TEMPLATE, device->getName().c_str(), "percentage-state");
            this->discoveryTopicName = std::string(buff);
        }

        JsonDocument toDiscovery() override {
            JsonDocument doc = HaDiscoverable::toDiscovery();

            doc["unique_id"] = device->getName() + "-hvac-speed";
            doc["object_id"] = device->getName() + "_hvac_speed";
            doc["name"] = "hvac speed";
            doc["icon"] = "mdi:percent-box-outline";
            doc["state_topic"] = "~/hvac/speed/percentage-state";
            doc["entity_category"] = "diagnostic";
            doc["unit_of_measurement"] = "%";

            return doc;
        }
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
        bool diagnostic;
    public:
        GenericHaSensor(std::string machineName, std::string sensorName, HaSensorCallback valueCallback, bool diagnostic = false) : HaSensor(machineName, sensorName) {
            this->diagnostic = diagnostic;
            this->callback = valueCallback;
        }

        const char * icon() override {
            return "";
        }

        const char * category() override {
            if (this->diagnostic) {
                return "diagnostic";
            }
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

class NumericHaSensor : public GenericHaSensor {
    public:
        NumericHaSensor(std::string machineName, std::string sensorName, HaSensorCallback valueCallback, bool diagnostic = false) : GenericHaSensor(machineName, sensorName, valueCallback, diagnostic) {}

        const int suggestedDisplayPrecision() override {
            return 0;
        }
        JsonDocument toDiscovery() override {
            JsonDocument doc = GenericHaSensor::toDiscovery();

            doc["state_class"] = "measurement";

            return doc;
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

class CO2HaSensor : public HaSensor {
    private:
        HaSensorCallback callback;
    public:
        CO2HaSensor(std::string machineName, std::string sensorName, HaSensorCallback valueCallback) : HaSensor(machineName, sensorName) {
            this->callback = valueCallback;
        }

        const char * icon() override {
            return "mdi:molecule-co2";
        }

        const char * category() override {
            return "";
        }

        std::string toValue() override {
            return this->callback();
        }

        const char * unit() override {
            return "ppm";
        }
        
        const char * deviceClass() override {
            return "carbon_dioxide";
        }

        const std::set<std::string> options() override {
            return {};
        }

        const int suggestedDisplayPrecision() override {
            return 0;
        }
};
