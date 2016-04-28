#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

void mprintf(const char *str, ...)
{
    va_list lst;
    va_start(lst, str);

    char buf[13];

    int j, tlen, i = 0, len = strlen(str);
    for (i = 0; i < len; i++)
    {
        if (str[i] == '%')
        {
            if (i + 1 < len && str[i + 1] == 'd')
            {
                _itoa(va_arg(lst, int), buf, 10);
                tlen = strlen(buf);
                for (j = 0; j < tlen; j++)
                    putchar(buf[j]);
                i++;
            }
            else if (i + 1 < len && str[i + 1] == 'f')
            {
                _gcvt(va_arg(lst, double), 10, buf);
                tlen = strlen(buf);
                for (j = 0; j < tlen; j++)
                    putchar(buf[j]);
                i++;
            }
            else
                putchar(str[i]);
        }
        else
            putchar(str[i]);
    }
}

int main()
{

    mprintf("Hello world %d %d\n", 12321312, 1);

    getchar();
    return 0;
}