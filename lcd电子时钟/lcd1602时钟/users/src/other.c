#include "common.h"

uchar bcd_to_dec(uchar source_data)
{
    return source_data / 16 * 10 + source_data % 16;
}

uchar dec_to_bcd(uchar source_data)
{
    return source_data / 10 * 16 + source_data % 10;
}

void delay_us(uint n)
{
    while(--n);
}

void delay_ms(uint c)
{
	uint i, j;
	for (i = c; i > 0; i--)
		for (j = 125; j > 0; j--);
}