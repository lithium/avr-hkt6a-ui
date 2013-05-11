#include "screen.h"
#include "lcd.h"

static InputInfo _mixes_inputs[] = {
    //x,y, min,max
    {5,0,   0,7},//src
    {9,0,  0,5},//dest
    {13,0,  0,3},//switch
    {4,1,   -100,100},//up    
    {13,1,   -100,100},//down    
};
#define _mixes_inputs_size (sizeof(_mixes_inputs)/sizeof(InputInfo))

static const char *_srcdest_names[] = {"CH1", "CH2", "CH3", "CH4", "CH5", "CH6", "VRA", "VRB"};
static const char *_mixer_switch_names[] = {"SWA", "SWB", " ON", "OFF"};
static uint8_t _cur_mix = 0;

void _change_mix(TxProfile *txp, uint8_t new_mix)
{
    _cur_mix = new_mix;

    struct TxMixer *mix = &txp->mixers[_cur_mix];
    input_assign(0, &(mix->src));
    input_assign(1, &(mix->dest));
    input_assign(2, &(mix->sw));
    input_assign(3, &(mix->up_rate));
    input_assign(4, &(mix->down_rate));
}

void screen_mixes_setup(Screen *scr, TxProfile *txp)
{
    lcd_clear();
    lcd_printfxy(0,0,"MIX");
    lcd_printfxy(0,1,"UP:      DN:");

    lcd_display(0b101); // cursor off but blinking

    input_init(_mixes_inputs, _mixes_inputs_size);
    _change_mix(txp, 0);

}
void screen_mixes_destroy(Screen *scr, TxProfile *txp)
{
    input_destroy();
}
void screen_mixes_paint(Screen *scr, TxProfile *txp)
{
    struct TxMixer *mix = &(txp->mixers[_cur_mix]);

    lcd_printfxy(3,0, "%d %3s\x7e%3s %s", 
        _cur_mix+1,
        _srcdest_names[mix->src],
        _srcdest_names[mix->dest],
        _mixer_switch_names[mix->sw]);

    lcd_printfxy(3,1, "%+04i  DN:%+04i", mix->up_rate, mix->down_rate);

    input_cursor();
}



void screen_mixes_event(Screen *scr, TxProfile *txp, Event *e)
{
    if (e->type == EVENT_ANALOG_DOWN || e->type == EVENT_ANALOG_UP) {
        if (e->v.analog.number == 1) {
            _change_mix(txp, MAP(e->v.analog.position, 0, 255, 0, 3));
        }
        else 
        if (e->v.analog.number == 2) {
            int8_t v = (e->v.analog.position - 128)/64; // -7 .. +7
            input_value(v);
        }
    }
    else
    if (e->type == EVENT_CLICK) {
        input_next();
        _cursor = 0;
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
