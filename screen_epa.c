#include "screen.h"
#include "lcd.h"
#include "input.h"
#include "utils.h"




static uint8_t _cur_channel = 0;



static InputInfo _epa_inputs[] = {
    //x,y, min,max
    {4,0,   -1,-1}, // we'll do input:0 ourselves
    {9,0,   0,100},
    {13,0,  0,100},
    {3,1,   0,120},
    {7,1,   0,120},
    {13,1,  -127,127},
};
#define _epa_inputs_size (sizeof(_epa_inputs)/sizeof(InputInfo))




void _change_channel(TxProfile *txp, uint8_t new_channel)
{
    _cur_channel = new_channel;

    input_assign(1, &(txp->dual_rate[_cur_channel].on));
    input_assign(2, &(txp->dual_rate[_cur_channel].off));
    input_assign(3, &(txp->endpoints[_cur_channel].ep1));
    input_assign(4, &(txp->endpoints[_cur_channel].ep2));
    input_assign(5, &(txp->subtrim[_cur_channel]));
}

void screen_epa_setup(Screen *scr, TxProfile *txp)
{
    lcd_printfxy(0,0,"CH    DR:");
    lcd_printfxy(0,1,"EP:");

    lcd_display(0b101); // cursor off but blinking


    input_init(_epa_inputs, _epa_inputs_size);
    _change_channel(txp, 0);
}
void screen_epa_destroy(Screen *scr, TxProfile *txp)
{
    input_destroy();

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

    input_cursor();
}

void screen_epa_event(Screen *scr, TxProfile *txp, Event *e)
{
    if (e->type == EVENT_ANALOG_DOWN || e->type == EVENT_ANALOG_UP) {
        if (e->v.analog.number == 1) {
            _change_channel(txp, MAP(e->v.analog.position, 0, 255, 0, 6));
        }
        else if (e->v.analog.number == 2) {
            if (input_current() == 0) {
                if (e->v.analog.position < 128-32)
                    txp->reversed &= ~(1<<_cur_channel);
                else
                if (e->v.analog.position > 128+32)
                    txp->reversed |= (1<<_cur_channel);
            } 
            else {
                int8_t v = (e->v.analog.position - 128)/16; // -7 .. +7
                input_value(v);

                txp->subtrim[_cur_channel] = v;

            }
        }
    }
    else
    if (e->type == EVENT_CLICK) {
        input_next();
    }
    else
    if (e->type == EVENT_LONG_CLICK) {
        screen_change(SCREEN_SWITCHES);
    }
    else {
        return; //ignore any other event
    }

    scr->is_dirty=1;
}
