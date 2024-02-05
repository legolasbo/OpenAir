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
I2CManager *i2cManager;
SensorFactory *sensorFactory;
SensorConfigurations *sensorConfigs;

std::vector<SpeedCalculator *> calculators;

void setup() {
  Serial.begin(115200);
  Serial.println("Booting...");

  i2cManager = new I2CManager();
  sensorFactory = new SensorFactory(i2cManager);
  sensorConfigs = new SensorConfigurations();

  startInterface(sensorFactory, sensorConfigs);
  SensorConfiguration * sht20 = new SensorConfiguration(X4, I2C, SHT20Sensor);
  SensorConfiguration * threepos = new SensorConfiguration(X6, I2C, ThreePositionSwitchSensor);

  sensorConfigs->add(sht20);
  sensorConfigs->add(threepos);

  calculators.push_back(new SHT20SpeedCalculator(sensorFactory->fromConfiguration(sht20)));
  calculators.push_back(new ThreePositionCalculator(sensorFactory->fromConfiguration(threepos)));

  // tachometer.begin();
  // fan.begin();
}

int calculatedSpeed = 0;
void loop() {
  loopInterface();

  int newSpeed = 0;
  for (auto&& c : calculators) {
    newSpeed = max(c->calculate(), newSpeed);
  }

  if (newSpeed != calculatedSpeed) {
    calculatedSpeed = newSpeed;
    Serial.printf("Calculated speed: %d \n", calculatedSpeed);
  }

  // fan.setFanSpeed(calculatedSpeed);
  // fan.loop();
}
