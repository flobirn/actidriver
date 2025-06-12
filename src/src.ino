#include "globaldata.h"
#include "display.h"
#include "mainmenu.h"
#include "fsm.h"
#include "pins.h"
#include "adc.h"
#include "debug.h"
#include "handleInterface.hpp"

extern GlobalData_t globals;
extern HandleInterface* handleInterfaces[];

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
    Serial.println("Setup rotary encoder");
// rotary encoder
    pinMode(BUTTON_A_PIN, INPUT);
    pinMode(BUTTON_B_PIN, INPUT);
    pinMode(BUTTON_SW_PIN, INPUT);
    attachInterrupt(digitalPinToInterrupt(BUTTON_A_PIN), button_down_isr, RISING);
    globals.buttonState = 1;
//  handle setup
    
//  adc setup
    Serial.println("Setup reference for ADC");
    analogReference(EXTERNAL);
    setupAdc();
    //calibrateAdc();

    Serial.println("Actidriver setup done");
}


void loop() {

  uint8_t serialIn;
  
  /* only for tests */
  handleInterfaces[0]->targetTemperature = 350;
  handleInterfaces[1]->targetTemperature = 380;

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

    analogReference(EXTERNAL);
    handleInterfaces[0]->update();
    handleInterfaces[1]->update();
    mainmenu_display();
    delay(50);
  }
    
}
