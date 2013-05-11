#ifndef TX_PROFILE_H
#define TX_PROFILE_H
#include <avr/io.h>
#include <avr/eeprom.h>
#include <string.h>


#define STICK_MODE1 0
#define STICK_MODE2 1
#define STICK_MODE3 2
#define STICK_MODE4 3

#define SWITCH_FUNC_NULL 0
#define SWITCH_FUNC_DUAL_RATE 1
#define SWITCH_FUNC_THROTTLE_CUT 2
#define SWITCH_FUNC_NORMAL_IDLE 3

#define MIXER_SWITCH_A 0
#define MIXER_SWITCH_B 1
#define MIXER_SWITCH_ON 2
#define MIXER_SWITCH_OFF 3

#define NUM_CHANNELS 6
#define NUM_MIXERS 3
typedef struct {
    char name[12];
    uint8_t stick_mode;
    uint8_t reversed;
    struct {
        uint8_t on;
        uint8_t off;
    } dual_rate[3];
    struct {
        uint8_t ep1;
        uint8_t ep2;
    } endpoints[NUM_CHANNELS];
    int8_t subtrim[NUM_CHANNELS];
    uint8_t switch_a;
    uint8_t switch_b;
    struct TxMixer {
        uint8_t src;
        uint8_t dest;
        int8_t up_rate;
        int8_t down_rate;
        uint8_t sw;
    } mixers[NUM_MIXERS];
} TxProfile;

typedef struct {
    uint8_t cur_profile;
} TxSettings;


extern TxProfile DefaultProfile;
extern TxSettings DefaultSettings;





#define PROFILE_MAX_COUNT 6


#define PROFILE_EEPROM_SAVEBLOCK_OFFSET 32
#define PROFILE_EEPROM_SETTINGS_OFFSET 0


#define PROFILE_SAVEBLOCK_SIZE (sizeof(TxProfile)+2)  // 2 bytes for header
#define PROFILE_SAVEBLOCK_HEADER_MASK  0xFFF0
#define PROFILE_SAVEBLOCK_FLAG_MASK    0x000F
#define PROFILE_SAVEBLOCK_HEADER       0xADC0       //101011011100


//profile flags
#define PROFILE_FLAG_MIX1 0
#define PROFILE_FLAG_MIX2 1
#define PROFILE_FLAG_MIX3 2
#define PROFILE_FLAG_DR 3
#define PROFILE_FLAG_TC 4
typedef struct {
    char name[12];
    uint8_t reversed;
    uint8_t profile_flags;
} TxProfileCache;

int load_settings_from_eeprom(TxSettings *txs);
int save_settings_to_eeprom(TxSettings *txs);

void init_profile_cache(TxProfileCache *adapter, uint8_t size);
int update_profile_cache_from_eeprom(uint8_t profile_id, TxProfileCache *cache);
void save_profile_to_eeprom(uint8_t profile_id, TxProfile *txp);
int load_profile_from_eeprom(uint8_t profile_id, TxProfile *txp);

void profile_change(uint8_t profile_id);
void force_clean_eeprom(uint8_t size);

int update_profile_cache(uint8_t profile_id, TxProfile *txp);

#endif
