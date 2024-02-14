#ifndef THREE_POSITION_SWITCH_CALCULATOR_CONFIGURATION
#define THREE_POSITION_SWITCH_CALCULATOR_CONFIGURATION

#include "CalculatorConfiguration.h"
#include <vector>
#include "../sensors/SensorTypes.h"

class ThreePositionSwitchCalculatorConfiguration : public CalculatorConfiguration {
    public:
    ThreePositionSwitchCalculatorConfiguration() : CalculatorConfiguration() {
    }
    ThreePositionSwitchCalculatorConfiguration(const char * uuid) : CalculatorConfiguration(uuid) {
    }

    virtual std::vector<SensorType> supportedSensorTypes() {
        return std::vector<SensorType>{
            ThreePositionSwitchSensor,
        };
    }

    virtual const char * machineName() {
        return "3possw";
    }

    virtual std::string editForm() {
        std::string out = "Three Position Switch ";
        out.append(CalculatorConfiguration::editForm());
        return out;
    }

    static ThreePositionSwitchCalculatorConfiguration * fromJson(JsonObject json) {
        if (!json.containsKey("uuid")) {
            return nullptr;
        }

        const char * uuid = json["uuid"].as<const char*>();
        return new ThreePositionSwitchCalculatorConfiguration(uuid);
    }

};

#endif
