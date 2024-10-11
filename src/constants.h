#pragma once

#define VERSION "0.0.2"

/**************************/
/****   Development    ****/
/**************************/
#define DEVELOPMENT_MODE false

/**************************/
/****      SPIFFS      ****/
/**************************/
#define CONFIGURATION_FILE_PATH "/config.json"

/**************************/
/****      PINOUT      ****/
/**************************/
#define UART1_RX 13
#define UART1_TX 12
#define UART2_RX 25
#define UART2_TX 26

#define S1_SDA 19
#define S1_SCL 18
#define S2_SDA 16
#define S2_SCL 4

#define LED_STATUS 33

#if DEVELOPMENT_MODE
    #define PWM_MOTOR_SPEED 32
    #define TACHOMETER 33
#else
    #define PWM_MOTOR_SPEED 15
    #define TACHOMETER 14
#endif

/**************************/
/****       WIFI       ****/
/**************************/
#define HOSTNAME "OpenAir"
#define AP_PASSWORD "OpenUpAir"

