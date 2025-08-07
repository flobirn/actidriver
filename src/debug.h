#ifndef __DEBUG_H__
#define __DEBUG_H__

#undef DEBUG_LOG_ON

#ifdef DEBUG_LOG_ON

#define dbgText(msg) {Serial.println(msg);}
#define dbgVariable(msg, var) {Serial.print(msg); Serial.println(var);}

#else
#define dbgText(msg)
#define dbgVariable(msg, var)

#endif

#endif