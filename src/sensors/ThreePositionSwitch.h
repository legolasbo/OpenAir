#pragma once

#include <HardwareSerial.h>
#include "Sensor.h"
#include "../Measurements.h"

enum SelectedMode {
  MODE_LOW = 1,
  MODE_MEDIUM = 2,
  MODE_HIGH = 3
};

class ThreePositionSwitch : public  Sensor, public Measurements::SwitchPosition {
  private:
    static const int defaultAddress = 32;

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
        return this->read();
    }

    SelectedMode read() {
      SensorConnector conn = this->getOption("connector")->toConnector();

      if (conn == UNKNOWN_CONNECTOR) {
        Log.errorln("Unable to read three position switch: unknown connector.");
        return SelectedMode::MODE_LOW;
      }

      auto i2c = DI::GetContainer()->resolve<I2CManager>()->fromConnector(conn); 

      i2c.begin();
      SelectedMode mode = MODE_LOW;
      
      try {
        i2c.requestFrom(this->getAddress(), 1);
        if (i2c.available()) {
            mode = (SelectedMode) i2c.read();
        }
      }
      catch(const std::exception& e) {
          Log.errorln("Failed to read Three Position Switch on address %i because: %s", this->getAddress(), e.what());
      }
      
      i2c.end();
      return mode;
    }
};

