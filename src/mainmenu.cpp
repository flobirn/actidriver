#include "globaldata.h"
#include "display.h"
#include "solarized.h"
#include "fsm.h"
//#include "logisoso09_64pt7b.h"

typedef struct {
    uint16_t tipTemperature;
    uint8_t handleTemperature;
    uint16_t targetTemperature;

    HandleType_t handleType;

    uint16_t lowestSetpoint;
    uint16_t middleSetpoint;
    uint16_t highestSetpoint;
} DisplayedValues_t;

DisplayedValues_t shownValues = {0};

/* main menu configuration */

#define  ERROR_BG_COLOR SOLARIZED_24b_base3
#define  ERROR_FG_COLOR SOLARIZED_24b_red

#define  TIP_TEMPERATURE_LENGTH 3
#define  TIP_TEMPERATURE_X 0
#define  TIP_TEMPERATURE_Y 64
#define  TIP_TEMPERATURE_CHAR_WIDTH 40
#define  TIP_TEMPERATURE_BG_COLOR SOLARIZED_24b_base3
#define  TIP_TEMPERATURE_FG_COLOR SOLARIZED_24b_blue
#define  TIP_TEMPERATURE_FONT ucg_font_inb53_mn

#define  TIP_TEMPERATURE_ERROR_BG_COLOR SOLARIZED_24b_red
#define  TIP_TEMPERATURE_ERROR_FG_COLOR SOLARIZED_24b_base00
#define  TIP_TEMPERATURE_ERROR_FONT ucg_font_helvB12_hr
#define  TIP_TEMPERATURE_ERROR_MASK 0x8000u
#define  TIP_TEMPERATURE_ERROR_X 20
#define  TIP_TEMPERATURE_ERROR_Y 32

#define  TIP_TEMPERATURE_STANDBY_BG_COLOR SOLARIZED_24b_base03
#define  TIP_TEMPERATURE_STANDBY_FG_COLOR SOLARIZED_24b_cyan
#define  TIP_TEMPERATURE_STANDBY_FONT ucg_font_helvB12_hr
#define  TIP_TEMPERATURE_STANDBY_MASK 0x4000u
#define  TIP_TEMPERATURE_STANDBY_X 20
#define  TIP_TEMPERATURE_STANDBY_Y 32


static inline void displayTipTemperature() {
    //do we have to display error state?
    uint16_t temp;
    if (globals.actual.flagsRegister.heaterTempSensorError) {
        if ((shownValues.tipTemperature & TIP_TEMPERATURE_ERROR_MASK) != TIP_TEMPERATURE_ERROR_MASK) {
            display.setFont(TIP_TEMPERATURE_ERROR_FONT);
            display.setColor(FG_COLOR_IDX, TIP_TEMPERATURE_ERROR_FG_COLOR);
            display.setColor(BG_COLOR_IDX, TIP_TEMPERATURE_ERROR_BG_COLOR);
            display.setPrintPos(TIP_TEMPERATURE_ERROR_X, TIP_TEMPERATURE_ERROR_Y);
            // ignore everything else, just display error
            display.println("------");
            display.println("!!! ERROR TIP TEMP !!!");
            shownValues.tipTemperature = TIP_TEMPERATURE_ERROR_MASK;
        }
    } else if (globals.actual.flagsRegister.heaterStandby) {
        // stand by mode, display temperature and "standby"
        if ((shownValues.tipTemperature & TIP_TEMPERATURE_STANDBY_MASK) != (globals.actual.tipTemperature & TIP_TEMPERATURE_STANDBY_MASK)) {
            display.setFont(TIP_TEMPERATURE_STANDBY_FONT);
            display.setColor(FG_COLOR_IDX, TIP_TEMPERATURE_STANDBY_FG_COLOR);
            display.setColor(BG_COLOR_IDX, TIP_TEMPERATURE_STANDBY_BG_COLOR);
            display.setPrintPos(TIP_TEMPERATURE_STANDBY_X, TIP_TEMPERATURE_STANDBY_Y);

            // display standby and tip temperature
            if (shownValues.tipTemperature < TIP_TEMPERATURE_STANDBY_MASK)
                display.println("STANDBY");

            if ((shownValues.tipTemperature & ~TIP_TEMPERATURE_STANDBY_MASK) != globals.actual.tipTemperature)
                display.println(globals.actual.tipTemperature);

            shownValues.tipTemperature = globals.actual.tipTemperature | TIP_TEMPERATURE_STANDBY_MASK;
        }
    } else {
        //normal display
        display.setFont(TIP_TEMPERATURE_FONT);
        display.setPrintPos(TIP_TEMPERATURE_X, TIP_TEMPERATURE_Y);
        uint8_t tempNew = globals.actual.tipTemperature / 100; //hundreds number
        uint8_t tempOld = shownValues.tipTemperature / 100; //hundreds number
    
        display.setColor(FG_COLOR_IDX, TIP_TEMPERATURE_FG_COLOR);
        display.setColor(BG_COLOR_IDX, TIP_TEMPERATURE_BG_COLOR);
        if (tempNew != tempOld) {
            display.print(tempNew);
        }
        // tens
        tempNew = (globals.actual.tipTemperature % 100) / 10;
        tempOld = (shownValues.tipTemperature % 100) / 10;
        if (tempNew != tempOld) {
            display.setPrintPos(TIP_TEMPERATURE_X + TIP_TEMPERATURE_CHAR_WIDTH, TIP_TEMPERATURE_Y);
            display.print(tempNew);
        }
        //units
        tempNew = globals.actual.tipTemperature % 10;
        tempOld = shownValues.tipTemperature % 10;
        if (tempNew != tempOld) {
            display.setPrintPos(TIP_TEMPERATURE_X + 2 * TIP_TEMPERATURE_CHAR_WIDTH, TIP_TEMPERATURE_Y);
            display.print(tempNew);
        }
        shownValues.tipTemperature = globals.actual.tipTemperature;
    }
}


#define  TARGET_TEMPERATURE_LENGTH 3
#define  TARGET_TEMPERATURE_X 48
#define  TARGET_TEMPERATURE_Y 106
#define  TARGET_TEMPERATURE_BG_COLOR SOLARIZED_24b_base3
#define  TARGET_TEMPERATURE_FG_COLOR SOLARIZED_24b_green
#define  SELECTED_TARGET_TEMPERATURE_BG_COLOR SOLARIZED_24b_base01
#define  SELECTED_TARGET_TEMPERATURE_FG_COLOR SOLARIZED_24b_yellow
#define  HIGHLIGHTED_TARGET_TEMPERATURE_BG_COLOR SOLARIZED_24b_base01
#define  HIGHLIGHTED_TARGET_TEMPERATURE_FG_COLOR SOLARIZED_24b_orange
#define  TARGET_TEMPERATURE_FONT ucg_font_logisoso22_hr
#define  TARGET_TEMPERATURE_FONT_SIZE 2
#define  TARGET_TEMPERATURE_SELECTED_MASK 0x8000u
#define  TARGET_TEMPERATURE_HIGHLIGHTED_MASK 0x4000u

static inline void displayTargetTemperature() {
    extern State_t selectSetPoint;
    uint16_t whatToShow = globals.persistent.targetTemperature;
    display.setFont(TARGET_TEMPERATURE_FONT);
    display.setPrintPos(TARGET_TEMPERATURE_X, TARGET_TEMPERATURE_Y);
    if (selectSetPoint.flags.selected) {
        display.setColor(FG_COLOR_IDX, SELECTED_TARGET_TEMPERATURE_FG_COLOR);
        display.setColor(BG_COLOR_IDX, SELECTED_TARGET_TEMPERATURE_BG_COLOR);
        whatToShow = globals.actual.fsmTargetTemperature | TARGET_TEMPERATURE_SELECTED_MASK;
    } else if (selectSetPoint.flags.highlighted) {
        display.setColor(FG_COLOR_IDX, HIGHLIGHTED_TARGET_TEMPERATURE_FG_COLOR);
        display.setColor(BG_COLOR_IDX, HIGHLIGHTED_TARGET_TEMPERATURE_BG_COLOR);
        whatToShow = whatToShow | TARGET_TEMPERATURE_HIGHLIGHTED_MASK;
    } else {
        display.setColor(FG_COLOR_IDX, TARGET_TEMPERATURE_FG_COLOR);
        display.setColor(BG_COLOR_IDX, TARGET_TEMPERATURE_BG_COLOR);
    }
    if (whatToShow != shownValues.targetTemperature) {
        display.print(whatToShow & (TARGET_TEMPERATURE_SELECTED_MASK -1) & (TARGET_TEMPERATURE_HIGHLIGHTED_MASK -1));
        shownValues.targetTemperature = whatToShow;
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
//#define  MENU_LINE_FONT ucg_font_helvB08_hr
#define  MENU_LINE_FONT ucg_font_helvB12_hr
#define  MENU_LINE_FONT_SIZE 2

static inline void displayMenuBar() {
    display.setFont(MENU_LINE_FONT);
    display.setPrintPos(MENU_LINE_X, MENU_LINE_Y);
    
    display.setColor(FG_COLOR_IDX, MENU_LINE_FG_COLOR);
    display.setColor(BG_COLOR_IDX, MENU_LINE_BG_COLOR);
    display.print("   SAVE  DISP  MENU ");

}

#define  SETPOINT_X 10
#define  SETPOINT_Y 154
#define  SETPOINT_BG_COLOR SOLARIZED_24b_base3
#define  SETPOINT_FG_COLOR SOLARIZED_24b_blue
#define  SETPOINT_WIDTH 40
#define  HIGHLIGHTED_SETPOINT_BG_COLOR SOLARIZED_24b_base02
#define  HIGHLIGHTED_SETPOINT_FG_COLOR SOLARIZED_24b_yellow

//#define  MENU_LINE_FONT ucg_font_helvB08_hr
#define  SETPOINT_FONT ucg_font_helvB12_hr


static inline void displaySetpoints() {
    extern State_t lowestSetPoint;
    extern State_t middleSetPoint;
    extern State_t highestSetPoint;

    display.setFont(SETPOINT_FONT);
    
    //setpoint 1
    uint16_t temp = globals.persistent.setpoints[0];
    display.setPrintPos(SETPOINT_X, SETPOINT_Y);
    if (lowestSetPoint.flags.highlighted) {
        temp = temp | 0x4000;
        display.setColor(FG_COLOR_IDX, HIGHLIGHTED_SETPOINT_FG_COLOR);
        display.setColor(BG_COLOR_IDX, HIGHLIGHTED_SETPOINT_BG_COLOR);
    } else {
        display.setColor(FG_COLOR_IDX, SETPOINT_FG_COLOR);
        display.setColor(BG_COLOR_IDX, SETPOINT_BG_COLOR);
    }
    if (temp != shownValues.lowestSetpoint) {
        display.print(globals.persistent.setpoints[0]);
        shownValues.lowestSetpoint = temp;
    }

    //setpoint 2
    temp = globals.persistent.setpoints[1];
    display.setPrintPos(SETPOINT_X + SETPOINT_WIDTH, SETPOINT_Y);
    if (middleSetPoint.flags.highlighted) {
        temp = temp | 0x4000;
        display.setColor(FG_COLOR_IDX, HIGHLIGHTED_SETPOINT_FG_COLOR);
        display.setColor(BG_COLOR_IDX, HIGHLIGHTED_SETPOINT_BG_COLOR);
    } else {
        display.setColor(FG_COLOR_IDX, SETPOINT_FG_COLOR);
        display.setColor(BG_COLOR_IDX, SETPOINT_BG_COLOR);
    }
    if (temp != shownValues.middleSetpoint) {
        display.print(globals.persistent.setpoints[1]);
        shownValues.middleSetpoint = temp;
    }

    //setpoint 3
    temp = globals.persistent.setpoints[1];
    display.setPrintPos(SETPOINT_X + 2 * SETPOINT_WIDTH, SETPOINT_Y);
    if (highestSetPoint.flags.highlighted) {
        temp = temp | 0x4000;
        display.setColor(FG_COLOR_IDX, HIGHLIGHTED_SETPOINT_FG_COLOR);
        display.setColor(BG_COLOR_IDX, HIGHLIGHTED_SETPOINT_BG_COLOR);
    } else {
        display.setColor(FG_COLOR_IDX, SETPOINT_FG_COLOR);
        display.setColor(BG_COLOR_IDX, SETPOINT_BG_COLOR);
    }
    if (temp != shownValues.highestSetpoint) {
        display.print(globals.persistent.setpoints[2]);
        shownValues.highestSetpoint = temp;
    }

    

}


void mainmenu_display() {
    //display tip temperature
    displayTipTemperature();
    //display target temperature
    displayTargetTemperature();
    displayHandleType();
    //displayMenuBar();
    displaySetpoints();

}
