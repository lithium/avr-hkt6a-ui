#include "txprofile.h"


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