#include "globaldata.h"
#include "display.h"
#include "solarized.h"
//#include "logisoso09_64pt7b.h"

typedef struct {
    uint16_t tipTemperature;
    uint8_t handleTemperature;
    uint16_t targetTemperature;

    HandleType_t handleType;
} DisplayedValues_t;

DisplayedValues_t shownValues = {0};

/* main menu configuration */

#define  ERROR_BG_COLOR SOLARIZED_24b_base3
#define  ERROR_FG_COLOR SOLARIZED_24b_red

#define  TIP_TEMPERATURE_LENGTH 3
#define  TIP_TEMPERATURE_X 0
#define  TIP_TEMPERATURE_Y 64
#define  TIP_TEMPERATURE_BG_COLOR SOLARIZED_24b_base3
#define  TIP_TEMPERATURE_FG_COLOR SOLARIZED_24b_blue
#define  TIP_TEMPERATURE_FONT ucg_font_inb53_mn
#define  TIP_TEMPERATURE_FONT_SIZE 1

static inline void displayTipTemperature() {
    display.setFont(TIP_TEMPERATURE_FONT);
    display.setPrintPos(TIP_TEMPERATURE_X, TIP_TEMPERATURE_Y);

    display.setColor(FG_COLOR_IDX, TIP_TEMPERATURE_FG_COLOR);
    display.setColor(BG_COLOR_IDX, TIP_TEMPERATURE_BG_COLOR);
    if (globals.actual.tipTemperature != shownValues.tipTemperature) {
        display.print(globals.actual.tipTemperature);
        shownValues.tipTemperature = globals.actual.tipTemperature;
    }

}


#define  TARGET_TEMPERATURE_LENGTH 3
#define  TARGET_TEMPERATURE_X 48
#define  TARGET_TEMPERATURE_Y 100
#define  TARGET_TEMPERATURE_BG_COLOR SOLARIZED_24b_base3
#define  TARGET_TEMPERATURE_FG_COLOR SOLARIZED_24b_blue
#define  TARGET_TEMPERATURE_FONT ucg_font_logisoso22_hr
#define  TARGET_TEMPERATURE_FONT_SIZE 2

static inline void displayTargetTemperature() {
    display.setFont(TARGET_TEMPERATURE_FONT);
    display.setPrintPos(TARGET_TEMPERATURE_X, TARGET_TEMPERATURE_Y);

    display.setColor(FG_COLOR_IDX, TARGET_TEMPERATURE_FG_COLOR);
    display.setColor(BG_COLOR_IDX, TARGET_TEMPERATURE_BG_COLOR);
    if (globals.persistent.targetTemperature != shownValues.targetTemperature) {
        display.print(globals.persistent.targetTemperature);
        shownValues.targetTemperature = globals.persistent.targetTemperature;
    }

}


#define  HANDLE_TYPE_X 8
#define  HANDLE_TYPE_Y 132
#define  HANDLE_TYPE_BG_COLOR SOLARIZED_24b_base3
#define  HANDLE_TYPE_FG_COLOR SOLARIZED_24b_blue
#define  HANDLE_TYPE_FONT ucg_font_helvB12_hr
#define  HANDLE_TYPE_FONT_SIZE 2

static inline void displayHandleType() {
    display.setFont(HANDLE_TYPE_FONT);
    display.setPrintPos(HANDLE_TYPE_X, HANDLE_TYPE_Y);
    
    if (globals.actual.handleType != shownValues.handleType) {
        switch (globals.actual.handleType) {
            case HT_FMRP:
                display.setColor(FG_COLOR_IDX, HANDLE_TYPE_FG_COLOR);
                display.setColor(BG_COLOR_IDX, HANDLE_TYPE_BG_COLOR);
                display.print("  FMRP: ");
                display.print(globals.actual.handleTemperature);
                display.print("`C  ");
                break;
            default:
                display.setColor(FG_COLOR_IDX, ERROR_FG_COLOR);
                display.setColor(BG_COLOR_IDX, ERROR_BG_COLOR);
                display.print(" NO HANDLE!  ");
                break;
        }
        shownValues.handleType = globals.actual.handleType;
    }

}


#define  MENU_LINE_X 0
#define  MENU_LINE_Y 154
#define  MENU_LINE_BG_COLOR SOLARIZED_24b_base3
#define  MENU_LINE_FG_COLOR SOLARIZED_24b_blue
#define  MENU_LINE_FONT ucg_font_helvB08_hr
#define  MENU_LINE_FONT_SIZE 2

static inline void displayMenuBar() {
    display.setFont(MENU_LINE_FONT);
    display.setPrintPos(MENU_LINE_X, MENU_LINE_Y);
    
    display.setColor(FG_COLOR_IDX, MENU_LINE_FG_COLOR);
    display.setColor(BG_COLOR_IDX, MENU_LINE_BG_COLOR);
    display.print("   SAVE  DISP  MENU ");

}

void mainmenu_display() {
    //display tip temperature
    displayTipTemperature();
    //display target temperature
    displayTargetTemperature();
    displayHandleType();
    displayMenuBar();

}
