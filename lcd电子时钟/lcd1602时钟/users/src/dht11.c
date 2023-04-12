#include "common.h"

bit dht11_init(void)
{
    static uchar delayCount = 0;
    DHT11_DAT = 0;
    //delay_ms(20);
    delayCount++;

    if (delayCount <= 4) 
        return (bit)0;

    delayCount = 0;
    DHT11_DAT = 1;
    delay_us(3);

    if(DHT11_DAT == 0)
    {
        while(!DHT11_DAT);
        while(DHT11_DAT);
        return (bit)1;
    }

    return (bit)0;
}

bit dht11_read_bit(void)
{
    while(!DHT11_DAT);
    delay_us(5);
    if (DHT11_DAT)
    {
        while(DHT11_DAT);
        return (bit)1;
    }
    else
        return (bit)0;
}

uchar dht11_read_byte(void)
{
    uchar dat = 0x00;
    uchar i = 0;

    for (i = 0; i < 8; i++)
    {
        dat <<= 1;
        dat = dat | dht11_read_bit();
    }

    return dat;
}

void dht11_read_dat(Environment* information)
{
    uchar i = 0;
    uchar check_sum, check;
    uchar dht11_buf[4] = {0};

    if (dht11_init())
    {
        dht11_buf[0] = dht11_read_byte();
        dht11_buf[1] = dht11_read_byte();
        dht11_buf[2] = dht11_read_byte();
        dht11_buf[4] = dht11_read_byte();
        check = dht11_read_byte();
        check_sum = dht11_buf[0] + dht11_buf[1] + dht11_buf[2] + dht11_buf[3];

        if (check_sum == check)
        {
            information->humidity[0] = dht11_buf[0];
            information->humidity[1] = dht11_buf[1];
            information->temperature[0] = dht11_buf[2];
            information->temperature[1] = dht11_buf[3];
        }
        else
        {
            information->humidity[0] = 89;
            information->humidity[1] = 98;
            information->temperature[0] = 95;
            information->temperature[1] = 59;
        }
    }
}