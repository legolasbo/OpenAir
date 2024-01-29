#include "SpeedCalculator.h"
#include "../sensors/SHT20Sensor/SHT20Reader.h"
  
class SHT20SpeedCalculator : public SpeedCalculator {
    private:
        SHT20Reader * sensor;

    public:
        SHT20SpeedCalculator(I2CSensor *sensor) {
            if (sensor->getSensorType() != SHT20Sensor) {
                throw std::invalid_argument("Incorrect sensor type. Only SHT20Sensor is supported");
            }
            this->sensor = (SHT20Reader*)sensor;
        }

        SHT20SpeedCalculator(SHT20Reader *sensor) {
            this->sensor = sensor;
        }

        int calculate() {
            return (int) this->sensor->takeReading().humidity - 40;
        }

        std::string name() {
            return "SHT20SpeedCalculator";
        }

};

  
  