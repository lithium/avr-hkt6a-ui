#include <avr/io.h>
#include <util/delay.h>


#define LED_ON() PORTB |= _BV(5)
#define LED_OFF() PORTB |= _BV(5)

#include "lcd.h"
#include <avr/interrupt.h>

int main(void) 
{
    DDRB |= _BV(PB5);
    LED_OFF();


    //setup ADC 
    // prescaler: 128
    ADCSRA |= (1<<ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);// | (1<<ADFR);
    ADMUX = (1<<REFS0)|(1<<ADLAR);


    //turn on adc sampling
    ADCSRA |= (1<<ADSC) | (1<<ADATE);

    //free running mode
    ADCSRB = 0;



    lcd_init();
    LED_ON();



    lcd_cursor(0,0);
    lcd_write("A0:");
    static char buf[10];

    for (;;) {
        sprintf(buf, "%02d", ADCH);
        lcd_cursor(3,0);
        lcd_write(buf);
    }

    
    return 0;
}