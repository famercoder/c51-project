#include <reg52.h>
#include "common.h"

Environment gEnvironment;
uchar gDispMode = eDisplayMode_time_date;

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
    ds1302_read_time(&gEnvironment);
    dht11_read_dat(&gEnvironment);
    lcd1602_display(gDispMode, &gEnvironment);

    ticketCount++;
    if (ticketCount == 100)
    {
        ticketCount = 0;
        LED_RUN = ~LED_RUN;
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
}

void key2_action()
{
}

void key3_action()
{
    gDispMode++;
    if (gDispMode >= eDisplayMode_max) 
        gDispMode = eDisplayMode_time_date;
}