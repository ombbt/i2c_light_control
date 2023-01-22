// Include the Wire library for I2C
#include <Wire.h>
 
// LED on pin 13
const int led_pin_R = 9;
const int led_pin_G = 11;
const int led_pin_B = 10;

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
int efecton; //=1 si un effect est activé
int nothingtodo; // =1 si un effet a besoin d'un arrierre plan
unsigned long eventtimmer;
unsigned long timespend;  //fonction loop: temp depuis le dernier message smbus reçu part le raspberry

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////                                               fonctions generales                                                               //////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void timeonefect( unsigned long finaltime){
  if(finaltime < timespend){
    efecton = 0;
  }
}

void programchange( int programnumber, unsigned long finaltime){
  if(finaltime < timespend){
    program = programnumber;
  }
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////                                                        fonctions sorties pwm ( program et efect )                                            /////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void fadetofix(int startcolor, int endcolor, unsigned long initime, unsigned long finaltime) {
  int i;
  float advance;
  advance = 1000*initime/finaltime;
  if(advance < 1000){
    i = startcolor*(1000 - (advance))/1000 + endcolor*(advance)/1000;
    analogWrite(led_pin_R, i);
    analogWrite(led_pin_G, 0);
    analogWrite(led_pin_B, 0);
    Serial.print(" timespend ");
    Serial.print(advance);
    Serial.print(" i ");
    Serial.println(i);
  }
  if(advance >= 1000){
    analogWrite(led_pin_R, endcolor);
    analogWrite(led_pin_G, 0);
    analogWrite(led_pin_B, 0);
  }
}


unsigned long timmer3;   //a tester aussi en variables globales
int timmerset1;
int onoff;

void strombo(int oncolor[], int offcolor[], unsigned long ontime, unsigned long offtime) {
  nothingtodo = 0;  
  if(timmerset1 == 0){
    timmer3 = millis();
    timmerset1 = 1;
    }
  if(onoff == 1){
    analogWrite(led_pin_R, oncolor[0]);
    analogWrite(led_pin_G, oncolor[1]);
    analogWrite(led_pin_B, oncolor[2]);
    if(millis() > (ontime + timmer3)){
      onoff = 0;
      timmer3 = millis();
    }
  }
  if(onoff == 0){
    analogWrite(led_pin_R, offcolor[0]);
    analogWrite(led_pin_G, offcolor[1]);
    analogWrite(led_pin_B, offcolor[2]);
    if(millis() > offtime + timmer3){
      onoff = 1;
      timmer3 = millis();
    }
  }
}

unsigned long timmer4;   //a tester aussi en variables globales
int timmerset4;
int onoff4;

void backgroundstrombo(int oncolor[], unsigned long ontime, unsigned long offtime) {
  if(timmerset4 == 0){
    timmer4 = millis();
    timmerset4 = 1;
    }
  if(onoff4 == 1){
    nothingtodo = 0;
    analogWrite(led_pin_R, oncolor[0]);
    analogWrite(led_pin_G, oncolor[1]);
    analogWrite(led_pin_B, oncolor[2]);
    if(millis() > (ontime + timmer4)){
      onoff4 = 0;
      timmer4 = millis();
    }
  }
  if(onoff4 == 0){
    nothingtodo = 1;
    if(millis() > offtime + timmer4){
      onoff4 = 1;
      timmer4 = millis();
    }
  }
}

unsigned long timmer5;   //a tester aussi en variables globales
int timmerset5;
int onoff5;
int someflaschactivate;
int someflaschactive;

void someflasch(int oncolor[], unsigned long ontime, unsigned long offtime) {
  if(timmerset5 == 0){
    timmer5 = millis();
    timmerset5 = 1;
    }
  if(onoff5 == 1){
    nothingtodo = 0;
    someflaschactive = 1;
    analogWrite(led_pin_R, oncolor[0]);
    analogWrite(led_pin_G, oncolor[1]);
    analogWrite(led_pin_B, oncolor[2]);
    if(millis() > (ontime + timmer5)){
      onoff5 = 0;
      timmer5 = millis();
    }
  }
  if(onoff5 == 0){
    nothingtodo = 1;
    someflaschactive = 0;
    if(millis() > offtime + timmer5){
      onoff5 = 1;
      timmer5 = millis();
    }
  }
}

unsigned long timmer6;   //a tester aussi en variables globales
int timmerset6;
int onoff6;
int someblackactivate;

void someblack(int oncolor[], unsigned long ontime, unsigned long offtime) {
  if(timmerset6 == 0){
    timmer6 = millis();
    timmerset6 = 1;
    }
  if(onoff6 == 1){
    nothingtodo = 0;
    analogWrite(led_pin_R, oncolor[0]);
    analogWrite(led_pin_G, oncolor[1]);
    analogWrite(led_pin_B, oncolor[2]);
    if(millis() > (ontime + timmer6)){
      onoff6 = 0;
      timmer6 = millis();
    }
  }
  if(onoff6 == 0){
    nothingtodo = 1;
    if(millis() > offtime + timmer6){
      onoff6 = 1;
      timmer6 = millis();
    }
  }
}


unsigned long timmer8;   //a tester aussi en variables globales
int timmerset8;
int status8;
int somemulticolorsactivate;
int multicolorsactive;

void somemulticolors(int color1[], int color2[], int color3[], int color4[], unsigned long time1, unsigned long time2, unsigned long time3, unsigned long time4, unsigned long donothingtime) {
  if(timmerset8 == 0){
    timmer8 = millis();
    timmerset8 = 1;
    }
  if(status8 == 0){
    nothingtodo = 0;
    multicolorsactive = 1;
    analogWrite(led_pin_R, color1[0]);
    analogWrite(led_pin_G, color1[1]);
    analogWrite(led_pin_B, color1[2]);
    if(millis() > (timmer8 + time1)){
      status8 = 1;
      timmer8 = millis();
    }
  }
  if(status8 == 1){
    nothingtodo = 0;
    multicolorsactive = 1;
    analogWrite(led_pin_R, color2[0]);
    analogWrite(led_pin_G, color2[1]);
    analogWrite(led_pin_B, color2[2]);
    if(millis() > timmer8 + time2){
      status8 = 2;
      timmer8 = millis();
    }
  }
  if(status8 == 2){
    nothingtodo =0;
    multicolorsactive = 1;
    analogWrite(led_pin_R, color3[0]);
    analogWrite(led_pin_G, color3[1]);
    analogWrite(led_pin_B, color3[2]);
    if(millis() > timmer8 + time3){
      status8 = 3;
      timmer8 = millis();
    }
  }
  if(status8 == 3){
    nothingtodo =0;
    multicolorsactive = 1;
    analogWrite(led_pin_R, color4[0]);
    analogWrite(led_pin_G, color4[1]);
    analogWrite(led_pin_B, color4[2]);
    if(millis() > timmer8 + time4){
      status8 = 4;
      timmer8 = millis();
    }
  }
  if(status8 == 4){
    nothingtodo =1;
    multicolorsactive = 0;
    if(millis() > timmer8 + donothingtime){
      status8 = 0;
      timmer8 = millis();
    }
  }
}





unsigned long timmer7;   //a tester aussi en variables globales
int timmerset7;
int status7;

void colors3(int color1[], int color2[], int color3[], unsigned long time1, unsigned long time2, unsigned long time3) {
  if(timmerset7 == 0){
    timmer7 = millis();
    timmerset7 = 1;
    }
  if(status7 == 0){
    analogWrite(led_pin_R, color1[0]);
    analogWrite(led_pin_G, color1[1]);
    analogWrite(led_pin_B, color1[2]);
    if(millis() > (timmer7 + time1)){
      status7 = 1;
      timmer7 = millis();
    }
  }
  if(status7 == 1){
    analogWrite(led_pin_R, color2[0]);
    analogWrite(led_pin_G, color2[1]);
    analogWrite(led_pin_B, color2[2]);
    if(millis() > timmer7 + time2){
      status7 = 2;
      timmer7 = millis();
    }
  }
  if(status7 == 2){
    analogWrite(led_pin_R, color3[0]);
    analogWrite(led_pin_G, color3[1]);
    analogWrite(led_pin_B, color3[2]);
    if(millis() > timmer7 + time3){
      status7 = 0;
      timmer7 = millis();
    }
  }
}


  
//fadesirenvariables
int increase;
int timmerset;
unsigned long timmer1;
unsigned long timmer2;

void fadesiren(int startcolor[], int endcolor[], unsigned long finaltime) {
  int r;
  int g;
  int b;
  float advance;
/*
  Serial.print("  advance  ");
    Serial.print(advance);
    Serial.print(" r ");
    Serial.print(r);
    Serial.print(" g ");
    Serial.print(g);
    Serial.print(" b ");
    Serial.println(b);

    Serial.print("  startcolor 1  ");
    Serial.print(startcolor[1]);
    Serial.print("  startcolor 2  ");
    Serial.print(startcolor[1]);
    Serial.print("  startcolor 3  ");
    Serial.print(startcolor[3]);
    */
  if(timmerset == 0){
    timmer1 = millis();
    timmerset = 1;
  }
  timmer2 = millis() - timmer1;
  advance = 1000*timmer2/finaltime;
  if(increase == 1){
    r = startcolor[0]*(1000 - (advance))/1000 + endcolor[0]*(advance)/1000;
    g = startcolor[1]*(1000 - (advance))/1000 + endcolor[1]*(advance)/1000;
    b = startcolor[2]*(1000 - (advance))/1000 + endcolor[2]*(advance)/1000;

    analogWrite(led_pin_R, r);
    analogWrite(led_pin_G, g);
    analogWrite(led_pin_B, b);
    //if(millis() > (finaltime + timmer1)){
    //  increase = 0;
    //  timmerset = 0;
    //} 
    }
    /*
    Serial.print(" increase ");
    Serial.print(increase);
    Serial.print(" timmerset1 ");
    Serial.print(timmerset);
    Serial.print(" i ");
    Serial.print(i);
    Serial.print(" timmer1 ");
    Serial.print(timmer1);
    Serial.print(" timmer2 ");
    Serial.println(timmer2);
    */
    
  if(increase == 0){
    r = endcolor[0]*(1000 - (advance))/1000 + startcolor[0]*(advance)/1000;
    g = endcolor[1]*(1000 - (advance))/1000 + startcolor[1]*(advance)/1000;
    b = endcolor[2]*(1000 - (advance))/1000 + startcolor[2]*(advance)/1000;

    analogWrite(led_pin_R, r);
    analogWrite(led_pin_G, g);
    analogWrite(led_pin_B, b);
  }
  if(timmer2 > (finaltime)){
     timmerset = 0;
     switch(increase){
      case 0:
      increase = 1;
      Serial.print("bonjourlesnoobs");
         delay(200);
      break;

      case 1:
      increase = 0;
      break;
      
     }
     
  }  
  
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////                                                  fonctions modes                                                ///////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void mode0() {
  Serial.print(" someflaschactivate ");
  Serial.print(someflaschactivate);
  Serial.print(" nothingtodo ");
  Serial.print(nothingtodo);
 // Serial.print(" efecton ");
  //Serial.println(efecton);
  if(efecton == 1){
    switch(efect){  //de 144 a 160 (de 0x90 a 0x9F)
      case 144:
        timeonefect(10000);
        strombo(vert, bleu, 10,600); //oncolor, offcolor, ontime, offtime
      break;
      
      case 145:
        timeonefect(7000);
        programchange(131, 6990); //programchange(programnumber, time) durée doit etre inferieure a celle du timeoneffect 
        backgroundstrombo(bleu, 20, 700); //oncolor, ontime, offtime

      break;
      
      case 146:
        timeonefect(7000);
        programchange(131, 10000); //programchange(programnumber, time) durée doit etre inferieure a celle du timeoneffect 
        strombo(blanc, noir, 10,1200);
      break;
      
      case 147:
        strombo(blanc, vert, 10,150);

      break;
      case 148:
        efecton = 0;
      break;
      
      case 149:

      break;
    }
  }
  if((efecton == 0 or nothingtodo == 1) and someflaschactivate == 1){
    someflasch(blanc, 20, 6000);
  }
  if((efecton == 0 or nothingtodo == 1) and somemulticolorsactivate == 1 and someflaschactive == 0){
    somemulticolors(bleu, vert, blanc, rose, 100, 100, 100, 100, 4000);
  }
  if((efecton == 0 or nothingtodo == 1) and someblackactivate == 1 and multicolorsactive == 0 and someflaschactive == 0){
    someblack(noir, 3500, 4000);
  }
 

  
  if((someflaschactivate == 0 and someblackactivate == 0 and somemulticolorsactivate == 0 and efecton == 0) or nothingtodo == 1){
    switch(program){//de 128 a 143 (de 0x80 a 0x8F)
      
      case 128:
        fadesiren(bleu, vert, 5000);
        someflaschactivate = 1;
        someblackactivate = 1;
        somemulticolorsactivate = 1;
      break;
        
      case 129:
        fadesiren(rouge, bleu, 15000);
        someflaschactivate = 0;
        someblackactivate = 1;
        somemulticolorsactivate = 0;

      break;

      case 130:
        colors3(jaune, violet, rose, 1500, 5000, 3000);
        someflaschactivate = 0;
        someblackactivate = 0;
        somemulticolorsactivate =1;

      break;

      case 131:
        analogWrite(led_pin_R, 60);
        analogWrite(led_pin_G, 56);
        analogWrite(led_pin_B, 160);
        someflaschactivate = 0;
       

      break;

      case 132:
        analogWrite(led_pin_R, 60);
        analogWrite(led_pin_G, 206);
        analogWrite(led_pin_B, 10);
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
  
  pinMode(led_pin_R, OUTPUT);
  pinMode(led_pin_G, OUTPUT);
  pinMode(led_pin_B, OUTPUT);
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
