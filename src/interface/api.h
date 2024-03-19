#ifndef INTERFACE_API_H
#define INTERFACE_API_H

#include <ESPAsyncWebServer.h>
#include "../configuration/Configuration.h"
#include "responses.h"
#include "../DependencyInjectionContainer.hpp"

class API {
    protected:
    DI * container;
    Configuration * config;
    AsyncWebServer * server;

    std::string extractValidUuid(AsyncWebServerRequest * request) {
        if (!request->hasParam("uuid") && !request->hasArg("uuid")) {
            internalServerErrorResponse(request, "uuid is required to configure a sensor");
            return "";
        }

        std::string uuid;
        if (request->hasParam("uuid")) {
            uuid = request->getParam("uuid")->value().c_str();
        }
        else {
            uuid = request->arg("uuid").c_str();
        }

        if (!this->config->getSensors()->exists(uuid)) {
            internalServerErrorResponse(request, "Unknown uuid");
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

#endif
