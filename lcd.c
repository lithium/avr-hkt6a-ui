#include "lcd.h"


/* written by: Wiggins

 */
//     10(pb2) -> LCD 4(rs)
//      9(pb1) -> LCD 5(rw)
//      8(pb0) -> LCD 6(e)
//      7(pd7) -> LCD 11(DB7)
//      6(pd6) -> LCD 12(DB6)
//      5(pd5) -> LCD 13(DB5)
//      4(pd4) -> LCD 14(DB4)

#define RS_PORT PORTB
#define RS_PIN 2
#define RW_PORT PORTB
#define RW_PIN 1
#define E_PORT PORTB
#define E_PIN 0

#define RS_HIGH() RS_PORT |= _BV(RS_PIN)
#define RS_LOW() RS_PORT &= ~_BV(RS_PIN)
#define RW_HIGH() RW_PORT |= _BV(RW_PIN)
#define RW_LOW() RW_PORT &= ~_BV(RW_PIN)
#define E_HIGH() E_PORT |= _BV(E_PIN)
#define E_LOW() E_PORT &= ~_BV(E_PIN)


static void E_TOGGLE(void)
{
    E_LOW();
    _delay_us(1);
    E_HIGH();
    _delay_us(1);
    E_LOW();
    _delay_us(100);
}
static void lcd_write4(uint8_t data)
{
    DDRD |= 0b11110000;
    PORTD = ((data&0x0F)<<4)|(PORTD&0x0F);
    E_TOGGLE();
}


void lcd_send(uint8_t value, uint8_t is_data)
{
    if (is_data)
        RS_HIGH();
    else
        RS_LOW();
    RW_LOW();

    lcd_write4(value>>4);
    lcd_write4(value);
}

void lcd_clear()
{
    lcd_cmd(0b00000001);
    _delay_us(2000);
}
void lcd_home()
{
    lcd_cmd(0b00000010);
    _delay_us(2000);
}

void lcd_display(uint8_t cb_bits)
{
    lcd_cmd(0b1000 | (cb_bits & 0b0111));
}

void lcd_cursor(uint8_t col, uint8_t row)
{
    static uint8_t _offsets[] = {0x00, 0x40};
    if (row > 1)
        row = 1;

    //set ddram
    lcd_cmd(0b10000000 | (col + _offsets[row]));
}

void lcd_init()
{
    DDRB |= 0b00000111;  // set rs/rw/e as outputs
    DDRD |= 0b11110000;  // set db lines as outputs

    //wait for power-up
    _delay_us(50000);

    RS_LOW();
    RW_LOW();
    E_LOW();

    //set 8 bit mode 3 times...
    lcd_write4(0b0011);
    _delay_us(4500);

    lcd_write4(0b0011);
    _delay_us(4500);

    lcd_write4(0b0011);
    _delay_us(150);

    //now set 4 bit mode
    lcd_write4(0b0010);


    //function set:4bit-mode with 2 lines 5x8
    lcd_cmd(0b00101000);

    //display on cursor off blink off
    lcd_cmd(0b00001100);

    lcd_clear();
}

void lcd_write(const char *s)
{
    for (;*s; s++) {
        lcd_putc(*s);
    }
}

static char _print_buffer[16]; 
void lcd_printfxy(uint8_t x, uint8_t y, const char *fmt, ...)
{
    lcd_cursor(x,y);

    va_list args;
    va_start(args,fmt);
    vsnprintf(_print_buffer, 16, fmt, args);
    va_end(args);

    lcd_write(_print_buffer);
}
