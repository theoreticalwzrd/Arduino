//*****************************************//
//   This code was written by A.J. Welsh   //
//   and is included in the Supplementary  //
//   Material for Chaos "Simulating Waves, //
//   Chaos and Synchronization with a      //
//   Microcontroller." It was last updated //
//   May 31, 2019.                         //
//   Contact: Andrea.Jayne.Welsh@gmail.com //
//*****************************************//

#include <Adafruit_NeoPixel.h>
#include <Adafruit_GFX.h>

#ifndef PSTR
 #define PSTR
#endif

#define PIN 6

int photocellPin = 0;     // the cell and 10K pulldown are connected to a0
int photocellReading;     // the analog reading from the sensor divider

float a = 0.1;
float b = 0.5;
float epsilon = 0.0095;
float delta = 0;
float dt = 0.4;
float dx2 = 1;
double impulse=0;
double min=100;


float w[60];
float vt[60];
float v[60];
float vr[60];
int i = 0;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, 6, NEO_GRB + NEO_KHZ800);


void setup(){
  strip.begin();
  //Initial condtions for a wave propegating in one direction
  do{
    vt[i] = 0;
    w[i] = 0;
    v[i] = 0;
    vr[i] = 0;
    i = i +1;
  } while(i < 60);
  i = 8;
  do{
    v[i] = 0.5;
    i = i + 1;
  }while(i < 13);

  i = 3;
  do{
    w[i]= 0.6;
    i = i + 1;
  }while(i < 5);
  Serial.begin(9600);
  Serial.println(dx2);
}


void loop(){
  //Reading in the photocell to find the min value it is recording
  photocellReading = analogRead(photocellPin);  
  if(photocellReading<min){
    min=photocellReading;
  }

  //Printing current photocell value and min value to serial
  Serial.print("Analog reading = ");
  Serial.println(photocellReading);
  Serial.println(min);
  i = 0 ;
  //start reading the photocell after the simulation has begun
  if(t>20){
    //scale the photocell reading so that it is within model range to act as an impulse
    impulse =-(photocellReading-1023)/130.;
    Serial.print("impulse = ");
    Serial.println(impulse);
    Serial.println(t);
  }
  //intergation of the FHN model
  //the impulse will affect the integration of oscillator zero
  vt[i] = v[i] + dt * (v[i]*(a-v[i])*(v[i]-1)-w[i]) + dt *(v[i+1]+v[59]-2 * v[i])/dx2+impulse;
  w[i] = w[i] + dt * epsilon*(b*v[i]-w[i]-delta);
  Serial.println(vt[0]);
  i = 59 ;
  vt[i] = v[i] + dt * (v[i]*(a-v[i])*(v[i]-1)-w[i]) + dt *(v[0]+v[i-1]-2 * v[i])/dx2;
  w[i] = w[i] + dt * epsilon*(b*v[i]-w[i]-delta);
  i = 1;
  do {
    vt[i] = v[i] + dt * (v[i]*(a-v[i])*(v[i]-1)-w[i]) + dt *(v[i+1]+v[i-1]-2 * v[i])/dx2;
    w[i] = w[i] + dt * epsilon*(b*v[i]-w[i]-delta);
    i = i+1;
  }while (i < 59);
  i = 0;
  do {
    v[i] = vt[i];
    i = i+1;
  } while ( i < 60);

  //determine color of each LED dependant on v of each oscillator
  i = 0;
  do {
    vr[i] = floor(100 * v[i]);
    i = i+1;
  } while ( i < 60);
  
  i = 0;
  do {
    strip.setPixelColor(i, vr[i],0,0);
    i = i+1;
  } while ( i < 60);
  //display LEDs   
  strip.setBrightness(30);
  strip.show();
 }
