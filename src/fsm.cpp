#include "globaldata.h"
#include "fsm.h"

State *activeState; 

State               initialState;
SelectSetPointState selectSetPoint;
SetPointState       setPoint;

//intialState
State* InitialState::onUp() {
    return (switchTo(&selectSetPoint));
}

//selectSetPoint
State* SelectSetPointState::onUp() {
    if (flags.selected) {
        globals.handleActuals[handle].fsmTargetTemperature += globals.counterNew;
        if (globals.handleActuals[handle].fsmTargetTemperature > 500) globals.handleActuals[handle].fsmTargetTemperature = 500;
        return this;
    }
    if (handle > 0 ) {
        handle--;
        return this;
    }
    return (switchTo(&initialState));
}
State* SelectSetPointState::onDown() {
    if (flags.selected) {
        globals.handleActuals[handle].fsmTargetTemperature -= globals.counterNew;
        if (globals.handleActuals[handle].fsmTargetTemperature < 100) globals.handleActuals[handle].fsmTargetTemperature = 100;
        return this;
    }
    if (handle < MAX_HANDLES - 1) {
        handle++;
        return this;
    }
    return (switchTo(&setPoint));
}

State* SelectSetPointState::onClick() {
    if (!flags.selected) {
        flags.selected = true;
        globals.handleActuals[handle].fsmTargetTemperature = globals.handlePersistent[handle].targetTemperature;
    } else {
        globals.handlePersistent[handle].targetTemperature = globals.handleActuals[handle].fsmTargetTemperature;
        flags.selected = false;
    }
    return this;
}

State* SetPointState::onUp() {
    if ((flags.selected) &&
        (handle < MAX_HANDLES -1)) {
        handle++;
        return this;
    }
    if (setPoint < NUMBER_OF_SETPOINTS) {
        setPoint++;
        return this;
    }
    return (switchTo(&initialState));
}
State* SetPointState::onDown() {
    if ((flags.selected) &&
        (handle > 0)) {
        handle--;
        return this;
    }
    if (setPoint > 0) {
        setPoint--;
        return this;
    }
    return (switchTo(&selectSetPoint));
}
State* SetPointState::onClick() {
    if (!flags.selected) {
        flags.selected = true;
        return this;
    } 
    globals.handlePersistent[handle].targetTemperature = globals.setpoints[setPoint];
    flags.selected = 0;
    return this;
}

void fsm_init() {
    activeState = &initialState;
}


void fsm_handleEvent(Event_t event) {
    if (activeState == NULL)
        activeState = &initialState;

    switch (event) {
        case EVT_UP:
            activeState->onUp();
            break;
        case EVT_DOWN:
            activeState->onDown();
            break;
        case EVT_CLICK:
            activeState->onClick();
            break;
        default:
            activeState = &initialState;
    }
}


