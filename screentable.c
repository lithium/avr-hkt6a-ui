#include "screen.h"


#include "screen_epa.c"

Screen_vtable ScreenTable[] = {
    // {&mainscreen_setup, &mainscreen_paint, &mainscreen_event},
    {&screen_epa_setup, &screen_epa_paint, &screen_epa_event, &screen_epa_destroy},
};
const uint8_t ScreenTableSize = sizeof(ScreenTable)/sizeof(Screen_vtable);
