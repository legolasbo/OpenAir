#pragma once

#include <unordered_map>
#include "Option.h"
#include <UUID.h>

class Configurable {
    private:
        std::unordered_map<std::string, std::shared_ptr<Option>> options;

        void logOptions() {
            Log.traceln("%s: Available options:", typeid(*this).name());
            for (auto &&i : this->availableOptions()) {
                Log.traceln("%s: %s", i.first.c_str(), i.second->toStr().c_str());
            }
            Log.traceln("%s: Configured options:", typeid(*this).name());
            for (auto &&i : this->options) {
                Log.traceln("%s: %s", i.first.c_str(), i.second->toStr().c_str());
            }
        }

    protected:
        static std::string generateUuid() {
            UUID uuid;
            uuid.seed(esp_random(), esp_random());
            uuid.setRandomMode();
            uuid.generate();
            return std::string(uuid.toCharArray());
        }
        std::string uuid;

    public:
        Configurable() {
            this->uuid = Configurable::generateUuid();
        }
        Configurable(const std::string &uuid) {
            this->uuid = uuid == "" ? Configurable::generateUuid() : uuid;
        }
        virtual ~Configurable() = default;

        const std::string getUuid() {
            return this->uuid;
        }

        virtual std::unordered_map<std::string, std::shared_ptr<Option>> availableOptions() = 0;

        bool isAvailableOption(const std::string &name) {
            auto opts = this->availableOptions();
            return opts.find(name) != opts.end();
        }

        bool isConfiguredOption(const std::string &name) {
            return this->options.find(name) != this->options.end();
        }

        std::shared_ptr<Option> getOption(const std::string &name) {
            if (!this->isAvailableOption(name)) {
                Log.errorln("%s: Unknown option %s", typeid(*this).name(), name.c_str());
                return std::make_shared<Option>();
            }

            if (this->options.find(name) == this->options.end()) {
                std::shared_ptr<Option> defaultValue = this->availableOptions().at(name);
                Log.traceln("%s: Accessing unconfigured option %s. Using default value %s", typeid(*this).name(), name.c_str(), defaultValue->toStr().c_str());
                return defaultValue;
            }

            return this->options.at(name);
        }

        std::shared_ptr<Option> getDefaultOption(const std::string &name) {
            auto opts = this->availableOptions();
            if (opts.find(name) == this->options.end()) {
                Log.errorln("%s: Unknown option %s", typeid(*this).name(), name.c_str());
                return std::make_shared<Option>();
            }

            return opts.at(name);
        }

        bool setOption(const std::string &name, const std::string &value) {
            auto opt = this->getDefaultOption(name);
            switch(opt->getType()) {
                case Option::Type::INTEGER : return this->setOption(name, opt->newValue(atoi(value.c_str())));
            }
            return this->setOption(name, opt->newValue(value));
        }

        bool setOption(const std::string &name, std::shared_ptr<Option> value) {
            if (!this->isAvailableOption(name)) {
                Log.warningln("Option %s is not one of the available options.", name.c_str());
                this->logOptions();
                return false;
            }

            if (this->availableOptions().at(name)->getType() != value->getType()) {
                Log.warningln("Unable to set option %s to %s. Types don't match", name.c_str(), value->toStr());
                return false;
            }

            this->options.erase(name);
            bool success = this->options.emplace(name, value).second;
            Log.verboseln("Setting option %s to value %s %s", name.c_str(), value->toStr().c_str(), success ? "succeeded" : "failed");
            return success;
        }
        
        virtual JsonDocument toDetails() {
            JsonDocument doc;

            for (auto p : this->availableOptions()) {
                Log.traceln("Option %s", p.first.c_str());
                std::string label = p.first;
                label.at(0) = toUpperCase(label.at(0));

                doc[p.first]["label"] = label;
                doc[p.first]["type"] = "key/value";
                doc[p.first]["value"] = p.second->toStr();

                if (this->isConfiguredOption(p.first)) {
                    doc[p.first]["value"] = this->getOption(p.first)->toStr();
                }
            }

            return doc;
        }

        virtual JsonDocument toInterfaceOptions() {
            JsonDocument doc;

            for (auto p : this->availableOptions()) {

                if (this->isConfiguredOption(p.first)) {
                    doc[p.first] = this->getOption(p.first)->toInterfaceOption();
                }
                else {
                    doc[p.first] = p.second->toInterfaceOption();
                }
            }

            doc["uuid"]["type"] = "hidden";
            doc["uuid"]["value"] = this->uuid;

            return doc;
        }

        virtual bool configureFromJson(JsonObject doc) {
            if (doc.containsKey("uuid")) {
                this->uuid = doc["uuid"].as<std::string>();
            }

            if (!doc.containsKey("options")) {
                return true;
            }

            for (const auto& p : doc["options"].as<JsonObject>()) {
                std::string key = p.key().c_str();
                this->setOption(key, doc["options"][key].as<std::string>());
            }
            return true;
        }

        virtual JsonDocument toJson() {
            JsonDocument doc;

            doc["uuid"] = this->uuid;

            for (auto p : this->options) {
                doc["options"][p.first] = p.second->toStr();
            }

            return doc;
        }
};
