#ifndef LCD_H
#define LCD_H

#include <avr/io.h>
#include <util/delay.h>
#include <stdarg.h>
#include <stdio.h>


/*
Author: Wiggins
*/

void lcd_init();

void lcd_send(uint8_t value, uint8_t is_data);
#define lcd_cmd(c) lcd_send(c,0)
#define lcd_putc(c) lcd_send(c,1)

void lcd_write(const char *c);
// void lcd_printf(const char *fmt, ...);
void lcd_printfxy(uint8_t x, uint8_t y, const char *fmt, ...);

void lcd_display(uint8_t cb_bits);
void lcd_clear();
void lcd_home();
void lcd_cursor(uint8_t col, uint8_t row);
void lcd_send_cg(uint8_t char_num, uint8_t glyph[]);


#endif //LCD_H
