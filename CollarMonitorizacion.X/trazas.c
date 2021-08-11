#include "trazas.h"

char mibuf[80];

void writeConsole(char *text)
{
    while(*text != 0)
    {
        while(!EUSART_is_tx_ready());
        EUSART_Write(*text);
        text++;
    }
}

void printdec(int16_t val)
{
    char btmp[10];
    int i = 0;
    char *buf;
    
    buf = mibuf;
    if(val < 0)
    {
        *buf = '-';
        val = -val;
        buf++;
    }
    if(val == 0)
    {
        *buf = '0';
        *(buf+1) = 0;
        return;
    }
    while(val > 0)
    {
        btmp[i] = (val%10)+'0';
        val = val/10;
        i++;
    }
    i--;
    for(;i>=0;i--)
    {
        *buf = btmp[i];
        buf++;
    }
    *buf = 0;
    writeConsole(mibuf);
}

char tohex(char c)
{
    if(c > 9)
        return(c-10+'a');
    else
        return(c+'0');
}

void printhex(uint16_t val)
{
    char *buf = mibuf;
    *buf++ = '0';
    *buf++ = 'x';
    *buf++ = tohex((val >> 12) & 0xf);
    *buf++ = tohex((val >> 8) & 0xf);
    *buf++ = tohex((val >> 4) & 0xf);
    *buf++ = tohex(val & 0xf);
    *buf = 0;
    writeConsole(mibuf);
}
