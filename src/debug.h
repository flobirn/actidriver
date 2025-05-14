#ifndef __DEBUG_H__
#define __DEBUG_H__

#ifdef DEBUG_LOG_ON

#define dbgText(msg) {Serial.println(msg);}
#define dbgVariable(msg, var) {Serial.print(msg); Serial.println(var);}

#else
#define dbgText(msg)
#define dbgVariable(msg, var)

#endif

#endif