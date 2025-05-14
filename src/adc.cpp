#include "adc.h"
#include "debug.h"

#define DEBUG_LOG_ON

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

#define ADC_1V1_VALUE ((1100 * SAMPLES / REFERENCE_VOLTAGE_VALUE) * 1024) // adc value for the 1.1V internal reference * 16

uint32_t adc_offset = 0;
int32_t adc_1V1_error = 0;

/* REFS[1:0]	Voltage Reference Selection
00	AREF, Internal Vref turned OFF
01	AVCC with external capacitor at AREF pin
10	Reserved
11	Internal 1.1V voltage reference with external capacitor at AREF pin */
#define SELECTED_REFERENCE_VOLTAGE 0

/*
ADLAR ADC Left Adjust Result
0 - right adjust
1 - left adjust
*/
#define ADC_ADJUST 0

/*
MUX[3:0]	Single Ended Input
0000 - 0111:    ADC0 - ADC7
1000	        Temperature sensor
1001 - 1101 	Reserved
1110        	1.1V (VBG)
1111	        0V (GND)
*/

/*
ADPS[2:0]	Division Factor
000	2
001	2
010	4
011	8
100	16
101	32
110	64
111	128
*/
#define ADC_PRESCALER    5 //16Mhz/32 -> 500kHz ~ 2us -> first conversion ~ 26us

void setupAdc() {
    //ADMUX register: set reference voltage source, left/right adjustment
    ADMUX = ((SELECTED_REFERENCE_VOLTAGE) << 6) + ((ADC_ADJUST & 0x01) << 5) ; //
     // ADCSRA register: enable auto-trigger, prescaller
    ADCSRA = _BV(ADATE) + ADC_PRESCALER;
    // 	ADCSRB register: disable AC, free running mode
    ADCSRB = 0;
    //allow the voltage reference switch to stabilize
    delay(10);
}

void calibrateAdc() {
    int32_t temp = 0;
    adc_offset = 0;
    adc_1V1_error = 0;

    readAdc(&temp, Int_0V_channel);
    adc_offset = temp;
    dbgVariable("ADC offset error: ", adc_offset)

    readAdc(&temp, Int_1V1_channel);
    if (temp == 0) return;
    adc_1V1_error = (temp - ADC_1V1_VALUE) / temp;
    dbgVariable("ADC value for 1.1V: ", temp);
    dbgVariable("ADC error at 1.1V: ", adc_1V1_error);

    readAdc(&temp, Int_0V_channel);
    dbgVariable("ADC value for 0V after calibration: ", temp);

    readAdc(&temp, Int_1V1_channel);
    dbgVariable("ADC value for 1.1V after calibration: ", temp);
}

void readAdc(int32_t* adcValue, Adc_Channel_t channel)
{
    (*adcValue) = 0;

    //ADMUX register
    ADMUX = ((SELECTED_REFERENCE_VOLTAGE) << 6) + ((ADC_ADJUST & 0x01) << 5) + channel; //
 
    // ADCSRA register: enable ADC, enable auto-trigger, prescaller
    ADCSRA = _BV(ADEN)  + _BV(ADATE) + ADC_PRESCALER;

    // 	ADCSRB register: disable AC, free running mode
    ADCSRB = 0;

    // wait for sample & hold capacitor to update
	delay(1);

    //start first conversion
    int8_t count = SAMPLES - 1;
    sbi(ADCSRA, ADSC);

    //free running to get SAMPLES values
    while (count > 0) {
        if ((ADCSRA & _BV(ADIF)) != 0) {
            //start new conversion
            sbi(ADCSRA, ADIF);
            (*adcValue) += ADC;
            count--;
        }
    }

    //apply calibration results
    (*adcValue) -= adc_offset;
    (*adcValue) -= (*adcValue) * adc_1V1_error;

    // stop ADC
    cbi(ADCSRA, ADEN);
}

#ifdef DEBUG_LOG_ON
#undef DEBUG_LOG_ON
#endif
