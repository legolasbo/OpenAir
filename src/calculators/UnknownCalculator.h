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
        UnknownCalculator() {}
        virtual bool isValid() {
            return false;
        }

        virtual std::unordered_map<std::string, std::shared_ptr<Option>> availableOptions() {
            return {};
        };
};
