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


//KINTAMIEJI
unsigned long previousMillis, logMillis, TMillis;
float t, h;
float min_temp = 30.00, max_temp = 50.00;
int min_fan = 30, max_fan = 50;
int lcd_kontrastas = 80;
unsigned char sildymas_power, sildymas, fan_power, fan, apsvietimas, dziovykle;
unsigned char meniu = 0, set_meniu = 0;
unsigned char heat_off, heat_on, poza, save = 0;

#include "rotary_encoder.h";
#include "eeprom.h";
#include "lcd.h";

// The Interrupt Service Routine for Pin Change Interrupt 1
// This routine will only be called on any signal change on A2 and A3: exactly where we need to check.
ISR(PCINT1_vect) {
  encoder.tick(); // just call tick() to check the state.
}

void setup(void) {
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
  //nuskaitomas rotary encoder
  rotary_encoder();
  //lcd kontrastaS
  //analogWrite(LCD_PVM_PIN, map(lcd_kontrastas, 0, 100, 0, 255));
  
  //Atliekamas temperaturos nuskaitymas
  unsigned long Temp_Millis = millis();
  if(Temp_Millis - TMillis >= 500){
    TMillis = Temp_Millis;
    //SHT3X daviklis
    t = sht31.readTemperature();
    h = sht31.readHumidity();
  }

  //unsigned long LCD_Millis = millis();
  //if(LCD_Millis - previousMillis >= 100){
    //previousMillis = LCD_Millis;
    //Nuskaitom RTC
    rtc.refresh();
  
  //isvedam informacija i LCD
  switch (set_meniu) {
    //PRADINIS LANGAS
    case 1:
    pradinis_langas();
    break;
    
    //MENIU
    case 2:
    nustatymai_meniu();
    break;
    
    //LAIKRODIS
    case 3:
    laiko_datos_nustatymai(); 
    break;
    
    //SILDYMAS
    case 4:
    sildymo_nustatymai();
    break;
    
    //VENTILIATORIUS
    case 5:
    ventiliatoriaus_nustatymai();         
    break;

    //KITA
    case 6:
    kiti_nustatymai();
    break;

    //ISSAUGOTI
    case 10:
    issaugoti();
    break;
            
    //BOOT
    default:
    u8g2.firstPage();
    do {
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
//}

//LOGIKA SILDYMUI IR APSVIETIMUI
  unsigned long LOGIKA_Millis = millis();
  if(LOGIKA_Millis - logMillis >= 1000){
    logMillis = LOGIKA_Millis;
    sild_sk++;
    
    //paziurim ar isvis ijungta dziovykle
    if(dziovykle){
      //Cia reik padaryt kad kai ijungi ar  isjungi kol nebus pakeitimu kad nesuveiktu sitas kodas, nesjau ivykdytas ir nekartoti
      if(fan_power){
        digitalWrite(FAN_RELAY, LOW);
        }else{
          digitalWrite(FAN_RELAY, HIGH);}

      if(apsvietimas){
        digitalWrite(LIGHT_RELAY, LOW);
        }else{
          digitalWrite(LIGHT_RELAY, HIGH);}
          
      //SILDYMO VALDYMAS
      //tikrinam kai sildytuvas isjungtas ar vis dar syla nuo sildytuvo ar jau atveso
       if(sild_sk > 3){
        sild_sk = 0;
        //1: kyla, 0: krenta
        if(t > sena_temp){poza = 1;}
        if(t < sena_temp){poza = 0;}
        sena_temp = t;
       }
  
       //jei tempertura nukrepta zemiau MIN
      if(min_temp >= t && tempa == 1){
          //ijungiam rele vel sildyti
          digitalWrite(HEATER_RELAY, LOW);
          sildymas = 1; tempa = 0;
        }
  
        //kai pradedam sildyti yra ijungiamas sildytuvas, kas kazkiek laiko isjungiamas ir ziurima kiek kilti reik, jei truksta vel pasildo
        if(min_temp >= t && tempa == 0){
          heat_off++; heat_on++;
          //dar POZA itraukti jei yra kilima ar kritimas nebelaukti kol laikas baigsis
  
          //
          if(heat_off > 5 && sildymas || poza){
            //reik isjungti ir ziuret ar nepasieke reikiamos temperaturos
            digitalWrite(HEATER_RELAY, HIGH);
            sildymas = 0; heat_off = 0; heat_on = 0;
          }
            
          //patikrinam ar temperatura krenta, ir pasiziuirim kelinta karta ijungta, sildymas, nes 3 kartu uztenka kad spetu vel isilti plokste
          if(!poza){
            if(heat_on > 20 && !sildymas){
              //reik dar biski sildyti, nes neuzteko silumos is elemento pasildyti
              digitalWrite(HEATER_RELAY, LOW);
              sildymas = 1; heat_on = 0; heat_off = 0;
            }
          }else{heat_on = 0; heat_off = 0;}
          
        }
        
      //jei temperatura pakyla virs MAX 
      if(max_temp <= t && tempa == 0){
        //isjungiam rele
        digitalWrite(HEATER_RELAY, HIGH);
        sildymas = 0; tempa = 1; sildymas_sk = 1;
        //Riba pasiekata, gali atlikti jau darbus
        if(!first_time){first_time = 1;}
      }

      
      if(t >= min_temp && t <= max_temp){
        //cia irgi reik pagal laika isjungti ir ijungti jei nepasiekia to ko reik
        heat_off++; heat_on++;          
        //cia viska reiketu sujungti i viena tikrinima, kur yra virsuje.
        //gal ir sitas variantas veiks
        if(heat_off > 5 && sildymas  || poza){
            //reik isjungti ir ziuret ar nepasieke reikiamos temperaturos
            digitalWrite(HEATER_RELAY, HIGH);
            sildymas = 0; heat_off = 0; heat_on = 0; tempa = 0;
          }
  
          if(!poza){
            if(heat_on > 10 && !sildymas){
              //reik dar biski sildyti, nes neuzteko silumos is elemento pasildyti
              digitalWrite(HEATER_RELAY, LOW);
              sildymas = 1; heat_on = 0; heat_off = 0; tempa = 0;
            }
          }else{
            heat_on = 0; heat_off = 0;}
          
      } 
    }
    
  }
}

