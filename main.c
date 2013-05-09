#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>



#define LED_ON() PORTB |= _BV(5)
#define LED_OFF() PORTB |= _BV(5)

#include "lcd.h"
#include "millis.h"

static uint8_t g_cur_pot=0;
#define g_n_pots 2
static uint8_t g_pots[g_n_pots];
static uint8_t g_button=0;

static uint8_t g_dirty=1;

int frame=0;

int main(void) 
{

    DDRB |= _BV(PB5);
    LED_OFF();

    sei();


    millis_init();


    //set up timer 1 for input resolution
    TCCR1A = (1<<WGM12); //CTC
    TCCR1B = 0b011; // divide by 256
    OCR1A = 20; //~300ms
    TIMSK1 |= (1<<OCIE1A);


    //pushbutton on PB4
    DDRB &= ~_BV(4); //input pin
    PORTB &= ~_BV(4); // turn off pull up


    //setup ADC 
    // prescaler: 128
    ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0)| (1<<ADEN);
    ADMUX = (1<<REFS0)|(1<<ADLAR);
    //trigger one conversion for interrupt
    ADCSRA |= (1<<ADIE)|(1<<ADSC);


    lcd_init();
    LED_ON();



    lcd_cursor(0,0);
    lcd_write("A0:");
    lcd_cursor(0,1);
    lcd_write("A1:");

    for (;;) {
        if (g_dirty) {
            frame++;
            paint();
            g_dirty = 0;
        }
        sleep_enable();
        sleep_cpu();
        sleep_disable();
    }

    
    return 0;
}

void paint()
{
    static char buf[10];
    sprintf(buf, "%03d", g_pots[0]);
    lcd_cursor(3,0);
    lcd_write(buf);

    sprintf(buf, "%03d", g_pots[1]);
    lcd_cursor(3,1);
    lcd_write(buf);

    sprintf(buf, "f:%03d", frame);
    lcd_cursor(10,0);
    lcd_write(buf);

    sprintf(buf, "c:%03d", g_button);
    lcd_cursor(10,1);
    lcd_write(buf);
}

ISR(ADC_vect)
{
    cli();
    uint8_t v = ADCH;
    sei();

    if (v != g_pots[g_cur_pot]) {
        g_pots[g_cur_pot] = v;
        g_dirty = 1;
    }


    if (++g_cur_pot >= g_n_pots) {
        g_cur_pot = 0;
        // turn off ADC interrupts for now
        ADCSRA &= ~((1<<ADSC)|(1<<ADEN));
    }

    // switch input pin
    ADMUX = (ADMUX&0xF0) | (g_cur_pot & 0x0F);
}

ISR(TIMER1_COMPA_vect)
{
    ADCSRA |= ((1<<ADSC)|(1<<ADEN));

    uint8_t v = (PINB & _BV(4));
    if (v != g_button) {
        g_button=v;
        g_dirty=1;
    }
}
