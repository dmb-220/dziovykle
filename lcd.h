#define LCD_PVM_PIN               5

const char* meniu_list[5] = {
  "Laikrodis","Temperatura","Ventiliatorius", "Kiti", "Atgal",
  };
const char* savaites_dienos[7] = {
  "Pirmadienis", "Antradienis", "Treciadienis", "Ketvirtadienis", "Penktadienis", "Sestadienis", "Sekmadienis"
  };

unsigned char tempa = 1;
int sildymas_sk = 1, sild_sk = 0;
float sena_temp;
  
unsigned char eile = 0, vieta = 1, pozicija;

//laikrodzio kintamieji
unsigned char hours, minute, second;
byte set_valandos = 12, set_minutes = 30, set_sekundes = 0, set_menesis, set_diena, set_savaite;
//nes 2018 i char netelpa
int set_metai;
unsigned char first_time = 0;

void set_laikrodis(){
  set_sekundes = rtc.second();
  set_minutes = rtc.minute();
  set_valandos = rtc.hour();
  set_diena = rtc.day();
  set_menesis = rtc.month();
  set_savaite = rtc.dayOfWeek();
  set_metai = rtc.year()+2000;
}

void setRTCTime(){
  rtc.set(set_sekundes, set_minutes, set_valandos, set_savaite, set_diena, set_menesis, set_metai-2000);
}

void pradinis_langas(){
  u8g2.firstPage();
  do {
    //nuskaitomas rotary encoder
    rotary_encoder();
    
    u8g2.setFont(u8g2_font_t0_11_mn);
    u8g2.drawFrame(0,0,128,14);
    
    if(rtc.hour() >= 10){
      u8g2.setCursor(72, 11);
      u8g2.print(rtc.hour());}else{
        u8g2.setCursor(72, 11);
        u8g2.print("0");
        
        u8g2.setCursor(78, 11);
        u8g2.print(rtc.hour());
      }
      
    u8g2.setCursor(85, 10);
    u8g2.print(":");
    if(rtc.minute() >= 11){
      u8g2.setCursor(92, 11);
      u8g2.print(rtc.minute());}else{
        u8g2.setCursor(92, 11);
        u8g2.print("0");
        
        u8g2.setCursor(98, 11);
        u8g2.print(rtc.minute());
      }
    
    u8g2.setCursor(105, 10);
    u8g2.print(":");
    if(rtc.second() >= 11){
      u8g2.setCursor(112, 11);
      u8g2.print(rtc.second());}else{
        u8g2.setCursor(112, 11);
        u8g2.print("0");
        
        u8g2.setCursor(118, 11);
        u8g2.print(rtc.second());
      }
      
       u8g2.setFont(u8g2_font_7x13B_mf);
       u8g2.setCursor(70, 45);
       u8g2.print(t);
       u8g2.print(" °C");
       u8g2.setCursor(70, 60);
       u8g2.print(h);
       u8g2.print(" %");

       //
       u8g2.setFont(u8g2_font_profont12_tr);
       u8g2.drawFrame(0,15,51,13);
       if(dziovykle){
        u8g2.drawStr(2, 25, "Ijungta");}else{
          u8g2.drawStr(2, 25, "Isjungta");}

       u8g2.drawFrame(0,28,51,12);
       u8g2.drawStr(2, 38, "APS:");
       if(apsvietimas){u8g2.drawStr(30, 38, " ON");
       }else{u8g2.drawStr(30, 38, "OFF");}

       u8g2.drawFrame(0,40,51,12);
       u8g2.drawStr(2, 50, "FAN:");
       if(fan_power){u8g2.drawStr(30, 50, " ON");
       }else{u8g2.drawStr(30, 50, "OFF");}
       

       u8g2.drawFrame(0,52,51,12);
       u8g2.drawStr(2, 62, "SLD:");
       if(sildymas){u8g2.drawStr(30, 62, " ON");
       }else{u8g2.drawStr(30, 62, "OFF");}
       

       if(button!=btn){
          if(btn == 2){if(dziovykle){apsvietimas = !apsvietimas;} btn = 0;}
          if(btn == 3){if(dziovykle){apsvietimas = !apsvietimas;} btn = 0;}
          if(btn == 1){set_meniu=2; btn = 0;}
       }
    
  } while ( u8g2.nextPage() );
}

void nustatymai_meniu(){
  u8g2.firstPage();
  do {
     //nuskaitomas rotary encoder
    rotary_encoder();
    
    u8g2.setFont(u8g2_font_profont12_tr);
    u8g2.drawStr(30, 9,"NUSTATYMAI");
    u8g2.drawLine(10, 11, 117, 11);
    
    for (int i=1; i <= 5; i++){
      if(vieta == i){
        u8g2.drawStr(0, (10*i)+12,"[]");
      }
      u8g2.drawStr(15, (10*i)+12,meniu_list[i-1]);
      }
      
    if(button!=btn){
      //laikrodis
      if(btn == 1 && vieta == 1){set_laikrodis(); set_meniu=3; btn = 0;}
      //Sildymas
      if(btn == 1 && vieta == 2){set_meniu = 4; vieta = 1; btn = 0;}
      //ventiliatorius
      if(btn == 1 && vieta == 3){set_meniu = 5; vieta = 1; btn = 0;}
      //kiti
      if(btn == 1 && vieta == 4){set_meniu = 6; vieta = 1; btn = 0;}
      //Atgal
      if(btn == 1 && vieta == 5){set_meniu = 1; vieta = 1; btn = 0;}
      
       //if(btn == 1 && vieta == 3){set_meniu=5; vieta = 1; btn = 0;}
       //if(btn == 3){set_meniu=1; vieta = 1; btn = 0;}
       //Meniu pasirinkimas
       if(btn == 2){if(vieta < 6){vieta++;} if(vieta == 6){vieta = 1;} btn = 0;}
       if(btn == 3){if(vieta > 0){vieta--;} if(vieta == 0){vieta = 5;}btn = 0;}
       } 
       } while ( u8g2.nextPage() );
}

void issaugoti(){
    u8g2.firstPage();
  do {
     //nuskaitomas rotary encoder
    rotary_encoder();
    u8g2.setFont(u8g2_font_7x13B_mf);
    u8g2.drawFrame(0,20,128,20);
    u8g2.drawStr(20, 35,"ISSAUGOTA !");
    
    if(button!=btn){
      //Fiksuojam paspaudima 
      if(btn == 1/* && vieta != 4*/){set_meniu = 2; btn = 0;}
       } 
     } while ( u8g2.nextPage() );
  }

void laiko_datos_nustatymai(){
  u8g2.firstPage();
  do {
     //nuskaitomas rotary encoder
    rotary_encoder();
    u8g2.setFont(u8g2_font_profont12_tr);
    u8g2.drawStr(25, 9,"LAIKAS IR DATA");
    u8g2.drawLine(10, 11, 117, 11);
   char tarpas = 13;
   
    if(vieta){
      if(!pozicija){
        u8g2.drawStr(0, (vieta*13)+11,"[]");
      }else{
        u8g2.drawStr(0, (vieta*13)+11,"[>]");
      }
      }
    if(pozicija){
      //u8g2.drawLine(sb[eile-1],sa[vieta-1]-2,sb[eile-1]+12,sa[vieta-1]-2);
      //u8g2.drawLine(sb[eile-1],sa[vieta-1]+8,sb[eile-1]+12,sa[vieta-1]+8);
    }
    
    //LAIKAS
    u8g2.setFont(u8g2_font_profont12_tr);
    if(set_valandos >= 10){
      u8g2.setCursor(45, 24); u8g2.print(set_valandos);
      }else{
        u8g2.setCursor(45, 24); u8g2.print("0");        
        u8g2.setCursor(51, 24); u8g2.print(set_valandos);
      }
      
    u8g2.setCursor(58, 23); u8g2.print(":");
    if(set_minutes >= 10){
      u8g2.setCursor(65, 24); u8g2.print(set_minutes);
      }else{
        u8g2.setCursor(65, 24); u8g2.print("0");       
        u8g2.setCursor(71, 24); u8g2.print(set_minutes);
      }
    
    u8g2.setCursor(78, 23); u8g2.print(":");
    if(set_sekundes >= 10){
      u8g2.setCursor(85, 24); u8g2.print(set_sekundes);
      }else{
        u8g2.setCursor(85, 24); u8g2.print("0");        
        u8g2.setCursor(91, 24); u8g2.print(set_sekundes);
      }
  
    //DATA
    u8g2.setCursor(45, 37); u8g2.print(set_metai);   
    u8g2.setCursor(71, 37); u8g2.print("-");
    
    if(set_menesis >= 10){
      u8g2.setCursor(77, 37); u8g2.print(set_menesis);
      }else{
        u8g2.setCursor(77, 37); u8g2.print("0");      
        u8g2.setCursor(82, 37); u8g2.print(set_menesis);
    }
    
    u8g2.setCursor(91 , 37); u8g2.print("-");
    
    if(set_diena >= 10){
      u8g2.setCursor(98, 37); u8g2.print(set_diena);
      }else{
        u8g2.setCursor(98, 37); u8g2.print("0");        
        u8g2.setCursor(103, 37); u8g2.print(set_diena);
      }
      
     u8g2.drawStr(45, 50, savaites_dienos[3]);
     u8g2.setCursor(25, 50); u8g2.print(set_savaite);
     
     u8g2.drawStr(45, 63, "ATGAL");
    
    if(button!=btn){
      //Fiksuojam paspaudima 
      if(btn == 1 && vieta != 4){pozicija = !pozicija; btn = 0;}
      //Atgal
      if(btn == 1 && vieta == 4){set_meniu = 2; vieta = 1; btn = 0;}
       //Meniu pasirinkimas
       if(!pozicija){
         if(btn == 2){if(vieta < 5){vieta++;} if(vieta == 5){vieta = 1;} btn = 0;}
         if(btn == 3){if(vieta > 0){vieta--;} if(vieta == 0){vieta = 4;}btn = 0;}
       }
       } 
     } while ( u8g2.nextPage() );
}

void sildymo_nustatymai(){
    u8g2.firstPage();
  do {
     //nuskaitomas rotary encoder
    rotary_encoder();
    u8g2.setFont(u8g2_font_profont12_tr);
    u8g2.drawStr(35, 9,"SILDYMAS");
    u8g2.drawLine(10, 11, 117, 11);
      
    if(vieta){
      if(!pozicija){
        u8g2.drawStr(0, (vieta*13)+11,"[]");
      }else{
        u8g2.drawStr(0, (vieta*13)+11,"[>]");
      }
    }
    u8g2.setCursor(45, 24);
    u8g2.print("POWER: ");
    if(sildymas_power){
      u8g2.print(" ON");}else{
        u8g2.print("OFF");}
    u8g2.setCursor(45, 37);
    u8g2.print("MIN: ");
    u8g2.print(min_temp);
    u8g2.print(" °C");
    u8g2.setCursor(45, 50);
    u8g2.print("MAX: ");
    u8g2.print(max_temp);
    u8g2.print(" °C");

    if(!save){
    u8g2.drawBox(28, 54, 33,10                                                                                     );
    u8g2.setColorIndex(0);
    u8g2.drawStr(30, 63, "ATGAL");
    u8g2.setColorIndex(1);
    u8g2.drawStr(70, 63, "ISSAUGOTI");
    }else{
      u8g2.drawBox(68, 54, 57,10                                                                                     );
      u8g2.setColorIndex(0);
      u8g2.drawStr(70, 63, "ISSAUGOTI");
      u8g2.setColorIndex(1);
      u8g2.drawStr(30, 63, "ATGAL");
      }
    
    if(button!=btn){
       //Meniu pasirinkimas
       if(!pozicija){
         if(btn == 2){if(vieta < 5){vieta++;} if(vieta == 5){vieta = 1;} btn = 0;}
         if(btn == 3){if(vieta > 0){vieta--;} if(vieta == 0){vieta = 4;} btn = 0;}
       }
       if(pozicija){
        if(vieta == 1){
          if(btn == 2 || btn == 3){sildymas_power = !sildymas_power; btn = 0;}}
        if(vieta == 2){
          if(btn == 2){min_temp++; btn = 0;}
          if(btn == 3){min_temp--; btn = 0;}
          }
        if(vieta == 3){
          if(btn == 2){max_temp++; btn = 0;}
          if(btn == 3){max_temp--; btn = 0;}
          }
          if(vieta == 4){
          if(btn == 2){save = !save; btn = 0;}
          if(btn == 3){save = !save; btn = 0;}

          //padaryti kad nukreiptu i puslapi IRASYTA ar pan
          if(btn == 1){
            if(save){irasyti_temperatura(); set_meniu = 10; vieta = 2; pozicija = 0; btn = 0;
            }else{
              set_meniu = 2; vieta = 2; pozicija = 0; btn = 0;}
            }
          }
        }
      //Fiksuojam paspaudima 
      if(btn == 1/* && vieta != 4*/){pozicija = !pozicija; btn = 0;}
      
       } 
     } while ( u8g2.nextPage() );
  }

void ventiliatoriaus_nustatymai(){
    u8g2.firstPage();
  do {
     //nuskaitomas rotary encoder
    rotary_encoder();
    u8g2.setFont(u8g2_font_profont12_tr);
    u8g2.drawStr(25, 9,"VENTILIATORIUS");
    u8g2.drawLine(10, 11, 117, 11);
      
    if(vieta){
      if(!pozicija){
        u8g2.drawStr(0, (vieta*13)+11,"[]");
      }else{
        u8g2.drawStr(0, (vieta*13)+11,"[>]");
      }
      }

   //min_fan = 40;
   //max_fan = 60;
    u8g2.setCursor(45, 24);
    u8g2.print("POWER: ");
    if(fan_power){
      u8g2.print(" ON");}else{
        u8g2.print("OFF");}
    u8g2.setCursor(45, 37);
    u8g2.print("MIN: ");
    u8g2.print(min_fan);
    u8g2.print(" %");
    u8g2.setCursor(45, 50);
    u8g2.print("MAX: ");
    u8g2.print(max_fan);
    u8g2.print(" %");
     
     if(!save){
      u8g2.drawBox(28, 54, 33,10                                                                                     );
      u8g2.setColorIndex(0);
      u8g2.drawStr(30, 63, "ATGAL");
      u8g2.setColorIndex(1);
      u8g2.drawStr(70, 63, "ISSAUGOTI");
      }else{
        u8g2.drawBox(68, 54, 57,10                                                                                     );
        u8g2.setColorIndex(0);
        u8g2.drawStr(70, 63, "ISSAUGOTI");
        u8g2.setColorIndex(1);
        u8g2.drawStr(30, 63, "ATGAL");
        }
    
    if(button!=btn){
       //Meniu pasirinkimas
       if(!pozicija){
         if(btn == 2){if(vieta < 5){vieta++;} if(vieta == 5){vieta = 1;} btn = 0;}
         if(btn == 3){if(vieta > 0){vieta--;} if(vieta == 0){vieta = 4;} btn = 0;}
       }
       if(pozicija){
        if(vieta == 1){
          if(btn == 2 || btn == 3){fan_power = !fan_power; btn = 0;}}
        if(vieta == 2){
          if(btn == 2){min_fan++; btn = 0;}
          if(btn == 3){min_fan--; btn = 0;}
          }
        if(vieta == 3){
          if(btn == 2){max_fan++; btn = 0;}
          if(btn == 3){max_fan--; btn = 0;}
          }
       if(vieta == 4){
          if(btn == 2){save = !save; btn = 0;}
          if(btn == 3){save = !save; btn = 0;}

          //padaryti kad nukreiptu i puslapi IRASYTA ar pan
          if(btn == 1){
            if(save){irasyti_ventiliatoriu(); set_meniu = 10; vieta = 3; pozicija = 0; btn = 0;
            }else{
              set_meniu = 2; vieta = 3; pozicija = 0; btn = 0;}
            }
          }
        }
      //Fiksuojam paspaudima 
      if(btn == 1/* && vieta != 4*/){pozicija = !pozicija; btn = 0;}
       } 
     } while ( u8g2.nextPage() );
  }

void kiti_nustatymai(){
    u8g2.firstPage();
  do {
     //nuskaitomas rotary encoder
    rotary_encoder();
    u8g2.setFont(u8g2_font_profont12_tr);
    u8g2.drawStr(45, 9,"KITI");
    u8g2.drawLine(10, 11, 117, 11);
      
    if(vieta){
      if(!pozicija){
        u8g2.drawStr(0, (vieta*13)+11,"[]");
      }else{
        u8g2.drawStr(0, (vieta*13)+11,"[>]");
      }
      }
    //lcd_kontrastas = 70;
    u8g2.setCursor(45, 24);
    u8g2.print("POWER: ");
    if(dziovykle){
      u8g2.print(" ON");}else{
        u8g2.print("OFF");}
    //u8g2.setCursor(45, 37);
    //u8g2.print("MIN: ");
    //u8g2.print(min_fan);
    //u8g2.print(" %");
    u8g2.setCursor(45, 50);
    u8g2.print("LCD: ");
    u8g2.print(lcd_kontrastas);
    u8g2.print(" %");
    
    if(!save){
      u8g2.drawBox(28, 54, 33,10                                                                                     );
      u8g2.setColorIndex(0);
      u8g2.drawStr(30, 63, "ATGAL");
      u8g2.setColorIndex(1);
      u8g2.drawStr(70, 63, "ISSAUGOTI");
      }else{
        u8g2.drawBox(68, 54, 57,10                                                                                     );
        u8g2.setColorIndex(0);
        u8g2.drawStr(70, 63, "ISSAUGOTI");
        u8g2.setColorIndex(1);
        u8g2.drawStr(30, 63, "ATGAL");
        }
    
    if(button!=btn){
       //Meniu pasirinkimas
       if(!pozicija){
         if(btn == 2){if(vieta < 5){vieta++;} if(vieta == 5){vieta = 1;} btn = 0;}
         if(btn == 3){if(vieta > 0){vieta--;} if(vieta == 0){vieta = 4;} btn = 0;}
       }
       if(pozicija){
        if(vieta == 1){
          if(btn == 2 || btn == 3){dziovykle = !dziovykle; btn = 0;}}
          
        /*if(vieta == 2){
          if(btn == 2){min_fan++; btn = 0;}
          if(btn == 3){min_fan--; btn = 0;}
          }*/
        if(vieta == 3){
          if(btn == 2){lcd_kontrastas++; btn = 0;}
          if(btn == 3){lcd_kontrastas--; btn = 0;}
          }
        if(vieta == 4){
          if(btn == 2){save = !save; btn = 0;}
          if(btn == 3){save = !save; btn = 0;}

          //padaryti kad nukreiptu i puslapi IRASYTA ar pan
          if(btn == 1){
            if(save){irasyti_kiti(); set_meniu = 10; vieta = 4; pozicija = 0; btn = 0;
            }else{
              set_meniu = 2; vieta = 4; pozicija = 0; btn = 0;}
            }
          }
        }
      //Fiksuojam paspaudima 
      if(btn == 1/* && vieta != 4*/){pozicija = !pozicija; btn = 0;}
       } 
     } while ( u8g2.nextPage() );
  }
