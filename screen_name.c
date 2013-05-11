#include "screen.h"
#include "lcd.h"


extern uint8_t g_CurProfile;

void screen_name_setup(Screen *scr, TxProfile *txp)
{
    lcd_clear();
    lcd_printfxy(0,0,"#%d:", g_CurProfile+1);
    lcd_printfxy(0,1,"MODE:");

    lcd_display(0b101); // cursor off but blinking

}
void screen_name_destroy(Screen *scr, TxProfile *txp)
{

}
void screen_name_paint(Screen *scr, TxProfile *txp)
{
    lcd_printfxy(4,0,txp->name);

    lcd_printfxy(6,1, "%x", txp->stick_mode);
}
void screen_name_event(Screen *scr, TxProfile *txp, Event *e)
{

}
