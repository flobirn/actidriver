#include "globaldata.h"
#include "display.h"
#include "solarized.h"

typedef struct {
    uint16_t tipTemperature;
    uint16_t handleTemperature;
    uint16_t targetTemperature;
} DisplayedValues_t;

/* main menu confih\guration */
#define  TIP_TEMPERATURE_LENGTH 3
#define  TIP_TEMPERATURE_X 16
#define  TIP_TEMPERATURE_Y 64
#define  TIP_TEMPERATURE__BG_COLOR SOLARIZED_BASE2
#define  TIP_TEMPERATURE__FG_COLOR SOLARIZED_CYAN
#define  TIP_TEMPERATURE_FONT logisoso09_64pt7
#define  TIP_TEMPERATURE_FONT_SIZE 1

#define  TARGET_TEMPERATURE_LENGTH 3
#define  TARGET_TEMPERATURE_X 24
#define  TARGET_TEMPERATURE_Y 80
#define  TARGET_TEMPERATURE_BG_COLOR SOLARIZED_BASE2
#define  TARGET_TEMPERATURE_FG_COLOR SOLARIZED_CYAN
#define  TARGET_TEMPERATURE_FONT NULL
#define  TARGET_TEMPERATURE_FONT_SIZE 2

DisplayedValues_t shownValues = {0};

void mainmenu_display() {
    //display tip temperature
    

}

