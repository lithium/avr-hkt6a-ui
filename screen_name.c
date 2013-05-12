#include "screen.h"
#include "lcd.h"
#include "input.h"
#include "global.h"



static InputInfo _name_inputs[] = {
    //x,y, min,max
    {4,0,   ' ','}'}, // printable ascii
    {6,1,   0,3},
};
#define _name_inputs_size (sizeof(_name_inputs)/sizeof(InputInfo))

static uint8_t _cursor=0;

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

    if (input_current() == 0) {
        lcd_cursor(4+_cursor,0);
    } else {
        input_cursor();
    }
}

void _change_cursor(TxProfile *txp, uint8_t c)
{
    _cursor = c;
    input_assign(0, txp->name + _cursor);
}
void screen_name_event(Screen *scr, TxProfile *txp, Event *e)
{
    if (e->type == EVENT_ANALOG_DOWN || e->type == EVENT_ANALOG_UP) {
        if (e->v.analog.number == 1) {

            _change_cursor(txp, MAP(e->v.analog.position, EVENT_ANALOG_MAP_THR_MIN, EVENT_ANALOG_MAP_THR_MAX, 0, 11));
        }
        else
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
        screen_change(SCREEN_EPA);
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
