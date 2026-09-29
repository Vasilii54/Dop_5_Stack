#include <stdio.h>
#include <stdarg.h>

void display(char* format, ...)
{
    int d;
    double f;
    va_list factor;         // указатель на необязательный параметр
    va_start(factor, format);   // устанавливаем указатель

    for(char *c = format;*c; c++)
    {
        if(*c!='%')
        {
            printf("%c", *c);
            continue;
        }
        switch(*++c)    // если символ - %, то переходим к следующему символу
        {
            case 'd':
                d = va_arg(factor, int);
                printf("%d", d);
                break;
            case 'f':
                f = va_arg(factor, double);
                printf("%.2lf", f);
                break;
            default:
                printf("%c", *c);
        }
    }
    va_end(factor);
}

int main(void)
{
    display("Count: %d \tPrice: %f", 24, 68.4);
    return 0;
}
