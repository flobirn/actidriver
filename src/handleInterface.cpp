#include "handleInterface.hpp"
#include "pins.h"
#include "handleconfig.h"
#include "adc.h"
#include "debug.h"
#include <EEPROM.h>

extern HandleConfig_t Handles[];

#define HEATER_VOLTAGE 12
/** wait time for the opamp output to be stable */
#define OPAMP_WAIT_MS 10

void HandleInterface::update() {
    /** inhibit heating */
    analogWrite(pins.heaterPin, 0);
    //wait 10ms for the analog comparators to stabilize
    delay(OPAMP_WAIT_MS);
    uint16_t handleId;
    readAdc(&handleId, analogPinToChannel(pins.idPin));
    int8_t newHandle = findHandle(handleId);

    if (handle != newHandle) idTimestamp = millis();
    switch (state) {
        case HS_NO_HANDLE:
            if (newHandle > 0) {
                this->handle = newHandle;
                activate_heating();
                state = HS_ACTIVE;
            }
            break;
        case HS_ACTIVE:
            if (newHandle == 0) {
                inStand();
            } else if (handle != newHandle) {
                disconnect();
            }
            break;
        case HS_IN_STAND:
            if (newHandle == handle) {
                activate_heating();
            } else {
                disconnect();
            }
            break;
        case HS_STANDBY:
            if (newHandle != handle) {
                disconnect();
            }
            break;
        case HS_SLEEP:
            break;
    }

    // activate back the heating
    if (state == HS_ACTIVE || state == HS_IN_STAND || state == HS_STANDBY) {
        uint16_t temp;
        readAdc(&temp, analogPinToChannel(pins.temperaturePin));
        handleTemperature = getHandleTemperature(temp);
        flags.handleOvertemp = 0;
        if (handleTemperature > config.maxTemperature) {
            flags.handleOvertemp = 1;
        }
        readAdc(&temp, analogPinToChannel(pins.heaterTemperaturePin));
        heaterTemperature = getHeaterTemperature(temp) + handleTemperature;
        flags.heaterOvertemp = 0;
        if (heaterTemperature > config.heater.maxTemperature) {
            flags.heaterOvertemp = 1;
        }
        pidInput = heaterTemperature;
        pid.Compute();
        /**todo: based on pidOutput Values, detect standby and sleep
         * probably we need a new timestamp to se set as long as output > threshold
         * threshold to be added to HandleConfig_t
         * or maybe use the same timestamp as for the id
         */
        if (pidOutput > pidUpperLimit / 10) {
            idTimestamp = millis();
        }
        if ((state == HS_ACTIVE) && (millis() - idTimestamp > config.standbyTimeout * 1000U)) {
            standby();
        }
        if ((millis() - idTimestamp) > config.sleepTimeout * 60000U) {
            sleep();
        }
        /** should still be heating? */
        if ((state == HS_ACTIVE || state == HS_IN_STAND || state == HS_STANDBY) && 
            flags.heaterOvertemp == 0) {
            analogWrite(pins.heaterPin, pidOutput);
        }
    }
}

void HandleInterface::activate_heating() {
    // todo: two short bips
    //start with pwm off
    analogWrite(pins.heaterPin, 0);
    if (targetTemperature == 0) {
        // first time in active mode?
        targetTemperature  = config.heater.startTemperature;
    }

    pid.SetTunings(config.heater.fastPidParam.Kp, config.heater.fastPidParam.Ki, config.heater.fastPidParam.Kd);
    pidUpperLimit = (config.heater.maxPower / (HEATER_VOLTAGE*HEATER_VOLTAGE/config.heater.resistance));
    pid.SetOutputLimits(0.0, pidUpperLimit);
    pid.SetSampleTime(100);
    pidSetpoint = targetTemperature;
    pid.SetControllerDirection(DIRECT);
    state = HS_ACTIVE;
    dbgText("HS_ACTIVE");
}

void HandleInterface::disconnect() {
    state = HS_NO_HANDLE;
    /** to do: 2 long bips, pwm off */
    dbgText("HS_NO_HANDLE");
}

void HandleInterface::inStand() {
    if ((millis() - idTimestamp) > Handles[handle].inStandTimeout * 1000U) {
        pidSetpoint = config.heater.idleTemperature * 5;
        state = HS_IN_STAND;
        dbgText("HS_IN_STAND");
        /** todo: 1 short bip, backup setpoint, set in stand setpoint */
    }
}

void HandleInterface::standby() {
    state = HS_STANDBY;
    /** todo: 1 short bip, backup setpoint, set in stand setpoint */
    pidSetpoint = config.heater.idleTemperature * 5U;
    dbgText("HS_STANDBY");
}

void HandleInterface::sleep() {
    state = HS_SLEEP;
    /** todo: 1 long bip, backup setpoint, set in stand setpoint */
    pidSetpoint = 0;
}

HandleInterface::HandleInterface(int configEepromAddr, uint8_t handleIndex) :
    handle(FH_NOTFOUND),
    state(HS_NO_HANDLE),
    flags({0}),
    handleTemperature(0),
    heaterTemperature(0),
    pwmDutyCycle(0),
    targetTemperature(0),
    pidOutput(0),
    pidInput(0),
    pidSetpoint(0),
    pidUpperLimit(0),
    pid(&pidInput, &pidOutput, &pidSetpoint, 0, 0, 0, DIRECT) {
    
    /* copy eeprom config values */
    for (uint8_t i = 0; i < sizeof(HandleConfig_t); i++) {
        (reinterpret_cast<uint8_t*>(&config))[i] = EEPROM.read(configEepromAddr + i);
    }

    switch (handleIndex) {
        case 0:
            /* left handle */
            pins.heaterPin = HEATER_1_PIN;
            pins.heaterTemperaturePin = TIP_TC_1_PIN;
            pins.idPin = HANDLE_ID_1_PIN;
            pins.temperaturePin = HANDLE_PTC_1_PIN;
            break;
        case 1:
            /* left handle */
            pins.heaterPin = HEATER_2_PIN;
            pins.heaterTemperaturePin = TIP_TC_2_PIN;
            pins.idPin = HANDLE_ID_2_PIN;
            pins.temperaturePin = HANDLE_PTC_2_PIN;
            break;
    }

    idTimestamp = millis();
    heatingTimestamp = millis();
    
    //pwm off
    pinMode(pins.heaterPin, INPUT);
}

uint8_t HandleInterface::getHandleTemperature(uint16_t adcValue) {

        uint16_t refferenceAdc;
        uint16_t previousRefAdc;
        uint8_t i;

        for (i = 0; i < VT_SIZE; i++) {
            refferenceAdc = config.vtMapping[i].adcValue;
            if (adcValue < refferenceAdc) break;
            previousRefAdc = refferenceAdc; //store as previous value
        }

        if (i == 0) {
            /* smaller than the smallest refference value:
            assume a straight line fom (0,0) to (first adc value, first temperature)
            */
            double temperature = (config.vtMapping[0].temperature / refferenceAdc) * adcValue;
            return (uint8_t) temperature;
            
        }
        if (i < VT_SIZE) {
            uint8_t referenceTemperature = config.vtMapping[i-1].temperature;
            double slope = (config.vtMapping[i].temperature - referenceTemperature) / (refferenceAdc - previousRefAdc);
            return (uint8_t) ((slope*(adcValue-previousRefAdc)) + referenceTemperature);
        }

        /* greater than the biggest adcValue in vtMapping:
           take slope from the last vtMapping values and extend it up to the curent adcValue
        */
        uint8_t referenceTemperature = config.vtMapping[VT_SIZE-1].temperature;
        double slope = (referenceTemperature - config.vtMapping[VT_SIZE-2].temperature) / (refferenceAdc - previousRefAdc);
        return (uint8_t) ((slope*(adcValue-refferenceAdc)) + referenceTemperature);
}

uint8_t HandleInterface::getHeaterTemperature(uint16_t adcValue) {
    uint16_t refferenceAdc;
    uint16_t previousRefAdc;
    uint8_t i;

    for (i = 0; i < VT_SIZE; i++) {
        refferenceAdc = config.heater.vtMapping[i].adcValue;
        if (adcValue < refferenceAdc) break;
        previousRefAdc = refferenceAdc; //store as previous value
    }

    if (i == 0) {
        /* smaller than the smallest refference value:
        assume a straight line fom (0,0) to (first adc value, first temperature)
        */
        double temperature = (config.heater.vtMapping[0].temperature / refferenceAdc) * adcValue;
        return (uint8_t) temperature;
        
    }
    if (i < VT_SIZE) {
        uint8_t referenceTemperature = config.heater.vtMapping[i-1].temperature;
        double slope = (config.heater.vtMapping[i].temperature - referenceTemperature) / (refferenceAdc - previousRefAdc);
        return (uint8_t) ((slope*(adcValue-previousRefAdc)) + referenceTemperature);
    }

    /* greater than the biggest adcValue in vtMapping:
        take slope from the last vtMapping values and extend it up to the curent adcValue
    */
    uint8_t referenceTemperature = config.heater.vtMapping[VT_SIZE-1].temperature;
    double slope = (referenceTemperature - config.heater.vtMapping[VT_SIZE-2].temperature) / (refferenceAdc - previousRefAdc);
    return (uint8_t) ((slope*(adcValue-refferenceAdc)) + referenceTemperature);
}
