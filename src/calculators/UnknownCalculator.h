#include <stdexcept>
#include "../Measurements.h"
#include "SpeedCalculator.h"
#include "../sensors/sensor.h"
  
class UnknownCalculator : public SpeedCalculator {
    private:
    
    protected:
        int _calculate()  override {
            return 0;
        }
        const char * name() override {
            return "Unknown";
        }

    public:
        bool isValid() override {
            return false;
        }

        CalculatorType type() override {
            return UNKNOWN_CALCULATOR_TYPE;
        }

        std::map<std::string, std::shared_ptr<Option>> availableOptions() override {
            return {};
        };
        
        Measurements::MeasurementTypeList supportedMeasurementTypes() override {
            return {};
        }
};
