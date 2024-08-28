#pragma once

#include <ArduinoJson.h>
#include <ArduinoLog.h>
#include <espMqttClientAsync.h>
#include <ESPAsyncWebServer.h>
#include <string>
#include <WiFi.h>
#include "repositories/SensorRepository.hpp"
#include "repositories/RepositorySubscriber.hpp"
#include "homeassistant/discovery.h"
#include "inputs/tachometer.h"
#include "DependencyInjectionContainer.hpp"
#include <queue>
#include <set>

#define HOMEASSISTANT_STATUS_TOPIC "homeassistant/status"
#if DEVELOPMENT_MODE
    #define BASE_TOPIC "openair-dev"
#else
    #define BASE_TOPIC "openair"
#endif
#define AVAILABILITY_TOPIC BASE_TOPIC "/availability"


class MQTT : public RepositorySubscriber<Sensor> {
    private:
        const int PUBLISH_INTERVAL = 1000;
        const int SUBSCRIBE_INTERVAL = 100;
        const int NO_WIFI = 5000;
        const int NO_CONFIG = 5000;
        const int RECONNECT = 10000;
        std::queue<std::shared_ptr<HaDiscoverable>> discoveryQueue;
        std::queue<std::string> subscriptionQueue;
        std::set<std::string> sensorTopics;
        std::set<std::shared_ptr<MQTTSensor>> subscribers;
        std::set<std::string> subscriptions;

        std::set<std::shared_ptr<HaDiscoverable>> haDiscoverables {
            std::make_shared<HaFanSpeed>(),
            std::make_shared<IpHaSensor>("ip", "ip"),
            std::make_shared<FreeMemoryHaSensor>("free-memory","free memory"),
            std::make_shared<NumericHaSensor>("fan-rpm","fan RPM", [this](){
                char buff[sizeof(int)*8+1];
                itoa(this->getRPM(), buff, 10);
                return std::string(buff);
            }, true),
            std::make_shared<NumericHaSensor>("fan-stalls","fan stalls", [this](){
                char buff[sizeof(int)*8+1];
                itoa(this->getStalls(), buff, 10);
                return std::string(buff);
            }, true),
            std::make_shared<GenericHaSensor>("governor", "Current speed governor", [this](){
                return this->getGovernor();
            }, true),
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

        int getRPM() {
            return DI::GetContainer()->resolve<Tachometer>()->RPM();
        }

        int getStalls() {
            return DI::GetContainer()->resolve<Fan>()->stallCount();
        }

        std::string getGovernor() {
            return DI::GetContainer()->resolve<Fan>()->governor();
        }

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
            client.setClientId(DI::GetContainer()->resolve<Device>()->getName().c_str());
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
            Log.infoln("Queing discovery");
            this->discoveryPublished = true;
            for(auto ha : this->haDiscoverables) {
                this->discoveryQueue.emplace(ha);
            }

            auto sensorRepo = DI::GetContainer()->resolve<SensorRepository>();
            for (auto uuid : sensorRepo->getUuids()) {
                this->sensorUuids.emplace(uuid);
                auto sensor = sensorRepo->getInstance(uuid);
                for (auto haSensor : sensor->getHaSensors()) {
                    this->discoveryQueue.emplace(haSensor);
                }
            }
        }

        void onConnect(bool sessionPresent) {
            Log.infoln("MQTT connected");
            this->connecting = false;

            this->client.publish(AVAILABILITY_TOPIC, 0, true, HA_ONLINE);
            this->client.subscribe(HOMEASSISTANT_STATUS_TOPIC, 0);

            for (std::string topic : this->sensorTopics) {
                this->client.subscribe(topic.c_str(), 0);
            }

            this->resubscribe();
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

            if (strcmp(topic, HOMEASSISTANT_STATUS_TOPIC) == 0) {
                if (strcmp(HA_ONLINE, strval) == 0) {
                    this->sendDiscovery();
                    delete[] strval;
                    return;
                }
            }

            for (std::shared_ptr<MQTTSensor> s : this->subscribers) {
                if (strcmp(topic, s->getTopic().c_str()) == 0) {
                    s->onMessage(strval);
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
            this->client.publish(AVAILABILITY_TOPIC, 0, true, HA_ONLINE);
            this->processSensorChanges();
            this->publishDiagnostics();
            this->publishSensors();
        }

        void subscribe(std::shared_ptr<MQTTSensor> sensor) {
            if (!sensor) {
                return;
            }

            Log.traceln("Subscribing %s -> %s", sensor->getName().c_str(), sensor->getTopic().c_str());
            std::string topic = sensor->getTopic();
            if (!topic.size()) {
                Log.errorln("Skipping subscription of sensor %s because topic is empty", sensor->getUuid().c_str());
                return;
            }
            this->client.subscribe(topic.c_str(), 1);
            this->subscriptions.emplace(topic);
        }

        void unsubscribe(std::shared_ptr<MQTTSensor> sensor) {
            if (!sensor) {
                return;
            }

            Log.traceln("Removing subscription for %s", sensor->getName().c_str());
            this->subscribers.erase(sensor);
            this->resubscribe();
        }

        void resubscribe() {
            Log.traceln("Resubscribing");
            for (std::string s : this->subscriptions) {
                this->client.unsubscribe(s.c_str());
            }
            this->subscriptions.clear();

            for (std::shared_ptr<MQTTSensor> s : this->subscribers) {
                subscribe(s);
            }
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

            auto sensorRepo = DI::GetContainer()->resolve<SensorRepository>();
            sensorRepo->registerSubscriber(std::shared_ptr<RepositorySubscriber<Sensor>>(this));
        }

        void instanceAdded(std::shared_ptr<Sensor> sensor) override {
            auto mqttSensor = std::dynamic_pointer_cast<MQTTSensor>(sensor);
            if (!mqttSensor) {
                return;
            }

            this->addSubscriber(mqttSensor);
        }

        void instanceRemoved(std::shared_ptr<Sensor> sensor) override {
            auto mqttSensor = std::dynamic_pointer_cast<MQTTSensor>(sensor);
            if (!mqttSensor) {
                return;
            }

            this->removeSubscriber(mqttSensor);
        }

        void setHostname(const std::string &hostname) {
            if (this->hostname == hostname) {
                return;
            }
            this->hostname = hostname;
            this->reConnect();
        }

        void setPort (int port) {
            if (this->port == port) {
                return;
            }
            this->port = port;
            this->reConnect();
        }

        void setUser(const std::string &user) {
            if (this->user == user) {
                return;
            }
            this->user = user;
            this->reConnect();
        }

        void setPass(const std::string &pass) {
            if (this->pass == pass) {
                return;
            }
            this->pass = pass;
            this->reConnect();
        }

        void addSubscriber(std::shared_ptr<MQTTSensor> sensor) {
            this->subscribers.emplace(sensor);
            this->subscribe(sensor);
        }

        void removeSubscriber(std::shared_ptr<MQTTSensor> sensor) {
            this->unsubscribe(sensor);
        }

        void addSensorTopic(const std::string &topic) {
            if (topic == "") {
                return;
            }

            int sizeBefore = this->sensorTopics.size();
            Log.traceln("Adding sensor topic %s", topic.c_str());
            this->sensorTopics.emplace(topic);

            if (this->sensorTopics.size() != sizeBefore) {
                this->subscriptionQueue.emplace(topic);
            }
        }

        void removeSensorTopic(const std::string &topic) {
            Log.traceln("Removing topic %s", topic.c_str());
            this->client.unsubscribe(topic.c_str());
            this->sensorTopics.erase(topic);
        }

        std::set<std::string> getSensorTopics() {
            return this->sensorTopics;
        }

        TickType_t runTask() {
            if (!this->connect())
                return RECONNECT;

            if (this->discoveryQueue.size() > 0) {
                auto item = this->discoveryQueue.front();
                this->sendHaSensorDiscovery(item);
                this->discoveryQueue.pop();
                return PUBLISH_INTERVAL;
            }

            if (this->subscriptionQueue.size() > 0) {
                std::string item = this->subscriptionQueue.front();
                this->client.subscribe(item.c_str(), 0);
                this->subscriptionQueue.pop();
                Log.infoln("Subscribing to %s", item.c_str());
                return SUBSCRIBE_INTERVAL; 
            }

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
            if(json["sensorTopics"].is<JsonArray>()) {
                for (auto item : json["sensorTopics"].as<JsonArray>()) {
                    this->addSensorTopic(item.as<std::string>());
                }
            }
        }

        JsonDocument toJson() {
            JsonDocument doc;

            doc["hostname"] = this->hostname;
            doc["port"] = this->port;
            doc["user"] = this->user;
            doc["pass"] = this->pass;
            int i = 0;
            for (std::string topic : this->sensorTopics) {
                doc["sensorTopics"][i++] = topic;
            }

            return doc;
        }

};
