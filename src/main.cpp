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

Web webInterface;

void setup() {
  Serial.begin(115200);
  Serial.println("Booting...");
  Log.begin(LOG_LEVEL_TRACE, &Serial);
  Log.setShowLevel(true);


  DI::GetContainer()->resolve<Configuration>()->load();
  webInterface.begin();

  Log.traceln("Config:");
  if (Log.getLevel() == LOG_LEVEL_TRACE) {
    serializeJsonPretty(DI::GetContainer()->resolve<Configuration>()->toJson(), Serial);
    Serial.println();
  }

  // tachometer.begin();
  // fan.begin();
}

int calculatedSpeed = 0;
void loop() {
  webInterface.loop();

  int newSpeed = 0;
  auto repo = DI::GetContainer()->resolve<CalculatorRepository>();

  for (std::string uuid : repo->getUuids()) {
    auto instance = repo->getInstance(uuid);
    if (!instance || !instance->isValid()) {
      continue;
    }

    newSpeed = max(instance->calculate(), newSpeed);
  }

  if (newSpeed != calculatedSpeed) {
    calculatedSpeed = newSpeed;
    Log.traceln("New speed: %d \n", calculatedSpeed);
  }

  delay(1000);
  // fan.setFanSpeed(calculatedSpeed);
  // fan.loop();
}
