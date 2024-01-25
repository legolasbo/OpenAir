#include <Arduino.h>
#include <vector>

#include "constants.h"
#include "ModeSelect.h"
#include "SHT20Reader.h"
#include "speedCalculators/ModeSelectSpeedCalculator.h"
#include "speedCalculators/SHT20SpeedCalculator.h"
#include "inputs/tachometer.h"
#include "outputs/fan.h"

TwoWire *bus1 = &Wire;
TwoWire *bus2 = &Wire1;
Tachometer tachometer(TACHOMETER);
Fan fan(PWM_MOTOR_SPEED, tachometer);

std::vector<SpeedCalculator *> calculators;

int minimumSpeed = 10;
int currentSpeed = 0;
int maximumSpeed = 100;

void setup() {
  Serial.begin(9600);
  Serial.println("Booting...");

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
