#pragma once

#include <unordered_map>
#include "Option.h"

class Configurable {
    private:
        std::unordered_map<const char *, Option> options;

    public:
        virtual ~Configurable() = default;

        virtual std::unordered_map<const char *, Option> availableOptions() = 0;
        std::unordered_map<const char *, Option> configuredOptions() {
            if (options.size() == 0) {
                this->options = this->availableOptions();
            }
            return this->options;
        };

        bool setOption(const char * name, Option value) {
            if (this->availableOptions().find(name) == this->availableOptions().end()) {
                Log.warningln("Unable to set option %s to %s. it does not exist", name, value.toStr());
                return false;
            }

            if (this->availableOptions().at(name).getType() != value.getType()) {
                Log.warningln("Unable to set option %s to %s. Types don't match", name, value.toStr());
                return false;
            }

            Log.info("Set option %s to %s", name, value.toStr());
            this->options.emplace(name, value);
        }
};
