#include <reg52.h>
#include "common.h"

Environment gEnvironment;
uchar gDispMode = eDisplayMode_dateTime;
bit gSettingTime = 0;
bit gSettingDate = 0;
uchar gLcd1602CurSettingRow = 0;
uchar gLcd1602CurSettingCol = 0;
uchar gLedRunTicket = 0;
uchar gLcd1602SettingTicket = 0;
uchar gSettingMode = eSetting_null;

void init();
void run();
void led_run();
void timer0_init();
void key1_action();
void key2_action();
void key3_action();
void check_setting_mode();

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

void led_run()
{
    gLedRunTicket++;
    if (gLedRunTicket == 100) //500ms
    {
        gLedRunTicket = 0;
       // LED_RUN = ~LED_RUN;
    }
}

void run()
{
    key_run();
    led_run();
    check_setting_mode();

    if (gSettingMode == eSetting_null)
    {
        gLcd1602SettingTicket = 0;
        ds1302_read_time(&gEnvironment);
        dht11_read_dat(&gEnvironment);
        lcd1602_display(gDispMode, gEnvironment);
    }
    else
    {
        gLcd1602SettingTicket++;
        if (gLcd1602SettingTicket == 100)
        {
            gLcd1602SettingTicket = 0;
            lcd1602_display_setting(gEnvironment, gSettingMode);
        }
        else
        {
            lcd1602_display(gDispMode, gEnvironment);
        }
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
    if (gDispMode != eDisplayMode_dateTime) 
        return;
    
    gLcd1602CurSettingCol++;
    if (gLcd1602CurSettingCol > 3)
    {
        gLcd1602CurSettingCol = 1;
        gLcd1602CurSettingRow++;

        if (gLcd1602CurSettingRow > 1)
            gLcd1602CurSettingRow = 0;
    }
}

void key2_action()
{
    LED_RUN = ~LED_RUN;
    switch (gLcd1602CurSettingCol)
    {
        case 1:
        {   
            if (gLcd1602CurSettingRow == 0)    
            {
                gEnvironment.year++;
                if (gEnvironment.year > 99) gEnvironment.year = 0;
            }
            else if (gLcd1602CurSettingRow == 1)
            {
                gEnvironment.hour++;
                if (gEnvironment.hour > 23) gEnvironment.hour = 0;
            }

            break;
        }
        case 2:
        {  
            if (gLcd1602CurSettingRow == 0)    
            {
                gEnvironment.mon++;
                if (gEnvironment.mon > 12) gEnvironment.mon = 1;
            }
            else if (gLcd1602CurSettingRow == 1)
            {
                gEnvironment.min++;
                if (gEnvironment.min > 59) gEnvironment.min = 0;
            }

            break;
        }
        case 3:
        {
            if (gLcd1602CurSettingRow == 0)    
            {
                gEnvironment.day++;
                if (gEnvironment.day > 31) gEnvironment.day = 1;
            }
            else if (gLcd1602CurSettingRow == 1)
            {
                gEnvironment.sec++;
                if (gEnvironment.sec > 59) gEnvironment.sec = 0;
            }

            break;
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
    gLcd1602CurSettingCol = 0;
}

void check_setting_mode()
{
    if (gLcd1602CurSettingRow == 0) 
    {
        if (gLcd1602CurSettingCol == 1)
        {
            gSettingMode = eSetting_year;
            return;
        }

        if (gLcd1602CurSettingCol == 2)
        {
            gSettingMode = eSetting_mon;
            return;
        }

        if (gLcd1602CurSettingCol == 3)
        {
            gSettingMode = eSetting_day;
            return;
        }
    }

    if (gLcd1602CurSettingRow == 1) 
    {
        if (gLcd1602CurSettingCol == 1)
        {
            gSettingMode = eSetting_hour;
            return;
        }

        if (gLcd1602CurSettingCol == 2)
        {
            gSettingMode = eSetting_min;
            return;
        }

        if (gLcd1602CurSettingCol == 3)
        {
            gSettingMode = eSetting_sec;
            return;
        }
    } 

    gSettingMode = eSetting_null;
}
