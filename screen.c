#include "screen.h"
#include "global.h"


void screen_change(uint8_t scr_id)
{
    if (g_Screen.vtable.destroy_func) {
        g_Screen.vtable.destroy_func(&g_Screen, &g_Profile);
    }
    memset(&g_Screen, 0, sizeof(Screen));
    memcpy(&(g_Screen.vtable), &ScreenTable[scr_id], sizeof(Screen_vtable));
    g_Screen.is_dirty = 1;
    g_Screen.vtable.setup_func(&g_Screen, &g_Profile);
}


void save_or_abort()
{
    if (IS_PROFILE_DIRTY()) {
        screen_change(SCREEN_SAVE);
        CLEAR_PROFILE_DIRTY();
    }
    else {
        screen_change(SCREEN_MAIN);
    }
}
