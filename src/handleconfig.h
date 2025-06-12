#ifndef __HANDLECONFIG__
#define __HANDLECONFIG__

#include "Arduino.h"

#define HANDLE_NAME_LENGTH 3

/** maximum number of elements in the adc -> temperature mapping */
#define VT_SIZE 5

/** convert from an ADC value to temperature using heater ADC to temperature mapping
 * \param handle the index af the handle for which the temperature was read from ADC
 * \param adcValue the value read from the ADC input of the handle temperature sensor
 * \return the corresponding temperature as multiple of 5 degrees
*/
uint8_t getHeaterTemperature(uint8_t handle, uint16_t adcValue);
/** convert from an ADC value to temperature using handle ADC to temperature mapping
 * \param handle the index af the handle for which the temperature was read from ADC
 * \param adcValue the value read from the ADC input for the heater thermocouple
 * \return the corresponding temperature as multiple of 5 degrees
*/
uint8_t getTemperature(uint8_t handle, uint16_t adcValue);
/** find a handle with a specific id
 * \param adcValue the value read from ADC if the id voltage
 * \return \li `#FH_INSTAND` if adcValue is smaller than a threshold value\n
 * \li `#FH_NOTFOUND` if no handle with an id == adcValue (+/- accepted difference) was found\n
 * \li 0...VT_SIZE - 1 the index of a handle with id id == adcValue (+/- accepted difference)
 */
int8_t findHandle(uint16_t adcValue);

typedef enum {
    HT_NONE = 0, /**< dummyvalue */
    HT_FMRP,     /**< handle according to actidriver schematics */
    HT_USB       /**< handle controlled via USB */
} HandleType_t;

typedef enum {
    FH_NOTFOUND = -2,
    FH_INSTAND  = -1,
} FindHandle_t;

// cppcheck-suppress-begin[unusedStructMember] false positive
typedef struct {
    uint16_t adcValue;
    uint8_t  temperature; //multiples of 5 degrees
} VT_Mapping_t;
// cppcheck-suppress-end[unusedStructMember]

// cppcheck-suppress-begin[unusedStructMember] false positive
typedef struct {
    double Kp;
    double Ki;
    double Kd;
} PIDParams_t;
// cppcheck-suppress-end[unusedStructMember]

typedef struct {
    uint8_t      maxPower; //in Watts
    double       resistance; // in Ohms
    VT_Mapping_t vtMapping[VT_SIZE];
    uint8_t      maxTemperature;  // * 5 degrees C
    uint8_t      idleTemperature; // * 5 degrees C
    uint8_t      startTemperature; // * 5 degrees C
    PIDParams_t  slowPidParam;
    PIDParams_t  fastPidParam;
} Heater_t;

typedef struct {
    HandleType_t type;
    // cppcheck-suppress[unusedStructMember] false positive
    uint8_t      name[HANDLE_NAME_LENGTH];
    // cppcheck-suppress[unusedStructMember] false positive
    uint16_t     id;
    // cppcheck-suppress[unusedStructMember] false positive
    VT_Mapping_t vtMapping[VT_SIZE];
    // cppcheck-suppress[unusedStructMember] false positive
    uint8_t      inStandTimeout; //seconds
    // cppcheck-suppress[unusedStructMember] false positive
    uint8_t      standbyTimeout;
    // cppcheck-suppress[unusedStructMember] false positive
    uint8_t      sleepTimeout;
    uint8_t      maxTemperature;  // * 5 degrees C
    Heater_t     heater;
} HandleConfig_t;


#endif