#ifndef __GLOBALDATA__
#define __GLOBALDATA__

#include "Arduino.h"

#include "pins.h"
#include "handleconfig.h"

#define MAX_HANDLES 2

#define NUMBER_OF_SETPOINTS 3

typedef struct {
    // data needed by fsm and menus
    uint16_t fsmTargetTemperature; 
} HandleVolatileData_t;

typedef struct {
    //handle related data
    HandleVolatileData_t   handleActuals[MAX_HANDLES];

    uint16_t               setpoints[NUMBER_OF_SETPOINTS];

    //rotary encoder
    int8_t  counterNew;
    int8_t  counterOld;
    uint8_t buttonState:1;

} GlobalData_t;


extern GlobalData_t globals;

#endif