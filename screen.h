#ifndef SCREEN_H
#define SCREEN_H

#include <avr/io.h>
#include "event.h"
#include "txprofile.h"

typedef struct Screen_struct Screen;

struct Screen_struct {
    void (*setup_func)(Screen *, TxProfile *);
    void (*paint_func)(Screen *, TxProfile *);
    void (*event_func)(Screen *, TxProfile *, Event*);
    uint8_t is_dirty;
};


extern Screen ScreenTable[];
extern const uint8_t ScreenTableSize;

#endif