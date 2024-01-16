//*****************************************//
//   This code was written by A.J. Welsh   //
//   and is included in the Supplementary  //
//   Material for Chaos "Simulating Waves, //
//   Chaos and Synchronization with a      //
//   Microcontroller." It was last updated //
//   May 31, 2019.                         //
//   Contact: Andrea.Jayne.Welsh@gmail.com //
//*****************************************//


/**************From graphicstest **************/

// IMPORTANT: Adafruit_TFTLCD LIBRARY MUST BE SPECIFICALLY
// CONFIGURED FOR EITHER THE TFT SHIELD OR THE BREAKOUT BOARD.
// SEE RELEVANT COMMENTS IN Adafruit_TFTLCD.h FOR SETUP.

#include <Adafruit_GFX.h>    // Core graphics library
#include <SPI.h>
#include "Adafruit_HX8357.h" //include for touchscreen?
#include <Adafruit_TFTLCD.h> // Hardware-specific library
#include "TouchScreen.h"

// The control pins for the LCD can be assigned to any digital or
// analog pins...but we'll use the analog pins as this allows us to
// double up the pins with the touch screen (see the TFT paint example).
#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0

#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin
/******************************************/


/**************From tftpaint **************/
// These are the four touchscreen analog pins
#define YP A2  // must be an analog pin, use "An" notation!
#define XM A3  // must be an analog pin, use "An" notation!
#define YM 7   // can be a digital pin
#define XP 8   // can be a digital pin

// When using the BREAKOUT BOARD only, use these 8 data lines to the LCD:
// For the Arduino Uno, Duemilanove, Diecimila, etc.:
//   D0 connects to digital pin 8  (Notice these are
//   D1 connects to digital pin 9   NOT in order!)
//   D2 connects to digital pin 2
//   D3 connects to digital pin 3
//   D4 connects to digital pin 4
//   D5 connects to digital pin 5
//   D6 connects to digital pin 6
//   D7 connects to digital pin 7
// For the Arduino Mega, use digital pins 22 through 29
// (on the 2-row header at the end of the board).

// For the Arduino Due, use digital pins 33 through 40
// (on the 2-row header at the end of the board).
//   D0 connects to digital pin 33
//   D1 connects to digital pin 34
//   D2 connects to digital pin 35
//   D3 connects to digital pin 36
//   D4 connects to digital pin 37
//   D5 connects to digital pin 38
//   D6 connects to digital pin 39
//   D7 connects to digital pin 40

// These are the four touchscreen analog pins
#define YP A2  // must be an analog pin, use "An" notation!
#define XM A3  // must be an analog pin, use "An" notation!
#define YM 7   // can be a digital pin
#define XP 8   // can be a digital pin


#define TS_MINX 110
#define TS_MINY 80
#define TS_MAXX 900
#define TS_MAXY 940

// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// For the one we're using, its 300 ohms across the X plate
//TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);


// The display uses hardware SPI, plus #9 & #10
#define TFT_RST -1  // dont use a reset pin, tie to arduino RST if you like
#define TFT_DC 9
#define TFT_CS 10

Adafruit_HX8357 tft = Adafruit_HX8357(TFT_CS, TFT_DC, TFT_RST);
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);


// Assign human-readable names to some common 16-bit color values:
#define  BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF


/****************************************/



float w;
float vt;
float v;
float actionPotential[480];

float a = 2.5;
float b = -1*a;
float c=0;
float I=0;
float epsilon =1;
float dt = 0.05;
int t=0;


void setup(void) {
  Serial.begin(9600);
  Serial.println(F("TFT LCD test"));
  tft.begin(HX8357D);
  tft.fillScreen(HX8357_BLACK);
  //intial conditions of the oscillator
  v = 6;  
  w = 5;

  #ifdef USE_ADAFRUIT_SHIELD_PINOUT
    Serial.println(F("Using Adafruit 2.8\" TFT Arduino Shield Pinout"));
  #else
    Serial.println(F("Using Adafruit 2.8\" TFT Breakout Board Pinout"));
  #endif
    Serial.print("TFT size is "); Serial.print(tft.width()); Serial.print("x"); Serial.println(tft.height());
}


void loop(){
  
    calc(); //calls the method that makes the calculations
    drawAP(255);//to draw the pulse
    drawAP(0);//to clear the pulse
    copy();//copies it over to the next location so we can see the history of the phases of the oscillator
    
/**************From Breakouttouchpaint **************/
  // Retrieve a point  
  TSPoint p = ts.getPoint();
 
  // we have some minimum pressure we consider 'valid'
  // pressure of 0 means no pressing!
  if (p.z < MINPRESSURE || p.z > MAXPRESSURE) {
     return;
  }
  Serial.print("\tPressure = ");
  Serial.println(p.z);
  v+=p.z/130;//touch will add a at most 10 at min 0 to the voltage
/*****************************************************/
}


   
void drawAP(int color2) {
  //Draws the pulse to the screen
  uint16_t color=tft.color565(color2,color2,color2);
  for(int i=0;i<480;i++){
    double y1=(actionPotential[i]+6)*20+50;
    double y2=(actionPotential[i+1]+6)*20+50;
    tft.drawLine(y2, i*2+2,y1, i*2, color);
 }
}


double calc(){
    //calculates the current value of the oscillator
    vt = v + dt * (-1*v*(v-a)*(v-b)-w);
    w = w + dt * epsilon*(v-c*w);
    actionPotential[0]=vt;
}

double copy(){
  for(int i=479;i>0;i--){
      actionPotential[i]=actionPotential[i-1];
  }
  v = vt;
}
