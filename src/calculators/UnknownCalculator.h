#include <stdexcept>
#include "../Measurements.h"
#include "SpeedCalculator.h"
#include "../sensors/sensor.h"
  
class UnknownCalculator : public SpeedCalculator {
    private:
    
    protected:
        int _calculate()  override {
            Log.warningln("UnknownCalculator::_calculate called. Maybe you failed to add your calculator to CalculatorRepository::create");
            return 0;
        }
        const char * name() override {
            Log.warningln("UnknownCalculator::name called. Maybe you failed to add your calculator to CalculatorRepository::create");
            return "Unknown";
        }

    public:
        bool isValid() override {
            Log.warningln("UnknownCalculator::isValid called. Maybe you failed to add your calculator to CalculatorRepository::create");
            return false;
        }

        CalculatorType type() override {
            Log.warningln("UnknownCalculator::type called. Maybe you failed to add your calculator to CalculatorRepository::create");
            return UNKNOWN_CALCULATOR_TYPE;
        }

        std::map<std::string, std::shared_ptr<Option>> availableOptions() override {
            Log.warningln("UnknownCalculator::availableOptions called. Maybe you failed to add your calculator to CalculatorRepository::create");
            return {};
        };
        
        Measurements::MeasurementTypeList supportedMeasurementTypes() override {
            Log.warningln("UnknownCalculator::supportedMeasurementTypes called. Maybe you failed to add your calculator to CalculatorRepository::create");
            return {};
        }
};
