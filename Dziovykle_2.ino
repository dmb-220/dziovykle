//###########################################################################################################################
//### DZIOVYKLE
//### Autorius: Andrius Norkus (DMB-220)
//### 2018-08-25 Pakeista kad nustatyt viena temperaturos reiksme, padarytas rezoliucijos pasirinkimas
//### 2018-08-26 Padarytas elektros skaiciavimas kiek suvartoje apytiksliai elektros dziovykle
//### 2018-08-26 padarytas antras LCD meniu langas, elektros parodymai, laikmatis
//###########################################################################################################################
#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>
#include <Wire.h>
#include "uRTCLib.h"
#include <RotaryEncoder.h>
#include "Adafruit_SHT31.h"

U8G2_ST7920_128X64_1_HW_SPI u8g2(U8G2_R2, 10, 8);
uRTCLib rtc(0x68, 0x57);
Adafruit_SHT31 sht31 = Adafruit_SHT31();
RotaryEncoder encoder(A2, A3);

//#define NO_RELAY                  A1 //220
#define HEATER_RELAY              2 //220
#define FAN_RELAY                 A0 //12
#define LIGHT_RELAY               3 //220

#define FAN_PVM                   6
#define FAN_SENSE                 7  

#define LCD_PVM_PIN               5

#define TENAS_GALIA               760

//KINTAMIEJI
unsigned long previousMillis, logMillis, TMillis;
float t, h;
float temp;
int min_fan = 30, max_fan = 50;
//int lcd_kontrastas = 80;
unsigned char sildymas_power, sildymas=0, fan_power, fan, fan_speed, fan_reiksme, apsvietimas, dziovykle=0;
unsigned char meniu = 0, set_meniu = 0;
unsigned char heat, poza, save = 0, rezoliucija, kadras = 0;
int off_sildymas = 30, on_sildymas = 20;
//elektra
float laikas, kwh = 0;
float el_sk = 0, el_min = 0, el_val = 0, el_day = 0; 

#include "rotary_encoder.h";
#include "eeprom.h";
#include "lcd.h";

// The Interrupt Service Routine for Pin Change Interrupt 1
// This routine will only be called on any signal change on A2 and A3: exactly where we need to check.
ISR(PCINT1_vect) {
  encoder.tick(); // just call tick() to check the state.
}

void setup(void) {
  //Serial.begin(9600);
  pinMode(HEATER_RELAY, OUTPUT);
  pinMode(FAN_RELAY, OUTPUT);
  pinMode(LIGHT_RELAY, OUTPUT);
  digitalWrite(HEATER_RELAY, HIGH);
  digitalWrite(FAN_RELAY, HIGH);
  digitalWrite(LIGHT_RELAY, HIGH);
  
  Wire.begin();
  u8g2.begin(); 
  u8g2.enableUTF8Print();
  // You may have to modify the next 2 lines if using other pins than A2 and A3
  PCICR |= (1 << PCIE1);    // This enables Pin Change Interrupt 1 that covers the Analog input pins or Port C.
  PCMSK1 |= (1 << PCINT10) | (1 << PCINT11);  // This enables the interrupt for pin 2 and 3 of Port C.
  
  encoder.setPosition(ROTARYMAX / 2);
  sht31.begin(0x44);
  //nuskaitom pradinius nustatymus is EEPROM
  nuskaityti();
}

void loop(void) {
  //Atliekamas temperaturos nuskaitymas
  unsigned long Temp_Millis = millis();
  if(Temp_Millis - TMillis >= 500){
    TMillis = Temp_Millis;
    //SHT3X daviklis
    t = sht31.readTemperature();
    h = sht31.readHumidity();
  }


  //Nuskaitom RTC
  rtc.refresh();
  
  //isvedam informacija i LCD
  switch (set_meniu) {
    //PRADINIS LANGAS
    case 1: pradinis_langas(); break;    
    //MENIU
    case 2: nustatymai_meniu(); break;  
    //LAIKRODIS
    case 3: laiko_datos_nustatymai();  break;  
    //SILDYMAS
    case 4: sildymo_nustatymai(); break;
    //VENTILIATORIUS
    case 5: ventiliatoriaus_nustatymai(); break;
    //KITA
    case 6: kiti_nustatymai(); break;

    //ELEKTROS PARODYMAI
    //case 7: elektros_parodymai(); break;
    //LAIKMATIS
    //case 8: laikmatis(); break;

    //ISSAUGOTI
    //case 10: issaugoti(); break;
            
    //BOOT
    default:
    u8g2.firstPage();
    do {
      //nuskaitomas rotary encoder
      rotary_encoder();
      u8g2.setFont(u8g2_font_7x13B_mf);
      u8g2.drawStr(30, 15,"DZIOVYKLE");
      
      u8g2.setFont(u8g2_font_profont12_tr);
      u8g2.drawStr(15, 30,"Vaisiai, darzoves");
      u8g2.drawStr(35, 40,"ir grybai");
  
      //myktukai
      if(button!=btn){
         if(btn == 1){set_meniu=1; btn = 0;}
         }  
       } while ( u8g2.nextPage() );
    break;
    }

/////////////////////////////////////////////////LOGIKA SILDYMUI///////////////////////////////////
  unsigned long LOGIKA_Millis = millis();
  if(LOGIKA_Millis - logMillis >= 1000){
    logMillis = LOGIKA_Millis;
    
    //paziurim ar isvis ijungta dziovykle
    if(dziovykle){
      //skaiciuojam apytiksliai elektros sanaudas
      elektra();
      
      //Ijungiamas ventiliatoriau maitinimas
      if(fan_power){
        digitalWrite(FAN_RELAY, LOW);}else{
          digitalWrite(FAN_RELAY, HIGH);}
      //Apsvietimo ijungimas
      if(apsvietimas){digitalWrite(LIGHT_RELAY, LOW);}else{
          digitalWrite(LIGHT_RELAY, HIGH);}
          
      //Ventiliatoriaus reguliavimas
      fan_speed = map(fan_reiksme, 0, 100, 0, 255);
      analogWrite(FAN_PVM, fan_speed);
          
      //Ziurim ar temperatura kyla ar krempta
      sild_sk++;
      //kas 10 s. tikrinam yra kilimas ar kritimas
      if(sild_sk > 10){
        sild_sk = 0;
        //1: kyla, 0: krenta
        //pirma karta imituojam kritima
        if(t > sena_temp){poza = 1;}
        if(t < sena_temp){poza = 0;}
        sena_temp = t;
      }

      //paskaiciuojam MIN ir MAX reiksmes, 1 laipsnio svyravimas
      //nustatyti pagal rezoliucija kiek gali svyruoti, jei kokiem kiausiniams sugalvociau perinti
      float min_temp = temp - 0.5;
      float max_temp = temp + 0.5;
      
      //skaiciuojam
      heat++;
      //pasiskaiciuojam koficienta
      //jei reikia galim +1 ar +2 greitam pasildymui
      float skirtumas = (max_temp - t)/10 +1;
      if(skirtumas < 1){skirtumas = 1;}
      
      //jei tempertura nukrepta zemiau MIN arba yra dar nepakilusi      
      //kas kazkiek laiko isjungiamas ir ziurima kiek kilti reik, jei truksta vel pasildo
      if(min_temp >= t){
        if(heat > (off_sildymas*skirtumas) && sildymas && poza){
          //reik isjungti ir ziuret ar nepasieke reikiamos temperaturos
          digitalWrite(HEATER_RELAY, HIGH);
          fan_reiksme = max_fan; sildymas = 0; heat = 0;
        }           
        //patikrinam ar temperatura krenta
        //if(!poza){
          if(heat > (on_sildymas*skirtumas) && !sildymas && !poza){
            //reik dar biski sildyti, nes neuzteko silumos is elemento pasildyti
            digitalWrite(HEATER_RELAY, LOW);
            fan_reiksme = min_fan; sildymas = 1; heat = 0;
          }
        //}else{heat_on = 0; heat_off = 0;}        
      }

      ////cia reik padaryti kad nusileistu iki nuo max bent jau 1 laipsniu, nes krita, riba ir vel sildo
      if(t >= min_temp && t <= max_temp){
        //cia irgi reik pagal laika isjungti ir ijungti jei nepasiekia to ko reik                
        if(heat > off_sildymas && sildymas  && poza){
          //reik isjungti ir ziuret ar nepasieke reikiamos temperaturos
          digitalWrite(HEATER_RELAY, HIGH);
          fan_reiksme = max_fan; sildymas = 0; heat = 0;
        }  
        //if(!poza){
          if(heat > on_sildymas && !sildymas && !poza){
            //reik dar biski sildyti, nes neuzteko silumos is elemento pasildyti
            digitalWrite(HEATER_RELAY, LOW);
            fan_reiksme = min_fan; sildymas = 1; heat = 0;
          }
        //}else{heat_on = 0; heat_off = 0;}         
      }

      //jei temperatura pakyla virs MAX 
      if(max_temp <= t){
        //isjungiam rele
        digitalWrite(HEATER_RELAY, HIGH);
        fan_reiksme = max_fan; sildymas = 0; heat = 0;
        //Riba pasiekata, gali atlikti jau darbus
        if(!first_time){first_time = 1;}
      }
            
    }  
  }
}

