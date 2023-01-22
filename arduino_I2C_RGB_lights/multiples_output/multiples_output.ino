// Include the Wire library for I2C
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>


 Adafruit_PWMServoDriver pwm1 = Adafruit_PWMServoDriver(0x40);
// LED on pin 13
const int top_led_pin_R = 9;
const int top_led_pin_G = 11;
const int top_led_pin_B = 10;

const int bot_led_pin_R = 3;
const int bot_led_pin_G = 5;
const int bot_led_pin_B = 6;

/*const int disco_led_pin_R = 9;
const int disco_led_pin_G = 11;
const int disco_led_pin_B = 10;
*/
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////                                                   tableaux de couleurs                                                          //////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int rouge[3]={255, 0, 0};
int vert[3]={0, 255, 0};
int bleu[3]={0, 0, 255};
int noir[3]={0, 0, 0};
int blanc[3]={255, 255, 255};

int rose[3]={255, 0, 100};
int violet[3]={100, 0, 255};

int jaune[3]={255, 255, 0};
int orange[3]={255, 100, 0};

// variables globles fonctionement generales du programme
int x;
int mode; //un mode par musique (fonction loop)
int program; //corespond aux pads du controlleur midi (fonction loop)
int efect; //efect lumineux pads du controlleur midi
int efecton;
int efectfin; //=1 si un effect est activé
int top_nothingtodo; // =1 si un effet a besoin d'un arrierre plan
int bot_nothingtodo;
int disco_nothingtodo;
unsigned long eventtimmer;
unsigned long timespend;  //fonction loop: temp depuis le dernier message smbus reçu part le raspberry

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
      Serial.print("bonjourlesnoobs");
         delay(200);
      break;

      case 1:
      top_fadesiren_status = 0;
      break;
      
     }     
  }  
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////                                                        fonctions sorties pwm ( program et efect ) bot                                            /////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*void bot_fadetofix(int startcolor, int endcolor, unsigned long initime, unsigned long finaltime) {
  int i;
  float advance;
  advance = 1000*initime/finaltime;
  if(advance < 1000){
    i = startcolor*(1000 - (advance))/1000 + endcolor*(advance)/1000;
    pwm1.setPWM(0, r 0);
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
    pwm1.setPWM(0, 0, oncolor[0]);
    pwm1.setPWM(2, 0, oncolor[1]);
    pwm1.setPWM(1, 0, oncolor[2]);
    if(millis() > (ontime + bot_strombo_timmer)){
      bot_strombo_status = 0;
      bot_strombo_timmer = millis();
    }
  }
  if(bot_strombo_status == 0){
    pwm1.setPWM(0, 0, 16*offcolor[0]);
    pwm1.setPWM(2, 0, 16*offcolor[1]);
    pwm1.setPWM(1, 0, 16*offcolor[2]);
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
    pwm1.setPWM(0, 0, 16*oncolor[0]);
    pwm1.setPWM(2, 0, 16*oncolor[1]);
    pwm1.setPWM(1, 0, 16*oncolor[2]);
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
    pwm1.setPWM(0, 0, 16*oncolor[0]);
    pwm1.setPWM(2, 0, 16*oncolor[1]);
    pwm1.setPWM(1, 0, 16*oncolor[2]);
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
    pwm1.setPWM(0, 0, 16*oncolor[0]);
    pwm1.setPWM(2, 0, 16*oncolor[1]);
    pwm1.setPWM(1, 0, 16*oncolor[2]);
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
    pwm1.setPWM(0, 0, 16*color1[0]);
    pwm1.setPWM(2, 0, 16*color1[1]);
    pwm1.setPWM(1, 0, 16*color1[2]);
    if(millis() > (bot_somemulticolors_timmer + time1)){
      bot_somemulticolors_status = 1;
      bot_somemulticolors_timmer = millis();
    }
  }
  if(bot_somemulticolors_status == 1){
    bot_nothingtodo = 0;
    bot_somemulticolors_active = 1;
    pwm1.setPWM(0, 0, 16*color2[0]);
    pwm1.setPWM(2, 0, 16*color2[1]);
    pwm1.setPWM(1, 0, 16*color2[2]);
    if(millis() > bot_somemulticolors_timmer + time2){
      bot_somemulticolors_status = 2;
      bot_somemulticolors_timmer = millis();
    }
  }
  if(bot_somemulticolors_status == 2){
    bot_nothingtodo =0;
    bot_somemulticolors_active = 1;
    pwm1.setPWM(0, 0, 16*color3[0]);
    pwm1.setPWM(2, 0, 16*color3[1]);
    pwm1.setPWM(1, 0, 16*color3[2]);
    if(millis() > bot_somemulticolors_timmer + time3){
      bot_somemulticolors_status = 3;
      bot_somemulticolors_timmer = millis();
    }
  }
  if(bot_somemulticolors_status == 3){
    bot_nothingtodo =0;
    bot_somemulticolors_active = 1;
    pwm1.setPWM(0, 0, 16*color4[0]);
    pwm1.setPWM(2, 0, 16*color4[1]);
    pwm1.setPWM(1, 0, 16*color4[2]);
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
    pwm1.setPWM(0, 0, 16*color1[0]);
    pwm1.setPWM(2, 0, 16*color1[1]);
    pwm1.setPWM(1, 0, 16*color1[2]);
    if(millis() > (bot_colors3_timmer + time1)){
      bot_colors3_status = 1;
      bot_colors3_timmer = millis();
    }
  }
  if(bot_colors3_status == 1){
    pwm1.setPWM(0, 0, 16*color2[0]);
    pwm1.setPWM(2, 0, 16*color2[1]);
    pwm1.setPWM(1, 0, 16*color2[2]);
    if(millis() > bot_colors3_timmer + time2){
      bot_colors3_status = 2;
      bot_colors3_timmer = millis();
    }
  }
  if(bot_colors3_status == 2){
    pwm1.setPWM(0, 0, 16*color3[0]);
    pwm1.setPWM(2, 0, 16*color3[1]);
    pwm1.setPWM(1, 0, 16*color3[2]);
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
    pwm1.setPWM(0, 0, 16*color1[0]);
    pwm1.setPWM(2, 0, 16*color1[1]);
    pwm1.setPWM(1, 0, 16*color1[2]);
    if(millis() > (bot_colors8_timmer + time1)){
      bot_colors8_status = 1;
      bot_colors8_timmer = millis();
    }
  }
  if(bot_colors8_status == 1){
    pwm1.setPWM(0, 0, 16*color2[0]);
    pwm1.setPWM(2, 0, 16*color2[1]);
    pwm1.setPWM(1, 0, 16*color2[2]);
    if(millis() > bot_colors8_timmer + time2){
      bot_colors8_status = 2;
      bot_colors8_timmer = millis();
    }
  }
  if(bot_colors8_status == 2){
    pwm1.setPWM(0, 0, 16*color3[0]);
    pwm1.setPWM(2, 0, 16*color3[1]);
    pwm1.setPWM(1, 0, 16*color3[2]);
    if(millis() > bot_colors8_timmer + time3){
      bot_colors8_status = 3;
      bot_colors8_timmer = millis();
    }
  }
  if(bot_colors8_status == 3){
    pwm1.setPWM(0, 0, 16*color4[0]);
    pwm1.setPWM(2, 0, 16*color4[1]);
    pwm1.setPWM(1, 0, 16*color4[2]);
    if(millis() > bot_colors8_timmer + time4){
      bot_colors8_status = 4;
      bot_colors8_timmer = millis();
    }
  }
  if(bot_colors8_status == 4){
    pwm1.setPWM(0, 0, 16*color5[0]);
    pwm1.setPWM(2, 0, 16*color5[1]);
    pwm1.setPWM(1, 0, 16*color5[2]);
    if(millis() > bot_colors8_timmer + time5){
      bot_colors8_status = 5;
      bot_colors8_timmer = millis();
    }
  }
  if(bot_colors8_status == 5){
    pwm1.setPWM(0, 0, 16*color6[0]);
    pwm1.setPWM(2, 0, 16*color6[1]);
    pwm1.setPWM(1, 0, 16*color6[2]);
    if(millis() > bot_colors8_timmer + time6){
      bot_colors8_status = 6;
      bot_colors8_timmer = millis();
    }
  }
  if(bot_colors8_status == 6){
    pwm1.setPWM(0, 0, 16*color7[0]);
    pwm1.setPWM(2, 0, 16*color7[1]);
    pwm1.setPWM(1, 0, 16*color7[2]);
    if(millis() > bot_colors8_timmer + time7){
      bot_colors8_status = 7;
      bot_colors8_timmer = millis();
    }
  }
  if(bot_colors8_status == 7){
    pwm1.setPWM(0, 0, 16*color8[0]);
    pwm1.setPWM(2, 0, 16*color8[1]);
    pwm1.setPWM(1, 0, 16*color8[2]);
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
    r = (startcolor[0]*(1000 - (advance))/1000 + endcolor[0]*(advance)/1000)*16;
    g = (startcolor[1]*(1000 - (advance))/1000 + endcolor[1]*(advance)/1000)*16;
    b = (startcolor[2]*(1000 - (advance))/1000 + endcolor[2]*(advance)/1000)*16;

    pwm1.setPWM(0, 0, r);
    pwm1.setPWM(2, 0, g);
    pwm1.setPWM(1, 0, b);
    }
    
  if(bot_fadesiren_status == 0){
    r = (endcolor[0]*(1000 - (advance))/1000 + startcolor[0]*(advance)/1000)*16;
    g = (endcolor[1]*(1000 - (advance))/1000 + startcolor[1]*(advance)/1000)*16;
    b = (endcolor[2]*(1000 - (advance))/1000 + startcolor[2]*(advance)/1000)*16;

    pwm1.setPWM(0, 0, r);
    pwm1.setPWM(2, 0, g);
    pwm1.setPWM(1, 0, b);
  }
  if(bot_fadesiren_timmer2 > (finaltime)){
     bot_fadesiren_timeset = 0;
     switch(bot_fadesiren_status){
      case 0:
      bot_fadesiren_status = 1;
      Serial.print("bonjourlesnoobs");
         delay(200);
      break;

      case 1:
      bot_fadesiren_status = 0;
      break;
      
     }     
  }  
}
/*
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
      Serial.print("bonjourlesnoobs");
         delay(200);
      break;

      case 1:
      disco_fadesiren_status = 0;
      break;
      
     }     
  }  
}
*/
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////                                                  fonctions modes                                                ///////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void mode0() {
  Serial.print(" someflaschactivate ");
  Serial.print(top_someflasch_activate);
  Serial.print(" nothingtodo ");
  Serial.print(top_nothingtodo);
 // Serial.print(" efecton ");
  //Serial.println(efecton);
  if(efecton == 1){
    switch(efect){  //de 144 a 160 (de 0x90 a 0x9F)
      case 144:
        timeonefect(10000);
        top_strombo(vert, bleu, 10,600); //oncolor, offcolor, ontime, offtime
      break;
      
      case 145:
        timeonefect(7000);
        programchange(131, 6990); //programchange(programnumber, time) durée doit etre inferieure a celle du timeoneffect 
        top_backgroundstrombo(bleu, 20, 700); //oncolor, ontime, offtime

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
        top_fadesiren(bleu, vert, 5000);
        top_someflasch_activate = 1;
        top_someblack_activate = 1;
        top_somemulticolors_activate = 1;

        break;
        
      case 129:
        top_fadesiren(rouge, bleu, 15000);
        top_someflasch_activate = 0;
        top_someblack_activate = 1;
        top_somemulticolors_activate = 0;

      break;

      case 130:
        top_colors8(jaune, violet, rose, bleu, noir, violet, noir, vert, 1500, 5000, 3000, 150, 300, 150, 1000, 1000);
        top_someflasch_activate = 0;
        top_someblack_activate = 0;
        top_somemulticolors_activate =0;
        
      break;

      case 131:
        top_someflasch_activate = 0;
       

      break;

      case 132:
        
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

        break;
        
      case 129:
        bot_fadesiren(rouge, bleu, 2000);
        bot_someflasch_activate = 1;
        bot_someblack_activate = 1;
        bot_somemulticolors_activate = 0;

      break;

      case 130:
        bot_colors8(jaune, violet, rose, bleu, noir, violet, noir, vert, 1500, 5000, 3000, 150, 300, 150, 1000, 1000);
        bot_someflasch_activate = 0;
        bot_someblack_activate = 0;
        bot_somemulticolors_activate =0;
        
      break;

      case 131:
        bot_someflasch_activate = 0;
       

      break;

      case 132:
        
      break;
      
    }
  }
}

void mode1() {
  //strombo(255, 5, 10, 700);
 // delay(250);
}



void setup() {
  // Join I2C bus as slave with address 8
  Wire.begin(0x8);
  
  // Call receiveEvent when data received                
  Wire.onReceive(receiveEvent);
  
  pinMode(top_led_pin_R, OUTPUT);
  pinMode(top_led_pin_G, OUTPUT);
  pinMode(top_led_pin_B, OUTPUT);

  pwm1.begin();
  pwm1.setOscillatorFrequency(27000000);
  pwm1.setPWMFreq(100);
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

void loop() {
  timespend = millis() - eventtimmer; 
  if(x <= 127)  //de 0 a 127, (de 0x00 a 0x7F) le message envoyé par le raspberry est un changement de mode
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
   switch(mode){
      case 0:
      
        mode0();
      break;

      case 1:
      
        mode1();
      break;
      }
      
  Serial.print(" mode ");
  Serial.print(mode);
  Serial.print(" program ");
  Serial.print(program);
  Serial.print(" efect ");
  Serial.print(efect);
  Serial.print(" timespend ");
  Serial.print(timespend);
  Serial.print(" efecton ");
  Serial.println(efecton);
  
}
