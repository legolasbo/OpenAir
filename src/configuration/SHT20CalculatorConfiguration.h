#ifndef SHT20_CALCULATOR_CONFIGURATION
#define SHT20_CALCULATOR_CONFIGURATION

#include "CalculatorConfiguration.h"
#include <vector>
#include "../sensors/SensorTypes.h"

class SHT20CalculatorConfiguration : public CalculatorConfiguration {
    public:
    SHT20CalculatorConfiguration() : CalculatorConfiguration() {
    }

    SHT20CalculatorConfiguration(const char * uuid) : CalculatorConfiguration(uuid) {
    }

    virtual std::vector<SensorType> supportedSensorTypes() {
        return std::vector<SensorType> {
            SHT20Sensor,
        };
    }

    virtual const char * machineName() {
        return "sht20";
    }

    virtual std::string editForm() {
        std::string out = "SHT20 ";
        out.append(CalculatorConfiguration::editForm());
        return out;
    }

    static SHT20CalculatorConfiguration * fromJson(JsonObject json) {
        if (!json.containsKey("uuid")) {
            return nullptr;
        }

        const char * uuid = json["uuid"].as<const char *>();
        return new SHT20CalculatorConfiguration(uuid);

    }
};

#endif
