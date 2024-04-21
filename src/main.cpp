#include <Arduino.h>
#include <ArduinoLog.h>
#include <vector>

#include "constants.h"
#include "configuration/Configuration.h"
#include "factories/CalculatorFactory.hpp"
#include "factories/SensorFactory.h"
#include "inputs/tachometer.h"
#include "outputs/fan.h"
#include "interface/web.h"
#include "DependencyInjectionContainer.hpp"

Tachometer tachometer(TACHOMETER);
Fan fan(PWM_MOTOR_SPEED, tachometer);
Configuration * config;
CalculatorFactory * calculatorFactory;
DI container;

Web webInterface(container);

void setup() {
  Serial.begin(115200);
  Serial.println("Booting...");
  Log.begin(LOG_LEVEL_TRACE, &Serial);
  Log.setShowLevel(true);

  config = Configuration::fromFile(container, "/config.json");
  container.registerInstance<I2CManager>(new I2CManager());
  container.registerInstance<Configuration>(config);
  container.registerInstance<SensorConfigurations>(config->getSensors());
  container.registerInstance<CalculatorConfigurations>(config->getCalculators());
  container.registerInstance<SensorFactory>(new SensorFactory(container));
  calculatorFactory = new CalculatorFactory(container);

  webInterface.begin();

#if DEVELOPMENT_MODE == true
  SensorConfiguration * defaultSensor = new SensorConfiguration(container, SHT20Sensor);
  if (config->getSensors()->getUuids().size() == 0) {
    defaultSensor->setOption("name", "Default sensor");
    defaultSensor->setOption("connector", X4);
    defaultSensor->setOption("connection", I2C);
    config->getSensors()->add(defaultSensor);
    Serial.printf("Added default sensor: %s\n", defaultSensor->getUuid().c_str());
  }

  CalculatorConfiguration * defaultCalculator = new HumidityCalculatorConfiguration(container);
  if (config->getCalculators()->getUuids().size() == 0) {
    defaultCalculator->setOption("name", "Default calculator");
    defaultCalculator->setOption("sensor", defaultSensor->getUuid());
    config->getCalculators()->add(defaultCalculator);
    Serial.printf("Added default calculator %s\n", defaultCalculator->getUuid().c_str());
    serializeJsonPretty(defaultCalculator->toJson(), Serial);
  }
  // config->save();
#endif


  Log.traceln("Config:");
  if (Log.getLevel() == LOG_LEVEL_TRACE) {
    serializeJsonPretty(config->toJson(), Serial);
    Serial.println();
  }

  // tachometer.begin();
  // fan.begin();
}

int calculatedSpeed = 0;
void loop() {
  webInterface.loop();

  if (config->isDirty()) {
    container.resolve<SensorFactory>().get()->destroyInstances();
    calculatorFactory->destroyInstances();

    config->markClean();
    Log.traceln("Config updated");
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
    Log.traceln("New speed: %d \n", calculatedSpeed);
  }

  delay(1000);
  // fan.setFanSpeed(calculatedSpeed);
  // fan.loop();
}
