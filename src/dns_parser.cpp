#include "dns_parser.h"
#include "debug.h"

String readDomain(byte buffer[], int length, int &pos) {
    pos = 0;
    String domain = "";
    
    
    while(pos < length) {

      byte labelLength = buffer[pos];
      pos++;


      if(labelLength ==0) {
        break;
      }


      for(int i =0; i < labelLength; i++) {
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
  int pos =12;

  //Skip question domain name
  while (pos < responseLength)
  {
    byte length = response[pos];

    pos++;
    if(length == 0)
    {
      break;
    }

    pos += length;
  }

  //Skip QTYPE + QCLASS
  pos += 4;

  //Check we actually have an answer
  if(pos +10 > responseLength)
  {
    return 60; // fallback TTL
  }

  //Skip Name field
  pos += 2;

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