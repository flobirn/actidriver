#include "globaldata.h"
#include "display.h"
#include "solarized.h"
//#include "logisoso09_64pt7b.h"

typedef struct {
    uint16_t tipTemperature;
    uint16_t handleTemperature;
    uint16_t targetTemperature;
} DisplayedValues_t;

/* main menu configuration */
#define  TIP_TEMPERATURE_LENGTH 3
#define  TIP_TEMPERATURE_X 16
#define  TIP_TEMPERATURE_Y 64
#define  TIP_TEMPERATURE_BG_COLOR SOLARIZED_24b_base3
#define  TIP_TEMPERATURE_FG_COLOR SOLARIZED_24b_blue
#define  TIP_TEMPERATURE_FONT logisoso09_64pt7
#define  TIP_TEMPERATURE_FONT_SIZE 1

#define  TARGET_TEMPERATURE_LENGTH 3
#define  TARGET_TEMPERATURE_X 24
#define  TARGET_TEMPERATURE_Y 80
#define  TARGET_TEMPERATURE_BG_COLOR SOLARIZED_24b_base3
#define  TARGET_TEMPERATURE_FG_COLOR SOLARIZED_24b_blue
#define  TARGET_TEMPERATURE_FONT NULL
#define  TARGET_TEMPERATURE_FONT_SIZE 2

DisplayedValues_t shownValues = {0};

uint8_t  cursor_x;
uint8_t  cursor_y;

uint8_t  textsize_x;
uint8_t  textsize_y;

void mainmenu_display() {
    //display tip temperature

    display.setFont(TIP_TEMPERATURE_FONT);
    display.setPrintPos(TIP_TEMPERATURE_X, TIP_TEMPERATURE_Y);

    display.setColor(FG_COLOR_IDX, TIP_TEMPERATURE_FG_COLOR);
    display.setColor(BG_COLOR_IDX, TIP_TEMPERATURE_BG_COLOR);
    if (globals.actual.tipTemperature != shownValues.tipTemperature) {
        display.print(globals.actual.tipTemperature);
        shownValues.tipTemperature = globals.actual.tipTemperature;
    }
}
