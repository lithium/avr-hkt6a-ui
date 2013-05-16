#include "screen.h"
#include "lcd.h"

static InputInfo _type_inputs[] = {
    //x,y, min,max
    {6,0,   0,3},
    {1,1,   -100,100},
    {6,1,   -100,100},
    {10,1,   -100,100},
};
#define _type_inputs_size (sizeof(_type_inputs)/sizeof(InputInfo))

static const char *_txtype_names[] = {"ACRO   ", "Heli-120", "Heli-90 ", "Heli-140"};

void screen_type_setup(Screen *scr, TxProfile *txp)
{
    lcd_clear();
    lcd_write("TYPE:");

    lcd_display(0b101); // cursor off but blinking

    input_init(_type_inputs, _type_inputs_size);
    input_assign(0, &txp->tx_mode);
    input_assign(1, &txp->swash[0]);
    input_assign(2, &txp->swash[1]);
    input_assign(3, &txp->swash[2]);
}
void screen_type_destroy(Screen *scr, TxProfile *txp)
{
    input_destroy();
}
void screen_type_paint(Screen *scr, TxProfile *txp)
{
    // lcd_printfxy(5,0, "%-11s", _switch_names[txp->switch_a]);
    // lcd_printfxy(5,1, "%-11s", _switch_names[txp->switch_b]);

    lcd_cursor(6,0);
    lcd_write(_txtype_names[txp->tx_mode & 3]);
    if (txp->tx_mode != TX_MODE_ACRO) {
        lcd_printfxy(1,1, "%+04i %+04i %+04i", txp->swash[0], txp->swash[1], txp->swash[2]);
    }
    else {
        lcd_cursor(1,1);
        lcd_write("              ");
    }

    input_cursor();
}
void screen_type_event(Screen *scr, TxProfile *txp, Event *e)
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
        if (txp->tx_mode == TX_MODE_ACRO) {
            screen_change(SCREEN_NAME);
        }
        else {
            screen_change(SCREEN_CURVE);
        }

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
