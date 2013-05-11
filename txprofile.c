#include "txprofile.h"
#include "lcd.h"
#include "global.h"


TxProfile DefaultProfile = {
    "            ", //12 bytes
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
    },
    {(int8_t)0,(int8_t)0,(int8_t)0,(int8_t)0,(int8_t)0,(int8_t)0}, //subtrim
    (uint8_t)1, //swa - dualrate
    (uint8_t)2, //swb - throttlecut
    {
        {(uint8_t)0,(uint8_t)0,(int8_t)100,(int8_t)100,(uint8_t)3},
        {(uint8_t)0,(uint8_t)0,(int8_t)100,(int8_t)100,(uint8_t)3},
        {(uint8_t)0,(uint8_t)0,(int8_t)100,(int8_t)100,(uint8_t)3},
    }, // 3mixers off by default
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

