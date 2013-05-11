#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>



#define LED_ON() PORTB |= _BV(5)
#define LED_OFF() PORTB |= _BV(5)

#include "lcd.h"
#include "millis.h"
#include "event.h"
#include "screen.h"


// int frame=0;
Screen g_Screen;
TxProfile g_Profile;





int main(void) 
{
    sei();

    //indicator led
    DDRB |= _BV(PB5);
    LED_OFF();


    millis_init();
    lcd_init();
    event_init();


    //pushbutton on PB4
    DDRB &= ~_BV(4); //input pin
    PORTB &= ~_BV(4); // turn off pull up
    event_register_button(1,&PINB,_BV(PB4));

    event_register_analog(1,0);
    event_register_analog(2,1);



    LED_ON();


    memset(&g_Profile, 0, sizeof(TxProfile));
    screen_change(0);

    for (;;) {
        if (event_peek()) {
            Event e = event_pop();
            g_Screen.vtable.event_func(&g_Screen, &g_Profile, &e);
        }

        if (g_Screen.is_dirty) {
            // frame++;
            g_Screen.vtable.paint_func(&g_Screen, &g_Profile);
            g_Screen.is_dirty = 0;
        }

        sleep_enable();
        sleep_cpu();
        sleep_disable();
    }

    
    return 0;
}
