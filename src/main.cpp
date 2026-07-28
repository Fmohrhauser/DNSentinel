//libs
#include <arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <secrets.h>
#include "dns_parser.h"
#include "blocklist.h"
#include "dns_server.h"

//objects

//constants




//variables




//extra functions











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


  startDNSServer();

}
//main program
void loop() {
  handleDNS();
  
}




