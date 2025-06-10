#include "handleconfig.h"
#include <EEPROM.h>

#define ADC_ID_INSTAND 1000
#define MAX_ADC_ID_DIFFERENCE 100

static uint16_t readEepromWord(int location);

HandleConfig_t EEMEM Handles[] = {
    {
        .type = HT_FMRP,
        .name = {'F', 'H', 'T'},
        .id = 8192,
        .vtMapping = {
            {.adcValue = 8184, .temperature = 5}, //25C
            {.adcValue = 8349, .temperature = 7}, //35C
            {.adcValue = 8429, .temperature = 8}, //40C
            {.adcValue = 8507, .temperature = 9}, //45C
            {.adcValue = 8659, .temperature = 10}, //50C
        },
        .inStandTimeout = 30, //seconds
        .standbyTimeout = 120, //seconds
        .sleepTimeout   =  10, //minutes
        .heater = {
            .maxPower = 40,
            .resistance = 2.1,
            .vtMapping =  {
                {.adcValue = 1369, .temperature = 20}, //100C
                {.adcValue = 2928, .temperature = 40}, //200C
                {.adcValue = 4589, .temperature = 60}, //300C
                {.adcValue = 6265, .temperature = 80}, //400C
                {.adcValue = 7867, .temperature = 100}, //500C
            },
            .maxTemperature = 100, //500C
            .idleTemperature = 30, //150C
            .slowPidParam = {.Kp = 10.0, .Ki = 10.0, .Kd = 10.0},
            .fastPidParam = {.Kp = 10.0, .Ki = 10.0, .Kd = 10.0},
        },
    },
};

uint8_t EEMEM handleCount = sizeof (Handles) / sizeof (HandleConfig_t);

int8_t findHandle(uint16_t adcValue) {
    if (adcValue < ADC_ID_INSTAND)
        return FH_INSTAND;
    
    for (uint8_t i = 0; i < handleCount; i++) {
        uint16_t id = readEepromWord(Handles[i].id);
        if (abs(id - adcValue) < MAX_ADC_ID_DIFFERENCE)
            return i;
    }

    return FH_NOTFOUND;
}

static uint16_t readEepromWord(int location){
  uint16_t val;

  val = (EEPROM.read(location + 1) << 8);
  val |= EEPROM.read(location);
  return val;
}

uint8_t getTemperature(uint8_t handle, uint16_t adcValue) {
    if (handle < handleCount) {
        uint16_t refferenceAdc;
        uint16_t previousRefAdc;
        uint8_t i;

        for (i = 0; i < VT_SIZE; i++) {
            refferenceAdc = readEepromWord(Handles[handle].vtMapping[i].adcValue);
            if (adcValue < refferenceAdc) break;
            previousRefAdc = refferenceAdc; //store as previous value
        }

        if (i == 0) {
            /* smaller than the smallest refference value:
            assume a straight line fom (0,0) to (first adc value, first temperature)
            */
            double temperature = (EEPROM.read(Handles[handle].vtMapping[0].temperature) / refferenceAdc) * adcValue;
            return (uint8_t) temperature;
            
        }
        if (i < VT_SIZE) {
            uint8_t referenceTemperature = EEPROM.read(Handles[handle].vtMapping[i-1].temperature);
            double slope = (EEPROM.read(Handles[handle].vtMapping[i].temperature) - referenceTemperature) / (refferenceAdc - previousRefAdc);
            return (uint8_t) ((slope*(adcValue-previousRefAdc)) + referenceTemperature);
        }

        /* greater than the biggest adcValue in vtMapping:
           take slope from the last vtMapping values and extend it up to the curent adcValue
        */
        uint8_t referenceTemperature = EEPROM.read(Handles[handle].vtMapping[VT_SIZE-1].temperature);
        double slope = (referenceTemperature - EEPROM.read(Handles[handle].vtMapping[VT_SIZE-2].temperature)) / (refferenceAdc - previousRefAdc);
        return (uint8_t) ((slope*(adcValue-refferenceAdc)) + referenceTemperature);
    }
    return 0xFF; // 1275 degrees C
}

uint8_t getHeaterTemperature(uint8_t handle, uint16_t adcValue) {
    if (handle < handleCount) {
        uint16_t refferenceAdc;
        uint16_t previousRefAdc;
        uint8_t i;

        for (i = 0; i < VT_SIZE; i++) {
            refferenceAdc = readEepromWord(Handles[handle].heater.vtMapping[i].adcValue);
            if (adcValue < refferenceAdc) break;
            previousRefAdc = refferenceAdc; //store as previous value
        }

        if (i == 0) {
            /* smaller than the smallest refference value:
            assume a straight line fom (0,0) to (first adc value, first temperature)
            */
            double temperature = (EEPROM.read(Handles[handle].heater.vtMapping[0].temperature) / refferenceAdc) * adcValue;
            return (uint8_t) temperature;
            
        }
        if (i < VT_SIZE) {
            uint8_t referenceTemperature = EEPROM.read(Handles[handle].heater.vtMapping[i-1].temperature);
            double slope = (EEPROM.read(Handles[handle].heater.vtMapping[i].temperature) - referenceTemperature) / (refferenceAdc - previousRefAdc);
            return (uint8_t) ((slope*(adcValue-previousRefAdc)) + referenceTemperature);
        }

        /* greater than the biggest adcValue in vtMapping:
           take slope from the last vtMapping values and extend it up to the curent adcValue
        */
        uint8_t referenceTemperature = EEPROM.read(Handles[handle].heater.vtMapping[VT_SIZE-1].temperature);
        double slope = (referenceTemperature - EEPROM.read(Handles[handle].heater.vtMapping[VT_SIZE-2].temperature)) / (refferenceAdc - previousRefAdc);
        return (uint8_t) ((slope*(adcValue-refferenceAdc)) + referenceTemperature);
    }
    return 0xFF; // 1275 degrees C
}

