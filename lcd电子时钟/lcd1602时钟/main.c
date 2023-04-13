#include <reg52.h>
#include "common.h"

Environment gEnvironment;
uchar gDispMode = eDisplayMode_dateTime;
uchar gSettingTime = 0;
uchar gSettingDate = 0;
uchar gLcd1602CurSettingRow = 0;
char  gLcd1602CurSettingColPos = -1;
uchar gLcd1602SettingCols[] = {5,6, 8,9, 11,12};
uchar gKey2SettingValue = 0;

void init();
void run();
void timer0_init();
void key1_action();
void key2_action();
void key3_action();

void main()
{
    init();
    while(1)
    {
        while(!TF0);
        run();
    }
}

void init()
{
    gEnvironment.year = 23;
    gEnvironment.mon = 4;
    gEnvironment.day = 10;
    gEnvironment.hour = 22;
    gEnvironment.min = 40;
    gEnvironment.sec = 50;
    gEnvironment.temperature[0] = 25,
    gEnvironment.temperature[1] = 0,
    gEnvironment.humidity[0] = 65,
    gEnvironment.humidity[1] = 0,

    timer0_init();
    ds1302_init();
    lcd1602_init();

    key_register(0, key1_action);
    key_register(1, key2_action);
    key_register(2, key3_action);
}

void run()
{
    static uint ticketCount = 0;
    key_run();

    if (gLcd1602CurSettingColPos == -1)
        ds1302_read_time(&gEnvironment); 

    dht11_read_dat(&gEnvironment);
    lcd1602_display(gDispMode, &gEnvironment);

    ticketCount++;
    if (ticketCount == 100)
    {
        ticketCount = 0;
        //LED_RUN = ~LED_RUN;
    }
}

void timer0_init()
{
    TMOD = 0x01;
    TH0 = (65536 - 5000) / 256;
    TL0 = (65536 - 5000) % 256;
    TF0 = 0;
    TR0 = 1;
    ET0 = 1;
    EA = 1;
}

void timer0() interrupt 1
{
    TH0 = (65536 - 5000) / 256;
    TL0 = (65536 - 5000) % 256;
}

void key1_action()
{
    if (gDispMode == eDisplayMode_dateTime) 
    {
        gLcd1602CurSettingColPos++;
        if (gLcd1602CurSettingColPos >= sizeof(gLcd1602SettingCols) / sizeof(uchar))
        {
            gLcd1602CurSettingColPos = 0;
            gLcd1602CurSettingRow++;

            if (gLcd1602CurSettingRow > 1)
                gLcd1602CurSettingRow = 0;
        }

        lcd1602_open_gb(gLcd1602CurSettingRow, gLcd1602SettingCols[gLcd1602CurSettingColPos]);
    }
}

void key2_action()
{
    if (gLcd1602CurSettingRow == 0) 
    {
        switch (gLcd1602CurSettingColPos)
        {
            case 0:
            {
                gKey2SettingValue++;
                if (gKey2SettingValue >= 10)
                    gKey2SettingValue = 0;
                
                lcd1602_display_char(0, 5, gKey2SettingValue);
                LED_RUN = ~LED_RUN;
                break;
            }
            case 1:
            {  
                break;
            }
            case 2:
            {
                break;
            }
        }
    }
    else if (gLcd1602CurSettingRow == 1)
    {
        switch (gLcd1602CurSettingColPos)
        {
            case 0:
            {
                break;
            }
            case 1:
            {
                break;
            }
            case 2:
            {
                break;
            }
        }
    }
}

void key3_action()
{
    gDispMode++;
    if (gDispMode >= eDisplayMode_max) 
        gDispMode = eDisplayMode_dateTime;

    if (gDispMode == eDisplayMode_tpHumi)
    {
        if (gSettingTime)
        {
            ds1302_set_time(eDs1302_Set_hms, &gEnvironment);
            gSettingTime = 0;
        }

        if (gSettingDate)
        {
            ds1302_set_time(eDs1302_Set_ymd, &gEnvironment);
            gSettingDate = 0;
        }
    }

    gLcd1602CurSettingRow = 0;
    gLcd1602CurSettingColPos = -1;
    lcd1602_close_gb();
}