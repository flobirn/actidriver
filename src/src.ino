#include "globaldata.h"
#include "display.h"
#include "mainmenu.h"
#include "fsm.h"
#include "pins.h"


extern GlobalData_t globals;

void button_down_isr() {
  if (digitalRead(BUTTON_B_PIN)) {
    globals.actual.counterNew++;
    //fsm_handleEvent(EVT_DOWN);
  } else {
    globals.actual.counterNew--;
    //fsm_handleEvent(EVT_UP);
  }
}

void setup() {
    Serial.begin(115200); 
 
    fsm_init();
    display_setup();
    delay(1000);
    mainmenu_init();
// rotarry encoder
    pinMode(BUTTON_A_PIN, INPUT);
    pinMode(BUTTON_B_PIN, INPUT);
    pinMode(BUTTON_SW_PIN, INPUT);
    attachInterrupt(digitalPinToInterrupt(BUTTON_A_PIN), button_down_isr, RISING);
    globals.actual.flagsRegister.buttonState = 1;
    Serial.println("Actidriver setup done");
}


void loop() {
  int8_t dir = 1;
  uint8_t counter = 0;
  uint8_t serialIn;
  
  globals.persistent.targetTemperature = 350;
  globals.actual.tipTemperature = 250;
  globals.actual.handleType = HT_FMRP;
  globals.actual.handleTemperature = 45;


  while (1) {
    if(Serial.available()){
      serialIn = Serial.read();
      switch (serialIn) {
        case 'u':
          fsm_handleEvent(EVT_UP);
          break;
        case 'd':
          fsm_handleEvent(EVT_DOWN);
          break;
        case 'c':
          fsm_handleEvent(EVT_CLICK);
          break;
        case 'h':
          if (globals.actual.handleTemperature == HT_FMRP) 
            globals.actual.handleType = HT_NONE;
          else 
            globals.actual.handleType = HT_FMRP;
          break;
        case 's':
          globals.actual.flagsRegister.heaterStandby = ~globals.actual.flagsRegister.heaterStandby;
          break;
        case 'e':
          globals.actual.flagsRegister.heaterTempSensorError = ~globals.actual.flagsRegister.heaterTempSensorError;
          break;


      }

    }

    // detect rotary encoder event
    if(globals.actual.counterNew > 0)
      fsm_handleEvent(EVT_UP);
    if(globals.actual.counterNew < 0)
      fsm_handleEvent(EVT_DOWN);

    noInterrupts();
    globals.actual.counterNew = 0;
    interrupts();

    if (globals.actual.tipTemperature < 100) dir = 1;
    if (globals.actual.tipTemperature > 500) dir = -1;
    globals.actual.tipTemperature += dir;
    //if (counter % 5 == 1) globals.actual.handleType = HT_NONE;
    if (counter % 5 == 3) globals.actual.handleType = HT_FMRP;
    //unsigned long start = micros();
    mainmenu_display();
    //unsigned long end = micros();
    //Serial.println(end - start);
    
    // read rotarry encoder switch
    if ((!digitalRead(BUTTON_SW_PIN)) && //button pressed?
        (globals.actual.flagsRegister.buttonState))
          fsm_handleEvent(EVT_CLICK);

    globals.actual.flagsRegister.buttonState = digitalRead(BUTTON_SW_PIN);

    counter++;
    delay(1000);
  }
    
}
