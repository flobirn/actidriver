#include "globaldata.h"

GlobalData_t globals = {
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