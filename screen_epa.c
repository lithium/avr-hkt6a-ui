#include "screen.h"
#include "lcd.h"



static uint8_t _cur_channel = 0;
static int8_t _tmp = 0.0;

void screen_epa_setup(Screen *scr, TxProfile *txp)
{
    // _cur_channel = event_analog_state(THR_AXIS);

    lcd_printfxy(0,0,"CH    DR:");
    lcd_printfxy(0,1,"EP:");

}

void screen_epa_paint(Screen *scr, TxProfile *txp)
{
    lcd_printfxy(2,0, "%d", _cur_channel+1);

    lcd_printfxy(4,0, "%c", txp->reversed & (1<<_cur_channel) ? 'R' : 'N');

    lcd_printfxy(9,0, "%03d/%03d", 
        txp->dual_rate[_cur_channel].on,
        txp->dual_rate[_cur_channel].off);

    lcd_printfxy(3,1, "%03d/%03d", 
        txp->endpoints[_cur_channel].ep1,
        txp->endpoints[_cur_channel].ep2);

    lcd_printfxy(12,1, "%+04i", txp->subtrim[_cur_channel]);
    // lcd_printfxy(12,1, "%+04i", _tmp);
}

void screen_epa_event(Screen *scr, TxProfile *txp, Event *e)
{
    if (e->type == EVENT_ANALOG_DOWN || e->type == EVENT_ANALOG_UP) {
            // _tmp = ((int8_t)e->v.analog.position / 255.0);
        // if (e->v.analog.number == 1) { //THR
        // }
        // _cur_channel = 5;

    }

    // scr->is_dirty=1;
}