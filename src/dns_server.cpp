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
#include "api.h"

WiFiUDP udp;
WiFiUDP upstreamUdp;
byte dnsPacket[MAX_DNS_PACKET_SIZE];
unsigned long statsDelay = 0;
int pos;
bool upstreamOnline = false;
unsigned long lastUpstreamSuccess = 0;
unsigned long totalUpstreamLatency = 0;
unsigned long upstreamRequests = 0;
unsigned long upstreamFailures = 0;



void startDNSServer(){
    bool success = udp.begin(DNS_PORT);
  upstreamUdp.begin(0);

  if(success){
    Serial.println("DNS Server started");
  }
  statsDelay = millis();
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
bool forwardDNS(byte packet[], int length, byte response[], int &responseLength) {

  Settings currentSettings = getSettings();
  Serial.print("Forwarding DNS to: ");
  Serial.println(currentSettings.upstreamDNS);

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

  return false;
}

void requestStatistics(){
  if (millis() - statsDelay > 20000){
    Serial.println("---- DNS Stats ----");
    Serial.print("Total: ");
    Serial.println(totalRequests);
    Serial.print("Blocked: ");
    Serial.println(blockedRequests);
    Serial.print("Forwarded: ");
    Serial.println(forwardedRequests);
    statsDelay = millis();
  }
  
}


void handleDNS(){
    int packetSize = udp.parsePacket();




  if(packetSize){
    totalRequests++;
    DEBUG_PRINTLN("PACKET!");

    int bytesRead = udp.read(dnsPacket, MAX_DNS_PACKET_SIZE);

    DEBUG_PRINT("RAW PACKET:");
    for (int i = 0; i < bytesRead; i++) {
      DEBUG_PRINTF("%02x ", dnsPacket[i]);
    } 
      DEBUG_PRINTLN();
    

    DEBUG_PRINT("Bytes copied: ");
    DEBUG_PRINTLN(bytesRead);

    DEBUG_PRINT("Size: ");
    DEBUG_PRINTLN(packetSize);

    DEBUG_PRINT("From: ");
    DEBUG_PRINTLN(udp.remoteIP());

    DEBUG_PRINT("Port: ");
    DEBUG_PRINTLN(udp.remotePort());
  
    //save transaction id
    byte idHigh = dnsPacket[0];
    byte idLow = dnsPacket[1];
  

    //Skip the 10 byte DNS header post ID
    pos = 12;
    byte question[64];
    int questionLength = 0;


    while(pos < bytesRead && questionLength < 64) {
      question[questionLength] = dnsPacket[pos];
      questionLength++;
      pos++;
    }


    String domain = readDomain(question, questionLength, pos);

    byte qTypeHigh = question[pos];
    byte qTypeLow = question[pos + 1];
    byte qClassHigh = question[pos + 2];
    byte qClassLow = question[pos + 3];
    pos += 4; // moves parser position after qtype and qclass as we already "read" those 

    int qType = (qTypeHigh << 8) | qTypeLow;
    int qClass = (qClassHigh << 8) | qClassLow;
    DEBUG_PRINT("QTYPE: ");
    DEBUG_PRINTLN(qType);
    DEBUG_PRINT("QCLASS: ");
    DEBUG_PRINTLN(qClass);
    
    

    


    DEBUG_PRINTLN("Parser finished");



    domain.toLowerCase();


    DEBUG_PRINT("DNS request: ");
    DEBUG_PRINTLN(domain);

   


    // DNS Response Header
    byte response[12];


    createHeader(response, idHigh, idLow, true);

    bool blocked =isBlocked(domain);


    DEBUG_PRINT("Blocked? ");
    DEBUG_PRINTLN(blocked);

    byte ipv41;
    byte ipv42;
    byte ipv43;
    byte ipv44;


    if(getSettings().blockingEnabled && blocked){
    
      logQuery(domain, BLOCKED);
      blockedRequests++;
      incrementBlockedDomain(domain);
      if(qType == 1)
      {
      ipv41 = 0;
      ipv42 = 0;
      ipv43 = 0;
      ipv44 = 0;
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

            DEBUG_PRINTLN("Sent cached response");

            return;
        }
        
        forwardedRequests++;
        logQuery(domain, FORWARDED);
        byte upstreamResponse[MAX_DNS_PACKET_SIZE];
        int responseLength = 0;

        bool success = forwardDNS(
          dnsPacket,
          bytesRead,
          upstreamResponse,
          responseLength
        );

        

      
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

        DEBUG_PRINTLN("Forwarded DNS response");
        DEBUG_PRINT("TTL: ");
        DEBUG_PRINTLN(getDNSResponseTTL(
          upstreamResponse,
          responseLength
        ));

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
    if(qType != 1 && qType != 28){//excludes unsupported DNS formats
      DEBUG_PRINT("Unsupported DNS type: ");
      DEBUG_PRINTLN(qType);


      byte response[12];

      createHeader(response, idHigh, idLow, false);


      udp.beginPacket(udp.remoteIP(),udp.remotePort());


      udp.write(response, sizeof(response));
      udp.write(question,questionLength);

      //Include qtype and qclass
      udp.write(question[pos]);
      udp.write(question[pos+1]);
      udp.write(question[pos+2]);
      udp.write(question[pos+3]);



      udp.endPacket();
      
      return;
    }
    else{
      udp.beginPacket(udp.remoteIP(),udp.remotePort());


    udp.write(response, sizeof(response));
    udp.write(question, questionLength);
    if (qType == 1)
    {
      udp.write(answerv4,sizeof(answerv4));
      DEBUG_PRINTLN("Sending DNS response");
      DEBUG_PRINT("Response size: ");
      DEBUG_PRINTLN(sizeof(response) + questionLength +sizeof(answerv4));
    }
    else if(qType == 28)
    {
       udp.write(answerv6,sizeof(answerv6));
       DEBUG_PRINTLN("Sending DNS response");
       DEBUG_PRINT("Response size: ");
       DEBUG_PRINTLN(sizeof(response) + questionLength +sizeof(answerv6));
    }

    
    udp.endPacket();


    }
  }

    requestStatistics();
}
