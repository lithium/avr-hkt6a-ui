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


void slider_vertical_setup()
{
    uint8_t i;
    uint8_t glyph[8];
    for (i=0; i < 8; i++) {

        uint8_t j=0;
        //write blank lines
        while (j < 8-(i+1)) {
            glyph[j++] = 0;
        }

        while (j < 8) {
            glyph[j++] = 0xFF;
        }

        lcd_send_cg(i, glyph);
    }
}

void slider_vertical_draw(uint8_t col, uint8_t value)
{
    uint8_t v = MAP(value, 0,255, 0,15);
    uint8_t c = (v % 8);

    lcd_cursor(col,0);
    if (v < 8) {
        lcd_putc(' ');
        lcd_cursor(col,1);
        lcd_putc(c);
    }
    else {
        lcd_putc(c);
        lcd_cursor(col,1);
        lcd_putc(7);
    }
    // lcd_putc( ? c : ' ');
}
