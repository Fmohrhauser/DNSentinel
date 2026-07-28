#include "blocklist.h"
#include "debug.h"




String blockedDomains[] = {
  "doubleclick.net",
  "googleadservices.com"
};

const int blockedCount = sizeof(blockedDomains)/ sizeof(blockedDomains[0]);

bool isBlocked(String domain) {
    DEBUG_PRINT("Checking: ");
    DEBUG_PRINTLN(domain);

    //Match exact domains and subdomains
    //Prevent false positives like notexample.com
    for(int i =0; i < blockedCount; i++) {
      if(domain == blockedDomains[i])
      {
        return true;
      }

      String suffix = "." + blockedDomains[i];

      if(domain.endsWith(suffix)) {
        return true;
      }
    }


    return false;
}