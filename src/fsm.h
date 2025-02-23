#ifndef __FSM_H__
#define __FSM_H__

#include "Arduino.h"

typedef enum{
    EVT_NONE,
    EVT_UP,
    EVT_DOWN,
    EVT_CLICK
} Event_t;

typedef struct {
    uint8_t highlighted:1;
    uint8_t selected:1;
} StateFlags_t;

typedef struct {
    void (*on_up) ();
    void (*on_down) ();
    void (*on_click) ();

    StateFlags_t flags;

} State_t;

void fsm_init();
void fsm_handleEvent(Event_t event);

#endif