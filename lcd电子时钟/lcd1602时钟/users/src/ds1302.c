#include "common.h"

void ds1302_init()
{
    DS1302_CE = 0;
    DS1302_SCLK = 0;
    DS1302_IO = 0;
}

void ds1302_write_bytes(uchar addr, uchar write_data)
{
    uchar i = 0;
    DS1302_CE = 0;
    _nop_();
    DS1302_CE = 1;
    _nop_();
    DS1302_SCLK = 0;
    _nop_();
    DS1302_IO = 0;
    _nop_();
    
    for(i = 0; i < 8; i++)
    {
        DS1302_IO = addr & (0x01 << i);
        DS1302_SCLK = 1;
        _nop_();
        DS1302_SCLK = 0;
        _nop_();
    }

    for (i = 0; i < 8; i++)
    {
        DS1302_IO = write_data & (0x01 << i);
        DS1302_SCLK = 1;
        _nop_();
        DS1302_SCLK = 0;
        _nop_();
    }

    DS1302_CE = 0;
    _nop_();
}

uchar ds1302_read_bytes(uchar addr)
{
    uchar readData = 0x00;
    uchar i = 0;
    addr = addr | 0x01; //转换成读指令
    DS1302_CE = 0;
    _nop_();
    DS1302_CE = 1;
    _nop_();
    DS1302_SCLK = 0;
    _nop_();
    DS1302_IO = 0;
    _nop_();

    for (i = 0; i < 8; i++)
    {
        DS1302_IO = addr & (0x01 << i);
        DS1302_SCLK = 0;
        _nop_();
        DS1302_SCLK = 1;
        _nop_();
    }

    for (i = 0; i < 8; i++)
    {
        DS1302_SCLK = 1;
        _nop_();
        DS1302_SCLK = 0;
        _nop_();

        if (DS1302_IO) {readData = readData | (0x01 << i);}
    }

    DS1302_CE = 0;
     _nop_();
    DS1302_IO = 0;
     _nop_();
    return readData;
}

void ds1302_set_time(enum DS1302_SetMode mode, Environment* information)
{
    ds1302_write_bytes(DS1302_WP, 0x00);
    switch (mode)
    {
        case eDs1302_set_all:
        {
            ds1302_write_bytes(DS1302_YEAR, dec_to_bcd(information->year));
            ds1302_write_bytes(DS1302_MONTH, dec_to_bcd(information->mon));
            ds1302_write_bytes(DS1302_DATE, dec_to_bcd(information->day));
            ds1302_write_bytes(DS1302_HOUR, dec_to_bcd(information->hour));
            ds1302_write_bytes(DS1302_MINUTE, dec_to_bcd(information->min));
            ds1302_write_bytes(DS1302_SECOND, dec_to_bcd(information->sec));
            break;
        }
        case eDs1302_Set_ymd:
        {
            ds1302_write_bytes(DS1302_YEAR, dec_to_bcd(information->year));
            ds1302_write_bytes(DS1302_MONTH, dec_to_bcd(information->mon));
            ds1302_write_bytes(DS1302_DATE, dec_to_bcd(information->day));
            break;
        }
        case eDs1302_Set_hms:
        {
            ds1302_write_bytes(DS1302_HOUR, dec_to_bcd(information->hour));
            ds1302_write_bytes(DS1302_MINUTE, dec_to_bcd(information->min));
            ds1302_write_bytes(DS1302_SECOND, dec_to_bcd(information->sec));
            break;
        }
    }

    ds1302_write_bytes(DS1302_WP, 0x80);
}

void ds1302_read_time(Environment* information)
{
    information->year = bcd_to_dec(ds1302_read_bytes(DS1302_YEAR));
    information->mon = bcd_to_dec(ds1302_read_bytes(DS1302_MONTH));
    information->day = bcd_to_dec(ds1302_read_bytes(DS1302_DATE));
    information->hour = bcd_to_dec(ds1302_read_bytes(DS1302_HOUR));
    information->min = bcd_to_dec(ds1302_read_bytes(DS1302_MINUTE));
    information->sec = bcd_to_dec(ds1302_read_bytes(DS1302_SECOND));
}
