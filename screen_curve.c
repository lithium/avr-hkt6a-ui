#include "screen.h"
#include "lcd.h"
#include "progress.h"
#include "input.h"

static uint8_t _curve_is_pitch=0;
static uint8_t _curve_cur_input=0;

static InputInfo _curve_inputs[] = {
    //x,y, min,max
    {0,0,   0,100},
};


void _curve_switch_input(TxProfile *txp)
{
    struct TxCurve *eps = _curve_is_pitch ? &(txp->pitch_curve) : &(txp->throttle_curve);
    if (_curve_cur_input < 5)
        input_assign(0, &(eps[_curve_cur_input].normal));
    else
        input_assign(0, &(eps[_curve_cur_input % 5].idle));
 
}

void screen_curve_setup(Screen *scr, TxProfile *txp)
{
    lcd_clear();
    // lcd_write("TYPE:");

    lcd_display(0b101); // cursor off but blinking
    slider_vertical_setup();

    input_init(_curve_inputs, 1);
    _curve_cur_input = 0;
    _curve_switch_input(txp);
}
void screen_curve_destroy(Screen *scr, TxProfile *txp)
{
    input_destroy();
}
void screen_curve_paint(Screen *scr, TxProfile *txp)
{
    lcd_cursor(0,0);
    lcd_write(_curve_is_pitch ? "PTCH" : "THR ");

    lcd_cursor(0,1);
    lcd_write(_curve_cur_input < 5 ? "norm" : "idle");
    lcd_putc('1' + (_curve_cur_input % 5));

    struct TxCurve *eps = _curve_is_pitch ? &(txp->pitch_curve) : &(txp->throttle_curve);
    uint8_t i;
    for (i=0; i < 5; i++) {
        slider_vertical_draw(5+i, MAP(eps[i].normal, 0,100, 0,255));
    }
    for (; i < 10; i++) {
        slider_vertical_draw(6+i, MAP(eps[i % 5].idle, 0,100, 0,255));
    }


}
void screen_curve_event(Screen *scr, TxProfile *txp, Event *e)
{
    if (e->type == EVENT_ANALOG_DOWN || e->type == EVENT_ANALOG_UP) {
        if (e->v.analog.number == 1) {
            if (e->v.analog.position < 128) {
                _curve_is_pitch = 0;
            }
            else {
                _curve_is_pitch = 1;
            }
            _curve_switch_input(txp);
        }
        else
        if (e->v.analog.number == 2) {
            int8_t v = (e->v.analog.position - 128)/64; // -7 .. +7
            input_value(v);

            // struct TxCurve *eps = _curve_is_pitch ? &(txp->pitch_curve) : &(txp->throttle_curve);
            // if (_curve_cur_input < 5)
            //     eps[_curve_cur_input].normal += v;
            // else
            //     eps[_curve_cur_input % 5].idle += v;
            // input_assign

            if (v) {
                SET_PROFILE_DIRTY();
            }
        }
    }
    else
    if (e->type == EVENT_CLICK) {
        // input_next();
        if (++_curve_cur_input >= 10) 
            _curve_cur_input = 0;

        _curve_switch_input(txp);

    }
    else
    if (e->type == EVENT_LONG_CLICK) {
        screen_change(SCREEN_NAME);
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
