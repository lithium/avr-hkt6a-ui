#include "serial.h"

#include <util/delay.h>

void serial_init(void)
{


    UCSR0A = _BV(U2X0);
    UCSR0C = _BV(UCSZ01) | _BV(UCSZ00); /* async, N 8 1 */
    UCSR0B = _BV(RXEN0) | _BV(TXEN0) ; // | _BV(RXCIE0);   /* Enable RX and TX */

    //16Mhz @ 115200 UBRR values:  U2Xn=0:8  U2Xn=1:16
    UBRR0H = 0;
    UBRR0L = 16;


}


uint8_t serial_readchar(void) {
    loop_until_bit_is_set(UCSR0A, RXC0);
    return UDR0;
}

void serial_writechar(uint8_t c) {
    UDR0 = c;
    loop_until_bit_is_set(UCSR0A, TXC0);
}

extern uint8_t _batt_voltage;
extern uint8_t _counter;

#include "global.h"

ISR(USART_RX_vect)
{
    uint8_t data = UDR0;

    _batt_voltage = data;
    _counter++;
    g_Screen.is_dirty = 1;

    if (data == 85) {
        _batt_voltage = 42;
    } 
}