#include <SPI.h>
#include "DW1000Ranging.h"
#include "DW1000Device.h"
#include <MatrixMath.h>
#include <math.h>
//A3: 1
//A2: 3
//A1: 2
// connection pins
const uint8_t PIN_RST = 3; // reset pin
const uint8_t PIN_IRQ = 7; // irq pin
const uint8_t PIN_SS = 10; // spi select pin
volatile unsigned long delaySent = 0;
volatile boolean sentAck = false;
volatile float range = 0;
volatile int address = 0;

volatile float dist[3] = {655.35, 655.35, 655.35};
float beacons[3][2] = {{0.0f, 2.94f},
                       {0.0f, 0.0f},
                       {2.94f, 0.0f}};

float A[3][3];
void setup() {
  Serial.begin(115200);
  delay(1000);
  pinMode(6,OUTPUT);
  //init the configuration
  DW1000Ranging.initCommunication(PIN_RST, PIN_SS, PIN_IRQ);
  DW1000Ranging.attachNewRange(newRange);
  DW1000Ranging.startAsTag("7D:00:22:EA:82:60:3B:9C", DW1000.MODE_LONGDATA_RANGE_ACCURACY);
  unsigned long start, step_start, finished;

  start = micros();
  
  for(int i = 0; i < 3; i++) {
    A[i][0] = -2.0f*beacons[i][0];
    A[i][1] = -2.0f*beacons[i][1];
    A[i][2] = 1.0f;
  }

  Matrix.Invert((float*)A, 3);

  step_start = micros();

  Serial.println(step_start - start);
}
void loop() {
  DW1000Ranging.loop();
}

void newRange() {
  address = DW1000Ranging.getDistantDevice()->getShortAddress();

  range = DW1000Ranging.getDistantDevice()->getRange();
  range -= 0.2;
  range *= 0.7257;
  dist[0] = address==0x1A1A?range:dist[0];
  dist[1] = address==0x2A1A?range:dist[1];
  dist[2] = address==0x1A3A?range:dist[2];
  Serial.print("1: ");Serial.print(dist[0]);Serial.print(" 2: ");Serial.print(dist[1]);Serial.print(" 3: ");Serial.print(dist[2]);
  
  // ITERATIVE STEPS BELOW

  float y[3];
  for(int i = 0; i < 3; i++) {
    y[i] = float((dist[i])*(dist[i]) - (beacons[i][0])*(beacons[i][0]) - (beacons[i][1])*(beacons[i][1]));
  }
  float x[3];
  Matrix.Multiply((float*)A, (float*)y, 3, 3, 1, (float*)x);

  float estimate[2] = {x[0], x[1]};

  Serial.print(" Location: ["); Serial.print(estimate[0]); Serial.print("m, "); Serial.print(estimate[1]); Serial.print("m] \n");
  
  if(estimate[0] > 4.0f){
    digitalWrite(6,HIGH);
  }
  else digitalWrite(6,LOW);
}

