#include "globaldata.h"
#include <avr/pgmspace.h>
#include "handleInterface.hpp"
#include "eeprom_map.h"


HandleInterface leftHandle(EEPROM_HANDLE_1_ADDRESS, 0);

HandleInterface rightHandle(EEPROM_HANDLE_2_ADDRESS, 1);

HandleInterface* handleInterfaces[] = {&leftHandle, &rightHandle};

GlobalData_t globals = {
    .handleActuals = {
        { 0 }, // handle 1
        { 0 }  // handle 2
    },
    .setpoints = {250, 300, 350},
    .counterNew = 0,
    .counterOld = 0,
    .buttonState = 0,
    
};
