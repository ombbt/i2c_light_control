// Include the Wire library for I2C
#include <Wire.h>
//#include <Adafruit_PWMServoDriver.h>
#include <FastLED.h>

#define NUM_LEDS 60
#define DATA_PIN 22

// Define the array of leds
CRGB leds[NUM_LEDS];

// Adafruit_PWMServoDriver pwm1 = Adafruit_PWMServoDriver(0x40);
// LED on pin 13
const int top_led_pin_R = 5;
const int top_led_pin_G = 6;
const int top_led_pin_B = 7;

const int bot_led_pin_R = 2;
const int bot_led_pin_G = 3;
const int bot_led_pin_B = 4;


const int disco_led_pin_R = 8;
const int disco_led_pin_G = 9;
const int disco_led_pin_B = 10;


//////////////////////////////////////////////////////////////////////////////////////
//                                        piezo                                     //
//////////////////////////////////////////////////////////////////////////////////////
int analogPin = A0; //  connected to analog pin 0                 // outside leads to ground and +5V
int A0_val = 0;  // variable to store the value read
int A0_max = 0;
//unsigned long A0_timer1 = 0;
//unsigned long A0_timer2 = 0;



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////                                                   tableaux de couleurs                                                          //////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int rouge[3]={255, 0, 0};
int vert[3]={0, 255, 0};
int bleu[3]={0, 0, 255};

int darkrouge[3]={40, 0, 0};
int darkvert[3]={0, 40, 0};
int darkbleu[3]={0, 0, 40};
int darkorange[3]={30, 5, 0};


int verydarkrouge[3]={1, 0, 0};
int verydarkvert[3]={0, 1, 0};
int verydarkbleu[3]={0, 0, 1};
int verydarkorange[3]={5, 2, 0};


int noir[3]={0, 0, 0};
int blanc[3]={255, 255, 255};
int gris[3]={100, 100, 100};

int rose[3]={255, 0, 100};
int violet[3]={100, 0, 255};

int jaune[3]={255, 255, 0};
int soleil[3]={255, 200, 50};
int orange[3]={255, 100, 0};

int emeraude[3] = {0, 255, 50};
int turquoise[3] = {0, 200, 255};
int ciel[3] = {60, 220, 200};



// variables globles fonctionement generales du programme
int x;
bool lightonoff;
int mode; //un mode par musique (fonction loop)
int program; //corespond aux pads du controlleur midi (fonction loop)
int efect; //efect lumineux pads du controlleur midi
int efecton;
int efectfin; //=1 si un effect est activé
int namestatus;
int top_nothingtodo; // =1 si un effet a besoin d'un arrierre plan
int bot_nothingtodo;
int disco_nothingtodo;
unsigned long eventtimmer;
unsigned long timespend; //fonction loop: temp depuis le dernier message smbus reçu part le raspberry

int name_speedup;


int offset_list1[22]={0, 300, 600, 900, 1200, 1500, 1800, 2100, 2400, 2700, 3000, 3300, 3600, 3900, 4200, 4500, 4800, 5100, 5400, 5700, 6000, 6300};
int offset_list2[22]={0, 50, 100, 150, 200, 250, 300, 350, 400, 450, 500, 550, 600, 650, 700, 750, 800, 850, 900, 950, 1000, 1050};
int offset_list3[22]={0, 500, 1000, 1500, 2000, 1500, 1000, 500, 0, 500, 1000, 1500, 2000, 1500, 1000, 500, 0, 500, 1000, 1700, 800, 0};
int offset_list4[22]={0, 1000, 2000, 3000, 4000, 5000, 5900, 5000, 4000, 3000, 1500, 0, 1000, 2000, 3000, 4000, 5000, 5900, 5000, 4000, 3000, 1500};
int offset_list5[22]={0, 500, 1000, 1500, 2000, 2500, 3000, 3500, 4000, 4500, 5000, 5500, 6000, 6500, 7000, 7500, 8000, 8500, 9000, 9500, 10000, 10500};



int status_list1[22]={0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////                                               fonctions generales                                                               //////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void timeonefect( unsigned long finaltime){
  if(finaltime < timespend){
    efecton = 0;
  }
   if((finaltime - 20) > timespend){
    efectfin = 0;
  }
  if((finaltime - 20) > timespend and (finaltime - 10) < timespend){
    efectfin = 1;
  }
  if((finaltime - 10) < timespend){
    efectfin = 0;
  }
}

void programchange( int programnumber, unsigned long finaltime){
  if(finaltime < timespend){
    program = programnumber;
  }
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////                                                        fonctions sorties pwm ( program et efect ) top                                            /////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void top_fadetofix(int startcolor, int endcolor, unsigned long initime, unsigned long finaltime) {
  int i;
  float advance;
  advance = 1000*initime/finaltime;
  if(advance < 1000){
    i = startcolor*(1000 - (advance))/1000 + endcolor*(advance)/1000;
    analogWrite(top_led_pin_R, i);
    analogWrite(top_led_pin_G, 0);
    analogWrite(top_led_pin_B, 0);
  }
  if(advance >= 1000){
    analogWrite(top_led_pin_R, endcolor);
    analogWrite(top_led_pin_G, 0);
    analogWrite(top_led_pin_B, 0);
  }
}

int top_fadetofix2_timeset;
unsigned long top_fadetofix2_timmer1;
unsigned long top_fadetofix2_timmer2;


void top_fadetofix2(int startcolor[], int endcolor[], unsigned long finaltime) {
  int r;
  int g;
  int b;
  float advance;
  if(top_fadetofix2_timeset == 0){
    top_fadetofix2_timmer1 = millis();
    top_fadetofix2_timeset = 1;
  }
  top_fadetofix2_timmer2 = millis() - top_fadetofix2_timmer1;
  advance = 1000*top_fadetofix2_timmer2/finaltime;
  if(advance < 1000){
    r = startcolor[0]*(1000 - (advance))/1000 + endcolor[0]*(advance)/1000;
    g = startcolor[1]*(1000 - (advance))/1000 + endcolor[1]*(advance)/1000;
    b = startcolor[2]*(1000 - (advance))/1000 + endcolor[2]*(advance)/1000;
    analogWrite(top_led_pin_R, r);
    analogWrite(top_led_pin_G, g);
    analogWrite(top_led_pin_B, b);
  }
  if(advance >= 1000){
    analogWrite(top_led_pin_R, endcolor[0]);
    analogWrite(top_led_pin_G, endcolor[1]);
    analogWrite(top_led_pin_B, endcolor[2]);
  }
}


unsigned long top_strombo_timmer;   //a tester aussi en variables globales
int top_strombo_timeset;
int top_strombo_status;

void top_strombo(int oncolor[], int offcolor[], unsigned long ontime, unsigned long offtime) {
  top_nothingtodo = 0;  
  if(top_strombo_timeset == 0){
    top_strombo_timmer = millis();
    top_strombo_timeset = 1;
    }
  if(top_strombo_status == 1){
    analogWrite(top_led_pin_R, oncolor[0]);
    analogWrite(top_led_pin_G, oncolor[1]);
    analogWrite(top_led_pin_B, oncolor[2]);
    if(millis() > (ontime + top_strombo_timmer)){
      top_strombo_status = 0;
      top_strombo_timmer = millis();
    }
  }
  if(top_strombo_status == 0){
    analogWrite(top_led_pin_R, offcolor[0]);
    analogWrite(top_led_pin_G, offcolor[1]);
    analogWrite(top_led_pin_B, offcolor[2]);
    if(millis() > offtime + top_strombo_timmer){
      top_strombo_status = 1;
      top_strombo_timmer = millis();
    }
  }
}

unsigned long top_backgroundstrombo_timmer;
int top_backgroundstrombo_timeset;
int top_backgroundstrombo_status;

void top_backgroundstrombo(int oncolor[], unsigned long ontime, unsigned long offtime) {
  if(top_backgroundstrombo_timeset == 0){
    top_backgroundstrombo_timmer = millis();
    top_backgroundstrombo_timeset = 1;
    }
  if(top_backgroundstrombo_status == 1){
    top_nothingtodo = 0;
    analogWrite(top_led_pin_R, oncolor[0]);
    analogWrite(top_led_pin_G, oncolor[1]);
    analogWrite(top_led_pin_B, oncolor[2]);
    if(millis() > (ontime + top_backgroundstrombo_timmer)){
      top_backgroundstrombo_status = 0;
      top_backgroundstrombo_timmer = millis();
    }
  }
  if(top_backgroundstrombo_status == 0){
    top_nothingtodo = 1;
    if(millis() > offtime + top_backgroundstrombo_timmer){
      top_backgroundstrombo_status = 1;
      top_backgroundstrombo_timmer = millis();
    }
  }
}

unsigned long top_someflasch_timmer;   //a tester aussi en variables globales
int top_someflasch_timeset;
int top_someflasch_status;
int top_someflasch_activate;
int top_someflasch_active;

void top_someflasch(int oncolor[], unsigned long ontime, unsigned long offtime) {
  if(top_someflasch_timeset == 0){
    top_someflasch_timmer = millis();
    top_someflasch_timeset = 1;
    }
  if(top_someflasch_status == 1){
    top_nothingtodo = 0;
    top_someflasch_active = 1;
    analogWrite(top_led_pin_R, oncolor[0]);
    analogWrite(top_led_pin_G, oncolor[1]);
    analogWrite(top_led_pin_B, oncolor[2]);
    if(millis() > (ontime + top_someflasch_timmer)){
      top_someflasch_status = 0;
      top_someflasch_timmer = millis();
    }
  }
  if(top_someflasch_status == 0){
    top_nothingtodo = 1;
    top_someflasch_active = 0;
    if(millis() > offtime + top_someflasch_timmer){
      top_someflasch_status = 1;
      top_someflasch_timmer = millis();
    }
  }
}

unsigned long top_someblack_timmer;   //a tester aussi en variables globales
int top_someblack_timeset;
int top_someblack_status;
int top_someblack_activate;

void top_someblack(int oncolor[], unsigned long ontime, unsigned long offtime) {
  if(top_someblack_timeset == 0){
    top_someblack_timmer = millis();
    top_someblack_timeset = 1;
    }
  if(top_someblack_status == 1){
    top_nothingtodo = 0;
    analogWrite(top_led_pin_R, oncolor[0]);
    analogWrite(top_led_pin_G, oncolor[1]);
    analogWrite(top_led_pin_B, oncolor[2]);
    if(millis() > (ontime + top_someblack_timmer)){
      top_someblack_status = 0;
      top_someblack_timmer = millis();
    }
  }
  if(top_someblack_status == 0){
    top_nothingtodo = 1;
    if(millis() > offtime + top_someblack_timmer){
      top_someblack_status = 1;
      top_someblack_timmer = millis();
    }
  }
}


unsigned long top_somemulticolors_timmer;   //a tester aussi en variables globales
int top_somemulticolors_timeset;
int top_somemulticolors_status;
int top_somemulticolors_activate;
int top_somemulticolors_active;

void top_somemulticolors(int color1[], int color2[], int color3[], int color4[], unsigned long time1, unsigned long time2, unsigned long time3, unsigned long time4, unsigned long donothingtime) {
  if(top_somemulticolors_timeset == 0){
    top_somemulticolors_timmer = millis();
    top_somemulticolors_timeset = 1;
    }
  if(top_somemulticolors_status == 0){
    top_nothingtodo = 0;
    top_somemulticolors_active = 1;
    analogWrite(top_led_pin_R, color1[0]);
    analogWrite(top_led_pin_G, color1[1]);
    analogWrite(top_led_pin_B, color1[2]);
    if(millis() > (top_somemulticolors_timmer + time1)){
      top_somemulticolors_status = 1;
      top_somemulticolors_timmer = millis();
    }
  }
  if(top_somemulticolors_status == 1){
    top_nothingtodo = 0;
    top_somemulticolors_active = 1;
    analogWrite(top_led_pin_R, color2[0]);
    analogWrite(top_led_pin_G, color2[1]);
    analogWrite(top_led_pin_B, color2[2]);
    if(millis() > top_somemulticolors_timmer + time2){
      top_somemulticolors_status = 2;
      top_somemulticolors_timmer = millis();
    }
  }
  if(top_somemulticolors_status == 2){
    top_nothingtodo =0;
    top_somemulticolors_active = 1;
    analogWrite(top_led_pin_R, color3[0]);
    analogWrite(top_led_pin_G, color3[1]);
    analogWrite(top_led_pin_B, color3[2]);
    if(millis() > top_somemulticolors_timmer + time3){
      top_somemulticolors_status = 3;
      top_somemulticolors_timmer = millis();
    }
  }
  if(top_somemulticolors_status == 3){
    top_nothingtodo =0;
    top_somemulticolors_active = 1;
    analogWrite(top_led_pin_R, color4[0]);
    analogWrite(top_led_pin_G, color4[1]);
    analogWrite(top_led_pin_B, color4[2]);
    if(millis() > top_somemulticolors_timmer + time4){
      top_somemulticolors_status = 4;
      top_somemulticolors_timmer = millis();
    }
  }
  if(top_somemulticolors_status == 4){
    top_nothingtodo =1;
    top_somemulticolors_active = 0;
    if(millis() > top_somemulticolors_timmer + donothingtime){
      top_somemulticolors_status = 0;
      top_somemulticolors_timmer = millis();
    }
  }
}


unsigned long top_colors3_timmer;   //a tester aussi en variables globales
int top_colors3_timeset;
int top_colors3_status;

void top_colors3(int color1[], int color2[], int color3[], unsigned long time1, unsigned long time2, unsigned long time3) {
  if(top_colors3_timeset == 0){
    top_colors3_timmer = millis();
    top_colors3_timeset = 1;
    }
  if(top_colors3_status == 0){
    analogWrite(top_led_pin_R, color1[0]);
    analogWrite(top_led_pin_G, color1[1]);
    analogWrite(top_led_pin_B, color1[2]);
    if(millis() > (top_colors3_timmer + time1)){
      top_colors3_status = 1;
      top_colors3_timmer = millis();
    }
  }
  if(top_colors3_status == 1){
    analogWrite(top_led_pin_R, color2[0]);
    analogWrite(top_led_pin_G, color2[1]);
    analogWrite(top_led_pin_B, color2[2]);
    if(millis() > top_colors3_timmer + time2){
      top_colors3_status = 2;
      top_colors3_timmer = millis();
    }
  }
  if(top_colors3_status == 2){
    analogWrite(top_led_pin_R, color3[0]);
    analogWrite(top_led_pin_G, color3[1]);
    analogWrite(top_led_pin_B, color3[2]);
    if(millis() > top_colors3_timmer + time3){
      top_colors3_status = 0;
      top_colors3_timmer = millis();
    }
  }
}


unsigned long top_colors8_timmer;   //a tester aussi en variables globales
int top_colors8_timeset;
int top_colors8_status;

void top_colors8(int color1[], int color2[], int color3[], int color4[], int color5[], int color6[], int color7[], int color8[], unsigned long time1, unsigned long time2, unsigned long time3, unsigned long time4, unsigned long time5, unsigned long time6, unsigned long time7, unsigned long time8) {
  if(top_colors8_timeset == 0){
    top_colors8_timmer = millis();
    top_colors8_timeset = 1;
    }
  if(top_colors8_status == 0){
    analogWrite(top_led_pin_R, color1[0]);
    analogWrite(top_led_pin_G, color1[1]);
    analogWrite(top_led_pin_B, color1[2]);
    if(millis() > (top_colors8_timmer + time1)){
      top_colors8_status = 1;
      top_colors8_timmer = millis();
    }
  }
  if(top_colors8_status == 1){
    analogWrite(top_led_pin_R, color2[0]);
    analogWrite(top_led_pin_G, color2[1]);
    analogWrite(top_led_pin_B, color2[2]);
    if(millis() > top_colors8_timmer + time2){
      top_colors8_status = 2;
      top_colors8_timmer = millis();
    }
  }
  if(top_colors8_status == 2){
    analogWrite(top_led_pin_R, color3[0]);
    analogWrite(top_led_pin_G, color3[1]);
    analogWrite(top_led_pin_B, color3[2]);
    if(millis() > top_colors8_timmer + time3){
      top_colors8_status = 3;
      top_colors8_timmer = millis();
    }
  }
  if(top_colors8_status == 3){
    analogWrite(top_led_pin_R, color4[0]);
    analogWrite(top_led_pin_G, color4[1]);
    analogWrite(top_led_pin_B, color4[2]);
    if(millis() > top_colors8_timmer + time4){
      top_colors8_status = 4;
      top_colors8_timmer = millis();
    }
  }
  if(top_colors8_status == 4){
    analogWrite(top_led_pin_R, color5[0]);
    analogWrite(top_led_pin_G, color5[1]);
    analogWrite(top_led_pin_B, color5[2]);
    if(millis() > top_colors8_timmer + time5){
      top_colors8_status = 5;
      top_colors8_timmer = millis();
    }
  }
  if(top_colors8_status == 5){
    analogWrite(top_led_pin_R, color6[0]);
    analogWrite(top_led_pin_G, color6[1]);
    analogWrite(top_led_pin_B, color6[2]);
    if(millis() > top_colors8_timmer + time6){
      top_colors8_status = 6;
      top_colors8_timmer = millis();
    }
  }
  if(top_colors8_status == 6){
    analogWrite(top_led_pin_R, color7[0]);
    analogWrite(top_led_pin_G, color7[1]);
    analogWrite(top_led_pin_B, color7[2]);
    if(millis() > top_colors8_timmer + time7){
      top_colors8_status = 7;
      top_colors8_timmer = millis();
    }
  }
  if(top_colors8_status == 7){
    analogWrite(top_led_pin_R, color8[0]);
    analogWrite(top_led_pin_G, color8[1]);
    analogWrite(top_led_pin_B, color8[2]);
    if(millis() > top_colors8_timmer + time8){
      top_colors8_status = 0;
      top_colors8_timmer = millis();
    }
  }
}

  
//fadesirenvariables
int top_fadesiren_status;
int top_fadesiren_timeset;
unsigned long top_fadesiren_timmer1;
unsigned long top_fadesiren_timmer2;

void top_fadesiren(int startcolor[], int endcolor[], unsigned long finaltime) {
  int r;
  int g;
  int b;
  float advance;
  if(top_fadesiren_timeset == 0){
    top_fadesiren_timmer1 = millis();
    top_fadesiren_timeset = 1;
  }
  top_fadesiren_timmer2 = millis() - top_fadesiren_timmer1;
  advance = 1000*top_fadesiren_timmer2/finaltime;
  if(top_fadesiren_status == 1){
    r = startcolor[0]*(1000 - (advance))/1000 + endcolor[0]*(advance)/1000;
    g = startcolor[1]*(1000 - (advance))/1000 + endcolor[1]*(advance)/1000;
    b = startcolor[2]*(1000 - (advance))/1000 + endcolor[2]*(advance)/1000;

    analogWrite(top_led_pin_R, r);
    analogWrite(top_led_pin_G, g);
    analogWrite(top_led_pin_B, b);
    }
    
  if(top_fadesiren_status == 0){
    r = endcolor[0]*(1000 - (advance))/1000 + startcolor[0]*(advance)/1000;
    g = endcolor[1]*(1000 - (advance))/1000 + startcolor[1]*(advance)/1000;
    b = endcolor[2]*(1000 - (advance))/1000 + startcolor[2]*(advance)/1000;

    analogWrite(top_led_pin_R, r);
    analogWrite(top_led_pin_G, g);
    analogWrite(top_led_pin_B, b);
  }
  if(top_fadesiren_timmer2 > (finaltime)){
     top_fadesiren_timeset = 0;
     switch(top_fadesiren_status){
      case 0:
      top_fadesiren_status = 1;
      break;

      case 1:
      top_fadesiren_status = 0;
      break;
      
     }     
  }  
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////                                                        fonctions sorties analogWrite(bot_led_pin_B, oncolor[2]); ( program et efect ) bot                                            /////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*void bot_fadetofix(int startcolor, int endcolor, unsigned long initime, unsigned long finaltime) {
  int i;
  float advance;
  advance = 1000*initime/finaltime;
  if(advance < 1000){
    i = startcolor*(1000 - (advance))/1000 + endcolor*(advance)/1000;
    analogWrite(bot_led_pin_B, oncolor[2]);1.setanalogWrite(bot_led_pin_B, oncolor[2]);(0, r 0);
    pwm1.setPWM(2, g, 0);
    pwm1.setPWM(1, b 0);
  }
  if(advance >= 1000){
    pwm1.setPWM(0, r endcolor);
    pwm1.setPWM(2, g, 0);
    pwm1.setPWM(1, b 0);
  }
}
*/
int bot_fadetofix2_timeset;
unsigned long bot_fadetofix2_timmer1;
unsigned long bot_fadetofix2_timmer2;


void bot_fadetofix2(int startcolor[], int endcolor[], unsigned long finaltime) {
  int r;
  int g;
  int b;
  float advance;
  if(bot_fadetofix2_timeset == 0){
    bot_fadetofix2_timmer1 = millis();
    bot_fadetofix2_timeset = 1;
  }
  bot_fadetofix2_timmer2 = millis() - bot_fadetofix2_timmer1;
  advance = 1000*bot_fadetofix2_timmer2/finaltime;
  if(advance < 1000){
    r = startcolor[0]*(1000 - (advance))/1000 + endcolor[0]*(advance)/1000;
    g = startcolor[1]*(1000 - (advance))/1000 + endcolor[1]*(advance)/1000;
    b = startcolor[2]*(1000 - (advance))/1000 + endcolor[2]*(advance)/1000;
    analogWrite(bot_led_pin_R, r);
    analogWrite(bot_led_pin_G, g);
    analogWrite(bot_led_pin_B, b);
  }
  if(advance >= 1000){
    analogWrite(bot_led_pin_R, endcolor[0]);
    analogWrite(bot_led_pin_G, endcolor[1]);
    analogWrite(bot_led_pin_B, endcolor[2]);
  }
}


unsigned long bot_strombo_timmer;   //a tester aussi en variables globales
int bot_strombo_timeset;
int bot_strombo_status;

void bot_strombo(int oncolor[], int offcolor[], unsigned long ontime, unsigned long offtime) {
  bot_nothingtodo = 0;  
  if(bot_strombo_timeset == 0){
    bot_strombo_timmer = millis();
    bot_strombo_timeset = 1;
    }
  if(bot_strombo_status == 1){
    analogWrite(bot_led_pin_R, oncolor[0]);
    analogWrite(bot_led_pin_G, oncolor[1]);
    analogWrite(bot_led_pin_B, oncolor[2]);
    if(millis() > (ontime + bot_strombo_timmer)){
      bot_strombo_status = 0;
      bot_strombo_timmer = millis();
    }
  }
  if(bot_strombo_status == 0){
    analogWrite(bot_led_pin_R, offcolor[0]);
    analogWrite(bot_led_pin_G, offcolor[1]);
    analogWrite(bot_led_pin_B, offcolor[2]);
    if(millis() > offtime + bot_strombo_timmer){
      bot_strombo_status = 1;
      bot_strombo_timmer = millis();
    }
  }
}

unsigned long bot_backgroundstrombo_timmer;
int bot_backgroundstrombo_timeset;
int bot_backgroundstrombo_status;

void bot_backgroundstrombo(int oncolor[], unsigned long ontime, unsigned long offtime) {
  if(bot_backgroundstrombo_timeset == 0){
    bot_backgroundstrombo_timmer = millis();
    bot_backgroundstrombo_timeset = 1;
    }
  if(bot_backgroundstrombo_status == 1){
    bot_nothingtodo = 0;
    analogWrite(bot_led_pin_R, oncolor[0]);
    analogWrite(bot_led_pin_G, oncolor[1]);
    analogWrite(bot_led_pin_B, oncolor[2]);
    if(millis() > (ontime + bot_backgroundstrombo_timmer)){
      bot_backgroundstrombo_status = 0;
      bot_backgroundstrombo_timmer = millis();
    }
  }
  if(bot_backgroundstrombo_status == 0){
    bot_nothingtodo = 1;
    if(millis() > offtime + bot_backgroundstrombo_timmer){
      bot_backgroundstrombo_status = 1;
      bot_backgroundstrombo_timmer = millis();
    }
  }
}

unsigned long bot_someflasch_timmer;   //a tester aussi en variables globales
int bot_someflasch_timeset;
int bot_someflasch_status;
int bot_someflasch_activate;
int bot_someflasch_active;

void bot_someflasch(int oncolor[], unsigned long ontime, unsigned long offtime) {
  if(bot_someflasch_timeset == 0){
    bot_someflasch_timmer = millis();
    bot_someflasch_timeset = 1;
    }
  if(bot_someflasch_status == 1){
    bot_nothingtodo = 0;
    bot_someflasch_active = 1;
    analogWrite(bot_led_pin_R, oncolor[0]);
    analogWrite(bot_led_pin_G, oncolor[1]);
    analogWrite(bot_led_pin_B, oncolor[2]);
    if(millis() > (ontime + bot_someflasch_timmer)){
      bot_someflasch_status = 0;
      bot_someflasch_timmer = millis();
    }
  }
  if(bot_someflasch_status == 0){
    bot_nothingtodo = 1;
    bot_someflasch_active = 0;
    if(millis() > offtime + bot_someflasch_timmer){
      bot_someflasch_status = 1;
      bot_someflasch_timmer = millis();
    }
  }
}

unsigned long bot_someblack_timmer;   //a tester aussi en variables globales
int bot_someblack_timeset;
int bot_someblack_status;
int bot_someblack_activate;

void bot_someblack(int oncolor[], unsigned long ontime, unsigned long offtime) {
  if(bot_someblack_timeset == 0){
    bot_someblack_timmer = millis();
    bot_someblack_timeset = 1;
    }
  if(bot_someblack_status == 1){
    bot_nothingtodo = 0;
    analogWrite(bot_led_pin_R, oncolor[0]);
    analogWrite(bot_led_pin_G, oncolor[1]);
    analogWrite(bot_led_pin_B, oncolor[2]);
    if(millis() > (ontime + bot_someblack_timmer)){
      bot_someblack_status = 0;
      bot_someblack_timmer = millis();
    }
  }
  if(bot_someblack_status == 0){
    bot_nothingtodo = 1;
    if(millis() > offtime + bot_someblack_timmer){
      bot_someblack_status = 1;
      bot_someblack_timmer = millis();
    }
  }
}


unsigned long bot_somemulticolors_timmer;   //a tester aussi en variables globales
int bot_somemulticolors_timeset;
int bot_somemulticolors_status;
int bot_somemulticolors_activate;
int bot_somemulticolors_active;

void bot_somemulticolors(int color1[], int color2[], int color3[], int color4[], unsigned long time1, unsigned long time2, unsigned long time3, unsigned long time4, unsigned long donothingtime) {
  if(bot_somemulticolors_timeset == 0){
    bot_somemulticolors_timmer = millis();
    bot_somemulticolors_timeset = 1;
    }
  if(bot_somemulticolors_status == 0){
    bot_nothingtodo = 0;
    bot_somemulticolors_active = 1;
    analogWrite(bot_led_pin_R, color1[0]);
    analogWrite(bot_led_pin_G, color1[1]);
    analogWrite(bot_led_pin_B, color1[2]);
    if(millis() > (bot_somemulticolors_timmer + time1)){
      bot_somemulticolors_status = 1;
      bot_somemulticolors_timmer = millis();
    }
  }
  if(bot_somemulticolors_status == 1){
    bot_nothingtodo = 0;
    bot_somemulticolors_active = 1;
    analogWrite(bot_led_pin_R, color2[0]);
    analogWrite(bot_led_pin_G, color2[1]);
    analogWrite(bot_led_pin_B, color2[2]);
    if(millis() > bot_somemulticolors_timmer + time2){
      bot_somemulticolors_status = 2;
      bot_somemulticolors_timmer = millis();
    }
  }
  if(bot_somemulticolors_status == 2){
    bot_nothingtodo =0;
    bot_somemulticolors_active = 1;
    analogWrite(bot_led_pin_R, color3[0]);
    analogWrite(bot_led_pin_G, color3[1]);
    analogWrite(bot_led_pin_B, color3[2]);
    if(millis() > bot_somemulticolors_timmer + time3){
      bot_somemulticolors_status = 3;
      bot_somemulticolors_timmer = millis();
    }
  }
  if(bot_somemulticolors_status == 3){
    bot_nothingtodo =0;
    bot_somemulticolors_active = 1;
    analogWrite(bot_led_pin_R, color4[0]);
    analogWrite(bot_led_pin_G, color4[1]);
    analogWrite(bot_led_pin_B, color4[2]);
    if(millis() > bot_somemulticolors_timmer + time4){
      bot_somemulticolors_status = 4;
      bot_somemulticolors_timmer = millis();
    }
  }
  if(bot_somemulticolors_status == 4){
    bot_nothingtodo =1;
    bot_somemulticolors_active = 0;
    if(millis() > bot_somemulticolors_timmer + donothingtime){
      bot_somemulticolors_status = 0;
      bot_somemulticolors_timmer = millis();
    }
  }
}


unsigned long bot_colors3_timmer;   //a tester aussi en variables globales
int bot_colors3_timeset;
int bot_colors3_status;

void bot_colors3(int color1[], int color2[], int color3[], unsigned long time1, unsigned long time2, unsigned long time3) {
  if(bot_colors3_timeset == 0){
    bot_colors3_timmer = millis();
    bot_colors3_timeset = 1;
    }
  if(bot_colors3_status == 0){
    analogWrite(bot_led_pin_R, color1[0]);
    analogWrite(bot_led_pin_G, color1[1]);
    analogWrite(bot_led_pin_B, color1[2]);
    if(millis() > (bot_colors3_timmer + time1)){
      bot_colors3_status = 1;
      bot_colors3_timmer = millis();
    }
  }
  if(bot_colors3_status == 1){
    analogWrite(bot_led_pin_R, color2[0]);
    analogWrite(bot_led_pin_G, color2[1]);
    analogWrite(bot_led_pin_B, color2[2]);
    if(millis() > bot_colors3_timmer + time2){
      bot_colors3_status = 2;
      bot_colors3_timmer = millis();
    }
  }
  if(bot_colors3_status == 2){
    analogWrite(bot_led_pin_R, color3[0]);
    analogWrite(bot_led_pin_G, color3[1]);
    analogWrite(bot_led_pin_B, color3[2]);
    if(millis() > bot_colors3_timmer + time3){
      bot_colors3_status = 0;
      bot_colors3_timmer = millis();
    }
  }
}


unsigned long bot_colors8_timmer;   //a tester aussi en variables globales
int bot_colors8_timeset;
int bot_colors8_status;

void bot_colors8(int color1[], int color2[], int color3[], int color4[], int color5[], int color6[], int color7[], int color8[], unsigned long time1, unsigned long time2, unsigned long time3, unsigned long time4, unsigned long time5, unsigned long time6, unsigned long time7, unsigned long time8) {
  if(bot_colors8_timeset == 0){
    bot_colors8_timmer = millis();
    bot_colors8_timeset = 1;
    }
  if(bot_colors8_status == 0){
    analogWrite(bot_led_pin_R, color1[0]);
    analogWrite(bot_led_pin_G, color1[1]);
    analogWrite(bot_led_pin_B, color1[2]);
    if(millis() > (bot_colors8_timmer + time1)){
      bot_colors8_status = 1;
      bot_colors8_timmer = millis();
    }
  }
  if(bot_colors8_status == 1){
    analogWrite(bot_led_pin_R, color2[0]);
    analogWrite(bot_led_pin_G, color2[1]);
    analogWrite(bot_led_pin_B, color2[2]);
    if(millis() > bot_colors8_timmer + time2){
      bot_colors8_status = 2;
      bot_colors8_timmer = millis();
    }
  }
  if(bot_colors8_status == 2){
    analogWrite(bot_led_pin_R, color3[0]);
    analogWrite(bot_led_pin_G, color3[1]);
    analogWrite(bot_led_pin_B, color3[2]);
    if(millis() > bot_colors8_timmer + time3){
      bot_colors8_status = 3;
      bot_colors8_timmer = millis();
    }
  }
  if(bot_colors8_status == 3){
    analogWrite(bot_led_pin_R, color4[0]);
    analogWrite(bot_led_pin_G, color4[1]);
    analogWrite(bot_led_pin_B, color4[2]);
    if(millis() > bot_colors8_timmer + time4){
      bot_colors8_status = 4;
      bot_colors8_timmer = millis();
    }
  }
  if(bot_colors8_status == 4){
    analogWrite(bot_led_pin_R, color5[0]);
    analogWrite(bot_led_pin_G, color5[1]);
    analogWrite(bot_led_pin_B, color5[2]);
    if(millis() > bot_colors8_timmer + time5){
      bot_colors8_status = 5;
      bot_colors8_timmer = millis();
    }
  }
  if(bot_colors8_status == 5){
    analogWrite(bot_led_pin_R, color6[0]);
    analogWrite(bot_led_pin_G, color6[1]);
    analogWrite(bot_led_pin_B, color6[2]);
    if(millis() > bot_colors8_timmer + time6){
      bot_colors8_status = 6;
      bot_colors8_timmer = millis();
    }
  }
  if(bot_colors8_status == 6){
    analogWrite(bot_led_pin_R, color7[0]);
    analogWrite(bot_led_pin_G, color7[1]);
    analogWrite(bot_led_pin_B, color7[2]);
    if(millis() > bot_colors8_timmer + time7){
      bot_colors8_status = 7;
      bot_colors8_timmer = millis();
    }
  }
  if(bot_colors8_status == 7){
    analogWrite(bot_led_pin_R, color8[0]);
    analogWrite(bot_led_pin_G, color8[1]);
    analogWrite(bot_led_pin_B, color8[2]);
    if(millis() > bot_colors8_timmer + time8){
      bot_colors8_status = 0;
      bot_colors8_timmer = millis();
    }
  }
}

  
//fadesirenvariables
int bot_fadesiren_status;
int bot_fadesiren_timeset;
unsigned long bot_fadesiren_timmer1;
unsigned long bot_fadesiren_timmer2;

void bot_fadesiren(int startcolor[], int endcolor[], unsigned long finaltime) {
  int r;
  int g;
  int b;
  float advance;
  if(bot_fadesiren_timeset == 0){
    bot_fadesiren_timmer1 = millis();
    bot_fadesiren_timeset = 1;
  }
  bot_fadesiren_timmer2 = millis() - bot_fadesiren_timmer1;
  advance = 1000*bot_fadesiren_timmer2/finaltime;
  if(bot_fadesiren_status == 1){
    r = (startcolor[0]*(1000 - (advance))/1000 + endcolor[0]*(advance)/1000);
    g = (startcolor[1]*(1000 - (advance))/1000 + endcolor[1]*(advance)/1000);
    b = (startcolor[2]*(1000 - (advance))/1000 + endcolor[2]*(advance)/1000);

    analogWrite(bot_led_pin_R, r);
    analogWrite(bot_led_pin_G, g);
    analogWrite(bot_led_pin_B, b);
    }
    
  if(bot_fadesiren_status == 0){
    r = (endcolor[0]*(1000 - (advance))/1000 + startcolor[0]*(advance)/1000);
    g = (endcolor[1]*(1000 - (advance))/1000 + startcolor[1]*(advance)/1000);
    b = (endcolor[2]*(1000 - (advance))/1000 + startcolor[2]*(advance)/1000);

    analogWrite(bot_led_pin_R, r);
    analogWrite(bot_led_pin_G, g);
    analogWrite(bot_led_pin_B, b);
  }
  if(bot_fadesiren_timmer2 > (finaltime)){
     bot_fadesiren_timeset = 0;
     switch(bot_fadesiren_status){
      case 0:
      bot_fadesiren_status = 1;
      break;

      case 1:
      bot_fadesiren_status = 0;
      break;
      
     }     
  }  
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////                                                        fonctions sorties pwm ( program et efect ) disco                                            /////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void disco_fadetofix(int startcolor, int endcolor, unsigned long initime, unsigned long finaltime) {
  int i;
  float advance;
  advance = 1000*initime/finaltime;
  if(advance < 1000){
    i = startcolor*(1000 - (advance))/1000 + endcolor*(advance)/1000;
    analogWrite(disco_led_pin_R, i);
    analogWrite(disco_led_pin_G, 0);
    analogWrite(disco_led_pin_B, 0);
  }
  if(advance >= 1000){
    analogWrite(disco_led_pin_R, endcolor);
    analogWrite(disco_led_pin_G, 0);
    analogWrite(disco_led_pin_B, 0);
  }
}


int disco_fadetofix2_timeset;
unsigned long disco_fadetofix2_timmer1;
unsigned long disco_fadetofix2_timmer2;


void disco_fadetofix2(int startcolor[], int endcolor[], unsigned long finaltime) {
  int r;
  int g;
  int b;
  float advance;
  if(disco_fadetofix2_timeset == 0){
    disco_fadetofix2_timmer1 = millis();
    disco_fadetofix2_timeset = 1;
  }
  disco_fadetofix2_timmer2 = millis() - disco_fadetofix2_timmer1;
  advance = 1000*disco_fadetofix2_timmer2/finaltime;
  if(advance < 1000){
    r = startcolor[0]*(1000 - (advance))/1000 + endcolor[0]*(advance)/1000;
    g = startcolor[1]*(1000 - (advance))/1000 + endcolor[1]*(advance)/1000;
    b = startcolor[2]*(1000 - (advance))/1000 + endcolor[2]*(advance)/1000;
    analogWrite(disco_led_pin_R, r);
    analogWrite(disco_led_pin_G, g);
    analogWrite(disco_led_pin_B, b);
  }
  if(advance >= 1000){
    analogWrite(disco_led_pin_R, endcolor[0]);
    analogWrite(disco_led_pin_G, endcolor[1]);
    analogWrite(disco_led_pin_B, endcolor[2]);
  }
}

unsigned long disco_strombo_timmer;   //a tester aussi en variables globales
int disco_strombo_timeset;
int disco_strombo_status;

void disco_strombo(int oncolor[], int offcolor[], unsigned long ontime, unsigned long offtime) {
  disco_nothingtodo = 0;  
  if(disco_strombo_timeset == 0){
    disco_strombo_timmer = millis();
    disco_strombo_timeset = 1;
    }
  if(disco_strombo_status == 1){
    analogWrite(disco_led_pin_R, oncolor[0]);
    analogWrite(disco_led_pin_G, oncolor[1]);
    analogWrite(disco_led_pin_B, oncolor[2]);
    if(millis() > (ontime + disco_strombo_timmer)){
      disco_strombo_status = 0;
      disco_strombo_timmer = millis();
    }
  }
  if(disco_strombo_status == 0){
    analogWrite(disco_led_pin_R, offcolor[0]);
    analogWrite(disco_led_pin_G, offcolor[1]);
    analogWrite(disco_led_pin_B, offcolor[2]);
    if(millis() > offtime + disco_strombo_timmer){
      disco_strombo_status = 1;
      disco_strombo_timmer = millis();
    }
  }
}

unsigned long disco_backgroundstrombo_timmer;
int disco_backgroundstrombo_timeset;
int disco_backgroundstrombo_status;

void disco_backgroundstrombo(int oncolor[], unsigned long ontime, unsigned long offtime) {
  if(disco_backgroundstrombo_timeset == 0){
    disco_backgroundstrombo_timmer = millis();
    disco_backgroundstrombo_timeset = 1;
    }
  if(disco_backgroundstrombo_status == 1){
    disco_nothingtodo = 0;
    analogWrite(disco_led_pin_R, oncolor[0]);
    analogWrite(disco_led_pin_G, oncolor[1]);
    analogWrite(disco_led_pin_B, oncolor[2]);
    if(millis() > (ontime + disco_backgroundstrombo_timmer)){
      disco_backgroundstrombo_status = 0;
      disco_backgroundstrombo_timmer = millis();
    }
  }
  if(disco_backgroundstrombo_status == 0){
    disco_nothingtodo = 1;
    if(millis() > offtime + disco_backgroundstrombo_timmer){
      disco_backgroundstrombo_status = 1;
      disco_backgroundstrombo_timmer = millis();
    }
  }
}

unsigned long disco_someflasch_timmer;   //a tester aussi en variables globales
int disco_someflasch_timeset;
int disco_someflasch_status;
int disco_someflasch_activate;
int disco_someflasch_active;

void disco_someflasch(int oncolor[], unsigned long ontime, unsigned long offtime) {
  if(disco_someflasch_timeset == 0){
    disco_someflasch_timmer = millis();
    disco_someflasch_timeset = 1;
    }
  if(disco_someflasch_status == 1){
    disco_nothingtodo = 0;
    disco_someflasch_active = 1;
    analogWrite(disco_led_pin_R, oncolor[0]);
    analogWrite(disco_led_pin_G, oncolor[1]);
    analogWrite(disco_led_pin_B, oncolor[2]);
    if(millis() > (ontime + disco_someflasch_timmer)){
      disco_someflasch_status = 0;
      disco_someflasch_timmer = millis();
    }
  }
  if(disco_someflasch_status == 0){
    disco_nothingtodo = 1;
    disco_someflasch_active = 0;
    if(millis() > offtime + disco_someflasch_timmer){
      disco_someflasch_status = 1;
      disco_someflasch_timmer = millis();
    }
  }
}

unsigned long disco_someblack_timmer;   //a tester aussi en variables globales
int disco_someblack_timeset;
int disco_someblack_status;
int disco_someblack_activate;

void disco_someblack(int oncolor[], unsigned long ontime, unsigned long offtime) {
  if(disco_someblack_timeset == 0){
    disco_someblack_timmer = millis();
    disco_someblack_timeset = 1;
    }
  if(disco_someblack_status == 1){
    disco_nothingtodo = 0;
    analogWrite(disco_led_pin_R, oncolor[0]);
    analogWrite(disco_led_pin_G, oncolor[1]);
    analogWrite(disco_led_pin_B, oncolor[2]);
    if(millis() > (ontime + disco_someblack_timmer)){
      disco_someblack_status = 0;
      disco_someblack_timmer = millis();
    }
  }
  if(disco_someblack_status == 0){
    disco_nothingtodo = 1;
    if(millis() > offtime + disco_someblack_timmer){
      disco_someblack_status = 1;
      disco_someblack_timmer = millis();
    }
  }
}


unsigned long disco_somemulticolors_timmer;   //a tester aussi en variables globales
int disco_somemulticolors_timeset;
int disco_somemulticolors_status;
int disco_somemulticolors_activate;
int disco_somemulticolors_active;

void disco_somemulticolors(int color1[], int color2[], int color3[], int color4[], unsigned long time1, unsigned long time2, unsigned long time3, unsigned long time4, unsigned long donothingtime) {
  if(disco_somemulticolors_timeset == 0){
    disco_somemulticolors_timmer = millis();
    disco_somemulticolors_timeset = 1;
    }
  if(disco_somemulticolors_status == 0){
    disco_nothingtodo = 0;
    disco_somemulticolors_active = 1;
    analogWrite(disco_led_pin_R, color1[0]);
    analogWrite(disco_led_pin_G, color1[1]);
    analogWrite(disco_led_pin_B, color1[2]);
    if(millis() > (disco_somemulticolors_timmer + time1)){
      disco_somemulticolors_status = 1;
      disco_somemulticolors_timmer = millis();
    }
  }
  if(disco_somemulticolors_status == 1){
    disco_nothingtodo = 0;
    disco_somemulticolors_active = 1;
    analogWrite(disco_led_pin_R, color2[0]);
    analogWrite(disco_led_pin_G, color2[1]);
    analogWrite(disco_led_pin_B, color2[2]);
    if(millis() > disco_somemulticolors_timmer + time2){
      disco_somemulticolors_status = 2;
      disco_somemulticolors_timmer = millis();
    }
  }
  if(disco_somemulticolors_status == 2){
    disco_nothingtodo =0;
    disco_somemulticolors_active = 1;
    analogWrite(disco_led_pin_R, color3[0]);
    analogWrite(disco_led_pin_G, color3[1]);
    analogWrite(disco_led_pin_B, color3[2]);
    if(millis() > disco_somemulticolors_timmer + time3){
      disco_somemulticolors_status = 3;
      disco_somemulticolors_timmer = millis();
    }
  }
  if(disco_somemulticolors_status == 3){
    disco_nothingtodo =0;
    disco_somemulticolors_active = 1;
    analogWrite(disco_led_pin_R, color4[0]);
    analogWrite(disco_led_pin_G, color4[1]);
    analogWrite(disco_led_pin_B, color4[2]);
    if(millis() > disco_somemulticolors_timmer + time4){
      disco_somemulticolors_status = 4;
      disco_somemulticolors_timmer = millis();
    }
  }
  if(disco_somemulticolors_status == 4){
    disco_nothingtodo =1;
    disco_somemulticolors_active = 0;
    if(millis() > disco_somemulticolors_timmer + donothingtime){
      disco_somemulticolors_status = 0;
      disco_somemulticolors_timmer = millis();
    }
  }
}


unsigned long disco_colors3_timmer;   //a tester aussi en variables globales
int disco_colors3_timeset;
int disco_colors3_status;

void disco_colors3(int color1[], int color2[], int color3[], unsigned long time1, unsigned long time2, unsigned long time3) {
  if(disco_colors3_timeset == 0){
    disco_colors3_timmer = millis();
    disco_colors3_timeset = 1;
    }
  if(disco_colors3_status == 0){
    analogWrite(disco_led_pin_R, color1[0]);
    analogWrite(disco_led_pin_G, color1[1]);
    analogWrite(disco_led_pin_B, color1[2]);
    if(millis() > (disco_colors3_timmer + time1)){
      disco_colors3_status = 1;
      disco_colors3_timmer = millis();
    }
  }
  if(disco_colors3_status == 1){
    analogWrite(disco_led_pin_R, color2[0]);
    analogWrite(disco_led_pin_G, color2[1]);
    analogWrite(disco_led_pin_B, color2[2]);
    if(millis() > disco_colors3_timmer + time2){
      disco_colors3_status = 2;
      disco_colors3_timmer = millis();
    }
  }
  if(disco_colors3_status == 2){
    analogWrite(disco_led_pin_R, color3[0]);
    analogWrite(disco_led_pin_G, color3[1]);
    analogWrite(disco_led_pin_B, color3[2]);
    if(millis() > disco_colors3_timmer + time3){
      disco_colors3_status = 0;
      disco_colors3_timmer = millis();
    }
  }
}


unsigned long disco_colors8_timmer;   //a tester aussi en variables globales
int disco_colors8_timeset;
int disco_colors8_status;

void disco_colors8(int color1[], int color2[], int color3[], int color4[], int color5[], int color6[], int color7[], int color8[], unsigned long time1, unsigned long time2, unsigned long time3, unsigned long time4, unsigned long time5, unsigned long time6, unsigned long time7, unsigned long time8) {
  if(disco_colors8_timeset == 0){
    disco_colors8_timmer = millis();
    disco_colors8_timeset = 1;
    }
  if(disco_colors8_status == 0){
    analogWrite(disco_led_pin_R, color1[0]);
    analogWrite(disco_led_pin_G, color1[1]);
    analogWrite(disco_led_pin_B, color1[2]);
    if(millis() > (disco_colors8_timmer + time1)){
      disco_colors8_status = 1;
      disco_colors8_timmer = millis();
    }
  }
  if(disco_colors8_status == 1){
    analogWrite(disco_led_pin_R, color2[0]);
    analogWrite(disco_led_pin_G, color2[1]);
    analogWrite(disco_led_pin_B, color2[2]);
    if(millis() > disco_colors8_timmer + time2){
      disco_colors8_status = 2;
      disco_colors8_timmer = millis();
    }
  }
  if(disco_colors8_status == 2){
    analogWrite(disco_led_pin_R, color3[0]);
    analogWrite(disco_led_pin_G, color3[1]);
    analogWrite(disco_led_pin_B, color3[2]);
    if(millis() > disco_colors8_timmer + time3){
      disco_colors8_status = 3;
      disco_colors8_timmer = millis();
    }
  }
  if(disco_colors8_status == 3){
    analogWrite(disco_led_pin_R, color4[0]);
    analogWrite(disco_led_pin_G, color4[1]);
    analogWrite(disco_led_pin_B, color4[2]);
    if(millis() > disco_colors8_timmer + time4){
      disco_colors8_status = 4;
      disco_colors8_timmer = millis();
    }
  }
  if(disco_colors8_status == 4){
    analogWrite(disco_led_pin_R, color5[0]);
    analogWrite(disco_led_pin_G, color5[1]);
    analogWrite(disco_led_pin_B, color5[2]);
    if(millis() > disco_colors8_timmer + time5){
      disco_colors8_status = 5;
      disco_colors8_timmer = millis();
    }
  }
  if(disco_colors8_status == 5){
    analogWrite(disco_led_pin_R, color6[0]);
    analogWrite(disco_led_pin_G, color6[1]);
    analogWrite(disco_led_pin_B, color6[2]);
    if(millis() > disco_colors8_timmer + time6){
      disco_colors8_status = 6;
      disco_colors8_timmer = millis();
    }
  }
  if(disco_colors8_status == 6){
    analogWrite(disco_led_pin_R, color7[0]);
    analogWrite(disco_led_pin_G, color7[1]);
    analogWrite(disco_led_pin_B, color7[2]);
    if(millis() > disco_colors8_timmer + time7){
      disco_colors8_status = 7;
      disco_colors8_timmer = millis();
    }
  }
  if(disco_colors8_status == 7){
    analogWrite(disco_led_pin_R, color8[0]);
    analogWrite(disco_led_pin_G, color8[1]);
    analogWrite(disco_led_pin_B, color8[2]);
    if(millis() > disco_colors8_timmer + time8){
      disco_colors8_status = 0;
      disco_colors8_timmer = millis();
    }
  }
}

  
//fadesirenvariables
int disco_fadesiren_status;
int disco_fadesiren_timeset;
unsigned long disco_fadesiren_timmer1;
unsigned long disco_fadesiren_timmer2;

void disco_fadesiren(int startcolor[], int endcolor[], unsigned long finaltime) {
  int r;
  int g;
  int b;
  float advance;
  if(disco_fadesiren_timeset == 0){
    disco_fadesiren_timmer1 = millis();
    disco_fadesiren_timeset = 1;
  }
  disco_fadesiren_timmer2 = millis() - disco_fadesiren_timmer1;
  advance = 1000*disco_fadesiren_timmer2/finaltime;
  if(disco_fadesiren_status == 1){
    r = startcolor[0]*(1000 - (advance))/1000 + endcolor[0]*(advance)/1000;
    g = startcolor[1]*(1000 - (advance))/1000 + endcolor[1]*(advance)/1000;
    b = startcolor[2]*(1000 - (advance))/1000 + endcolor[2]*(advance)/1000;

    analogWrite(disco_led_pin_R, r);
    analogWrite(disco_led_pin_G, g);
    analogWrite(disco_led_pin_B, b);
    }
    
  if(disco_fadesiren_status == 0){
    r = endcolor[0]*(1000 - (advance))/1000 + startcolor[0]*(advance)/1000;
    g = endcolor[1]*(1000 - (advance))/1000 + startcolor[1]*(advance)/1000;
    b = endcolor[2]*(1000 - (advance))/1000 + startcolor[2]*(advance)/1000;

    analogWrite(disco_led_pin_R, r);
    analogWrite(disco_led_pin_G, g);
    analogWrite(disco_led_pin_B, b);
  }
  if(disco_fadesiren_timmer2 > (finaltime)){
     disco_fadesiren_timeset = 0;
     switch(disco_fadesiren_status){
      case 0:
      disco_fadesiren_status = 1;
      break;

      case 1:
      disco_fadesiren_status = 0;
      break;
      
     }     
  }  
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////                                                        fonctions sorties pwm ( program et efect ) name                                            /////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

unsigned long name_strombo_timmer;  
int name_strombo_timeset;
int name_strombo_status;

void name_strombo(int oncolor[], int offcolor[], unsigned long ontime, unsigned long offtime) {  
  if(name_strombo_timeset == 0){
    name_strombo_timmer = millis();
    name_strombo_timeset = 1;
    }
  if(name_strombo_status == 1){
    fill_solid(leds, NUM_LEDS, CRGB(oncolor[0], oncolor[1], oncolor[2]));
    
    if(millis() > (ontime + name_strombo_timmer)){
      name_strombo_status = 0;
      name_strombo_timmer = millis();
    }
  }
  if(name_strombo_status == 0){
    fill_solid(leds, NUM_LEDS, CRGB(offcolor[0], offcolor[1], offcolor[2]));
    
    if(millis() > offtime + name_strombo_timmer){
      name_strombo_status = 1;
      name_strombo_timmer = millis();
    }
  }
}

unsigned long name_backgroundstrombo_timmer;
int name_backgroundstrombo_timeset;
int name_backgroundstrombo_status;

void name_backgroundstrombo(int oncolor[], unsigned long ontime, unsigned long offtime) {
  if(name_backgroundstrombo_timeset == 0){
    name_backgroundstrombo_timmer = millis();
    name_backgroundstrombo_timeset = 1;
    }
  if(name_backgroundstrombo_status == 1){
    fill_solid(leds, NUM_LEDS, CRGB(oncolor[0], oncolor[1], oncolor[2]));
    
    if(millis() > (ontime + name_backgroundstrombo_timmer)){
      name_backgroundstrombo_status = 0;
      name_backgroundstrombo_timmer = millis();
    }
  }
  if(name_backgroundstrombo_status == 0){
    if(millis() > offtime + name_backgroundstrombo_timmer){
      name_backgroundstrombo_status = 1;
      name_backgroundstrombo_timmer = millis();
    }
  }
}

unsigned long name_someflasch_timmer;   //a tester aussi en variables globales
int name_someflasch_timeset;
int name_someflasch_status;
int name_someflasch_activate;
int name_someflasch_active;

void name_someflasch(int oncolor[], unsigned long ontime, unsigned long offtime) {
  if(name_someflasch_timeset == 0){
    name_someflasch_timmer = millis();
    name_someflasch_timeset = 1;
    }
  if(name_someflasch_status == 1){
    name_someflasch_active = 1;
    fill_solid(leds, NUM_LEDS, CRGB(oncolor[0], oncolor[1], oncolor[2]));
    
    if(millis() > (ontime + name_someflasch_timmer)){
      name_someflasch_status = 0;
      name_someflasch_timmer = millis();
    }
  }
  if(name_someflasch_status == 0){
    name_someflasch_active = 0;
    if(millis() > offtime + name_someflasch_timmer){
      name_someflasch_status = 1;
      name_someflasch_timmer = millis();
    }
  }
}

unsigned long name_someblack_timmer;  
int name_someblack_timeset;
int name_someblack_status;
int name_someblack_activate;

void name_someblack(int oncolor[], unsigned long ontime, unsigned long offtime) {
  if(name_someblack_timeset == 0){
    name_someblack_timmer = millis();
    name_someblack_timeset = 1;
    }
  if(name_someblack_status == 1){
    fill_solid(leds, NUM_LEDS, CRGB(oncolor[0], oncolor[1], oncolor[2]));
    
    if(millis() > (ontime + name_someblack_timmer)){
      name_someblack_status = 0;
      name_someblack_timmer = millis();
    }
  }
  if(name_someblack_status == 0){
    if(millis() > offtime + name_someblack_timmer){
      name_someblack_status = 1;
      name_someblack_timmer = millis();
    }
  }
}


unsigned long name_somemulticolors_timmer;  
int name_somemulticolors_timeset;
int name_somemulticolors_status;
int name_somemulticolors_activate;
int name_somemulticolors_active;

void name_somemulticolors(int color1[], int color2[], int color3[], int color4[], unsigned long time1, unsigned long time2, unsigned long time3, unsigned long time4, unsigned long donothingtime) {
  if(name_somemulticolors_timeset == 0){
    name_somemulticolors_timmer = millis();
    name_somemulticolors_timeset = 1;
    }
  if(name_somemulticolors_status == 0){
    name_somemulticolors_active = 1;
    fill_solid(leds, NUM_LEDS, CRGB(color1[0], color1[1], color1[2]));
    
    if(millis() > (name_somemulticolors_timmer + time1)){
      name_somemulticolors_status = 1;
      name_somemulticolors_timmer = millis();
    }
  }
  if(name_somemulticolors_status == 1){
    name_somemulticolors_active = 1;
    fill_solid(leds, NUM_LEDS, CRGB(color2[0], color2[1], color2[2]));
    
    if(millis() > name_somemulticolors_timmer + time2){
      name_somemulticolors_status = 2;
      name_somemulticolors_timmer = millis();
    }
  }
  if(name_somemulticolors_status == 2){
    name_somemulticolors_active = 1;
    fill_solid(leds, NUM_LEDS, CRGB(color3[0], color3[1], color3[2]));
    
    if(millis() > name_somemulticolors_timmer + time3){
      name_somemulticolors_status = 3;
      name_somemulticolors_timmer = millis();
    }
  }
  if(name_somemulticolors_status == 3){
    name_somemulticolors_active = 1;
    fill_solid(leds, NUM_LEDS, CRGB(color4[0], color4[1], color4[2]));
    
    if(millis() > name_somemulticolors_timmer + time4){
      name_somemulticolors_status = 4;
      name_somemulticolors_timmer = millis();
    }
  }
  if(name_somemulticolors_status == 4){
    name_somemulticolors_active = 0;
    if(millis() > name_somemulticolors_timmer + donothingtime){
      name_somemulticolors_status = 0;
      name_somemulticolors_timmer = millis();
    }
  }
}


unsigned long name_colors3_timmer;   
int name_colors3_timeset;
int name_colors3_status;

void name_colors3(int color1[], int color2[], int color3[], unsigned long time1, unsigned long time2, unsigned long time3) {
  if(name_colors3_timeset == 0){
    name_colors3_timmer = millis();
    name_colors3_timeset = 1;
    }
  if(name_colors3_status == 0){
   fill_solid(leds, NUM_LEDS, CRGB(color1[0], color1[1], color1[2]));
   
    if(millis() > (name_colors3_timmer + time1)){
      name_colors3_status = 1;
      name_colors3_timmer = millis();
    }
  }
  if(name_colors3_status == 1){
    fill_solid(leds, NUM_LEDS, CRGB(color2[0], color2[1], color2[2]));
    
    if(millis() > name_colors3_timmer + time2){
      name_colors3_status = 2;
      name_colors3_timmer = millis();
    }
  }
  if(name_colors3_status == 2){
    fill_solid(leds, NUM_LEDS, CRGB(color3[0], color3[1], color3[2]));
    
    if(millis() > name_colors3_timmer + time3){
      name_colors3_status = 0;
      name_colors3_timmer = millis();
    }
  }
}


unsigned long name_colors8_timmer;  
int name_colors8_timeset;
int name_colors8_status;

void name_colors8(int color1[], int color2[], int color3[], int color4[], int color5[], int color6[], int color7[], int color8[], unsigned long time1, unsigned long time2, unsigned long time3, unsigned long time4, unsigned long time5, unsigned long time6, unsigned long time7, unsigned long time8) {
  if(name_colors8_timeset == 0){
    name_colors8_timmer = millis();
    name_colors8_timeset = 1;
    }
  if(name_colors8_status == 0){
    fill_solid(leds, NUM_LEDS, CRGB(color1[0], color1[1], color1[2]));
    
    if(millis() > (name_colors8_timmer + time1)){
      name_colors8_status = 1;
      name_colors8_timmer = millis();
    }
  }
  if(name_colors8_status == 1){
    fill_solid(leds, NUM_LEDS, CRGB(color2[0], color2[1], color2[2]));
    
    if(millis() > name_colors8_timmer + time2){
      name_colors8_status = 2;
      name_colors8_timmer = millis();
    }
  }
  if(name_colors8_status == 2){
    fill_solid(leds, NUM_LEDS, CRGB(color3[0], color3[1], color3[2]));
    
    if(millis() > name_colors8_timmer + time3){
      name_colors8_status = 3;
      name_colors8_timmer = millis();
    }
  }
  if(name_colors8_status == 3){
    fill_solid(leds, NUM_LEDS, CRGB(color4[0], color4[1], color4[2]));
    
    if(millis() > name_colors8_timmer + time4){
      name_colors8_status = 4;
      name_colors8_timmer = millis();
    }
  }
  if(name_colors8_status == 4){
    fill_solid(leds, NUM_LEDS, CRGB(color5[0], color5[1], color5[2]));
    
    if(millis() > name_colors8_timmer + time5){
      name_colors8_status = 5;
      name_colors8_timmer = millis();
    }
  }
  if(name_colors8_status == 5){
    fill_solid(leds, NUM_LEDS, CRGB(color6[0], color6[1], color6[2]));
    
    if(millis() > name_colors8_timmer + time6){
      name_colors8_status = 6;
      name_colors8_timmer = millis();
    }
  }
  if(name_colors8_status == 6){
    fill_solid(leds, NUM_LEDS, CRGB(color7[0], color7[1], color7[2]));
    if(millis() > name_colors8_timmer + time7){
      name_colors8_status = 7;
      name_colors8_timmer = millis();
    }
  }
  if(name_colors8_status == 7){
    fill_solid(leds, NUM_LEDS, CRGB(color8[0],color8[1],color8[2]));
    
    if(millis() > name_colors8_timmer + time8){
      name_colors8_status = 0;
      name_colors8_timmer = millis();
    }
  }
}

unsigned long name_lightoffbycenter_timmer;   
int name_lightoffbycenter_timeset;
int name_lightoffbycenter_status;
int name_lightoffbycenter_loopcount;
void name_lightoffbycenter(int T1color[], int H1color[], int E1color[], int O2color[], int N2color[], int E2color[], int M3color[], int A3color[], int N3color[], int B4color[], int I4color[], int G4color[], int B5color[], int A5color[], int N5color[], int D5color[], int T6color[],int H6color[],int E6color[], int O6color[], int R6color[], int Y6color[], unsigned long steptime){

    if(name_lightoffbycenter_timeset == 0){
    name_lightoffbycenter_timmer = millis();
    name_lightoffbycenter_timeset = 1;
    }
  if(name_lightoffbycenter_status == 0){
    
      name_fixed1(T1color, H1color, E1color, O2color, N2color, E2color, M3color, A3color, N3color, B4color, I4color, G4color, B5color, A5color, N5color, D5color, T6color, H6color, E6color, O6color, R6color, Y6color);
      
    if(millis() > (name_lightoffbycenter_timmer + steptime)){
      name_lightoffbycenter_status = 1;
      name_lightoffbycenter_timmer = millis();
    }
  }
  if(name_lightoffbycenter_status == 1){
    
            name_fixed1(H1color, E1color, O2color, N2color, E2color, M3color, A3color, N3color, B4color, I4color, noir, noir, G4color, B5color, A5color, N5color, D5color, T6color, H6color, E6color, O6color, R6color);

    
    if(millis() > name_lightoffbycenter_timmer + steptime){
      name_lightoffbycenter_status = 2;
      name_lightoffbycenter_timmer = millis();
    }
  }

  if(name_lightoffbycenter_status == 2){
    
            name_fixed1(E1color, O2color, N2color, E2color, M3color, A3color, N3color, B4color, I4color, noir, noir, noir, noir, G4color, B5color, A5color, N5color, D5color, T6color, H6color, E6color, O6color);
    
    if(millis() > name_lightoffbycenter_timmer + steptime){
      name_lightoffbycenter_status = 3;
      name_lightoffbycenter_timmer = millis();
    }
  }
   if(name_lightoffbycenter_status == 3){
    
            name_fixed1(O2color, N2color, E2color, M3color, A3color, N3color, B4color, I4color, noir, noir, noir, noir, noir, noir, G4color, B5color, A5color, N5color, D5color, T6color, H6color, E6color);
    
    if(millis() > name_lightoffbycenter_timmer + steptime){
      name_lightoffbycenter_status = 4;
      name_lightoffbycenter_timmer = millis();
    }
  }
  if(name_lightoffbycenter_status == 4){
    
            name_fixed1( N2color, E2color, M3color, A3color, N3color, B4color, I4color, noir, noir, noir, noir, noir, noir, noir, noir, G4color, B5color, A5color, N5color, D5color, T6color, H6color);
    
    if(millis() > name_lightoffbycenter_timmer + steptime){
      name_lightoffbycenter_status = 5;
      name_lightoffbycenter_timmer = millis();
    }
  }
  if(name_lightoffbycenter_status == 5){
    
            name_fixed1(E2color, M3color, A3color, N3color, B4color, I4color, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, G4color, B5color, A5color, N5color, D5color, T6color);
    
    if(millis() > name_lightoffbycenter_timmer + steptime){
      name_lightoffbycenter_status = 6;
      name_lightoffbycenter_timmer = millis();
    }
  }
  if(name_lightoffbycenter_status == 6){
    
            name_fixed1(M3color, A3color, N3color, B4color, I4color, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, G4color, B5color, A5color, N5color, D5color);
    
    if(millis() > name_lightoffbycenter_timmer + steptime){
      name_lightoffbycenter_status = 7;
      name_lightoffbycenter_timmer = millis();
    }
  }
  if(name_lightoffbycenter_status == 7){
    
            name_fixed1(A3color, N3color, B4color, I4color, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, G4color, B5color, A5color, N5color);
    
    if(millis() > name_lightoffbycenter_timmer + steptime){
      name_lightoffbycenter_status = 8;
      name_lightoffbycenter_timmer = millis();
    }
  }
  if(name_lightoffbycenter_status == 8){
    
            name_fixed1(N3color, B4color, I4color, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, G4color, B5color, A5color);
    
    if(millis() > name_lightoffbycenter_timmer + steptime){
      name_lightoffbycenter_status = 9;
      name_lightoffbycenter_timmer = millis();
    }
  }
  if(name_lightoffbycenter_status == 9){
    
            name_fixed1(B4color, I4color, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, G4color, B5color);
    
    if(millis() > name_lightoffbycenter_timmer + steptime){
      name_lightoffbycenter_status = 10;
      name_lightoffbycenter_timmer = millis();
    }
  }
  if(name_lightoffbycenter_status == 10){
    
            name_fixed1(I4color, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, G4color);
    
    if(millis() > name_lightoffbycenter_timmer + steptime){
      name_lightoffbycenter_status = 11;
      name_lightoffbycenter_timmer = millis();
    }
  }
  if(name_lightoffbycenter_status == 11){
    
            name_fixed1(noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_lightoffbycenter_timmer + steptime){
      name_lightoffbycenter_status = 0;
      name_lightoffbycenter_timmer = millis();
      name_lightoffbycenter_loopcount++;
    }
  }
}
//fadesirenvariables
int name_fadesiren_status;
int name_fadesiren_timeset;
unsigned long name_fadesiren_timmer1;
unsigned long name_fadesiren_timmer2;

void name_fadesiren(int startcolor[], int endcolor[], unsigned long finaltime) {
  int r;
  int g;
  int b;
  float advance;
  if(name_fadesiren_timeset == 0){
    name_fadesiren_timmer1 = millis();
    name_fadesiren_timeset = 1;
  }
  name_fadesiren_timmer2 = millis() - name_fadesiren_timmer1;
  advance = 1000*name_fadesiren_timmer2/finaltime;
  if(name_fadesiren_status == 1){
    r = startcolor[0]*(1000 - (advance))/1000 + endcolor[0]*(advance)/1000;
    g = startcolor[1]*(1000 - (advance))/1000 + endcolor[1]*(advance)/1000;
    b = startcolor[2]*(1000 - (advance))/1000 + endcolor[2]*(advance)/1000;
    fill_solid(leds, NUM_LEDS, CRGB(r,g,b));
    
    }
    
  if(name_fadesiren_status == 0){
    r = endcolor[0]*(1000 - (advance))/1000 + startcolor[0]*(advance)/1000;
    g = endcolor[1]*(1000 - (advance))/1000 + startcolor[1]*(advance)/1000;
    b = endcolor[2]*(1000 - (advance))/1000 + startcolor[2]*(advance)/1000;

    fill_solid(leds, NUM_LEDS, CRGB(r,g,b));
    
  }
  if(name_fadesiren_timmer2 > (finaltime)){
     name_fadesiren_timeset = 0;
     switch(name_fadesiren_status){
      case 0:
      name_fadesiren_status = 1;
      break;

      case 1:
      name_fadesiren_status = 0;
      break;
      
     }     
  }  
}


void name_fixedcolors(int color1[]) {
  
    fill_solid(leds, NUM_LEDS, CRGB(color1[0], color1[1], color1[2]));
    
}


unsigned long T1_timmer;
unsigned long T1_timmer2;
int T1_timmer_set;
int T1_status;

unsigned long H1_timmer;
unsigned long H1_timmer2;
int H1_timmer_set;
int H1_status;

unsigned long E1_timmer;
unsigned long E1_timmer2;
int E1_timmer_set;
int E1_status;

unsigned long O2_timmer;
unsigned long O2_timmer2;
int O2_timmer_set;
int O2_status;

unsigned long N2_timmer;
unsigned long N2_timmer2;
int N2_timmer_set;
int N2_status;

unsigned long E2_timmer;
unsigned long E2_timmer2;
int E2_timmer_set;
int E2_status;

unsigned long M3_timmer;
unsigned long M3_timmer2;
int M3_timmer_set;
int M3_status;

unsigned long A3_timmer;
unsigned long A3_timmer2;
int A3_timmer_set;
int A3_status;

unsigned long N3_timmer;
unsigned long N3_timmer2;
int N3_timmer_set;
int N3_status;

unsigned long B4_timmer;
unsigned long B4_timmer2;
int B4_timmer_set;
int B4_status;

unsigned long I4_timmer;
unsigned long I4_timmer2;
int I4_timmer_set;
int I4_status;

unsigned long G4_timmer;
unsigned long G4_timmer2;
int G4_timmer_set;
int G4_status;

unsigned long B5_timmer;
unsigned long B5_timmer2;
int B5_timmer_set;
int B5_status;

unsigned long A5_timmer;
unsigned long A5_timmer2;
int A5_timmer_set;
int A5_status;

unsigned long N5_timmer;
unsigned long N5_timmer2;
int N5_timmer_set;
int N5_status;

unsigned long D5_timmer;
unsigned long D5_timmer2;
int D5_timmer_set;
int D5_status;

unsigned long T6_timmer;
unsigned long T6_timmer2;
int T6_timmer_set;
int T6_status;

unsigned long H6_timmer;
unsigned long H6_timmer2;
int H6_timmer_set;
int H6_status;

unsigned long E6_timmer;
unsigned long E6_timmer2;
int E6_timmer_set;
int E6_status;

unsigned long O6_timmer;
unsigned long O6_timmer2;
int O6_timmer_set;
int O6_status;

unsigned long R6_timmer;
unsigned long R6_timmer2;
int R6_timmer_set;
int R6_status;

unsigned long Y6_timmer;
unsigned long Y6_timmer2;
int Y6_timmer_set;
int Y6_status;

int name_fade1_timeset;
int name_fade1_offsetset;
int name_fade1_init;

void name_fade1(int startcolor[], int endcolor[], unsigned long totaltime, int offsetlist[]) {
  int T1r;
  int T1g;
  int T1b;
  float T1_advance;

  int H1r;
  int H1g;
  int H1b;
  float H1_advance;

  int E1r;
  int E1g;
  int E1b;
  float E1_advance;

  int O2r;
  int O2g;
  int O2b;
  float O2_advance;

  int N2r;
  int N2g;
  int N2b;
  float N2_advance;

  int E2r;
  int E2g;
  int E2b;
  float E2_advance;

  int M3r;
  int M3g;
  int M3b;
  float M3_advance;

  int A3r;
  int A3g;
  int A3b;
  float A3_advance;

  int N3r;
  int N3g;
  int N3b;
  float N3_advance;

  int B4r;
  int B4g;
  int B4b;
  float B4_advance;

  int I4r;
  int I4g;
  int I4b;
  float I4_advance;

  int G4r;
  int G4g;
  int G4b;
  float G4_advance;

  int B5r;
  int B5g;
  int B5b;
  float B5_advance;

  int A5r;
  int A5g;
  int A5b;
  float A5_advance;

  int N5r;
  int N5g;
  int N5b;
  float N5_advance;
  
  int D5r;
  int D5g;
  int D5b;
  float D5_advance;

  int T6r;
  int T6g;
  int T6b;
  float T6_advance;

  int H6r;
  int H6g;
  int H6b;
  float H6_advance;

  int E6r;
  int E6g;
  int E6b;
  float E6_advance;

  int O6r;
  int O6g;
  int O6b;
  float O6_advance;

  int R6r;
  int R6g;
  int R6b;
  float R6_advance;

  int Y6r;
  int Y6g;
  int Y6b;
  float Y6_advance;

  int finaltime;
  finaltime= totaltime - name_speedup;

  if(name_fade1_init == 0){
    T1_timmer = millis() - offsetlist[0];
    H1_timmer = millis() - offsetlist[1];
    E1_timmer = millis() - offsetlist[2];
    O2_timmer = millis() - offsetlist[3];
    N2_timmer = millis() - offsetlist[4];
    E2_timmer = millis() - offsetlist[5];
    M3_timmer = millis() - offsetlist[6];
    A3_timmer = millis() - offsetlist[7];
    N3_timmer = millis() - offsetlist[8];
    B4_timmer = millis() - offsetlist[9];
    I4_timmer = millis() - offsetlist[10];
    G4_timmer = millis() - offsetlist[11];
    B5_timmer = millis() - offsetlist[12];
    A5_timmer = millis() - offsetlist[13];
    N5_timmer = millis() - offsetlist[14];
    D5_timmer = millis() - offsetlist[15];
    T6_timmer = millis() - offsetlist[16];
    H6_timmer = millis() - offsetlist[17];
    E6_timmer = millis() - offsetlist[18];
    O6_timmer = millis() - offsetlist[19];
    R6_timmer = millis() - offsetlist[20];
    Y6_timmer = millis() - offsetlist[21];
    name_fade1_init = 1;
    name_fade1_timeset = 1;
  }
    T1_timmer2 = millis() - T1_timmer + name_speedup;
    H1_timmer2 = millis() - H1_timmer + name_speedup;
    E1_timmer2 = millis() - E1_timmer + name_speedup;
    O2_timmer2 = millis() - O2_timmer + name_speedup;
    N2_timmer2 = millis() - N2_timmer + name_speedup;
    E2_timmer2 = millis() - E2_timmer + name_speedup;
    M3_timmer2 = millis() - M3_timmer + name_speedup;
    A3_timmer2 = millis() - A3_timmer + name_speedup;
    N3_timmer2 = millis() - N3_timmer + name_speedup;
    B4_timmer2 = millis() - B4_timmer + name_speedup;
    I4_timmer2 = millis() - I4_timmer + name_speedup;
    G4_timmer2 = millis() - G4_timmer + name_speedup;
    B5_timmer2 = millis() - B5_timmer + name_speedup;
    A5_timmer2 = millis() - A5_timmer + name_speedup;
    N5_timmer2 = millis() - N5_timmer + name_speedup;
    D5_timmer2 = millis() - D5_timmer + name_speedup;
    T6_timmer2 = millis() - T6_timmer + name_speedup;
    H6_timmer2 = millis() - H6_timmer + name_speedup;
    E6_timmer2 = millis() - E6_timmer + name_speedup;
    O6_timmer2 = millis() - O6_timmer + name_speedup;
    R6_timmer2 = millis() - R6_timmer + name_speedup;
    Y6_timmer2 = millis() - Y6_timmer + name_speedup;
    
    
  T1_advance = 2000*T1_timmer2/finaltime;
  if(T1_timmer2 < finaltime/2){
    T1r = startcolor[0]*(1000 - (T1_advance))/1000 + endcolor[0]*(T1_advance)/1000;
    T1g = startcolor[1]*(1000 - (T1_advance))/1000 + endcolor[1]*(T1_advance)/1000;
    T1b = startcolor[2]*(1000 - (T1_advance))/1000 + endcolor[2]*(T1_advance)/1000;
    leds[0] = CRGB(T1r,T1g,T1b);
    leds[1] = CRGB(T1r,T1g,T1b);
    }
    
  if(finaltime/2 <= T1_timmer2 and T1_timmer2 < finaltime){
   
    T1r = endcolor[0]*(1000 - (T1_advance -1000))/1000 + startcolor[0]*(T1_advance -1000)/1000;
    T1g = endcolor[1]*(1000 - (T1_advance -1000))/1000 + startcolor[1]*(T1_advance -1000)/1000;
    T1b = endcolor[2]*(1000 - (T1_advance -1000))/1000 + startcolor[2]*(T1_advance -1000)/1000;

    leds[0] = CRGB(T1r,T1g,T1b);
    leds[1] = CRGB(T1r,T1g,T1b);
    //
  }
  
  if(T1_timmer2 > (finaltime)){
     T1_timmer = millis();
     T1_timmer2 = 0;
  }
  
  H1_advance = 2000*H1_timmer2/finaltime;
  if(H1_timmer2 < finaltime/2){
    H1r = startcolor[0]*(1000 - (H1_advance))/1000 + endcolor[0]*(H1_advance)/1000;
    H1g = startcolor[1]*(1000 - (H1_advance))/1000 + endcolor[1]*(H1_advance)/1000;
    H1b = startcolor[2]*(1000 - (H1_advance))/1000 + endcolor[2]*(H1_advance)/1000;
    leds[2] = CRGB(H1r,H1g,H1b);
    leds[3] = CRGB(H1r,H1g,H1b);
    }
    
  if(finaltime/2 <= H1_timmer2 and H1_timmer2 < finaltime){
   
    H1r = endcolor[0]*(1000 - (H1_advance -1000))/1000 + startcolor[0]*(H1_advance -1000)/1000;
    H1g = endcolor[1]*(1000 - (H1_advance -1000))/1000 + startcolor[1]*(H1_advance -1000)/1000;
    H1b = endcolor[2]*(1000 - (H1_advance -1000))/1000 + startcolor[2]*(H1_advance -1000)/1000;

    leds[2] = CRGB(H1r,H1g,H1b);
    leds[3] = CRGB(H1r,H1g,H1b);
  }
  
  if(H1_timmer2 > (finaltime)){
     H1_timmer = millis();
     H1_timmer2 = 0;
  }

  E1_advance = 2000*E1_timmer2/finaltime;
  if(E1_timmer2 < finaltime/2){
    E1r = startcolor[0]*(1000 - (E1_advance))/1000 + endcolor[0]*(E1_advance)/1000;
    E1g = startcolor[1]*(1000 - (E1_advance))/1000 + endcolor[1]*(E1_advance)/1000;
    E1b = startcolor[2]*(1000 - (E1_advance))/1000 + endcolor[2]*(E1_advance)/1000;
    leds[4] = CRGB(E1r,E1g,E1b);
    leds[5] = CRGB(E1r,E1g,E1b);
    }
    
  if(finaltime/2 <= E1_timmer2 and E1_timmer2 < finaltime){
   
    E1r = endcolor[0]*(1000 - (E1_advance -1000))/1000 + startcolor[0]*(E1_advance -1000)/1000;
    E1g = endcolor[1]*(1000 - (E1_advance -1000))/1000 + startcolor[1]*(E1_advance -1000)/1000;
    E1b = endcolor[2]*(1000 - (E1_advance -1000))/1000 + startcolor[2]*(E1_advance -1000)/1000;

    leds[4] = CRGB(E1r,E1g,E1b);
    leds[5] = CRGB(E1r,E1g,E1b);
  }
  
  if(E1_timmer2 > (finaltime)){
     E1_timmer = millis();
     E1_timmer2 = 0;
  }

   O2_advance = 2000*O2_timmer2/finaltime;
  if(O2_timmer2 < finaltime/2){
    O2r = startcolor[0]*(1000 - (O2_advance))/1000 + endcolor[0]*(O2_advance)/1000;
    O2g = startcolor[1]*(1000 - (O2_advance))/1000 + endcolor[1]*(O2_advance)/1000;
    O2b = startcolor[2]*(1000 - (O2_advance))/1000 + endcolor[2]*(O2_advance)/1000;
    leds[6] = CRGB(O2r,O2g,O2b);
    leds[7] = CRGB(O2r,O2g,O2b);
    }
    
  if(finaltime/2 <= O2_timmer2 and O2_timmer2 < finaltime){
   
    O2r = endcolor[0]*(1000 - (O2_advance -1000))/1000 + startcolor[0]*(O2_advance -1000)/1000;
    O2g = endcolor[1]*(1000 - (O2_advance -1000))/1000 + startcolor[1]*(O2_advance -1000)/1000;
    O2b = endcolor[2]*(1000 - (O2_advance -1000))/1000 + startcolor[2]*(O2_advance -1000)/1000;

    leds[6] = CRGB(O2r,O2g,O2b);
    leds[7] = CRGB(O2r,O2g,O2b);
    //
  }
  
  if(O2_timmer2 > (finaltime)){
     O2_timmer = millis();
     O2_timmer2 = 0;
  }

   N2_advance = 2000*N2_timmer2/finaltime;
  if(N2_timmer2 < finaltime/2){
    N2r = startcolor[0]*(1000 - (N2_advance))/1000 + endcolor[0]*(N2_advance)/1000;
    N2g = startcolor[1]*(1000 - (N2_advance))/1000 + endcolor[1]*(N2_advance)/1000;
    N2b = startcolor[2]*(1000 - (N2_advance))/1000 + endcolor[2]*(N2_advance)/1000;
    leds[8] = CRGB(N2r,N2g,N2b);
    leds[9] = CRGB(N2r,N2g,N2b);
    }
    
  if(finaltime/2 <= N2_timmer2 and N2_timmer2 < finaltime){
   
    N2r = endcolor[0]*(1000 - (N2_advance -1000))/1000 + startcolor[0]*(N2_advance -1000)/1000;
    N2g = endcolor[1]*(1000 - (N2_advance -1000))/1000 + startcolor[1]*(N2_advance -1000)/1000;
    N2b = endcolor[2]*(1000 - (N2_advance -1000))/1000 + startcolor[2]*(N2_advance -1000)/1000;

    leds[8] = CRGB(N2r,N2g,N2b);
    leds[9] = CRGB(N2r,N2g,N2b);
  }
  
  if(N2_timmer2 > (finaltime)){
     N2_timmer = millis();
     N2_timmer2 = 0;
  }

   E2_advance = 2000*E2_timmer2/finaltime;
  if(E2_timmer2 < finaltime/2){
    E2r = startcolor[0]*(1000 - (E2_advance))/1000 + endcolor[0]*(E2_advance)/1000;
    E2g = startcolor[1]*(1000 - (E2_advance))/1000 + endcolor[1]*(E2_advance)/1000;
    E2b = startcolor[2]*(1000 - (E2_advance))/1000 + endcolor[2]*(E2_advance)/1000;
    leds[10] = CRGB(E2r,E2g,E2b);
    leds[11] = CRGB(E2r,E2g,E2b);
    }
    
  if(finaltime/2 <= E2_timmer2 and E2_timmer2 < finaltime){
   
    E2r = endcolor[0]*(1000 - (E2_advance -1000))/1000 + startcolor[0]*(E2_advance -1000)/1000;
    E2g = endcolor[1]*(1000 - (E2_advance -1000))/1000 + startcolor[1]*(E2_advance -1000)/1000;
    E2b = endcolor[2]*(1000 - (E2_advance -1000))/1000 + startcolor[2]*(E2_advance -1000)/1000;

    leds[10] = CRGB(E2r,E2g,E2b);
    leds[11] = CRGB(E2r,E2g,E2b);
  }
  
  
  
  if(E2_timmer2 > (finaltime)){
     E2_timmer = millis();
     E2_timmer2 = 0;
  }

   M3_advance = 2000*M3_timmer2/finaltime;
  if(M3_timmer2 < finaltime/2){
    M3r = startcolor[0]*(1000 - (M3_advance))/1000 + endcolor[0]*(M3_advance)/1000;
    M3g = startcolor[1]*(1000 - (M3_advance))/1000 + endcolor[1]*(M3_advance)/1000;
    M3b = startcolor[2]*(1000 - (M3_advance))/1000 + endcolor[2]*(M3_advance)/1000;
    leds[12] = CRGB(M3r,M3g,M3b);
    leds[13] = CRGB(M3r,M3g,M3b);
    }
    
  if(finaltime/2 <= M3_timmer2 and M3_timmer2 < finaltime){
   
    M3r = endcolor[0]*(1000 - (M3_advance -1000))/1000 + startcolor[0]*(M3_advance -1000)/1000;
    M3g = endcolor[1]*(1000 - (M3_advance -1000))/1000 + startcolor[1]*(M3_advance -1000)/1000;
    M3b = endcolor[2]*(1000 - (M3_advance -1000))/1000 + startcolor[2]*(M3_advance -1000)/1000;

    leds[12] = CRGB(M3r,M3g,M3b);
    leds[13] = CRGB(M3r,M3g,M3b);
  }
 
  if(M3_timmer2 > (finaltime)){
     M3_timmer = millis();
     M3_timmer2 = 0;
  }
  
   A3_advance = 2000*A3_timmer2/finaltime;
  if(A3_timmer2 < finaltime/2){
    A3r = startcolor[0]*(1000 - (A3_advance))/1000 + endcolor[0]*(A3_advance)/1000;
    A3g = startcolor[1]*(1000 - (A3_advance))/1000 + endcolor[1]*(A3_advance)/1000;
    A3b = startcolor[2]*(1000 - (A3_advance))/1000 + endcolor[2]*(A3_advance)/1000;
    leds[14] = CRGB(A3r,A3g,A3b);
    leds[15] = CRGB(A3r,A3g,A3b);
    }
    
  if(finaltime/2 <= A3_timmer2 and A3_timmer2 < finaltime){
   
    A3r = endcolor[0]*(1000 - (A3_advance -1000))/1000 + startcolor[0]*(A3_advance -1000)/1000;
    A3g = endcolor[1]*(1000 - (A3_advance -1000))/1000 + startcolor[1]*(A3_advance -1000)/1000;
    A3b = endcolor[2]*(1000 - (A3_advance -1000))/1000 + startcolor[2]*(A3_advance -1000)/1000;

    leds[14] = CRGB(A3r,A3g,A3b);
    leds[15] = CRGB(A3r,A3g,A3b);
  }
  
  if(A3_timmer2 > (finaltime)){
     A3_timmer = millis();
     A3_timmer2 = 0;
  }

   N3_advance = 2000*N3_timmer2/finaltime;
  if(N3_timmer2 < finaltime/2){
    N3r = startcolor[0]*(1000 - (N3_advance))/1000 + endcolor[0]*(N3_advance)/1000;
    N3g = startcolor[1]*(1000 - (N3_advance))/1000 + endcolor[1]*(N3_advance)/1000;
    N3b = startcolor[2]*(1000 - (N3_advance))/1000 + endcolor[2]*(N3_advance)/1000;
    leds[16] = CRGB(N3r,N3g,N3b);
    leds[17] = CRGB(N3r,N3g,N3b);
    }
    
  if(finaltime/2 <= N3_timmer2 and N3_timmer2 < finaltime){
   
    N3r = endcolor[0]*(1000 - (N3_advance -1000))/1000 + startcolor[0]*(N3_advance -1000)/1000;
    N3g = endcolor[1]*(1000 - (N3_advance -1000))/1000 + startcolor[1]*(N3_advance -1000)/1000;
    N3b = endcolor[2]*(1000 - (N3_advance -1000))/1000 + startcolor[2]*(N3_advance -1000)/1000;

    leds[16] = CRGB(N3r,N3g,N3b);
    leds[17] = CRGB(N3r,N3g,N3b);
  }
  
  if(N3_timmer2 > (finaltime)){
     N3_timmer = millis();
     N3_timmer2 = 0;
  }

   B4_advance = 2000*B4_timmer2/finaltime;
  if(B4_timmer2 < finaltime/2){
    B4r = startcolor[0]*(1000 - (B4_advance))/1000 + endcolor[0]*(B4_advance)/1000;
    B4g = startcolor[1]*(1000 - (B4_advance))/1000 + endcolor[1]*(B4_advance)/1000;
    B4b = startcolor[2]*(1000 - (B4_advance))/1000 + endcolor[2]*(B4_advance)/1000;
    leds[18] = CRGB(B4r,B4g,B4b);
    leds[19] = CRGB(B4r,B4g,B4b);
    }
    
  if(finaltime/2 <= B4_timmer2 and B4_timmer2 < finaltime){
   
    B4r = endcolor[0]*(1000 - (B4_advance -1000))/1000 + startcolor[0]*(B4_advance -1000)/1000;
    B4g = endcolor[1]*(1000 - (B4_advance -1000))/1000 + startcolor[1]*(B4_advance -1000)/1000;
    B4b = endcolor[2]*(1000 - (B4_advance -1000))/1000 + startcolor[2]*(B4_advance -1000)/1000;

    leds[18] = CRGB(B4r,B4g,B4b);
    leds[19] = CRGB(B4r,B4g,B4b);
  }
  
  if(B4_timmer2 > (finaltime)){
     B4_timmer = millis();
     B4_timmer2 = 0;
  }

  I4_advance = 2000*I4_timmer2/finaltime;
  if(I4_timmer2 < finaltime/2){
    I4r = startcolor[0]*(1000 - (I4_advance))/1000 + endcolor[0]*(I4_advance)/1000;
    I4g = startcolor[1]*(1000 - (I4_advance))/1000 + endcolor[1]*(I4_advance)/1000;
    I4b = startcolor[2]*(1000 - (I4_advance))/1000 + endcolor[2]*(I4_advance)/1000;
    leds[20] = CRGB(I4r,I4g,I4b);
    leds[21] = CRGB(I4r,I4g,I4b);
    }
    
  if(finaltime/2 <= I4_timmer2 and I4_timmer2 < finaltime){
   
    I4r = endcolor[0]*(1000 - (I4_advance -1000))/1000 + startcolor[0]*(I4_advance -1000)/1000;
    I4g = endcolor[1]*(1000 - (I4_advance -1000))/1000 + startcolor[1]*(I4_advance -1000)/1000;
    I4b = endcolor[2]*(1000 - (I4_advance -1000))/1000 + startcolor[2]*(I4_advance -1000)/1000;

    leds[20] = CRGB(I4r,I4g,I4b);
    leds[21] = CRGB(I4r,I4g,I4b);
  }
  
  if(I4_timmer2 > (finaltime)){
     I4_timmer = millis();
     I4_timmer2 = 0;
  }

  G4_advance = 2000*G4_timmer2/finaltime;
  if(G4_timmer2 < finaltime/2){
    G4r = startcolor[0]*(1000 - (G4_advance))/1000 + endcolor[0]*(G4_advance)/1000;
    G4g = startcolor[1]*(1000 - (G4_advance))/1000 + endcolor[1]*(G4_advance)/1000;
    G4b = startcolor[2]*(1000 - (G4_advance))/1000 + endcolor[2]*(G4_advance)/1000;
    leds[22] = CRGB(G4r,G4g,G4b);
    leds[23] = CRGB(G4r,G4g,G4b);
    }
    
  if(finaltime/2 <= G4_timmer2 and G4_timmer2 < finaltime){
   
    G4r = endcolor[0]*(1000 - (G4_advance -1000))/1000 + startcolor[0]*(G4_advance -1000)/1000;
    G4g = endcolor[1]*(1000 - (G4_advance -1000))/1000 + startcolor[1]*(G4_advance -1000)/1000;
    G4b = endcolor[2]*(1000 - (G4_advance -1000))/1000 + startcolor[2]*(G4_advance -1000)/1000;

    leds[22] = CRGB(G4r,G4g,G4b);
    leds[23] = CRGB(G4r,G4g,G4b);
    
  }
  
  if(G4_timmer2 > (finaltime)){
     G4_timmer = millis();
     G4_timmer2 = 0;
  }

  B5_advance = 2000*B5_timmer2/finaltime;
  if(B5_timmer2 < finaltime/2){
    B5r = startcolor[0]*(1000 - (B5_advance))/1000 + endcolor[0]*(B5_advance)/1000;
    B5g = startcolor[1]*(1000 - (B5_advance))/1000 + endcolor[1]*(B5_advance)/1000;
    B5b = startcolor[2]*(1000 - (B5_advance))/1000 + endcolor[2]*(B5_advance)/1000;
    leds[24] = CRGB(B5r,B5g,B5b);
    leds[25] = CRGB(B5r,B5g,B5b);
    }
    
  if(finaltime/2 <= B5_timmer2 and B5_timmer2 < finaltime){
   
    B5r = endcolor[0]*(1000 - (B5_advance -1000))/1000 + startcolor[0]*(B5_advance -1000)/1000;
    B5g = endcolor[1]*(1000 - (B5_advance -1000))/1000 + startcolor[1]*(B5_advance -1000)/1000;
    B5b = endcolor[2]*(1000 - (B5_advance -1000))/1000 + startcolor[2]*(B5_advance -1000)/1000;

    leds[24] = CRGB(B5r,B5g,B5b);
    leds[25] = CRGB(B5r,B5g,B5b);
  }
  
  if(B5_timmer2 > (finaltime)){
     B5_timmer = millis();
     B5_timmer2 = 0;
  }

  A5_advance = 2000*A5_timmer2/finaltime;
  if(A5_timmer2 < finaltime/2){
    A5r = startcolor[0]*(1000 - (A5_advance))/1000 + endcolor[0]*(A5_advance)/1000;
    A5g = startcolor[1]*(1000 - (A5_advance))/1000 + endcolor[1]*(A5_advance)/1000;
    A5b = startcolor[2]*(1000 - (A5_advance))/1000 + endcolor[2]*(A5_advance)/1000;
    leds[26] = CRGB(A5r,A5g,A5b);
    leds[27] = CRGB(A5r,A5g,A5b);
    }
    
  if(finaltime/2 <= A5_timmer2 and A5_timmer2 < finaltime){
   
    A5r = endcolor[0]*(1000 - (A5_advance -1000))/1000 + startcolor[0]*(A5_advance -1000)/1000;
    A5g = endcolor[1]*(1000 - (A5_advance -1000))/1000 + startcolor[1]*(A5_advance -1000)/1000;
    A5b = endcolor[2]*(1000 - (A5_advance -1000))/1000 + startcolor[2]*(A5_advance -1000)/1000;

    leds[26] = CRGB(A5r,A5g,A5b);
    leds[27] = CRGB(A5r,A5g,A5b);
  }
  
  if(A5_timmer2 > (finaltime)){
     A5_timmer = millis();
     A5_timmer2 = 0;
  }
  N5_advance = 2000*N5_timmer2/finaltime;
  if(N5_timmer2 < finaltime/2){
    N5r = startcolor[0]*(1000 - (N5_advance))/1000 + endcolor[0]*(N5_advance)/1000;
    N5g = startcolor[1]*(1000 - (N5_advance))/1000 + endcolor[1]*(N5_advance)/1000;
    N5b = startcolor[2]*(1000 - (N5_advance))/1000 + endcolor[2]*(N5_advance)/1000;
    leds[28] = CRGB(N5r,N5g,N5b);
    leds[29] = CRGB(N5r,N5g,N5b);
    }
    
  if(finaltime/2 <= N5_timmer2 and N5_timmer2 < finaltime){
    N5r = endcolor[0]*(1000 - (N5_advance -1000))/1000 + startcolor[0]*(N5_advance -1000)/1000;
    N5g = endcolor[1]*(1000 - (N5_advance -1000))/1000 + startcolor[1]*(N5_advance -1000)/1000;
    N5b = endcolor[2]*(1000 - (N5_advance -1000))/1000 + startcolor[2]*(N5_advance -1000)/1000;

    leds[28] = CRGB(N5r,N5g,N5b);
    leds[29] = CRGB(N5r,N5g,N5b);
  }
  
  if(N5_timmer2 > (finaltime)){
     N5_timmer = millis();
     N5_timmer2 = 0;
  }
  D5_advance = 2000*D5_timmer2/finaltime;
  if(D5_timmer2 < finaltime/2){
    D5r = startcolor[0]*(1000 - (D5_advance))/1000 + endcolor[0]*(D5_advance)/1000;
    D5g = startcolor[1]*(1000 - (D5_advance))/1000 + endcolor[1]*(D5_advance)/1000;
    D5b = startcolor[2]*(1000 - (D5_advance))/1000 + endcolor[2]*(D5_advance)/1000;
    leds[30] = CRGB(D5r,D5g,D5b);
    leds[31] = CRGB(D5r,D5g,D5b);
    }
    
  if(finaltime/2 <= D5_timmer2 and D5_timmer2 < finaltime){
    D5r = endcolor[0]*(1000 - (D5_advance -1000))/1000 + startcolor[0]*(D5_advance -1000)/1000;
    D5g = endcolor[1]*(1000 - (D5_advance -1000))/1000 + startcolor[1]*(D5_advance -1000)/1000;
    D5b = endcolor[2]*(1000 - (D5_advance -1000))/1000 + startcolor[2]*(D5_advance -1000)/1000;

    leds[30] = CRGB(D5r,D5g,D5b);
    leds[31] = CRGB(D5r,D5g,D5b);
  }
  
  if(D5_timmer2 > (finaltime)){
     D5_timmer = millis();
     D5_timmer2 = 0;
  }
  T6_advance = 2000*T6_timmer2/finaltime;
  if(T6_timmer2 < finaltime/2){
    T6r = startcolor[0]*(1000 - (T6_advance))/1000 + endcolor[0]*(T6_advance)/1000;
    T6g = startcolor[1]*(1000 - (T6_advance))/1000 + endcolor[1]*(T6_advance)/1000;
    T6b = startcolor[2]*(1000 - (T6_advance))/1000 + endcolor[2]*(T6_advance)/1000;
    leds[32] = CRGB(T6r,T6g,T6b);
    leds[33] = CRGB(T6r,T6g,T6b);
    }
    
  if(finaltime/2 <= T6_timmer2 and T6_timmer2 < finaltime){
    T6r = endcolor[0]*(1000 - (T6_advance -1000))/1000 + startcolor[0]*(T6_advance -1000)/1000;
    T6g = endcolor[1]*(1000 - (T6_advance -1000))/1000 + startcolor[1]*(T6_advance -1000)/1000;
    T6b = endcolor[2]*(1000 - (T6_advance -1000))/1000 + startcolor[2]*(T6_advance -1000)/1000;

    leds[32] = CRGB(T6r,T6g,T6b);
    leds[33] = CRGB(T6r,T6g,T6b);
  }
  
  if(T6_timmer2 > (finaltime)){
     T6_timmer = millis();
     T6_timmer2 = 0;
  }

  H6_advance = 2000*H6_timmer2/finaltime;
  if(H6_timmer2 < finaltime/2){
    H6r = startcolor[0]*(1000 - (H6_advance))/1000 + endcolor[0]*(H6_advance)/1000;
    H6g = startcolor[1]*(1000 - (H6_advance))/1000 + endcolor[1]*(H6_advance)/1000;
    H6b = startcolor[2]*(1000 - (H6_advance))/1000 + endcolor[2]*(H6_advance)/1000;
    leds[34] = CRGB(H6r,H6g,H6b);
    leds[35] = CRGB(H6r,H6g,H6b);
    }
    
  if(finaltime/2 <= H6_timmer2 and H6_timmer2 < finaltime){
    H6r = endcolor[0]*(1000 - (H6_advance -1000))/1000 + startcolor[0]*(H6_advance -1000)/1000;
    H6g = endcolor[1]*(1000 - (H6_advance -1000))/1000 + startcolor[1]*(H6_advance -1000)/1000;
    H6b = endcolor[2]*(1000 - (H6_advance -1000))/1000 + startcolor[2]*(H6_advance -1000)/1000;

    leds[34] = CRGB(H6r,H6g,H6b);
    leds[35] = CRGB(H6r,H6g,H6b);
  }
  
  if(H6_timmer2 > (finaltime)){
     H6_timmer = millis();
     H6_timmer2 = 0;
  }
    E6_advance = 2000*E6_timmer2/finaltime;
  if(E6_timmer2 < finaltime/2){
    E6r = startcolor[0]*(1000 - (E6_advance))/1000 + endcolor[0]*(E6_advance)/1000;
    E6g = startcolor[1]*(1000 - (E6_advance))/1000 + endcolor[1]*(E6_advance)/1000;
    E6b = startcolor[2]*(1000 - (E6_advance))/1000 + endcolor[2]*(E6_advance)/1000;
    leds[36] = CRGB(E6r,E6g,E6b);
    leds[37] = CRGB(E6r,E6g,E6b);
    }
    
  if(finaltime/2 <= E6_timmer2 and E6_timmer2 < finaltime){
    E6r = endcolor[0]*(1000 - (E6_advance -1000))/1000 + startcolor[0]*(E6_advance -1000)/1000;
    E6g = endcolor[1]*(1000 - (E6_advance -1000))/1000 + startcolor[1]*(E6_advance -1000)/1000;
    E6b = endcolor[2]*(1000 - (E6_advance -1000))/1000 + startcolor[2]*(E6_advance -1000)/1000;

    leds[36] = CRGB(E6r,E6g,E6b);
    leds[37] = CRGB(E6r,E6g,E6b);
  }
  
  if(E6_timmer2 > (finaltime)){
     E6_timmer = millis();
     E6_timmer2 = 0;
  }

   O6_advance = 2000*O6_timmer2/finaltime;
  if(O6_timmer2 < finaltime/2){
    O6r = startcolor[0]*(1000 - (O6_advance))/1000 + endcolor[0]*(O6_advance)/1000;
    O6g = startcolor[1]*(1000 - (O6_advance))/1000 + endcolor[1]*(O6_advance)/1000;
    O6b = startcolor[2]*(1000 - (O6_advance))/1000 + endcolor[2]*(O6_advance)/1000;
    leds[38] = CRGB(O6r,O6g,O6b);
    leds[39] = CRGB(O6r,O6g,O6b);
    }
    
  if(finaltime/2 <= O6_timmer2 and O6_timmer2 < finaltime){
    O6r = endcolor[0]*(1000 - (O6_advance -1000))/1000 + startcolor[0]*(O6_advance -1000)/1000;
    O6g = endcolor[1]*(1000 - (O6_advance -1000))/1000 + startcolor[1]*(O6_advance -1000)/1000;
    O6b = endcolor[2]*(1000 - (O6_advance -1000))/1000 + startcolor[2]*(O6_advance -1000)/1000;

    leds[38] = CRGB(O6r,O6g,O6b);
    leds[39] = CRGB(O6r,O6g,O6b);
  }
  
  if(O6_timmer2 > (finaltime)){
     O6_timmer = millis();
     O6_timmer2 = 0;
  }

  R6_advance = 2000*R6_timmer2/finaltime;
  if(R6_timmer2 < finaltime/2){
    R6r = startcolor[0]*(1000 - (R6_advance))/1000 + endcolor[0]*(R6_advance)/1000;
    R6g = startcolor[1]*(1000 - (R6_advance))/1000 + endcolor[1]*(R6_advance)/1000;
    R6b = startcolor[2]*(1000 - (R6_advance))/1000 + endcolor[2]*(R6_advance)/1000;
    leds[40] = CRGB(R6r,R6g,R6b);
    leds[41] = CRGB(R6r,R6g,R6b);
    }
    
  if(finaltime/2 <= R6_timmer2 and R6_timmer2 < finaltime){
    R6r = endcolor[0]*(1000 - (R6_advance -1000))/1000 + startcolor[0]*(R6_advance -1000)/1000;
    R6g = endcolor[1]*(1000 - (R6_advance -1000))/1000 + startcolor[1]*(R6_advance -1000)/1000;
    R6b = endcolor[2]*(1000 - (R6_advance -1000))/1000 + startcolor[2]*(R6_advance -1000)/1000;

    leds[40] = CRGB(R6r,R6g,R6b);
    leds[41] = CRGB(R6r,R6g,R6b);
  }
  
  if(R6_timmer2 > (finaltime)){
     R6_timmer = millis();
     R6_timmer2 = 0;
  }

  Y6_advance = 2000*Y6_timmer2/finaltime;
  if(Y6_timmer2 < finaltime/2){
    Y6r = startcolor[0]*(1000 - (Y6_advance))/1000 + endcolor[0]*(Y6_advance)/1000;
    Y6g = startcolor[1]*(1000 - (Y6_advance))/1000 + endcolor[1]*(Y6_advance)/1000;
    Y6b = startcolor[2]*(1000 - (Y6_advance))/1000 + endcolor[2]*(Y6_advance)/1000;
    leds[42] = CRGB(Y6r,Y6g,Y6b);
    leds[43] = CRGB(Y6r,Y6g,Y6b);
    }
    
  if(finaltime/2 <= Y6_timmer2 and Y6_timmer2 < finaltime){
    Y6r = endcolor[0]*(1000 - (Y6_advance -1000))/1000 + startcolor[0]*(Y6_advance -1000)/1000;
    Y6g = endcolor[1]*(1000 - (Y6_advance -1000))/1000 + startcolor[1]*(Y6_advance -1000)/1000;
    Y6b = endcolor[2]*(1000 - (Y6_advance -1000))/1000 + startcolor[2]*(Y6_advance -1000)/1000;

    leds[42] = CRGB(Y6r,Y6g,Y6b);
    leds[43] = CRGB(Y6r,Y6g,Y6b);
  }
  
  if(Y6_timmer2 > (finaltime)){
     Y6_timmer = millis();
     Y6_timmer2 = 0;
  }
}

int name_fade3_timeset;
int name_fade3_offsetset;
int name_fade3_init;

void name_fade3(int color1[], int color2[], int color3[], unsigned long totaltime, int offsetlist[]) {
  int T1r;
  int T1g;
  int T1b;
  float T1_advance;

  int H1r;
  int H1g;
  int H1b;
  float H1_advance;

  int E1r;
  int E1g;
  int E1b;
  float E1_advance;

  int O2r;
  int O2g;
  int O2b;
  float O2_advance;

  int N2r;
  int N2g;
  int N2b;
  float N2_advance;

  int E2r;
  int E2g;
  int E2b;
  float E2_advance;

  int M3r;
  int M3g;
  int M3b;
  float M3_advance;

  int A3r;
  int A3g;
  int A3b;
  float A3_advance;

  int N3r;
  int N3g;
  int N3b;
  float N3_advance;

  int B4r;
  int B4g;
  int B4b;
  float B4_advance;

  int I4r;
  int I4g;
  int I4b;
  float I4_advance;

  int G4r;
  int G4g;
  int G4b;
  float G4_advance;

  int B5r;
  int B5g;
  int B5b;
  float B5_advance;

  int A5r;
  int A5g;
  int A5b;
  float A5_advance;

  int N5r;
  int N5g;
  int N5b;
  float N5_advance;
  
  int D5r;
  int D5g;
  int D5b;
  float D5_advance;

  int T6r;
  int T6g;
  int T6b;
  float T6_advance;

  int H6r;
  int H6g;
  int H6b;
  float H6_advance;

  int E6r;
  int E6g;
  int E6b;
  float E6_advance;

  int O6r;
  int O6g;
  int O6b;
  float O6_advance;

  int R6r;
  int R6g;
  int R6b;
  float R6_advance;

  int Y6r;
  int Y6g;
  int Y6b;
  float Y6_advance;

  int finaltime;
  finaltime= totaltime - name_speedup;

  if(name_fade3_init == 0){
    T1_timmer = millis() - offsetlist[0];
    H1_timmer = millis() - offsetlist[1];
    E1_timmer = millis() - offsetlist[2];
    O2_timmer = millis() - offsetlist[3];
    N2_timmer = millis() - offsetlist[4];
    E2_timmer = millis() - offsetlist[5];
    M3_timmer = millis() - offsetlist[6];
    A3_timmer = millis() - offsetlist[7];
    N3_timmer = millis() - offsetlist[8];
    B4_timmer = millis() - offsetlist[9];
    I4_timmer = millis() - offsetlist[10];
    G4_timmer = millis() - offsetlist[11];
    B5_timmer = millis() - offsetlist[12];
    A5_timmer = millis() - offsetlist[13];
    N5_timmer = millis() - offsetlist[14];
    D5_timmer = millis() - offsetlist[15];
    T6_timmer = millis() - offsetlist[16];
    H6_timmer = millis() - offsetlist[17];
    E6_timmer = millis() - offsetlist[18];
    O6_timmer = millis() - offsetlist[19];
    R6_timmer = millis() - offsetlist[20];
    Y6_timmer = millis() - offsetlist[21];
    name_fade3_init = 1;
    name_fade3_timeset = 1;
  }
    T1_timmer2 = millis() - T1_timmer + name_speedup;
    H1_timmer2 = millis() - H1_timmer + name_speedup;
    E1_timmer2 = millis() - E1_timmer + name_speedup;
    O2_timmer2 = millis() - O2_timmer + name_speedup;
    N2_timmer2 = millis() - N2_timmer + name_speedup;
    E2_timmer2 = millis() - E2_timmer + name_speedup;
    M3_timmer2 = millis() - M3_timmer + name_speedup;
    A3_timmer2 = millis() - A3_timmer + name_speedup;
    N3_timmer2 = millis() - N3_timmer + name_speedup;
    B4_timmer2 = millis() - B4_timmer + name_speedup;
    I4_timmer2 = millis() - I4_timmer + name_speedup;
    G4_timmer2 = millis() - G4_timmer + name_speedup;
    B5_timmer2 = millis() - B5_timmer + name_speedup;
    A5_timmer2 = millis() - A5_timmer + name_speedup;
    N5_timmer2 = millis() - N5_timmer + name_speedup;
    D5_timmer2 = millis() - D5_timmer + name_speedup;
    T6_timmer2 = millis() - T6_timmer + name_speedup;
    H6_timmer2 = millis() - H6_timmer + name_speedup;
    E6_timmer2 = millis() - E6_timmer + name_speedup;
    O6_timmer2 = millis() - O6_timmer + name_speedup;
    R6_timmer2 = millis() - R6_timmer + name_speedup;
    Y6_timmer2 = millis() - Y6_timmer + name_speedup;
    
  T1_advance = 3000*T1_timmer2/finaltime;
  
  if(T1_advance < 1000){
    T1r = color1[0]*(1000 - (T1_advance))/1000 + color2[0]*(T1_advance)/1000;
    T1g = color1[1]*(1000 - (T1_advance))/1000 + color2[1]*(T1_advance)/1000;
    T1b = color1[2]*(1000 - (T1_advance))/1000 + color2[2]*(T1_advance)/1000;
    leds[0] = CRGB(T1r,T1g,T1b);
    leds[1] = CRGB(T1r,T1g,T1b);
    }
    
  if( T1_advance >= 1000 and T1_advance < 2000){
   
    T1r = color2[0]*(1000 - (T1_advance -1000))/1000 + color3[0]*(T1_advance -1000)/1000;
    T1g = color2[1]*(1000 - (T1_advance -1000))/1000 + color3[1]*(T1_advance -1000)/1000;
    T1b = color2[2]*(1000 - (T1_advance -1000))/1000 + color3[2]*(T1_advance -1000)/1000;

    leds[0] = CRGB(T1r,T1g,T1b);
    leds[1] = CRGB(T1r,T1g,T1b);
  }
    if( T1_advance >= 2000 and T1_advance < 3000){
   
    T1r = color3[0]*(1000 - (T1_advance -2000))/1000 + color1[0]*(T1_advance -2000)/1000;
    T1g = color3[1]*(1000 - (T1_advance -2000))/1000 + color1[1]*(T1_advance -2000)/1000;
    T1b = color3[2]*(1000 - (T1_advance -2000))/1000 + color1[2]*(T1_advance -2000)/1000;

    leds[0] = CRGB(T1r,T1g,T1b);
    leds[1] = CRGB(T1r,T1g,T1b);
    //
  }
  
  if(T1_timmer2 > (finaltime)){
     T1_timmer = millis();
     T1_timmer2 = 0;
  }
H1_advance = 3000*H1_timmer2/finaltime;
  
  if(H1_advance < 1000){
    H1r = color1[0]*(1000 - (H1_advance))/1000 + color2[0]*(H1_advance)/1000;
    H1g = color1[1]*(1000 - (H1_advance))/1000 + color2[1]*(H1_advance)/1000;
    H1b = color1[2]*(1000 - (H1_advance))/1000 + color2[2]*(H1_advance)/1000;
    leds[2] = CRGB(H1r,H1g,H1b);
    leds[3] = CRGB(H1r,H1g,H1b);
    }
    
  if( H1_advance >= 1000 and H1_advance < 2000){
   
    H1r = color2[0]*(1000 - (H1_advance -1000))/1000 + color3[0]*(H1_advance -1000)/1000;
    H1g = color2[1]*(1000 - (H1_advance -1000))/1000 + color3[1]*(H1_advance -1000)/1000;
    H1b = color2[2]*(1000 - (H1_advance -1000))/1000 + color3[2]*(H1_advance -1000)/1000;

    leds[2] = CRGB(H1r,H1g,H1b);
    leds[3] = CRGB(H1r,H1g,H1b);
  }
    if( H1_advance >= 2000 and H1_advance < 3000){
   
    H1r = color3[0]*(1000 - (H1_advance -2000))/1000 + color1[0]*(H1_advance -2000)/1000;
    H1g = color3[1]*(1000 - (H1_advance -2000))/1000 + color1[1]*(H1_advance -2000)/1000;
    H1b = color3[2]*(1000 - (H1_advance -2000))/1000 + color1[2]*(H1_advance -2000)/1000;

    leds[2] = CRGB(H1r,H1g,H1b);
    leds[3] = CRGB(H1r,H1g,H1b);
    //
  }
  
  if(H1_timmer2 > (finaltime)){
     H1_timmer = millis();
     H1_timmer2 = 0;
  }
  
E1_advance = 3000*E1_timmer2/finaltime;
  
  if(E1_advance < 1000){
    E1r = color1[0]*(1000 - (E1_advance))/1000 + color2[0]*(E1_advance)/1000;
    E1g = color1[1]*(1000 - (E1_advance))/1000 + color2[1]*(E1_advance)/1000;
    E1b = color1[2]*(1000 - (E1_advance))/1000 + color2[2]*(E1_advance)/1000;
    leds[4] = CRGB(E1r,E1g,E1b);
    leds[5] = CRGB(E1r,E1g,E1b);
    }
    
  if( E1_advance >= 1000 and E1_advance < 2000){
   
    E1r = color2[0]*(1000 - (E1_advance -1000))/1000 + color3[0]*(E1_advance -1000)/1000;
    E1g = color2[1]*(1000 - (E1_advance -1000))/1000 + color3[1]*(E1_advance -1000)/1000;
    E1b = color2[2]*(1000 - (E1_advance -1000))/1000 + color3[2]*(E1_advance -1000)/1000;

    leds[4] = CRGB(E1r,E1g,E1b);
    leds[5] = CRGB(E1r,E1g,E1b);
  }
    if( E1_advance >= 2000 and E1_advance < 3000){
   
    E1r = color3[0]*(1000 - (E1_advance -2000))/1000 + color1[0]*(E1_advance -2000)/1000;
    E1g = color3[1]*(1000 - (E1_advance -2000))/1000 + color1[1]*(E1_advance -2000)/1000;
    E1b = color3[2]*(1000 - (E1_advance -2000))/1000 + color1[2]*(E1_advance -2000)/1000;

    leds[4] = CRGB(E1r,E1g,E1b);
    leds[5] = CRGB(E1r,E1g,E1b);
    //
  }
  
  if(E1_timmer2 > (finaltime)){
     E1_timmer = millis();
     E1_timmer2 = 0;
  }
  O2_advance = 3000*O2_timmer2/finaltime;
  
  if(O2_advance < 1000){
    O2r = color1[0]*(1000 - (O2_advance))/1000 + color2[0]*(O2_advance)/1000;
    O2g = color1[1]*(1000 - (O2_advance))/1000 + color2[1]*(O2_advance)/1000;
    O2b = color1[2]*(1000 - (O2_advance))/1000 + color2[2]*(O2_advance)/1000;
    leds[6] = CRGB(O2r,O2g,O2b);
    leds[7] = CRGB(O2r,O2g,O2b);
    }
    
  if( O2_advance >= 1000 and O2_advance < 2000){
   
    O2r = color2[0]*(1000 - (O2_advance -1000))/1000 + color3[0]*(O2_advance -1000)/1000;
    O2g = color2[1]*(1000 - (O2_advance -1000))/1000 + color3[1]*(O2_advance -1000)/1000;
    O2b = color2[2]*(1000 - (O2_advance -1000))/1000 + color3[2]*(O2_advance -1000)/1000;

    leds[6] = CRGB(O2r,O2g,O2b);
    leds[7] = CRGB(O2r,O2g,O2b);
  }
    if( O2_advance >= 2000 and O2_advance < 3000){
   
    O2r = color3[0]*(1000 - (O2_advance -2000))/1000 + color1[0]*(O2_advance -2000)/1000;
    O2g = color3[1]*(1000 - (O2_advance -2000))/1000 + color1[1]*(O2_advance -2000)/1000;
    O2b = color3[2]*(1000 - (O2_advance -2000))/1000 + color1[2]*(O2_advance -2000)/1000;

    leds[6] = CRGB(O2r,O2g,O2b);
    leds[7] = CRGB(O2r,O2g,O2b);
    //
  }
   
  if(O2_timmer2 > (finaltime)){
     O2_timmer = millis();
     O2_timmer2 = 0;
  }
  
N2_advance = 3000*N2_timmer2/finaltime;
  
  if(N2_advance < 1000){
    N2r = color1[0]*(1000 - (N2_advance))/1000 + color2[0]*(N2_advance)/1000;
    N2g = color1[1]*(1000 - (N2_advance))/1000 + color2[1]*(N2_advance)/1000;
    N2b = color1[2]*(1000 - (N2_advance))/1000 + color2[2]*(N2_advance)/1000;
    leds[8] = CRGB(N2r,N2g,N2b);
    leds[9] = CRGB(N2r,N2g,N2b);
    }
    
  if( N2_advance >= 1000 and N2_advance < 2000){
   
    N2r = color2[0]*(1000 - (N2_advance -1000))/1000 + color3[0]*(N2_advance -1000)/1000;
    N2g = color2[1]*(1000 - (N2_advance -1000))/1000 + color3[1]*(N2_advance -1000)/1000;
    N2b = color2[2]*(1000 - (N2_advance -1000))/1000 + color3[2]*(N2_advance -1000)/1000;

    leds[8] = CRGB(N2r,N2g,N2b);
    leds[9] = CRGB(N2r,N2g,N2b);
  }
    if( N2_advance >= 2000 and N2_advance < 3000){
   
    N2r = color3[0]*(1000 - (N2_advance -2000))/1000 + color1[0]*(N2_advance -2000)/1000;
    N2g = color3[1]*(1000 - (N2_advance -2000))/1000 + color1[1]*(N2_advance -2000)/1000;
    N2b = color3[2]*(1000 - (N2_advance -2000))/1000 + color1[2]*(N2_advance -2000)/1000;

    leds[8] = CRGB(N2r,N2g,N2b);
    leds[9] = CRGB(N2r,N2g,N2b);
    //
  }
  
  if(N2_timmer2 > (finaltime)){
     N2_timmer = millis();
     N2_timmer2 = 0;
  }
E2_advance = 3000*E2_timmer2/finaltime;
  
  if(E2_advance < 1000){
    E2r = color1[0]*(1000 - (E2_advance))/1000 + color2[0]*(E2_advance)/1000;
    E2g = color1[1]*(1000 - (E2_advance))/1000 + color2[1]*(E2_advance)/1000;
    E2b = color1[2]*(1000 - (E2_advance))/1000 + color2[2]*(E2_advance)/1000;
    leds[10] = CRGB(E2r,E2g,E2b);
    leds[11] = CRGB(E2r,E2g,E2b);
    }
    
  if( E2_advance >= 1000 and E2_advance < 2000){
   
    E2r = color2[0]*(1000 - (E2_advance -1000))/1000 + color3[0]*(E2_advance -1000)/1000;
    E2g = color2[1]*(1000 - (E2_advance -1000))/1000 + color3[1]*(E2_advance -1000)/1000;
    E2b = color2[2]*(1000 - (E2_advance -1000))/1000 + color3[2]*(E2_advance -1000)/1000;

    leds[10] = CRGB(E2r,E2g,E2b);
    leds[11] = CRGB(E2r,E2g,E2b);
  }
    if( E2_advance >= 2000 and E2_advance < 3000){
   
    E2r = color3[0]*(1000 - (E2_advance -2000))/1000 + color1[0]*(E2_advance -2000)/1000;
    E2g = color3[1]*(1000 - (E2_advance -2000))/1000 + color1[1]*(E2_advance -2000)/1000;
    E2b = color3[2]*(1000 - (E2_advance -2000))/1000 + color1[2]*(E2_advance -2000)/1000;

    leds[10] = CRGB(E2r,E2g,E2b);
    leds[11] = CRGB(E2r,E2g,E2b);
    //
  }
  
  if(E2_timmer2 > (finaltime)){
     E2_timmer = millis();
     E2_timmer2 = 0;
  }
  M3_advance = 3000*M3_timmer2/finaltime;
  
  if(M3_advance < 1000){
    M3r = color1[0]*(1000 - (M3_advance))/1000 + color2[0]*(M3_advance)/1000;
    M3g = color1[1]*(1000 - (M3_advance))/1000 + color2[1]*(M3_advance)/1000;
    M3b = color1[2]*(1000 - (M3_advance))/1000 + color2[2]*(M3_advance)/1000;
    leds[12] = CRGB(M3r,M3g,M3b);
    leds[13] = CRGB(M3r,M3g,M3b);
    }
    
  if( M3_advance >= 1000 and M3_advance < 2000){
   
    M3r = color2[0]*(1000 - (M3_advance -1000))/1000 + color3[0]*(M3_advance -1000)/1000;
    M3g = color2[1]*(1000 - (M3_advance -1000))/1000 + color3[1]*(M3_advance -1000)/1000;
    M3b = color2[2]*(1000 - (M3_advance -1000))/1000 + color3[2]*(M3_advance -1000)/1000;

    leds[12] = CRGB(M3r,M3g,M3b);
    leds[13] = CRGB(M3r,M3g,M3b);
  }
    if( M3_advance >= 2000 and M3_advance < 3000){
   
    M3r = color3[0]*(1000 - (M3_advance -2000))/1000 + color1[0]*(M3_advance -2000)/1000;
    M3g = color3[1]*(1000 - (M3_advance -2000))/1000 + color1[1]*(M3_advance -2000)/1000;
    M3b = color3[2]*(1000 - (M3_advance -2000))/1000 + color1[2]*(M3_advance -2000)/1000;

    leds[12] = CRGB(M3r,M3g,M3b);
    leds[13] = CRGB(M3r,M3g,M3b);
    //
  }
  
  if(M3_timmer2 > (finaltime)){
     M3_timmer = millis();
     M3_timmer2 = 0;
  }
  A3_advance = 3000*A3_timmer2/finaltime;
  
  if(A3_advance < 1000){
    A3r = color1[0]*(1000 - (A3_advance))/1000 + color2[0]*(A3_advance)/1000;
    A3g = color1[1]*(1000 - (A3_advance))/1000 + color2[1]*(A3_advance)/1000;
    A3b = color1[2]*(1000 - (A3_advance))/1000 + color2[2]*(A3_advance)/1000;
    leds[14] = CRGB(A3r,A3g,A3b);
    leds[15] = CRGB(A3r,A3g,A3b);
    }
    
  if( A3_advance >= 1000 and A3_advance < 2000){
   
    A3r = color2[0]*(1000 - (A3_advance -1000))/1000 + color3[0]*(A3_advance -1000)/1000;
    A3g = color2[1]*(1000 - (A3_advance -1000))/1000 + color3[1]*(A3_advance -1000)/1000;
    A3b = color2[2]*(1000 - (A3_advance -1000))/1000 + color3[2]*(A3_advance -1000)/1000;

    leds[14] = CRGB(A3r,A3g,A3b);
    leds[15] = CRGB(A3r,A3g,A3b);
  }
    if( A3_advance >= 2000 and A3_advance < 3000){
   
    A3r = color3[0]*(1000 - (A3_advance -2000))/1000 + color1[0]*(A3_advance -2000)/1000;
    A3g = color3[1]*(1000 - (A3_advance -2000))/1000 + color1[1]*(A3_advance -2000)/1000;
    A3b = color3[2]*(1000 - (A3_advance -2000))/1000 + color1[2]*(A3_advance -2000)/1000;

    leds[14] = CRGB(A3r,A3g,A3b);
    leds[15] = CRGB(A3r,A3g,A3b);
    //
  }
  
  if(A3_timmer2 > (finaltime)){
     A3_timmer = millis();
     A3_timmer2 = 0;
  }
  N3_advance = 3000*N3_timmer2/finaltime;
  
  if(N3_advance < 1000){
    N3r = color1[0]*(1000 - (N3_advance))/1000 + color2[0]*(N3_advance)/1000;
    N3g = color1[1]*(1000 - (N3_advance))/1000 + color2[1]*(N3_advance)/1000;
    N3b = color1[2]*(1000 - (N3_advance))/1000 + color2[2]*(N3_advance)/1000;
    leds[16] = CRGB(N3r,N3g,N3b);
    leds[17] = CRGB(N3r,N3g,N3b);
    }
    
  if( N3_advance >= 1000 and N3_advance < 2000){
   
    N3r = color2[0]*(1000 - (N3_advance -1000))/1000 + color3[0]*(N3_advance -1000)/1000;
    N3g = color2[1]*(1000 - (N3_advance -1000))/1000 + color3[1]*(N3_advance -1000)/1000;
    N3b = color2[2]*(1000 - (N3_advance -1000))/1000 + color3[2]*(N3_advance -1000)/1000;

    leds[16] = CRGB(N3r,N3g,N3b);
    leds[17] = CRGB(N3r,N3g,N3b);
  }
    if( N3_advance >= 2000 and N3_advance < 3000){
   
    N3r = color3[0]*(1000 - (N3_advance -2000))/1000 + color1[0]*(N3_advance -2000)/1000;
    N3g = color3[1]*(1000 - (N3_advance -2000))/1000 + color1[1]*(N3_advance -2000)/1000;
    N3b = color3[2]*(1000 - (N3_advance -2000))/1000 + color1[2]*(N3_advance -2000)/1000;

    leds[16] = CRGB(N3r,N3g,N3b);
    leds[17] = CRGB(N3r,N3g,N3b);
    //
  }
  
  if(N3_timmer2 > (finaltime)){
     N3_timmer = millis();
     N3_timmer2 = 0;
  }
B4_advance = 3000*B4_timmer2/finaltime;
  
  if(B4_advance < 1000){
    B4r = color1[0]*(1000 - (B4_advance))/1000 + color2[0]*(B4_advance)/1000;
    B4g = color1[1]*(1000 - (B4_advance))/1000 + color2[1]*(B4_advance)/1000;
    B4b = color1[2]*(1000 - (B4_advance))/1000 + color2[2]*(B4_advance)/1000;
    leds[18] = CRGB(B4r,B4g,B4b);
    leds[19] = CRGB(B4r,B4g,B4b);
    }
    
  if( B4_advance >= 1000 and B4_advance < 2000){
   
    B4r = color2[0]*(1000 - (B4_advance -1000))/1000 + color3[0]*(B4_advance -1000)/1000;
    B4g = color2[1]*(1000 - (B4_advance -1000))/1000 + color3[1]*(B4_advance -1000)/1000;
    B4b = color2[2]*(1000 - (B4_advance -1000))/1000 + color3[2]*(B4_advance -1000)/1000;

    leds[18] = CRGB(B4r,B4g,B4b);
    leds[19] = CRGB(B4r,B4g,B4b);
  }
    if( B4_advance >= 2000 and B4_advance < 3000){
   
    B4r = color3[0]*(1000 - (B4_advance -2000))/1000 + color1[0]*(B4_advance -2000)/1000;
    B4g = color3[1]*(1000 - (B4_advance -2000))/1000 + color1[1]*(B4_advance -2000)/1000;
    B4b = color3[2]*(1000 - (B4_advance -2000))/1000 + color1[2]*(B4_advance -2000)/1000;

    leds[18] = CRGB(B4r,B4g,B4b);
    leds[19] = CRGB(B4r,B4g,B4b);
    //
  }
  
  if(B4_timmer2 > (finaltime)){
     B4_timmer = millis();
     B4_timmer2 = 0;
  }
I4_advance = 3000*I4_timmer2/finaltime;
  
  if(I4_advance < 1000){
    I4r = color1[0]*(1000 - (I4_advance))/1000 + color2[0]*(I4_advance)/1000;
    I4g = color1[1]*(1000 - (I4_advance))/1000 + color2[1]*(I4_advance)/1000;
    I4b = color1[2]*(1000 - (I4_advance))/1000 + color2[2]*(I4_advance)/1000;
    leds[20] = CRGB(I4r,I4g,I4b);
    leds[21] = CRGB(I4r,I4g,I4b);
    }
    
  if( I4_advance >= 1000 and I4_advance < 2000){
   
    I4r = color2[0]*(1000 - (I4_advance -1000))/1000 + color3[0]*(I4_advance -1000)/1000;
    I4g = color2[1]*(1000 - (I4_advance -1000))/1000 + color3[1]*(I4_advance -1000)/1000;
    I4b = color2[2]*(1000 - (I4_advance -1000))/1000 + color3[2]*(I4_advance -1000)/1000;

    leds[20] = CRGB(I4r,I4g,I4b);
    leds[21] = CRGB(I4r,I4g,I4b);
  }
    if( I4_advance >= 2000 and I4_advance < 3000){
   
    I4r = color3[0]*(1000 - (I4_advance -2000))/1000 + color1[0]*(I4_advance -2000)/1000;
    I4g = color3[1]*(1000 - (I4_advance -2000))/1000 + color1[1]*(I4_advance -2000)/1000;
    I4b = color3[2]*(1000 - (I4_advance -2000))/1000 + color1[2]*(I4_advance -2000)/1000;

    leds[20] = CRGB(I4r,I4g,I4b);
    leds[21] = CRGB(I4r,I4g,I4b);
    //
  }
  
  if(I4_timmer2 > (finaltime)){
     I4_timmer = millis();
     I4_timmer2 = 0;
  }
  G4_advance = 3000*G4_timmer2/finaltime;
  
  if(G4_advance < 1000){
    G4r = color1[0]*(1000 - (G4_advance))/1000 + color2[0]*(G4_advance)/1000;
    G4g = color1[1]*(1000 - (G4_advance))/1000 + color2[1]*(G4_advance)/1000;
    G4b = color1[2]*(1000 - (G4_advance))/1000 + color2[2]*(G4_advance)/1000;
    leds[22] = CRGB(G4r,G4g,G4b);
    leds[23] = CRGB(G4r,G4g,G4b);
    }
    
  if( G4_advance >= 1000 and G4_advance < 2000){
   
    G4r = color2[0]*(1000 - (G4_advance -1000))/1000 + color3[0]*(G4_advance -1000)/1000;
    G4g = color2[1]*(1000 - (G4_advance -1000))/1000 + color3[1]*(G4_advance -1000)/1000;
    G4b = color2[2]*(1000 - (G4_advance -1000))/1000 + color3[2]*(G4_advance -1000)/1000;

    leds[22] = CRGB(G4r,G4g,G4b);
    leds[23] = CRGB(G4r,G4g,G4b);
  }
    if( G4_advance >= 2000 and G4_advance < 3000){
   
    G4r = color3[0]*(1000 - (G4_advance -2000))/1000 + color1[0]*(G4_advance -2000)/1000;
    G4g = color3[1]*(1000 - (G4_advance -2000))/1000 + color1[1]*(G4_advance -2000)/1000;
    G4b = color3[2]*(1000 - (G4_advance -2000))/1000 + color1[2]*(G4_advance -2000)/1000;

    leds[22] = CRGB(G4r,G4g,G4b);
    leds[23] = CRGB(G4r,G4g,G4b);
    //
    
  }
  
  if(G4_timmer2 > (finaltime)){
     G4_timmer = millis();
     G4_timmer2 = 0;
  }
B5_advance = 3000*B5_timmer2/finaltime;
  
  if(B5_advance < 1000){
    B5r = color1[0]*(1000 - (B5_advance))/1000 + color2[0]*(B5_advance)/1000;
    B5g = color1[1]*(1000 - (B5_advance))/1000 + color2[1]*(B5_advance)/1000;
    B5b = color1[2]*(1000 - (B5_advance))/1000 + color2[2]*(B5_advance)/1000;
    leds[24] = CRGB(B5r,B5g,B5b);
    leds[25] = CRGB(B5r,B5g,B5b);
    }
    
  if( B5_advance >= 1000 and B5_advance < 2000){
   
    B5r = color2[0]*(1000 - (B5_advance -1000))/1000 + color3[0]*(B5_advance -1000)/1000;
    B5g = color2[1]*(1000 - (B5_advance -1000))/1000 + color3[1]*(B5_advance -1000)/1000;
    B5b = color2[2]*(1000 - (B5_advance -1000))/1000 + color3[2]*(B5_advance -1000)/1000;

    leds[24] = CRGB(B5r,B5g,B5b);
    leds[25] = CRGB(B5r,B5g,B5b);
  }
    if( B5_advance >= 2000 and B5_advance < 3000){
   
    B5r = color3[0]*(1000 - (B5_advance -2000))/1000 + color1[0]*(B5_advance -2000)/1000;
    B5g = color3[1]*(1000 - (B5_advance -2000))/1000 + color1[1]*(B5_advance -2000)/1000;
    B5b = color3[2]*(1000 - (B5_advance -2000))/1000 + color1[2]*(B5_advance -2000)/1000;

    leds[24] = CRGB(B5r,B5g,B5b);
    leds[25] = CRGB(B5r,B5g,B5b);
    //
  }
  
  if(B5_timmer2 > (finaltime)){
     B5_timmer = millis();
     B5_timmer2 = 0;
  }  

A5_advance = 3000*A5_timmer2/finaltime;
  
  if(A5_advance < 1000){
    A5r = color1[0]*(1000 - (A5_advance))/1000 + color2[0]*(A5_advance)/1000;
    A5g = color1[1]*(1000 - (A5_advance))/1000 + color2[1]*(A5_advance)/1000;
    A5b = color1[2]*(1000 - (A5_advance))/1000 + color2[2]*(A5_advance)/1000;
    leds[26] = CRGB(A5r,A5g,A5b);
    leds[27] = CRGB(A5r,A5g,A5b);
    }
    
  if( A5_advance >= 1000 and A5_advance < 2000){
   
    A5r = color2[0]*(1000 - (A5_advance -1000))/1000 + color3[0]*(A5_advance -1000)/1000;
    A5g = color2[1]*(1000 - (A5_advance -1000))/1000 + color3[1]*(A5_advance -1000)/1000;
    A5b = color2[2]*(1000 - (A5_advance -1000))/1000 + color3[2]*(A5_advance -1000)/1000;

    leds[26] = CRGB(A5r,A5g,A5b);
    leds[27] = CRGB(A5r,A5g,A5b);
  }
    if( A5_advance >= 2000 and A5_advance < 3000){
   
    A5r = color3[0]*(1000 - (A5_advance -2000))/1000 + color1[0]*(A5_advance -2000)/1000;
    A5g = color3[1]*(1000 - (A5_advance -2000))/1000 + color1[1]*(A5_advance -2000)/1000;
    A5b = color3[2]*(1000 - (A5_advance -2000))/1000 + color1[2]*(A5_advance -2000)/1000;

    leds[26] = CRGB(A5r,A5g,A5b);
    leds[27] = CRGB(A5r,A5g,A5b);
    //
  }
  
  if(A5_timmer2 > (finaltime)){
     A5_timmer = millis();
     A5_timmer2 = 0;
  }
N5_advance = 3000*N5_timmer2/finaltime;
  
  if(N5_advance < 1000){
    N5r = color1[0]*(1000 - (N5_advance))/1000 + color2[0]*(N5_advance)/1000;
    N5g = color1[1]*(1000 - (N5_advance))/1000 + color2[1]*(N5_advance)/1000;
    N5b = color1[2]*(1000 - (N5_advance))/1000 + color2[2]*(N5_advance)/1000;
    leds[28] = CRGB(N5r,N5g,N5b);
    leds[29] = CRGB(N5r,N5g,N5b);
    }
    
  if( N5_advance >= 1000 and N5_advance < 2000){
   
    N5r = color2[0]*(1000 - (N5_advance -1000))/1000 + color3[0]*(N5_advance -1000)/1000;
    N5g = color2[1]*(1000 - (N5_advance -1000))/1000 + color3[1]*(N5_advance -1000)/1000;
    N5b = color2[2]*(1000 - (N5_advance -1000))/1000 + color3[2]*(N5_advance -1000)/1000;

    leds[28] = CRGB(N5r,N5g,N5b);
    leds[29] = CRGB(N5r,N5g,N5b);
  }
    if( N5_advance >= 2000 and N5_advance < 3000){
   
    N5r = color3[0]*(1000 - (N5_advance -2000))/1000 + color1[0]*(N5_advance -2000)/1000;
    N5g = color3[1]*(1000 - (N5_advance -2000))/1000 + color1[1]*(N5_advance -2000)/1000;
    N5b = color3[2]*(1000 - (N5_advance -2000))/1000 + color1[2]*(N5_advance -2000)/1000;

    leds[28] = CRGB(N5r,N5g,N5b);
    leds[29] = CRGB(N5r,N5g,N5b);
    //
  }
  
  if(N5_timmer2 > (finaltime)){
     N5_timmer = millis();
     N5_timmer2 = 0;
  }
D5_advance = 3000*D5_timmer2/finaltime;
  
  if(D5_advance < 1000){
    D5r = color1[0]*(1000 - (D5_advance))/1000 + color2[0]*(D5_advance)/1000;
    D5g = color1[1]*(1000 - (D5_advance))/1000 + color2[1]*(D5_advance)/1000;
    D5b = color1[2]*(1000 - (D5_advance))/1000 + color2[2]*(D5_advance)/1000;
    leds[30] = CRGB(D5r,D5g,D5b);
    leds[31] = CRGB(D5r,D5g,D5b);
    }
    
  if( D5_advance >= 1000 and D5_advance < 2000){
   
    D5r = color2[0]*(1000 - (D5_advance -1000))/1000 + color3[0]*(D5_advance -1000)/1000;
    D5g = color2[1]*(1000 - (D5_advance -1000))/1000 + color3[1]*(D5_advance -1000)/1000;
    D5b = color2[2]*(1000 - (D5_advance -1000))/1000 + color3[2]*(D5_advance -1000)/1000;

    leds[30] = CRGB(D5r,D5g,D5b);
    leds[31] = CRGB(D5r,D5g,D5b);
  }
    if( D5_advance >= 2000 and D5_advance < 3000){
   
    D5r = color3[0]*(1000 - (D5_advance -2000))/1000 + color1[0]*(D5_advance -2000)/1000;
    D5g = color3[1]*(1000 - (D5_advance -2000))/1000 + color1[1]*(D5_advance -2000)/1000;
    D5b = color3[2]*(1000 - (D5_advance -2000))/1000 + color1[2]*(D5_advance -2000)/1000;

    leds[30] = CRGB(D5r,D5g,D5b);
    leds[31] = CRGB(D5r,D5g,D5b);
    //
  }
  
  if(D5_timmer2 > (finaltime)){
     D5_timmer = millis();
     D5_timmer2 = 0;
  }
  T6_advance = 3000*T6_timmer2/finaltime;
  
  if(T6_advance < 1000){
    T6r = color1[0]*(1000 - (T6_advance))/1000 + color2[0]*(T6_advance)/1000;
    T6g = color1[1]*(1000 - (T6_advance))/1000 + color2[1]*(T6_advance)/1000;
    T6b = color1[2]*(1000 - (T6_advance))/1000 + color2[2]*(T6_advance)/1000;
    leds[32] = CRGB(T6r,T6g,T6b);
    leds[33] = CRGB(T6r,T6g,T6b);
    }
    
  if( T6_advance >= 1000 and T6_advance < 2000){
   
    T6r = color2[0]*(1000 - (T6_advance -1000))/1000 + color3[0]*(T6_advance -1000)/1000;
    T6g = color2[1]*(1000 - (T6_advance -1000))/1000 + color3[1]*(T6_advance -1000)/1000;
    T6b = color2[2]*(1000 - (T6_advance -1000))/1000 + color3[2]*(T6_advance -1000)/1000;

    leds[32] = CRGB(T6r,T6g,T6b);
    leds[33] = CRGB(T6r,T6g,T6b);
  }
    if( T6_advance >= 2000 and T6_advance < 3000){
   
    T6r = color3[0]*(1000 - (T6_advance -2000))/1000 + color1[0]*(T6_advance -2000)/1000;
    T6g = color3[1]*(1000 - (T6_advance -2000))/1000 + color1[1]*(T6_advance -2000)/1000;
    T6b = color3[2]*(1000 - (T6_advance -2000))/1000 + color1[2]*(T6_advance -2000)/1000;

    leds[32] = CRGB(T6r,T6g,T6b);
    leds[33] = CRGB(T6r,T6g,T6b);
    //
  }
  
  if(T6_timmer2 > (finaltime)){
     T6_timmer = millis();
     T6_timmer2 = 0;
  }
  H6_advance = 3000*H6_timmer2/finaltime;
  
  if(H6_advance < 1000){
    H6r = color1[0]*(1000 - (H6_advance))/1000 + color2[0]*(H6_advance)/1000;
    H6g = color1[1]*(1000 - (H6_advance))/1000 + color2[1]*(H6_advance)/1000;
    H6b = color1[2]*(1000 - (H6_advance))/1000 + color2[2]*(H6_advance)/1000;
    leds[34] = CRGB(H6r,H6g,H6b);
    leds[35] = CRGB(H6r,H6g,H6b);
    }
    
  if( H6_advance >= 1000 and H6_advance < 2000){
   
    H6r = color2[0]*(1000 - (H6_advance -1000))/1000 + color3[0]*(H6_advance -1000)/1000;
    H6g = color2[1]*(1000 - (H6_advance -1000))/1000 + color3[1]*(H6_advance -1000)/1000;
    H6b = color2[2]*(1000 - (H6_advance -1000))/1000 + color3[2]*(H6_advance -1000)/1000;

    leds[34] = CRGB(H6r,H6g,H6b);
    leds[35] = CRGB(H6r,H6g,H6b);
  }
    if( H6_advance >= 2000 and H6_advance < 3000){
   
    H6r = color3[0]*(1000 - (H6_advance -2000))/1000 + color1[0]*(H6_advance -2000)/1000;
    H6g = color3[1]*(1000 - (H6_advance -2000))/1000 + color1[1]*(H6_advance -2000)/1000;
    H6b = color3[2]*(1000 - (H6_advance -2000))/1000 + color1[2]*(H6_advance -2000)/1000;

    leds[34] = CRGB(H6r,H6g,H6b);
    leds[35] = CRGB(H6r,H6g,H6b);
    //
  }
  
  if(H6_timmer2 > (finaltime)){
     H6_timmer = millis();
     H6_timmer2 = 0;
  }
E6_advance = 3000*E6_timmer2/finaltime;
  
  if(E6_advance < 1000){
    E6r = color1[0]*(1000 - (E6_advance))/1000 + color2[0]*(E6_advance)/1000;
    E6g = color1[1]*(1000 - (E6_advance))/1000 + color2[1]*(E6_advance)/1000;
    E6b = color1[2]*(1000 - (E6_advance))/1000 + color2[2]*(E6_advance)/1000;
    leds[36] = CRGB(E6r,E6g,E6b);
    leds[37] = CRGB(E6r,E6g,E6b);
    }
    
  if( E6_advance >= 1000 and E6_advance < 2000){
   
    E6r = color2[0]*(1000 - (E6_advance -1000))/1000 + color3[0]*(E6_advance -1000)/1000;
    E6g = color2[1]*(1000 - (E6_advance -1000))/1000 + color3[1]*(E6_advance -1000)/1000;
    E6b = color2[2]*(1000 - (E6_advance -1000))/1000 + color3[2]*(E6_advance -1000)/1000;

    leds[36] = CRGB(E6r,E6g,E6b);
    leds[37] = CRGB(E6r,E6g,E6b);
  }
    if( E6_advance >= 2000 and E6_advance < 3000){
   
    E6r = color3[0]*(1000 - (E6_advance -2000))/1000 + color1[0]*(E6_advance -2000)/1000;
    E6g = color3[1]*(1000 - (E6_advance -2000))/1000 + color1[1]*(E6_advance -2000)/1000;
    E6b = color3[2]*(1000 - (E6_advance -2000))/1000 + color1[2]*(E6_advance -2000)/1000;

    leds[36] = CRGB(E6r,E6g,E6b);
    leds[37] = CRGB(E6r,E6g,E6b);
    //
  }
  
  if(E6_timmer2 > (finaltime)){
     E6_timmer = millis();
     E6_timmer2 = 0;
  }
O6_advance = 3000*O6_timmer2/finaltime;
  
  if(O6_advance < 1000){
    O6r = color1[0]*(1000 - (O6_advance))/1000 + color2[0]*(O6_advance)/1000;
    O6g = color1[1]*(1000 - (O6_advance))/1000 + color2[1]*(O6_advance)/1000;
    O6b = color1[2]*(1000 - (O6_advance))/1000 + color2[2]*(O6_advance)/1000;
    leds[38] = CRGB(O6r,O6g,O6b);
    leds[39] = CRGB(O6r,O6g,O6b);
    }
    
  if( O6_advance >= 1000 and O6_advance < 2000){
   
    O6r = color2[0]*(1000 - (O6_advance -1000))/1000 + color3[0]*(O6_advance -1000)/1000;
    O6g = color2[1]*(1000 - (O6_advance -1000))/1000 + color3[1]*(O6_advance -1000)/1000;
    O6b = color2[2]*(1000 - (O6_advance -1000))/1000 + color3[2]*(O6_advance -1000)/1000;

    leds[38] = CRGB(O6r,O6g,O6b);
    leds[39] = CRGB(O6r,O6g,O6b);
  }
    if( O6_advance >= 2000 and O6_advance < 3000){
   
    O6r = color3[0]*(1000 - (O6_advance -2000))/1000 + color1[0]*(O6_advance -2000)/1000;
    O6g = color3[1]*(1000 - (O6_advance -2000))/1000 + color1[1]*(O6_advance -2000)/1000;
    O6b = color3[2]*(1000 - (O6_advance -2000))/1000 + color1[2]*(O6_advance -2000)/1000;

    leds[38] = CRGB(O6r,O6g,O6b);
    leds[39] = CRGB(O6r,O6g,O6b);
    //
  }
  
  if(O6_timmer2 > (finaltime)){
     O6_timmer = millis();
     O6_timmer2 = 0;
  }
  R6_advance = 3000*R6_timmer2/finaltime;
  
  if(R6_advance < 1000){
    R6r = color1[0]*(1000 - (R6_advance))/1000 + color2[0]*(R6_advance)/1000;
    R6g = color1[1]*(1000 - (R6_advance))/1000 + color2[1]*(R6_advance)/1000;
    R6b = color1[2]*(1000 - (R6_advance))/1000 + color2[2]*(R6_advance)/1000;
    leds[40] = CRGB(R6r,R6g,R6b);
    leds[41] = CRGB(R6r,R6g,R6b);
    }
    
  if( R6_advance >= 1000 and R6_advance < 2000){
   
    R6r = color2[0]*(1000 - (R6_advance -1000))/1000 + color3[0]*(R6_advance -1000)/1000;
    R6g = color2[1]*(1000 - (R6_advance -1000))/1000 + color3[1]*(R6_advance -1000)/1000;
    R6b = color2[2]*(1000 - (R6_advance -1000))/1000 + color3[2]*(R6_advance -1000)/1000;

    leds[40] = CRGB(R6r,R6g,R6b);
    leds[41] = CRGB(R6r,R6g,R6b);
  }
    if( R6_advance >= 2000 and R6_advance < 3000){
   
    R6r = color3[0]*(1000 - (R6_advance -2000))/1000 + color1[0]*(R6_advance -2000)/1000;
    R6g = color3[1]*(1000 - (R6_advance -2000))/1000 + color1[1]*(R6_advance -2000)/1000;
    R6b = color3[2]*(1000 - (R6_advance -2000))/1000 + color1[2]*(R6_advance -2000)/1000;

    leds[40] = CRGB(R6r,R6g,R6b);
    leds[41] = CRGB(R6r,R6g,R6b);
    //
  }
  
  if(R6_timmer2 > (finaltime)){
     R6_timmer = millis();
     R6_timmer2 = 0;
  }
  Y6_advance = 3000*Y6_timmer2/finaltime;
  
  if(Y6_advance < 1000){
    Y6r = color1[0]*(1000 - (Y6_advance))/1000 + color2[0]*(Y6_advance)/1000;
    Y6g = color1[1]*(1000 - (Y6_advance))/1000 + color2[1]*(Y6_advance)/1000;
    Y6b = color1[2]*(1000 - (Y6_advance))/1000 + color2[2]*(Y6_advance)/1000;
    leds[42] = CRGB(Y6r,Y6g,Y6b);
    leds[43] = CRGB(Y6r,Y6g,Y6b);
    }
    
  if( Y6_advance >= 1000 and Y6_advance < 2000){
   
    Y6r = color2[0]*(1000 - (Y6_advance -1000))/1000 + color3[0]*(Y6_advance -1000)/1000;
    Y6g = color2[1]*(1000 - (Y6_advance -1000))/1000 + color3[1]*(Y6_advance -1000)/1000;
    Y6b = color2[2]*(1000 - (Y6_advance -1000))/1000 + color3[2]*(Y6_advance -1000)/1000;

    leds[42] = CRGB(Y6r,Y6g,Y6b);
    leds[43] = CRGB(Y6r,Y6g,Y6b);
  }
    if( Y6_advance >= 2000 and Y6_advance < 3000){
   
    Y6r = color3[0]*(1000 - (Y6_advance -2000))/1000 + color1[0]*(Y6_advance -2000)/1000;
    Y6g = color3[1]*(1000 - (Y6_advance -2000))/1000 + color1[1]*(Y6_advance -2000)/1000;
    Y6b = color3[2]*(1000 - (Y6_advance -2000))/1000 + color1[2]*(Y6_advance -2000)/1000;

    leds[42] = CRGB(Y6r,Y6g,Y6b);
    leds[43] = CRGB(Y6r,Y6g,Y6b);
    //
  }
  
  if(Y6_timmer2 > (finaltime)){
     Y6_timmer = millis();
     Y6_timmer2 = 0;
  }


  
}


  
void name_fixed1(int color1[], int color2[], int color3[], int color4[], int color5[], int color6[], int color7[], int color8[], int color9[], int color10[], int color11[], int color12[], int color13[], int color14[], int color15[], int color16[], int color17[],int color18[],int color19[], int color20[], int color21[], int color22[]){
leds[0] = CRGB(color1[0],color1[1],color1[2]);
leds[1] = CRGB(color1[0],color1[1],color1[2]);

leds[2] = CRGB(color2[0],color2[1],color2[2]);
leds[3] = CRGB(color2[0],color2[1],color2[2]);

leds[4] = CRGB(color3[0],color3[1],color3[2]);
leds[5] = CRGB(color3[0],color3[1],color3[2]);

leds[6] = CRGB(color4[0],color4[1],color4[2]);
leds[7] = CRGB(color4[0],color4[1],color4[2]);

leds[8] = CRGB(color5[0],color5[1],color5[2]);
leds[9] = CRGB(color5[0],color5[1],color5[2]);

leds[10] = CRGB(color6[0],color6[1],color6[2]);
leds[11] = CRGB(color6[0],color6[1],color6[2]);

leds[12] = CRGB(color7[0],color7[1],color7[2]);
leds[13] = CRGB(color7[0],color7[1],color7[2]);

leds[14] = CRGB(color8[0],color8[1],color8[2]);
leds[15] = CRGB(color8[0],color8[1],color8[2]);

leds[16] = CRGB(color9[0],color9[1],color9[2]);
leds[17] = CRGB(color9[0],color9[1],color9[2]);

leds[18] = CRGB(color10[0],color10[1],color10[2]);
leds[19] = CRGB(color10[0],color10[1],color10[2]);

leds[20] = CRGB(color11[0],color11[1],color11[2]);
leds[21] = CRGB(color11[0],color11[1],color11[2]);

leds[22] = CRGB(color12[0],color12[1],color12[2]);
leds[23] = CRGB(color12[0],color12[1],color12[2]);

leds[24] = CRGB(color13[0],color13[1],color13[2]);
leds[25] = CRGB(color13[0],color13[1],color13[2]);

leds[26] = CRGB(color14[0],color14[1],color14[2]);
leds[27] = CRGB(color14[0],color14[1],color14[2]);

leds[28] = CRGB(color15[0],color15[1],color15[2]);
leds[29] = CRGB(color15[0],color15[1],color15[2]);

leds[30] = CRGB(color16[0],color16[1],color16[2]);
leds[31] = CRGB(color16[0],color16[1],color16[2]);

leds[32] = CRGB(color17[0],color17[1],color17[2]);
leds[33] = CRGB(color17[0],color17[1],color17[2]);

leds[34] = CRGB(color18[0],color18[1],color18[2]);
leds[35] = CRGB(color18[0],color18[1],color18[2]);

leds[36] = CRGB(color19[0],color19[1],color19[2]);
leds[37] = CRGB(color19[0],color19[1],color19[2]);

leds[38] = CRGB(color20[0],color20[1],color20[2]);
leds[39] = CRGB(color20[0],color20[1],color20[2]);

leds[40] = CRGB(color21[0],color21[1],color21[2]);
leds[41] = CRGB(color21[0],color21[1],color21[2]);

leds[42] = CRGB(color22[0],color22[1],color22[2]);
leds[43] = CRGB(color22[0],color22[1],color22[2]);
}

void name_THEfixedcolors(int Tcolor[], int Hcolor[], int Ecolor[]) {
  
    leds[0] = CRGB(Tcolor[0],Tcolor[1],Tcolor[2]);
    leds[1] = CRGB(Tcolor[0],Tcolor[1],Tcolor[2]);

    leds[2] = CRGB(Hcolor[0],Hcolor[1],Hcolor[2]);
    leds[3] = CRGB(Hcolor[0],Hcolor[1],Hcolor[2]);

    leds[4] = CRGB(Ecolor[0],Ecolor[1],Ecolor[2]);
    leds[5] = CRGB(Ecolor[0],Ecolor[1],Ecolor[2]);
    
}

void name_ONEfixedcolors(int Ocolor[], int Ncolor[], int Ecolor[]) {
  
    leds[6] = CRGB(Ocolor[0],Ocolor[1],Ocolor[2]);
    leds[7] = CRGB(Ocolor[0],Ocolor[1],Ocolor[2]);

    leds[8] = CRGB(Ncolor[0],Ncolor[1],Ncolor[2]);
    leds[9] = CRGB(Ncolor[0],Ncolor[1],Ncolor[2]);

    leds[10] = CRGB(Ecolor[0],Ecolor[1],Ecolor[2]);
    leds[11] = CRGB(Ecolor[0],Ecolor[1],Ecolor[2]);
    
}

void name_MANfixedcolors(int Mcolor[], int Acolor[], int Ncolor[]) {
  
    leds[12] = CRGB(Mcolor[0],Mcolor[1],Mcolor[2]);
    leds[13] = CRGB(Mcolor[0],Mcolor[1],Mcolor[2]);
    
    leds[14] = CRGB(Acolor[0],Acolor[1],Acolor[2]);
    leds[15] = CRGB(Acolor[0],Acolor[1],Acolor[2]);
    
    leds[16] = CRGB(Ncolor[0],Ncolor[1],Ncolor[2]);
    leds[17] = CRGB(Ncolor[0],Ncolor[1],Ncolor[2]);
    
}

void name_BIGfixedcolors(int Bcolor[], int Icolor[], int Gcolor[]) {
  
    leds[18] = CRGB(Bcolor[0],Bcolor[1],Bcolor[2]);
    leds[19] = CRGB(Bcolor[0],Bcolor[1],Bcolor[2]);
    
    leds[20] = CRGB(Icolor[0],Icolor[1],Icolor[2]);
    leds[21] = CRGB(Icolor[0],Icolor[1],Icolor[2]);
    
    leds[22] = CRGB(Gcolor[0],Gcolor[1],Gcolor[2]);
    leds[23] = CRGB(Gcolor[0],Gcolor[1],Gcolor[2]);
}

void name_BANDfixedcolors(int Bcolor[], int Acolor[], int Ncolor[], int Dcolor[]) {
  
    leds[24] = CRGB(Bcolor[0],Bcolor[1],Bcolor[2]);
    leds[25] = CRGB(Bcolor[0],Bcolor[1],Bcolor[2]);
    
    leds[26] = CRGB(Acolor[0],Acolor[1],Acolor[2]);
    leds[27] = CRGB(Acolor[0],Acolor[1],Acolor[2]);
    
    leds[28] = CRGB(Ncolor[0],Ncolor[1],Ncolor[2]);
    leds[29] = CRGB(Ncolor[0],Ncolor[1],Ncolor[2]);

    leds[30] = CRGB(Dcolor[0],Dcolor[1],Dcolor[2]);
    leds[31] = CRGB(Dcolor[0],Dcolor[1],Dcolor[2]);
     
}

void name_THEORYfixedcolors(int Tcolor[], int Hcolor[], int Ecolor[], int Ocolor[], int Rcolor[], int Ycolor[]) {
  
    leds[32] = CRGB(Tcolor[0],Tcolor[1],Tcolor[2]);
    leds[33] = CRGB(Tcolor[0],Tcolor[1],Tcolor[2]);

    leds[34] = CRGB(Hcolor[0],Hcolor[1],Hcolor[2]);
    leds[35] = CRGB(Hcolor[0],Hcolor[1],Hcolor[2]);

    leds[36] = CRGB(Ecolor[0],Ecolor[1],Ecolor[2]);
    leds[37] = CRGB(Ecolor[0],Ecolor[1],Ecolor[2]);

    leds[38] = CRGB(Ocolor[0],Ocolor[1],Ocolor[2]);
    leds[39] = CRGB(Ocolor[0],Ocolor[1],Ocolor[2]);

    leds[40] = CRGB(Rcolor[0],Rcolor[1],Rcolor[2]);
    leds[41] = CRGB(Rcolor[0],Rcolor[1],Rcolor[2]);

    leds[42] = CRGB(Ycolor[0],Ycolor[1],Ycolor[2]);
    leds[43] = CRGB(Ycolor[0],Ycolor[1],Ycolor[2]);
    
}

unsigned long name_THEblink_timmer;   
int name_THEblink_timeset;
int name_THEblink_status;
int name_THEblink_loopcount;
void name_THEblink(int T1color[], int H1color[], int E1color[], unsigned long blacktime, unsigned long colortime){
  if(name_THEblink_timeset == 0){
    name_THEblink_timmer = millis();
    name_THEblink_timeset = 1;
    }
    if(name_THEblink_status == 0){
    
    name_THEfixedcolors(T1color, H1color, E1color);      
    if(millis() > (name_THEblink_timmer + colortime)){
      name_THEblink_status = 1;
      name_THEblink_timmer = millis();
    }
  }
  if(name_THEblink_status == 1){
    
    name_THEfixedcolors(noir, noir, noir);
    if(millis() > name_THEblink_timmer + blacktime){
      name_THEblink_status = 0;
      name_THEblink_timmer = millis();
      name_THEblink_loopcount++;
    }
  }
}

unsigned long name_ONEblink_timmer;   
int name_ONEblink_timeset;
int name_ONEblink_status;
int name_ONEblink_loopcount;
void name_ONEblink(int O2color[], int N2color[], int E2color[], unsigned long blacktime, unsigned long colortime){
  if(name_ONEblink_timeset == 0){
    name_ONEblink_timmer = millis();
    name_ONEblink_timeset = 1;
    }
    if(name_ONEblink_status == 0){
    
    name_ONEfixedcolors(O2color, N2color, E2color);      
    if(millis() > (name_ONEblink_timmer + colortime)){
      name_ONEblink_status = 1;
      name_ONEblink_timmer = millis();
    }
  }
  if(name_ONEblink_status == 1){
    
    name_ONEfixedcolors(noir, noir, noir);
    if(millis() > name_ONEblink_timmer + blacktime){
      name_ONEblink_status = 0;
      name_ONEblink_timmer = millis();
      name_ONEblink_loopcount++;
    }
  }
}



unsigned long name_MANblink_timmer;   
int name_MANblink_timeset;
int name_MANblink_status;
int name_MANblink_loopcount;
void name_MANblink(int M31color[], int A31color[], int N31color[], unsigned long blacktime, unsigned long colortime){
  if(name_MANblink_timeset == 0){
    name_MANblink_timmer = millis();
    name_MANblink_timeset = 1;
    }
    if(name_MANblink_status == 0){
    
    name_MANfixedcolors(M31color, A31color, N31color);
    //Serial.print(" status 0 ");     
    if(millis() > (name_MANblink_timmer + colortime)){
      name_MANblink_status = 1;
      name_MANblink_timmer = millis();
      
    }
  }
  if(name_MANblink_status == 1){
    
    name_MANfixedcolors(noir, noir, noir);
    if(millis() > (name_MANblink_timmer + blacktime)){
      name_MANblink_status = 0;
      name_MANblink_timmer = millis();
      name_MANblink_loopcount++;
    }
  }
  //Serial.print(" M31color ");
  //Serial.println(M31color[0]);
  Serial.println("millis");
  Serial.println(millis());
}

unsigned long name_BIGblink_timmer;   
int name_BIGblink_timeset;
int name_BIGblink_status;
int name_BIGblink_loopcount;
void name_BIGblink(int B4color[], int I4color[], int G4color[], unsigned long blacktime, unsigned long colortime){
  if(name_BIGblink_timeset == 0){
    name_BIGblink_timmer = millis();
    name_BIGblink_timeset = 1;
    }
    if(name_BIGblink_status == 0){
    
    name_BIGfixedcolors(B4color, I4color, G4color);      
    if(millis() > (name_BIGblink_timmer + colortime)){
      name_BIGblink_status = 1;
      name_BIGblink_timmer = millis();
    }
  }
  if(name_BIGblink_status == 1){
    
    name_BIGfixedcolors(noir, noir, noir);
    if(millis() > name_BIGblink_timmer + blacktime){
      name_BIGblink_status = 0;
      name_BIGblink_timmer = millis();
      name_BIGblink_loopcount++;
    }
  }
}


unsigned long name_BANDblink_timmer;   
int name_BANDblink_timeset;
int name_BANDblink_status;
int name_BANDblink_loopcount;
void name_BANDblink(int B5color[], int A5color[], int N5color[], int D5color[], unsigned long blacktime, unsigned long colortime){
  if(name_BANDblink_timeset == 0){
    name_BANDblink_timmer = millis();
    name_BANDblink_timeset = 1;
    }
    if(name_BANDblink_status == 0){
    
    name_BANDfixedcolors(B5color, A5color, N5color, D5color);      
    if(millis() > (name_BANDblink_timmer + colortime)){
      name_BANDblink_status = 1;
      name_BANDblink_timmer = millis();
    }
  }
  if(name_BANDblink_status == 1){
    
    name_BANDfixedcolors(noir, noir, noir, noir);
    if(millis() > name_BANDblink_timmer + blacktime){
      name_BANDblink_status = 0;
      name_BANDblink_timmer = millis();
      name_BANDblink_loopcount++;
    }
  }
}

unsigned long name_THEORYblink_timmer;   
int name_THEORYblink_timeset;
int name_THEORYblink_status;
int name_THEORYblink_loopcount;
void name_THEORYblink(int T6color[], int H6color[], int E6color[], int O6color[], int R6color[], int Y6color[], unsigned long blacktime, unsigned long colortime){
  if(name_THEORYblink_timeset == 0){
    name_THEORYblink_timmer = millis();
    name_THEORYblink_timeset = 1;
    }
    if(name_THEORYblink_status == 0){
    
    name_THEORYfixedcolors(T6color, H6color, E6color, O6color, R6color, Y6color);      
    if(millis() > (name_THEORYblink_timmer + colortime)){
      name_THEORYblink_status = 1;
      name_THEORYblink_timmer = millis();
    }
  }
  if(name_THEORYblink_status == 1){
    
    name_THEORYfixedcolors(noir, noir, noir, noir, noir, noir);
    if(millis() > name_THEORYblink_timmer + blacktime){
      name_THEORYblink_status = 0;
      name_THEORYblink_timmer = millis();
      name_THEORYblink_loopcount++;
    }
  }
}

unsigned long name_THEchenillar1_timmer;   
int name_THEchenillar1_timeset;
int name_THEchenillar1_status;
int name_THEchenillar1_loopcount;
void name_THEchenillar1(int T1color[], int H1color[], int E1color[], unsigned long steptime){
  if(name_THEchenillar1_timeset == 0){
    name_THEchenillar1_timmer = millis();
    name_THEchenillar1_timeset = 1;
    }
    if(name_THEchenillar1_status == 0){
    
    name_THEfixedcolors(T1color, noir, noir);      
    if(millis() > (name_THEchenillar1_timmer + steptime)){
      name_THEchenillar1_status = 1;
      name_THEchenillar1_timmer = millis();
    }
  }
  if(name_THEchenillar1_status == 1){
    
    name_THEfixedcolors(noir, H1color, noir);
    if(millis() > name_THEchenillar1_timmer + steptime){
      name_THEchenillar1_status = 2;
      name_THEchenillar1_timmer = millis();
    }
  }

  if(name_THEchenillar1_status == 2){
    
    name_THEfixedcolors(noir, noir, E1color);
  if(millis() > name_THEchenillar1_timmer + steptime){
      name_THEchenillar1_status = 0;
      name_THEchenillar1_timmer = millis();
      name_THEchenillar1_loopcount++;
    }
  }
}

unsigned long name_THEchenillar2_timmer;   
int name_THEchenillar2_timeset;
int name_THEchenillar2_status;
int name_THEchenillar2_loopcount;
void name_THEchenillar2(int T1color[], int H1color[], int E1color[], unsigned long steptime){
  if(name_THEchenillar2_timeset == 0){
    name_THEchenillar2_timmer = millis();
    name_THEchenillar2_timeset = 1;
    }
    if(name_THEchenillar2_status == 0){
    
    name_THEfixedcolors(noir, noir, E1color);      
    if(millis() > (name_THEchenillar2_timmer + steptime)){
      name_THEchenillar2_status = 1;
      name_THEchenillar2_timmer = millis();
    }
  }
  if(name_THEchenillar2_status == 1){
    
    name_THEfixedcolors(noir, H1color, noir);
    if(millis() > name_THEchenillar2_timmer + steptime){
      name_THEchenillar2_status = 2;
      name_THEchenillar2_timmer = millis();
    }
  }

  if(name_THEchenillar2_status == 2){
    name_THEfixedcolors(T1color, noir, noir);
    
  if(millis() > name_THEchenillar2_timmer + steptime){
      name_THEchenillar2_status = 0;
      name_THEchenillar2_timmer = millis();
      name_THEchenillar2_loopcount++;
    }
  }
}

unsigned long name_THEchenillar3_timmer;   
int name_THEchenillar3_timeset;
int name_THEchenillar3_status;
int name_THEchenillar3_loopcount;
void name_THEchenillar3(int T1color[], int H1color[], int E1color[], unsigned long steptime){
  if(name_THEchenillar3_timeset == 0){
    name_THEchenillar3_timmer = millis();
    name_THEchenillar3_timeset = 1;
    }
    if(name_THEchenillar3_status == 0){
    
    name_THEfixedcolors(T1color, noir, noir);      
    if(millis() > (name_THEchenillar3_timmer + steptime)){
      name_THEchenillar3_status = 1;
      name_THEchenillar3_timmer = millis();
    }
  }
  if(name_THEchenillar3_status == 1){
    
    name_THEfixedcolors(noir, H1color, noir);
    if(millis() > name_THEchenillar3_timmer + steptime){
      name_THEchenillar3_status = 2;
      name_THEchenillar3_timmer = millis();
    }
  }
  if(name_THEchenillar3_status == 2){
    
    name_THEfixedcolors(noir, noir, E1color);
    if(millis() > name_THEchenillar3_timmer + steptime){
      name_THEchenillar3_status = 3;
      name_THEchenillar3_timmer = millis();
    }
  }
  
  if(name_THEchenillar3_status == 3){
    
    name_THEfixedcolors(noir, H1color, noir);
    if(millis() > name_THEchenillar3_timmer + steptime){
      name_THEchenillar3_status = 0;
      name_THEchenillar3_timmer = millis();
      name_THEchenillar3_loopcount++;
    }
  }
}

unsigned long name_ONEchenillar1_timmer;   
int name_ONEchenillar1_timeset;
int name_ONEchenillar1_status;
int name_ONEchenillar1_loopcount;
void name_ONEchenillar1(int O2color[], int N2color[], int E2color[], unsigned long steptime){
  if(name_ONEchenillar1_timeset == 0){
    name_ONEchenillar1_timmer = millis();
    name_ONEchenillar1_timeset = 1;
    }
    if(name_ONEchenillar1_status == 0){
    
    name_ONEfixedcolors(O2color, noir, noir);      
    if(millis() > (name_ONEchenillar1_timmer + steptime)){
      name_ONEchenillar1_status = 1;
      name_ONEchenillar1_timmer = millis();
    }
  }
  if(name_ONEchenillar1_status == 1){
    
    name_ONEfixedcolors(noir, N2color, noir);
    if(millis() > name_ONEchenillar1_timmer + steptime){
      name_ONEchenillar1_status = 2;
      name_ONEchenillar1_timmer = millis();
    }
  }

  if(name_ONEchenillar1_status == 2){
    
    name_ONEfixedcolors(noir, noir, E2color);
  if(millis() > name_ONEchenillar1_timmer + steptime){
      name_ONEchenillar1_status = 0;
      name_ONEchenillar1_timmer = millis();
      name_ONEchenillar1_loopcount++;
    }
  }
}

unsigned long name_ONEchenillar2_timmer;   
int name_ONEchenillar2_timeset;
int name_ONEchenillar2_status;
int name_ONEchenillar2_loopcount;
void name_ONEchenillar2(int O2color[], int N2color[], int E2color[], unsigned long steptime){
  if(name_ONEchenillar2_timeset == 0){
    name_ONEchenillar2_timmer = millis();
    name_ONEchenillar2_timeset = 1;
    }
    if(name_ONEchenillar2_status == 0){
    
    name_ONEfixedcolors(noir, noir, E2color);      
    if(millis() > (name_ONEchenillar2_timmer + steptime)){
      name_ONEchenillar2_status = 1;
      name_ONEchenillar2_timmer = millis();
    }
  }
  if(name_ONEchenillar2_status == 1){
    
    name_ONEfixedcolors(noir, N2color, noir);
    if(millis() > name_ONEchenillar2_timmer + steptime){
      name_ONEchenillar2_status = 2;
      name_ONEchenillar2_timmer = millis();
    }
  }

  if(name_ONEchenillar2_status == 2){
    name_ONEfixedcolors(O2color, noir, noir);
    
  if(millis() > name_ONEchenillar2_timmer + steptime){
      name_ONEchenillar2_status = 0;
      name_ONEchenillar2_timmer = millis();
      name_ONEchenillar2_loopcount++;
    }
  }
}

unsigned long name_ONEchenillar3_timmer;   
int name_ONEchenillar3_timeset;
int name_ONEchenillar3_status;
int name_ONEchenillar3_loopcount;
void name_ONEchenillar3(int O2color[], int N2color[], int E2color[], unsigned long steptime){
  if(name_ONEchenillar3_timeset == 0){
    name_ONEchenillar3_timmer = millis();
    name_ONEchenillar3_timeset = 1;
    }
    if(name_ONEchenillar3_status == 0){
    
    name_ONEfixedcolors(O2color, noir, noir);      
    if(millis() > (name_ONEchenillar3_timmer + steptime)){
      name_ONEchenillar3_status = 1;
      name_ONEchenillar3_timmer = millis();
    }
  }
  if(name_ONEchenillar3_status == 1){
    
    name_ONEfixedcolors(noir, N2color, noir);
    if(millis() > name_ONEchenillar3_timmer + steptime){
      name_ONEchenillar3_status = 2;
      name_ONEchenillar3_timmer = millis();
    }
  }
  if(name_ONEchenillar3_status == 2){
    
    name_ONEfixedcolors(noir, noir, E2color);
    if(millis() > name_ONEchenillar3_timmer + steptime){
      name_ONEchenillar3_status = 3;
      name_ONEchenillar3_timmer = millis();
    }
  }
  
  if(name_ONEchenillar3_status == 3){
    
    name_ONEfixedcolors(noir, N2color, noir);
    if(millis() > name_ONEchenillar3_timmer + steptime){
      name_ONEchenillar3_status = 0;
      name_ONEchenillar3_timmer = millis();
      name_ONEchenillar3_loopcount++;
    }
  }
}

unsigned long name_MANchenillar1_timmer;   
int name_MANchenillar1_timeset;
int name_MANchenillar1_status;
int name_MANchenillar1_loopcount;
void name_MANchenillar1(int M3color[], int A3color[], int N3color[], unsigned long steptime){
  if(name_MANchenillar1_timeset == 0){
    name_MANchenillar1_timmer = millis();
    name_MANchenillar1_timeset = 1;
    }
    if(name_MANchenillar1_status == 0){
    
    name_MANfixedcolors(M3color, noir, noir);      
    if(millis() > (name_MANchenillar1_timmer + steptime)){
      name_MANchenillar1_status = 1;
      name_MANchenillar1_timmer = millis();
    }
  }
  if(name_MANchenillar1_status == 1){
    
    name_MANfixedcolors(noir, A3color, noir);
    if(millis() > name_MANchenillar1_timmer + steptime){
      name_MANchenillar1_status = 2;
      name_MANchenillar1_timmer = millis();
    }
  }

  if(name_MANchenillar1_status == 2){
    
    name_MANfixedcolors(noir, noir, N3color);
  if(millis() > name_MANchenillar1_timmer + steptime){
      name_MANchenillar1_status = 0;
      name_MANchenillar1_timmer = millis();
      name_MANchenillar1_loopcount++;
    }
  }
}

unsigned long name_MANchenillar2_timmer;   
int name_MANchenillar2_timeset;
int name_MANchenillar2_status;
int name_MANchenillar2_loopcount;
void name_MANchenillar2(int M3color[], int A3color[], int N3color[], unsigned long steptime){
  if(name_MANchenillar2_timeset == 0){
    name_MANchenillar2_timmer = millis();
    name_MANchenillar2_timeset = 1;
    }
    if(name_MANchenillar2_status == 0){
    
    name_MANfixedcolors(noir, noir, N3color);      
    if(millis() > (name_MANchenillar2_timmer + steptime)){
      name_MANchenillar2_status = 1;
      name_MANchenillar2_timmer = millis();
    }
  }
  if(name_MANchenillar2_status == 1){
    
    name_MANfixedcolors(noir, A3color, noir);
    if(millis() > name_MANchenillar2_timmer + steptime){
      name_MANchenillar2_status = 2;
      name_MANchenillar2_timmer = millis();
    }
  }

  if(name_MANchenillar2_status == 2){
    name_MANfixedcolors(M3color, noir, noir);
    
  if(millis() > name_MANchenillar2_timmer + steptime){
      name_MANchenillar2_status = 0;
      name_MANchenillar2_timmer = millis();
      name_MANchenillar2_loopcount++;
    }
  }
}

unsigned long name_MANchenillar3_timmer;   
int name_MANchenillar3_timeset;
int name_MANchenillar3_status;
int name_MANchenillar3_loopcount;
void name_MANchenillar3(int M3color[], int A3color[], int N3color[], unsigned long steptime){
  if(name_MANchenillar3_timeset == 0){
    name_MANchenillar3_timmer = millis();
    name_MANchenillar3_timeset = 1;
    }
    if(name_MANchenillar3_status == 0){
    
    name_MANfixedcolors(M3color, noir, noir);      
    if(millis() > (name_MANchenillar3_timmer + steptime)){
      name_MANchenillar3_status = 1;
      name_MANchenillar3_timmer = millis();
    }
  }
  if(name_MANchenillar3_status == 1){
    
    name_MANfixedcolors(noir, A3color, noir);
    if(millis() > name_MANchenillar3_timmer + steptime){
      name_MANchenillar3_status = 2;
      name_MANchenillar3_timmer = millis();
    }
  }
  if(name_MANchenillar3_status == 2){
    
    name_MANfixedcolors(noir, noir, N3color);
    if(millis() > name_MANchenillar3_timmer + steptime){
      name_MANchenillar3_status = 3;
      name_MANchenillar3_timmer = millis();
    }
  }
  
  if(name_MANchenillar3_status == 3){
    
    name_MANfixedcolors(noir, A3color, noir);
    if(millis() > name_MANchenillar3_timmer + steptime){
      name_MANchenillar3_status = 0;
      name_MANchenillar3_timmer = millis();
      name_MANchenillar3_loopcount++;
    }
  }
}

unsigned long name_BIGchenillar1_timmer;   
int name_BIGchenillar1_timeset;
int name_BIGchenillar1_status;
int name_BIGchenillar1_loopcount;
void name_BIGchenillar1(int B4color[], int I4color[], int G4color[], unsigned long steptime){
  if(name_BIGchenillar1_timeset == 0){
    name_BIGchenillar1_timmer = millis();
    name_BIGchenillar1_timeset = 1;
    }
    if(name_BIGchenillar1_status == 0){
    
    name_BIGfixedcolors(B4color, noir, noir);      
    if(millis() > (name_BIGchenillar1_timmer + steptime)){
      name_BIGchenillar1_status = 1;
      name_BIGchenillar1_timmer = millis();
    }
  }
  if(name_BIGchenillar1_status == 1){
    
    name_BIGfixedcolors(noir, I4color, noir);
    if(millis() > name_BIGchenillar1_timmer + steptime){
      name_BIGchenillar1_status = 2;
      name_BIGchenillar1_timmer = millis();
    }
  }

  if(name_BIGchenillar1_status == 2){
    
    name_BIGfixedcolors(noir, noir, G4color);
  if(millis() > name_BIGchenillar1_timmer + steptime){
      name_BIGchenillar1_status = 0;
      name_BIGchenillar1_timmer = millis();
      name_BIGchenillar1_loopcount++;
    }
  }
}

unsigned long name_BIGchenillar2_timmer;   
int name_BIGchenillar2_timeset;
int name_BIGchenillar2_status;
int name_BIGchenillar2_loopcount;
void name_BIGchenillar2(int B4color[], int I4color[], int G4color[], unsigned long steptime){
  if(name_BIGchenillar2_timeset == 0){
    name_BIGchenillar2_timmer = millis();
    name_BIGchenillar2_timeset = 1;
    }
    if(name_BIGchenillar2_status == 0){
    
    name_BIGfixedcolors(noir, noir, G4color);      
    if(millis() > (name_BIGchenillar2_timmer + steptime)){
      name_BIGchenillar2_status = 1;
      name_BIGchenillar2_timmer = millis();
    }
  }
  if(name_BIGchenillar2_status == 1){
    
    name_BIGfixedcolors(noir, I4color, noir);
    if(millis() > name_BIGchenillar2_timmer + steptime){
      name_BIGchenillar2_status = 2;
      name_BIGchenillar2_timmer = millis();
    }
  }

  if(name_BIGchenillar2_status == 2){
    name_BIGfixedcolors(B4color, noir, noir);
    
  if(millis() > name_BIGchenillar2_timmer + steptime){
      name_BIGchenillar2_status = 0;
      name_BIGchenillar2_timmer = millis();
      name_BIGchenillar2_loopcount++;
    }
  }
}

unsigned long name_BIGchenillar3_timmer;   
int name_BIGchenillar3_timeset;
int name_BIGchenillar3_status;
int name_BIGchenillar3_loopcount;
void name_BIGchenillar3(int B4color[], int I4color[], int G4color[], unsigned long steptime){
  if(name_BIGchenillar3_timeset == 0){
    name_BIGchenillar3_timmer = millis();
    name_BIGchenillar3_timeset = 1;
    }
    if(name_BIGchenillar3_status == 0){
    
    name_BIGfixedcolors(B4color, noir, noir);      
    if(millis() > (name_BIGchenillar3_timmer + steptime)){
      name_BIGchenillar3_status = 1;
      name_BIGchenillar3_timmer = millis();
    }
  }
  if(name_BIGchenillar3_status == 1){
    
    name_BIGfixedcolors(noir, I4color, noir);
    if(millis() > name_BIGchenillar3_timmer + steptime){
      name_BIGchenillar3_status = 2;
      name_BIGchenillar3_timmer = millis();
    }
  }
  if(name_BIGchenillar3_status == 2){
    
    name_BIGfixedcolors(noir, noir, G4color);
    if(millis() > name_BIGchenillar3_timmer + steptime){
      name_BIGchenillar3_status = 3;
      name_BIGchenillar3_timmer = millis();
    }
  }
  
  if(name_BIGchenillar3_status == 3){
    
    name_BIGfixedcolors(noir, I4color, noir);
    if(millis() > name_BIGchenillar3_timmer + steptime){
      name_BIGchenillar3_status = 0;
      name_BIGchenillar3_timmer = millis();
      name_BIGchenillar3_loopcount++;
    }
  }
}

unsigned long name_BANDchenillar1_timmer;   
int name_BANDchenillar1_timeset;
int name_BANDchenillar1_status;
int name_BANDchenillar1_loopcount;
void name_BANDchenillar1(int B5color[], int A5color[], int N5color[], int D5color[], unsigned long steptime){
  if(name_BANDchenillar1_timeset == 0){
    name_BANDchenillar1_timmer = millis();
    name_BANDchenillar1_timeset = 1;
    }
    if(name_BANDchenillar1_status == 0){
    
    name_BANDfixedcolors(B5color, noir, noir, noir);      
    if(millis() > (name_BANDchenillar1_timmer + steptime)){
      name_BANDchenillar1_status = 1;
      name_BANDchenillar1_timmer = millis();
    }
  }
  if(name_BANDchenillar1_status == 1){
    
    name_BANDfixedcolors(noir, A5color, noir, noir);
    if(millis() > name_BANDchenillar1_timmer + steptime){
      name_BANDchenillar1_status = 2;
      name_BANDchenillar1_timmer = millis();
    }
  }
  if(name_BANDchenillar1_status == 2){
    
    name_BANDfixedcolors(noir, noir, N5color, noir);
    if(millis() > name_BANDchenillar1_timmer + steptime){
      name_BANDchenillar1_status = 3;
      name_BANDchenillar1_timmer = millis();
    }
  }

  if(name_BANDchenillar1_status == 3){
    
    name_BANDfixedcolors(noir, noir, noir, D5color);
  if(millis() > name_BANDchenillar1_timmer + steptime){
      name_BANDchenillar1_status = 0;
      name_BANDchenillar1_timmer = millis();
      name_BANDchenillar1_loopcount++;
    }
  }
}

unsigned long name_BANDchenillar2_timmer;   
int name_BANDchenillar2_timeset;
int name_BANDchenillar2_status;
int name_BANDchenillar2_loopcount;
void name_BANDchenillar2(int B5color[], int A5color[], int N5color[], int D5color[], unsigned long steptime){
  if(name_BANDchenillar2_timeset == 0){
    name_BANDchenillar2_timmer = millis();
    name_BANDchenillar2_timeset = 1;
    }
    if(name_BANDchenillar2_status == 0){
    
    name_BANDfixedcolors(noir, noir, noir, D5color);      
    if(millis() > (name_BANDchenillar2_timmer + steptime)){
      name_BANDchenillar2_status = 1;
      name_BANDchenillar2_timmer = millis();
    }
  }
  if(name_BANDchenillar2_status == 1){
    
    name_BANDfixedcolors(noir, noir, N5color, noir);      
    if(millis() > (name_BANDchenillar2_timmer + steptime)){
      name_BANDchenillar2_status = 2;
      name_BANDchenillar2_timmer = millis();
    }
  }
  if(name_BANDchenillar2_status == 2){
    
    name_BANDfixedcolors(noir, A5color, noir, noir);
    if(millis() > name_BANDchenillar2_timmer + steptime){
      name_BANDchenillar2_status = 3;
      name_BANDchenillar2_timmer = millis();
    }
  }

  if(name_BANDchenillar2_status == 3){
    name_BANDfixedcolors(B5color, noir, noir, noir);
    
  if(millis() > name_BANDchenillar2_timmer + steptime){
      name_BANDchenillar2_status = 0;
      name_BANDchenillar2_timmer = millis();
      name_BANDchenillar2_loopcount++;
    }
  }
}

unsigned long name_BANDchenillar3_timmer;   
int name_BANDchenillar3_timeset;
int name_BANDchenillar3_status;
int name_BANDchenillar3_loopcount;
void name_BANDchenillar3(int B5color[], int A5color[],int N5color[],int D5color[], unsigned long steptime){
  if(name_BANDchenillar3_timeset == 0){
    name_BANDchenillar3_timmer = millis();
    name_BANDchenillar3_timeset = 1;
    }
    if(name_BANDchenillar3_status == 0){
    
    name_BANDfixedcolors(B5color, noir, noir, noir);      
    if(millis() > (name_BANDchenillar3_timmer + steptime)){
      name_BANDchenillar3_status = 1;
      name_BANDchenillar3_timmer = millis();
    }
  }
  if(name_BANDchenillar3_status == 1){
    
    name_BANDfixedcolors(noir, A5color, noir, noir);
    if(millis() > name_BANDchenillar3_timmer + steptime){
      name_BANDchenillar3_status = 2;
      name_BANDchenillar3_timmer = millis();
    }
  }
  if(name_BANDchenillar3_status == 2){
    
    name_BANDfixedcolors(noir, noir, N5color, noir);
    if(millis() > name_BANDchenillar3_timmer + steptime){
      name_BANDchenillar3_status = 3;
      name_BANDchenillar3_timmer = millis();
    }
  }
  if(name_BANDchenillar3_status == 3){
    
    name_BANDfixedcolors(noir, noir, noir, D5color);
    if(millis() > name_BANDchenillar3_timmer + steptime){
      name_BANDchenillar3_status = 4;
      name_BANDchenillar3_timmer = millis();
    }
  }
  
  if(name_BANDchenillar3_status == 4){
    
    name_BANDfixedcolors(noir, noir, N5color, noir);
    if(millis() > name_BANDchenillar3_timmer + steptime){
      name_BANDchenillar3_status = 5;
      name_BANDchenillar3_timmer = millis();
      
    }
  }
  if(name_BANDchenillar3_status == 5){
    
    name_BANDfixedcolors(noir, A5color, noir, noir);
    if(millis() > name_BANDchenillar3_timmer + steptime){
      name_BANDchenillar3_status = 0;
      name_BANDchenillar3_timmer = millis();
      name_BANDchenillar3_loopcount++;
    }
  }
}

unsigned long name_THEORYchenillar1_timmer;   
int name_THEORYchenillar1_timeset;
int name_THEORYchenillar1_status;
int name_THEORYchenillar1_loopcount;
void name_THEORYchenillar1(int T6color[], int H6color[], int E6color[], int O6color[], int R6color[], int Y6color[], unsigned long steptime){
  if(name_THEORYchenillar1_timeset == 0){
    name_THEORYchenillar1_timmer = millis();
    name_THEORYchenillar1_timeset = 1;
    }
    if(name_THEORYchenillar1_status == 0){
    
    name_THEORYfixedcolors(T6color, noir, noir, noir, noir, noir);      
    if(millis() > (name_THEORYchenillar1_timmer + steptime)){
      name_THEORYchenillar1_status = 1;
      name_THEORYchenillar1_timmer = millis();
    }
  }
  if(name_THEORYchenillar1_status == 1){
    
    name_THEORYfixedcolors(noir, H6color, noir, noir, noir, noir);
    if(millis() > name_THEORYchenillar1_timmer + steptime){
      name_THEORYchenillar1_status = 2;
      name_THEORYchenillar1_timmer = millis();
    }
  }
  if(name_THEORYchenillar1_status == 2){
    
    name_THEORYfixedcolors(noir, noir, E6color, noir, noir, noir);
    if(millis() > name_THEORYchenillar1_timmer + steptime){
      name_THEORYchenillar1_status = 3;
      name_THEORYchenillar1_timmer = millis();
    }
  }

  if(name_THEORYchenillar1_status == 3){
    
    name_THEORYfixedcolors(noir, noir, noir, O6color, noir, noir);
  if(millis() > name_THEORYchenillar1_timmer + steptime){
      name_THEORYchenillar1_status = 4;
      name_THEORYchenillar1_timmer = millis();
      
    }
  }

  if(name_THEORYchenillar1_status == 4){
    
    name_THEORYfixedcolors(noir, noir, noir, noir, R6color, noir);
  if(millis() > name_THEORYchenillar1_timmer + steptime){
      name_THEORYchenillar1_status = 5;
      name_THEORYchenillar1_timmer = millis();
      
    }
  }

  if(name_THEORYchenillar1_status == 5){
    
    name_THEORYfixedcolors(noir, noir, noir, noir, noir, Y6color);
  if(millis() > name_THEORYchenillar1_timmer + steptime){
      name_THEORYchenillar1_status = 0;
      name_THEORYchenillar1_timmer = millis();
      name_THEORYchenillar1_loopcount++;
    }
  }
}




unsigned long name_THEORYchenillar2_timmer;   
int name_THEORYchenillar2_timeset;
int name_THEORYchenillar2_status;
int name_THEORYchenillar2_loopcount;
void name_THEORYchenillar2(int T6color[], int H6color[], int E6color[],int O6color[], int R6color[], int Y6color[], unsigned long steptime){
  if(name_THEORYchenillar2_timeset == 0){
    name_THEORYchenillar2_timmer = millis();
    name_THEORYchenillar2_timeset = 1;
    }
 if(name_THEORYchenillar2_status == 0){
    
    name_THEORYfixedcolors(noir, noir, noir, noir, noir, Y6color);      
    if(millis() > (name_THEORYchenillar2_timmer + steptime)){
      name_THEORYchenillar2_status = 1;
      name_THEORYchenillar2_timmer = millis();
    }
  }
  if(name_THEORYchenillar2_status == 1){
    
    name_THEORYfixedcolors(noir, noir, noir, noir, R6color, noir);      
    if(millis() > (name_THEORYchenillar2_timmer + steptime)){
      name_THEORYchenillar2_status = 2;
      name_THEORYchenillar2_timmer = millis();
    }
  }
    
    if(name_THEORYchenillar2_status == 2){
    
    name_THEORYfixedcolors(noir, noir, noir, O6color, noir, noir);      
    if(millis() > (name_THEORYchenillar2_timmer + steptime)){
      name_THEORYchenillar2_status = 3;
      name_THEORYchenillar2_timmer = millis();
    }
  }
  if(name_THEORYchenillar2_status == 3){
    
    name_THEORYfixedcolors(noir, noir, E6color, noir, noir, noir);      
    if(millis() > (name_THEORYchenillar2_timmer + steptime)){
      name_THEORYchenillar2_status = 4;
      name_THEORYchenillar2_timmer = millis();
    }
  }
  if(name_THEORYchenillar2_status == 4){
    
    name_THEORYfixedcolors(noir, H6color, noir, noir, noir, noir);
    if(millis() > name_THEORYchenillar2_timmer + steptime){
      name_THEORYchenillar2_status = 5;
      name_THEORYchenillar2_timmer = millis();
    }
  }

  if(name_THEORYchenillar2_status == 5){
    name_THEORYfixedcolors(T6color, noir, noir, noir, noir, noir);
    
  if(millis() > name_THEORYchenillar2_timmer + steptime){
      name_THEORYchenillar2_status = 0;
      name_THEORYchenillar2_timmer = millis();
      name_THEORYchenillar2_loopcount++;
    }
  }
}

unsigned long name_THEORYchenillar3_timmer;   
int name_THEORYchenillar3_timeset;
int name_THEORYchenillar3_status;
int name_THEORYchenillar3_loopcount;
void name_THEORYchenillar3(int T6color[], int H6color[],int E6color[],int O6color[], int R6color[], int Y6color[], unsigned long steptime){
  if(name_THEORYchenillar3_timeset == 0){
    name_THEORYchenillar3_timmer = millis();
    name_THEORYchenillar3_timeset = 1;
    }
    if(name_THEORYchenillar3_status == 0){
    
    name_THEORYfixedcolors(T6color, noir, noir, noir, noir, noir);      
    if(millis() > (name_THEORYchenillar3_timmer + steptime)){
      name_THEORYchenillar3_status = 1;
      name_THEORYchenillar3_timmer = millis();
    }
  }
  if(name_THEORYchenillar3_status == 1){
    
    name_THEORYfixedcolors(noir, H6color, noir, noir, noir, noir);
    if(millis() > name_THEORYchenillar3_timmer + steptime){
      name_THEORYchenillar3_status = 2;
      name_THEORYchenillar3_timmer = millis();
    }
  }
  if(name_THEORYchenillar3_status == 2){
    
    name_THEORYfixedcolors(noir, noir, E6color, noir, noir, noir);
    if(millis() > name_THEORYchenillar3_timmer + steptime){
      name_THEORYchenillar3_status = 3;
      name_THEORYchenillar3_timmer = millis();
    }
  }

  if(name_THEORYchenillar3_status == 3){
    
    name_THEORYfixedcolors(noir, noir, noir, O6color, noir, noir);
  if(millis() > name_THEORYchenillar3_timmer + steptime){
      name_THEORYchenillar3_status = 4;
      name_THEORYchenillar3_timmer = millis();
      
    }
  }

  if(name_THEORYchenillar3_status == 4){
    
    name_THEORYfixedcolors(noir, noir, noir, noir, R6color, noir);
  if(millis() > name_THEORYchenillar3_timmer + steptime){
      name_THEORYchenillar3_status = 5;
      name_THEORYchenillar3_timmer = millis();
      
    }
  }

  if(name_THEORYchenillar3_status == 5){
    
    name_THEORYfixedcolors(noir, noir, noir, noir, noir, Y6color);
  if(millis() > name_THEORYchenillar3_timmer + steptime){
      name_THEORYchenillar3_status = 6;
      name_THEORYchenillar3_timmer = millis();
      
    }
  }
  if(name_THEORYchenillar3_status == 6){
    
    name_THEORYfixedcolors(noir, noir, noir, noir, R6color, noir);      
    if(millis() > (name_THEORYchenillar3_timmer + steptime)){
      name_THEORYchenillar3_status = 7;
      name_THEORYchenillar3_timmer = millis();
    }
  }
    
    if(name_THEORYchenillar3_status == 7){
    
    name_THEORYfixedcolors(noir, noir, noir, O6color, noir, noir);      
    if(millis() > (name_THEORYchenillar3_timmer + steptime)){
      name_THEORYchenillar3_status = 8;
      name_THEORYchenillar3_timmer = millis();
    }
  }
  if(name_THEORYchenillar3_status == 8){
    
    name_THEORYfixedcolors(noir, noir, E6color, noir, noir, noir);      
    if(millis() > (name_THEORYchenillar3_timmer + steptime)){
      name_THEORYchenillar3_status = 9;
      name_THEORYchenillar3_timmer = millis();
    }
  }
  if(name_THEORYchenillar3_status == 9){
    
    name_THEORYfixedcolors(noir, H6color, noir, noir, noir, noir);
    if(millis() > name_THEORYchenillar3_timmer + steptime){
      name_THEORYchenillar3_status = 0;
      name_THEORYchenillar3_timmer = millis();
      name_THEORYchenillar3_loopcount++;
    }
  }
}

unsigned long name_THEORYchenillar4_timmer;   
int name_THEORYchenillar4_timeset;
int name_THEORYchenillar4_status;
int name_THEORYchenillar4_loopcount;
void name_THEORYchenillar4(int T6color[], int H6color[],int E6color[],int O6color[], int R6color[], int Y6color[], unsigned long steptime){
  if(name_THEORYchenillar4_timeset == 0){
    name_THEORYchenillar4_timmer = millis();
    name_THEORYchenillar4_timeset = 1;
    }
    if(name_THEORYchenillar4_status == 0){
    
    name_THEORYfixedcolors(noir, noir, E6color, O6color, noir, noir);      
    if(millis() > (name_THEORYchenillar4_timmer + steptime)){
      name_THEORYchenillar4_status = 1;
      name_THEORYchenillar4_timmer = millis();
    }
  }
  if(name_THEORYchenillar4_status == 1){
    
    name_THEORYfixedcolors(noir, H6color, noir, noir, R6color, noir);
    if(millis() > name_THEORYchenillar4_timmer + steptime){
      name_THEORYchenillar4_status = 2;
      name_THEORYchenillar4_timmer = millis();
    }
  }
  if(name_THEORYchenillar4_status == 2){
    
    name_THEORYfixedcolors(H6color, noir, noir, noir, noir, Y6color);
    if(millis() > name_THEORYchenillar4_timmer + steptime){
      name_THEORYchenillar4_status = 3;
      name_THEORYchenillar4_timmer = millis();
    }
  }

  if(name_THEORYchenillar4_status == 3){
    
    name_THEORYfixedcolors(noir, H6color, noir, noir, R6color, noir);
  if(millis() > name_THEORYchenillar4_timmer + steptime){
      name_THEORYchenillar4_status = 0;
      name_THEORYchenillar4_timmer = millis();
      name_THEORYchenillar4_loopcount++;
    }
  }

}


unsigned long name_THEORYchenillar5_timmer;   
int name_THEORYchenillar5_timeset;
int name_THEORYchenillar5_status;
int name_THEORYchenillar5_loopcount;
void name_THEORYchenillar5(int T6color[], int H6color[],int E6color[],int O6color[], int R6color[], int Y6color[], unsigned long steptime){
  if(name_THEORYchenillar5_timeset == 0){
    name_THEORYchenillar5_timmer = millis();
    name_THEORYchenillar5_timeset = 1;
    }
    if(name_THEORYchenillar5_status == 0){
    
    name_THEORYfixedcolors(noir, noir, E6color, O6color, noir, noir);      
    if(millis() > (name_THEORYchenillar5_timmer + steptime)){
      name_THEORYchenillar5_status = 1;
      name_THEORYchenillar5_timmer = millis();
    }
  }
  if(name_THEORYchenillar5_status == 1){
    
    name_THEORYfixedcolors(noir, H6color, noir, noir, R6color, noir);
    if(millis() > name_THEORYchenillar5_timmer + steptime){
      name_THEORYchenillar5_status = 2;
      name_THEORYchenillar5_timmer = millis();
    }
  }
  if(name_THEORYchenillar5_status == 2){
    
    name_THEORYfixedcolors(H6color, noir, noir, noir, noir, Y6color);
    if(millis() > name_THEORYchenillar5_timmer + steptime){
      name_THEORYchenillar5_status = 0;
      name_THEORYchenillar5_timmer = millis();
      name_THEORYchenillar5_loopcount++;
    }
  }
}


unsigned long name_chenillarblanc_timmer;   
int name_chenillarblanc_timeset;
int name_chenillarblanc_status;
int name_chenillarblanc_loopcount;
void name_chenillarblanc(int steptime){

    if(name_chenillarblanc_timeset == 0){
    name_chenillarblanc_timmer = millis();
    name_chenillarblanc_timeset = 1;
    }
  if(name_chenillarblanc_status == 0){
    
    name_fixed1(blanc, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir);
      
    if(millis() > (name_chenillarblanc_timmer + steptime)){
      name_chenillarblanc_status = 1;
      name_chenillarblanc_timmer = millis();
    }
  }
  if(name_chenillarblanc_status == 1){
    
    name_fixed1(noir, blanc, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_chenillarblanc_timmer + steptime){
      name_chenillarblanc_status = 2;
      name_chenillarblanc_timmer = millis();
    }
  }

  if(name_chenillarblanc_status == 2){
    
    name_fixed1(noir, noir, blanc, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_chenillarblanc_timmer + steptime){
      name_chenillarblanc_status = 3;
      name_chenillarblanc_timmer = millis();
    }
  }

  if(name_chenillarblanc_status == 3){
    
    name_fixed1(noir, noir, noir, blanc, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_chenillarblanc_timmer + steptime){
      name_chenillarblanc_status = 4;
      name_chenillarblanc_timmer = millis();
    }
  }

  if(name_chenillarblanc_status == 4){
    
    name_fixed1(noir, noir, noir, noir, blanc, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_chenillarblanc_timmer + steptime){
      name_chenillarblanc_status = 5;
      name_chenillarblanc_timmer = millis();
    }
  }

  if(name_chenillarblanc_status == 5){
    
    name_fixed1(noir, noir, noir, noir, noir, blanc, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_chenillarblanc_timmer + steptime){
      name_chenillarblanc_status = 6;
      name_chenillarblanc_timmer = millis();
    }
  }

  if(name_chenillarblanc_status == 6){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, blanc, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_chenillarblanc_timmer + steptime){
      name_chenillarblanc_status = 7;
      name_chenillarblanc_timmer = millis();
    }
  }

  if(name_chenillarblanc_status == 7){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, noir, blanc, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_chenillarblanc_timmer + steptime){
      name_chenillarblanc_status = 8;
      name_chenillarblanc_timmer = millis();
    }
  }

  if(name_chenillarblanc_status == 8){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, noir, noir, blanc, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_chenillarblanc_timmer + steptime){
      name_chenillarblanc_status = 9;
      name_chenillarblanc_timmer = millis();
    }
  }

  if(name_chenillarblanc_status == 9){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, noir, noir, noir, blanc, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_chenillarblanc_timmer + steptime){
      name_chenillarblanc_status = 10;
      name_chenillarblanc_timmer = millis();
    }
  }

  if(name_chenillarblanc_status == 10){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, blanc, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_chenillarblanc_timmer + steptime){
      name_chenillarblanc_status = 11;
      name_chenillarblanc_timmer = millis();
    }
  }

  if(name_chenillarblanc_status == 11){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, blanc, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_chenillarblanc_timmer + steptime){
      name_chenillarblanc_status = 12;
      name_chenillarblanc_timmer = millis();
    }
  }

  if(name_chenillarblanc_status == 12){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, blanc, noir, noir, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_chenillarblanc_timmer + steptime){
      name_chenillarblanc_status = 13;
      name_chenillarblanc_timmer = millis();
    }
  }

  if(name_chenillarblanc_status == 13){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, blanc, noir, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_chenillarblanc_timmer + steptime){
      name_chenillarblanc_status = 14;
      name_chenillarblanc_timmer = millis();
    }
  }

  if(name_chenillarblanc_status == 14){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, blanc, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_chenillarblanc_timmer + steptime){
      name_chenillarblanc_status = 15;
      name_chenillarblanc_timmer = millis();
    }
  }

  if(name_chenillarblanc_status == 15){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, blanc, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_chenillarblanc_timmer + steptime){
      name_chenillarblanc_status = 16;
      name_chenillarblanc_timmer = millis();
    }
  }

  if(name_chenillarblanc_status == 16){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, blanc, noir, noir, noir, noir, noir);
    
    if(millis() > name_chenillarblanc_timmer + steptime){
      name_chenillarblanc_status = 17;
      name_chenillarblanc_timmer = millis();
    }
  }

  if(name_chenillarblanc_status == 16){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, blanc, noir, noir, noir, noir);
    
    if(millis() > name_chenillarblanc_timmer + steptime){
      name_chenillarblanc_status = 17;
      name_chenillarblanc_timmer = millis();
    }
  }

  if(name_chenillarblanc_status == 17){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, blanc, noir, noir, noir);
    
    if(millis() > name_chenillarblanc_timmer + steptime){
      name_chenillarblanc_status = 18;
      name_chenillarblanc_timmer = millis();
    }
  }

  if(name_chenillarblanc_status == 18){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, blanc, noir, noir);
    
    if(millis() > name_chenillarblanc_timmer + steptime){
      name_chenillarblanc_status = 19;
      name_chenillarblanc_timmer = millis();
    }
  }

  if(name_chenillarblanc_status == 19){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, blanc, noir);
    
    if(millis() > name_chenillarblanc_timmer + steptime){
      name_chenillarblanc_status = 20;
      name_chenillarblanc_timmer = millis();
    }
  }

  if(name_chenillarblanc_status == 20){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, blanc);
    
    if(millis() > name_chenillarblanc_timmer + steptime){
      name_chenillarblanc_status = 21;
      name_chenillarblanc_timmer = millis();
    }
  }

  if(name_chenillarblanc_status == 21){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_chenillarblanc_timmer + steptime){
      name_chenillarblanc_status = 0;
      name_chenillarblanc_loopcount++;
      name_chenillarblanc_timmer = millis();
    }
  }
}


unsigned long name_enchainement1_timmer;   
int name_enchainement1_timeset;
int name_enchainement1_status;

void name_enchainement1(int count1, int time2, int time3){
    int realcount1 = count1 -1;
    if(name_enchainement1_timeset == 0){
    name_enchainement1_timmer = millis();
    name_enchainement1_timeset = 1;
    }
  if(name_enchainement1_status == 0){
    name_chenillarblanc(60);
     
    if(name_chenillarblanc_loopcount > realcount1){
      name_enchainement1_status = 1;
      name_enchainement1_timmer = millis();
    }
  }
  if(name_enchainement1_status == 1){
    name_strombo(blanc, noir, 100, 100);
     
    if(millis() > (name_enchainement1_timmer + time2)){
      name_enchainement1_status = 2;
      name_fade1_init = 0;
      name_enchainement1_timmer = millis();
    }
  }
  if(name_enchainement1_status == 2){
    name_fade1(vert, gris, 3000, offset_list3);
     
    if(millis() > (name_enchainement1_timmer + time3)){
      name_enchainement1_status = 0;
      name_chenillarblanc_loopcount = 0;
      name_enchainement1_timmer = millis();
    }
  }
   Serial.print(" name_chenillarblanc_loopcount ");
  Serial.println(name_chenillarblanc_loopcount);
}


unsigned long name_chenillarcolor1_timmer;   
int name_chenillarcolor1_timeset;
int name_chenillarcolor1_status;
int name_chenillarcolor1_loopcount;
void name_chenillarcolor1(int color1[], unsigned long steptime){

    if(name_chenillarcolor1_timeset == 0){
    name_chenillarcolor1_timmer = millis();
    name_chenillarcolor1_timeset = 1;
    }
  if(name_chenillarcolor1_status == 0){
    
    name_fixed1(color1, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir);
      
    if(millis() > (name_chenillarcolor1_timmer + steptime)){
      name_chenillarcolor1_status = 1;
      name_chenillarcolor1_timmer = millis();
    }
  }
  if(name_chenillarcolor1_status == 1){
    
    name_fixed1(noir, color1, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_chenillarcolor1_timmer + steptime){
      name_chenillarcolor1_status = 2;
      name_chenillarcolor1_timmer = millis();
    }
  }

  if(name_chenillarcolor1_status == 2){
    
    name_fixed1(noir, noir, color1, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_chenillarcolor1_timmer + steptime){
      name_chenillarcolor1_status = 3;
      name_chenillarcolor1_timmer = millis();
    }
  }

  if(name_chenillarcolor1_status == 3){
    
    name_fixed1(noir, noir, noir, color1, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_chenillarcolor1_timmer + steptime){
      name_chenillarcolor1_status = 4;
      name_chenillarcolor1_timmer = millis();
    }
  }

  if(name_chenillarcolor1_status == 4){
    
    name_fixed1(noir, noir, noir, noir, color1, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_chenillarcolor1_timmer + steptime){
      name_chenillarcolor1_status = 5;
      name_chenillarcolor1_timmer = millis();
    }
  }

  if(name_chenillarcolor1_status == 5){
    
    name_fixed1(noir, noir, noir, noir, noir, color1, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_chenillarcolor1_timmer + steptime){
      name_chenillarcolor1_status = 6;
      name_chenillarcolor1_timmer = millis();
    }
  }

  if(name_chenillarcolor1_status == 6){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, color1, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_chenillarcolor1_timmer + steptime){
      name_chenillarcolor1_status = 7;
      name_chenillarcolor1_timmer = millis();
    }
  }

  if(name_chenillarcolor1_status == 7){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, noir, color1, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_chenillarcolor1_timmer + steptime){
      name_chenillarcolor1_status = 8;
      name_chenillarcolor1_timmer = millis();
    }
  }

  if(name_chenillarcolor1_status == 8){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, noir, noir, color1, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_chenillarcolor1_timmer + steptime){
      name_chenillarcolor1_status = 9;
      name_chenillarcolor1_timmer = millis();
    }
  }

  if(name_chenillarcolor1_status == 9){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, noir, noir, noir, color1, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_chenillarcolor1_timmer + steptime){
      name_chenillarcolor1_status = 10;
      name_chenillarcolor1_timmer = millis();
    }
  }

  if(name_chenillarcolor1_status == 10){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, color1, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_chenillarcolor1_timmer + steptime){
      name_chenillarcolor1_status = 11;
      name_chenillarcolor1_timmer = millis();
    }
  }

  if(name_chenillarcolor1_status == 11){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, color1, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_chenillarcolor1_timmer + steptime){
      name_chenillarcolor1_status = 12;
      name_chenillarcolor1_timmer = millis();
    }
  }

  if(name_chenillarcolor1_status == 12){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, color1, noir, noir, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_chenillarcolor1_timmer + steptime){
      name_chenillarcolor1_status = 13;
      name_chenillarcolor1_timmer = millis();
    }
  }

  if(name_chenillarcolor1_status == 13){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, color1, noir, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_chenillarcolor1_timmer + steptime){
      name_chenillarcolor1_status = 14;
      name_chenillarcolor1_timmer = millis();
    }
  }

  if(name_chenillarcolor1_status == 14){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, color1, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_chenillarcolor1_timmer + steptime){
      name_chenillarcolor1_status = 15;
      name_chenillarcolor1_timmer = millis();
    }
  }

  if(name_chenillarcolor1_status == 15){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, color1, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_chenillarcolor1_timmer + steptime){
      name_chenillarcolor1_status = 16;
      name_chenillarcolor1_timmer = millis();
    }
  }

  if(name_chenillarcolor1_status == 16){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, color1, noir, noir, noir, noir, noir);
    
    if(millis() > name_chenillarcolor1_timmer + steptime){
      name_chenillarcolor1_status = 17;
      name_chenillarcolor1_timmer = millis();
    }
  }

  if(name_chenillarcolor1_status == 16){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, color1, noir, noir, noir, noir);
    
    if(millis() > name_chenillarcolor1_timmer + steptime){
      name_chenillarcolor1_status = 17;
      name_chenillarcolor1_timmer = millis();
    }
  }

  if(name_chenillarcolor1_status == 17){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, color1, noir, noir, noir);
    
    if(millis() > name_chenillarcolor1_timmer + steptime){
      name_chenillarcolor1_status = 18;
      name_chenillarcolor1_timmer = millis();
    }
  }

  if(name_chenillarcolor1_status == 18){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, color1, noir, noir);
    
    if(millis() > name_chenillarcolor1_timmer + steptime){
      name_chenillarcolor1_status = 19;
      name_chenillarcolor1_timmer = millis();
    }
  }

  if(name_chenillarcolor1_status == 19){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, color1, noir);
    
    if(millis() > name_chenillarcolor1_timmer + steptime){
      name_chenillarcolor1_status = 20;
      name_chenillarcolor1_timmer = millis();
    }
  }

  if(name_chenillarcolor1_status == 20){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, color1);
    
    if(millis() > name_chenillarcolor1_timmer + steptime){
      name_chenillarcolor1_status = 21;
      name_chenillarcolor1_timmer = millis();
    }
  }

  if(name_chenillarcolor1_status == 21){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_chenillarcolor1_timmer + steptime){
      name_chenillarcolor1_status = 0;
      name_chenillarcolor1_loopcount++;
      name_chenillarcolor1_timmer = millis();
    }
  }
}

unsigned long name_chenillar3color_timmer;   
int name_chenillar3color_timeset;
int name_chenillar3color_status;
int name_chenillar3color_loopcount;
void name_chenillar3color(int color1[], int color2[], int color3[], unsigned long steptime){

    if(name_chenillar3color_timeset == 0){
    name_chenillar3color_timmer = millis();
    name_chenillar3color_timeset = 1;
    }
  if(name_chenillar3color_status == 0){
    
    name_fixed1(color1, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir);
      
    if(millis() > (name_chenillar3color_timmer + steptime)){
      name_chenillar3color_status = 1;
      name_chenillar3color_timmer = millis();
    }
  }
  if(name_chenillar3color_status == 1){
    
    name_fixed1(noir, color2, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_chenillar3color_timmer + steptime){
      name_chenillar3color_status = 2;
      name_chenillar3color_timmer = millis();
    }
  }

  if(name_chenillar3color_status == 2){
    
    name_fixed1(noir, noir, color3, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_chenillar3color_timmer + steptime){
      name_chenillar3color_status = 3;
      name_chenillar3color_timmer = millis();
    }
  }

  if(name_chenillar3color_status == 3){
    
    name_fixed1(noir, noir, noir, color1, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_chenillar3color_timmer + steptime){
      name_chenillar3color_status = 4;
      name_chenillar3color_timmer = millis();
    }
  }

  if(name_chenillar3color_status == 4){
    
    name_fixed1(noir, noir, noir, noir, color2, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_chenillar3color_timmer + steptime){
      name_chenillar3color_status = 5;
      name_chenillar3color_timmer = millis();
    }
  }

  if(name_chenillar3color_status == 5){
    
    name_fixed1(noir, noir, noir, noir, noir, color3, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_chenillar3color_timmer + steptime){
      name_chenillar3color_status = 6;
      name_chenillar3color_timmer = millis();
    }
  }

  if(name_chenillar3color_status == 6){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, color1, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_chenillar3color_timmer + steptime){
      name_chenillar3color_status = 7;
      name_chenillar3color_timmer = millis();
    }
  }

  if(name_chenillar3color_status == 7){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, noir, color2, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_chenillar3color_timmer + steptime){
      name_chenillar3color_status = 8;
      name_chenillar3color_timmer = millis();
    }
  }

  if(name_chenillar3color_status == 8){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, noir, noir, color3, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_chenillar3color_timmer + steptime){
      name_chenillar3color_status = 9;
      name_chenillar3color_timmer = millis();
    }
  }

  if(name_chenillar3color_status == 9){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, noir, noir, noir, color1, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_chenillar3color_timmer + steptime){
      name_chenillar3color_status = 10;
      name_chenillar3color_timmer = millis();
    }
  }

  if(name_chenillar3color_status == 10){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, color2, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_chenillar3color_timmer + steptime){
      name_chenillar3color_status = 11;
      name_chenillar3color_timmer = millis();
    }
  }

  if(name_chenillar3color_status == 11){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, color3, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_chenillar3color_timmer + steptime){
      name_chenillar3color_status = 12;
      name_chenillar3color_timmer = millis();
    }
  }

  if(name_chenillar3color_status == 12){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, color1, noir, noir, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_chenillar3color_timmer + steptime){
      name_chenillar3color_status = 13;
      name_chenillar3color_timmer = millis();
    }
  }

  if(name_chenillar3color_status == 13){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, color2, noir, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_chenillar3color_timmer + steptime){
      name_chenillar3color_status = 14;
      name_chenillar3color_timmer = millis();
    }
  }

  if(name_chenillar3color_status == 14){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, color3, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_chenillar3color_timmer + steptime){
      name_chenillar3color_status = 15;
      name_chenillar3color_timmer = millis();
    }
  }

  if(name_chenillar3color_status == 15){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, color1, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_chenillar3color_timmer + steptime){
      name_chenillar3color_status = 16;
      name_chenillar3color_timmer = millis();
    }
  }

  if(name_chenillar3color_status == 16){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, color2, noir, noir, noir, noir, noir);
    
    if(millis() > name_chenillar3color_timmer + steptime){
      name_chenillar3color_status = 17;
      name_chenillar3color_timmer = millis();
    }
  }

  if(name_chenillar3color_status == 16){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, color3, noir, noir, noir, noir);
    
    if(millis() > name_chenillar3color_timmer + steptime){
      name_chenillar3color_status = 17;
      name_chenillar3color_timmer = millis();
    }
  }

  if(name_chenillar3color_status == 17){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, color1, noir, noir, noir);
    
    if(millis() > name_chenillar3color_timmer + steptime){
      name_chenillar3color_status = 18;
      name_chenillar3color_timmer = millis();
    }
  }

  if(name_chenillar3color_status == 18){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, color2, noir, noir);
    
    if(millis() > name_chenillar3color_timmer + steptime){
      name_chenillar3color_status = 19;
      name_chenillar3color_timmer = millis();
    }
  }

  if(name_chenillar3color_status == 19){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, color3, noir);
    
    if(millis() > name_chenillar3color_timmer + steptime){
      name_chenillar3color_status = 20;
      name_chenillar3color_timmer = millis();
    }
  }

  if(name_chenillar3color_status == 20){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, color1);
    
    if(millis() > name_chenillar3color_timmer + steptime){
      name_chenillar3color_status = 21;
      name_chenillar3color_timmer = millis();
      name_chenillar3color_loopcount++;
    }
  }
  if(name_chenillar3color_status == 21){ 
    
    name_fixed1(color2, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_chenillar3color_timmer + steptime){
      name_chenillar3color_status = 22;
      name_chenillar3color_timmer = millis();
    }
  }

  if(name_chenillar3color_status == 22){
    
    name_fixed1(noir, color3, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_chenillar3color_timmer + steptime){
      name_chenillar3color_status = 23;
      name_chenillar3color_timmer = millis();
    }
  }

  if(name_chenillar3color_status == 23){
    
    name_fixed1(noir, noir, color1, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_chenillar3color_timmer + steptime){
      name_chenillar3color_status = 24;
      name_chenillar3color_timmer = millis();
    }
  }

  if(name_chenillar3color_status == 24){
    
    name_fixed1(noir, noir, noir, color2, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_chenillar3color_timmer + steptime){
      name_chenillar3color_status = 25;
      name_chenillar3color_timmer = millis();
    }
  }

  if(name_chenillar3color_status == 25){
    
    name_fixed1(noir, noir, noir, noir, color3, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_chenillar3color_timmer + steptime){
      name_chenillar3color_status = 26;
      name_chenillar3color_timmer = millis();
    }
  }

  if(name_chenillar3color_status == 26){
    
    name_fixed1(noir, noir, noir, noir, noir, color1, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_chenillar3color_timmer + steptime){
      name_chenillar3color_status = 27;
      name_chenillar3color_timmer = millis();
    }
  }

  if(name_chenillar3color_status == 27){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, color2, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_chenillar3color_timmer + steptime){
      name_chenillar3color_status = 28;
      name_chenillar3color_timmer = millis();
    }
  }

  if(name_chenillar3color_status == 28){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, noir, color3, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_chenillar3color_timmer + steptime){
      name_chenillar3color_status = 29;
      name_chenillar3color_timmer = millis();
    }
  }

  if(name_chenillar3color_status == 29){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, noir, noir, color1, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_chenillar3color_timmer + steptime){
      name_chenillar3color_status = 30;
      name_chenillar3color_timmer = millis();
    }
  }

  if(name_chenillar3color_status == 30){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, noir, noir, noir, color2, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_chenillar3color_timmer + steptime){
      name_chenillar3color_status = 31;
      name_chenillar3color_timmer = millis();
    }
  }

  if(name_chenillar3color_status == 31){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, color3, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_chenillar3color_timmer + steptime){
      name_chenillar3color_status = 32;
      name_chenillar3color_timmer = millis();
    }
  }

  if(name_chenillar3color_status == 32){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, color1, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_chenillar3color_timmer + steptime){
      name_chenillar3color_status = 33;
      name_chenillar3color_timmer = millis();
    }
  }

  if(name_chenillar3color_status == 33){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, color2, noir, noir, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_chenillar3color_timmer + steptime){
      name_chenillar3color_status = 34;
      name_chenillar3color_timmer = millis();
    }
  }

  if(name_chenillar3color_status == 34){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, color3, noir, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_chenillar3color_timmer + steptime){
      name_chenillar3color_status = 35;
      name_chenillar3color_timmer = millis();
    }
  }

  if(name_chenillar3color_status == 35){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, color1, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_chenillar3color_timmer + steptime){
      name_chenillar3color_status = 36;
      name_chenillar3color_timmer = millis();
    }
  }

  if(name_chenillar3color_status == 36){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, color2, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_chenillar3color_timmer + steptime){
      name_chenillar3color_status = 37;
      name_chenillar3color_timmer = millis();
    }
  }

  if(name_chenillar3color_status == 37){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, color3, noir, noir, noir, noir, noir);
    
    if(millis() > name_chenillar3color_timmer + steptime){
      name_chenillar3color_status = 38;
      name_chenillar3color_timmer = millis();
    }
  }

  if(name_chenillar3color_status == 38){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, color1, noir, noir, noir, noir);
    
    if(millis() > name_chenillar3color_timmer + steptime){
      name_chenillar3color_status = 40;
      name_chenillar3color_timmer = millis();
    }
  }

  if(name_chenillar3color_status == 40){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, color2, noir, noir, noir);
    
    if(millis() > name_chenillar3color_timmer + steptime){
      name_chenillar3color_status = 41;
      name_chenillar3color_timmer = millis();
    }
  }

  if(name_chenillar3color_status == 41){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, color3, noir, noir);
    
    if(millis() > name_chenillar3color_timmer + steptime){
      name_chenillar3color_status = 42;
      name_chenillar3color_timmer = millis();
    }
  }

  if(name_chenillar3color_status == 42){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, color1, noir);
    
    if(millis() > name_chenillar3color_timmer + steptime){
      name_chenillar3color_status = 43;
      name_chenillar3color_timmer = millis();
    }
  }
  if(name_chenillar3color_status == 43){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, color2);
    
    if(millis() > name_chenillar3color_timmer + steptime){
      name_chenillar3color_status = 44;
      name_chenillar3color_timmer = millis();
      name_chenillar3color_loopcount++;

    }
  }

  if(name_chenillar3color_status == 44){
    
    name_fixed1(color3, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_chenillar3color_timmer + steptime){
      name_chenillar3color_status = 45;
      name_chenillar3color_timmer = millis();
    }
  }

  if(name_chenillar3color_status == 45){
    
    name_fixed1(noir, color1, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_chenillar3color_timmer + steptime){
      name_chenillar3color_status = 46;
      name_chenillar3color_timmer = millis();
    }
  }

  if(name_chenillar3color_status == 46){
    
    name_fixed1(noir, noir, color2, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_chenillar3color_timmer + steptime){
      name_chenillar3color_status = 47;
      name_chenillar3color_timmer = millis();
    }
  }

  if(name_chenillar3color_status == 47){
    
    name_fixed1(noir, noir, noir, color3, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_chenillar3color_timmer + steptime){
      name_chenillar3color_status = 48;
      name_chenillar3color_timmer = millis();
    }
  }

  if(name_chenillar3color_status == 48){
    
    name_fixed1(noir, noir, noir, noir, color1, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_chenillar3color_timmer + steptime){
      name_chenillar3color_status = 49;
      name_chenillar3color_timmer = millis();
    }
  }

  if(name_chenillar3color_status == 49){
    
    name_fixed1(noir, noir, noir, noir, noir, color2, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_chenillar3color_timmer + steptime){
      name_chenillar3color_status = 50;
      name_chenillar3color_timmer = millis();
    }
  }

  if(name_chenillar3color_status == 50){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, color3, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_chenillar3color_timmer + steptime){
      name_chenillar3color_status = 51;
      name_chenillar3color_timmer = millis();
    }
  }

  if(name_chenillar3color_status == 51){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, noir, color1, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_chenillar3color_timmer + steptime){
      name_chenillar3color_status = 52;
      name_chenillar3color_timmer = millis();
    }
  }

  if(name_chenillar3color_status == 52){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, noir, noir, color2, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_chenillar3color_timmer + steptime){
      name_chenillar3color_status = 53;
      name_chenillar3color_timmer = millis();
    }
  }

  if(name_chenillar3color_status == 53){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, noir, noir, noir, color3, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_chenillar3color_timmer + steptime){
      name_chenillar3color_status = 54;
      name_chenillar3color_timmer = millis();
    }
  }

  if(name_chenillar3color_status == 54){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, color1, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_chenillar3color_timmer + steptime){
      name_chenillar3color_status = 55;
      name_chenillar3color_timmer = millis();
    }
  }

  if(name_chenillar3color_status == 55){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, color2, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_chenillar3color_timmer + steptime){
      name_chenillar3color_status = 56;
      name_chenillar3color_timmer = millis();
    }
  }

  if(name_chenillar3color_status == 56){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, color3, noir, noir, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_chenillar3color_timmer + steptime){
      name_chenillar3color_status = 57;
      name_chenillar3color_timmer = millis();
    }
  }

  if(name_chenillar3color_status == 57){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, color1, noir, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_chenillar3color_timmer + steptime){
      name_chenillar3color_status = 58;
      name_chenillar3color_timmer = millis();
    }
  }

  if(name_chenillar3color_status == 58){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, color2, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_chenillar3color_timmer + steptime){
      name_chenillar3color_status = 59;
      name_chenillar3color_timmer = millis();
    }
  }

  if(name_chenillar3color_status == 59){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, color3, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_chenillar3color_timmer + steptime){
      name_chenillar3color_status = 60;
      name_chenillar3color_timmer = millis();
    }
  }

  if(name_chenillar3color_status == 60){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, color1, noir, noir, noir, noir, noir);
    
    if(millis() > name_chenillar3color_timmer + steptime){
      name_chenillar3color_status = 61;
      name_chenillar3color_timmer = millis();
    }
  }

  if(name_chenillar3color_status == 61){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, color2, noir, noir, noir, noir);
    
    if(millis() > name_chenillar3color_timmer + steptime){
      name_chenillar3color_status = 62;
      name_chenillar3color_timmer = millis();
    }
  }

  if(name_chenillar3color_status == 62){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, color3, noir, noir, noir);
    
    if(millis() > name_chenillar3color_timmer + steptime){
      name_chenillar3color_status = 63;
      name_chenillar3color_timmer = millis();
    }
  }

  if(name_chenillar3color_status == 63){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, color1, noir, noir);
    
    if(millis() > name_chenillar3color_timmer + steptime){
      name_chenillar3color_status = 64;
      name_chenillar3color_timmer = millis();
    }
  }

  if(name_chenillar3color_status == 64){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, color2, noir);
    
    if(millis() > name_chenillar3color_timmer + steptime){
      name_chenillar3color_status = 65;
      name_chenillar3color_timmer = millis();
    }
  }

  if(name_chenillar3color_status == 65){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, color3);
    
    if(millis() > name_chenillar3color_timmer + steptime){
      name_chenillar3color_status = 0;
      name_chenillar3color_timmer = millis();
      name_chenillar3color_loopcount++;

    }
  }

}


unsigned long name_chenillartype2et3color_timmer;   
int name_chenillartype2et3color_timeset;
int name_chenillartype2et3color_status;
int name_chenillartype2et3color_loopcount;
void name_chenillartype2et3color(int color1[], int color2[], int color3[], unsigned long steptime){

    if(name_chenillartype2et3color_timeset == 0){
    name_chenillartype2et3color_timmer = millis();
    name_chenillartype2et3color_timeset = 1;
    }
  if(name_chenillartype2et3color_status == 0){
    
    name_fixed1(color1, color2, color3, color1, color2, color3, color1, color2, color3, color1, color2, color3, color1, color2, color3, color1, color2, color3, color1, color2, color3, color1);
      
    if(millis() > (name_chenillartype2et3color_timmer + steptime)){
      name_chenillartype2et3color_status = 1;
      name_chenillartype2et3color_timmer = millis();
    }
  }
  if(name_chenillartype2et3color_status == 1){
    
    name_fixed1(color2, color3, color1, color2, color3, color1, color2, color3, color1, color2, color3, color1, color2, color3, color1, color2, color3, color1, color2, color3, color1, color2);
    
    if(millis() > name_chenillartype2et3color_timmer + steptime){
      name_chenillartype2et3color_status = 2;
      name_chenillartype2et3color_timmer = millis();
    }
  }

  if(name_chenillartype2et3color_status == 2){
    
    name_fixed1(color3, color1, color2, color3, color1, color2, color3, color1, color2, color3, color1, color2, color3, color1, color2, color3, color1, color2, color3, color1, color2, color3);
    
    if(millis() > name_chenillartype2et3color_timmer + steptime){
      name_chenillartype2et3color_status = 0;
      name_chenillartype2et3color_timmer = millis();
      name_chenillartype2et3color_loopcount++;
    }
  }

}

unsigned long name_chenillartype3et3color_timmer;   
int name_chenillartype3et3color_timeset;
int name_chenillartype3et3color_status;
int name_chenillartype3et3color_loopcount;
void name_chenillartype3et3color(int color1[], int color2[], int color3[], unsigned long steptime){

    if(name_chenillartype3et3color_timeset == 0){
    name_chenillartype3et3color_timmer = millis();
    name_chenillartype3et3color_timeset = 1;
    }
  if(name_chenillartype3et3color_status == 0){
    
    name_fixed1(color2, color1, color3, color2, color1, color3, color2, color1, color3, color2, color1, color1, color2, color3, color1, color2, color3, color1, color2, color3, color1, color2);
      
    if(millis() > (name_chenillartype3et3color_timmer + steptime)){
      name_chenillartype3et3color_status = 1;
      name_chenillartype3et3color_timmer = millis();
    }
  }
  if(name_chenillartype3et3color_status == 1){
    
    name_fixed1(color1, color3, color2, color1, color3, color2, color1, color3, color2, color1, color3, color3, color1, color2, color3, color1, color2, color3, color1, color2, color3, color1);
    
    if(millis() > name_chenillartype3et3color_timmer + steptime){
      name_chenillartype3et3color_status = 2;
      name_chenillartype3et3color_timmer = millis();
    }
  }

  if(name_chenillartype3et3color_status == 2){
    
    name_fixed1(color3, color2, color1, color3, color2, color1, color3, color2, color1 ,color3 ,color2, color2, color3, color1, color2, color3, color1, color2, color3, color1, color2, color3);
    
    if(millis() > name_chenillartype3et3color_timmer + steptime){
      name_chenillartype3et3color_status = 0;
      name_chenillartype3et3color_timmer = millis();
      name_chenillartype3et3color_loopcount++;
    }
  }

}

unsigned long name_automaticchenillar_timmer;   
int name_automaticchenillar_timeset;
int name_automaticchenillar_status;
int name_automaticchenillar_loopcount;
void name_automaticchenillar(unsigned long thetime, unsigned long onetime, unsigned long mantime, unsigned long bigtime, unsigned long bandtime, unsigned long theorytime, unsigned long wordtime){

int T1[3] = {60, 220, 200};
int H1[3] = {100, 0, 255};
int E1[3] = {255, 255, 255};

int O2[3] = {255, 255, 0};
int N2[3] = {255, 0, 100};
int E2[3] = {60, 220, 200};

int M3[3] = {255, 0, 100};
int A3[3] = {60, 220, 200};
int N3[3] = {255, 255, 255};

int B4[3] = {255, 255, 0};
int I4[3] = {255, 0, 100};
int G4[3] = {60, 220, 200};

int B5[3] = {255, 200, 50};
int A5[3] = {0, 255, 0};
int N5[3] = {255, 255, 255};
int D5[3] = {255, 0, 100};

int T6[3] = {0, 0, 255};
int H6[3] = {255, 255, 0};
int E6[3] = {100, 100, 100};
int O6[3] = {255, 200, 50};
int R6[3] = {60, 220, 200};
int Y6[3] = {255, 0, 100};

    if(name_automaticchenillar_timeset == 0){
    name_automaticchenillar_timmer = millis();
    name_automaticchenillar_timeset = 1;
    }
  if(name_automaticchenillar_status == 0){
    
    name_fixed1(  T1, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir);
      
    if(millis() > (name_automaticchenillar_timmer + thetime)){
      name_automaticchenillar_status = 1;
      name_automaticchenillar_timmer = millis();
    }
  }
  if(name_automaticchenillar_status == 1){
    
    name_fixed1(noir,   H1, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir);

    if(millis() > name_automaticchenillar_timmer + thetime){
      name_automaticchenillar_status = 2;
      name_automaticchenillar_timmer = millis();
    }
  }

  if(name_automaticchenillar_status == 2){
    
    name_fixed1(noir, noir,   E1, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir);    
    if(millis() > name_automaticchenillar_timmer + thetime){
      name_automaticchenillar_status = 3;
      name_automaticchenillar_timmer = millis();
    }
  }

  if(name_automaticchenillar_status == 3){
    
    name_fixed1(  T1, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir);    
    if(millis() > name_automaticchenillar_timmer + thetime){
      name_automaticchenillar_status = 4;
      name_automaticchenillar_timmer = millis();
    }
  }

  if(name_automaticchenillar_status == 4){
    
    name_fixed1(noir,   H1, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_automaticchenillar_timmer + thetime){
      name_automaticchenillar_status = 5;
      name_automaticchenillar_timmer = millis();
    }
  }

  if(name_automaticchenillar_status == 5){
    
    name_fixed1(noir, noir,   E1, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_automaticchenillar_timmer + thetime){
      name_automaticchenillar_status = 6;
      name_automaticchenillar_timmer = millis();
    }
  }

  if(name_automaticchenillar_status == 6){
    
    name_fixed1(  T1, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_automaticchenillar_timmer + thetime){
      name_automaticchenillar_status = 7;
      name_automaticchenillar_timmer = millis();
    }
  }

  if(name_automaticchenillar_status == 7){
    
    name_fixed1(noir,   H1, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_automaticchenillar_timmer + thetime){
      name_automaticchenillar_status = 8;
      name_automaticchenillar_timmer = millis();
    }
  }

  if(name_automaticchenillar_status == 8){
    
    name_fixed1(noir, noir,   E1, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_automaticchenillar_timmer + thetime){
      name_automaticchenillar_status = 9;
      name_automaticchenillar_timmer = millis();
    }
  }

  if(name_automaticchenillar_status == 9){
    
    name_fixed1(T1, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_automaticchenillar_timmer + thetime){
      name_automaticchenillar_status = 10;
      name_automaticchenillar_timmer = millis();
    }
  }

  if(name_automaticchenillar_status == 10){
    
    name_fixed1(noir, H1, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, Y6);
    
    if(millis() > name_automaticchenillar_timmer + thetime){
      name_automaticchenillar_status = 11;
      name_automaticchenillar_timmer = millis();
    }
  }

  if(name_automaticchenillar_status == 11){
    
    name_fixed1(T1, H1, E1, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_automaticchenillar_timmer + wordtime){
      name_automaticchenillar_status = 12;
      name_automaticchenillar_timmer = millis();
    }
  }

  if(name_automaticchenillar_status == 12){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_automaticchenillar_timmer + mantime){
      name_automaticchenillar_status = 13;
      name_automaticchenillar_timmer = millis();
    }
  }

  if(name_automaticchenillar_status == 13){
    
    name_fixed1(noir, noir, noir, noir, noir, noir,   M3,   A3,   N3, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_automaticchenillar_timmer + mantime){
      name_automaticchenillar_status = 14;
      name_automaticchenillar_timmer = millis();
    }
  }

  if(name_automaticchenillar_status == 14){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_automaticchenillar_timmer + mantime){
      name_automaticchenillar_status = 15;
      name_automaticchenillar_timmer = millis();
    }
  }

  if(name_automaticchenillar_status == 15){
    
    name_fixed1(noir, noir, noir, noir, noir, noir,   M3,   A3,   N3, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_automaticchenillar_timmer + mantime){
      name_automaticchenillar_status = 16;
      name_automaticchenillar_timmer = millis();
    }
  }

  if(name_automaticchenillar_status == 16){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_automaticchenillar_timmer + mantime){
      name_automaticchenillar_status = 17;
      name_automaticchenillar_timmer = millis();
    }
  }

  if(name_automaticchenillar_status == 17){
    
    name_fixed1(noir, noir, noir, noir, noir, noir,   M3,   A3,   N3, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_automaticchenillar_timmer + mantime){
      name_automaticchenillar_status = 18;
      name_automaticchenillar_timmer = millis();
    }
  }

  if(name_automaticchenillar_status == 18){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_automaticchenillar_timmer + mantime){
      name_automaticchenillar_status = 19;
      name_automaticchenillar_timmer = millis();
    }
  }

  if(name_automaticchenillar_status == 19){
    
    name_fixed1(noir, noir, noir, noir, noir, noir,   M3,   A3,   N3, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_automaticchenillar_timmer + mantime){
      name_automaticchenillar_status = 20;
      name_automaticchenillar_timmer = millis();
    }
  }

  if(name_automaticchenillar_status == 20){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_automaticchenillar_timmer + mantime){
      name_automaticchenillar_status = 21;
      name_automaticchenillar_timmer = millis();
    }
  }

  if(name_automaticchenillar_status == 21){
    
  name_fixed1(noir, noir, noir, noir, noir, noir,   M3,   A3,   N3, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_automaticchenillar_timmer + wordtime){
      name_automaticchenillar_status = 22;
      name_automaticchenillar_loopcount++;
      name_automaticchenillar_timmer = millis();
    }
  }
  if(name_automaticchenillar_status == 22){
    
  name_fixed1(noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir,   D5, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_automaticchenillar_timmer + bandtime){
      name_automaticchenillar_status = 23;
      name_automaticchenillar_timmer = millis();
    }
  }
  if(name_automaticchenillar_status == 23){
    
   name_fixed1(noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir,   N5, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_automaticchenillar_timmer + bandtime){
      name_automaticchenillar_status = 24;
      name_automaticchenillar_timmer = millis();
    }
  }
  if(name_automaticchenillar_status == 24){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir,   A5, noir, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_automaticchenillar_timmer + bandtime){
      name_automaticchenillar_status = 25;
      name_automaticchenillar_timmer = millis();
    }
  }
  if(name_automaticchenillar_status == 25){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir,   B5, noir, noir, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_automaticchenillar_timmer + bandtime){
      name_automaticchenillar_status = 26;
      name_automaticchenillar_timmer = millis();
    }
  }
  if(name_automaticchenillar_status == 26){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir,   B5,   A5,   N5,   D5, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_automaticchenillar_timmer + wordtime){
      name_automaticchenillar_status = 27;
      name_automaticchenillar_timmer = millis();
    }
  }

  if(name_automaticchenillar_status == 27){
    
    name_fixed1(noir, noir, noir,   O2, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_automaticchenillar_timmer + onetime){
      name_automaticchenillar_status = 28;
      name_automaticchenillar_timmer = millis();
    }
  }

  if(name_automaticchenillar_status == 28){
    
    name_fixed1(noir, noir, noir, noir, noir,   E2, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_automaticchenillar_timmer + onetime){
      name_automaticchenillar_status = 29;
      name_automaticchenillar_timmer = millis();
    }
  }

  if(name_automaticchenillar_status == 29){
    
    name_fixed1(noir, noir, noir, noir,   N2, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_automaticchenillar_timmer + onetime){
      name_automaticchenillar_status = 30;
      name_automaticchenillar_timmer = millis();
    }
  }
  if(name_automaticchenillar_status == 30){
    
    name_fixed1(noir, noir, noir,   O2,   N2,   E2, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir);
    
    if(millis() > name_automaticchenillar_timmer + wordtime){
      name_automaticchenillar_status = 31;
      name_automaticchenillar_timmer = millis();
    }
  }
  
  if(name_automaticchenillar_status == 31){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, noir, noir, noir,   B4,   I4,   G4, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir);    
    if(millis() > name_automaticchenillar_timmer + bigtime){
      name_automaticchenillar_status = 32;
      name_automaticchenillar_timmer = millis();
    }
  }
  if(name_automaticchenillar_status == 32){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir,   E6,   O6, noir, noir);
    
    if(millis() > name_automaticchenillar_timmer + theorytime){
      name_automaticchenillar_status = 33;
      name_automaticchenillar_timmer = millis();
    }
  }
  if(name_automaticchenillar_status == 33){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir,   H6, noir, noir,   R6, noir);
      
    if(millis() > name_automaticchenillar_timmer + theorytime){
      name_automaticchenillar_status = 34;
      name_automaticchenillar_timmer = millis();
    }
  }
  if(name_automaticchenillar_status == 34){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir,   T6, noir, noir, noir, noir,   Y6);
    
    if(millis() > name_automaticchenillar_timmer + theorytime){
      name_automaticchenillar_status = 35;
      name_automaticchenillar_timmer = millis();
    }
  }
  if(name_automaticchenillar_status == 35){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir,   E6,   O6, noir, noir);
    
    if(millis() > name_automaticchenillar_timmer + theorytime){
      name_automaticchenillar_status = 36;
      name_automaticchenillar_timmer = millis();
    }
  }
  if(name_automaticchenillar_status == 36){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir,   H6, noir, noir,   R6, noir);
    
    if(millis() > name_automaticchenillar_timmer + theorytime){
      name_automaticchenillar_status = 37;
      name_automaticchenillar_timmer = millis();
    }
  }
  if(name_automaticchenillar_status == 37){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir,   T6, noir, noir, noir, noir,   Y6);
    
    if(millis() > name_automaticchenillar_timmer + theorytime){
      name_automaticchenillar_status = 38;
      name_automaticchenillar_timmer = millis();
    }
  }
  if(name_automaticchenillar_status == 38){
    
    name_fixed1(noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir,   T6,   H6,   E6,   O6,   R6,   Y6);    
    if(millis() > name_automaticchenillar_timmer + wordtime){
      name_automaticchenillar_status = 0;
      name_automaticchenillar_timmer = millis();
    }
  }
}


unsigned long int name_automaticenchainement1_timmer;
int name_automaticenchainement1_timeset;
int name_automaticenchainement1_status;
int name_automaticenchainement1_loopcount;

void name_automaticenchainement1(int T1color[], int H1color[], int E1color[], int O2color[], int N2color[], int E2color[], int M3color[], int A3color[], int N3color[], int B4color[], int I4color[], int G4color[], int B5color[], int A5color[], int N5color[], int D5color[], int T6color[],int H6color[],int E6color[], int O6color[], int R6color[], int Y6color[]){

  if(name_automaticenchainement1_timeset == 0){
    name_automaticenchainement1_timmer = millis();
    name_automaticenchainement1_timeset = 1;
    name_THEchenillar1_loopcount = 0;
    name_fixed1(noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir);
    }
  if(name_automaticenchainement1_status == 0){
    name_THEchenillar1(T1color, H1color, E1color, 200);
    name_THEORYfixedcolors(noir, noir, noir, noir, noir, noir);
     if(name_THEchenillar1_loopcount >= 3){
      name_automaticenchainement1_status = 1;
      name_automaticenchainement1_timmer = millis();
      name_THEchenillar1_loopcount = 0;
    }
  
  }
  if(name_automaticenchainement1_status == 1){
    name_THEfixedcolors(T1color, H1color, E1color);
     if(millis() > name_automaticenchainement1_timmer + 1400){
      name_automaticenchainement1_status = 2;
      name_automaticenchainement1_timmer = millis();
      
    }
  
  }
  if(name_automaticenchainement1_status == 2){
    name_THEfixedcolors(noir, noir, noir);
    name_MANblink(gris, gris, gris, 40, 40);
     if(name_MANblink_loopcount >= 5){
      name_automaticenchainement1_status = 3;
      name_automaticenchainement1_timmer = millis();
      name_MANblink_loopcount = 0;
    }
  
  }
  if(name_automaticenchainement1_status == 3){
    name_MANfixedcolors(M3color, A3color, N3color);
     if(millis() > name_automaticenchainement1_timmer + 1400){
      name_automaticenchainement1_status = 4;
      name_automaticenchainement1_timmer = millis();
      
    }
  
  }
  if(name_automaticenchainement1_status == 4){
    name_BANDchenillar2(B5color, A5color, N5color, D5color, 300);
    name_MANfixedcolors(noir, noir, noir);
     if(name_BANDchenillar2_loopcount >= 2){
      name_automaticenchainement1_status = 5;
      name_automaticenchainement1_timmer = millis();
      name_BANDchenillar2_loopcount = 0;
    }
  
  }
  if(name_automaticenchainement1_status == 5){
    name_BANDfixedcolors(B5color, A5color, N5color, D5color);
     if(millis() > name_automaticenchainement1_timmer + 1400){
      name_automaticenchainement1_status = 6;
      name_automaticenchainement1_timmer = millis();
      
    }
  
  }

  if(name_automaticenchainement1_status == 6){
    name_ONEchenillar3(O2color, N2color, E2color, 100);
    name_BANDfixedcolors(noir, noir, noir, noir);
     if(name_ONEchenillar3_loopcount >= 1){
      name_automaticenchainement1_status = 7;
      name_automaticenchainement1_timmer = millis();
      name_ONEchenillar3_loopcount = 0;
    }
  
  }
  if(name_automaticenchainement1_status == 7){
    name_ONEfixedcolors(O2color, N2color, E2color);
     if(millis() > name_automaticenchainement1_timmer + 1400){
      name_automaticenchainement1_status = 8;
      name_automaticenchainement1_timmer = millis();
      
    }
  
  }

  if(name_automaticenchainement1_status == 8){
    name_ONEfixedcolors(noir, noir, noir);
    name_BIGblink(B4color, I4color, G4color, 30, 30);
     if(name_BIGblink_loopcount >= 7){
      name_automaticenchainement1_status = 9;
      name_automaticenchainement1_timmer = millis();
      name_BIGblink_loopcount = 0;
    }
  
  }
  if(name_automaticenchainement1_status == 9){
    name_BIGfixedcolors(B4color, I4color, G4color);
     if(millis() > name_automaticenchainement1_timmer + 1200){
      name_automaticenchainement1_status = 10;
      name_automaticenchainement1_timmer = millis();
      
    }
  
  }

  if(name_automaticenchainement1_status == 10){
    name_THEORYchenillar5(T6color, H6color, E6color, O6color, R6color, Y6color, 300);
    name_BIGfixedcolors(noir, noir, noir);
     if(name_THEORYchenillar5_loopcount >= 2){
      name_automaticenchainement1_status = 11;
      name_automaticenchainement1_timmer = millis();
      name_THEORYchenillar5_loopcount = 0;
    }
  
  }
  if(name_automaticenchainement1_status == 11){
    name_THEORYfixedcolors(T6color, H6color, E6color, O6color, R6color, Y6color);
     if(millis() > name_automaticenchainement1_timmer + 2400){
      name_automaticenchainement1_status = 12;
      name_automaticenchainement1_timmer = millis();
      
    }
  }
  if(name_automaticenchainement1_status == 12){
      name_fixed1(noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir);
     if(millis() > name_automaticenchainement1_timmer + 600){
      name_automaticenchainement1_status = 13;
      name_automaticenchainement1_timmer = millis();
    }
   }

   if(name_automaticenchainement1_status == 13){
    name_chenillar3color( rose, ciel, jaune, 60);
     if(name_chenillar3color_loopcount >= 1){
      name_automaticenchainement1_status = 14;
      name_automaticenchainement1_timmer = millis();
      name_chenillar3color_loopcount = 0;
    }
  
  }
  if(name_automaticenchainement1_status == 14){
      name_fixed1(noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir);
     if(millis() > name_automaticenchainement1_timmer + 1000){
      name_automaticenchainement1_status = 15;
      name_automaticenchainement1_timmer = millis();
    }
   }
  if(name_automaticenchainement1_status == 15){
      name_fixed1(T1color, H1color, E1color, O2color, N2color, E2color, M3color, A3color, N3color, B4color, I4color, G4color, B5color, A5color, N5color, D5color, T6color, H6color, E6color, O6color, R6color, Y6color);
     if(millis() > name_automaticenchainement1_timmer + 3000){
      name_automaticenchainement1_status = 16;
      name_automaticenchainement1_timmer = millis();
    }
   }

   if(name_automaticenchainement1_status == 16){
      name_lightoffbycenter(T1color, H1color, E1color, O2color, N2color, E2color, M3color, A3color, N3color, B4color, I4color, G4color, B5color, A5color, N5color, D5color, T6color, H6color, E6color, O6color, R6color, Y6color, 200);
     if(name_lightoffbycenter_loopcount >= 1){
      name_automaticenchainement1_status = 17;
      name_automaticenchainement1_timmer = millis();
      name_lightoffbycenter_loopcount = 0;
    }
   }
  if(name_automaticenchainement1_status == 17){
      name_fixed1(noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir, noir);
     if(millis() > name_automaticenchainement1_timmer + 800){
      name_automaticenchainement1_status = 0;
      name_automaticenchainement1_timmer = millis();
    }
   }

   
}




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////                                                                         fonctions modes                                                                                        ///////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void all_lightoff() {
    fill_solid(leds, NUM_LEDS, CRGB(0,0,0));
    analogWrite(bot_led_pin_R, 0);
    analogWrite(bot_led_pin_G, 0);
    analogWrite(bot_led_pin_B, 0);
    
    analogWrite(top_led_pin_R, 0);
    analogWrite(top_led_pin_G, 0);
    analogWrite(top_led_pin_B, 0);
  
}

void mode2() {
  //Serial.print(" someflaschactivate ");
  //Serial.print(top_someflasch_activate);
  //Serial.print(" nothingtodo ");
  //Serial.print(top_nothingtodo);
 // Serial.print(" efecton ");
  //Serial.println(efecton);
  if(efecton == 1){
    switch(efect){  //de 144 a 160 (de 0x90 a 0x9F)
      case 144:
        timeonefect(10000);
        top_strombo(blanc, noir, 15,600); //oncolor, offcolor, ontime, offtime
        
      break;
      
      case 145:
        timeonefect(7000);
        programchange(131, 6990); //programchange(programnumber, time) durée doit etre inferieure a celle du timeoneffect 
        top_backgroundstrombo(blanc, 20, 700); //oncolor, ontime, offtime
        //name_speedup = name_speedup + 20;

      break;
      
      case 146:
        timeonefect(7000);
        programchange(131, 10000); //programchange(programnumber, time) durée doit etre inferieure a celle du timeoneffect 
        top_strombo(blanc, noir, 10,1200);
      break;
      
      case 147:
        top_strombo(blanc, vert, 10,150);

      break;
      case 148:
        efecton = 0;
      break;
      
      case 149:

      break;
    }
  }
  if((efecton == 0 or top_nothingtodo == 1) and top_someflasch_activate == 1){
    top_someflasch(blanc, 20, 6000);
  }
  if((efecton == 0 or top_nothingtodo == 1) and top_somemulticolors_activate == 1 and top_someflasch_active == 0){
    top_somemulticolors(bleu, vert, blanc, rose, 100, 100, 100, 100, 4000);
  }
  if((efecton == 0 or top_nothingtodo == 1) and top_someblack_activate == 1 and top_somemulticolors_active == 0 and top_someflasch_active == 0){
    top_someblack(noir, 800, 4000);
  }

  if((efecton == 0 or bot_nothingtodo == 1) and bot_someflasch_activate == 1){
    bot_someflasch(blanc, 20, 5000);
  }
  if((efecton == 0 or bot_nothingtodo == 1) and bot_somemulticolors_activate == 1 and bot_someflasch_active == 0){
    bot_somemulticolors(bleu, vert, blanc, rose, 100, 100, 100, 100, 4000);
  }
  if((efecton == 0 or bot_nothingtodo == 1) and bot_someblack_activate == 1 and bot_somemulticolors_active == 0 and bot_someflasch_active == 0){
    bot_someblack(noir, 400, 3600);
  }
 

  
  if((top_someflasch_activate == 0 and top_someblack_activate == 0 and top_somemulticolors_activate == 0 and efecton == 0) or top_nothingtodo == 1){
    switch(program){//de 128 a 143 (de 0x80 a 0x8F)
      
      case 128:
        //top_fadesiren(bleu, vert, 5000);
        if(A0_max > 0){
          top_fadetofix2_timeset = 0;
        }
        top_fadetofix2(bleu, noir, 1000);
        top_someflasch_activate = 0;
        top_someblack_activate = 0;
        top_somemulticolors_activate = 0;
        namestatus = 0;

        break;
        
      case 129:
        top_fadesiren(rouge, bleu, 15000);
        top_someflasch_activate = 0;
        top_someblack_activate = 1;
        top_somemulticolors_activate = 0;
        namestatus = 1;

      break;

      case 130:
        top_colors8(jaune, violet, rose, bleu, noir, violet, noir, vert, 1500, 5000, 3000, 150, 300, 150, 1000, 1000);
        top_someflasch_activate = 0;
        top_someblack_activate = 0;
        top_somemulticolors_activate =0;
        namestatus = 2;
        
      break;

      case 131:
        top_someflasch_activate = 0;
        namestatus = 3;
       

      break;
      
    }
  }
  if((bot_someflasch_activate == 0 and bot_someblack_activate == 0 and bot_somemulticolors_activate == 0 and efecton == 0) or bot_nothingtodo == 1){
    switch(program){//de 128 a 143 (de 0x80 a 0x8F)
      
      case 128:
        bot_fadesiren(bleu, vert, 8000);
        bot_someflasch_activate = 1;
        bot_someblack_activate = 1;
        bot_somemulticolors_activate = 1;
        name_speedup = 0;

        break;
        
      case 129:
        bot_fadesiren(rouge, bleu, 2000);
        bot_someflasch_activate = 1;
        bot_someblack_activate = 1;
        bot_somemulticolors_activate = 0;
        name_speedup = 0;

      break;

      case 130:
        bot_colors8(jaune, violet, rose, bleu, noir, violet, noir, vert, 1500, 5000, 3000, 150, 300, 150, 1000, 1000);
        bot_someflasch_activate = 0;
        bot_someblack_activate = 0;
        bot_somemulticolors_activate =0;
        name_speedup = 0;
        
      break;

      case 131:
        bot_someflasch_activate = 0;
       

      break;

      
    }
  }
  switch(namestatus){  
      case 0:
        name_fixedcolors(noir);
        name_fade1_offsetset = 0;
      break;
            
      case 1:
        if(timespend < 80 and efecton == 0){
           name_fade1_init = 0;
        }
        name_fade1(vert, rose, 5000, offset_list2);
      break;
      
      case 2:
        if(timespend < 80 and efecton == 0){
           name_fade1_init = 0;
        }
        name_enchainement1(3, 2000, 2000);
      break;
      
      case 3:
        if(timespend < 80 and efecton == 0){
           name_fade1_init = 0;
        }
        name_fade1(ciel, orange, 12000, offset_list3);
      break;
      
      case 4:

      break;
      
      case 5:
        
      break;
      
      case 6:

      break;
    }
}

/*0x02 hit the road jack
 *0x03 billie jean
 *0x04 automatic
 *0x05 if you feel it
 *0x06 feel good inc
 *0w07 snoop dog
 *0x08 bad girls
 *0x09 loose yourself to dance
 *0x10 finaly
 *0x11 the police
 *0x12 solo clean
 *0x13 solo satu
 *0x14 soft light mode
*/


//billie jean
void mode3() {
  disco_colors3(rouge, vert, bleu, 5000, 2000, 1000);
  bot_colors3(rouge, vert, bleu, 5000, 2000, 1000);
  top_colors3(rouge, vert, bleu, 5000, 2000, 1000);
  name_chenillarcolor1(bleu, 100);

}

//automatic
void mode4() {
   name_automaticchenillar(100, 200, 30, 3000, 400, 150, 2000);

  
}

//if you feel it
void mode5() {
  name_automaticenchainement1(jaune, ciel, rose, bleu, jaune, rose, vert, rose, soleil, blanc, jaune, ciel, violet, blanc, rose, jaune, turquoise, gris, rose, jaune, ciel, violet);
  
}
// feel good inc
void mode6() {
  name_chenillartype3et3color(vert, violet, noir, 500); //type3 et type 2 disponibles
  
}

//snoop dog
void mode7() {
  if(timespend < 80){
  name_fade3_init = 0;
  }
  name_fade3(ciel, orange, vert, 12000, offset_list3);
  
}

// bad girls
void mode8() {
  
}

// loose yourself to dance
void mode9() {
  name_THEchenillar1(bleu, vert, rouge, 200);
  name_ONEchenillar1(bleu, vert, rouge, 200);
  name_MANchenillar1(bleu, vert, rouge, 200);
  name_BIGchenillar1(bleu, vert, rouge, 200);
  name_BANDchenillar1(bleu, vert, rouge, rose, 200);
  name_THEORYchenillar1(rose, rose, rose, rose, rose, rose, 200);
  
}

//finaly  faire un bleu jaune blanc, un rose vert blanc,
void mode10() {
  name_THEchenillar2(bleu, vert, rouge, 200);
  name_ONEchenillar2(bleu, vert, rouge, 200);
  name_MANchenillar2(bleu, vert, rouge, 200);
  name_BIGchenillar2(bleu, vert, rouge, 200);
  name_BANDchenillar2(bleu, vert, rouge, rose, 200);
  name_THEORYchenillar4(rose, rose, rose, rose, rose, rose, 200);
}

void mode11() {
  name_THEchenillar3(bleu, vert, rouge, 200);
  name_ONEchenillar3(bleu, vert, rouge, 200);
  name_MANchenillar3(bleu, vert, rouge, 200);
  name_BIGchenillar3(bleu, vert, rouge, 200);
  name_BANDchenillar3(bleu, vert, rouge, rose, 200);
  name_THEORYchenillar5(rose, rose, rose, rose, rose, rose, 200);
}
void mode12() {
    name_fade3(verydarkorange,verydarkorange, darkrouge, 20000, offset_list5);

}

//solo satu
void mode13() {
  
    if(timespend < 80){
  name_fade3_init = 0;
  }
  name_fade3(verydarkrouge,verydarkrouge, darkrouge, 20000, offset_list5);
  
  
}


void setup() {
  // Join I2C bus as slave with address 8
  Wire.begin(0x8);
  Serial.begin(9600);
  
  // Call receiveEvent when data received                
  Wire.onReceive(receiveEvent);
  
  pinMode(top_led_pin_R, OUTPUT);
  pinMode(top_led_pin_G, OUTPUT);
  pinMode(top_led_pin_B, OUTPUT);

  pinMode(bot_led_pin_R, OUTPUT);
  pinMode(bot_led_pin_G, OUTPUT);
  pinMode(bot_led_pin_B, OUTPUT);

  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);

 // pwm1.begin();  pour l'extendeur de pwm I2C
 // pwm1.setOscillatorFrequency(27000000);
 // pwm1.setPWMFreq(100);
/*
  pinMode(bot_led_pin_R, OUTPUT);
  pinMode(bot_led_pin_G, OUTPUT);
  pinMode(bot_led_pin_B, OUTPUT);

 /* pinMode(disco_led_pin_R, OUTPUT);
  pinMode(disco_led_pin_G, OUTPUT);
  pinMode(disco_led_pin_B, OUTPUT);
  */
}
 
// Function that executes whenever data is received from master
void receiveEvent(int howMany) {
  while (Wire.available()) { // loop through all but the last
    //char c = Wire.read(); // receive byte as a character
    //digitalWrite(ledPin, c);
    x = Wire.read();
    eventtimmer = millis();
    //Serial.println(x);

  }
}
/*0x00 light off
 * 
 *0x02 hit the road jack
 *0x03 billie jean
 *0x04 automatic
 *0x05 if you feel it
 *0x06 feel good inc
 *0w07 snoop dog
 *0x08 bad girls
 *0x09 loose yourself to dance
 *0x10 finaly
 *0x11 the police
 *0x12 solo clean
 *0x13 solo satu
 *0x14 soft light mode
 *
 *effect: 144: strombo1, 145, multicolor, 146 boule disco 148 strombo2, 149 fadesiren //150 fils lumineux
 *
 *
 */
//                                              pads                                        //
//////////////////////////////////////////////////////////////////////////////////////////////
// 128  // 129 // 144 // 145 // 146 //        // 151
/////////////////////////////////////////////////////////////////////////////////////////////:
// 130  // 131 // 148 // 149 // 150 //        // 152 
///////////////////////////////////////////////////////////////////////////////////////////// 
void loop() {
  timespend = millis() - eventtimmer; 
  if(x >= 2 and x <= 127)  //de 0 a 127, (de 0x00 a 0x7F) le message envoyé par le raspberry est un changement de mode
    {
     mode = x;
     if(timespend < 100){
      efecton = 0;
     }
    }
  if(128 <= x and x < 144)
    {
      program = x; //de 128 a 143 (de 0x80 a 0x8F) le message envoyé par le raspberry est un controle ctrl
      if(timespend < 100){
      efecton = 0;
     }
    }
   if(144 <= x and x < 160)
    {
     efect = x; //de 144 a 160 (de 0x90 a 0x9F) le message envoyé par le raspberry est un controle ctrl
     if(timespend < 100){
      efecton = 1;
     }
    }
    if( x == 0){
      lightonoff = false;
    }
    if( x == 1){
      lightonoff = true;
    }
    if(lightonoff == false){
      all_lightoff();
    }
    if(lightonoff == true){
       switch(mode){

        case 2:
      
          mode2();
        break;

        case 3:
      
          mode3();
        break;

        case 4:
      
          mode4();
        break;

        case 5:
      
          mode5();
        break;

        case 6:
      
          mode6();
        break;

        case 7:
      
          mode7();
        break;

        case 8:
      
          mode8();
        break;

        case 9:
      
          mode9();
        break;

        case 10:
      
          mode10();
        break;

        case 11:
      
          mode11();
        break;

        case 12:
      
          mode12();
        break;

        case 13:
      
          mode13();
        break;
        }
      }
      //delay(70);
      FastLED.show();

      ///////////////////// lecture piezo //////////////////////
      A0_val = map(analogRead(analogPin), 0, 1023, 60, 127);
      if(A0_val > 66){
        
        A0_max = A0_val;
        
      }
      if(A0_val < 66){
        
        A0_max = 0;
        
      }
      
     
 /* Serial.print(" name_MANblink_loopcount ");
  Serial.print(name_MANblink_loopcount);
  Serial.print(" name_MANblink_timmer ");
  Serial.print(name_MANblink_timmer);
  Serial.print(" name_MANblink_status ");
  Serial.print(name_MANblink_status);
 
  Serial.print(" name_automaticenchainement1_status ");
  Serial.print(name_automaticenchainement1_status);
  Serial.print(" program ");
  Serial.print(program);
  Serial.print(" efect ");
  Serial.print(efect);
  Serial.print(" timespend ");
  Serial.print(timespend);
  Serial.print(" efecton ");
  Serial.println(efecton);
 
  */
}
