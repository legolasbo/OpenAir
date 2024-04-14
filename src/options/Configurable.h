#pragma once

#include <unordered_map>
#include "Option.h"

class Configurable {
    private:
        std::unordered_map<std::string, Option> options;

        void logOptions() {
            Log.traceln("%s: Available options:", typeid(*this).name());
            for (auto &&i : this->availableOptions()) {
                Log.traceln("%s: %s", i.first, i.second.toCharPtr());
            }
            Log.traceln("%s: Configured options:", typeid(*this).name());
            for (auto &&i : this->options) {
                Log.traceln("%s: %s", i.first, i.second.toCharPtr());
            }
        }

    public:
        virtual ~Configurable() = default;

        virtual std::unordered_map<std::string, Option> availableOptions() = 0;

        bool isAvailableOption(std::string name) {
            auto options = this->availableOptions();
            return options.find(name) != options.end();
        }

        Option getOption(std::string name) {
            if (!this->isAvailableOption(name)) {
                Log.errorln("%s: Unknown option %s", typeid(*this).name(), name);
                return Option();
            }

            if (this->options.find(name) == this->options.end()) {
                Log.warningln("%s: Unconfigured option %s", typeid(*this).name(), name);
                return this->availableOptions().at(name);
            }

            return this->options.at(name);
        }

        bool setOption(std::string name, std::string value) {
            switch(this->getOption(name).getType()) {
                case Option::Type::INTEGER : return this->setOption(name, Option(atoi(value.c_str())));
            }
            return this->setOption(name, Option(value));
        }

        bool setOption(std::string name, Option value) {
            if (!this->isAvailableOption(name)) {
                Log.warningln("Option %s is not one of the available options.", name);
                this->logOptions();
                return false;
            }

            if (this->availableOptions().at(name).getType() != value.getType()) {
                Log.warningln("Unable to set option %s to %s. Types don't match", name, value.toStr());
                return false;
            }

            this->options.erase(name);
            bool success = this->options.emplace(name, value).second;
            Log.verboseln("Setting option %s to value %s %s", name.c_str(), value.toCharPtr(), success ? "succeeded" : "failed");
            return success;
        }
};
