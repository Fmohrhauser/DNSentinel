#include "dns_server.h"
#include "dns_parser.h"
#include "blocklist.h"
#include "debug.h"
#include <WiFiUdp.h>
#include "config.h"
#include "dns_cache.h"
#include "query_log.h"
#include "stats.h"
#include "blocked_stats.h"
#include "settings.h"
#include "cache_stats.h"
#include "dns_health_stats.h"
#include "whitelist.h"

WiFiUDP udp;
WiFiUDP upstreamUdp;
byte dnsPacket[MAX_DNS_PACKET_SIZE];
unsigned long statsDelay = 0;
int pos;





void startDNSServer(){
    bool success = udp.begin(DNS_PORT);
  upstreamUdp.begin(0);

  if(success){
    Serial.println("DNS Server started");
  }
  statsDelay = millis();
}

void parseIP(
  String ip,
  byte &a,
  byte &b,
  byte &c,
  byte &d
)
{
  int first = ip.indexOf('.');
  int second = ip.indexOf('.', first + 1);
  int third = ip.indexOf('.', second + 1);

  a = ip.substring(0, first).toInt();
  b = ip.substring(first + 1, second).toInt();
  c = ip.substring(second + 1, third).toInt();
  d = ip.substring(third + 1).toInt();
}





void createHeader(byte response[], byte idHigh, byte idLow, bool hasAnswer) {
  response[0] = idHigh;
  response[1] = idLow;


  //response + recursion available
  response[2] = 0x81;
  response[3] = 0x80;


  //Questions = 1
  response[4] = 0x00;
  response[5] = 0x01;


  if(hasAnswer) {
    //Answers = 1
    response[6] = 0x00;
    response[7] = 0x01;
  }
  else {
    //Answers = 0
    response[6] = 0x00; 
    response[7] = 0x00;

  }


  //Authority = 0
  response[8] = 0x00;
  response[9] = 0x00;


  //Additional = 0
  response[10] = 0x00;
  response[11] = 0x00;
}

void sendNXDOMAIN(
  byte idHigh,
  byte idLow,
  byte question[],
  int questionLength
)
{
  byte response[12];

  createHeader(
    response,
    idHigh,
    idLow,
    false
  );

  // Set RCODE = 3 (NXDOMAIN)
  response[3] = 0x83;

  udp.beginPacket(
    udp.remoteIP(),
    udp.remotePort()
  );

  udp.write(response, sizeof(response));

  udp.write(
    question,
    questionLength
  );

  udp.endPacket();

}


bool forwardDNS(byte packet[], int length, byte response[], int &responseLength) {

  Settings currentSettings = getSettings();

  unsigned long start = millis();

  upstreamUdp.beginPacket(currentSettings.upstreamDNS.c_str(), 53);


  upstreamUdp.write(packet, length);


  upstreamUdp.endPacket();


  


  while(millis() - start < UPSTREAM_TIMEOUT){
    int size = upstreamUdp.parsePacket();

    if(size){

      unsigned long latency =
        millis() - start;

      totalUpstreamLatency +=latency;

      upstreamRequests++;

      lastUpstreamSuccess = millis();

      upstreamOnline = true;
      upstreamChecked = true;

      responseLength =
        upstreamUdp.read(
          response,
          MAX_DNS_PACKET_SIZE
        );

      return true;
    }
  }

  upstreamFailures ++;

  upstreamOnline = false;
  upstreamChecked = true;

  return false;
}



void handleDNS(){
    int packetSize = udp.parsePacket();




  if(packetSize){
    incrementTotalRequests();


    int bytesRead = udp.read(dnsPacket, MAX_DNS_PACKET_SIZE);


    for (int i = 0; i < bytesRead; i++) {

    } 

    
    //save transaction id
    byte idHigh = dnsPacket[0];
    byte idLow = dnsPacket[1];
  

    //Skip the 10 byte DNS header post ID
    pos = 12;
    byte question[260];
    int questionLength = 0;


    while(pos < bytesRead && questionLength < sizeof(question)) {
      question[questionLength] = dnsPacket[pos];
      questionLength++;
      pos++;
    }

    if(pos < bytesRead)
    {
      return;
    }


    String domain = readDomain(question, questionLength, pos);

    if(domain.length() == 0)
    {
      return;
    }
    
    if(pos + 4 > questionLength)
    {
      return;
    }

    byte qTypeHigh = question[pos];
    byte qTypeLow = question[pos + 1];
    byte qClassHigh = question[pos + 2];
    byte qClassLow = question[pos + 3];
    pos += 4; // moves parser position after qtype and qclass as we already "read" those 

    int qType = (qTypeHigh << 8) | qTypeLow;
    int qClass = (qClassHigh << 8) | qClassLow;




    domain.toLowerCase();


   


    // DNS Response Header
    byte response[12];


    createHeader(response, idHigh, idLow, true);

    bool blocked = !isWhitelisted(domain) && isBlocked(domain);
    byte ipv41;
    byte ipv42;
    byte ipv43;
    byte ipv44;


    if(getSettings().blockingEnabled && blocked){
    
      logQuery(domain, BLOCKED);
      incrementBlockedRequests();
      incrementBlockedDomain(domain);

            if(qType != 1 && qType !=28){ //use NXDOMAIN for DNS record types without a premade blocking response/modes

        sendNXDOMAIN(
          idHigh,
          idLow,
          question,
          questionLength
        );
        return;
      }

      Settings currentSettings = getSettings();

      if(currentSettings.blockingMode == NULL_IP)
      {
        if(qType == 1)
        {
          ipv41 = 0;
          ipv42 = 0;
          ipv43 = 0;
          ipv44 = 0;
        }
      }
      else if(currentSettings.blockingMode == REDIRECT)
      {
        if(qType == 1)
        {
        int firstDot = currentSettings.redirectIP.indexOf('.');
        int secondDot = currentSettings.redirectIP.indexOf('.', firstDot + 1);
        int thirdDot = currentSettings.redirectIP.indexOf('.', secondDot + 1);

        ipv41 = currentSettings.redirectIP.substring(0,firstDot).toInt();
        ipv42 = currentSettings.redirectIP.substring(firstDot + 1, secondDot).toInt();
        ipv43 = currentSettings.redirectIP.substring(secondDot + 1, thirdDot).toInt();
        ipv44 = currentSettings.redirectIP.substring(thirdDot + 1).toInt();
        }
      }
      else if(currentSettings.blockingMode == NXDOMAIN)
      {
        sendNXDOMAIN(
          idHigh,
          idLow,
          question,
          questionLength
 
        );

        return;
      }
      
      
      
      
    }
    else{
      
        byte cachedResponse[MAX_DNS_PACKET_SIZE];
        int cachedLength = 0;

        if(cacheLookup(domain, qType, cachedResponse, cachedLength))
        {

            logQuery(domain, CACHE_HIT);
            //Restore transaction ID from current request
            cachedResponse[0] = dnsPacket[0];
            cachedResponse[1] = dnsPacket[1];


            udp.beginPacket(
              udp.remoteIP(),
              udp.remotePort()
            );

            udp.write(cachedResponse, cachedLength);

            udp.endPacket();



            return;
        }
        
        
        byte upstreamResponse[MAX_DNS_PACKET_SIZE];
        int responseLength = 0;

        bool success = forwardDNS(
          dnsPacket,
          bytesRead,
          upstreamResponse,
          responseLength
        );
        if(success)
        {
          incrementForwardedRequests();

          logQuery(
            domain,
            FORWARDED
        );
        }

        

      
      if(success){
        cacheInsert(
          domain,
          qType,
          upstreamResponse,
          responseLength
        );

        udp.beginPacket(
          udp.remoteIP(),
          udp.remotePort()
        );

        udp.write(
          upstreamResponse,
          responseLength
        );
        udp.endPacket();


      }
      else{
        DEBUG_PRINTLN("Upstream DNS failed");
      }


      return;
    }


    //DNS answer
     byte answerv4[] = {
       0xC0, 0x0C,


       0x00,0x01,
       0x00,0x01,


       0x00,0x00,0x00,0x3C,


       0x00, 0x04,


       ipv41,ipv42,ipv43,ipv44
       };
      byte answerv6[] = {
        0xC0, 0x0C,
        0x00, 0x1C,
        0x00, 0x01,
        0x00, 0x00, 0x01, 0x2C,
        0x00, 0x10,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
      };
      


    //Send Response
      udp.beginPacket(udp.remoteIP(),udp.remotePort());


    udp.write(response, sizeof(response));
    udp.write(question, questionLength);
    if (qType == 1)
    {
      udp.write(answerv4,sizeof(answerv4));

    }
    else if(qType == 28)
    {
       udp.write(answerv6,sizeof(answerv6));

    }

    
    udp.endPacket();


    }
  }


