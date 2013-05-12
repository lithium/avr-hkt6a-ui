#include "serial.h"

#include <util/delay.h>


void serial_init(void)
{


    UCSR0A = _BV(U2X0);
    UCSR0C = _BV(UCSZ01) | _BV(UCSZ00); /* async, N 8 1 */
    UCSR0B = _BV(RXEN0) | _BV(TXEN0) | _BV(RXCIE0);   /* Enable RX and TX */

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



uint8_t _convert_channel_data_packet(uint8_t *packet, int16_t *out)
{
    uint8_t i;
    uint16_t chksum=packet[14]<<8 | packet[15];
    for (i=0; i < 14; i++) 
        chksum -= packet[i];

    if (chksum) {
        //chksum mismatch!
        return 0;
    }

    for (i=0; i<6; i++) {
        out[i] = ((packet[i*2]<<8 | packet[i*2+1])-1000) - 512;
    }
    return 1; 
}

extern uint8_t _batt_voltage;
extern uint16_t _counter;

#include "global.h"

static uint16_t _packet_status;
static uint8_t _packet_buffer[69]; //max packet size
static uint8_t _packet_counter=0;
static uint8_t *_packet;

#define PACKET_HEADER 8 
#define PACKET_READING 9

// queries:   
//      get settings: 85,250,0
//      set settings: 85,255,packet

ISR(USART_RX_vect)
{
    uint8_t data = UDR0;

    if (_packet_status & _BV(PACKET_READING)) {
        *_packet = data;
        _packet++;
        if (--_packet_counter == 0) {
            if ((_packet_status & 0xFF) == 252) {
                Event e = create_event(EVENT_CHANNEL_DATA);
                if (_convert_channel_data_packet((uint8_t*)_packet_buffer, e.v.channel.values)) {

                    event_push(e);
                }
            }
            _packet_status = 0;
        }
    }
    else
    if (_packet_status & _BV(PACKET_HEADER)) {
        _packet_status &= ~_BV(PACKET_HEADER);
        _packet_status |= data;
        if (data == 252) { // channel data
            _packet_counter = 16;
        }
        else 
        if (data == 253) { // settings packet
            _packet_counter = 67;
        }
        if (_packet_counter) {
            _packet_status |= _BV(PACKET_READING);
            _packet = _packet_buffer;
        }
    }
    else
    if (data == 85) {
        _packet_status = _BV(PACKET_HEADER);
    } 

                // _batt_voltage = 252;
    // g_Screen.is_dirty = 1;
}

