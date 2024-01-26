#include <Arduino.h>
#include <vector>

#include "constants.h"
#include "sensors/SensorFactory.h"
#include "speedCalculators/ModeSelectSpeedCalculator.h"
#include "speedCalculators/SHT20SpeedCalculator.h"
#include "inputs/tachometer.h"
#include "outputs/fan.h"
#include "interface/web.h"

TwoWire *bus1 = &Wire;
TwoWire *bus2 = &Wire1;
Tachometer tachometer(TACHOMETER);
Fan fan(PWM_MOTOR_SPEED, tachometer);
SensorFactory sensorFactory;

std::vector<SpeedCalculator *> calculators;

void setup() {
  Serial.begin(115200);
  Serial.println("Booting...");
  
  startInterface();

  bus1->begin(S2_SDA, S2_SCL);
  bus2->begin(S1_SDA, S1_SCL);

  calculators.push_back(new ThreePositionCalculator(sensorFactory.createI2CSensor(ThreePositionSwitchSensor, bus1)));
  calculators.push_back(new SHT20SpeedCalculator(sensorFactory.createI2CSensor(SHT20Sensor, bus2)));

  // calculators.push_back(new ThreePositionCalculator(new  ThreePositionSwitch(bus1), 50, 100));
  // calculators.push_back(new SHT20SpeedCalculator(new SHT20Reader(bus2)));

  tachometer.begin();
  fan.begin();
}

void loop() {
  loopInterface();
  fan.loop();

  int calculatedSpeed = 0;
  for (auto&& c : calculators) {
    calculatedSpeed = max(c->calculate(), calculatedSpeed);
  }

  Serial.printf("Calculated speed: %d \n", calculatedSpeed);
  fan.setFanSpeed(calculatedSpeed);
}
