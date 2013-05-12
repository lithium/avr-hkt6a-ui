#ifndef SERIAL_H
#define SERIAL_H


#include <avr/io.h>

void serial_init(void);
uint8_t serial_readchar(void);
void serial_writechar(uint8_t c);


#endif
