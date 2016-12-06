#include <SPI.h>
#include "DW1000Ranging.h"
#include "DW1000Device.h"
#include <MatrixMath.h>
#include <math.h>

#define CALIBRATE_OFFSET false
#define CALIBRATE_MULTIPLIER false
#define CALIBRATION_DISTANCE 1.0

#define BEACON_1_ADDRESS 0x1A1A
#define BEACON_2_ADDRESS 0x2A1A
#define BEACON_3_ADDRESS 0x1A3A

#define BEACON_1_X 10.0f
#define BEACON_1_Y 5.0f

#define BEACON_2_X 10.0f
#define BEACON_2_Y 0.0f

#define BEACON_3_X 0.0f
#define BEACON_3_Y 0.0f

#define BEACON_1_OFFSET -0.09f
#define BEACON_1_MULTIPLIER 1.2f

#define BEACON_2_OFFSET -0.09f
#define BEACON_2_MULTIPLIER 0.7257f

#define BEACON_3_OFFSET -0.09f
#define BEACON_3_MULTIPLIER 1.55f

// connection pins
const uint8_t PIN_RST = 3; // reset pin
const uint8_t PIN_IRQ = 7; // irq pin
const uint8_t PIN_SS = 10; // spi select pin
volatile unsigned long delaySent = 0;
volatile boolean sentAck = false;
volatile float range = 0;
volatile int address = 0;

volatile float dist[3] = {655.35, 655.35, 655.35};

float beacons[3][2] = {{BEACON_1_X, BEACON_1_Y},
                       {BEACON_2_X, BEACON_2_Y},
                       {BEACON_3_X, BEACON_3_Y}};

float shortestDistanceBeacon1 = 900.0f;
float shortestDistanceBeacon2 = 900.0f;
float shortestDistanceBeacon3 = 900.0f;

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
  if(range < 0) address = NULL;
  if(address == BEACON_1_ADDRESS){
    range += CALIBRATE_OFFSET?0.0f:BEACON_1_OFFSET;
    range /= CALIBRATE_MULTIPLIER?1.0f:BEACON_1_MULTIPLIER;
    dist[0] = range;
    if(CALIBRATE_OFFSET){
      if(dist[0] < shortestDistanceBeacon1 && dist[0] > 0){
        shortestDistanceBeacon1 = dist[0];
      }
    }
  }
  else if(address == BEACON_2_ADDRESS){
    range += CALIBRATE_OFFSET?0.0f:BEACON_2_OFFSET;
    range /= CALIBRATE_MULTIPLIER?1.0f:BEACON_2_MULTIPLIER;
    dist[1] = range;
    if(CALIBRATE_OFFSET){
      if(dist[1] < shortestDistanceBeacon2 && dist[1] > 0){
        shortestDistanceBeacon2 = dist[1];
      }
    }
  }
  else if(address == BEACON_3_ADDRESS){
    range += CALIBRATE_OFFSET?0.0f:BEACON_3_OFFSET;
    range /= CALIBRATE_MULTIPLIER?1.0f:BEACON_3_MULTIPLIER;
    dist[2] = range;
    if(CALIBRATE_OFFSET){
      if(dist[2] < shortestDistanceBeacon3 && dist[2] > 0){
        shortestDistanceBeacon3 = dist[2];
      }
    }
  }
  if(CALIBRATE_OFFSET){
    Serial.print("Offset 1: ");Serial.print(shortestDistanceBeacon1);Serial.print(" Offset 2: ");Serial.print(shortestDistanceBeacon2);Serial.print(" Offset 3: ");Serial.print(shortestDistanceBeacon3);
  }
  else if(CALIBRATE_MULTIPLIER){
    Serial.print("MULT 1: ");Serial.print(dist[0]/CALIBRATION_DISTANCE);Serial.print(" MULT 2: ");Serial.print(dist[1]/CALIBRATION_DISTANCE);Serial.print(" MULT 3: ");Serial.print(dist[2]/CALIBRATION_DISTANCE);
  }
  else{
    Serial.print("1: ");Serial.print(dist[0]);Serial.print(" 2: ");Serial.print(dist[1]);Serial.print(" 3: ");Serial.print(dist[2]);
  }
  
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

