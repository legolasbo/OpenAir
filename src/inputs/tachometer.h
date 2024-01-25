#include <Arduino.h>
#if !SOC_PCNT_SUPPORTED
#error "This application requires an ESP32 with hardware pulsecounter."
#endif

#ifndef TACHOMETER_H
#define TACHOMETER_H

#include "driver/pcnt.h"
#include "driver/ledc.h"
#include <soc/pcnt_struct.h>


class Tachometer {
  private:
    int pin;
    pcnt_unit_t unit;
    ulong lastMicros = 0;
    int16_t lastMinute[60];
    int lastSecond = 0;

    ulong elapsedMicros(ulong oldMicros, ulong newMicros) {
      if (newMicros < oldMicros) {
        // we wrapped.
        ulong microsBeforeWrap = ULONG_LONG_MAX - oldMicros;
        ulong actualMicros = microsBeforeWrap + newMicros;
        return actualMicros;
      }

      return newMicros - oldMicros;
    }
  
  public:
    Tachometer(int pin, pcnt_unit_t unit = PCNT_UNIT_0) {
      this->pin = pin;
      this->unit = unit;
    }

    void begin() {
      pinMode(this->pin, INPUT_PULLDOWN);

      pcnt_config_t conf;
      conf.pulse_gpio_num = this->pin;
      conf.ctrl_gpio_num = PCNT_PIN_NOT_USED;
      conf.hctrl_mode = PCNT_MODE_KEEP;
      conf.lctrl_mode = PCNT_MODE_KEEP;
      conf.pos_mode = PCNT_COUNT_INC;
      conf.neg_mode = PCNT_COUNT_DIS;
      conf.counter_h_lim = 0;
      conf.counter_l_lim = 0;
      conf.unit = this->unit;
      conf.channel = PCNT_CHANNEL_0;
      
      esp_err_t error = pcnt_unit_config(&conf);
      if (error != ESP_OK) {
        Serial.printf("Configuring counter failed: %s", esp_err_to_name(error));
        return;
      }

      error = pcnt_set_filter_value(this->unit, 100);
      if (error != ESP_OK) {
        Serial.printf("Setting filter failed: %s", esp_err_to_name(error));
        return;
      }
      error = pcnt_filter_enable(this->unit);
      if (error != ESP_OK) {
        Serial.printf("Enabling filter failed: %s", esp_err_to_name(error));
        return;
      }

      pcnt_counter_clear(this->unit);
    }

    int16_t read() {
      int16_t count;
      esp_err_t error = pcnt_get_counter_value(this->unit, &count);
      if (error != ESP_OK) {
        Serial.printf("Failed to read tachometer on pin %d: %s", this->pin, esp_err_to_name(error));
        return 0;
      }
      return count;
    }

    bool loop() {
      if(this->elapsedMicros(this->lastMicros, micros()) < 1000000) {
        return false;
      }

      if (++this->lastSecond > 59) {
        this->lastSecond = 0;
      }

      this->lastMinute[this->lastSecond] = this->read();
      pcnt_counter_clear(this->unit);
      this->lastMicros = micros();
      return true;
    }

    int16_t RPS() {
      return this->lastMinute[this->lastSecond];
    }

    int RPM() {
      int sum = 0;
      for (int i = 0; i < 60; i++) {
        sum += this->lastMinute[i];
      }
      return sum;
    }
};

#endif