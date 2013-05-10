#ifndef EVENTS_H
#define EVENTS_H
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include "millis.h"



#define EVENT_BUTTON_MAX_COUNT 1
#define EVENT_ANALOG_MAX_COUNT 2

#define EVENT_QUEUE_MAX_SIZE 5


#define EVENT_TIMING_DOUBLE_CLICK 300
#define EVENT_TIMING_LONG_CLICK 1800

typedef enum {
    EVENT_INVALID=0,
    EVENT_CLICK,
    EVENT_DOUBLE_CLICK,
    EVENT_LONG_CLICK,
    EVENT_ANALOG_UP,
    EVENT_ANALOG_DOWN,
    EVENT_NUM
} EventType;


typedef struct {
    uint8_t number;
    int16_t position;
} AnalogEvent;
typedef struct {
    uint8_t number;
    uint8_t channel;
    uint8_t val;
} AnalogState;


typedef struct {
    uint8_t number;
} ButtonEvent;
typedef struct {
    volatile uint8_t *port;
    volatile uint8_t mask;
    uint8_t val;
    uint8_t last_val;
    uint16_t last_millis;
} ButtonState;


typedef struct {
    uint8_t id;
    EventType type;
    uint16_t millis;
    union {
        ButtonEvent button;
        AnalogEvent analog;
    } v;
} Event;



extern Event InvalidEvent;


void event_init();

uint8_t event_publish(Event e);

uint8_t event_push(Event e);
Event *event_peek();
Event event_pop();

Event event_next();


uint8_t event_register_button(uint8_t button_number, volatile uint8_t *port, uint8_t mask);
uint8_t event_register_analog(uint8_t analog_number, uint8_t channel);




#endif