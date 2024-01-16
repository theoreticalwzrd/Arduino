//Updated May 30 2019. Most recent update.

#include <Adafruit_NeoPixel.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#include <Gaussian.h>
#ifndef PSTR
 #define PSTR
#endif

#define PIN 6

Adafruit_NeoPixel strip = Adafruit_NeoPixel(24, 6, NEO_GRB + NEO_KHZ800);
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(8, 8, 0,
  NEO_MATRIX_TOP     + NEO_MATRIX_RIGHT +
  NEO_MATRIX_COLUMNS + NEO_MATRIX_PROGRESSIVE,
  NEO_GRB            + NEO_KHZ800);

const int L=8;
boolean NN= true;
const float pi=4*atan(1.);
double adj[L][L][L][L];
double theta[L][L];
double theta_old[L][L];
double sum[L][L];
double w[L][L];
int phi[24];
int M;
int t=0;
double g=10;//.1;

const uint16_t colors[] = {
  matrix.Color(255, 0, 0), matrix.Color(0, 255, 0), matrix.Color(0, 0, 255) };

void setup() {
  Gaussian myGaussian(3*pi/2,pi/2);
  for(int i =0; i<L; i++){
    for(int j=0;j<L;j++){
      int random2 = random(0,6280);
      theta[i][j]=(double)random2/1000;
      w[i][j]=myGaussian.random();
   }
 }
 for (int i=0; i<24;i++){
   phi[i]=0; 
 }
 strip.begin();
 strip.show(); // Initialize all pixels to 'off'
 strip.setBrightness(100);
 matrix.begin();
 matrix.setTextWrap(false);
 matrix.setBrightness(20);
 matrix.setTextColor(colors[0]);
 /*for (int i=0; i<L;i++){
   for(int j=0; j<L;j++){
     for(int q=0;q<L;q++){
       for (int p=0;p<L;p++){
	         adj[i][j][q][p]=1;
        /*if((abs(i-q)==1&&j==p)||(abs(j-p)==1&&i==q)){
           adj[i][j][q][p]=1;
        }if((abs(i-q)==7&&j==p)||(abs(j-p)==7&&i==q)){
              adj[i][j][q][p]=1;  
        }
	  
        }
      }
    }
  }

*/




/*for (int i=0;i<L;i++){
  for (int j=0;j<L;j++){
    for (int q=0;q<L;q++){
      for (int p=0;p<L;p++){
        adj[i][j][q][p]=0;
        if(NN){
          if((abs(i-q)==1&&j==p)||(abs(j-p)==1&&i==q)){
             adj[i][j][q][p]=1/2;
          }if((abs(i-q)==7&&j==p)||(abs(j-p)==7&&i==q)){
             adj[i][j][q][p]=1/2;
          }
        }else{
         if(i!=q||j!=p){
          adj[i][j][q][p]=1/2;
         }
        }
      }
    }
  }
}*/
}
void loop(){
 for(int i=0;i<24;i++){
   phi[i]=0; 
  }
  
  for (int i=0;i<L;i++){
    for (int j=0;j<L;j++){
     theta_old[i][j]=theta[i][j]; 
    }
  }
  for (int i=0; i<L;i++){
    for (int j=0;j<L;j++){
     sum[i][j]=0;
       if(NN){
         if(i==0&&j>0&&j<7){
           sum[i][j]=sin(theta_old[0][j+1]-theta_old[i][j])+sin(theta_old[0][j-1]-theta_old[i][j])+sin(theta_old[7][j]-theta_old[i][j])+sin(theta_old[1][j]-theta_old[i][j]);
         } else if(j==0&&i>0&&i<7){
           sum[i][j]=sin(theta_old[i+1][0]-theta_old[i][j])+sin(theta_old[i-1][0]-theta_old[i][j])+sin(theta_old[i][7]-theta_old[i][j])+sin(theta_old[i][1]-theta_old[i][j]);
         } else if(i==7&&j>0&&j<7){
           sum[i][j]=sin(theta_old[7][j+1]-theta_old[i][j])+sin(theta_old[7][j-1]-theta_old[i][j])+sin(theta_old[0][j]-theta_old[i][j])+sin(theta_old[6][j]-theta_old[i][j]);
         } else if(j==7&&i>0&&i<7){
           sum[i][j]=sin(theta_old[i+1][7]-theta_old[i][j])+sin(theta_old[i-1][7]-theta_old[i][j])+sin(theta_old[i][0]-theta_old[i][j])+sin(theta_old[i][6]-theta_old[i][j]);
         }
        //BC for four corners
          else if(i==0&&j==0){
            sum[i][j]=sin(theta_old[0][1]-theta_old[i][j])+sin(theta_old[1][0]-theta_old[i][j])+sin(theta_old[7][0]-theta_old[i][j])+sin(theta_old[0][7]-theta_old[i][j]);
         }else if(i==0&&j==7){
            sum[i][j]=sin(theta_old[0][0]-theta_old[i][j])+sin(theta_old[7][7]-theta_old[i][j])+sin(theta_old[0][6]-theta_old[i][j])+sin(theta_old[1][7]-theta_old[i][j]);
         } else if(i==7&&j==0){
            sum[i][j]=sin(theta_old[0][0]-theta_old[i][j])+sin(theta_old[7][7]-theta_old[i][j])+sin(theta_old[6][0]-theta_old[i][j])+sin(theta_old[7][1]-theta_old[i][j]);
          }else if(i==7&&i==7){ 
            sum[i][j]=sin(theta_old[6][7]-theta_old[i][j])+sin(theta_old[7][6]-theta_old[i][j])+sin(theta_old[7][0]-theta_old[i][j])+sin(theta_old[0][7]-theta_old[i][j]);
          }else{
           sum[i][j]=sin(theta_old[i+1][j]-theta_old[i][j])+sin(theta_old[i-1][j]-theta_old[i][j])+sin(theta_old[i][j+1]-theta_old[i][j])+sin(theta_old[i][(j-1)]-theta_old[i][j]);
          }
       }else{
            for(int q=0;q<L;q++){
              for(int p=0;p<L;p++){
        
           sum[i][j]+=sin(theta_old[q][p]-theta_old[i][j]);
         }
     }
       }
    
     //for(int q=0;q<L;q++){
       // for(int p=0;p<L;p++){
        
         // sum[i][j]+=adj[i][j][p][j]*sin(theta_old[p][q]-theta_old[i][j]);
         //}
     //}
       
     theta[i][j]=theta[i][j]+.005*(w[i][j]+g*sum[i][j]);
     //Serial.println(theta[i][j]);
     while(theta[i][j]>2*pi){
       theta[i][j]=theta[i][j]-2*pi;
     }while(theta[i][j]<0){
       theta[i][j]=theta[i][j]+2*pi;
     }
     double ang = (theta[i][j]*180/pi)/15;
     int phase=(int) ang;
     for (int j=0;j<24;j++){     
       if (phase==j){
         phi[j]++;
       }
     } 
   }
  }
  
  for(int i=0; i<L;i++){
    for(int j=0;j<L;j++){
      int red=0;
      int blue=0;
      int green=0;
      if(theta[i][j]<=2*pi/3){
         green=theta[i][j]*255/(2*pi);
         red=255-theta[i][j]*255/(2*pi/3);
         blue=0;
      }else if(theta[i][j]<=4*pi/3){
         green=255-(theta[i][j]-2*pi/3)*255/(2*pi/3);
         blue=(theta[i][j]-2*pi/3)*255/(2*pi/3);
         red=0;
      }   //.5pi-1.5pi
     else{
         blue=255-(theta[i][j]-4*pi/3)*255/(2*pi/3);
         red=(theta[i][j]-4*pi/3)*255/(2*pi/3);
         green=0;
      }
     matrix.setPixelColor(i+j*L, red, green, blue);
    }
  }
  
  for (int i=0;i<24;i++){
    int red=0;;//=phi[i]*5;
    int blue=0;//phi[i]*5;
    int green=0;//phi[i]*5;
    
    if(i<8){
      red=(255-(i*32-1))*phi[i]/64;
      green=(i*32-1)*phi[i]/64;
      blue=0;
    }else if(i<16){
      green=(255-((i-8)*32-1))*phi[i]/64;
      blue=((i-8)*32-1)*phi[i]/64;
      red=0;
    }else {
      blue=phi[i]*(255-(i-16)*32-1)/64;
      red=((i-16)*32-1)*phi[i]/64;
      green=0;
    }
    strip.setPixelColor(i,red,green,blue);
    if(phi[i]==0){
      strip.setPixelColor(i,0,0,0);
    }   
  }  
  matrix.show();
  strip.show();   
}
