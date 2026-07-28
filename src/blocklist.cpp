#include "blocklist.h"


const int blockedCount = 2;


String blockedDomains[] = {
  "doubleclick.net",
  "googleadservices.com"
};



bool isBlocked(String domain) {

    for(int i =0; i < blockedCount; i++) {

      if (domain.endsWith(blockedDomains[i])){
        return true;
      }
    }


    return false;
}