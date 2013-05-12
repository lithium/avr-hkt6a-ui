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
#include "txprofile.h"
#include "serial.h"

// int frame=0;
Screen g_Screen;
TxProfile g_Profile;
uint8_t g_CurProfile=0;
TxProfileCache g_ProfileAdapter[PROFILE_MAX_COUNT];
TxSettings g_Settings;



void profile_change(uint8_t profile_id) 
{
    g_CurProfile = profile_id;
    load_profile_from_eeprom(g_CurProfile, &g_Profile);
    g_Settings.cur_profile = profile_id;
    save_settings_to_eeprom(&g_Settings);
}


extern uint8_t _batt_voltage;

int main(void) 
{
    serial_init(); // must occur with interrupts off


    sei();

    //indicator led
    DDRB |= _BV(PB5);
    LED_OFF();


    millis_init();
    lcd_init();
    event_init();


    // force_clean_eeprom(PROFILE_MAX_COUNT);

    load_settings_from_eeprom(&g_Settings);
    profile_change(g_Settings.cur_profile);

    init_profile_cache(g_ProfileAdapter, PROFILE_MAX_COUNT);




    //pushbutton on PB4 - active low
    DDRB &= ~_BV(4); //input pin
    PORTB |= _BV(4); // turn on pull up
    event_register_button(1,&PINB,_BV(PB4),0);

    event_register_analog(1,0,0);
    event_register_analog(2,1,1);
    event_register_analog(3,2,0);



    LED_ON();

    memset(&g_Screen, 0, sizeof(Screen));
    screen_change(SCREEN_MAIN);

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
