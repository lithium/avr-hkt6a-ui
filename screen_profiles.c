#include "screen.h"
#include "lcd.h"
#include "global.h"
#include "utils.h"

static uint8_t _cur_profile;

static const char *_profiles_types[] = {"ACRO ", "H-120", "H-90 ", "H-140"};

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

    lcd_printfxy(0,0, "#%d: %s", _cur_profile+1, cache->name);

    static char rev[7];
    uint8_t i;
    for (i=0; i < 6; i++) {
        rev[i] = cache->reversed & (1<<i) ? 'R' : 'n';
    }

    rev[7]=0; //null terminate string
    // lcd_printfxy(0,1, "%s %s %s ", rev,
    //     cache->profile_flags & (1<<PROFILE_FLAG_DR) ? "DR" : "  ",
    //     cache->profile_flags & (1<<PROFILE_FLAG_TC) ? "TC" : "  ");

    lcd_cursor(0,1);
    lcd_write(rev);
    lcd_cursor(7,1);
    lcd_write(_profiles_types[cache->mode & 0x0F]);
    lcd_cursor(13,1);
    lcd_putc(cache->profile_flags & (1<<PROFILE_FLAG_MIX1) ? '1' : ' ');
    lcd_putc(cache->profile_flags & (1<<PROFILE_FLAG_MIX2) ? '2' : ' ');
    lcd_putc(cache->profile_flags & (1<<PROFILE_FLAG_MIX3) ? '3' : ' ');

}
void screen_profiles_event(Screen *scr, TxProfile *txp, Event *e)
{
    if (e->type == EVENT_ANALOG_DOWN || e->type == EVENT_ANALOG_UP) {
        if (e->v.analog.number == 1) {
            _change_profile(MAP(e->v.analog.position, EVENT_ANALOG_MAP_THR_MIN, EVENT_ANALOG_MAP_THR_MAX, 0, 5));
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