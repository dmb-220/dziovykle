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

//KINTAMIEJI
unsigned long previousMillis, logMillis, TMillis;
float t, h;
float min_temp = 30.00, max_temp = 50.00;
unsigned char sildymas_power, sildymas;
unsigned char meniu = 0, set_meniu = 0;

#include "rotary_encoder.h";
#include "lcd.h";

// The Interrupt Service Routine for Pin Change Interrupt 1
// This routine will only be called on any signal change on A2 and A3: exactly where we need to check.
ISR(PCINT1_vect) {
  encoder.tick(); // just call tick() to check the state.
}

void setup(void) {
  Wire.begin();
  u8g2.begin(); 
  u8g2.enableUTF8Print();
  // You may have to modify the next 2 lines if using other pins than A2 and A3
  PCICR |= (1 << PCIE1);    // This enables Pin Change Interrupt 1 that covers the Analog input pins or Port C.
  PCMSK1 |= (1 << PCINT10) | (1 << PCINT11);  // This enables the interrupt for pin 2 and 3 of Port C.
  
  encoder.setPosition(100 / 2);
  sht31.begin(0x44); 
}

void loop(void) {
  //nuskaitomas rotary encoder
  rotary_encoder();
  
  //Atliekamas temperaturos nuskaitymas
  unsigned long Temp_Millis = millis();
  if(Temp_Millis - TMillis >= 500){
    TMillis = Temp_Millis;
    //SHT3X daviklis
    t = sht31.readTemperature();
    h = sht31.readHumidity();
  }

  unsigned long LCD_Millis = millis();
  if(LCD_Millis - previousMillis >= 300){
    previousMillis = LCD_Millis;
    //Nuskaitom RTC
    rtc.refresh();
  }
  
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
    
    //APSVIETIMAS
    case 5:
    //apsvietimas_vienas();         
    break;

    //APSVIETIMAS 2
    case 6:
    //apsvietimas_du();
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
}

