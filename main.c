#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>


#define LED_ON() PORTB |= _BV(5)
#define LED_OFF() PORTB |= _BV(5)

#include "lcd.h"

static uint8_t g_cur_pot=0;
#define g_n_pots 2
static uint8_t g_pots[g_n_pots];

static uint8_t g_dirty=1;

void read_pots()
{
    ADCSRA |= (1<<ADSC);
}

int main(void) 
{
    DDRB |= _BV(PB5);
    LED_OFF();


    //setup ADC 
    // prescaler: 128
    ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0)| (1<<ADEN);
    ADMUX = (1<<REFS0)|(1<<ADLAR);

    //trigger one conversion for interrupt
    ADCSRA |= (1<<ADIE)|(1<<ADSC);
    sei();


    lcd_init();
    LED_ON();



    lcd_cursor(0,0);
    lcd_write("A0:");
    lcd_cursor(0,1);
    lcd_write("A1:");

    for (;;) {
        if (g_dirty) {
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

}

ISR(ADC_vect)
{

    g_pots[g_cur_pot] = ADCH;
    g_dirty = 1;


    int new_pot = g_cur_pot+1;
    if (new_pot >= g_n_pots) {
        new_pot = 0;
    }
    g_cur_pot = new_pot;

    // switch input pin
    ADMUX = (ADMUX&0xF0) | (new_pot & 0x0F);
    // trigger another interrupt
    ADCSRA |= (1<<ADSC);
}