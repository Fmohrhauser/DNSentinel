#ifndef DNS_PARSER_H
#define DNS_PARSER_H


#include <Arduino.h>
String readDomain(byte buffer[], int length, int &pos);

unsigned long getDNSResponseTTL(
    const byte response[],
    int responseLength
);

#endif