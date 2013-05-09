#ifndef EVENTS_H
#define EVENTS_H
#include <avr/io.h>


#define EVENT_QUEUE_MAX_SIZE 5

typedef enum {
    EVENT_INVALID=0,
    EVENT_BUTTON_DOWN,
    EVENT_BUTTON_UP,
    EVENT_CLICK,
    EVENT_DOUBLE_CLICK,
    EVENT_LONG_CLICK,
    EVENT_NUM
} EventType;


typedef struct {
    uint8_t axis;
    uint8_t value;
} MotionEvent;


typedef struct {
    uint8_t button;
} ClickEvent;

typedef struct {
    uint8_t id;
    EventType type;
    union {
        ClickEvent click;
    } v;
} Event;


extern Event InvalidEvent;


void event_init();

uint8_t event_publish(Event e);

uint8_t event_push(Event e);
Event event_peek();
Event event_pop();

Event event_next();



#endif