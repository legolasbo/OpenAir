#ifndef THREE_POSITION_SWITCH_H
#define THREE_POSITION_SWITCH_H

#include <HardwareSerial.h>
#include "I2CSensor.h"
#include "SensorTypes.h"
#include "../Measurements.h"

enum SelectedMode {
  MODE_LOW = 1,
  MODE_MEDIUM = 2,
  MODE_HIGH = 3
};

class ThreePositionSwitch : public  I2CSensor, public Measurements::SwitchPosition {
  private:
    static const int defaultAddress = 32;

    int getAddress() {
      Option address = this->configuredOptions().at("address");
      return address.toIntOr(this->defaultAddress);
    }
                                  
  public:
    ThreePositionSwitch(std::string uuid, TwoWire *i2cBus) : I2CSensor(uuid, i2cBus) {}

    static const SensorType sensorType = ThreePositionSwitchSensor;

    SensorType getSensorType(){
        return ThreePositionSwitch::sensorType;
    }

    Measurements::MeasurementTypeList getMeasurementTypes() {
      return Measurements::MeasurementTypeList {
        Measurements::Type::SwitchPositionMeasurement
      };
    }

    std::unordered_map<const char *, Option> availableOptions() {
      return {
        {"address", BoundedOption(this->defaultAddress, 32, 39)}
      };
    }

    uint8_t getNumberOfPositions() {
      return 3;
    }

    uint8_t getSelectedPosition() {
        return this->read();
    }

    SelectedMode read() {
      this->i2cBus->begin();
      SelectedMode mode = MODE_LOW;
      
      try {
        this->i2cBus->requestFrom(this->getAddress(), 1);
        if (this->i2cBus->available()) {
            mode = (SelectedMode) this->i2cBus->read();
        }

      }
      catch(const std::exception& e) {
          Serial.println(e.what());
      }
      
      this->i2cBus->end();
      return mode;
    }
};

#endif