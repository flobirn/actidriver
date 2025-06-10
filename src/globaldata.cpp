#include "globaldata.h"
#include <avr/pgmspace.h>

GlobalData_t globals = {
    .handleInterface = {
        { // left handle
            .heaterPin = HEATER_1_PIN,
            .tipTcPin  = TIP_TC_1_PIN,
            .ptcPin    = HANDLE_PTC_1_PIN,
            .idPin     = HANDLE_ID_1_PIN
        },
        { // right handle
            .heaterPin = HEATER_2_PIN,
            .tipTcPin  = TIP_TC_2_PIN,
            .ptcPin    = HANDLE_PTC_2_PIN,
            .idPin     = HANDLE_ID_2_PIN
        }
    },
    .handleActuals = {
        { 0 }, // handle 1
        { 0 }  // handle 2
    },
    .handlePersistent = { 
        { //handle 1
            .targetTemperature = 320,
            .Kp = 0.1,
            .Ki = 0.1,
            .Kd = 0.1,
        },
        { // handle 2
            .targetTemperature = 320,
            .Kp = 0.1,
            .Ki = 0.1,
            .Kd = 0.1,
        }
    },
    .setpoints = {250, 300, 350},
    .counterNew = 0,
    .counterOld = 0,
    .buttonState = 0,
    
};

HandleConfig_t knownHandles[1] = {
    {//my adapation of the Weller handle
    .type = HT_FMRP,
    .idVoltage = 7984, //2,048V /2
    .maxPWMPercent = 128, //for a 8-bit PWM; 0.5*12[V]^2/2[Ohm] ~ 36W
    .coldJointRefVoltages     = {7984, 8064, 8144, 8224, 8304,},
    .coldJointRefTemperatures = { 25,   30,   35,   40,   45  },
    .hotJointRefVoltages      = {5824, 7472, 9136, 10816, 12480},
    .hotJointRefTemperatures  = { 200,  250,  300,   350,   400}
    }
};