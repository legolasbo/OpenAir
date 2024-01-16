#include "../ModeSelect.h"
#include "ModeSelectSpeedCalculator.h"

int ModeSelectSpeedCalculator::calculate() {
    switch (selectSwitch->read()) {
        case MODE_MEDIUM: return medium;
        case MODE_HIGH: return high;
        default: return 0;
    }
}

std::string ModeSelectSpeedCalculator::name() {
    return "ModeSelectSpeedCalculator";
}
