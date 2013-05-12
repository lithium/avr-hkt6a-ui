#include "screen.h"
#include "lcd.h"
#include "global.h"
#include "utils.h"

int16_t _channel_data[6];
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

    lcd_printfxy(0,0, " %+04i %+04i %+04i", _channel_data[0], _channel_data[1], _channel_data[2]);
    lcd_printfxy(0,1, " %+04i %+04i %+04i", _channel_data[3], _channel_data[4], _channel_data[5]);

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
    else {
        return; //ignore any other event
    }

    scr->is_dirty=1;
}


