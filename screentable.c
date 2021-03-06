#include "screen.h"


#include "screen_main.c"
#include "screen_name.c"
#include "screen_epa.c"
#include "screen_switches.c"
#include "screen_mixes.c"
#include "screen_profiles.c"
#include "screen_save.c"
#include "screen_channels.c"
#include "screen_type.c"
#include "screen_curve.c"

Screen_vtable ScreenTable[] = {
    {&screen_main_setup, &screen_main_paint, &screen_main_event, &screen_main_destroy},
    {&screen_name_setup, &screen_name_paint, &screen_name_event, &screen_name_destroy},
    {&screen_epa_setup, &screen_epa_paint, &screen_epa_event, &screen_epa_destroy},
    {&screen_switches_setup, &screen_switches_paint, &screen_switches_event, &screen_switches_destroy},
    {&screen_mixes_setup, &screen_mixes_paint, &screen_mixes_event, &screen_mixes_destroy},
    {&screen_profiles_setup, &screen_profiles_paint, &screen_profiles_event, &screen_profiles_destroy},
    {&screen_save_setup, &screen_save_paint, &screen_save_event, &screen_save_destroy},
    {&screen_channels_setup, &screen_channels_paint, &screen_channels_event, &screen_channels_destroy},
    {&screen_type_setup, &screen_type_paint, &screen_type_event, &screen_type_destroy},
    {&screen_curve_setup, &screen_curve_paint, &screen_curve_event, &screen_curve_destroy},
};
const uint8_t ScreenTableSize = sizeof(ScreenTable)/sizeof(Screen_vtable);
