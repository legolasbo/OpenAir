#include <Arduino.h>
#include <vector>

#include "constants.h"
#include "ModeSelect.h"
#include "SHT20Reader.h"
#include "speedCalculators/ModeSelectSpeedCalculator.h"
#include "speedCalculators/SHT20SpeedCalculator.h"

TwoWire *bus1 = &Wire;
TwoWire *bus2 = &Wire1;

std::vector<SpeedCalculator *> calculators;

void setup() {
  Serial.begin(9600);
  Serial.println("Booting...");

  bus1->begin(S2_SDA, S2_SCL);
  bus2->begin(S1_SDA, S1_SCL);

  calculators.push_back(new ModeSelectSpeedCalculator(new  ModeSelectSwitch(bus1), 50, 100));
  calculators.push_back(new SHT20SpeedCalculator(new SHT20Reader(bus2)));
}

void loop() {
  int calculatedSpeed = 0;
  for (auto&& c : calculators) {
    int speed = c->calculate();
    Serial.printf("Speed: %d\n", speed);
    if (speed > calculatedSpeed) {
      calculatedSpeed = speed;
    }
  }

  Serial.printf("Calculated speed: %d \n", calculatedSpeed);

  delay(5000);   
}
