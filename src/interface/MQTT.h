#pragma once

#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <string>
#include <WiFi.h>
#include "api.h"
#include "mqtt/MQTT.h"

class MqttAPI : public API {
    public:
        void configureCallbacks(AsyncWebServer &server) override {
            server.on("/api/mqtt", HTTP_GET, [this](AsyncWebServerRequest * request) {
                this->respondJson(DI::GetContainer()->resolve<MQTT>()->toJson(), request);
            });
            server.on("/api/mqtt", HTTP_POST, [this](AsyncWebServerRequest * request) {
                auto mqtt = DI::GetContainer()->resolve<MQTT>();
                if (request->hasArg("hostname"))
                    mqtt->setHostname(request->arg("hostname").c_str());
                if (request->hasArg("port"))
                    mqtt->setPort(request->arg("port").toInt());
                if (request->hasArg("user"))
                    mqtt->setUser(request->arg("user").c_str());
                if (request->hasArg("pass"))
                    mqtt->setPass(request->arg("pass").c_str());
                request->redirect("/mqtt");
                DI::GetContainer()->resolve<Configuration>()->save();
            });
        }

};
