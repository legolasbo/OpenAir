#include <Arduino.h>
#include <vector>

#include "constants.h"
#include "configuration/SensorConfigurations.h"
#include "sensors/SensorFactory.h"
#include "speedCalculators/ThreePositionCalculator.h"
#include "speedCalculators/SHT20SpeedCalculator.h"
#include "inputs/tachometer.h"
#include "outputs/fan.h"
#include "interface/web.h"

Tachometer tachometer(TACHOMETER);
Fan fan(PWM_MOTOR_SPEED, tachometer);
I2CManager i2cManager(Wire, Wire1);
SensorFactory sensorFactory(&i2cManager);
SensorConfigurations sensorConfigs;

std::vector<SpeedCalculator *> calculators;

void setup() {
  Serial.begin(115200);
  Serial.println("Booting...");
  
  startInterface(sensorFactory, sensorConfigs);

  calculators.push_back(new SHT20SpeedCalculator(sensorFactory.createI2CSensor(SHT20Sensor, i2cManager.fromConnector(X4))));
  calculators.push_back(new ThreePositionCalculator(sensorFactory.createI2CSensor(ThreePositionSwitchSensor, i2cManager.fromConnector(X6))));

  tachometer.begin();
  fan.begin();
}

void loop() {
  loopInterface();
  fan.loop();

  int calculatedSpeed = 0;
  for (auto&& c : calculators) {
    calculatedSpeed = max(c->calculate(), calculatedSpeed);
  }

  Serial.printf("Calculated speed: %d \n", calculatedSpeed);
  fan.setFanSpeed(calculatedSpeed);
}
