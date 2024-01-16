#include "SpeedCalculator.h"
#include "../ModeSelect.h"

class ModeSelectSpeedCalculator : public SpeedCalculator {
    private:
        ModeSelectSwitch *selectSwitch;
        int high, medium;

    public:
        ModeSelectSpeedCalculator(ModeSelectSwitch *select, int mediumSpeed = 50, int highSpeed = 100) {
            selectSwitch = select;

            if (highSpeed > 100) {
                high = 100;
            }
            else if (highSpeed < 10) {
                high = 10;
            }
            else {
                high = highSpeed;
            }

            if (mediumSpeed < 0 || mediumSpeed > high) {
                medium = high / 2;
            }
            else {
                medium = mediumSpeed;
            }
        }

        virtual int calculate();
        virtual std::string name();
};
