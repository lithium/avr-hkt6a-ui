#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>



#define LED_ON() PORTB |= _BV(5)
#define LED_OFF() PORTB |= _BV(5)

#include "lcd.h"
#include "millis.h"
#include "event.h"

static uint8_t g_cur_pot=0;
#define g_n_pots 2
static uint8_t g_pots[g_n_pots];
uint8_t g_button=0;

uint8_t g_dirty=1;

#define LONG_PRESS_MILLIS 1500

int frame=0;

void paint(void);

int main(void) 
{

    DDRB |= _BV(PB5);
    LED_OFF();

    sei();


    millis_init();
    lcd_init();
    event_init();


    //pushbutton on PB4
    DDRB &= ~_BV(4); //input pin
    PORTB &= ~_BV(4); // turn off pull up
    event_register_button(1,&PORTB,_BV(PB4));


    //setup ADC 
    // prescaler: 128
    ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0)| (1<<ADEN);
    ADMUX = (1<<REFS0)|(1<<ADLAR);
    //trigger one conversion for interrupt
    ADCSRA |= (1<<ADIE)|(1<<ADSC);


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
        Event *e = event_peek();
        // if ()

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

    // sprintf(buf, "f:%03d", frame);
    // lcd_cursor(10,0);
    // lcd_write(buf);

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
        // turn off ADC interrupts until next TIMER1 
        ADCSRA &= ~((1<<ADSC)|(1<<ADEN));
    }

    // switch input pin
    ADMUX = (ADMUX&0xF0) | (g_cur_pot & 0x0F);
}

/*
ISR(TIMER1_COMPA_vect)
{
    ADCSRA |= ((1<<ADSC)|(1<<ADEN));

    uint8_t v = (PINB & _BV(4));
    if (v != g_button) {
        // edge trigger
        g_button=v;
        g_button_ts=millis();

        lcd_cursor(6,0);
        if (g_button) {
            lcd_write("click");
        } else { //release
            lcd_write("         ");
        }
        g_dirty=1;
    }
    else
    if (g_button && g_button_ts) {
        unsigned long now = millis();
        unsigned long delta = now - g_button_ts;
        if (now - g_button_ts > LONG_PRESS_MILLIS) {
            lcd_cursor(6,0);
            static char f[10];
            sprintf(f,"long:%d",delta);
            lcd_write(f);
            g_button_ts = now;
            // g_dirty=1;
        }
    }

}
    */
