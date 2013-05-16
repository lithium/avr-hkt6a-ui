#include "txprofile.h"
#include "lcd.h"
#include "global.h"


TxProfile DefaultProfile = {
    "            ", //12 bytes
    (uint8_t)0, //acro
    (uint8_t)1, //mode2
    (uint8_t)0, //no channels reversed
    {
        {(uint8_t)100,(uint8_t)80},
        {(uint8_t)100,(uint8_t)80},
        {(uint8_t)100,(uint8_t)80}
    }, //dualrate
    {
        {(uint8_t)100,(uint8_t)100},
        {(uint8_t)100,(uint8_t)100},
        {(uint8_t)100,(uint8_t)100},
        {(uint8_t)100,(uint8_t)100},
        {(uint8_t)100,(uint8_t)100},
        {(uint8_t)100,(uint8_t)100},
    }, //endpoints
    {(int8_t)0,(int8_t)0,(int8_t)0,(int8_t)0,(int8_t)0,(int8_t)0}, //subtrim
    (uint8_t)1, //swa - dualrate
    (uint8_t)2, //swb - throttlecut
    {
        {(uint8_t)0,(uint8_t)0,(int8_t)100,(int8_t)100,(uint8_t)3},
        {(uint8_t)0,(uint8_t)0,(int8_t)100,(int8_t)100,(uint8_t)3},
        {(uint8_t)0,(uint8_t)0,(int8_t)100,(int8_t)100,(uint8_t)3},
    }, // 3mixers off by default
    (uint8_t)0, (uint8_t)0, (uint8_t)0, //swash
    {
        (uint8_t)10, (uint8_t)10,
        (uint8_t)25, (uint8_t)25,
        (uint8_t)50, (uint8_t)50,
        (uint8_t)75, (uint8_t)75,
        (uint8_t)100, (uint8_t)100,
    }, //linear throttle curve
    {
        (uint8_t)10, (uint8_t)10,
        (uint8_t)25, (uint8_t)25,
        (uint8_t)50, (uint8_t)50,
        (uint8_t)75, (uint8_t)75,
        (uint8_t)100, (uint8_t)100,
    }, //linear pitch curve
};

TxSettings DefaultSettings = {0};

void init_profile_cache(TxProfileCache *adapter, uint8_t size)
{
    uint8_t i;
    for (i=0; i < size; i++) {
        update_profile_cache_from_eeprom(i, &adapter[i]);
    }
}



uint8_t _read_profile_default(uint8_t *address, uint8_t profile_offset)
{
    uint8_t data = eeprom_read_byte(address + profile_offset);

    if (data != 0xFF) {
        return data;
    }

    uint8_t *d = (uint8_t*)&DefaultProfile;
    return d[profile_offset];
}

#include <avr/delay.h>
void _prep_save_block(uint8_t *address, uint8_t profile_id)
{
    eeprom_update_word((uint16_t*)address, PROFILE_SAVEBLOCK_HEADER);


    //default name
    char default_name[12] = "profile     ";
    default_name[7] = '1'+profile_id;
    eeprom_update_block(&default_name, address+2, 12);

    //write out 255 to each spot to use default value
    uint8_t i;
    for (i=2+12; i < PROFILE_SAVEBLOCK_SIZE; i++) {
        eeprom_update_byte(address+i, 0xFF);
    }

}

void force_clean_eeprom(uint8_t size)
{
    uint8_t i;
    //overwrite/prep save blocks
    for (i=0; i <size; i++) {
        uint8_t *address = (uint8_t*)PROFILE_EEPROM_SAVEBLOCK_OFFSET + PROFILE_SAVEBLOCK_SIZE*i;
        _prep_save_block(address, i);
    } 
}

int update_profile_cache(uint8_t profile_id, TxProfile *txp)
{
    if (profile_id >= PROFILE_MAX_COUNT) 
        return 0;

    TxProfileCache *cache = &g_ProfileAdapter[profile_id];

    memcpy(cache->name, txp->name, 12);

    cache->reversed = txp->reversed & 0b00111111;

    cache->mode = (txp->stick_mode&0x0F) <<4 | (txp->tx_mode & 0x0F);

    cache->profile_flags = 0;
    if (txp->switch_a == SWITCH_FUNC_DUAL_RATE || txp->switch_b == SWITCH_FUNC_DUAL_RATE) {
        cache->profile_flags |= (1<<PROFILE_FLAG_DR);
    }
    if (txp->switch_a == SWITCH_FUNC_THROTTLE_CUT || txp->switch_b == SWITCH_FUNC_THROTTLE_CUT) {
        cache->profile_flags |= (1<<PROFILE_FLAG_TC);
    }

    uint8_t i; 
    for (i=0; i <3; i++) {
        if (txp->mixers[i].sw != MIXER_SWITCH_OFF)
            cache->profile_flags |= (1<<i);
    }

    return 1;
}

int update_profile_cache_from_eeprom(uint8_t profile_id, TxProfileCache *cache)
{

    uint8_t i=0;
    if (profile_id >= PROFILE_MAX_COUNT) {
        return 0;
    }
    uint8_t *address = (uint8_t*)PROFILE_EEPROM_SAVEBLOCK_OFFSET + PROFILE_SAVEBLOCK_SIZE*profile_id;

    // first two bytes should be our TxProfile Header block:
    uint16_t header = eeprom_read_word((uint16_t*)address);
    if ((header & PROFILE_SAVEBLOCK_HEADER_MASK) != PROFILE_SAVEBLOCK_HEADER) {
        // _prep_save_block(address, profile_id);
        return 0;
    }

    //skip header
    address += 2;

    //name
    memset(cache->name, 0, 12);
    for (i=0; i < 12; i++) {
        cache->name[i] = _read_profile_default(address, offsetof(TxProfile, name)+i);
    }

    //reversed
    cache->reversed = _read_profile_default(address, offsetof(TxProfile, reversed));

    cache->mode = _read_profile_default(address, offsetof(TxProfile, stick_mode)) << 4;
    cache->mode |= _read_profile_default(address, offsetof(TxProfile, tx_mode))&0x0F;

    //determine dr/tc flags
    uint8_t switch_a = _read_profile_default(address, offsetof(TxProfile, switch_a));
    uint8_t switch_b = _read_profile_default(address, offsetof(TxProfile, switch_b));
    cache->profile_flags = 0;
    if (switch_a == SWITCH_FUNC_DUAL_RATE || switch_b == SWITCH_FUNC_DUAL_RATE) {
        cache->profile_flags |= (1<<PROFILE_FLAG_DR);
    }
    if (switch_a == SWITCH_FUNC_THROTTLE_CUT || switch_b == SWITCH_FUNC_THROTTLE_CUT) {
        cache->profile_flags |= (1<<PROFILE_FLAG_TC);
    }

    //determine which mixers are in use
    uint8_t sw_ofs = offsetof(TxProfile, mixers) + offsetof(struct TxMixer, sw);
    for (i=0; i <3; i++) {
        uint8_t mix_sw = _read_profile_default(address, sw_ofs + sizeof(struct TxMixer)*i);
        if (mix_sw != MIXER_SWITCH_OFF)
            cache->profile_flags |= (1<<i);
    }



    return 1;
}

void save_profile_to_eeprom(uint8_t profile_id, TxProfile *txp)
{
    uint8_t i=0;
    if (profile_id >= PROFILE_MAX_COUNT) {
        return;
    }
    uint8_t *address = (uint8_t*)PROFILE_EEPROM_SAVEBLOCK_OFFSET + PROFILE_SAVEBLOCK_SIZE*profile_id;



    // first two bytes should be our TxProfile Header block:
    uint16_t header = eeprom_read_word((uint16_t*)address);
    if ((header & PROFILE_SAVEBLOCK_HEADER_MASK) != PROFILE_SAVEBLOCK_HEADER) {
        _prep_save_block(address, profile_id);
    }

    uint8_t *p = (uint8_t*)txp;
    uint8_t *d = (uint8_t*)&DefaultProfile;
    for (i=2; i < PROFILE_SAVEBLOCK_SIZE; i++, p++, d++) {
        //only update bytes that differ from the default
        if (*p != *d) {
            eeprom_update_byte(address+i,*p);
        }
    }
}

int load_profile_from_eeprom(uint8_t profile_id, TxProfile *txp)
{
    if (profile_id >= PROFILE_MAX_COUNT)
        return 0;
    uint8_t *address = (uint8_t*)PROFILE_EEPROM_SAVEBLOCK_OFFSET + PROFILE_SAVEBLOCK_SIZE*profile_id;

    // first two bytes should be our TxProfile Header block:
    uint16_t header = eeprom_read_word((uint16_t*)address);
    if ((header & PROFILE_SAVEBLOCK_HEADER_MASK) != PROFILE_SAVEBLOCK_HEADER) {
        return 0;
    }

    memcpy(txp, &DefaultProfile, sizeof(TxProfile));

    uint8_t i=0;
    uint8_t *p = (uint8_t*)txp;
    for (i=2; i < PROFILE_SAVEBLOCK_SIZE; i++, p++) {
        uint8_t data = eeprom_read_byte(address+i);
        if (data != 0xFF) {
            *p = data;
        }
    }
 

    return 1;
}

int load_settings_from_eeprom(TxSettings *txs)
{
    uint8_t *address = PROFILE_EEPROM_SETTINGS_OFFSET;

    uint16_t header = eeprom_read_word((uint16_t*)address);
    if ((header & PROFILE_SAVEBLOCK_HEADER_MASK) != PROFILE_SAVEBLOCK_HEADER) {
        // no settings written before
        eeprom_update_word((uint16_t*)address, PROFILE_SAVEBLOCK_HEADER);
        eeprom_update_block(&DefaultSettings, address+2, sizeof(TxSettings));
        *txs = DefaultSettings;
        return 1;
    }

    eeprom_read_block(txs, address+2, sizeof(TxSettings));
    return 1;
}

int save_settings_to_eeprom(TxSettings *txs)
{
    uint8_t *address = PROFILE_EEPROM_SETTINGS_OFFSET;
    eeprom_update_word((uint16_t*)address, PROFILE_SAVEBLOCK_HEADER);
    eeprom_update_block(txs, address+2, sizeof(TxSettings));
}


void write_settings_packet(uint8_t *packet, TxProfile *txp)
{
    uint8_t i;

    packet[0] = 85;
    packet[1] = 255;

    packet[2] = (txp->stick_mode<<4) | (txp->tx_mode&0x0F);
    packet[3] = txp->reversed;

    //6 bytes: of dual rate
    memcpy(packet+4, &(txp->dual_rate), 6);

    //3 bytes: swash AFR
    memcpy(packet+10, &(txp->swash), 3);

    //12 bytes: endpoints
    memcpy(packet+13, &(txp->endpoints), 12);

    //20 bytes: throttle curve/pitch curve
    memcpy(packet+24, &(txp->throttle_curve), 10);
    memcpy(packet+34, &(txp->pitch_curve), 10);

    //6 bytes: subtrim
    memcpy(packet+45, &(txp->subtrim), 6);

    //12 bytes: mixer
    for (i=0; i<3; i++) {
        packet[51+i*4] = (txp->mixers[i].src << 4) | (txp->mixers[i].dest);
        packet[52+i*4] = txp->mixers[i].up_rate;
        packet[53+i*4] = txp->mixers[i].down_rate;
        packet[54+i*4] = txp->mixers[i].sw;
    }

    //4 bytes: switches/variables
    packet[63] = txp->switch_a;
    packet[64] = txp->switch_b;
    packet[65] = 1; // force on
    packet[66] = 1; // force on

    //2 bytes: checksum
    uint16_t chksum=0;
    for (i=2; i < 67; i++) chksum += packet[i];
    packet[67] = (chksum & 0xFF00) >> 8;
    packet[68] = chksum & 0x00FF;

}

uint8_t read_settings_packet(TxProfile *txp, uint8_t *packet)
{
    // if (*(uint16_t*)packet != 0x55FC) 
    //     return 0;

    uint8_t i;
    uint16_t chksum = (packet[65]<<8) | packet[66];
    for (i=0; i < 65; i++) chksum -= packet[i];
    if (chksum)
        return 0;


    txp->stick_mode = (packet[0] & 0xF0) >> 4;
    txp->tx_mode = (packet[0] & 0x0F);
    txp->reversed = packet[1];

    //6 bytes: of dual rate
    memcpy(&(txp->dual_rate), packet+2, 6);

    //3 bytes: swash AFR
    memcpy(&(txp->swash), packet+8, 6);

    //12 bytes: endpoints
    memcpy(&(txp->endpoints), packet+11, 12);

    //20 bytes: throttle curve/pitch curve
    memcpy(&(txp->throttle_curve), packet+23, 10);
    memcpy(&(txp->pitch_curve), packet+33, 10);

    //6 bytes: subtrim
    memcpy(&(txp->subtrim), packet+43, 6);

    //12 bytes: mixer
    for (i=0; i<3; i++) {
        txp->mixers[i].src = (packet[49+i*4] & 0xF0) >> 4;
        txp->mixers[i].dest = packet[49+i*4] & 0x0F;
        txp->mixers[i].up_rate = packet[50+i*4];
        txp->mixers[i].down_rate = packet[51+i*4];
        txp->mixers[i].sw = packet[52+i*4];
    }

    //4 bytes: switches/variables
    txp->switch_a = packet[61];
    txp->switch_b = packet[62];

    return 1;
}