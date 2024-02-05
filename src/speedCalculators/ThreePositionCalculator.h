#include "SpeedCalculator.h"
#include "../sensors/ThreePositionSwitch/ThreePositionSwitch.h"

class ThreePositionCalculator : public SpeedCalculator {
    private:
        ThreePositionSwitch *selectSwitch;
        int high, medium;

        ThreePositionCalculator(int m, int h){
            this->high = constrain(h, 0, 100);
            this->medium = constrain(m, 0, h);
        }

    public:
        ThreePositionCalculator(ThreePositionSwitch *select, int mediumSpeed = 50, int highSpeed = 100) : ThreePositionCalculator(mediumSpeed, highSpeed) {
            this->selectSwitch = select;
        }

        ThreePositionCalculator(Sensor *sensor, int mediumSpeed = 50, int highSpeed = 100) : ThreePositionCalculator(mediumSpeed, highSpeed) {
            if (sensor->getSensorType() != ThreePositionSwitchSensor) {
                throw std::invalid_argument("Incorrect sensor type. Only ThreePositionSwitch is supported");
            }
            this->selectSwitch = (ThreePositionSwitch*)sensor;
        }

        int calculate() {
            switch (selectSwitch->read()) {
                case MODE_MEDIUM: return medium;
                case MODE_HIGH: return high;
                default: return 0;
            }
        }

        std::string name() {
            return "ThreePositionCalculator";
        }

};
