#include "globaldata.h"
#include "fsm.h"

State_t initialState = {NULL, NULL, NULL};
State_t selectSetPoint = {0};
State_t lowestSetPoint = {0};
State_t middleSetPoint = {0};
State_t highestSetPoint = {0};

State_t *activeState; 
//main state
void initialState_on_up() {
    activeState = &initialState;
}
void initialState_on_down() {
    selectSetPoint.flags.highlighted = true;
    activeState = &selectSetPoint;
}

//selectSetPoint
void selectSetPoint_on_up() {
    if (selectSetPoint.flags.selected) {
        globals.actual.fsmTargetTemperature += globals.actual.counterNew;
        if (globals.actual.fsmTargetTemperature > 500) globals.actual.fsmTargetTemperature = 500;
        activeState = &selectSetPoint;
        return;
    } 
    selectSetPoint.flags.highlighted = false;
    activeState = &initialState;
}
void selectSetPoint_on_down() {
    if (selectSetPoint.flags.selected) {
        globals.actual.fsmTargetTemperature += globals.actual.counterNew;
        if (globals.actual.fsmTargetTemperature < 100) globals.actual.fsmTargetTemperature = 100;
        activeState = &selectSetPoint;
        return;
    } 
    selectSetPoint.flags.highlighted = false;
    highestSetPoint.flags.highlighted = true;
    activeState = &highestSetPoint;
}

void selectSetPoint_on_click() {
    if (!selectSetPoint.flags.selected) {
        selectSetPoint.flags.selected = true;
        globals.actual.fsmTargetTemperature = globals.persistent.targetTemperature;
    } else {
        globals.persistent.targetTemperature = globals.actual.fsmTargetTemperature;
        selectSetPoint.flags.selected = false;
    }
    activeState = &selectSetPoint;
}

// highest setPoint
void highestSetPoint_on_up() {
    highestSetPoint.flags.highlighted = false;
    selectSetPoint.flags.highlighted = true;
    activeState = &selectSetPoint;
}
void highestSetPoint_on_down() {
    highestSetPoint.flags.highlighted = false;
    middleSetPoint.flags.highlighted = true;
    activeState = &middleSetPoint;
}
void highestSetPoint_on_click() {
    globals.persistent.targetTemperature = globals.persistent.setpoints[2];
    activeState = &highestSetPoint;
}

// middle setPoint
void middleSetPoint_on_up() {
    middleSetPoint.flags.highlighted = false;
    highestSetPoint.flags.highlighted = true;
    activeState = &highestSetPoint;
}
void middleSetPoint_on_down() {
    middleSetPoint.flags.highlighted = false;
    lowestSetPoint.flags.highlighted = true;
    activeState = &lowestSetPoint;
}
void middleSetPoint_on_click() {
    globals.persistent.targetTemperature = globals.persistent.setpoints[1];
    activeState = &middleSetPoint;
}

// lowest setPoint
void lowestSetPoint_on_up() {
    lowestSetPoint.flags.highlighted = false;
    middleSetPoint.flags.highlighted = true;
    activeState = &middleSetPoint;
}
void lowestSetPoint_on_down() {
    lowestSetPoint.flags.highlighted = false;
    //lowestSetPoint.flags.highlighted = true;
    activeState = &initialState;
}
void lowestSetPoint_on_click() {
    globals.persistent.targetTemperature = globals.persistent.setpoints[0];
    activeState = &lowestSetPoint;
}


void fsm_init() {
    initialState.on_up = &initialState_on_up;
    initialState.on_down = &initialState_on_down;

    selectSetPoint.on_up = &selectSetPoint_on_up;
    selectSetPoint.on_down = &selectSetPoint_on_down;
    selectSetPoint.on_click = &selectSetPoint_on_click;

    highestSetPoint.on_up = &highestSetPoint_on_up;
    highestSetPoint.on_down = &highestSetPoint_on_down;
    highestSetPoint.on_click = &highestSetPoint_on_click;

    middleSetPoint.on_up = &middleSetPoint_on_up;
    middleSetPoint.on_down = &middleSetPoint_on_down;
    middleSetPoint.on_click = &middleSetPoint_on_click;

    lowestSetPoint.on_up = &lowestSetPoint_on_up;
    lowestSetPoint.on_down = &lowestSetPoint_on_down;
    lowestSetPoint.on_click = &lowestSetPoint_on_click;

    activeState = &initialState;
}


void fsm_handleEvent(Event_t event) {
    if (activeState == NULL)
        activeState = &initialState;

    switch (event) {
        case EVT_UP:
            if (activeState->on_up != NULL)
                activeState->on_up();
            break;
        case EVT_DOWN:
            if (activeState->on_down != NULL)
                activeState->on_down();
            break;
        case EVT_CLICK:
            if (activeState->on_click != NULL)
                activeState->on_click();
            break;
        default:
            activeState = &initialState;
    }
}


