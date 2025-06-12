#ifndef __ADC_H__
#define __ADC_H__

#include "Arduino.h"

#define SAMPLES 16
#define REFERENCE_VOLTAGE_VALUE 2048 // in mV

typedef enum {
    A0_channel = 0,
    A1_channel,
    A2_channel,
    A3_channel,
    A4_channel,
    A5_channel,
    A6_channel,
    A7_channel,
    Int_Temp_channel = 8,
    Int_1V1_channel  = 14,
    Int_0V_channel   = 15,
} Adc_Channel_t;

void setupAdc();
void calibrateAdc();
void readAdc(uint16_t* adcValue, uint8_t channel);
#endif