#include "SpeedCalculator.h"
#include "../sensors/ThreePositionSwitch.h"

class ThreePositionCalculator : public SpeedCalculator {
    private:
        ThreePositionSwitch *selectSwitch;
        int high, medium;

    public:
        ThreePositionCalculator(I2CSensor *sensor, int mediumSpeed = 50, int highSpeed = 100) {
            if (sensor->getType() != ThreePositionSwitchSensor) {
                throw std::invalid_argument("Incorrect sensor type. Only ThreePositionSwitch is supported");
            }
            ThreePositionCalculator((ThreePositionSwitch*)sensor, mediumSpeed, highSpeed);
        }

        ThreePositionCalculator(ThreePositionSwitch *select, int mediumSpeed = 50, int highSpeed = 100) {
            this->selectSwitch = select;

            high = constrain(highSpeed, 0, 100);
            medium = constrain(mediumSpeed, 0, highSpeed);
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
