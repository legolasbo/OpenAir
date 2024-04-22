#include <stdexcept>
#include "../Measurements.h"
#include "SpeedCalculator.h"
#include "../sensors/sensor.h"
  
class HumidityCalculator : public SpeedCalculator {
    private:
        Measurements::Humidity * sensor;
    
    protected:
        int _calculate() {
            return (int) this->sensor->getHumidity() - 40;
        }
        const char * name() {
            return "Humidity";
        }

    public:
        HumidityCalculator(Measurements::Humidity *sensor) {
            this->sensor = sensor;
        }

};
