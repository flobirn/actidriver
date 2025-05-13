#include "globaldata.h"
#include "display.h"
#include "solarized.h"
#include "fsm.h"
//#include "logisoso09_64pt7b.h"

typedef struct {
    uint16_t tipTemperature;
    uint8_t  handleTemperature;
    uint16_t targetTemperature;

    HandleType_t handleType;

} HandleDisplayedValues_t;

typedef struct {
    HandleDisplayedValues_t handle[MAX_HANDLES];
    uint16_t setPoints[NUMBER_OF_SETPOINTS];
} DisplayedData_t;


DisplayedData_t shownValues = {
    .handle = {{0}, {0}}, 
    .setPoints = {0},
};

/* main menu configuration */

#define SCREEN_HEIGHT 128
#define SCREEN_WIDTH  160
#define SCREEN_COL_WIDTH (SCREEN_WIDTH / MAX_HANDLES)

#define  ERROR_BG_COLOR SOLARIZED_24b_base3
#define  ERROR_FG_COLOR SOLARIZED_24b_red

#define  TIP_TEMPERATURE_LENGTH 3
//#define  TIP_TEMPERATURE_FONT ucg_font_inb53_mn
//#define  TIP_TEMPERATURE_FONT ucg_font_logisoso38_tn
#define  TIP_TEMPERATURE_FONT ucg_font_inb33_mn
#define  TIP_TEMPERATURE_X 0
#define  TIP_TEMPERATURE_CHAR_WIDTH 25
#define  TIP_TEMPERATURE_CHAR_HEIGHT 43
#define  TIP_TEMPERATURE_STRING_WIDTH (TIP_TEMPERATURE_LENGTH * TIP_TEMPERATURE_CHAR_WIDTH)
#define  TIP_TEMPERATURE_X2 (TIP_TEMPERATURE_X + (SCREEN_WIDTH / 2))
#define  TIP_TEMPERATURE_Y TIP_TEMPERATURE_CHAR_HEIGHT
#define  TIP_TEMPERATURE_BG_COLOR SOLARIZED_24b_base3
#define  TIP_TEMPERATURE_FG_COLOR SOLARIZED_24b_blue

#define  TIP_TEMPERATURE_ERROR_BG_COLOR SOLARIZED_24b_red
#define  TIP_TEMPERATURE_ERROR_FG_COLOR SOLARIZED_24b_base3
#define  TIP_TEMPERATURE_ERROR_FONT ucg_font_helvB12_hf
#define  TIP_TEMPERATURE_ERROR_LENGTH 5
#define  TIP_TEMPERATURE_ERROR_CHAR_WIDTH 14
#define  TIP_TEMPERATURE_ERROR_CHAR_HEIGHT 17
#define  TIP_TEMPERATURE_ERROR_STRING_WIDTH (TIP_TEMPERATURE_ERROR_LENGTH * TIP_TEMPERATURE_ERROR_CHAR_WIDTH)
#define  TIP_TEMPERATURE_ERROR_MASK 0x8000u
#define  TIP_TEMPERATURE_ERROR_X 0
#define  TIP_TEMPERATURE_ERROR_Y TIP_TEMPERATURE_ERROR_CHAR_HEIGHT

#define  TIP_TEMPERATURE_STANDBY_BG_COLOR SOLARIZED_24b_base3
#define  TIP_TEMPERATURE_STANDBY_FG_COLOR SOLARIZED_24b_cyan
#define  TIP_TEMPERATURE_STANDBY_FONT ucg_font_helvB12_hf
#define  TIP_TEMPERATURE_STANDBY_LENGTH 5
#define  TIP_TEMPERATURE_STANDBY_CHAR_WIDTH 14
#define  TIP_TEMPERATURE_STANDBY_CHAR_HEIGHT 17
#define  TIP_TEMPERATURE_STANDBY_STRING_WIDTH (TIP_TEMPERATURE_STANDBY_LENGTH * TIP_TEMPERATURE_STANDBY_CHAR_WIDTH)
#define  TIP_TEMPERATURE_STANDBY_MASK 0x4000u
#define  TIP_TEMPERATURE_STANDBY_X 0
#define  TIP_TEMPERATURE_STANDBY_Y TIP_TEMPERATURE_STANDBY_CHAR_HEIGHT
#define  TIP_TEMPERATURE_STANDBY_HEIGHT TIP_TEMPERATURE_STANDBY_CHAR_HEIGHT


static inline void displayTipTemperature() {
    for (int handle = 0; handle < MAX_HANDLES; handle++) {
        //do we have to display error state?
        if (globals.handleActuals[handle].flagsRegister.heaterTempSensorError) {
            if ((shownValues.handle[handle].tipTemperature & TIP_TEMPERATURE_ERROR_MASK) != TIP_TEMPERATURE_ERROR_MASK) {
                display.setColor(FG_COLOR_IDX, TIP_TEMPERATURE_BG_COLOR);
                display.drawBox(handle * SCREEN_COL_WIDTH, 0, (handle+1) * SCREEN_COL_WIDTH, TIP_TEMPERATURE_CHAR_HEIGHT);
                display.setFont(TIP_TEMPERATURE_ERROR_FONT);
                display.setColor(FG_COLOR_IDX, TIP_TEMPERATURE_ERROR_FG_COLOR);
                display.setColor(BG_COLOR_IDX, TIP_TEMPERATURE_ERROR_BG_COLOR);
                display.setPrintPos(TIP_TEMPERATURE_ERROR_X + handle * SCREEN_COL_WIDTH, TIP_TEMPERATURE_ERROR_Y);
                // ignore everything else, just display error
                display.print(" ERROR ");
                display.setPrintPos(TIP_TEMPERATURE_ERROR_X + handle * SCREEN_COL_WIDTH, TIP_TEMPERATURE_ERROR_Y+16);
                display.print("Tip Temp.");
                shownValues.handle[handle].tipTemperature = TIP_TEMPERATURE_ERROR_MASK;
            }
        } else if (globals.handleActuals[handle].flagsRegister.heaterStandby) {
            // stand by mode, display temperature and "standby"
            if ((shownValues.handle[handle].tipTemperature) != (globals.handleActuals[handle].tipTemperature | TIP_TEMPERATURE_STANDBY_MASK)) {        
                display.setFont(TIP_TEMPERATURE_STANDBY_FONT);
                // display standby and tip temperature
                if (shownValues.handle[handle].tipTemperature < TIP_TEMPERATURE_STANDBY_MASK) {
                    display.setColor(FG_COLOR_IDX, TIP_TEMPERATURE_BG_COLOR);
                    display.drawBox(0 + handle * SCREEN_COL_WIDTH, 0, (handle+1) * SCREEN_COL_WIDTH + TIP_TEMPERATURE_STRING_WIDTH, TIP_TEMPERATURE_CHAR_HEIGHT);
                    display.setColor(FG_COLOR_IDX, TIP_TEMPERATURE_STANDBY_FG_COLOR);
                    display.setColor(BG_COLOR_IDX, TIP_TEMPERATURE_STANDBY_BG_COLOR);
                    display.setPrintPos(TIP_TEMPERATURE_STANDBY_X + handle * SCREEN_COL_WIDTH, TIP_TEMPERATURE_STANDBY_Y);
                    display.print("STANDBY");
                }

                if ((shownValues.handle[handle].tipTemperature & ~TIP_TEMPERATURE_STANDBY_MASK) != globals.handleActuals[handle].tipTemperature) {
                    display.setPrintPos(TIP_TEMPERATURE_STANDBY_X + handle * SCREEN_COL_WIDTH, TIP_TEMPERATURE_STANDBY_Y+TIP_TEMPERATURE_STANDBY_HEIGHT);
                    display.print(globals.handleActuals[handle].tipTemperature);
                }

                shownValues.handle[handle].tipTemperature = globals.handleActuals[handle].tipTemperature | TIP_TEMPERATURE_STANDBY_MASK;
            }
        } else {
            //normal display
            display.setFont(TIP_TEMPERATURE_FONT);
            display.setPrintPos(TIP_TEMPERATURE_X + handle * TIP_TEMPERATURE_X2, TIP_TEMPERATURE_Y);
            uint8_t tempNew = globals.handleActuals[handle].tipTemperature / 100; //hundreds number
            uint8_t tempOld = shownValues.handle[handle].tipTemperature / 100; //hundreds number
        
            display.setColor(FG_COLOR_IDX, TIP_TEMPERATURE_FG_COLOR);
            display.setColor(BG_COLOR_IDX, TIP_TEMPERATURE_BG_COLOR);
            if (tempNew != tempOld) {
                display.setPrintPos(TIP_TEMPERATURE_X + (handle * TIP_TEMPERATURE_X2), TIP_TEMPERATURE_Y);
                //display.setColor(FG_COLOR_IDX, TIP_TEMPERATURE_BG_COLOR);
                //display.print(tempOld);
                //display.setPrintPos(TIP_TEMPERATURE_X + (handle * TIP_TEMPERATURE_X2), TIP_TEMPERATURE_Y);
                //display.setColor(FG_COLOR_IDX, TIP_TEMPERATURE_FG_COLOR);
                display.print(tempNew);
            }
            // tens
            tempNew = (globals.handleActuals[handle].tipTemperature % 100) / 10;
            tempOld = (shownValues.handle[handle].tipTemperature % 100) / 10;
            if (tempNew != tempOld) {
                display.setPrintPos(TIP_TEMPERATURE_X + (handle * TIP_TEMPERATURE_X2) + TIP_TEMPERATURE_CHAR_WIDTH, TIP_TEMPERATURE_Y);
                //display.setColor(FG_COLOR_IDX, TIP_TEMPERATURE_BG_COLOR);
                //display.print(tempOld);
                //display.setPrintPos(TIP_TEMPERATURE_X + (handle * TIP_TEMPERATURE_X2) + TIP_TEMPERATURE_CHAR_WIDTH, TIP_TEMPERATURE_Y);            
                //display.setColor(FG_COLOR_IDX, TIP_TEMPERATURE_FG_COLOR);
                display.print(tempNew);
            }
            //units
            tempNew = globals.handleActuals[handle].tipTemperature % 10;
            tempOld = shownValues.handle[handle].tipTemperature % 10;
            if (tempNew != tempOld) {
                display.setPrintPos(TIP_TEMPERATURE_X + handle * TIP_TEMPERATURE_X2 + 2 * TIP_TEMPERATURE_CHAR_WIDTH, TIP_TEMPERATURE_Y);
                //display.setColor(FG_COLOR_IDX, TIP_TEMPERATURE_BG_COLOR);
                //display.print(tempOld);
                //display.setPrintPos(TIP_TEMPERATURE_X + handle * TIP_TEMPERATURE_X2 + 2 * TIP_TEMPERATURE_CHAR_WIDTH, TIP_TEMPERATURE_Y);
                //display.setColor(FG_COLOR_IDX, TIP_TEMPERATURE_FG_COLOR);
                display.print(tempNew);
            }
            shownValues.handle[handle].tipTemperature = globals.handleActuals[handle].tipTemperature;
        }
    }
}


    #define  TARGET_TEMPERATURE_LENGTH 3
    #define  TARGET_TEMPERATURE_BG_COLOR SOLARIZED_24b_base3
    #define  TARGET_TEMPERATURE_FG_COLOR SOLARIZED_24b_green
    #define  SELECTED_TARGET_TEMPERATURE_BG_COLOR SOLARIZED_24b_base01
    #define  SELECTED_TARGET_TEMPERATURE_FG_COLOR SOLARIZED_24b_yellow
    #define  HIGHLIGHTED_TARGET_TEMPERATURE_BG_COLOR SOLARIZED_24b_base01
    #define  HIGHLIGHTED_TARGET_TEMPERATURE_FG_COLOR SOLARIZED_24b_orange
    #define  TARGET_TEMPERATURE_FONT ucg_font_logisoso22_hn
    #define  TARGET_TEMPERATURE_CHAR_WIDTH 13
    #define  TARGET_TEMPERATURE_CHAR_HEIGHT 27
    #define  TARGET_TEMPERATURE_X ((SCREEN_COL_WIDTH - (TARGET_TEMPERATURE_LENGTH*TARGET_TEMPERATURE_CHAR_WIDTH))/2)
    #define  TARGET_TEMPERATURE_Y (TIP_TEMPERATURE_Y + TARGET_TEMPERATURE_CHAR_HEIGHT + 5)
    #define  TARGET_TEMPERATURE_SELECTED_MASK 0x8000u
    #define  TARGET_TEMPERATURE_HIGHLIGHTED_MASK 0x4000u

static inline void displayTargetTemperature() {
    extern SelectSetPointState selectSetPoint;
    extern SetPointState setPoint;
    for (int handle = 0; handle < MAX_HANDLES; handle++) {
        uint16_t whatToShow = globals.handlePersistent[handle].targetTemperature;
        display.setFont(TARGET_TEMPERATURE_FONT);
        display.setPrintPos(TARGET_TEMPERATURE_X + handle * SCREEN_COL_WIDTH, TARGET_TEMPERATURE_Y);
        if (((selectSetPoint.handle == handle) && (selectSetPoint.flags.selected)) ||
            ((setPoint.handle == handle) && (setPoint.flags.selected))) {
            display.setColor(FG_COLOR_IDX, SELECTED_TARGET_TEMPERATURE_FG_COLOR);
            display.setColor(BG_COLOR_IDX, SELECTED_TARGET_TEMPERATURE_BG_COLOR);
            whatToShow = globals.handleActuals[handle].fsmTargetTemperature | TARGET_TEMPERATURE_SELECTED_MASK;
        } else if ((selectSetPoint.handle == handle) && (selectSetPoint.flags.highlighted)) {
            display.setColor(FG_COLOR_IDX, HIGHLIGHTED_TARGET_TEMPERATURE_FG_COLOR);
            display.setColor(BG_COLOR_IDX, HIGHLIGHTED_TARGET_TEMPERATURE_BG_COLOR);
            whatToShow = whatToShow | TARGET_TEMPERATURE_HIGHLIGHTED_MASK;
        } else {
            display.setColor(FG_COLOR_IDX, TARGET_TEMPERATURE_FG_COLOR);
            display.setColor(BG_COLOR_IDX, TARGET_TEMPERATURE_BG_COLOR);
        }
        if (whatToShow != shownValues.handle[handle].targetTemperature) {
            display.print(whatToShow & (~TARGET_TEMPERATURE_SELECTED_MASK) & (~TARGET_TEMPERATURE_HIGHLIGHTED_MASK));
            shownValues.handle[handle].targetTemperature = whatToShow;
        }
    }
}


#define  HANDLE_TYPE_X 8
#define  HANDLE_TYPE_BG_COLOR SOLARIZED_24b_base3
#define  HANDLE_TYPE_FG_COLOR SOLARIZED_24b_blue
#define  HANDLE_TYPE_FONT ucg_font_helvB12_hf
#define  HANDLE_TYPE_CHAR_WIDTH 15
#define  HANDLE_TYPE_CHAR_HEIGHT 20
#define  HANDLE_TYPE_Y (TARGET_TEMPERATURE_Y + HANDLE_TYPE_CHAR_HEIGHT)

static inline void displayHandleType() {
    display.setFont(HANDLE_TYPE_FONT);
    
    for (int handle = 0; handle < MAX_HANDLES; handle++) {
        display.setPrintPos(HANDLE_TYPE_X + handle * SCREEN_COL_WIDTH, HANDLE_TYPE_Y);
        if (globals.handleActuals[handle].handleType != shownValues.handle[handle].handleType) {
            switch (globals.handleActuals[handle].handleType) {
                case HT_FMRP:
                    display.setColor(FG_COLOR_IDX, HANDLE_TYPE_FG_COLOR);
                    display.setColor(BG_COLOR_IDX, HANDLE_TYPE_BG_COLOR);
                    display.print("FMRP: ");
                    display.print(globals.handleActuals[handle].handleTemperature);
                    display.print("`C ");
                    break;
                default:
                    display.setColor(FG_COLOR_IDX, ERROR_FG_COLOR);
                    display.setColor(BG_COLOR_IDX, ERROR_BG_COLOR);
                    display.print("NO HANDLE!");
                    break;
            }
            shownValues.handle[handle].handleType = globals.handleActuals[handle].handleType;
        }
    }
}


#define  MENU_LINE_X 0
#define  MENU_LINE_Y 154
#define  MENU_LINE_BG_COLOR SOLARIZED_24b_base3
#define  MENU_LINE_FG_COLOR SOLARIZED_24b_blue
//#define  MENU_LINE_FONT ucg_font_helvB08_hr
#define  MENU_LINE_FONT ucg_font_helvB12_hf
#define  MENU_LINE_CHAR_WIDTH 15
#define  MENU_LINE_CHAR_HEIGHT 20
#define  MENU_LINE_Y (HANDLE_TYPE_Y + MENU_LINE_CHAR_HEIGHT)

static inline void displayMenuBar() {
    display.setFont(MENU_LINE_FONT);
    display.setPrintPos(MENU_LINE_X, MENU_LINE_Y);
    
    display.setColor(FG_COLOR_IDX, MENU_LINE_FG_COLOR);
    display.setColor(BG_COLOR_IDX, MENU_LINE_BG_COLOR);
    display.print("   SAVE  DISP  MENU ");

}

#define  SETPOINT_X 10
#define  SETPOINT_BG_COLOR SOLARIZED_24b_base3
#define  SETPOINT_FG_COLOR SOLARIZED_24b_blue
#define  SETPOINT_SELECTED_MASK 0x8000u
#define  SETPOINT_HIGHLIGHTED_MASK 0x4000u
#define  HIGHLIGHTED_SETPOINT_BG_COLOR SOLARIZED_24b_base02
#define  HIGHLIGHTED_SETPOINT_FG_COLOR SOLARIZED_24b_yellow

//#define  MENU_LINE_FONT ucg_font_helvB08_hr
#define  SETPOINT_FONT ucg_font_helvB12_hf
#define  SETPOINT_CHAR_WIDTH 15
#define  SETPOINT_CHAR_HEIGHT 20
#define  SETPOINT_WIDTH (SETPOINT_CHAR_WIDTH * 3)
#define  SETPOINT_Y (MENU_LINE_Y /*+ SETPOINT_CHAR_HEIGHT*/)

static inline void displaySetpoints() {

    extern SetPointState setPoint;
    display.setFont(SETPOINT_FONT);
    
    for (uint8_t point = 0; point < NUMBER_OF_SETPOINTS; point++) {
        uint16_t temp = globals.setpoints[setPoint.setPoint];
        display.setPrintPos(SETPOINT_X + point * SETPOINT_WIDTH, SETPOINT_Y);
        if ((setPoint.flags.highlighted) && (point == setPoint.setPoint)) {
            temp = temp | SETPOINT_HIGHLIGHTED_MASK;
            display.setColor(FG_COLOR_IDX, HIGHLIGHTED_SETPOINT_FG_COLOR);
            display.setColor(BG_COLOR_IDX, HIGHLIGHTED_SETPOINT_BG_COLOR);
        } else {
            display.setColor(FG_COLOR_IDX, SETPOINT_FG_COLOR);
            display.setColor(BG_COLOR_IDX, SETPOINT_BG_COLOR);
        }
        if (temp != shownValues.setPoints[point]) {
            display.print(globals.setpoints[point]);
            shownValues.setPoints[point] = temp;
        }
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

void mainmenu_init() {
    
}