#ifndef PROGRESS_H
#define PROGRESS_H

#include <avr/io.h>
#include "lcd.h"


void progress_init(); 
void progress_draw(uint8_t x, uint8_t y, uint8_t width, uint8_t value);


#endif