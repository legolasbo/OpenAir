#pragma once

#include <ESPAsyncWebServer.h>
#include "../configuration/Configuration.h"
#include "responses.h"
#include "../DependencyInjectionContainer.hpp"

class API {
    protected:
    DI * container;
    Configuration * config;
    AsyncWebServer * server;

    void processFormValues(Configurable * conf, AsyncWebServerRequest * request) {
        for (size_t i = 0; i < request->args(); i++) {
            auto argName = request->argName(i).c_str();
            Serial.printf("Processing option %d: %s -> %s\n", i, argName, request->arg(argName).c_str());
            conf->setOption(argName, request->arg(argName).c_str());
        }
    }

    std::string extractUuid(AsyncWebServerRequest * request) {
        if (!request->hasParam("uuid") && !request->hasArg("uuid")) {
            return "";
        }

        std::string uuid = request->hasParam("uuid") ? request->getParam("uuid")->value().c_str() : request->arg("uuid").c_str();
        if (uuid == "undefined") {
            return "";
        }

        return uuid;
    }

    void respondJson(JsonDocument doc, AsyncWebServerRequest * request) {
        String json;
        serializeJsonPretty(doc, json);
        request->send(200, String("text/json"), json);
    }

    public:
    virtual void initialize(DI * container, AsyncWebServer * server, Configuration * config) {
        this->container = container;
        this->server = server;
        this->config = config;
    }
};

