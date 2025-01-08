#include "project.h"
#include "stdio.h"
void TCJSendEnd();

void TCJSendValue(char *name, int value);

void TCJSendTxt(char *name, char *value);

void TCJSendAnyProperty(char *object_name, char *property, char *value);

void TCJSendAny(char *any);
