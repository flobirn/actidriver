#include "globaldata.h"
#include "display.h"
#include "mainmenu.h"

extern GlobalData_t global;

void setup() {
    display_setup();
    
}


void loop() {
    mainmenu_display();
}
