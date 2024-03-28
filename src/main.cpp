#include <Arduino.h>
#include <vector>

#include "constants.h"
#include "configuration/Configuration.h"
#include "factories/CalculatorFactory.hpp"
#include "sensors/SensorFactory.h"
#include "inputs/tachometer.h"
#include "outputs/fan.h"
#include "interface/web.h"
#include "DependencyInjectionContainer.hpp"

Tachometer tachometer(TACHOMETER);
Fan fan(PWM_MOTOR_SPEED, tachometer);
Configuration * config;
CalculatorFactory * calculatorFactory;
DI container;

void setup() {
  Serial.begin(115200);
  Serial.println("Booting...");

  config = Configuration::fromFile(&container, "/config.json");
  container.registerInstance<SensorFactory>(new SensorFactory(new I2CManager(), config->getSensors()));
  container.registerInstance<Configuration>(config);
  container.registerInstance<SensorConfigurations>(config->getSensors());
  container.registerInstance<CalculatorConfigurations>(config->getCalculators());
  calculatorFactory = new CalculatorFactory(config->getCalculators());
  
  startInterface(&container, config);


#if DEVELOPMENT_MODE == true
  SensorConfiguration * defaultSensor = new SensorConfiguration(&container, X4, I2C, SHT20Sensor);
  defaultSensor->setName("Default sensor");
  if (config->getSensors()->getUuids().size() == 0) {
    config->getSensors()->add(defaultSensor);
    Serial.printf("Added default sensor: %s\n", defaultSensor->getUuid().c_str());
  }

  CalculatorConfiguration * defaultCalculator = new HumidityCalculatorConfiguration(&container);
  defaultCalculator->setName("Default calculator");
  defaultCalculator->setOption("sensor", defaultSensor->getUuid());
  if (config->getCalculators()->getUuids().size() == 0) {
    config->getCalculators()->add(defaultCalculator);
    Serial.printf("Added default calculator %s\n", defaultCalculator->getUuid().c_str());
    serializeJsonPretty(defaultCalculator->toJson(), Serial);
  }
  // config->save();
#endif

  serializeJsonPretty(config->toJson(), Serial);

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

  if (config->isDirty()) {
    container.resolve<SensorFactory>().get()->destroyInstances();
    calculatorFactory->destroyInstances();

    config->markClean();
  }

  int newSpeed = 0;
  for (auto uuid : config->getCalculators()->getUuids()) {
    if (!config->getCalculators()->get(uuid)->isValid()) {
      continue;
    }

    SpeedCalculator * c = calculatorFactory->fromUuid(uuid);
    if (c != nullptr) {
      newSpeed = max(c->calculate(), newSpeed);
    }
  }

  if (newSpeed != calculatedSpeed) {
    calculatedSpeed = newSpeed;
    Serial.printf("Calculated speed: %d \n", calculatedSpeed);
  }

  delay(1000);
  // fan.setFanSpeed(calculatedSpeed);
  // fan.loop();
}
