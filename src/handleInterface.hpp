#ifndef __HANDLEINTERFACE__
#define __HANDLEINTERFACE__

#include "Arduino.h"
#include <PID_v1.h>
#include "handleconfig.h"

typedef enum {
    HS_NO_HANDLE = 0,
    HS_ACTIVE,
    HS_IN_STAND,
    HS_STANDBY,
    HS_SLEEP
} HandleInterfaceStates_t;

typedef struct {
    uint8_t heaterOvertemp:1;
    uint8_t handleOvertemp:1;

} HandleInterfaceFlags_t;

typedef struct {
    uint8_t idPin;
    // cppcheck-suppress[unusedStructMember] false positive
    uint8_t temperaturePin;
    // cppcheck-suppress[unusedStructMember] false positive
    uint8_t heaterTemperaturePin;
    // cppcheck-suppress[unusedStructMember] false positive
    uint8_t heaterPin;
} HandleInterfacePins_t;

class HandleInterface {
    public:

    /** currently connected handle */
    uint8_t handle;
    
    /**current state */
    HandleInterfaceStates_t state;

    /** state flags */
    HandleInterfaceFlags_t flags;

    /** current handle temperature */
    uint16_t handleTemperature;

    /** current heater temperature */
    uint16_t heaterTemperature;

    uint8_t pwmDutyCycle; /**< PWM duty cycle: 0...255 */

    uint16_t targetTemperature;

    HandleConfig_t config;
    /** read inputs from handle: id, temperatures; detect handle and handle status */
    void update();

    /** heater is on standby */
    inline bool inStandby() { return (state == HS_STANDBY); };

    uint8_t getHeaterTemperature(uint16_t adcValue);
    uint8_t getHandleTemperature(uint16_t adcValue);

    /** constructor */
    HandleInterface(int configEepromAddr, uint8_t handleIndex);

    private:
    /** pins */
    HandleInterfacePins_t pins;

    /** timestamp last id read (milliseconds) */
    uint32_t idTimestamp;
    /** timestamp last significant heating */
    uint32_t heatingTimestamp;

    // cppcheck-suppress[unusedStructMember] false positive
    double pidOutput;
    // cppcheck-suppress[unusedStructMember] false positive
    double pidInput;
    // cppcheck-suppress[unusedStructMember] false positive
    double pidSetpoint;

    uint8_t pidUpperLimit;

    /** PID controller */
    PID pid;


    /* state handling */
    void activate_heating();
    void disconnect();
    void inStand();
    void standby();
    void sleep();

   
};

#endif