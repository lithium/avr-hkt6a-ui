
#include "millis.h"

volatile unsigned long timer0_overflow_count = 0;
volatile unsigned long timer0_millis = 0;
static unsigned char timer0_fract = 0;


extern int frame;
ISR(TIMER0_OVF_vect)
{
    unsigned long m = timer0_millis;
    unsigned char f = timer0_fract;

    m += MILLIS_INC;
    f += FRACT_INC;
    if (f >= FRACT_MAX) {
        f -= FRACT_MAX;
        m++;
    }

    timer0_fract = f;
    timer0_millis = m;
    timer0_overflow_count++;

}

unsigned long millis()
{
    unsigned long m;
    uint8_t oldSREG = SREG;

    cli();
    m = timer0_millis;
    SREG = oldSREG;

    return m;
}

void millis_init()
{
    //set up timer 0 for millis()
    TCCR0A |= (1<<WGM01)|(1<<WGM00);
    TCCR0B |= (1<<CS01)|(1<<CS00); // prescale 64
    TIMSK0 |= (1<<TOIE0);
    SREG |= (1<<7);
}