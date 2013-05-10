#include "event.h"



static uint8_t _event_id_counter=0;
static Event _event_queue[EVENT_QUEUE_MAX_SIZE];
static signed char _event_queue_head=-1;
static signed char _event_queue_tail=0;

Event InvalidEvent = {EVENT_INVALID, EVENT_INVALID};


typedef struct {
    volatile uint8_t *port;
    volatile uint8_t mask;
    uint8_t val;
} PortPin;

static PortPin _event_buttons[EVENT_BUTTON_MAX_COUNT];


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
    if (_event_queue_head == _event_queue_tail) {
        return EVENT_INVALID;
    }


    if (e.id == EVENT_INVALID) {
        e.id = ++_event_id_counter;
    }

    _event_queue[_event_queue_tail] = e;
    if (++_event_queue_tail >= EVENT_QUEUE_MAX_SIZE) {
        _event_queue_tail = 0;
    }

    return e.id;
}

Event *_event_visible(Event *e)
{
    if (e && e->type > EVENT_BUTTON_UP) 
        return e;
    return 0;
}
Event *event_peek()
{
    if (_event_queue_head == -1)
        _event_queue_head = 0;
    return &(_event_queue[_event_queue_head]);
}
Event event_pop()
{
    Event *e = event_peek();

    if (!e || e->id == EVENT_INVALID) 
        return InvalidEvent;

    _event_queue[_event_queue_head] = InvalidEvent;
    if (++_event_queue_head >= EVENT_QUEUE_MAX_SIZE) {
        _event_queue_head = 0;
    }
    return *e;
}


Event event_next()
{
    Event *e = event_peek();

    while (!e || e->id == EVENT_INVALID) {
        sleep_enable();
        sleep_cpu();
        sleep_disable();
        e = event_peek();
    }
    return event_pop();
}


// Event *_find_visible_event()
// {
//     uint8_t i;
//     for (i=_event_queue_head; i<=_event_queue_tail; ) {
//         Event *e = &_event_queue[i] ;
//         if (e->type > EVENT_BUTTON_UP) {
//             return e;
//         }
//         if (++i >= EVENT_QUEUE_MAX_SIZE) i=0;
//     } 
//     return 0;
// }
Event *_find_button_event(uint8_t button_no, EventType button_type)
{
    uint8_t i;
    for (i=_event_queue_head; i<=_event_queue_tail; ) {
        Event *e = &_event_queue[i] ;
        if (e->type == button_type && e->v.button.number == button_no) {
            return e;
        }

        if (++i >= EVENT_QUEUE_MAX_SIZE) i=0;
    } 
    return 0;
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


    return button_number+1;
}

extern uint8_t g_dirty;

#include <avr/delay.h>

ISR(TIMER1_COMPA_vect)
//ISR(PCINT0_vect)
{
    ADCSRA |= ((1<<ADSC)|(1<<ADEN));

    // read button states 
    uint8_t i;
    for (i=0; i < EVENT_BUTTON_MAX_COUNT; i++) {
        uint8_t v = *_event_buttons[i].port & _event_buttons[i].mask;
        uint8_t oldval = _event_buttons[i].val;
        _event_buttons[i].val = v;

        Event e = _create_event(EVENT_INVALID);
        e.v.button.number = i;

        e.type = EVENT_CLICK;

/*
        Event *be = _find_button_event(i, EVENT_BUTTON_DOWN);
        uint16_t delta = (e.millis - be->millis);

        if (v != oldval) {
            if (v) {
                // down edge trigger
                if (be && (delta < EVENT_TIMING_DOUBLE_CLICK)) {
                    be->type = EVENT_DOUBLE_CLICK;
                } else {
                    e.type = EVENT_BUTTON_DOWN;
                    event_push(e);
                }
            } else {
                // up edge trigger
                if (be) {
                    if (delta < EVENT_TIMING_DOUBLE_CLICK) {
                        //this could still be a double click...
                        be->type = EVENT_BUTTON_UP;
                    }
                    else {
                        be->type = EVENT_CLICK;
                    }
                } else {
                    //this shouldn't happen...
                }
            }
        }

        if (v) {

            if (be && delta > EVENT_TIMING_LONG_CLICK) {
                be->type = EVENT_LONG_CLICK;
            }
        } 
        else {
            be = _find_button_event(i, EVENT_BUTTON_UP);
            if (e.millis - be->millis > EVENT_TIMING_DOUBLE_CLICK) {
                be->type = EVENT_CLICK;
            }
        }
        */
    }

    // if (g_dirty) {
        // PORTB &= ~_BV(5);
        // _delay_ms(500);
        // PORTB |= _BV(5);
        event_push(_create_event(EVENT_PAINT));
    // }


}
