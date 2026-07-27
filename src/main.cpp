#include <Arduino.h>

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.printf("Flash size: %u bytes\n", ESP.getFlashChipSize());
  Serial.printf("PSRAM size: %u bytes\n", ESP.getPsramSize());
}

void loop() {

}