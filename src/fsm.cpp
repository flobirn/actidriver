#include "globaldata.h"
#include "fsm.h"

State_t initialState = {NULL, NULL, NULL};
State_t selectSetPoint = {0};
//main state
void* initialState_on_up() {
    return &initialState;
}
void* initialState_on_down() {
    return &initialState;
}


void fsm_init() {
    initialState.on_up = &initialState_on_up;
    initialState.on_down = &initialState_on_down;

}



