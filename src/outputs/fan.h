#pragma once

#include <Arduino.h>
#include "../inputs/tachometer.h"

const int maximumMinimumSpeed = 25;

enum FanMode {AUTO, MANUAL};

class Fan {
    private:
        int min = 10;
        int max = 100;
        int cur = 50;
        int pin, startSpeed;
        std::shared_ptr<Tachometer> tach;
        int numStalls = 0;
        FanMode mode;
        std::string governorName = "";


        void calibrate() {
            Log.infoln("Calibrating fan....");
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
                    if (this->min >= maximumMinimumSpeed) {
                        Log.warningln("Fan does not appear to start. Unable to calibrate");
                        return;
                    }
                    calibrate();
                }
            }
            Log.infoln("Calibration completed. minimum speed: %d", this->min);
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
            Log.traceln("Starting fan");
            this->setFanSpeed(startSpeed);
            delay(5000);
        }

        void increaseMinimumSpeed() {
            if (this->min == maximumMinimumSpeed) {
                return;
            }
            Log.traceln("Increasing minimum speed");
            this->min = constrain(this->min+1, 10, maximumMinimumSpeed);
        }

        void setFanSpeed(int speed) {
            int speedToSet = constrain(speed, this->min, this->max);
            if (this->cur == speedToSet)
                return;

            this->cur = speedToSet;
            float perc = float(this->cur) / 100;
            int newDuty = perc * 255;
            ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, newDuty);
            ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
            Log.traceln("Set speed to %d", this->cur);
        }

    public:
        Fan(int pin = PWM_MOTOR_SPEED, int startSpeed = maximumMinimumSpeed) {
            this->pin = pin;
            this->startSpeed = constrain(startSpeed, maximumMinimumSpeed, 100);
            this->tach = DI::GetContainer()->resolve<Tachometer>();
            this->mode = AUTO;
        }

        FanMode getMode() {
            return this->mode;
        }

        void begin() {
            this->configurePWM();
            this->calibrate();
        }

        void loop() {
            this->tach->loop();
            if (this->tach->RPM() == 0) {
                this->numStalls++;
                Log.warningln("Stall detected: restarting fan");
                this->increaseMinimumSpeed();
                this->startFan();
                delay(2000);
            }
        }

        int stallCount() {
            return this->numStalls;
        }

        void setManualFanSpeed(int speed) {
            this->mode = MANUAL;
            this->setFanSpeed(speed);
        }

        void resumeAuto() {
            this->mode = AUTO;
        }

        void setAutoFanSpeed(int speed) {
            if (this->mode == AUTO) {
                setFanSpeed(speed);
            }
        }

        void setGovernorName(std::string name) {
            this->governorName = name;
        }

        std::string governor() {
            if (this->mode == AUTO) {
                return this->governorName;
            }
            return "Manual";
        }

        int minimumSpeed() {return this->min;}
        int currentSpeed() {return this->cur;}
        int maximumSpeed() {return this->max;}
};
