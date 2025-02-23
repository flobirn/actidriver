#include "globaldata.h"

GlobalData_t globals = {
    .actual = { 0 },
    .persistent = {
        .targetTemperature = 320,
        .setpoints = {250, 300, 350},
        .Kp = 0.1,
        .Ki = 0.1,
        .Kd = 0.1,
    }
    
};