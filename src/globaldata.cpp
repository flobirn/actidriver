#include "globaldata.h"

GlobalData_t globals = {
    .handleConfig = {
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