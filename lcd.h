#ifndef LCD_H
#define LCD_H

#include <avr/io.h>
#include <util/delay.h>


/*
Author: Wiggins
*/

void lcd_init();

void lcd_send(uint8_t value, uint8_t is_data);
#define lcd_cmd(c) lcd_send(c,0)
#define lcd_putc(c) lcd_send(c,1)

void lcd_write(const char *c);

void lcd_display(uint8_t cb_bits);
void lcd_clear();
void lcd_home();
void lcd_cursor(uint8_t col, uint8_t row);


#endif //LCD_H
