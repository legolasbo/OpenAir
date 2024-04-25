#include <stdexcept>
#include "../Measurements.h"
#include "SpeedCalculator.h"
#include "../sensors/sensor.h"
  
class HumidityCalculator : public SpeedCalculator {
    private:
        std::shared_ptr<Measurements::Humidity> sensor;
    
    protected:
        int _calculate() {
            return (int) this->sensor->getHumidity() - 40;
        }
        const char * name() {
            return "Humidity";
        }

    public:
        HumidityCalculator(std::shared_ptr<Measurements::Humidity>sensor) {
            this->sensor = sensor;
        }

};
