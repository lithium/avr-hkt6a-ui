#include <avr/io.h>
#include <util/delay.h>


#define LED_ON() PORTB |= _BV(5)
#define LED_OFF() PORTB |= _BV(5)

#include "lcd.h"

int main(void) 
{
    DDRB |= _BV(PB5);
    LED_OFF();


    lcd_init();

    lcd_cursor(8,1);
    lcd_putc('W');
    lcd_putc('o');
    lcd_putc('o');
    lcd_putc('!');
    LED_ON();

    
    return 0;
}