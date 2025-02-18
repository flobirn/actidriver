#include "globaldata.h"
#include "fsm.h"

uint16_t selectedValue;

State_t initialState = {NULL, NULL, NULL};
State_t selectSetPoint = {0};
State_t *activeState; 
//main state
void* initialState_on_up() {
    return &initialState;
}
void* initialState_on_down() {
    selectSetPoint.flags.highlighted = true;
    return &selectSetPoint;
}

//selectSetPoint
void* selectSetPoint_on_up() {
    if (selectSetPoint.flags.selected) {
        selectedValue++;
        if (selectedValue > 500) selectedValue = 500;
        return &selectSetPoint;
    } 
    selectSetPoint.flags.highlighted = false;
    return &initialState;
}
void* selectSetPoint_on_down() {
    if (selectSetPoint.flags.selected) {
        selectedValue--;
        if (selectedValue < 100) selectedValue = 100;
        return &selectSetPoint;
    } 
    selectSetPoint.flags.highlighted = false;
    return &initialState;
}

void* selectSetPoint_on_click() {
    if (!selectSetPoint.flags.selected) {
        selectSetPoint.flags.selected = true;
        selectedValue = globals.persistent.targetTemperature;
    } else {
        globals.persistent.targetTemperature = selectedValue;
        selectSetPoint.flags.selected = false;
    }
    return &selectSetPoint;
}



State_t* fsm_init() {
    initialState.on_up = &initialState_on_up;
    initialState.on_down = &initialState_on_down;

    selectSetPoint.on_up = &selectSetPoint_on_up;
    selectSetPoint.on_down = &selectSetPoint_on_down;
    selectSetPoint.on_click = &selectSetPoint_on_click;

    activeState = &initialState;
    return activeState;
}


State_t* fsm_handleEvent(Event_t event) {
    if (activeState == NULL)
        return &initialState;

    switch (event) {
        case EVT_UP:
            if (activeState->on_up != NULL)
                return (State_t*) activeState->on_up();
            break;
        case EVT_DOWN:
            if (activeState->on_down != NULL)
                return (State_t*) activeState->on_down();
            break;
        case EVT_CLICK:
            if (activeState->on_click != NULL)
                return (State_t*) activeState->on_click();
            break;
        default:
            return activeState;
    }
}


