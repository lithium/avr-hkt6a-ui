#include "screen.h"
#include "lcd.h"

long map(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


static uint8_t _cur_channel = 0;
static int8_t _input = 0.0;

uint8_t *_input_assign = 0;


uint8_t _inputs[][2] = {
    {4,0},
    {11,0},
    {15,0},
    {5,1},
    {9,1},
    {15,1},
};
#define _inputs_size (sizeof(_inputs)/(sizeof(uint8_t)*2))
uint8_t _cur_input = 0;

static Screen *_g_screen;

uint8_t _scaler=0;

void screen_epa_setup(Screen *scr, TxProfile *txp)
{
    // _cur_channel = event_analog_state(THR_AXIS);

    lcd_printfxy(0,0,"CH    DR:");
    lcd_printfxy(0,1,"EP:");

    lcd_display(0b110); // cursor on and blinking


    //setup timer2 for key repeat
    TCCR2A = 0b011;
    TCCR2B = 0b100; // prescale/8
    TIMSK2 |= (1<<TOIE2);

    _g_screen = scr;
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

    lcd_cursor(_inputs[_cur_input][0],_inputs[_cur_input][1]);
}

void screen_epa_event(Screen *scr, TxProfile *txp, Event *e)
{
    if (e->type == EVENT_ANALOG_DOWN || e->type == EVENT_ANALOG_UP) {
        if (e->v.analog.number == 1) {

            _cur_channel = map(e->v.analog.position, 0, 255, 0, 5);
        }
        else if (e->v.analog.number == 2) {
            if (_cur_input == 0) {
                if (e->v.analog.position < 128-32)
                    txp->reversed &= ~(1<<_cur_channel);
                else
                if (e->v.analog.position > 128+32)
                    txp->reversed |= (1<<_cur_channel);
            } 
            else
            if (_cur_input == 1) {
                _input = (e->v.analog.position - 128)/16; // -3 .. +3
                // txp->dual_rate[_cur_channel].on += _input;
                _input_assign = &(txp->dual_rate[_cur_channel].on);

                txp->subtrim[_cur_channel] = _input;
            }
        }
    }
    else
    if (e->type == EVENT_CLICK) {
        if (++_cur_input >= _inputs_size) {
            _cur_input = 0;
        }
        _input_assign = 0;
    }
    else {
        return; //ignore event
    }

    scr->is_dirty=1;
    /*
    else
    if (e->type == EVENT_ANALOG_MAX) {
        if (_cur_input == 1) { 
            txp->dual_rate[_cur_channel].on++;
        }
    }
    else
    if (e->type == EVENT_ANALOG_MIN) {
        if (_cur_input == 1) {
            txp->dual_rate[_cur_channel].on--;
        }
    }
    */

}

#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define MIN(a,b) ((a) < (b) ? (a) : (b))

ISR(TIMER2_OVF_vect)
{
    if (++_scaler) 
        return;

    if (_input && _input_assign) {

        *_input_assign = MAX(0, MIN(100, *_input_assign + _input));

        // *_input_assign += _input;
        _g_screen->is_dirty = 1;
    }
}
