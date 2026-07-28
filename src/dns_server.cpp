#include "dns_server.h"
#include "dns_parser.h"
#include "blocklist.h"
#include "debug.h"
#include <WiFiUdp.h>

WiFiUDP udp;
WiFiUDP upstreamUdp;
IPAddress dnsServer(8,8,8,8);
byte dnsPacket[512];
unsigned long totalRequests = 0;
unsigned long blockedRequests = 0;
unsigned long forwardedRequests = 0;
unsigned long statsDelay = 0;
const int upstreamTimeout = 2000;
int pos;
const int dnsPort = 53;


void startDNSServer(){
    bool success = udp.begin(dnsPort);
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
  upstreamUdp.beginPacket(dnsServer, 53);


  upstreamUdp.write(packet, length);


  upstreamUdp.endPacket();


  unsigned long start = millis();


  while(millis() - start < upstreamTimeout){
    int size = upstreamUdp.parsePacket();

    if(size){


      responseLength = upstreamUdp.read(response, 512);

      return true;
    }
  }


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

    int bytesRead = udp.read(dnsPacket, 512);

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

    byte ip1;
    byte ip2;
    byte ip3;
    byte ip4;

    if(blocked){
      blockedRequests++;
      ip1 = 0;
      ip2 = 0;
      ip3 = 0;
      ip4 = 0;
    }
    else{
      forwardedRequests++;


      byte upstreamResponse[512];
      int responseLength = 0;


      bool success = forwardDNS(
        dnsPacket,
        bytesRead,
        upstreamResponse,
        responseLength
      );


      if(success){

        udp.beginPacket(udp.remoteIP(),udp.remotePort());
        udp.write(upstreamResponse, responseLength);
        udp.endPacket();
        DEBUG_PRINTLN("Forwarded DNS response");

      }
      else{
        DEBUG_PRINTLN("Upstream DNS failed");
      }


      return;
    }


    //DNS answer
    byte answer[] = {
      0xC0, 0x0C,


      0x00,0x01,
      0x00,0x01,


      0x00,0x00,0x00,0x3C,


      0x00, 0x04,


      ip1,ip2,ip3,ip4
    };


    //Send Response
    if(qType != 1){//ipv6 not supported yet
      DEBUG_PRINTLN("Non A record request");


      byte response[12];

      createHeader(response, idHigh, idLow, false);


      udp.beginPacket(udp.remoteIP(),udp.remotePort());


      udp.write(response, sizeof(response));
      udp.write(question,questionLength);


      udp.endPacket();
      
      return;
    }
    else{
      udp.beginPacket(udp.remoteIP(),udp.remotePort());


    udp.write(response, sizeof(response));
    udp.write(question, questionLength);
    udp.write(answer,sizeof(answer));

    DEBUG_PRINTLN("Sending DNS response");
    DEBUG_PRINT("Response size: ");
    DEBUG_PRINTLN(sizeof(response) + questionLength +sizeof(answer));
    udp.endPacket();




    }
  }

    requestStatistics();
}
