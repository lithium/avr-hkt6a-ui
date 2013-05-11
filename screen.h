#ifndef SCREEN_H
#define SCREEN_H

#include <avr/io.h>
#include "event.h"
#include "txprofile.h"



typedef struct Screen_struct Screen;

typedef struct {
    void (*setup_func)(Screen *, TxProfile *);
    void (*paint_func)(Screen *, TxProfile *);
    void (*event_func)(Screen *, TxProfile *, Event*);
    void (*destroy_func)(Screen *, TxProfile *);
} Screen_vtable;

struct Screen_struct {
    Screen_vtable vtable;
    uint8_t is_dirty;
};



extern Screen_vtable ScreenTable[];
extern const uint8_t ScreenTableSize;

// this order must match screentable.c!!!
#define SCREEN_MAIN 0
#define SCREEN_NAME 1
#define SCREEN_EPA 2
#define SCREEN_SWITCHES 3
#define SCREEN_MIXES 4
void screen_change(uint8_t scr_id);


#endif