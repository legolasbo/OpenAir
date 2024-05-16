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
      auto opt = this->getOption("address")->as<IntegerOption>();
      if (opt == nullptr) {
        Log.errorln("Could not determine address defaulting to %d", this->defaultAddress);
        return this->defaultAddress;
      }
      return opt->getValue();
    }
                                  
  public:
    SensorType getSensorType() override {
        return ThreePositionSwitchSensor;
    }

    Measurements::MeasurementTypeList getMeasurementTypes() override {
      return Measurements::MeasurementTypeList {
        Measurements::Type::SwitchPositionMeasurement,
        Measurements::Type::SwitchPositionCountMeasurement,
      };
    }

    Measurements::Measurement provide (Measurements::Type mt) override {
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

    std::map<std::string, std::shared_ptr<Option>> availableOptions() override {
      auto options = Sensor::availableOptions();

      std::vector<std::shared_ptr<Option>> defaultConnectorOptions = {
        createOption(X4, ToString(X4)), 
        createOption(X6, ToString(X6))
      };
      options.emplace("connector", std::make_shared<ListOption<SensorConnector>>(X4, defaultConnectorOptions, "Connector", true));
      options.emplace("address", std::make_shared<BoundedOption>(this->defaultAddress, 32, 39, "Address", true));

      return options;
    }

    std::vector<ConnectionType> getSupportedConnectionTypes() override {
        return {I2C};
    }

    uint8_t getNumberOfPositions() {
      return 3;
    }

    uint8_t getSelectedPosition() {
        return this->lastMode;
    }

    void loop() override {
      if (this->shouldMeasure()) {
        this->lastMode = this->read();
      }
    }

    SelectedMode read() {
      ListOption<SensorConnector>* listOptPtr = this->getOption("connector")->as<ListOption<SensorConnector>>();
      if (listOptPtr == nullptr) {
        Log.errorln("Could not cast %s to %s", listOptPtr->typeName().c_str(), typeid(ListOption<SensorConnector>*).name());
        return SelectedMode::MODE_LOW;
      }
      auto conn = listOptPtr->getValue();

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

