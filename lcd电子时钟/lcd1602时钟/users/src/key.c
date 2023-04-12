#include "common.h"

#define KEY_PRESS   0
#define KEY_COUNT   3

typedef struct {
    uchar           status;
    uchar           save;
    KeyActionFunc   action;
} KeyInfo;

KeyInfo gKeyInfos[KEY_COUNT];

void key_register(uchar key_id, KeyActionFunc action)
{
    gKeyInfos[key_id].status = 1;
    gKeyInfos[key_id].save = 1;
    gKeyInfos[key_id].action = action;
}

void key_action()
{
    uchar i = 0;
  
    for (i = 0; i < KEY_COUNT; i++)
    {
        if (gKeyInfos[i].save != gKeyInfos[i].status)
        {
            if (gKeyInfos[i].save == 0)
                gKeyInfos[i].action();

            gKeyInfos[i].save = gKeyInfos[i].status;
        }
    }
}

void key_scan()
{
    uchar i = 0;
    uchar keyValue = KEY_1;
    static uchar keybuff[KEY_COUNT] = {0xff,0xff,0xff};

    for (i = 0; i < KEY_COUNT; i++)
    {
        switch (i)
        {
            case 0: keyValue = KEY_1; break;
            case 1: keyValue = KEY_2; break;
            case 2: keyValue = KEY_3; break;
            default: break;
        }

        keybuff[i] = (keybuff[i] << 1) | keyValue;
        if (keybuff[i] == 0x00)     gKeyInfos[i].status = 0;
        else if(keybuff[i] == 0xff) gKeyInfos[i].status = 1;
    }
}

void key_run()
{
    key_scan();
    key_action();
}