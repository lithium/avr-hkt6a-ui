#include "screen.h"

extern Screen g_Screen;
extern TxProfile g_Profile;

void screen_change(uint8_t scr_id)
{
    memset(&g_Screen, 0, sizeof(Screen));
    memcpy(&(g_Screen.vtable), &ScreenTable[scr_id], sizeof(Screen_vtable));
    g_Screen.is_dirty = 1;
    g_Screen.vtable.setup_func(&g_Screen, &g_Profile);
}
