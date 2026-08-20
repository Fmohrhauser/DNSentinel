#include "dns_parser.h"
#include "debug.h"

String readDomain(byte buffer[], int length, int &pos) {
    pos = 0;
    String domain = "";
    
    
    while(pos < length) {

      byte labelLength = buffer[pos];
      pos++;


      if(labelLength == 0) {
        break;
      }
      if(labelLength > 63)
      {
        return "";
      }

      if(pos + labelLength > length)
      {
        return "";
      }


      for(int i = 0; i < labelLength; i++) {
        domain += char(buffer[pos]);
        pos++;
      }


      domain +=".";
    }

    if(domain.length() > 0) {
      domain.remove(domain.length() - 1);

    }
  
    return domain;
}


unsigned long getDNSResponseTTL(
  const byte response[],
  int responseLength
)
{
  if(responseLength < 12)
  {
    return 60;
  }
  int pos =12;

  //Skip question domain name
  while (pos < responseLength)
  {
    byte labelLength = response[pos];
    pos++;

    if((labelLength & 0xC0) == 0xC0)
    {
      if(pos >= responseLength)
      {
        return 60;
      }

      pos++;
      break;
    }

    if(labelLength == 0)
    {
      break;
    }

    if(labelLength > 63)
    {
      return 60;
    }

    if(pos + labelLength > responseLength)
    {
      return 60;
    }

    pos+= labelLength;
  }

  //Skip QTYPE + QCLASS
  if(pos + 4 > responseLength)
  {
    return 60;
  }
  pos += 4;

  //Check we actually have an answer
  if(pos +10 > responseLength)
  {
    return 60; // fallback TTL
  }

  //Skip Name field
 if(pos >= responseLength)
 {
  return 60;
 }

 byte nameByte = response[pos];

 if((nameByte & 0xC0) == 0xC0)
 {
  if(pos + 2 > responseLength)
  {
    return 60;
  }

  pos += 2;
 }
 else
 {
  while(pos < responseLength)
  {
    byte labelLength = response[pos];
    pos++;

    if(labelLength == 0)
    {
      break;
    }

    if(labelLength > 63)
    {
      return 60;
    }

    if(pos + labelLength > responseLength)
    {
      return 60;
    }

    pos += labelLength;
  }
 }

  //Skip TYPE
  pos += 2;

  //Skip CLASS
  pos += 2;

  //Read TTL
  unsigned long ttl =  //combine bytes
      ((unsigned long)response[pos] <<24) |
      ((unsigned long)response[pos + 1] << 16) |
      ((unsigned long)response[pos + 2] << 8) |
      ((unsigned long)response[pos + 3]);

  return ttl;
}