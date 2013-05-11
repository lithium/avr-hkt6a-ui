#include "screen.h"
#include "lcd.h"


extern uint8_t g_CurProfile;

static InputInfo _name_inputs[] = {
    //x,y, min,max
    {4,0,   -1,-1}, // we'll do input:0 ourselves
    {6,1,   0,3},
};
#define _name_inputs_size (sizeof(_name_inputs)/sizeof(InputInfo))


static const char *_mode_names[] = {"Mode1", "Mode2", "Mode3", "Mode4"};

void screen_name_setup(Screen *scr, TxProfile *txp)
{
    lcd_clear();
    lcd_printfxy(0,0,"#%d:", g_CurProfile+1);
    lcd_printfxy(0,1,"MODE:");

    lcd_display(0b101); // cursor off but blinking

    input_init(_name_inputs, _name_inputs_size);
    input_assign(1, &txp->stick_mode);
}
void screen_name_destroy(Screen *scr, TxProfile *txp)
{
    input_destroy();
}
void screen_name_paint(Screen *scr, TxProfile *txp)
{
    lcd_printfxy(4,0,txp->name);

    lcd_printfxy(6,1, _mode_names[txp->stick_mode & 3]);

    input_cursor();
}
void screen_name_event(Screen *scr, TxProfile *txp, Event *e)
{
    if (e->type == EVENT_ANALOG_DOWN || e->type == EVENT_ANALOG_UP) {
        if (e->v.analog.number == 1) {
        }
        else
        if (e->v.analog.number == 2) {
            if (input_current() == 0) {
                //
            } else {
                int8_t v = (e->v.analog.position - 128)/64; // -7 .. +7
                input_value(v);
            }

        }
    }
    else
    if (e->type == EVENT_CLICK) {
        input_next();
    }
    else
    if (e->type == EVENT_LONG_CLICK) {
        screen_change(SCREEN_EPA);

    }
    else {
        return; //ignore any other event
    }

    scr->is_dirty=1;

}
