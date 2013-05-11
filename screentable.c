#include "screen.h"


#include "screen_name.c"
#include "screen_epa.c"
#include "screen_switches.c"

Screen_vtable ScreenTable[] = {
    {&screen_name_setup, &screen_name_paint, &screen_name_event, &screen_name_destroy},
    {&screen_epa_setup, &screen_epa_paint, &screen_epa_event, &screen_epa_destroy},
    {&screen_switches_setup, &screen_switches_paint, &screen_switches_event, &screen_switches_destroy},
};
const uint8_t ScreenTableSize = sizeof(ScreenTable)/sizeof(Screen_vtable);
