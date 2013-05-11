#include "screen.h"
#include "lcd.h"

static InputInfo _switches_inputs[] = {
    //x,y, min,max
    {5,0,   0,3},
    {5,1,   0,3},
};
#define _switches_inputs_size (sizeof(_switches_inputs)/sizeof(InputInfo))

static const char *_switch_names[] = {"null", "dualrate", "throttlecut", "normal/idle"};

void screen_switches_setup(Screen *scr, TxProfile *txp)
{
    lcd_clear();
    lcd_printfxy(0,0,"SW.A:");
    lcd_printfxy(0,1,"SW.B:");

    lcd_display(0b101); // cursor off but blinking

    input_init(_switches_inputs, _switches_inputs_size);
    input_assign(0, &txp->switch_a);
    input_assign(1, &txp->switch_b);
}
void screen_switches_destroy(Screen *scr, TxProfile *txp)
{
    input_destroy();
}
void screen_switches_paint(Screen *scr, TxProfile *txp)
{
    lcd_printfxy(5,0, "%-11s", _switch_names[txp->switch_a]);
    lcd_printfxy(5,1, "%-11s", _switch_names[txp->switch_b]);

    input_cursor();
}
void screen_switches_event(Screen *scr, TxProfile *txp, Event *e)
{
    if (e->type == EVENT_ANALOG_DOWN || e->type == EVENT_ANALOG_UP) {
        if (e->v.analog.number == 2) {
            int8_t v = (e->v.analog.position - 128)/64; // -7 .. +7
            input_value(v);
            if (v) {
                SET_PROFILE_DIRTY();
            }

        }
    }
    else
    if (e->type == EVENT_CLICK) {
        input_next();
        _cursor = 0;
    }
    else
    if (e->type == EVENT_LONG_CLICK) {
        screen_change(SCREEN_MIXES);

    }
    else
    if (e->type == EVENT_DOUBLE_CLICK) {
        save_or_abort();
    }
    else {
        return; //ignore any other event
    }

    scr->is_dirty=1;

}
