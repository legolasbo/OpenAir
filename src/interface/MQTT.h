#pragma once

#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <string>
#include <WiFi.h>
#include "api.h"
#include "../MQTT.h"

class MqttAPI : public API {
    public:
        void configureCallbacks(AsyncWebServer &server) override {
            server.on("/api/mqtt", HTTP_GET, [this](AsyncWebServerRequest * request) {
                this->respondJson(DI::GetContainer()->resolve<MQTT>()->toJson(), request);
            });
            server.on("/api/mqtt", HTTP_POST, [this](AsyncWebServerRequest * request) {
                auto mqtt = DI::GetContainer()->resolve<MQTT>();

                std::set<std::string> topics;

                for (int i = 0; i < request->args(); i++) {
                    auto argName = request->argName(i);
                    if (argName == "hostname")
                        mqtt->setHostname(request->arg(i).c_str());
                    if (argName == "port")
                        mqtt->setPort(request->arg(i).toInt());
                    if (argName == "user")
                        mqtt->setUser(request->arg(i).c_str());
                    if (argName == "pass")
                        mqtt->setPass(request->arg(i).c_str());
                    if (argName == "sensorTopics[]")
                        topics.emplace(request->arg(i).c_str());
                }

                for (std::string topic : mqtt->getSensorTopics()) {
                    if (topics.find(topic) == topics.end()) {
                        mqtt->removeSensorTopic(topic);
                    }
                }

                for (std::string topic : topics) {
                    mqtt->addSensorTopic(topic);
                }

                request->redirect("/mqtt");
                DI::GetContainer()->resolve<Configuration>()->save();
            });
        }

};
