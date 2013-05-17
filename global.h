#ifndef GLOBAL_H
#define GLOBAL_H

#include "screen.h"

// declared in main.c
extern Screen g_Screen;
extern TxProfile g_Profile;
extern uint8_t g_CurProfile;
extern TxProfileCache g_ProfileAdapter[];
extern uint8_t g_Status;


// using the unuused high order bit of the reversed field
#define IS_PROFILE_DIRTY()    (g_Profile.reversed & 0b10000000)
#define SET_PROFILE_DIRTY()   (g_Profile.reversed |= 0b10000000)
#define CLEAR_PROFILE_DIRTY() (g_Profile.reversed &= ~0b10000000)



#define STATUS_NORMAL 0
#define STATUS_INITIAL_CONFIG 1
#define STATUS_PROFILE_MISMATCH 2

extern uint8_t _batt_voltage;
extern uint16_t _counter;


#endif
