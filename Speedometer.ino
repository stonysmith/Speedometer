#include <Arduino.h>
#include <TM1637Display.h>

// Module connection pins (Digital Pins)
#define CLK 5
#define DIO 4

#define blocked LOW
#define cleared HIGH

#define mmpermile 1609344  // (5280 * 12 * 25.4)
#define dist 50 //mm
#define scale 220
#define mshour 3600000
//#define scalemile mmpermile/scale
//#define scaledist scalemile/dist    // this is inverted - should be dist/scalemile

float scalemile = mmpermile/scale;
float scaledist = scalemile/dist;

//mph=(mshour/elapsed)/scaledist

// The amount of time (in milliseconds) between tests
#define TEST_DELAY 1000

uint8_t data[] = { 0xff, 0xff, 0xff, 0xff };
uint8_t blank[] = { 0x00, 0x00, 0x00, 0x00 };
TM1637Display display(CLK, DIO);

void clear(){
   display.setSegments(blank);
}

void doMPH(int elapsed){
  float mphf=(mshour/elapsed)/scaledist;
  int mph = mphf + .5;
  display.showNumberDec(mph, false); 
  Serial.print("MPH=");Serial.println(mph,DEC);
  }

void setup(){
  display.setBrightness(0x0f);
  //Serial.begin(9600);
  pinMode(6, INPUT);     
  pinMode(7, INPUT);
  //Serial.println("Starting");
  int i;
  for (i=9;i<13;i++) { 
    pinMode(i, OUTPUT);     
    digitalWrite(i, LOW);    
  }
  // All segments on
  display.setSegments(data);
  delay(TEST_DELAY);
  clear();
  //Serial.println("Started");
}

int wait_on(int side){
  int start=millis();
  int elapsed;
  //Serial.println("Trigger");
  while (digitalRead(side)==cleared) {
    elapsed=millis()-start;
    if (elapsed > 10000) break; 
    display.showNumberDec(elapsed, false); 
    }
  //Serial.println("Clear");
  //Serial.print("Time=");Serial.println(elapsed,DEC);
  delay(500);
  doMPH(elapsed);
  delay(4500);
  clear();
  //Serial.println("Waiting");
  return elapsed;
  }

void loop(){
  int k;
  int t;
  t=0;
  digitalWrite(13,HIGH);

  if (digitalRead(6)==blocked and digitalRead(7)==cleared) t=wait_on(7);
  if (digitalRead(7)==blocked and digitalRead(6)==cleared) t=wait_on(6);
  digitalWrite(13,LOW);
}

