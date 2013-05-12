#include "progress.h"
#include "utils.h"




uint8_t _progress_charset[64] = {
    // left end
    0b00001,
    0b00011,
    0b00011,
    0b00011,
    0b00011,
    0b00011,
    0b00011,
    0b00001,

    // right end
    0b10000,
    0b11000,
    0b11000,
    0b11000,
    0b11000,
    0b11000,
    0b11000,
    0b10000,

    // separator
    0b10001,
    0b11011,
    0b11011,
    0b11011,
    0b11011,
    0b11011,
    0b11011,
    0b10001,


    //fill-1
    0b10000,
    0b10000,
    0b10000,
    0b10000,
    0b10000,
    0b10000,
    0b10000,
    0b10000,

    //fill-2
    0b11000,
    0b11000,
    0b11000,
    0b11000,
    0b11000,
    0b11000,
    0b11000,
    0b11000,

    //fill-3
    0b11100,
    0b11100,
    0b11100,
    0b11100,
    0b11100,
    0b11100,
    0b11100,
    0b11100,

    //fill-4
    0b11110,
    0b11110,
    0b11110,
    0b11110,
    0b11110,
    0b11110,
    0b11110,
    0b11110,

    //fill-5
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b11111,

};


void progress_init()
{
    uint8_t i;
    for (i=0; i < 8; i++) {
        lcd_send_cg(i, _progress_charset+i*8);
    }
}


void progress_drawxy(uint8_t x, uint8_t y, uint8_t width, uint8_t value)
{

    lcd_cursor(x,y);
    progress_draw(width,value);
}
void progress_draw(uint8_t width, uint8_t value)
{
    uint8_t v = MAP(value, 0,255, 0, width*5);

    uint8_t full_spots = v / 5;
    uint8_t partial_fill = v % 5;

    uint8_t i;
    for (i=0; i < width; i++) {
        if (v <= i*5)
            lcd_putc(' ');
        else
        if (i<full_spots)
            lcd_putc(PROGRESS_FILL5);
        else 
        if (partial_fill)
            lcd_putc(PROGRESS_FILL1 + (partial_fill-1));
    }
}


