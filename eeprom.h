//EEPROM atminties adresai
#define TEMP              0  //FLOAT 32bit, uzima 4 vietas
#define REZO              4  //FLOAT 32bit, uzima 4 vietas
#define SILDYMAS          8  //CHAR 8bit
#define APSVIETIMAS       10
#define FAN               12
#define FAN_SPEED_MIN     14
#define FAN_SPEED_MAX     16
//#define LCD             18
#define DZIOVYKLE         20
#define OFF_SLD           22
#define ON_SLD            24


void nuskaityti(){
  rtc.eeprom_read(TEMP, &temp);
  rtc.eeprom_read(REZO, &rezoliucija);
  rtc.eeprom_read(SILDYMAS, &sildymas_power);

  rtc.eeprom_read(FAN_SPEED_MIN, &min_fan);
  rtc.eeprom_read(FAN_SPEED_MAX, &max_fan);
  rtc.eeprom_read(FAN, &fan_power);

  rtc.eeprom_read(DZIOVYKLE, &dziovykle);
  rtc.eeprom_read(OFF_SLD, &off_sildymas);
  rtc.eeprom_read(ON_SLD, &on_sildymas);
}

void irasyti_temperatura(){
  rtc.eeprom_write(TEMP, temp);
  rtc.eeprom_write(REZO, rezoliucija);
  rtc.eeprom_write(SILDYMAS, sildymas_power);
  }

void irasyti_ventiliatoriu(){
  rtc.eeprom_write(FAN_SPEED_MIN, min_fan);
  rtc.eeprom_write(FAN_SPEED_MAX, max_fan);
  rtc.eeprom_write(FAN, fan_power);
  }

void irasyti_kiti(){
  rtc.eeprom_write(OFF_SLD, off_sildymas);
  rtc.eeprom_write(ON_SLD, on_sildymas);
  rtc.eeprom_write(DZIOVYKLE, dziovykle);
  //rtc.eeprom_write(FAN, fan_power);
  }



