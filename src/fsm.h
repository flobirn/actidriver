#ifndef __FSM_H__
#define __FSM_H__

#include "Arduino.h"

typedef enum {
    EVT_NONE,
    EVT_RIGHT,
    EVT_LEFT,
    EVT_CLICK
} Event_t;

typedef struct {
    uint8_t highlighted:1;
    uint8_t selected:1;
 } StateFlags_t;

typedef struct {
    StateFlags_t flags;
    uint8_t      handle;

} State_t;

class State {
    public:
    virtual State* onRight() {return this;};
    virtual State* onLeft() {return this;};
    virtual State* onClick() {return this;};
    StateFlags_t flags;
    State* switchTo (State* next) {
        Serial.print("switch to ");
        Serial.println((uint16_t)next);
        flags.highlighted = false;
        next->flags.highlighted = true;
        return next;
    }
//   uint8_t      handle;

    State() : flags{0} {};
};

class InitialState : public State {
    public: virtual State* onRight();
};

class SelectSetPointState : public State {
    public:
    virtual State* onRight();
    virtual State* onLeft();
    virtual State* onClick();
    uint8_t handle;
};

class SetPointState : public State { 
    public:
    virtual State* onRight();
    virtual State* onLeft();
    virtual State* onClick();
    uint8_t setPoint;
    uint8_t handle;
};

void fsm_init();
void fsm_handleEvent(Event_t event);

#endif