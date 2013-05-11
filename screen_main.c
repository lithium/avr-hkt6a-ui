#include "screen.h"
#include "lcd.h"
#include "global.h"
#include "utils.h"

uint8_t _batt_voltage;

#define MAX_VOLTAGE 50   //5.0v
#define LOW_VOLTAGE 36   
#define KILL_VOLTAGE 35 


inline uint8_t _convert_voltage(uint8_t a)
{
    uint8_t v = MAP((uint8_t)a, 0,255, 0,MAX_VOLTAGE);
    return MAX_VOLTAGE-v;
}

void screen_main_setup(Screen *scr, TxProfile *txp)
{
    lcd_clear();
    lcd_display(0b100); // cursor off

    AnalogState *as = event_analog_state(3);
    if (as) {
        _batt_voltage = _convert_voltage(as->val);
    }
}
void screen_main_destroy(Screen *scr, TxProfile *txp)
{
}
void screen_main_paint(Screen *scr, TxProfile *txp)
{

    if (_batt_voltage <= LOW_VOLTAGE) {
        lcd_printfxy(10,0, "!batt!");
    }
    else {
        lcd_printfxy(11,0, "v%1d.%1d", _batt_voltage / 10, _batt_voltage % 10);
    }
    lcd_printfxy(0,1, "#%d: %s", g_CurProfile+1, g_Profile.name);

}
void screen_main_event(Screen *scr, TxProfile *txp, Event *e)
{
    if (e->type == EVENT_ANALOG_DOWN || e->type == EVENT_ANALOG_UP) {
        if (e->v.analog.number == 3) {
            _batt_voltage = _convert_voltage(e->v.analog.position);
        }
    }
    else
    if (e->type == EVENT_LONG_CLICK) {
        screen_change(SCREEN_NAME);
    }
    else {
        return; //ignore any other event
    }

    scr->is_dirty=1;

}
