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
  auto container = DI::GetContainer();


  #if DEVELOPMENT_MODE
    Log.begin(LOG_LEVEL_TRACE, &Serial);
    container->registerInstance<Tachometer>(std::make_shared<MockTachometer>());
    container->registerInstance<Device>(std::make_shared<Device>("openair-dev"));
  #else
    Log.begin(LOG_LEVEL_INFO, &Serial);
    container->registerInstance<Tachometer>(std::make_shared<FanTachometer>());
  #endif
  Log.setShowLevel(true);

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
  std::shared_ptr<SpeedCalculator> governor = nullptr;
  auto repo = container->resolve<CalculatorRepository>();

  for (std::string uuid : repo->getUuids()) {
    auto instance = repo->getInstance(uuid);
    if (!instance || !instance->isValid()) {
      continue;
    }

    int calculated = instance->calculate();
    if (calculated > newSpeed) {
      newSpeed = calculated;
      governor = instance;
    }
  }

  auto fan = container->resolve<Fan>();
  fan->setAutoFanSpeed(newSpeed);
  if (governor) {
    fan->setGovernorName(governor->getOption("name")->toStr());
  }

  delay(1000);
}
