#include <Arduino.h>
#include "../inputs/tachometer.h"

const int maximumMinimumSpeed = 25;

class Fan {
    private:
        int min = 10;
        int max = 100;
        int cur = 50;
        int pin, startSpeed;
        Tachometer *tach;


        void calibrate() {
            this->startFan();
            while(this->cur > this->min) {
                int newFanspeed = this->cur / 2;
                if (newFanspeed < this->min) {
                    newFanspeed = this->cur - 1;
                }
                
                setFanSpeed(newFanspeed);
                delay(2000);
                this->tach->loop();

                if (this->tach->RPS() == 0) {
                this->min = this->cur + 1;
                calibrate();
                }
            }
        }

        void configurePWM() {
            ledc_timer_config_t ledc_timer = {
                .speed_mode = LEDC_LOW_SPEED_MODE,
                .duty_resolution = LEDC_TIMER_8_BIT,
                .timer_num = LEDC_TIMER_0,
                .freq_hz = 4000,
                .clk_cfg = LEDC_AUTO_CLK
            };
            ledc_timer_config(&ledc_timer);

            ledc_channel_config_t ledc_channel = {
                .gpio_num = this->pin,
                .speed_mode = LEDC_LOW_SPEED_MODE,
                .channel = LEDC_CHANNEL_0,
                .intr_type = LEDC_INTR_DISABLE,
                .timer_sel = LEDC_TIMER_0,
                .duty = uint32_t(this->startSpeed),
                .hpoint = 0,
                .flags = {
                    .output_invert = 1
                }
            };
            ledc_channel_config(&ledc_channel);

            
        }

        void startFan() {
            this->setFanSpeed(startSpeed);
        }

        void increaseMinimumSpeed() {
            this->min = constrain(this->min+1, 10, maximumMinimumSpeed);
        }

    public:
        Fan(int pin, Tachometer &tach, int startSpeed = 100) {
            this->pin = pin;
            this->tach = &tach;
            this->startSpeed = constrain(startSpeed, maximumMinimumSpeed, 100);
        }

        void begin() {
            this->configurePWM();
            this->calibrate();

            do {
                this->setFanSpeed(this->max);
                delay(10000);
                this->setFanSpeed(this->min);
                delay(10000);

                if (this->tach->RPS() == 0) {
                    this->increaseMinimumSpeed();
                }
            }
            while (this->min < maximumMinimumSpeed);
        }

        void loop() {
            this->tach->loop();
            if (this->tach->RPM() == 0) {
                this->increaseMinimumSpeed();
                this->startFan();
                delay(2000);
            }
        }

        void setFanSpeed(int speed) {
            int speedToSet = constrain(speed, this->min, this->max);
            this->cur = speedToSet;
            float perc = float(this->cur) / 100;
            int newDuty = perc * 255;
            ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, newDuty);
            ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
            Serial.printf("Set speed to %d\n", this->cur);
        }

        int minimumSpeed() {return this->min;}
        int currentSpeed() {return this->cur;}
        int maximumSpeed() {return this->max;}
};
