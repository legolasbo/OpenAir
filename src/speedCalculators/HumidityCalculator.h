#include <stdexcept>
#include "../Measurements.h"
#include "SpeedCalculator.h"
#include "../sensors/sensor.h"
  
class HumidityCalculator : public SpeedCalculator {
    private:
        Measurements::Humidity * sensor;

    public:
        HumidityCalculator(Sensor *sensor) {
            if (!sensor->supportsMeasurementType(Measurements::Type::HumidityMeasurement)) {
                throw std::invalid_argument("Incorrect sensor type. Only SHT20Sensor is supported");
            }
            this->sensor = (Measurements::Humidity*) sensor;
        }

        HumidityCalculator(Measurements::Humidity *sensor) {
            this->sensor = sensor;
        }

        int calculate() {
            return (int) this->sensor->getHumidity() - 40;
        }

};
