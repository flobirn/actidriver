#include "globaldata.h"
#include "fsm.h"

State *activeState; 

InitialState        initialState;
SelectSetPointState selectSetPoint;
SetPointState       setPoint;

//intialState
State* InitialState::onRight() {
    Serial.println("InitialState::onRight()");
    return (switchTo(&selectSetPoint));
}

//selectSetPoint
State* SelectSetPointState::onRight() {
    Serial.println("SelectSetPointState::onRight()");
    if (flags.selected) {
        globals.handleActuals[handle].fsmTargetTemperature += globals.counterNew;
        if (globals.handleActuals[handle].fsmTargetTemperature > 500) globals.handleActuals[handle].fsmTargetTemperature = 500;
        return this;
    }
    if (handle < MAX_HANDLES -1) {
        handle++;
        return this;
    }
    return (switchTo(&setPoint));
}
State* SelectSetPointState::onLeft() {
    Serial.println("SelectSetPointState::onLeft()");
    if (flags.selected) {
        globals.handleActuals[handle].fsmTargetTemperature += globals.counterNew;
        if (globals.handleActuals[handle].fsmTargetTemperature < 100) globals.handleActuals[handle].fsmTargetTemperature = 100;
        return this;
    }
    if (handle > 0) {
        handle--;
        return this;
    }
    return (switchTo(&initialState));
}

State* SelectSetPointState::onClick() {
    if (!flags.selected) {
        flags.selected = true;
        globals.handleActuals[handle].fsmTargetTemperature = globals.handlePersistent[handle].targetTemperature;
    } else {
        globals.handlePersistent[handle].targetTemperature = globals.handleActuals[handle].fsmTargetTemperature;
        flags.selected = false;
    }
    Serial.print("selected:"); Serial.println(flags.selected);
    return this;
}

State* SetPointState::onRight() {
    if ((flags.selected) &&
        (handle < MAX_HANDLES -1)) {
        handle++;
        return this;
    }
    if (setPoint < NUMBER_OF_SETPOINTS - 1) {
        setPoint++;
        return this;
    }
    return (switchTo(&initialState));
}
State* SetPointState::onLeft() {
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
    initialState.flags = {0};
    selectSetPoint.flags = {0};
    selectSetPoint.handle = 0;
    setPoint.flags = {0};
    setPoint.handle = 0;
    setPoint.setPoint = 0;

}


void fsm_handleEvent(Event_t event) {
     if (activeState == NULL)
        activeState = &initialState;

    switch (event) {
        case EVT_RIGHT:
            Serial.println("up");
            activeState = activeState->onRight();
            break;
        case EVT_LEFT:
            Serial.println("down");
            activeState = activeState->onLeft();
            break;
        case EVT_CLICK:
            Serial.println("click");
            activeState = activeState->onClick();
            break;
        default:
            activeState = &initialState;
    }
    Serial.print("active state ");
    Serial.println((uint16_t) activeState);
}


