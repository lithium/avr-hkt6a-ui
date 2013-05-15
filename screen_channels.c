#include "screen.h"
#include "lcd.h"
#include "global.h"
#include "utils.h"
#include "progress.h"
#include "serial.h"

uint16_t _channel_data[6];
// static uint8_t _c;

void screen_channels_setup(Screen *scr, TxProfile *txp)
{
    lcd_clear();
    lcd_display(0b100); // cursor off

    memset(_channel_data, 0, 6);
}
void screen_channels_destroy(Screen *scr, TxProfile *txp)
{
}
void screen_channels_paint(Screen *scr, TxProfile *txp)
{

    // lcd_printfxy(0,0, " %+04i %+04i %+04i", _channel_data[0], _channel_data[1], _channel_data[2]);
    // lcd_printfxy(0,1, " %+04i %+04i %+04i", _channel_data[3], _channel_data[4], _channel_data[5]);

    lcd_cursor(0,0);
    lcd_putc(PROGRESS_LEFT);
    progress_draw(4, _channel_data[0]/4);
    lcd_putc(PROGRESS_SEP);
    progress_draw(4, _channel_data[1]/4);
    lcd_putc(PROGRESS_SEP);
    progress_draw(4, _channel_data[2]/4);
    lcd_putc(PROGRESS_RIGHT);

    lcd_cursor(0,1);
    lcd_putc(PROGRESS_LEFT);
    progress_draw(4, _channel_data[3]/4);
    lcd_putc(PROGRESS_SEP);

    /*
    progress_draw(4, _channel_data[4]/4);
    lcd_putc(PROGRESS_SEP);
    progress_draw(4, _channel_data[5]/4);
    lcd_putc(PROGRESS_RIGHT);
    */
    lcd_printfxy(7,1,"c:%d",_counter);

}
void screen_channels_event(Screen *scr, TxProfile *txp, Event *e)
{
    if (e->type == EVENT_CHANNEL_DATA) {
        memcpy(_channel_data, e->v.channel.values, sizeof(uint16_t)*6);
        // _c++;
    }
    else
    if (e->type == EVENT_DOUBLE_CLICK) {
        screen_change(SCREEN_MAIN);
    }
    else
    if (e->type == EVENT_LONG_CLICK) {
        serial_load_settings();
    }
    else {
        return; //ignore any other event
    }

    scr->is_dirty=1;
}


