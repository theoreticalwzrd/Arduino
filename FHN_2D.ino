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
#include "Adafruit_HX8357.h"
#include <Adafruit_TFTLCD.h> // Hardware-specific library
#include <TouchScreen.h>

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

//Determines how big each interaction with the touchscreen is
#define BOXSIZE 40
#define PENRADIUS 15

int Nx=64;
int Ny=96;
int cellSize=5;
float w[64][96];
float vt[64][96];
float v[64][96];

float a = 2.2;
float b = 1;
float d=0.05;
float epsilon = 1;
float delta = 0;
float dt = 0.06;
float dx2 = 1.2;
 
double max=0;
double min=0;


void setup(void) {
  Serial.begin(9600);
  Serial.println(F("TFT LCD test"));
  
  //Initial conditions to get a spiral wave
  for (int i=30;i<35;i++){
    for (int j=0;j<37;j++){
      v[i][j] = 6;  
      w[i][j] = 0;
    } 
  }
  for( int i=36;i<38;i++){
    for (int j=0;j<30;j++){
     w[i][j] = 5;
    }
  }

  /**************From graphicstest **************/
  #ifdef USE_ADAFRUIT_SHIELD_PINOUT
    Serial.println(F("Using Adafruit 2.8\" TFT Arduino Shield Pinout"));
  #else
    Serial.println(F("Using Adafruit 2.8\" TFT Breakout Board Pinout"));
  #endif

    Serial.print("TFT size is "); Serial.print(tft.width()); Serial.print("x"); Serial.println(tft.height());
    tft.begin(HX8357D);
    Serial.begin(9600);
    Serial.println(dx2);
    tft.fillScreen(HX8357_BLACK);
    /******************************************/
}

//define the pressure that the touchscreen will measure
#define MINPRESSURE 1
#define MAXPRESSURE 1000

void loop(){
  int t=0;
  do{
    testFilledRects();
    
    //look here for how to add touch
    digitalWrite(13, HIGH);
    TSPoint p = ts.getPoint();
    digitalWrite(13, LOW);

    // if sharing pins, you'll need to fix the directions of the touchscreen pins
    //pinMode(XP, OUTPUT);
    pinMode(XM, OUTPUT);
    pinMode(YP, OUTPUT);
    //pinMode(YM, OUTPUT);

    // we have some minimum pressure we consider 'valid'
    // pressure of 0 means no pressing!
    if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
      // scale from 0->1023 to tft.width
      p.x = map(p.x, TS_MINX, TS_MAXX, tft.width(), 0);
      p.y = map(p.y, TS_MINY, TS_MAXY, tft.height(), 0);

      //Bins the pixels on the screen into different boxes to determine which cells
      //of the similation you are touching and will be activated
      int xt=p.x/cellSize;
      int yt=p.y/cellSize;
      vt[xt][yt]=3;
      v[xt][yt]=3;
  
      for(int i=0;i<PENRADIUS;i++){
        for(int j=0;j<20;j++){
          xt=(i*cos(j*PI/10)+p.x)/cellSize;
          yt=(j*sin(j*PI/10)+p.y)/cellSize;
          vt[xt][yt]=3;
          v[xt][yt]=3;
        }
      }
      
    Serial.println(vt[xt][yt]);
    Serial.println(v[xt][yt]);
  
    }
    t++;
  }while(true);
}


   
void testFilledRects() {
  for (int i=0;i<Nx;i++) {
    for(int j=0;j<Ny;j++){
        //calculate the value of each oscillator i,j
        double x=calc(i,j);
        if (x>max){
          max=x;
            Serial.println(max);
          }
        if(x<min){
          min=x;
            Serial.println(min);
        }
                
        //determine colors from the value of each oscillator.
        //We use a color scheme similar to the BZ reaction
        int green;
        int blue;
        green= (int)((x+6)*57/6+91);
        blue= (int)((x+6)*86/6+33);
        uint16_t color3=tft.color565(205,green,blue);
        tft.fillRect(i*cellSize,j*cellSize,cellSize,cellSize, color3);
      }
  }
  for(int i=0;i<Nx;i++){
    for(int j=0;j<Ny;j++){
      copy(i,j);
    }
  }  
}

//Integration of an oscillator with periodic boundary conditions and four nearest neighbor coupling
double calc(int p, int q){
  if(p==0&&q>0&&q<Ny-1){
    vt[p][q] = v[p][q] + dt * (-1*v[p][q]*(v[p][q]-a)*(v[p][q]+a)-w[p][q]) + dt/dx2 *(2*v[1][q]+v[p][q-1]+v[p][q+1]-4 * v[p][q]);
    w[p][q] = w[p][q] + dt * epsilon*(b*v[p][q]-d*w[p][q]-delta);
  }
  else if(q==0&&p>0&&p<Nx-1){
    vt[p][q] = v[p][q] + dt * (-1*v[p][q]*(v[p][q]-a)*(v[p][q]+a)-w[p][q]) + dt/dx2 *(v[p+1][q]+v[p-1][q]+2*v[p][1]-4 * v[p][q]);
    w[p][q] = w[p][q] + dt * epsilon*(b*v[p][q]-d*w[p][q]-delta);
  }
  else if(p==Nx-1&&q>0&&q<Ny-1){
    vt[p][q] = v[p][q] + dt * (-1*v[p][q]*(v[p][q]-a)*(v[p][q]+a)-w[p][q]) + dt/dx2 *(2*v[Nx-2][q]+v[p][q-1]+v[p][q+1]-4 * v[p][q]);
    w[p][q] = w[p][q] + dt * epsilon*(b*v[p][q]-d*w[p][q]-delta);
  }else if(q==Ny-1&&p>0&&p<Nx-1){
    vt[p][q] = v[p][q] + dt * (-1*v[p][q]*(v[p][q]-a)*(v[p][q]+a)-w[p][q]) + dt/dx2 *(v[p+1][q]+v[p-1][q]+2*v[p][Ny-2]-4 * v[p][q]);
    w[p][q] = w[p][q] + dt * epsilon*(b*v[p][q]-d*w[p][q]-delta);
  
  }
  //BC for four corners
  else if(p==0&&q==0){
    vt[p][q] = v[p][q] + dt * (-1*v[p][q]*(v[p][q]-a)*(v[p][q]+a)-w[p][q]) + dt/dx2 *(2*v[0][1]+2*v[1][0]-4 * v[p][q]);
    w[p][q] = w[p][q] + dt * epsilon*(b*v[p][q]-d*w[p][q]-delta);
  }
  else if(p==0&&q==Ny-1){
    vt[p][q] = v[p][q] + dt * (-1*v[p][q]*(v[p][q]-a)*(v[p][q]+a)-w[p][q]) + dt/dx2 *(2*v[0][Ny-2]+2*v[1][Ny-1]-4 * v[p][q]);
    w[p][q] = w[p][q] + dt * epsilon*(b*v[p][q]-d*w[p][q]-delta);
  }
  else if(p==Nx-1&&q==0){
    vt[p][q] = v[p][q] + dt * (-1*v[p][q]*(v[p][q]-a)*(v[p][q]+a)-w[p][q]) + dt/dx2 *(2*v[Nx-2][0]+2*v[Nx-1][1]-4 * v[p][q]);
    w[p][q] = w[p][q] + dt * epsilon*(b*v[p][q]-d*w[p][q]-delta);
  }
  else if(p==Nx-1&&q==Ny-1){   
    vt[p][q] = v[p][q] + dt * (-1*v[p][q]*(v[p][q]-a)*(v[p][q]+a)-w[p][q]) + dt/dx2 *(2*v[Nx-1][Ny-2]+2*v[Nx-2][Ny-1]-4 * v[p][q]);
    w[p][q] = w[p][q] + dt * epsilon*(b*v[p][q]-d*w[p][q]-delta);
  }
  else{
    vt[p][q] = v[p][q] + dt * (-1*v[p][q]*(v[p][q]-a)*(v[p][q]+a)-w[p][q]) + dt /dx2*(v[p+1][q]+v[p-1][q]+v[p][q+1]+v[p][q-1]-4 * v[p][q]);
    w[p][q] = w[p][q] + dt * epsilon*(b*v[p][q]-d*w[p][q]-delta);
  }
  return vt[p][q];
}


double copy(int p, int q){
  v[p][q] = vt[p][q];
}
