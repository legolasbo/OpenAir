#include "SpeedCalculator.h"
#include "SHT20Reader.h"
  
class SHT20SpeedCalculator : public SpeedCalculator {
    private:
        SHT20Reader * sensor;

    public:
        SHT20SpeedCalculator(SHT20Reader *sensor) {
            this->sensor = sensor;
        }

        int calculate() {
            return (int) this->sensor->takeReading().humidity;
        }

        std::string name() {
            return "SHT20SpeedCalculator";
        }

};

  
  