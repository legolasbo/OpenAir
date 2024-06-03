#pragma once

#include <ArduinoJson.h>
#include <WiFi.h>

class Device {
    private:
        std::string name = "openair";

    public:
        Device() {}
        Device(std::string name) {
            this->name = name;
        }

        void setName(std::string name) {
            this->name = name;
        }

        std::string getName() {
            return this->name;
        }

        JsonDocument toJson() {
            JsonDocument doc;
            
            doc["name"] = this->name;
            doc["model"] = "Mini";
            doc["manufacturer"] = "Flamingo Tech";
            doc["sw_version"] = "0.0.1";
            doc["identifiers"][0] = this->name;
            doc["configuration_url"] = std::string("http://") + WiFi.localIP().toString().c_str();

            return doc;
        }
};