#include <Arduino.h>
#include <vector>

#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ESPAsyncWiFiManager.h>

#include "constants.h"
#include "ModeSelect.h"
#include "SHT20Reader.h"
#include "speedCalculators/ModeSelectSpeedCalculator.h"
#include "speedCalculators/SHT20SpeedCalculator.h"
#include "inputs/tachometer.h"
#include "outputs/fan.h"

AsyncWebServer server(80);
DNSServer dns;

TwoWire *bus1 = &Wire;
TwoWire *bus2 = &Wire1;
Tachometer tachometer(TACHOMETER);
Fan fan(PWM_MOTOR_SPEED, tachometer);

std::vector<SpeedCalculator *> calculators;

void setup() {
  pinMode(MCU_PROGRAM, INPUT_PULLUP);

  Serial.begin(9600);
  Serial.println("Booting...");

  AsyncWiFiManager wifiManager(&server,&dns);
  if (digitalRead(MCU_PROGRAM) == LOW) {
    Serial.println("Resetting WiFi Settings");
    wifiManager.resetSettings();
  }
  wifiManager.autoConnect(HOSTNAME, AP_PASSWORD);

  Serial.println("Wifi connected");
  bus1->begin(S2_SDA, S2_SCL);
  bus2->begin(S1_SDA, S1_SCL);

  calculators.push_back(new ModeSelectSpeedCalculator(new  ModeSelectSwitch(bus1), 50, 100));
  calculators.push_back(new SHT20SpeedCalculator(new SHT20Reader(bus2)));

  tachometer.begin();
  fan.begin();
}

void loop() {
  fan.loop();

  int calculatedSpeed = 0;
  for (auto&& c : calculators) {
    calculatedSpeed = max(c->calculate(), calculatedSpeed);
  }

  Serial.printf("Calculated speed: %d \n", calculatedSpeed);
  fan.setFanSpeed(calculatedSpeed);
}
