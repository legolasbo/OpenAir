#include <Arduino.h>
#include <ArduinoLog.h>
#include <vector>

#include "DependencyInjectionContainer.hpp"
#include "constants.h"
#include "configuration/Configuration.h"
#include "repositories/CalculatorRepository.hpp"
#include "repositories/SensorRepository.hpp"
#include "inputs/tachometer.h"
#include "outputs/fan.h"
#include "interface/web.h"

Tachometer tachometer(TACHOMETER);
Fan fan(PWM_MOTOR_SPEED, tachometer);
std::shared_ptr<Configuration> config;
CalculatorRepository calculatorFactory;

Web webInterface;

void setup() {
  Serial.begin(115200);
  Serial.println("Booting...");
  Log.begin(LOG_LEVEL_TRACE, &Serial);
  Log.setShowLevel(true);

  auto container = DI::GetContainer();

  config = Configuration::fromFile("/config.json");
  container->registerInstance<Configuration>(config);
  container->registerInstance<SensorConfigurations>(config->getSensors());
  container->registerInstance<CalculatorConfigurations>(config->getCalculators());

  webInterface.begin();

#if DEVELOPMENT_MODE == true
  SensorConfiguration * defaultSensor = new SensorConfiguration(SHT20Sensor);
  if (config->getSensors()->getUuids().size() == 0) {
    defaultSensor->setOption("name", "Default sensor");
    defaultSensor->setOption("connector", X4);
    defaultSensor->setOption("connection", I2C);
    config->getSensors()->add(defaultSensor);
    Serial.printf("Added default sensor: %s\n", defaultSensor->getUuid().c_str());
  }

  CalculatorConfiguration * defaultCalculator = new HumidityCalculatorConfiguration();
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
    DI::GetContainer()->resolve<SensorRepository>().get()->destroyInstances();
    calculatorFactory.destroyInstances();

    config->markClean();
    Log.traceln("Config updated");
  }

  int newSpeed = 0;
  for (auto uuid : config->getCalculators()->getUuids()) {
    if (!config->getCalculators()->get(uuid)->isValid()) {
      continue;
    }

    std::shared_ptr<SpeedCalculator> c = calculatorFactory.fromUuid(uuid);
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
