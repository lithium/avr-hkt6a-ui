#ifndef INPUT_H
#define INPUT_H

#include <avr/io.h>
#include "screen.h"
#include "lcd.h"
#include "utils.h"


typedef struct {
    uint8_t x,y;
    int16_t min,max;
    void *assign;
} InputInfo;

void input_init(InputInfo *info, uint8_t info_size);
void input_destroy();

uint8_t input_current();

void input_next();
void input_cursor();
void input_assign(uint8_t input_id, void *assign);
void input_value();

// void input_event(Event *e);


#endif
