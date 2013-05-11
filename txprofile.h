#ifndef TX_PROFILE_H
#define TX_PROFILE_H


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
    } dual_rate[NUM_CHANNELS];
    struct {
        uint8_t ep1;
        uint8_t ep2;
    } endpoints[NUM_CHANNELS];
    int8_t subtrim[NUM_CHANNELS];
    uint8_t switch_a;
    uint8_t switch_b;
    struct {
        uint8_t src;
        uint8_t dest;
        int8_t up_rate;
        int8_t down_rate;
        uint8_t sw;
    } mixers[NUM_MIXERS];
} TxProfile;


extern TxProfile g_CurrentProfile;

#endif
