#include "dns_parser.h"


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