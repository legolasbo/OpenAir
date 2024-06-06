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

  auto container = DI::GetContainer();

  #ifdef DEVELOPMENT_MODE
    container->registerInstance<Tachometer>(std::make_shared<MockTachometer>());
  #elif
    container->registerInstance<Tachometer>(std::make_shared<FanTachometer>());
  #endif

  container->resolve<Configuration>()->load();
  container->resolve<Web>()->begin();
  container->resolve<Tachometer>()->begin();
  container->resolve<Fan>()->begin();

  Log.traceln("Config:");
  if (Log.getLevel() == LOG_LEVEL_TRACE) {
    serializeJsonPretty(container->resolve<Configuration>()->toJson(), Serial);
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


}

int calculatedSpeed = 0;
void loop() {
  auto container = DI::GetContainer();
  container->resolve<Web>()->loop();
  container->resolve<SensorRepository>()->loop();
  container->resolve<Fan>()->loop();

  int newSpeed = 0;
  auto repo = container->resolve<CalculatorRepository>();

  for (std::string uuid : repo->getUuids()) {
    auto instance = repo->getInstance(uuid);
    if (!instance || !instance->isValid()) {
      continue;
    }

    newSpeed = max(instance->calculate(), newSpeed);
  }

  container->resolve<Fan>()->setFanSpeed(newSpeed);

  delay(1000);
}
