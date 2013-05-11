#include "input.h"




extern Screen g_Screen;
static uint8_t _input_current=0;
static uint8_t _input_info_size=0;
static uint16_t _input_value=0;
static uint8_t _timer_scaler=0;

#define INPUTS_MAX_SIZE 6
static InputInfo _input_info[INPUTS_MAX_SIZE];

void input_init(InputInfo *info, uint8_t info_size)
{
    memset(_input_info, 0, sizeof(_input_info));
    memcpy(_input_info, info, sizeof(InputInfo)*info_size);
    _input_info_size = info_size;
    _input_current = 0;

    _input_value=0;
    _timer_scaler=0;

    //setup timer2 for key repeat
    TCCR2A = 0b011;
    TCCR2B = 0b101; // prescale
    TIMSK2 |= (1<<TOIE2);

}
void input_destroy()
{
    TIMSK2 &= ~(1<<TOIE2);
}

void input_next()
{
    if (++_input_current >= _input_info_size) {
        _input_current = 0;
    }
    // _input_assign = 0;
}

void input_value(uint16_t v)
{
    _input_value = v; 
}
uint8_t input_current()
{
    return _input_current;
}

void input_assign(uint8_t input_id, void *assign)
{
    if (input_id < 0 || input_id >= _input_info_size)
        return;
    _input_info[input_id].assign = assign;
}


InputInfo *_current_input()
{
    if (_input_current < 0 || _input_current >= _input_info_size)
        return 0;
    return &_input_info[_input_current];
}

void input_cursor()
{
    InputInfo *info = _current_input();
    if (info)
        lcd_cursor(info->x, info->y);
        // lcd_cursor(_inputs[_cur_input][0],_inputs[_cur_input][1]);
}


void _assign_input()
{
    InputInfo *info = _current_input();
    if (!info || !info->assign)
        return;

    if (info->min < 0) {
        int8_t *assign = (int8_t*)info->assign;
        int16_t v = *assign + _input_value;
        *assign = MAX(info->min, MIN(info->max, v));
    }
    else {
        uint8_t *assign = (uint8_t*)info->assign;
        int16_t v = *assign + _input_value;
        *assign = MAX(info->min, MIN(info->max, v));
    }

    g_Screen.is_dirty = 1;
}

ISR(TIMER2_OVF_vect)
{
    if (++_timer_scaler) 
        return;
    _assign_input();
}
