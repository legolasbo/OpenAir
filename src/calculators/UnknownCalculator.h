#include <stdexcept>
#include "../Measurements.h"
#include "SpeedCalculator.h"
#include "../sensors/sensor.h"
  
class UnknownCalculator : public SpeedCalculator {
    private:
    
    protected:
        int _calculate() {
            return 0;
        }
        const char * name() {
            return "Unknown";
        }

    public:
        virtual bool isValid() {
            return false;
        }

        virtual CalculatorType type() {
            return UNKNOWN_CALCULATOR_TYPE;
        }

        virtual std::unordered_map<std::string, std::shared_ptr<Option>> availableOptions() {
            return {};
        };
        
        virtual Measurements::MeasurementTypeList supportedMeasurementTypes() {
            return {};
        }
};
