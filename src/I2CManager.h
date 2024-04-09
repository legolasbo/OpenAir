#pragma once

#include "constants.h"
#include <Wire.h>
#include "sensors/SensorConnectors.h"

class I2CManager {
    public:
        TwoWire * x4;
        TwoWire * x6;
        
        I2CManager() {
            this->x4 = &Wire;
            this->x6 = &Wire1;

            x4->begin(S1_SDA, S1_SCL);
            x6->begin(S2_SDA, S2_SCL);
        }

        TwoWire* fromConnector(SensorConnector connector) {
            switch(connector) {
                case X4: return this->x4;
                case X6: return this->x6;
                default: return this->x4;
            }
        }

};

