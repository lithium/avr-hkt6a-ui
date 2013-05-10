#include "event.h"



static uint8_t _event_id_counter=0;
static Event _event_queue[EVENT_QUEUE_MAX_SIZE];
static signed char _event_queue_head=0;
static signed char _event_queue_tail=0;
static uint8_t _event_queue_size=0;


Event InvalidEvent = {EVENT_INVALID, EVENT_INVALID};


ButtonState _event_buttons[EVENT_BUTTON_MAX_COUNT];


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
// Event *_find_button_event(uint8_t button_no, EventType button_type)
// {
//     uint8_t i;
//     for (i=_event_queue_head; i<=_event_queue_tail; ) {
//         Event *e = &_event_queue[i] ;
//         if (e->type == button_type && e->v.button.number == button_no) {
//             return e;
//         }

//         if (++i >= EVENT_QUEUE_MAX_SIZE) i=0;
//     } 
//     return 0;
// }


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
    ADCSRA |= ((1<<ADSC)|(1<<ADEN));

    // read button states 
    uint8_t i;
    for (i=0; i < EVENT_BUTTON_MAX_COUNT; i++) {
        ButtonState *bs = &_event_buttons[i];
        if (!bs->port) 
            continue;
        uint8_t v = *(bs->port) & (bs->mask);
        bs->val = v;
        // uint8_t oldval = bs->val;

        Event e = _create_event(EVENT_INVALID);
        e.v.button.number = i;

        if (v != bs->last_val) {
            if (v) {
                //down trigger
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
            if (v && (e.millis - bs->last_millis) > EVENT_TIMING_LONG_CLICK) {
                e.type = EVENT_LONG_CLICK;
                event_push(e);
                bs->last_millis = 0;
            }
        }

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


}
