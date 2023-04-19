#include <reg52.h>
#include "common.h"

Environment gEnvironment;
Environment gAlarmClockTime;
bit gSettingTime = 0;
bit gSettingDate = 0;
bit gAlarmClockTimeUp = 0;
uchar gLcd1602CurSettingRow = 0;
uchar gLcd1602CurSettingCol = 0;
uchar gLcd1602SettingTicket = 0;
uchar gLedRunTicket = 0;
uchar gLedRunTime = 0;
uchar gSettingMode = eSetting_null;
uchar gDispMode = eDisplayMode_dateTime;

void init();
void init_data();
void run();
void led_run();
void timer0_init();
void setDS1302Year();
void setDS1302Mon();
void setDS1302Day();
void setDS1302Hour();
void setDS1302Min();
void setDS1302Sec();
void setAlarmClockHour();
void setAlarmClockMin();
void setAlarmClockSec();
void key1_action();
void key2_action();
void key3_action();
void check_setting_mode();
void check_alarm_clock();

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
    init_data();
    timer0_init();
    ds1302_init();
    lcd1602_init();

    key_register(0, key1_action);
    key_register(1, key2_action);
    key_register(2, key3_action);
}

void init_data()
{
    gEnvironment.year = 23;
    gEnvironment.mon = 4;
    gEnvironment.day = 10;
    gEnvironment.hour = 22;
    gEnvironment.min = 40;
    gEnvironment.sec = 50;
    gEnvironment.temperature[0] = 25;
    gEnvironment.temperature[1] = 0;
    gEnvironment.humidity[0] = 65;
    gEnvironment.humidity[1] = 0;

    gAlarmClockTime.hour = 13;
    gAlarmClockTime.min = 25;
    gAlarmClockTime.sec = 30;
}

void led_run()
{
    if (gAlarmClockTimeUp)
    {
        gLedRunTime++;
        gLedRunTicket++;

        if (gLedRunTicket == 100) //500ms
        {
            gLedRunTicket = 0;
            LED_RUN = ~LED_RUN;
        }

        if (gLedRunTime == 1200)
        {
            gAlarmClockTimeUp = 0;
            gLedRunTime = 0;
            gLedRunTicket = 0;
        }
    }
}

void run()
{
    key_run();
    led_run();
    check_setting_mode();
    check_alarm_clock();

    if (gSettingMode == eSetting_null)
    {
        gLcd1602SettingTicket = 0;
        ds1302_read_time(&gEnvironment);
        dht11_read_dat(&gEnvironment);

        if (gDispMode == eDisplayMode_alarm_clock) 
            lcd1602_display(gDispMode, gAlarmClockTime);
        else 
            lcd1602_display(gDispMode, gEnvironment);
    }
    else
    {
        gLcd1602SettingTicket++;
        if (gLcd1602SettingTicket < 50) 
        {
            if (gDispMode == eDisplayMode_alarm_clock)
                lcd1602_display_setting(gAlarmClockTime, gSettingMode);
            else
                lcd1602_display_setting(gEnvironment, gSettingMode);
        }   
        else if (gLcd1602SettingTicket >= 50 && gLcd1602SettingTicket < 100) 
        {
            if (gDispMode == eDisplayMode_alarm_clock) 
                lcd1602_display(gDispMode, gAlarmClockTime);
            else 
                lcd1602_display(gDispMode, gEnvironment);
            }
        else
            gLcd1602SettingTicket = 0;
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

void setDS1302Year()
{
    gSettingDate = 1;
    gEnvironment.year++;

    if (gEnvironment.year > 99) 
        gEnvironment.year = 0;
}

void setDS1302Mon()
{
    gSettingDate = 1;
    gEnvironment.mon++;

    if (gEnvironment.mon > 12)
        gEnvironment.mon = 1;
}

void setDS1302Day()
{
    gSettingDate = 1;
    gEnvironment.day++;

    if (gEnvironment.day > 31)
        gEnvironment.day = 1;
}

void setDS1302Hour()
{
    gSettingTime = 1;
    gEnvironment.hour++;

    if (gEnvironment.hour > 23)
        gEnvironment.hour = 0;
}

void setDS1302Min()
{
    gSettingTime = 1;
    gEnvironment.min++;

    if (gEnvironment.min > 59)
        gEnvironment.min = 0;
}

void setDS1302Sec()
{
    gSettingTime = 1;
    gEnvironment.sec++;

    if (gEnvironment.sec > 59)
        gEnvironment.sec = 0;
}

void setAlarmClockHour()
{
    gAlarmClockTime.hour++;
    if (gAlarmClockTime.hour > 23)
        gAlarmClockTime.hour = 0;
}

void setAlarmClockMin()
{
    gAlarmClockTime.min++;
    if (gAlarmClockTime.min > 59)
        gAlarmClockTime.min = 0;
}

void setAlarmClockSec()
{
    gAlarmClockTime.sec++;
    if (gAlarmClockTime.sec > 59)
        gAlarmClockTime.sec = 0;
}

void key1_action()
{
    switch (gDispMode)
    {
        case eDisplayMode_dateTime:
        {
            gLcd1602CurSettingCol++;
            if (gLcd1602CurSettingCol > 3)
            {
                gLcd1602CurSettingCol = 1;
                gLcd1602CurSettingRow++;

                if (gLcd1602CurSettingRow > 1)
                    gLcd1602CurSettingRow = 0;
            } 

            break;       
        }
        case eDisplayMode_alarm_clock:
        {
            gLcd1602CurSettingRow = 1;
            gLcd1602CurSettingCol++;

            if (gLcd1602CurSettingCol > 3)
                gLcd1602CurSettingCol = 1;

            break;
        }
    }
}

void key2_action()
{
    switch (gDispMode)
    {
        case eDisplayMode_dateTime: 
        {
            if (gLcd1602CurSettingRow == 0 && gLcd1602CurSettingCol == 1) setDS1302Year();
            if (gLcd1602CurSettingRow == 0 && gLcd1602CurSettingCol == 2) setDS1302Mon(); 
            if (gLcd1602CurSettingRow == 0 && gLcd1602CurSettingCol == 3) setDS1302Day();
            if (gLcd1602CurSettingRow == 1 && gLcd1602CurSettingCol == 1) setDS1302Hour();
            if (gLcd1602CurSettingRow == 1 && gLcd1602CurSettingCol == 2) setDS1302Min();
            if (gLcd1602CurSettingRow == 1 && gLcd1602CurSettingCol == 3) setDS1302Sec();   
            return;
        }
        case eDisplayMode_alarm_clock: 
        {
            if (gLcd1602CurSettingRow == 1 && gLcd1602CurSettingCol == 1) setAlarmClockHour();
            if (gLcd1602CurSettingRow == 1 && gLcd1602CurSettingCol == 2) setAlarmClockMin();
            if (gLcd1602CurSettingRow == 1 && gLcd1602CurSettingCol == 3) setAlarmClockSec();
            return; 
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
    gSettingMode = eSetting_null;
    switch (gDispMode) 
    {
        case eDisplayMode_dateTime:
        {
            if (gLcd1602CurSettingRow == 0 && gLcd1602CurSettingCol == 1) gSettingMode = eSetting_year;
            if (gLcd1602CurSettingRow == 0 && gLcd1602CurSettingCol == 2) gSettingMode = eSetting_mon; 
            if (gLcd1602CurSettingRow == 0 && gLcd1602CurSettingCol == 3) gSettingMode = eSetting_day;
            if (gLcd1602CurSettingRow == 1 && gLcd1602CurSettingCol == 1) gSettingMode = eSetting_hour;
            if (gLcd1602CurSettingRow == 1 && gLcd1602CurSettingCol == 2) gSettingMode = eSetting_min;
            if (gLcd1602CurSettingRow == 1 && gLcd1602CurSettingCol == 3) gSettingMode = eSetting_sec;
            return;
        }
        case eDisplayMode_alarm_clock:
        {
            if (gLcd1602CurSettingRow == 1 && gLcd1602CurSettingCol == 1) gSettingMode = eSetting_hour;
            if (gLcd1602CurSettingRow == 1 && gLcd1602CurSettingCol == 2) gSettingMode = eSetting_min;
            if (gLcd1602CurSettingRow == 1 && gLcd1602CurSettingCol == 3) gSettingMode = eSetting_sec;
            return; 
        }       
    }
}

void check_alarm_clock()
{
    if (gEnvironment.hour == gAlarmClockTime.hour && 
    gEnvironment.min == gAlarmClockTime.min && 
    gEnvironment.sec == gAlarmClockTime.sec)
    {
        gAlarmClockTimeUp = 1;
        gLedRunTicket = 0;
        gLedRunTime = 0;
    }
}
