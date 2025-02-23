#ifndef __GLOBALDATA__
#define __GLOBALDATA__

#include "Arduino.h"

#define NUMBER_OF_SETPOINTS 3

typedef struct {
    uint8_t heaterActive:1;           //* heater state
    uint8_t heaterStandby:1;          //* heater in stand by mode
    uint8_t heaterTempSensorError:1;  //* temperature sensor for heater could not be read
    uint8_t buttonState:1;
    
    uint8_t reserve:4;
} FlagFields_t;

typedef union {
    uint8_t value;
    FlagFields_t flags;
} FlagRegister_t;

typedef enum {
    HT_NONE = 0,
    HT_FMRP, //* my handle
    HT_WMRP,
    HT_WMRT
} HandleType_t;

typedef struct {
    uint16_t tipTemperature;
    uint8_t  handleTemperature;

    uint16_t     pwmWidth; // value for the timer counter compare regiter
    
    HandleType_t handleType;

    FlagFields_t flagsRegister;

    // data needed by fsm and menus
    uint16_t fsmTargetTemperature;

    //rotary encoder
    int8_t counterNew;
    int8_t counterOld;
    
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


extern GlobalData_t globals;

#endif