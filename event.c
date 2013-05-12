#include "event.h"



static uint8_t _event_id_counter=0;
static Event _event_queue[EVENT_QUEUE_MAX_SIZE];
static signed char _event_queue_head=0;
static signed char _event_queue_tail=0;
static uint8_t _event_queue_size=0;

static ButtonState _event_buttons[EVENT_BUTTON_MAX_COUNT];
static AnalogState _event_analogs[EVENT_ANALOG_MAX_COUNT];
static signed char _event_analog_cur = -1;


Event InvalidEvent = {EVENT_INVALID, EVENT_INVALID};


AnalogState *event_analog_state(uint8_t analog_number)
{
    if (analog_number < 1 || analog_number > EVENT_ANALOG_MAX_COUNT)
        return 0;
    return &_event_analogs[analog_number-1];
}


void event_init()
{
    memset(_event_buttons, 0, sizeof(ButtonState)*EVENT_BUTTON_MAX_COUNT);
    memset(_event_analogs, 0, sizeof(AnalogState)*EVENT_ANALOG_MAX_COUNT);

    //set up timer 1 for input resolution
    TCCR1A = (1<<WGM12); //CTC
    TCCR1B = 0b010; // divide by 64
    OCR1A = 10; //~100ms
    TIMSK1 |= (1<<OCIE1A);

    //setup ADC 
    // prescaler: 128, 8 bit mode
    ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0)| (1<<ADIE);
    ADMUX = (1<<REFS0)|(1<<ADLAR);
}


Event create_event(EventType type)
{
    Event e;
    e.id = ++_event_id_counter;
    e.type = type;
    e.millis = millis();
    return e;
}


uint8_t event_push(Event e)
{
    // queue is full
    if (_event_queue_size >= EVENT_QUEUE_MAX_SIZE)
        return EVENT_INVALID;


    if (e.id == EVENT_INVALID) {
        e.id = ++_event_id_counter;
    }

    _event_queue[_event_queue_tail] = e;
    _event_queue_size++;
    if (++_event_queue_tail >= EVENT_QUEUE_MAX_SIZE) {
        _event_queue_tail = 0;
    }

    return e.id;
}

Event *event_peek()
{
    if (_event_queue_size < 1)
        return 0;

    Event *e = &_event_queue[_event_queue_head];
    if (e->type != EVENT_INVALID) 
        return e;
    return 0;
}
Event event_pop()
{
    Event *e = event_peek();

    if (!e || e->id == EVENT_INVALID) 
        return InvalidEvent;

    Event ret = *e;

    _event_queue[_event_queue_head] = InvalidEvent;
    _event_queue_size--;
    if (++_event_queue_head >= EVENT_QUEUE_MAX_SIZE) {
        _event_queue_head = 0;
    }
    return ret;
}


Event event_next()
{
    while (!event_peek()) {
        sleep_enable();
        sleep_cpu();
        sleep_disable();
    }
    return event_pop();
}

/*
 * event_register_button - Register a port/pin with a button number.
 *
 * You must set up DDR/pullup/pulldowns yourself.
 * buttons are 1-indexed.
 * 
 */
uint8_t event_register_button(uint8_t button_no, volatile uint8_t *port, uint8_t mask, uint8_t active_high)
{
    button_no--;
    if (button_no >= EVENT_BUTTON_MAX_COUNT) {
       return 0;
    }

    _event_buttons[button_no].port = port;
    _event_buttons[button_no].mask = mask;
    _event_buttons[button_no].val = active_high ? 0: 255;
    _event_buttons[button_no].last_val = active_high ? 0 : 255;
    _event_buttons[button_no].last_millis = 0;
    _event_buttons[button_no].active_high = active_high;


    return button_no+1;
}


uint8_t event_register_analog(uint8_t analog_no, uint8_t channel, uint8_t reversed)
{
    analog_no--;
    if (analog_no >= EVENT_ANALOG_MAX_COUNT) {
       return 0;
    }

    _event_analogs[analog_no].number = analog_no+1;
    _event_analogs[analog_no].channel = channel;
    _event_analogs[analog_no].reversed = reversed;

    // this is the first one, so set up channel to start sampling
    if (_event_analog_cur < 0) {
        _event_analog_cur = analog_no;
        ADMUX = (ADMUX&0xF0) | (channel & 0x0F);
    }


    return analog_no+1;

}



// extern uint8_t g_button;
// extern uint8_t g_dirty;

extern uint8_t _batt_voltage;
#include "global.h"

ISR(TIMER1_COMPA_vect)
{
    //trigger a read of ADC states
    if (_event_analog_cur >= 0)
        ADCSRA |= ((1<<ADSC)|(1<<ADEN));

    // read button states 
    uint8_t i;
    for (i=0; i < EVENT_BUTTON_MAX_COUNT; i++) {
        ButtonState *bs = &_event_buttons[i];
        if (!bs->port) 
            continue;
        uint8_t v = *(bs->port) & (bs->mask);
        bs->val = v;


        Event e = create_event(EVENT_INVALID);
        e.v.button.number = i;



        if (v != bs->last_val) {
            if (bs->active_high ? (255-v > EVENT_BUTTON_MINMAX_THRESHOLD) : (v <= EVENT_BUTTON_MINMAX_THRESHOLD)) {
                // e.type = EVENT_BUTTON_DOWN;
                if (bs->last_millis && (e.millis - bs->last_millis) < EVENT_TIMING_DOUBLE_CLICK) {
                    e.type = EVENT_DOUBLE_CLICK;
                    event_push(e);
                    bs->last_millis = 0;
                } 
                else {
                    bs->last_millis = e.millis;
                }
            }
            else {
                // e.type = EVENT_BUTTON_UP;
            }

            bs->last_val = v;
        }

        if (bs->last_millis) {
            if (bs->active_high ? (255-v > EVENT_BUTTON_MINMAX_THRESHOLD) : (v <= EVENT_BUTTON_MINMAX_THRESHOLD)) {
                if (e.millis - bs->last_millis > EVENT_TIMING_LONG_CLICK) {
                    e.type = EVENT_LONG_CLICK;
                    event_push(e);
                    bs->last_millis = 0;
                }
            } else {
                if (e.millis - bs->last_millis > EVENT_TIMING_DOUBLE_CLICK) {
                    e.type = EVENT_CLICK;
                    event_push(e);
                    bs->last_millis = 0;
                }
            }
        }

    }


}

ISR(ADC_vect)
{
    if (_event_analog_cur < 0) 
        return;

    cli();
    uint8_t v = ADCH;
    sei();

    AnalogState *as = &_event_analogs[_event_analog_cur];
    if (as->number) {

        if (v != as->val) {
            Event e = create_event(EVENT_INVALID);
            e.v.analog.number = as->number;
            e.v.analog.position = as->reversed ? 255-v : v;

/*
            if (as->reversed ? v == 255) {
                e.type = EVENT_ANALOG_MAX;
                event_push(e);
            }
            else
            if (v == 0) {
                e.type = EVENT_ANALOG_MIN;
                event_push(e);
            }
            else
            */
            if (v > as->val) {
                e.type = EVENT_ANALOG_UP;
                event_push(e);
            }
            else {
                e.type = EVENT_ANALOG_DOWN;
                event_push(e);
            }

            as->val = v;
        }
    }

    
    extern uint16_t _counter;    
    _counter++;
    g_Screen.is_dirty = 1;


    if (++_event_analog_cur >= EVENT_ANALOG_MAX_COUNT) {
        _event_analog_cur = 0;
        // turn off ADC interrupts until next TIMER1 
        ADCSRA &= ~((1<<ADSC)|(1<<ADEN));
    }

    // switch input pin, for next time
    uint8_t next_channel = _event_analogs[_event_analog_cur].channel;
    ADMUX = (ADMUX&0xF0) | (next_channel & 0x0F);
}

