#include "event.h"



static uint8_t _event_id_counter=0;
static Event _event_queue[EVENT_QUEUE_MAX_SIZE];
static signed char _event_queue_head=0;
static signed char _event_queue_tail=0;
static uint8_t _event_queue_size=0;

static ButtonState _event_buttons[EVENT_BUTTON_MAX_COUNT];


Event InvalidEvent = {EVENT_INVALID, EVENT_INVALID};




void event_init()
{
    //set up timer 1 for input resolution
    TCCR1A = (1<<WGM12); //CTC
    TCCR1B = 0b010; // divide by 64
    OCR1A = 10; //~100ms
    TIMSK1 |= (1<<OCIE1A);
}


Event _create_event(EventType type)
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
uint8_t event_register_button(uint8_t button_number, volatile uint8_t *port, uint8_t mask)
{
    button_number--;
    if (button_number >= EVENT_BUTTON_MAX_COUNT) {
       return 0;
    }

    _event_buttons[button_number].port = port;
    _event_buttons[button_number].mask = mask;
    _event_buttons[button_number].val = 0;
    _event_buttons[button_number].last_val = 0;
    _event_buttons[button_number].last_millis = 0;


    return button_number+1;
}


extern uint8_t g_button;
extern uint8_t g_dirty;


ISR(TIMER1_COMPA_vect)
{
    //trigger a read of ADC states
    ADCSRA |= ((1<<ADSC)|(1<<ADEN));

    // read button states 
    uint8_t i;
    for (i=0; i < EVENT_BUTTON_MAX_COUNT; i++) {
        ButtonState *bs = &_event_buttons[i];
        if (!bs->port) 
            continue;
        uint8_t v = *(bs->port) & (bs->mask);
        bs->val = v;

        Event e = _create_event(EVENT_INVALID);
        e.v.button.number = i;

        if (v != bs->last_val) {
            if (v) {
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
            if (v) {
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
