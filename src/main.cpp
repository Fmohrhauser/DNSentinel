//libs
#include <arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <secrets.h>
#include "dns_parser.h"
#include "blocklist.h"

//objects

//constants
const int upstreamTimeout = 2000;




const int dnsPort = 53;

//variables
WiFiUDP udp;
WiFiUDP upstreamUdp;
IPAddress dnsServer(8,8,8,8);
byte dnsPacket[512];

int pos;
unsigned long totalRequests = 0;
unsigned long blockedRequests = 0;
unsigned long forwardedRequests = 0;
unsigned long statsDelay = 0;
//extra functions










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
//setup
void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid,password);

  Serial.print("Connecting");

  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }

  Serial.println();

  Serial.println("Wifi connected!");
  Serial.print("ESP32 IP address: ");
  Serial.println(WiFi.localIP());


  bool success = udp.begin(dnsPort);
  upstreamUdp.begin(0);

  Serial.print("UDP begin result: ");
  Serial.println(success);
  statsDelay = millis();

}
//main program
void loop() {
  
  int packetSize = udp.parsePacket();




  if(packetSize){
    totalRequests++;
    Serial.println("PACKET!");

    int bytesRead = udp.read(dnsPacket, 512);

    Serial.println("RAW PACKET:");
    for (int i = 0; i < bytesRead; i++) {
      Serial.printf("%02x ", dnsPacket[i]);
    } 
      Serial.println();
    

    Serial.print("Bytes copied: ");
    Serial.println(bytesRead);

    Serial.print("Size: ");
    Serial.println(packetSize);

    Serial.print("From: ");
    Serial.println(udp.remoteIP());

    Serial.print("Port: ");
    Serial.println(udp.remotePort());
  
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
    Serial.print("QTYPE: ");
    Serial.println(qType);
    Serial.print("QCLASS: ");
    Serial.println(qClass);
    
    

    


    Serial.println("Parser finished");



    domain.toLowerCase();


    Serial.print("DNS request: ");
    Serial.println(domain);

   


    // DNS Response Header
    byte response[12];


    createHeader(response, idHigh, idLow, true);

    bool blocked =isBlocked(domain);


    Serial.print("Blocked? ");
    Serial.println(blocked);

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
        Serial.println("Forwarded DNS response");

      }
      else{
        Serial.println("Upstream DNS failed");
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
      Serial.println("Non A record request");


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

    Serial.println("Sending DNS response");
    Serial.print("Response size: ");
    Serial.println(sizeof(response) + questionLength +sizeof(answer));
    udp.endPacket();




    }
  }

    requestStatistics();
}




