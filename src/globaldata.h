#ifndef __GLOBALDATA__
#define __GLOBALDATA__

#include "Arduino.h"

#define NUMBER_OF_SETPOINTS 3

typedef struct {
    uint8_t heaterActive:1; //* heater state
    uint8_t dummy:7;
} FlagFields_t;

typedef union {
    uint8_t value;
    FlagFields_t flags;
} FlagRegister_t;

typedef struct {
    uint16_t tipTemperature;
    uint8_t  handleTemperature;

    uint16_t pwmWidth; // value for the timer counter compare regiter

    FlagFields_t flagsRegister;
} VolatileData_t;

typedef struct {
    // tip data
    uint16_t targetTemperature;
    uint16_t setpoints[NUMBER_OF_SETPOINTS];


    // PID parameters
    double Kp;
    double Ki;
    double Kd;
} PersistentData_t;

typedef struct {
    VolatileData_t   actual;
    PersistentData_t persistent;
} GlobalData_t;

#endif