#include "serial.h"
#include "global.h"
#include <util/delay.h>

static uint16_t _packet_status;
static uint8_t _packet_buffer[69]; //max packet size
static uint8_t _packet_counter=0;
static uint8_t *_packet;

#define PACKET_HEADER 8 
#define PACKET_READING 9
#define PACKET_WRITING 10

// queries:   
//      get settings: 85,250,0
//      set settings: 85,255,packet
// responses:
//      channel data: 85,252
//      settings data: 85,253



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



void serial_load_settings()
{
    _packet_buffer[0] = 85;
    _packet_buffer[1] = 250;
    _packet_buffer[2] = 0;
    _transmit_packet(3);
}

void serial_write_settings(TxProfile *txp)
{
    write_settings_packet(_packet_buffer, txp);
    _transmit_packet(69);
}


void _transmit_packet(uint8_t size)
{
    _packet = _packet_buffer+1;
    _packet_status = _BV(PACKET_WRITING);
    _packet_counter = size-1;

    _counter = 21;
    g_Screen.is_dirty = 1;

    serial_writechar(*_packet_buffer); // send first byte
    // UDR0 = *_packet_buffer;

    UCSR0B |= _BV(TXCIE0);  // transmit interrupt on
    UCSR0B &= ~_BV(RXCIE0);  
}

uint8_t _convert_channel_data_packet(uint8_t *packet, uint16_t *out)
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
        out[i] = ((packet[i*2]<<8 | packet[i*2+1])-1000);
    }
    return 1; 
}



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
            else
            if ((_packet_status & 0xFF) == 253) {
                _counter = 253;
                g_Screen.is_dirty = 1;

                read_settings_packet(&g_Profile, (uint8_t *)_packet_buffer);

            }
            // _counter = _packet_status & 0xFF;
            _packet_status = 0;
        }
    }
    else
    if (_packet_status & _BV(PACKET_HEADER)) {
        _packet_status &= ~_BV(PACKET_HEADER);
        _packet_status |= data;
        _packet_counter = 0;
        if (data == 252) { // channel data
            _packet_counter = 16;
        }
        else 
        if (data == 253) { // settings packet
            _counter = 253;
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

}

ISR(USART_TX_vect)
{

    if (_packet_status & _BV(PACKET_WRITING)) {
        // UDR0 = *_packet;
        serial_writechar(*_packet);
        _packet++;
        _packet_counter--;

        _counter++;
        g_Screen.is_dirty =1;

        if (_packet_counter == 0) {
            // uint8_t i;
            // for (i=0; i<69; i++) {
            //     _packet_buffer[i] = serial_readchar();
            // }
            // _counter = _packet_buffer[1];
        }
        else
            return;

    }
    

    // transmit interrupt off
    _packet_status = 0;
    UCSR0B &= ~_BV(TXCIE0);  
    UCSR0B |= _BV(RXCIE0);  
}
