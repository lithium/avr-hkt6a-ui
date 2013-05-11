#include "screen.h"
#include "lcd.h"

long map(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define MIN(a,b) ((a) < (b) ? (a) : (b))



static uint8_t _cur_channel = 0;

static uint8_t _inputs[][2] = {
    {4,0},
    {9,0},
    {13,0},
    {3,1},
    {7,1},
    {13,1},
};
#define _inputs_size (sizeof(_inputs)/(sizeof(uint8_t)*2))
static uint8_t _cur_input = 0;


static int8_t _input = 0.0;
void *_input_assign = 0;
int16_t _input_min = 0;
int16_t _input_max = 120;
static Screen *_g_screen;
uint8_t _scaler=0;



void screen_epa_setup(Screen *scr, TxProfile *txp)
{
    lcd_printfxy(0,0,"CH    DR:");
    lcd_printfxy(0,1,"EP:");

    lcd_display(0b101); // cursor off but blinking


    //setup timer2 for key repeat
    _g_screen = scr;
    TCCR2A = 0b011;
    TCCR2B = 0b101; // prescale
    TIMSK2 |= (1<<TOIE2);

}
void screen_epa_destroy(Screen *scr, TxProfile *txp)
{
    TIMSK2 &= ~(1<<TOIE2);

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

            _cur_channel = map(e->v.analog.position, 0, 255, 0, 6);
        }
        else if (e->v.analog.number == 2) {
            if (_cur_input == 0) {
                if (e->v.analog.position < 128-32)
                    txp->reversed &= ~(1<<_cur_channel);
                else
                if (e->v.analog.position > 128+32)
                    txp->reversed |= (1<<_cur_channel);
            } 
            else {
                _input = (e->v.analog.position - 128)/16; // -7 .. +7

                txp->subtrim[_cur_channel] = _input;

                switch(_cur_input) {
                    case 1:
                        _input_assign = &(txp->dual_rate[_cur_channel].on);
                        _input_min = 0;
                        _input_max = 100;
                        break;
                    case 2:
                        _input_assign = &(txp->dual_rate[_cur_channel].off);
                        _input_min = 0;
                        _input_max = 100;
                        break;
                    case 3:
                        _input_assign = &(txp->endpoints[_cur_channel].ep1);
                        _input_min = 0;
                        _input_max = 120;
                        break;
                    case 4:
                        _input_assign = &(txp->endpoints[_cur_channel].ep2);
                        _input_min = 0;
                        _input_max = 120;
                        break;
                    case 5:
                        _input_assign = &(txp->subtrim[_cur_channel]);
                        _input_min = -127;
                        _input_max = 127;
                        break;
                }
                // _assign_input();

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
        return; //ignore any other event
    }

    scr->is_dirty=1;
}

void _assign_input()
{
    if (_input && _input_assign) {
        if (_input_min < 0) {
            int8_t *assign = (int8_t*)_input_assign;
            int16_t v = *assign + _input;
            *assign = MAX(_input_min, MIN(_input_max, v));
        }
        else {
            uint8_t *assign = (uint8_t*)_input_assign;
            int16_t v = *assign + _input;
            *assign = MAX(_input_min, MIN(_input_max, v));
        }

        _g_screen->is_dirty = 1;
    }
 
}


ISR(TIMER2_OVF_vect)
{
    if (++_scaler) 
        return;
    _assign_input();
}
