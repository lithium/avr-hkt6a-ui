#include "screen.h"


static InputInfo _save_inputs[] = {
    //x,y, min,max
    {6,0,  0,1},
};
#define _save_inputs_size (sizeof(_save_inputs)/sizeof(InputInfo))

static const char *_save_names[] = {"No","Yes"};
uint8_t want_to_save=0;

void screen_save_setup(Screen *scr, TxProfile *txp)
{
    lcd_clear();
    lcd_printfxy(0,0,"Save?");
    lcd_printfxy(0,1,"#%d: %s", g_CurProfile+1, g_Profile.name);

    lcd_display(0b101); // cursor off but blinking

    input_init(_save_inputs, _save_inputs_size);
    input_assign(0, &want_to_save);
}
void screen_save_destroy(Screen *scr, TxProfile *txp)
{
    input_destroy();
}
void screen_save_paint(Screen *scr, TxProfile *txp)
{
    lcd_printfxy(6,0, _save_names[want_to_save]);

    input_cursor();
}
void _save_or_exit(uint8_t save)
{
    if (save) {
        save_profile_to_eeprom(g_CurProfile, &g_Profile);
    } 
    else {
        //reload profile
        profile_change(g_CurProfile); 
    }
    screen_change(SCREEN_MAIN);
}

void screen_save_event(Screen *scr, TxProfile *txp, Event *e)
{
    if (e->type == EVENT_ANALOG_DOWN || e->type == EVENT_ANALOG_UP) {
        if (e->v.analog.number == 2) {
            int8_t v = (e->v.analog.position - 128)/64; // -7 .. +7
            input_value(v);
        }
    }
    else
    if (e->type == EVENT_CLICK) {
        _save_or_exit(want_to_save);
    }
    else
    if (e->type == EVENT_LONG_CLICK) {
        _save_or_exit(want_to_save);
    }
    else
    if (e->type == EVENT_DOUBLE_CLICK) {
        //abort
        _save_or_exit(0);
    }
    else {
        return; //ignore any other event
    }

    scr->is_dirty=1;

}

