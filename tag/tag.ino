#include <SPI.h>
#include "DW1000Ranging.h"
#include "DW1000Device.h"
#include <MatrixMath.h>
#include <math.h>

// If false, then use the calibration. If true, don't use the calibration.
#define CALIBRATE_FIRST false
#define CALIBRATE_SECOND false

//First distance from ground truth 2 meters
//Second from ground truth 5 meters.
#define BEACON_1_FIRSTDIST 2.368
#define BEACON_1_SECONDDIST 5.372

#define USE_MEDIAN_FILT true

#define BEACON_1_ADDRESS 0x1A1A
#define BEACON_2_ADDRESS 0x2A1A
#define BEACON_3_ADDRESS 0x1A3A

#define BEACON_1_X 0.00f
#define BEACON_1_Y 7.01f

#define BEACON_2_X 0.0f
#define BEACON_2_Y 0.0f

#define BEACON_3_X 7.92f
#define BEACON_3_Y 7.92f

//First distance from ground truth 2 meters
//Second from ground truth 5 meters.
#define BEACON_1_FIRSTDIST 2.368
#define BEACON_1_SECONDDIST 5.372
float beacon_1_slope = 3.0f/(BEACON_1_SECONDDIST-BEACON_1_FIRSTDIST);
float beacon_1_offset = 5.0f-(1.0f/beacon_1_slope*BEACON_1_SECONDDIST);

//First distance from ground truth 2 meters
//Second from ground truth 5 meters.
#define BEACON_2_FIRSTDIST 2.368
#define BEACON_2_SECONDDIST 5.372
float beacon_2_slope = 3.0f/(BEACON_2_SECONDDIST-BEACON_2_FIRSTDIST);
float beacon_2_offset = 5.0f-(1.0f/beacon_2_slope*BEACON_2_SECONDDIST);

//First distance from ground truth 2 meters
//Second from ground truth 5 meters.
#define BEACON_3_FIRSTDIST 2.368
#define BEACON_3_SECONDDIST 5.372
float beacon_3_slope = 3.0f/(BEACON_3_SECONDDIST-BEACON_3_FIRSTDIST);
float beacon_3_offset = 5.0f-(1.0f/beacon_3_slope*BEACON_3_SECONDDIST);
//
//#define BEACON_1_OFFSET 0.0368f
//#define BEACON_1_MULTIPLIER 1.0692f
//
//#define BEACON_2_OFFSET 0.6336f
//#define BEACON_2_MULTIPLIER 1.0440f
//
//#define BEACON_3_OFFSET 0.5575f
//#define BEACON_3_MULTIPLIER 1.0666f

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

#define N (5)
struct cir {
    float a[N];
    unsigned h;
    unsigned t;
    unsigned n;
};
typedef volatile struct cir cir_t;

cir_t *cir_new() {
    cir_t *cir = malloc(sizeof(*cir));
    cir->h = 0;
    cir->t = N - 1;
    cir->n = N;
    return cir;
}


// if adding 1 to tail == head, then there is nothing
float cir_empty(cir_t *cir) {
    return (cir->t + 1) % N == cir->h;
}

// if adding 1 to head = tail, then we have no space.
float cir_full(cir_t *cir) {
    return cir->h == cir->t;
}

void cir_enqueue(cir_t *cir, float x) {
    if (cir_full(cir)) cir->t = (cir->t + 1) % N;
    cir->a[cir->h] = x;
    cir->h = (cir->h + 1) % N;
    
    // for(int i = 0; i < N; i++) {
    //     printf("%d ", cir->a[i]);
    // }
    // printf(": %d %d\n", cir->t, cir->h);
}


int compare(const void *x, const void * y) {
    float ix = *(float*)x;
    float iy = *(float*)y;
    return (int)(ix - iy);
}

float cir_median(cir_t *cir) {
    float q[cir->n];
    int qcount = 0;
    // Populate the array
    if(cir_empty(cir)) return 0;
    else if(cir_full(cir)) {
        qcount = cir->n;
        for(int i = 0; i < cir->n; i++) {
            q[i] = cir->a[i];
        }
    } else if(cir->t > cir->h) {
        for(int i = 0; i < cir->h; i++) {
            q[qcount] = cir->a[i];
            qcount++;
        }
        for(int i = cir->t; i < cir->n - 1; i++) {
            q[qcount] = cir->a[i];
            qcount++;
        }
    } else {
        for(int i = cir->t; i < cir->h; i++) {
            q[qcount] = cir->a[i];
            qcount++;
        }
    }
    // Sort the array
    qsort(q, qcount, sizeof(float), compare);
    
    // for(int i = 0; i < qcount; i++) {
    //     printf("%d ", q[i]);
    // }
    // printf("\n");
    
    return q[qcount/2];
}

cir_t *cir1 = cir_new();
cir_t *cir2 = cir_new();
cir_t *cir3 = cir_new();

void setup() {
  Serial.begin(115200);
  delay(1000);
  pinMode(6,OUTPUT);
  
  //init the configuration
  DW1000Ranging.initCommunication(PIN_RST, PIN_SS, PIN_IRQ);
  DW1000Ranging.attachNewRange(newRange);
  DW1000Ranging.startAsTag("7D:00:22:EA:82:60:3B:9C", DW1000.MODE_LONGDATA_RANGE_ACCURACY);
  //DW1000Ranging.useRangeFilter(true);
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
    range *= CALIBRATE_FIRST||CALIBRATE_SECOND?1.0f:beacon_1_slope;
    range -= CALIBRATE_FIRST||CALIBRATE_SECOND?0.0f:beacon_1_offset;
    
    cir_enqueue(cir1,range);
    dist[0] = (USE_MEDIAN_FILT)?cir_median(cir1):range;
    
    if(CALIBRATE_FIRST||CALIBRATE_SECOND){
      if(dist[0] < shortestDistanceBeacon1 && dist[0] > 0){
        shortestDistanceBeacon1 = dist[0];
      }
    }
  }
  else if(address == BEACON_2_ADDRESS){
    range *= CALIBRATE_FIRST||CALIBRATE_SECOND?1.0f:beacon_2_slope;
    range -= CALIBRATE_FIRST||CALIBRATE_SECOND?0.0f:beacon_2_offset;
    cir_enqueue(cir2,range);
    dist[1] = (USE_MEDIAN_FILT)?cir_median(cir2):range;
    
    if(CALIBRATE_FIRST||CALIBRATE_SECOND){
      if(dist[1] < shortestDistanceBeacon2 && dist[1] > 0){
        shortestDistanceBeacon2 = dist[1];
      }
    }
  }
  else if(address == BEACON_3_ADDRESS){
    range *= CALIBRATE_FIRST||CALIBRATE_SECOND?1.0f:beacon_3_slope;
    range -= CALIBRATE_FIRST||CALIBRATE_SECOND?0.0f:beacon_3_offset;
    cir_enqueue(cir3,range);
    dist[2] = (USE_MEDIAN_FILT)?cir_median(cir3):range;
    
    if(CALIBRATE_FIRST||CALIBRATE_SECOND){
      if(dist[2] < shortestDistanceBeacon3 && dist[2] > 0){
        shortestDistanceBeacon3 = dist[2];
      }
    }
  }
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

  DW1000Ranging.setLoc(estimate[0], estimate[1]);
}

