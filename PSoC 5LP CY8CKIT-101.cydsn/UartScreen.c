#include "UartScreen.h"
void TCJSendEnd()
{
    UART_2_PutString("\xff\xff\xff");
}

void TCJSendValue(char *name, int value)
{
    char tmp[30];
    sprintf(tmp, "%s.val=%d", name, value);
    UART_2_PutString(tmp);
    TCJSendEnd();
}

void TCJSendTxt(char *name, char *value)
{
    char tmp[30];
    sprintf(tmp, "%s.txt=\"%s\"", name, value);
    UART_2_PutString(tmp);
    TCJSendEnd();
}

void TCJSendAnyProperty(char *object_name, char *property, char *value)
{
    char tmp[30];
    sprintf(tmp, "%s.%s=%s", object_name, property, value);
    UART_2_PutString(tmp);
    TCJSendEnd();
}

void TCJSendAny(char *any)
{
    UART_2_PutString(any);
    TCJSendEnd();
}