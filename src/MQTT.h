#pragma once

#include <ArduinoJson.h>
#include <ArduinoLog.h>
#include <espMqttClientAsync.h>
#include <ESPAsyncWebServer.h>
#include <string>
#include <WiFi.h>
#include "repositories/SensorRepository.hpp"
#include "homeassistant/discovery.h"
#include "inputs/tachometer.h"
#include "DependencyInjectionContainer.hpp"

#define HOMEASSISTANT_STATUS_TOPIC "homeassistant/status"
#if DEVELOPMENT_MODE
    #define BASE_TOPIC "openair-dev"
#else
    #define BASE_TOPIC "openair"
#endif
#define AVAILABILITY_TOPIC BASE_TOPIC "/availability"


class MQTT {
    private:
        const int PUBLISH_INTERVAL = 1000;
        const int NO_WIFI = 5000;
        const int NO_CONFIG = 5000;
        const int RECONNECT = 10000;

        int getRPM() {
            return DI::GetContainer()->resolve<Tachometer>()->RPM();
        }

        std::set<std::shared_ptr<HaDiscoverable>> haDiscoverables {
            std::make_shared<IpHaSensor>("ip", "ip"),
            std::make_shared<FreeMemoryHaSensor>("free-memory","free memory"),
            std::make_shared<NumericHaSensor>("fan-rpm","fan RPM", [this](){
                char buff[10];
                itoa(this->getRPM(), buff, 10);
                return std::string(buff);
            }),
            std::make_shared<HaFan>(),
        };

        std::string hostname;
        int port = 1883;
        std::string user;
        std::string pass;
        espMqttClientAsync client;
        bool connecting = false;
        bool discoveryPublished = false;
        std::set<std::string> sensorUuids;

        bool connect() {
            if (!WiFi.isConnected()) {
                Log.warningln("Unable to connect to MQTT because WiFi is unavailable");
                return false;
            }

            if (this->hostname == "") {
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
            this->client.disconnect();
            return this->connect();
        }

        void sendHaSensorDiscovery(std::shared_ptr<HaDiscoverable> sensor) {
            Log.traceln("Sending %s", sensor->discoveryTopic().c_str());
            auto payload = sensor->toDiscovery();

            auto size = measureJson(payload) + 1;
            char buff[size];
            serializeJson(payload, buff, size);
            this->client.publish(sensor->discoveryTopic().c_str(), 0, false, buff);

            auto commandable = dynamic_cast<HaCommandable*>(sensor.get());
            if (!commandable)
                return;
            
            for (auto pair : commandable->getCommandTopics()) {
                this->client.subscribe(pair.first.c_str(), 0);
            }
        }

        void sendDiscovery() {
            Log.infoln("Sending discovery");
            this->discoveryPublished = true;
            for(auto ha : this->haDiscoverables) {
                sendHaSensorDiscovery(ha);
            }

            auto sensorRepo = DI::GetContainer()->resolve<SensorRepository>();
            for (auto uuid : sensorRepo->getUuids()) {
                this->sensorUuids.emplace(uuid);
                auto sensor = sensorRepo->getInstance(uuid);
                for (auto haSensor : sensor->getHaSensors()) {
                    sendHaSensorDiscovery(haSensor);
                }
            }
        }

        void onConnect(bool sessionPresent) {
            Log.infoln("MQTT connected");
            this->connecting = false;

            this->client.publish(AVAILABILITY_TOPIC, 0, true, HA_ONLINE);
            this->client.subscribe(HOMEASSISTANT_STATUS_TOPIC, 0);
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
                    delete[] strval;
                    return;
                }
            }

            for(auto discoverable : this->haDiscoverables) {
                auto commandable = dynamic_cast<HaCommandable*>(discoverable.get());
                if (!commandable){
                    continue;
                }

                for (auto p : commandable->getCommandTopics()) {
                    if (strcmp(topic, p.first.c_str()) == 0) {
                        p.second(strval);
                        delete[] strval;
                        return;
                    }
                }
            }

            delete[] strval;
        }

        void onPublish(uint16_t packetId) {

        }

        void publishDiagnostics() {
            for (auto hs : this->haDiscoverables) {
                auto sensor = dynamic_cast<HaSensor*>(hs.get());
                if (sensor) {
                    this->client.publish(sensor->stateTopic().c_str(), 0, false, sensor->toValue().c_str());
                    continue;
                }

                auto fan = dynamic_cast<HaFan*>(hs.get());
                if (fan) {
                    this->client.publish(fan->percentageStateTopic().c_str(), 0, false, fan->toPercentageState().c_str());
                    this->client.publish(fan->stateTopic().c_str(), 0, false, "ON");
                    this->client.publish(fan->modeStateTopic().c_str(), 0, false, fan->toModeState().c_str());
                    continue;
                }
            }
        }

        void publishSensors() {
            auto repo = DI::GetContainer()->resolve<SensorRepository>();
            auto uuids = repo->getUuids();

            for (auto uuid : uuids) {
                auto sensor = repo->getInstance(uuid);
                for (auto haSensor : sensor->getHaSensors()) {
                    this->client.publish(haSensor->stateTopic().c_str(), 0, false, haSensor->toValue().c_str());
                }
            }
        }

        void processSensorChanges() {
            if (!this->discoveryPublished)
                return;

            auto repo = DI::GetContainer()->resolve<SensorRepository>();
            auto uuids = repo->getUuids();
            
            if (this->sensorUuids.size() == uuids.size())
                return;

            Log.infoln("Sensor configuration changed");
            this->sendDiscovery();
        }

        void publish() {
            this->processSensorChanges();
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
