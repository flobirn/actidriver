#ifndef __FSM_H__
#define __FSM_H__

typedef struct {
    void* (*on_up) ();
    void* (*on_down) ();
    void* (*on_cick) ();

} State_t;

#endif