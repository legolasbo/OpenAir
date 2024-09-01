#pragma once

#include <ESPAsyncWebServer.h>
#include "../configuration/Configuration.h"
#include "responses.h"
#include "../DependencyInjectionContainer.hpp"
#include "../options/Configurable.h"

class Value {
    public:
        virtual String getValue() = 0;
        virtual bool add(String key, std::shared_ptr<Value> value) = 0;
        virtual bool contains(String key) = 0;
        virtual std::shared_ptr<Value> get(String key) = 0;
        virtual std::map<std::string, std::string> getValues() = 0;
};

class StringValue : public Value {
    private:
        String value;

    public:
        StringValue(String value) {
            this->value = value;
        }

        String getValue() override {
            return this->value;
        }

        boolean add(String key, std::shared_ptr<Value> value) override{
            Log.errorln("Unable to add to a string value");
            return false;
        }

        bool contains(String key) override {
            return false;
        }

        std::shared_ptr<Value> get(String key) override {
            return nullptr;
        }

        std::map<std::string, std::string> getValues() override {
            return {};
        }
};

class NestedValue : public Value {
    private:
        std::map<String, std::shared_ptr<Value>> children;
    
    public:
        NestedValue(String key, std::shared_ptr<Value> value) {
            this->children.emplace(key, value);
        }

        String getValue() override {
            return String("Nested");
        }

        boolean add(String key, std::shared_ptr<Value> value) override{
            auto result = this->children.emplace(key, value);
            return result.second;
        }

        bool contains(String key) override {
            return this->children.find(key) == this->children.end();
        }

        std::shared_ptr<Value> get(String key) {
            if (this->contains(key)) {
                return this->children.at(key);
            }
            return nullptr;
        }

        std::map<std::string, std::string> getValues() override {
            std::map<std::string, std::string> values;

            for (auto c : children) {
                values.emplace(c.first.c_str(), c.second->getValue().c_str());
            }

            return values;
        }

};

class API {
    protected:

    std::map<String, std::shared_ptr<Value>> extractFormValues(AsyncWebServerRequest * request) {
        std::map<String, std::shared_ptr<Value>> values;

        for (size_t i = 0; i < request->args(); i++) {
            String argName = request->argName(i);
            String val = request->arg(argName);

            int foundIndex = argName.indexOf('[');
            if (foundIndex == -1) {
                values.emplace(argName, std::make_shared<StringValue>(val));
                Log.traceln("Placing %s as StringValue %s", argName.c_str(), val.c_str());
                continue;
            }
            
            String name = argName.substring(0, foundIndex);
            String key = argName.substring(foundIndex+1, argName.length() -1);

            if (values.find(name) == values.end()) {
                values.emplace(name, std::make_shared<NestedValue>(key, std::make_shared<StringValue>(val)));
                Log.traceln("Placing %s as new NestedValue %s with key %s", argName.c_str(), key.c_str(), val.c_str());
                continue;
            }

            values.at(name)->add(key, std::make_shared<StringValue>(val));
            Log.traceln("Placing %s as addition to NestedValue %s with key %s", argName.c_str(), key.c_str(), val.c_str());
        }

        return values;
    }

    void processFormValues(std::shared_ptr<Configurable> conf, AsyncWebServerRequest * request) {
        std::map<String, std::shared_ptr<Value>> values = this->extractFormValues(request);

        for (auto i : values) {
            if (!conf->isAvailableOption(i.first.c_str())) {
                continue;
            }

            auto values = i.second->getValues();
            if (values.size()) {
                conf->setOption(i.first.c_str(), values);
                continue;
            }

            conf->setOption(i.first.c_str(), i.second->getValue().c_str());
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
    virtual ~API() {}
    virtual void configureCallbacks (AsyncWebServer & server) = 0;
};

