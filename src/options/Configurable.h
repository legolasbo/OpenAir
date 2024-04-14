#pragma once

#include <unordered_map>
#include "Option.h"

class Configurable {
    private:
        std::unordered_map<const char *, Option> options;

        void logOptions() {
            Log.traceln("%s: Available options:", typeid(*this).name());
            for (auto &&i : this->availableOptions()) {
                Log.traceln("%s: %s", i.first, i.second.toStr());
            }
            Log.traceln("%s: Configured options:", typeid(*this).name());
            for (auto &&i : this->options) {
                Log.traceln("%s: %s", i.first, i.second.toStr());
            }
        }

    public:
        virtual ~Configurable() = default;

        virtual std::unordered_map<const char *, Option> availableOptions() = 0;

        Option getOption(const char * name) {
            if (this->availableOptions().find(name) == this->availableOptions().end()) {
                Log.errorln("%s: Unknown option %s", typeid(*this).name(), name);
                return Option();
            }

            if (this->options.find(name) == this->options.end()) {
                Log.warningln("%s: Unconfigured option %s", typeid(*this).name(), name);
                return this->availableOptions().at(name);
            }

            return this->options.at(name);
        }

        bool setOption(const char * name, Option value) {
            if (this->availableOptions().find(name) == this->availableOptions().end()) {
                Log.warningln("Option %s is not one of the available options.", name);
                return false;
            }

            if (this->availableOptions().at(name).getType() != value.getType()) {
                Log.warningln("Unable to set option %s to %s. Types don't match", name, value.toStr());
                return false;
            }

            return this->options.emplace(name, value).second;
        }
};
