#ifndef SERIAL_H
#define SERIAL_H
#include "txprofile.h"

#include <avr/io.h>

void serial_init(void);
uint8_t serial_readchar(void);
void serial_writechar(uint8_t c);

void serial_write_settings(TxProfile *txp);
void serial_load_settings();
void serial_start_calibrate();
void serial_stop_calibrate();

#endif
