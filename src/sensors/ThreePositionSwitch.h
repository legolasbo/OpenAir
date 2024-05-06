#pragma once

#include <HardwareSerial.h>
#include "Sensor.h"
#include "../Measurements.h"

enum SelectedMode {
  MODE_LOW = 1,
  MODE_MEDIUM = 2,
  MODE_HIGH = 3
};

class ThreePositionSwitch : public Sensor {
  private:
    static const int defaultAddress = 32;
    volatile SelectedMode lastMode = MODE_LOW;

    int getAddress() {
      std::shared_ptr<Option> address = this->getOption("address");
      return address->toIntOr(this->defaultAddress);
    }
                                  
  public:
    static const SensorType sensorType = ThreePositionSwitchSensor;

    SensorType getSensorType(){
        return ThreePositionSwitch::sensorType;
    }

    Measurements::MeasurementTypeList getMeasurementTypes() {
      return Measurements::MeasurementTypeList {
        Measurements::Type::SwitchPositionMeasurement
      };
    }

    Measurements::Measurement provide (Measurements::Type mt) {
        switch (mt) {
            case Measurements::Type::SwitchPositionMeasurement: return Measurements::Measurement([this]() {
                return this->getSelectedPosition();
            });
            case Measurements::Type::SwitchPositionCountMeasurement: return Measurements::Measurement([this]() {
                return this->getNumberOfPositions();
            });
            default: return Measurements::Measurement();
        }
    }

    std::unordered_map<std::string, std::shared_ptr<Option>> availableOptions() {
      auto options = Sensor::availableOptions();

      std::vector<Option> defaultConnectorOptions = {Option(X4, ToString(X4)), Option(X6, ToString(X6))};
      options.emplace("connector", std::make_shared<ListOption>(X4, defaultConnectorOptions, "Connector", true));
      options.emplace("address", std::make_shared<BoundedOption>(this->defaultAddress, 32, 39, "Address", true));

      return options;
    }

    std::vector<ConnectionType> getSupportedConnectionTypes() {
        return {I2C};
    }

    uint8_t getNumberOfPositions() {
      return 3;
    }

    uint8_t getSelectedPosition() {
        return this->lastMode;
    }

    void loop() {
      if (this->shouldMeasure()) {
        this->lastMode = this->read();
      }
    }

    SelectedMode read() {
      SensorConnector conn = this->getOption("connector")->toConnector();

      if (conn == UNKNOWN_CONNECTOR) {
        Log.errorln("Unable to read three position switch: unknown connector.");
        return SelectedMode::MODE_LOW;
      }

      TwoWire &i2c = DI::GetContainer()->resolve<I2CManager>()->fromConnector(conn); 

      SelectedMode mode = MODE_LOW;
      if (i2c.requestFrom(this->getAddress(), 1)) {
          int result = max(0, min(3, i2c.read()));
          mode = (SelectedMode) result;
      };

      return mode;
    }
};

