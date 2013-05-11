#include "screen.h"
#include "lcd.h"
#include "global.h"
#include "utils.h"

static uint8_t _cur_profile;


void _change_profile(uint8_t new_profile)
{
    _cur_profile = new_profile;
}

void screen_profiles_setup(Screen *scr, TxProfile *txp)
{
    lcd_clear();
    lcd_display(0b100); // cursor off

    _cur_profile = g_CurProfile;
}
void screen_profiles_destroy(Screen *scr, TxProfile *txp)
{
}
void screen_profiles_paint(Screen *scr, TxProfile *txp)
{
    TxProfileCache *cache = &g_ProfileAdapter[_cur_profile];

    lcd_printfxy(0,1, "#%d: %s", _cur_profile+1, cache->name);
}
void screen_profiles_event(Screen *scr, TxProfile *txp, Event *e)
{
    if (e->type == EVENT_ANALOG_DOWN || e->type == EVENT_ANALOG_UP) {
        if (e->v.analog.number == 1) {
            _change_profile(MAP(e->v.analog.position, 0, 255, 0, 6));
        }
    }
    else
    if (e->type == EVENT_CLICK) {
        profile_change(_cur_profile);
        screen_change(SCREEN_MAIN);
    }
    else
    if (e->type == EVENT_LONG_CLICK) {
        profile_change(_cur_profile);
        screen_change(SCREEN_NAME);
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