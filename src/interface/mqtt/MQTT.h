#pragma once

#include <ArduinoJson.h>
#include <espMqttClientAsync.h>
#include <ESPAsyncWebServer.h>
#include <string>
#include <WiFi.h>
#include "../../repositories/SensorRepository.hpp"
#include "../homeassistant/device.h"
#include "../homeassistant/discovery.h"

#define HOMEASSISTANT_STATUS_TOPIC "homeassistant/status"
#define BASE_TOPIC "openair"
#define AVAILABILITY_TOPIC BASE_TOPIC "/availability"


class MQTT {
    private:
        const int PUBLISH_INTERVAL = 1000;
        const int NO_WIFI = 5000;
        const int NO_CONFIG = 5000;
        const int RECONNECT = 10000;

        Device device = Device("openair");
        std::set<std::shared_ptr<HaSensor>> haSensors {
            std::make_shared<IpHaSensor>(device, "ip"),
            std::make_shared<FreeMemorySensor>(device, "free memory"),
        };

        std::string hostname;
        int port = 1883;
        std::string user;
        std::string pass;
        espMqttClientAsync client;
        bool connecting = false;

        bool connect() {
            if (!WiFi.isConnected()) {
                Log.warningln("Unable to connect to MQTT because WiFi is unavailable");
                return false;
            }
            
            if (this->client.connected()) {
                return true;
            }

            if (this->connecting) {
                Log.traceln("MQTT connecting...");
                return true;
            }

            Log.infoln("Connecting to MQTT server at %s:%d with username `%s`", this->hostname.c_str(), this->port, this->user.c_str());
            client.setClientId("OpenAir");
            client.setServer(this->hostname.c_str(), this->port);
            client.setCredentials(this->user.c_str(), this->pass.c_str());
            client.setWill(AVAILABILITY_TOPIC, 0, true, HA_OFFLINE);

            this->connecting = true;
            return this->client.connect();
        }

        bool reConnect() {
            return this->client.disconnect() ? this->connect() : false;
        }

        void sendDiscovery() {
            Log.infoln("Sending discovery");
            for(auto ha : this->haSensors) {
                auto payload = ha->toDiscovery();

                auto size = measureJson(payload) + 1;
                char buff[size];
                serializeJson(payload, buff, size);
                this->client.publish(ha->discoveryTopic().c_str(), 0, false, buff);
            }
        }

        void onConnect(bool sessionPresent) {
            Log.infoln("MQTT connected");
            this->connecting = false;

            this->client.publish(AVAILABILITY_TOPIC, 0, true, HA_ONLINE);
            this->client.subscribe(HOMEASSISTANT_STATUS_TOPIC, 0);
            this->sendDiscovery();
        }

        void onDisconnect(espMqttClientTypes::DisconnectReason reason) {
            this->connecting = false;
            switch (reason) {
                case espMqttClientTypes::DisconnectReason::TCP_DISCONNECTED: 
                    Log.warningln("MQTT disconnected: TCP disconnect");
                    break;
                case espMqttClientTypes::DisconnectReason::MQTT_UNACCEPTABLE_PROTOCOL_VERSION: 
                    Log.warningln("MQTT disconnected: unacceptable protocol version");
                    break;
                case espMqttClientTypes::DisconnectReason::MQTT_IDENTIFIER_REJECTED: 
                    Log.warningln("MQTT disconnected: identifier rejected");
                    break;
                case espMqttClientTypes::DisconnectReason::MQTT_SERVER_UNAVAILABLE: 
                    Log.warningln("MQTT disconnected: server unavailable");
                    break;
                case espMqttClientTypes::DisconnectReason::MQTT_MALFORMED_CREDENTIALS: 
                    Log.warningln("MQTT disconnected: malformed credentials");
                    break;
                case espMqttClientTypes::DisconnectReason::MQTT_NOT_AUTHORIZED: 
                    Log.warningln("MQTT disconnected: not authorized");
                    break;
                case espMqttClientTypes::DisconnectReason::TLS_BAD_FINGERPRINT: 
                    Log.warningln("MQTT disconnected: bad TLS fingerprint");
                    break;
            }
        }

        void onMessage(const espMqttClientTypes::MessageProperties& properties, const char * topic, const uint8_t* payload, size_t len, size_t index, size_t total) {
            char* strval = new char[len + 1];
            memcpy(strval, payload, len);
            strval[len] = '\0';
            Log.traceln("MQTT message received on topic %s with payload %s", topic, strval);

            if (strcmp(topic, HOMEASSISTANT_STATUS_TOPIC) == 0) {
                if (strcmp(HA_ONLINE, strval) == 0) {
                    this->sendDiscovery();
                }
            }

            delete[] strval;
        }

        void onPublish(uint16_t packetId) {

        }

        void publishDiagnostics() {
            for (auto hs : this->haSensors) {
                this->client.publish(hs->stateTopic().c_str(), 0, false, hs->toValue().c_str());
            }
        }

        void publishSensors() {
            auto repo = DI::GetContainer()->resolve<SensorRepository>();
            auto uuids = repo->getUuids();

            for (auto uuid : uuids) {

            }
        }

        void publish() {
            this->publishDiagnostics();
            this->publishSensors();
        }

    public:
        MQTT() {

            client.onConnect([this](bool sessionPresent){
                this->onConnect(sessionPresent);
            });

            client.onDisconnect([this](espMqttClientTypes::DisconnectReason reason) {
                this->onDisconnect(reason);
            });

            client.onMessage([this](const espMqttClientTypes::MessageProperties& properties, const char * topic, const uint8_t* payload, size_t len, size_t index, size_t total) {
                this->onMessage(properties, topic, payload, len, index, total);
            });

            client.onPublish([this](uint16_t packetId) {
                this->onPublish(packetId);
            });
        }

        void setHostname(const std::string &hostname) {
            this->hostname = hostname;
            this->reConnect();
        }

        void setPort (int port) {
            this->port = port;
            this->reConnect();
        }

        void setUser(const std::string &user) {
            this->user = user;
            this->reConnect();
        }

        void setPass(const std::string &pass) {
            this->pass = pass;
            this->reConnect();
        }

        TickType_t runTask() {
            if (!this->connect())
                return RECONNECT;

            this->publish();
            return PUBLISH_INTERVAL;
        }

        void loadJson(JsonObject json) {
            if(json.containsKey("hostname"))
                this->setHostname(json["hostname"].as<std::string>());
            if(json.containsKey("port"))
                this->setPort(json["port"].as<int>());
            if(json.containsKey("user"))
                this->setUser(json["user"].as<std::string>());
            if(json.containsKey("pass"))
                this->setPass(json["pass"].as<std::string>());
        }

        JsonDocument toJson() {
            JsonDocument doc;

            doc["hostname"] = this->hostname;
            doc["port"] = this->port;
            doc["user"] = this->user;
            doc["pass"] = this->pass;

            return doc;
        }

};
