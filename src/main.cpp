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

TaskHandle_t mqttTask;

void mqttTaskFunc(void *parameter) {
  auto mqtt = DI::GetContainer()->resolve<MQTT>();
  while(true) {
    vTaskDelay(mqtt->runTask());
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("Booting...");
  Log.begin(LOG_LEVEL_TRACE, &Serial);
  Log.setShowLevel(true);

  DI::GetContainer()->resolve<Configuration>()->load();
  DI::GetContainer()->resolve<Web>()->begin();

  Log.traceln("Config:");
  if (Log.getLevel() == LOG_LEVEL_TRACE) {
    serializeJsonPretty(DI::GetContainer()->resolve<Configuration>()->toJson(), Serial);
    Serial.println();
  }


  xTaskCreatePinnedToCore(
    mqttTaskFunc, /* Function to implement the task */
    "MqttTask",   /* Name of the task */
    10000,        /* Stack size in words */
    NULL,         /* Task input parameter */
    // 1,  /* Priority of the task */
    configMAX_PRIORITIES - 2,
    &mqttTask, /* Task handle. */
    0);        /* Core where the task should run */


  // tachometer.begin();
  // fan.begin();
}

int calculatedSpeed = 0;
void loop() {
  DI::GetContainer()->resolve<Web>()->loop();
  DI::GetContainer()->resolve<SensorRepository>()->loop();

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
