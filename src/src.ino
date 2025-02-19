#include "globaldata.h"
#include "display.h"
#include "mainmenu.h"
#include "fsm.h"

extern GlobalData_t global;

void setup() {
    Serial.begin(115200); 
 
    fsm_init();
    display_setup();
    delay(1000);
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
      }

    }
    if (globals.actual.tipTemperature < 100) dir = 1;
    if (globals.actual.tipTemperature > 500) dir = -1;
    globals.actual.tipTemperature += dir;
    if (counter % 5 == 1) globals.actual.handleType = HT_NONE;
    if (counter % 5 == 3) globals.actual.handleType = HT_FMRP;

    mainmenu_display();

    counter++;
    delay(1000);
  }
    
}
