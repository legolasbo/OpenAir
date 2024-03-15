#include <Arduino.h>
#include <vector>

#include "constants.h"
#include "configuration/Configuration.h"
#include "sensors/SensorFactory.h"
#include "speedCalculators/SwitchPositionCalculator.h"
#include "inputs/tachometer.h"
#include "outputs/fan.h"
#include "interface/web.h"

Tachometer tachometer(TACHOMETER);
Fan fan(PWM_MOTOR_SPEED, tachometer);
I2CManager *i2cManager;
SensorFactory *sensorFactory;

std::vector<SpeedCalculator *> calculators;

void setup() {
  Serial.begin(115200);
  Serial.println("Booting...");

  i2cManager = new I2CManager();
  sensorFactory = new SensorFactory(i2cManager);
  
  Configuration * config = Configuration::fromFile("/config.json");
  startInterface(config);
  serializeJsonPretty(config->toJson(), Serial);

  SensorConfiguration * defaultSensor = new SensorConfiguration(X4, I2C, SHT20Sensor);
  defaultSensor->setName("Default sensor");
  if (config->getSensors()->getUuids().size() == 0) {
    config->getSensors()->add(defaultSensor);
    Serial.println("Added default sensor");
  }

  CalculatorConfiguration * defaultCalculator = new SHT20CalculatorConfiguration(config->getSensors());
  defaultCalculator->setName("Default calculator");
  defaultCalculator->setOption("sensor", defaultSensor->getUuid());
  if (config->getCalculators()->getUuids().size() == 0) {
    config->getCalculators()->add(defaultCalculator);
    Serial.println("Added default calculator");
  }

  // calculators.push_back(new SHT20SpeedCalculator(sensorFactory->fromConfiguration(config->getSensors()->get("x4_i2c_sht20"))));
  // calculators.push_back(new ThreePositionCalculator(sensorFactory->fromConfiguration(config->getSensors()->get("x6_i2c_3possw"))));
  // calculators.push_back(new SHT20SpeedCalculator(sensorFactory->fromConfiguration(new SensorConfiguration(X6, I2C, SHT20Sensor))));
  // calculators.push_back(new ThreePositionCalculator(sensorFactory->fromConfiguration(new SensorConfiguration(X4, I2C, ThreePositionSwitchSensor))));

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
