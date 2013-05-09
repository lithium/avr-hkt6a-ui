#include "event.h"



static uint8_t _event_id_counter=0;
static Event _event_queue[EVENT_QUEUE_MAX_SIZE];
static signed char _event_queue_head=-1;
static signed char _event_queue_tail=0;

Event InvalidEvent = {EVENT_INVALID, EVENT_INVALID};



void event_init()
{

}


Event _create_event(EventType type)
{
    Event e;
    e.id = ++_event_id_counter;
    e.type = type;
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
Event event_peek()
{
    if (_event_queue_head == -1)
        _event_queue_head = 0;
    return _event_queue[_event_queue_head];
}
Event event_pop()
{
    Event e = event_peek();

    if (e.id == EVENT_INVALID) 
        return e;

    _event_queue[_event_queue_head] = InvalidEvent;
    if (++_event_queue_head >= EVENT_QUEUE_MAX_SIZE) {
        _event_queue_head = 0;
    }
    return e;
}


Event event_next()
{

}