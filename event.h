#ifndef EVENTS_H
#define EVENTS_H
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include "millis.h"



#define EVENT_BUTTON_MAX_COUNT 1

#define EVENT_QUEUE_MAX_SIZE 5


#define EVENT_TIMING_DOUBLE_CLICK 300
#define EVENT_TIMING_LONG_CLICK 1800

typedef enum {
    EVENT_INVALID=0,
    EVENT_BUTTON_DOWN,
    EVENT_BUTTON_UP,
    EVENT_CLICK,
    EVENT_DOUBLE_CLICK,
    EVENT_LONG_CLICK,
    EVENT_PAINT,
    EVENT_NUM
} EventType;


typedef struct {
    uint8_t axis;
    uint8_t value;
} MotionEvent;


typedef struct {
    uint8_t number;
} ButtonEvent;
typedef struct {
    volatile uint8_t *port;
    volatile uint8_t mask;
    uint8_t val;
    uint8_t last_val;
} ButtonState;


typedef struct {
    uint8_t id;
    EventType type;
    uint16_t millis;
    union {
        ButtonEvent button;
    } v;
} Event;



extern Event InvalidEvent;
extern ButtonState _event_buttons[];


void event_init();

uint8_t event_publish(Event e);

uint8_t event_push(Event e);
Event *event_peek();
Event event_pop();

Event event_next();


uint8_t event_register_button(uint8_t button_number, volatile uint8_t *port, uint8_t mask);

#endif