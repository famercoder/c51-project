#ifndef __COMMON_H
#define __COMMON_H

#include <reg52.h>
#include <INTRINS.H>
#include <stdio.h>
#include <stdlib.h>
#include <STRING.H>

typedef unsigned char uchar;
typedef unsigned int uint;
typedef void (*KeyActionFunc)();

typedef struct {
    uchar year;
    uchar mon;
    uchar day;
    uchar hour;
    uchar min;
    uchar sec;
    uchar temperature[2];
    uchar humidity[2];
}Environment;

enum DisplayMode {
    eDisplayMode_time_date,
    eDisplayMode_tphumi,
    eDisplayMode_max,
};

enum DS1302_SetMode {
    eDs1302_Set_hms,
    eDs1302_Set_ymd,
    eDs1302_set_all,
};

#define DS1302_SECOND		0x80
#define DS1302_MINUTE		0x82
#define DS1302_HOUR			0x84
#define DS1302_DATE			0x86
#define DS1302_MONTH		0x88
#define DS1302_DAY			0x8A
#define DS1302_YEAR			0x8C
#define DS1302_WP			0x8E

#define LCD1602_WRITE_COMMAND 0
#define LCD1602_WRITE_DATA    1

/////////////////////////////////
//DHT11
sbit DHT11_DAT      = P2^0;
//key
sbit KEY_1          = P2^1;
sbit KEY_2          = P2^2;
sbit KEY_3          = P2^3;
//led
sbit LED_RUN        = P2^4;
//lcd1602
sbit LCD1602_RW         = P2^5; 
sbit LCD1602_RS         = P2^6; 
sbit LCD1602_E          = P2^7;

//ds1302
sbit DS1302_CE      = P1^0;
sbit DS1302_SCLK    = P1^1;
sbit DS1302_IO      = P1^2;

//lcd1602
sbit LCD1602_D4         = P0^4;
sbit LCD1602_D5         = P0^5;
sbit LCD1602_D6         = P0^6;
sbit LCD1602_D7         = P0^7;

////////////////////////////////
//ds1302
static void    ds1302_init();
static void    ds1302_write_bytes(uchar addr, uchar write_data);
static uchar   ds1302_read_bytes(uchar addr);
static void    ds1302_set_time(enum DS1302_SetMode mode, Environment* information);
static void    ds1302_read_time(Environment* information);

//key
static void    key_register(uchar key_id, KeyActionFunc action);
static void    key_action();
static void    key_scan();
static void    key_run();

//other
static uchar   bcd_to_dec(uchar source_data); //bcd -- dec
static uchar   dec_to_bcd(uchar source_data); //dec -- bcd
static void    delay_us(uint n);
static void    delay_ms(uint c);

//dth11
static bit     dht11_init(void);
static bit     dht11_read_bit(void);
static uchar   dht11_read_byte(void);
static void    dht11_read_dat(Environment* information);

//lcd1602
static void lcd1602_init();
static void lcd1602_write4bits(uchar bits, bit mode);
static void lcd1602_setcursor(uchar row, uchar col);
static void lcd1602_message(char* message);
static void lcd1602_clear();
static void lcd1602_pulseenable();
static bit  lcd1602_busy();
static void lcd1602_display_time(uchar row, uchar col, Environment* information);
static void lcd1602_display_date(uchar row, uchar col, Environment* information);
static void lcd1602_display_temperature(uchar row, uchar col, Environment* information);
static void lcd1602_display_humidity(uchar row, uchar col, Environment* information);
static void lcd1602_display(enum DisplayMode dispMode, Environment* information);

#endif