#include <SPI.h>
#include "DW1000Ranging.h"
#include "DW1000Device.h"

// connection pins
const uint8_t PIN_RST = 3; // reset pin
const uint8_t PIN_IRQ = 7; // irq pin
const uint8_t PIN_SS = 10; // spi select pin

void setup() {
  // Serial is for communication between the computer and the board.
  Serial.begin(115200);
  delay(1000);
  DW1000Ranging.initCommunication(PIN_RST, PIN_SS, PIN_IRQ);
  DW1000Ranging.attachNewRange(newRange);
  DW1000Ranging.startAsAnchor("3A:1A:1A:1A:1A:1A:1A:1A", DW1000.MODE_LONGDATA_RANGE_ACCURACY);
}

void loop() {
  DW1000Ranging.loop();

  Serial.print(DW1000Ranging.getTagX());
  Serial.print(" ");
  Serial.println(DW1000Ranging.getTagY());
  if(DW1000Ranging.getTagY() > 4.0f){
    digitalWrite(6,HIGH);
  }
  else digitalWrite(6,LOW);
}

void newRange() {
  // Empty.  
}

// 23 26 3
