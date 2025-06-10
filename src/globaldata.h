#ifndef __GLOBALDATA__
#define __GLOBALDATA__

#include "Arduino.h"

#include "pins.h"
#include "handleconfig.h"

#define MAX_HANDLES 2

#define NUMBER_OF_SETPOINTS 3

typedef struct {
    uint8_t heaterActive:1;           //* heater state
    uint8_t heaterStandby:1;          //* heater in stand by mode
    uint8_t heaterTempSensorError:1;  //* temperature sensor for heater could not be read
    
    uint8_t reserve:5;
} HandleFlagFields_t;

typedef union {
    uint8_t value;
    HandleFlagFields_t flags;
} HandleFlagRegister_t;

typedef struct {
    uint8_t heaterPin;
    uint8_t tipTcPin;
    uint8_t ptcPin;
    uint8_t idPin; //"in stand" and handle id
  
} HandleInterfaceData_t;

typedef struct {
    uint16_t tipTemperature;
    uint8_t  handleTemperature;

    uint16_t     pwmWidth; // value for the timer counter compare regiter
    
    HandleType_t handleType;

    HandleFlagFields_t flagsRegister;

    // data needed by fsm and menus
    uint16_t fsmTargetTemperature;

   
} HandleVolatileData_t;

typedef struct {
    // tip data
    uint16_t targetTemperature;


    // PID parameters
    double Kp;
    double Ki;
    double Kd;
} HandlePersistentData_t;

typedef struct {
    //handle related data
    HandleInterfaceData_t  handleInterface[MAX_HANDLES];
    HandleVolatileData_t   handleActuals[MAX_HANDLES];
    HandlePersistentData_t handlePersistent[MAX_HANDLES];

    uint16_t               setpoints[NUMBER_OF_SETPOINTS];

    //rotary encoder
    int8_t  counterNew;
    int8_t  counterOld;
    uint8_t buttonState:1;

} GlobalData_t;


extern GlobalData_t globals;

#endif