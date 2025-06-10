#ifndef __HANDLEINTERFACE__
#define __HANDLEINTERFACE__

#include "Arduino.h"
#include <PID_v1.h>

typedef enum {
    HS_NO_HANDLE = 0,
    HS_ACTIVE,
    HS_IN_STAND,
    HS_STANDBY,
    HS_SLEEP
} HandleInterfaceStates_t;

typedef struct {
    uint8_t idPin;
    uint8_t temperaturePin;
    uint8_t heaterTemperaturePin;
    uint8_t heaterPin;
} HandleInterfacePins_t;

class HandleInterface {
    public:

    private:
    /** pins */
    HandleInterfacePins_t pins;

    /** currently connected handle */
    uint8_t handle;

    /**current state */
    HandleInterfaceStates_t state;

    /** current handle temperature */
    uint16_t handleTemperature;

    /** current heater temperature */
    uint16_t heaterTemperature;

    /** PWM duty cycle */
    uint8_t pwmDutyCycle;

    /** PID controller */
    PID pid;

    /** timestamp last id read (milliseconds) */
    uint32_t idMillis;
    
};

#endif