#include "globaldata.h"
#include "display.h"
#include "mainmenu.h"
#include "fsm.h"
#include "pins.h"


extern GlobalData_t globals;

void button_down_isr() {
  if (digitalRead(BUTTON_B_PIN)) {
    //rotated left
    globals.counterNew--;
  } else {
    //rotated right
    globals.counterNew++;
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
    globals.buttonState = 1;
    Serial.println("Actidriver setup done");
}


void loop() {
  int8_t dir = 1;
  uint8_t counter = 0;
  uint8_t serialIn;
  
  globals.handlePersistent[0].targetTemperature = 350;
  globals.handleActuals[0].tipTemperature = 250;
  globals.handleActuals[0].handleType = HT_FMRP;
  globals.handleActuals[0].handleTemperature = 45;
  globals.handlePersistent[1].targetTemperature = 380;
  globals.handleActuals[1].tipTemperature = 290;
  globals.handleActuals[1].handleType = HT_FMRP;
  globals.handleActuals[1].handleTemperature = 55;



  while (1) {
    if(Serial.available()){
      serialIn = Serial.read();
      switch (serialIn) {
        case 'u':
          fsm_handleEvent(EVT_RIGHT);
          break;
        case 'd':
          fsm_handleEvent(EVT_LEFT);
          break;
        case 'c':
          fsm_handleEvent(EVT_CLICK);
          break;
        case 'h':
          if (globals.handleActuals[0].handleTemperature == HT_FMRP) 
            globals.handleActuals[0].handleType = HT_NONE;
          else 
            globals.handleActuals[0].handleType = HT_FMRP;
          break;
        case 's':
          globals.handleActuals[0].flagsRegister.heaterStandby = ~globals.handleActuals[0].flagsRegister.heaterStandby;
          globals.handleActuals[1].flagsRegister.heaterStandby = ~globals.handleActuals[1].flagsRegister.heaterStandby;
          break;
        case 'e':
          globals.handleActuals[0].flagsRegister.heaterTempSensorError = ~globals.handleActuals[0].flagsRegister.heaterTempSensorError;
          globals.handleActuals[1].flagsRegister.heaterTempSensorError = ~globals.handleActuals[1].flagsRegister.heaterTempSensorError;
          break;


      }

    }

    // detect rotary encoder event
    if(globals.counterNew > 0)
      fsm_handleEvent(EVT_RIGHT);
    if(globals.counterNew < 0)
      fsm_handleEvent(EVT_LEFT);

    noInterrupts();
    globals.counterNew = 0;
    interrupts();

    // read rotarry encoder switch
    if ((!digitalRead(BUTTON_SW_PIN)) && //button pressed?
        (globals.buttonState))
          fsm_handleEvent(EVT_CLICK);

    globals.buttonState = digitalRead(BUTTON_SW_PIN);

    if (globals.handleActuals[0].tipTemperature < 100) dir = 1;
    if (globals.handleActuals[0].tipTemperature > 500) dir = -1;
    globals.handleActuals[0].tipTemperature += dir;
    //if (counter % 5 == 1) globals.handleActuals.handleType = HT_NONE;
    if (counter % 5 == 3) globals.handleActuals[0].handleType = HT_FMRP;
    //unsigned long start = micros();
    mainmenu_display();
    //unsigned long end = micros();
    //Serial.println(end - start);
    


    counter++;
    delay(1000);
  }
    
}
