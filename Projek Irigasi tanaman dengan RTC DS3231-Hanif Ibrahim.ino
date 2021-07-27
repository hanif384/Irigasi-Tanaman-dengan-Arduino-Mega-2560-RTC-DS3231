#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <EEPROM.h>
#include "RTClib.h"
#include "DHT.h"


#define DHTTYPE DHT11

//LCD 20x4
LiquidCrystal_I2C lcd(0x27, 20,4);

//rtcds3231
RTC_DS3231 rtc;
char daysOfTheWeek[7][12] ={"Minggu", "Senin", "Selasa", "Rabu", "Kamis", "Jumat", "Sabtu",};
//#define L_On  0
//#define L_Off 1
//#define lampu_1 11
//#define lampu_2 12

//button Analog
#define btnRight 1
#define btnUp    2
#define btnDown  3
#define btnLeft  4
#define btnSelect 5
#define btnNone   6

int siram_st;
int siram_mnt;
int EPsiram_st;
int EPsiram_mnt;
int waktu_siram;

//
int Pompa =2;


//pompa A-B(nutrisi), Pompa C-D(pH down- pH up), Pompa 1 sirkulasi - Pompa 2 netralisir PPM
//int nutA        = 3;
//int nutB        = 4;
//int nutC        = 5;
//int nutD        = 6;
//int pompa_1     = 2;

//inisialisasi pompa dalam millis
//int nutstateA     = LOW;
//int nutstateB     = LOW;
//int nutstateC     = LOW;
//int nutstateD     = LOW;
//int pompastate2   = LOW;

//unsigned long previousMillisPPM = 0;
//unsigned long previousMillisPH  = 0;

//mengatur berapa lama nyala dan mati (nutA, nutB, nutC, nutD, Pompa_2)
//long ontimePPM            = 1000;
//long offtimePPM           = 30000;
//long ontimenetralPPM      = 1500;
//long offtimenetralPPM     = 10000;
//long ontimePH             = 1000;
//long offtimePH            = 30000;
unsigned long mantap;

//icon cokkk
byte customChar[] = {
  B00000,
  B00100,
  B01110,
  B01110,
  B11101,
  B11101,
  B11111,
  B01110
};
byte tempe[] = {
  B11111,
  B01010,
  B01010,
  B01110,
  B01110,
  B01110,
  B01110,
  B00100
};

byte arah_menu1[] ={  
  B00000,
  B00100,
  B00110,
  B11111,
  B11111,
  B00110,
  B00100,
  B00000
  };
byte arah_menu2[] = {
  B00000,
  B00100,
  B01100,
  B11111,
  B11111,
  B01100,
  B00100,
  B00000
};
byte matahari[] = {
  B00100,
  B10001,
  B01110,
  B01110,
  B01110,
  B10001,
  B00100,
  B00000
};
//6
byte jam[] = {
  B00000,
  B01110,
  B10101,
  B10101,
  B10111,
  B10001,
  B10101,
  B01110
};
//7
byte cai[] = {
  B01001,
  B10010,
  B00000,
  B01001,
  B10010,
  B00000,
  B11111,
  B01110
};
//8
byte ember[] = {
  B00000,
  B11111,
  B11111,
  B10001,
  B11111,
  B11111,
  B11101,
  B01110
};
//9
byte gunung[] = {
  B00000,
  B11011,
  B01110,
  B00000,
  B00100,
  B01110,
  B11111,
  B11111
};
//10
byte angin[] = {
  B00010,
  B00001,
  B11110,
  B00000,
  B11110,
  B00001,
  B00010,
  B00000
};
//11
byte tombol[] = {
  B00011,
  B00111,
  B01111,
  B00110,
  B00000,
  B11111,
  B10101,
  B01110
};
//pengaturan LCD
int adc_key_in  = 0;
int lcd_key     = 0;
int button      = 0;
int Screen      = 0;
int layar_Awal  = 1;
int layar_Akhir = 5;
int Reading     = 0;

//inisialisasi Variabel DHT11 
int suhu        = 3;
int s           = 0;
int r           = 0;
int minsuhu     = 0;
int maxsuhu     = 0;

//dht
DHT dht(suhu, DHTTYPE);


void setup() {
  Serial.begin(115200);
  lcd.begin();
  lcd.backlight();
  dht.begin();
  rtc.begin();
  Wire.begin();
  //
  //
  booting();

  

}

void loop() {
  //pompa sirkulasi dinyalakan secara terus menerus
  
  read_LCD_button();
  
  baca_LCD();
  
  sensor_suhu();
  
  rtcrelay();

  setting_banjur();

  setting_menit();

  read_EEPROM();

}

void rtcrelay(){
  DateTime now = rtc.now();
  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print('/');
  Serial.print(now.hour(), DEC);
  Serial.print('/');
  Serial.print(now.minute(), DEC);
  Serial.print('/');
  Serial.print(now.second(), DEC);
  Serial.println();
  delay(100);
  //setting waktu
  waktu_siram = EPsiram_mnt + 4 ;
  if(now.hour () == EPsiram_st & now.minute() == EPsiram_mnt){
    digitalWrite(Pompa, HIGH);
    Serial.println("banjur fase 1 berhasil");
  }else if(now.hour() >= EPsiram_st & now.minute () >= waktu_siram){
    digitalWrite(Pompa, LOW);
    Serial.println("banjur fase 2 belum");
  }
}


void read_LCD_button(){
  adc_key_in = analogRead(A0);
  Serial.println(adc_key_in);
  if(adc_key_in >= 850){
    button = 0;
  }else if(adc_key_in <= 200){
    button = 1;
  }else if(adc_key_in <= 670 ){
    button = 2;
  }else if(adc_key_in <= 750){
    button = 3;
  }else if(adc_key_in <= 810){
    button = 4;
  }else if(adc_key_in <= 845){
    button = 5;
  }
  if(button == 2){
    Screen++;
  }else if(button == 3){
    Screen--;
  }
  if(Screen >= layar_Akhir){  
    Screen = layar_Akhir;
  }else if(Screen <= layar_Awal){
    Screen = layar_Awal;
  }
  if(Screen >= layar_Akhir){
    Screen = layar_Akhir;
  }
  if(Screen <= layar_Awal){
    Screen = layar_Awal;
  }
  delay(100);
}

void baca_LCD(){
  if(button >= 1 || Reading == 0){
    Reading = 0;
    if(Screen == 1){
      DateTime now = rtc.now();
      lcd.setCursor(0,0);
      lcd.print("-----+ Menu 1 +----");
      lcd.createChar(1, customChar);
      lcd.createChar(2, tempe);
      lcd.createChar(3, arah_menu1);
      lcd.createChar(5, matahari);
      lcd.createChar(6, jam);
      lcd.setCursor(19,1);
      lcd.write(5);
      lcd.setCursor(19,0);
      lcd.write(3);
      lcd.setCursor(0,1);
      lcd.write(2);
      lcd.setCursor(0,2);
      lcd.write(1);
      lcd.setCursor(1,1);
      lcd.print(" Temp:            ");
      lcd.setCursor(8,1);
      lcd.print(s);
      lcd.setCursor(10,1);
      lcd.print("*C");
      lcd.setCursor(1,2);
      lcd.print(" Humi:             ");
      lcd.setCursor(8,2);
      lcd.print(r);
      lcd.setCursor(10,2);
      lcd.print("%");
      lcd.setCursor(0,3);
      lcd.write(6);
      lcd.setCursor(1,3);
      lcd.print(" ");
      lcd.setCursor(2,3);
      lcd.print(daysOfTheWeek[now.dayOfTheWeek()]);
      lcd.setCursor(8,3);
      lcd.print("/");
      lcd.setCursor(9,3);
      lcd.print(now.hour());
      lcd.setCursor(11,3);
      lcd.print(":");
      lcd.setCursor(12,3);
      lcd.print(now.minute());
      lcd.setCursor(14,3);
      lcd.print(":");
      lcd.setCursor(15,3);
      lcd.print(now.second());
      lcd.setCursor(17,3);
      lcd.print("   ");
    }else if(Screen == 2){
      DateTime now = rtc.now(); 
      lcd.setCursor(1,0);
      lcd.print("----+ Menu 2 +----");
      lcd.createChar(3, arah_menu1);
      lcd.createChar(4, arah_menu2);
      lcd.createChar(7, cai);
      lcd.createChar(8, ember);
      lcd.setCursor(0,3);
      lcd.write(8);
      lcd.setCursor(0,1);
      lcd.write(7);
      lcd.setCursor(19,0);
      lcd.write(3);
      lcd.setCursor(0,0);
      lcd.write(4);
      lcd.setCursor(0,2);
      lcd.write(7);
      lcd.setCursor(1,1);
      lcd.print(" Set Sir 1:        ");
      lcd.setCursor(12,1);
      lcd.print(EPsiram_st);
      lcd.setCursor(14,1);
      lcd.print(":");
      lcd.setCursor(15,1);
      lcd.print(EPsiram_mnt);
      lcd.setCursor(1,2);
      lcd.print(" Jam skrng:        ");
      lcd.setCursor(12,2);
      lcd.print(now.hour());
      lcd.setCursor(14,2);
      lcd.print(":");
      lcd.setCursor(15,2);
      lcd.print(now.minute());
      lcd.setCursor(1,3);
      lcd.print(" Stats cai:        ");
    }else if(Screen == 3){
      lcd.setCursor(1,0);
      lcd.print("----+ Menu 3 +----");
      lcd.createChar(3, arah_menu1);
      lcd.createChar(4, arah_menu2);
      lcd.createChar(9, gunung);
      lcd.createChar(10, angin);
      lcd.setCursor(0,1);
      lcd.write(9);
      lcd.setCursor(0,2);
      lcd.write(10);
      lcd.setCursor(19,0);
      lcd.write(3);
      lcd.setCursor(0,0);
      lcd.write(4);
      lcd.setCursor(1,1);
      lcd.print(" Alt :             ");
      lcd.setCursor(1,2);
      lcd.print(" Tek :             ");
      lcd.setCursor(0,3);
      lcd.print("                    ");
    }else if(Screen == 4){
      lcd.setCursor(0,0);
      lcd.print("---- Setting Jam ---");
      lcd.setCursor(0,1);
      lcd.print("Tekan tmb 3 (set)   ");
      lcd.setCursor(0,2);
      lcd.print("sett jam :          ");
      lcd.setCursor(11,2);
      lcd.print(EPsiram_st);
      lcd.setCursor(0,3);
      lcd.print("sett men :          ");
    }else if(Screen == 5){
      lcd.setCursor(0,0);
      lcd.print("--- Setting Menit --");
      lcd.setCursor(0,1);
      lcd.print("Tekan tmb 3 (set)   ");
      lcd.setCursor(0,2);
      lcd.print("sett men :          ");
      lcd.setCursor(11,2);
      lcd.print(EPsiram_mnt);
      lcd.setCursor(0,3);
      lcd.print("sett men :          ");
    }
  }
}
//
void setting_banjur(){
  if(Screen != 4){
    return;
  }
  if(button != 5){
    return;
  }else {
    delay(300);
  }
  if(button != 5){
    return;
  }
  while(1){
    read_LCD_button();
    lcd.setCursor(0,0);
    lcd.print("+   Mode Set Jam   +");
    lcd.setCursor(11,2);
    lcd.print("       ");
    lcd.setCursor(11,2);
    lcd.print(siram_st);
    lcd.setCursor(0,1);
    lcd.print("- Tmbl 2 Untuk ++ *-");
    lcd.setCursor(0,3);
    lcd.print("- Tmbl 3 untuk -- *-");
    if(button == 2){
      siram_st = siram_st + 1;
      if (siram_st >= 24){
        siram_st = 24;
      }
    }
    if(button == 3){
      siram_st = siram_st - 1;
      if (siram_st <= 1){
        siram_st = 1;
      }
    }
    if(button == 1){
      EPsiram_st = siram_st;
      EEPROM.write(1, EPsiram_st);
      break;
    }
  }
}

void setting_menit(){
  if(Screen != 5){
    return;
  }
  if(button != 5){
    return;
  }else {
    delay(300);
  }
  if(button != 5){
    return;
  }
  while(1){
    read_LCD_button();
    lcd.setCursor(0,0);
    lcd.print("+ Mode Set Menit +  ");
    lcd.setCursor(11,2);
    lcd.print("       ");
    lcd.setCursor(11,2);
    lcd.print(siram_mnt);
    lcd.setCursor(0,1);
    lcd.print("- Tmbl 2 Untuk ++ *-");
    lcd.setCursor(0,3);
    lcd.print("- Tmbl 3 untuk -- *-");
    if(button == 2){
      siram_mnt = siram_mnt + 1;
      if (siram_mnt >= 60){
        siram_mnt = 59;
      }
    }
    if(button == 3){
      siram_mnt = siram_mnt - 1;
      if (siram_mnt <= 1){
        siram_mnt = 1;
      }
    }
    if(button == 1){
      EPsiram_mnt = siram_mnt;
      EEPROM.write(2, EPsiram_mnt);
      break;
    }
  }
}

void booting(){
  lcd.setCursor(0,0);
  lcd.print("-----+ Nyiram +-----");
  lcd.setCursor(0,1);
  lcd.print("----+  Kembang +----");
  lcd.setCursor(0,2);
  lcd.print("----+ Otomatis +----");
  delay(4000);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("----+ tkn btn 2 +---");
  lcd.setCursor(0,1);
  lcd.print("--+untuk gsr menu+--");
  lcd.setCursor(0,2);
  lcd.print("---+-----------+----");
  lcd.setCursor(0,3);
  lcd.print("---+-----------+----");
  delay(4000);
}

void read_EEPROM(){
  EPsiram_st = EEPROM.read(1);
  EPsiram_mnt= EEPROM.read(2);
}

void sensor_suhu(){
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  s = t;
  r = h;
  delay(100);
}
