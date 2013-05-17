#ifndef PROGRESS_H
#define PROGRESS_H

#include <avr/io.h>
#include "lcd.h"
#define PROGRESS_LEFT 0
#define PROGRESS_RIGHT 1
#define PROGRESS_SEP 2

#define PROGRESS_FILL1 3
#define PROGRESS_FILL2 4
#define PROGRESS_FILL3 5
#define PROGRESS_FILL4 6
#define PROGRESS_FILL5 7


void progress_init(); 
void progress_draw(uint8_t width, uint8_t value);
void progress_drawxy(uint8_t x, uint8_t y, uint8_t width, uint8_t value);

void slider_vertical_setup();
void slider_vertical_draw(uint8_t col, uint8_t value);

#endif