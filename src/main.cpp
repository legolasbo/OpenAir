#include <Arduino.h>

#include "constants.h"
#include "ModeSelect.h"
#include "SHT20Reader.h"
#include "speedCalculators/ModeSelectSpeedCalculator.h"

ModeSelectSwitch modeSelect(S2_SDA, S2_SCL);
SHT20Reader sht20(S1_SDA, S1_SCL);

SpeedCalculator calculators[1];

void setup() {
  calculators[0] = ModeSelectSpeedCalculator(&modeSelect, 50, 100);
  Serial.begin(9600);
  Serial.println("Booting...");
}

void loop() {
  for (int i = 0; i < sizeof(calculators); i++) {
    calculators[i].print();
  }

  Serial.println(modeSelect.read());
  SHT20Reading reading = sht20.takeReading();

  Serial.print("Temperature: ");
  Serial.println(reading.temperatureDegC);
  Serial.print("Humidity: ");
  Serial.print(reading.humidity);
  Serial.println("%");
  Serial.print("Pressure: ");
  Serial.println(reading.pressure);
  Serial.print("Dew point: ");
  Serial.println(reading.dewPointDegC);

  delay(1000);   
}
