//EEPROM atminties adresai
#define TMIN              0  //FLOAT 32bit, uzima 4 vietas
#define TMAX              4  //FLOAT 32bit, uzima 4 vietas
#define SILDYMAS          8  //CHAR 8bit
#define APSVIETIMAS       10
#define FAN               12
#define FAN_SPEED_MIN     14
#define FAN_SPEED_MAX     16
#define LCD               18
#define DZIOVYKLE         20


void nuskaityti(){
  rtc.eeprom_read(TMIN, &min_temp);
  rtc.eeprom_read(TMAX, &max_temp);
  rtc.eeprom_read(SILDYMAS, &sildymas_power);

  rtc.eeprom_read(FAN_SPEED_MIN, &min_fan);
  rtc.eeprom_read(FAN_SPEED_MAX, &max_fan);
  rtc.eeprom_read(FAN, &fan_power);

   rtc.eeprom_read(DZIOVYKLE, &dziovykle);
  rtc.eeprom_read(LCD, &lcd_kontrastas);
  
  //rtc.eeprom_read(APSVIETIMAS, &apsvietimas_power);
}

void irasyti_temperatura(){
  rtc.eeprom_write(TMIN, min_temp);
  rtc.eeprom_write(TMAX, max_temp);
  rtc.eeprom_write(SILDYMAS, sildymas_power);
  }

void irasyti_ventiliatoriu(){
  rtc.eeprom_write(FAN_SPEED_MIN, min_fan);
  rtc.eeprom_write(FAN_SPEED_MAX, max_fan);
  rtc.eeprom_write(FAN, fan_power);
  }

void irasyti_kiti(){
  rtc.eeprom_write(LCD, lcd_kontrastas);
  rtc.eeprom_write(DZIOVYKLE, dziovykle);
  //rtc.eeprom_write(FAN, fan_power);
  }



