#include <Arduino.h>

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.printf("Flash: %u\n", ESP.getFlashChipSize());
  Serial.printf("PSRAM: %u\n", ESP.getPsramSize());

  if(psramFound()) {
    Serial.println("PSRAM FOUND");
  } else {
    Serial.println("NO PSRAM");
  }
}

void loop() {}