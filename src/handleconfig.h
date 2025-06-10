#ifndef __HANDLECONFIG__
#define __HANDLECONFIG__

#include "Arduino.h"

#define HANDLE_NAME_LENGTH 3
#define VT_SIZE 5

typedef enum {
    HT_NONE = 0,
    HT_FMRP, //* my handle
    HT_USB
} HandleType_t;

typedef enum {
    FH_NOTFOUND = -2,
    FH_INSTAND  = -1,
} FindHandle_t;

typedef struct {
    uint16_t adcValue;
    uint8_t  temperature; //multiples of 5 degrees
} VT_Mapping_t;

typedef struct {
    double Kp;
    double Ki;
    double Kd;
} PIDParams_t;

typedef struct {
    uint8_t      maxPower; //in Watts
    double       resistance; // in Ohms
    VT_Mapping_t vtMapping[VT_SIZE];
    uint8_t      maxTemperature;  //* 5 degrees C
    uint8_t      idleTemperature; //* 5 degrees C
    PIDParams_t  slowPidParam;
    PIDParams_t  fastPidParam;
} Heater_t;

typedef struct {
    HandleType_t type;
    uint8_t      name[HANDLE_NAME_LENGTH];
    uint16_t     id;
    VT_Mapping_t vtMapping[VT_SIZE];
    uint8_t      inStandTimeout;
    uint8_t      standbyTimeout;
    uint8_t      sleepTimeout;
    Heater_t     heater;
} HandleConfig_t;



#endif