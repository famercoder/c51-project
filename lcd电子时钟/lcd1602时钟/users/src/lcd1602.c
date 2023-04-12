#include "common.h"

void lcd1602_init()
{
    LCD1602_RW = 0;
    LCD1602_E = 0;

    lcd1602_write4bits(0x33, LCD1602_WRITE_COMMAND);
    lcd1602_write4bits(0x32, LCD1602_WRITE_COMMAND);
    lcd1602_write4bits(0x28, LCD1602_WRITE_COMMAND);
    lcd1602_write4bits(0x0c, LCD1602_WRITE_COMMAND);
    lcd1602_write4bits(0x06, LCD1602_WRITE_COMMAND);
    lcd1602_clear();

    lcd1602_message("Welcome to --->\n  SunFounder");
    delay_ms(3000);
    lcd1602_clear(); 
}

void lcd1602_write4bits(uchar bits, bit mode)
{
    while(lcd1602_busy());
    LCD1602_RS = mode;
    _nop_();

    P0 &= 0x0f; 
    P0 |= (bits & 0xf0);
    lcd1602_pulseenable();

    P0 &= 0x0f;
    P0 |= (bits << 4) & 0xf0;
    lcd1602_pulseenable();
}

void lcd1602_setcursor(uchar row, uchar col)
{
    uchar offsets[] = {0x00, 0x40, 0x14, 0x54};
    lcd1602_write4bits(0x80 | (col + offsets[row]), LCD1602_WRITE_COMMAND);
}

void lcd1602_message(char* message)
{
    char* p = message;
    while(*p != '\0')
    {
        if (*p == '\n') 
            lcd1602_write4bits(0xc0, LCD1602_WRITE_COMMAND);
        else 
            lcd1602_write4bits(*p, LCD1602_WRITE_DATA);
        p++;
    }
}

void lcd1602_clear()
{
    lcd1602_write4bits(0x01, LCD1602_WRITE_COMMAND);
    delay_ms(5);
}

void lcd1602_pulseenable()
{
    LCD1602_E = 0;
    _nop_();
    LCD1602_E = 1;
    _nop_();
    _nop_();
    LCD1602_E = 0;
    _nop_();
    _nop_();
}

bit  lcd1602_busy()
{
  bit result = 0;
//	LCD1602_RS = 0;
//	LCD1602_RW = 1;
//	LCD1602_E = 1;
//	_nop_();
//	_nop_();
//	_nop_();
//	_nop_();
//	result = (bit)(P0 & 0x80);	 
//	LCD1602_E = 0;
//  LCD1602_RW = 0;
    return result;
}

void lcd1602_display_time(uchar row, uchar col, Environment* information)
{
    char message[] = "Time:xx:xx:xx";
    uchar i = 0, pos = 0;
    uchar displayValues[6] = {0};

    displayValues[0] = information->hour / 10;
    displayValues[1] = information->hour % 10;
    displayValues[2] = information->min / 10;
    displayValues[3] = information->min % 10;
    displayValues[4] = information->sec / 10;
    displayValues[5] = information->sec % 10;

    for (i = 0; i < sizeof(message) / sizeof(char); i++)
    {
        if (message[i] == 'x')
        {
            message[i] = displayValues[pos]+'0';
            pos++;
        }
    }

    lcd1602_setcursor(row, col);
    lcd1602_message(message);
}

void lcd1602_display_date(uchar row, uchar col, Environment* information)
{
    char message[] = "Date:20xx-xx-xx";
    uchar i = 0, pos = 0;
    uchar displayValues[6] = {0};

    displayValues[0] = information->year / 10;
    displayValues[1] = information->year % 10;
    displayValues[2] = information->mon / 10;
    displayValues[3] = information->mon % 10;
    displayValues[4] = information->day / 10;
    displayValues[5] = information->day % 10;

    for (i = 0; i < sizeof(message) / sizeof(char); i++)
    {
        if (message[i] == 'x')
        {
            message[i] = displayValues[pos]+'0';
            pos++;
        }
    }

    lcd1602_setcursor(row, col);
    lcd1602_message(message);  
}

void lcd1602_display_temperature(uchar row, uchar col, Environment* information)
{
    char message[] = "Temp:xx.xx C";
    uchar i = 0, pos = 0;
    uchar displayValues[4] = {0};

    displayValues[0] = information->temperature[0] / 10;
    displayValues[1] = information->temperature[0] % 10;
    displayValues[2] = information->temperature[1] / 10;
    displayValues[3] = information->temperature[1] % 10;

    for (i = 0; i < sizeof(message) / sizeof(char); i++)
    {
        if (message[i] == 'x')
        {
            message[i] = displayValues[pos]+'0';
            pos++;
        }
    }

    lcd1602_setcursor(row, col);
    lcd1602_message(message);  
}

void lcd1602_display_humidity(uchar row, uchar col, Environment* information)
{
    char message[] = "Humi:xx.xx %";
    uchar i = 0, pos = 0;
    uchar displayValues[4] = {0};

    displayValues[0] = information->humidity[0] / 10;
    displayValues[1] = information->humidity[0] % 10;
    displayValues[2] = information->humidity[1] / 10;
    displayValues[3] = information->humidity[1] % 10;

    for (i = 0; i < sizeof(message) / sizeof(char); i++)
    {
        if (message[i] == 'x')
        {
            message[i] = displayValues[pos]+'0';
            pos++;
        }
    }

    lcd1602_setcursor(row, col);
    lcd1602_message(message);  
}

void lcd1602_display(enum DisplayMode dispMode, Environment* information)
{
    static enum DisplayMode lastDispMode = eDisplayMode_time_date;
    if (lastDispMode != dispMode)
    {
        lcd1602_clear();
        delay_ms(500);
    }

    lastDispMode = dispMode;
    switch (dispMode)
    {
        case eDisplayMode_time_date:
        {
            lcd1602_display_time(0, 0, information);
            lcd1602_display_date(1, 0, information);
            break;
        }
        case eDisplayMode_tphumi:
        {
            lcd1602_display_temperature(0, 0, information);
            lcd1602_display_humidity(1, 0, information);
            break;
        }
        default:
        {
            lcd1602_clear();
            lcd1602_message("Error --->\n  SunFounder");
            break;
        }
    }
}