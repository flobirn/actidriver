#ifndef __PINS_HEADER__
#define __PINS_HEADER__

#define analogPinToChannel(pin) (pin - A0)
//rottary encoder
#define BUTTON_A_PIN 2
#define BUTTON_B_PIN 7
#define BUTTON_SW_PIN 12
//heaters
#define HEATER_1_PIN 9 //left heater
#define HEATER_2_PIN 3 //right heater
//buzzer
#define BUZZER_PIN 6
//thermocoouple sensors
#define TIP_TC_1_PIN A1
#define TIP_TC_2_PIN A0
//cold joint temperature sensors
#define HANDLE_PTC_1_PIN A2
#define HANDLE_PTC_2_PIN A3
// identification resistor dividers
#define HANDLE_ID_1_PIN A4
#define HANDLE_ID_2_PIN A5
//VCC measurement pin
#define V12_PIN A6
#endif