#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include<EEPROM.h>
#include <OneWire.h>
#define turnOn 13
#define fan 12
#define bulb 11
OneWire  ds(10);  // on pin 10 (a 4.7K resistor is necessary)
const byte ROWS = 4; //four rows
const byte COLS = 4; //three columns
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
LiquidCrystal_I2C lcd(0x27, 20, 4); // set the LCD address to 0x27 for a 16 chars and 2 line display
byte rowPins[ROWS] = {9, 8, 7, 6}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {5, 4, 3, 2};
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
int T_max = 1440;
int T = 1204;

char c_time[5] = {'.', '.', '.', '.', '\0'};
char ac_time[5] = {'.', '.', '.', '.', '\0'};
char c_temp[3] = {'.', '.', '\0'};
char ac_temp[3] = {'.', '.', '\0'};
long temps_restant = 0;
long temps_prog = 0;
float Tmin = 20;
float Tmax = 80;
long temps_ecoule = 0;
long atemps_restant = 0;
long t_1 = 0;
long prev = 0;
long prev_pre = 0;
int sensorValue = 0;
long temps_prog_eeprom = 0;
long temps_ecoule_eeprom = 0;
int s = 0;
int Tp = 0;
int aTp = 0;
int Tm = 0;
int cc = 0;
int asser = 0;
int enc = 0;
int jeu = 0;
float rpm = 0.0;
float dd=0;
void setup() {
  Serial.begin(9600);
  //EEPROM.write(0,
  // put your setup code here, to run once:
  temps_prog_eeprom = Read_eeprom(0);
  temps_ecoule_eeprom = Read_eeprom(1);
  Tp = read_eeprom_temp();
  temps_prog = temps_prog_eeprom;
  Serial.print("prog=");
  Serial.print(temps_prog_eeprom);
  Serial.print("   ecoul=");
  Serial.println(temps_ecoule_eeprom);
  //temps_ecoule=temps_ecoule*60000;
  temps_restant = temps_prog - temps_ecoule_eeprom;
  lcd.init();
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("P=");
  lcd.setCursor(2, 0);
  lcd.print(c_time);
  lcd.setCursor(6, 0);
  lcd.print("mn");
  pinMode(turnOn, OUTPUT);
  pinMode(fan, OUTPUT);
  pinMode(bulb, OUTPUT);
  start_shake(0);
  start_fan_bulb(0);
  Serial.print(60 * (float)1000000 / (float)(400 * map(analogRead(A0), 550, 1023, 600, 2000)));
  delay(6000);
}

void loop() {
  //temps_restant=temps_prog;
  Tm = Calcul_temp_celsius_M();
  affiche_temp_mes(Tm);
  prev = millis();

  long p_eeprom = millis();
  if (temps_restant > 0)
  {
    int cx = (((float)temps_ecoule_eeprom) / ((float)temps_prog_eeprom)) * 100;
    sensorValue = analogRead(A0);
    sensorValue = sensorValue - sensorValue % 60;
    rpm = N(sensorValue);
    affiche_rpm(rpm);
    if (jeu < 1)
    {
      Serial.write('*');
      jeu = jeu + 1;
    }
    if (Tm >= Tp)
    {
      start_fan_bulb(0);
      asser = 1;
    }
    else
    {
      start_fan_bulb(1);
      // Serial.write('*');
    }
  }

  //Tp=read_eeprom_temp();
  while (temps_restant > 0 && asser != 0)
  {
    asser = 1;
    s = 1;
    int mis_a_jour_led = 0;
    if (millis() - p_eeprom > 60000)
    {
      temps_prog_eeprom = temps_prog;
      temps_ecoule_eeprom = temps_ecoule_eeprom + millis() - p_eeprom;
      Save_eeprom(0, temps_prog_eeprom);
      Save_eeprom(1, temps_ecoule_eeprom);
      save_eeprom_temp(Tp);
      p_eeprom = millis();
      mis_a_jour_led = 1;
      // Serial.println(temps_ecoule_eeprom);
    }
    Tm = Calcul_temp_celsius_M();
    affiche_temp_mes(Tm);
    affiche_temp_prog(Tp);
    temps_ecoule = millis() - prev;
    temps_restant = temps_prog_eeprom - (temps_ecoule_eeprom);
    affiche(temps_prog / 60000);
    sensorValue = analogRead(A0);
    sensorValue = sensorValue - sensorValue % 60;
    rpm = N(sensorValue);
    affiche_rpm(rpm);
    cc = (((float)temps_ecoule_eeprom) / ((float)temps_prog_eeprom)) * 100;

    if (cc < 20)
    {
      Serial.write('0');
    }
    else if (cc < 40)
    {
      Serial.write('1');
    }
    else if (cc < 60)
    {
      Serial.write('2');
    }
    else if (cc < 80)
    {
      Serial.write('3');
    }
    else if (cc < 100)
    {
      Serial.write('4');
    }
    else if (cc < 120)
    {
      Serial.write('5');
    }

    start_shake(1);
    char z = keypad.getKey();
    if (z)
    {
      set_param(z);
    }
    if (Tm >= Tp)
    {
      start_fan_bulb(0);
    }
    else
    {
      start_fan_bulb(1);
    }
  }
  if (s == 1)
  {
    temps_ecoule_eeprom = temps_prog_eeprom;
    Save_eeprom(1, temps_ecoule_eeprom);
    asser = 0;
    start_fan_bulb(0);
    s = 0;
  }

  start_shake(0);
  char z1 = keypad.getKey();
  if (z1)
  {
    set_param(z1);
  }
  affiche(temps_prog / 60000);
  affiche_temp_prog(Tp);
 sensorValue = analogRead(A0);
    sensorValue = sensorValue - sensorValue % 60;
    rpm = N(sensorValue);
    affiche_rpm(rpm);
  // affiche_rpm(60 * (float)1000000 / (float)(400 * map(analogRead(A0), 550, 1023, 600, 2000)));
}

void affiche_temp_mes(int x)
{
  lcd.setCursor(14, 1);
  lcd.write(0xDF);//Symbole du degre en hexadecimal. Correspondant a 223 en decimal
  lcd.setCursor(15, 1);
  lcd.print("C");
  lcd.setCursor(9, 1);
  lcd.print("Tm=");
  lcd.print(x);
}

void affiche_temp_prog(int x)
{
  lcd.setCursor(0, 1);
  lcd.print("Tp=");
  lcd.setCursor(5, 1);
  lcd.write(0xDF);//Symbole du degre en hexadecimal. Correspondant a 223 en decimal
  lcd.setCursor(6, 1);
  lcd.print("C");
  if (x >= 0 && x < 10)
  {
    lcd.setCursor(3, 1);
    lcd.print("0");
    lcd.setCursor(4, 1);
    lcd.print(x);
  }
  else if (x < 100)
  {
    lcd.setCursor(3, 1);
    lcd.print(x);
  }
}
void set_param(char c)
{
  if (c == 'A')
  {
    set_time('A');
  }
  else if (c == 'B')
  {
    set_temp('B');
  }
}
void set_temp(char c)
{

  int k = 0;
  char keye1 = keypad.getKey();
  keye1 = c;
  // put your main code here, to run repeatedly:

  // temps_restant= chiffr_saisi(c_time[0])*1000+chiffr_saisi(c_time[1])*100+chiffr_saisi(c_time[2])*10+chiffr_saisi(c_time[3]);
  // atemps_restant=temps_restant;
  if (keye1) {
    //Serial.println(keye);
    if (keye1 == 'B')
    {
      ac_temp[0] = c_temp[0];
      ac_temp[1] = c_temp[1];
      c_temp[0] = '0';
      c_temp[1] = '0';
      aTp = Tp;
      lcd.setCursor(3, 1);
      lcd.print(c_temp);
      lcd.setCursor(5, 1);
      lcd.print("  ");
      long previous = millis();
      char key1 = '\0';
      do
      {
        key1 = keypad.getKey();
        if (key1)
        {
          //Serial.println(key);
          if (chiffr_saisi(key1) <= 9)
          {
            lcd.setCursor(3, 1);
            if (k == 0)
            {
              previous = millis();
              c_temp[1] = key1;
              k = 1;
              lcd.print(c_temp);
            }
            else if (k == 1)
            {
              c_temp[0] = c_temp[1];
              c_temp[1] = key1;
              k = k + 1;
              lcd.print(c_temp);
              previous = millis();
            }
          }
          if (key1 == 'D')
          {
            c_temp[0] = '0';
            c_temp[1] = '0';
            lcd.setCursor(3, 1);
            lcd.print(c_temp);
            key1 = 0;
            k = 0;
          }
        }

      } while (millis() - previous < 20000 && key1 != '#');
      k = 0;
      if (key1 == '#')
      {
        //Serial.print(chiffr_saisi(key));
        Tp = chiffr_saisi(c_temp[0]) * 10 + chiffr_saisi(c_temp[1]);
        save_eeprom_temp(Tp);
        //affiche_temp_prog(Tp);
        ac_temp[0] = c_temp[0];
        ac_temp[1] = c_temp[1];
        k = 0;
      }
      else
      {
        c_temp[0] = ac_temp[0];
        c_temp[1] = ac_temp[1];
        Tp = aTp;
        affiche_temp_prog(Tp);
        k = 0;
      }
    }
  }
}
void set_time(char c)
{

  int k = 0;
  char keye1 = keypad.getKey();
  keye1 = c;
  // put your main code here, to run repeatedly:

  // temps_restant= chiffr_saisi(c_time[0])*1000+chiffr_saisi(c_time[1])*100+chiffr_saisi(c_time[2])*10+chiffr_saisi(c_time[3]);
  // atemps_restant=temps_restant;
  if (keye1) {
    //Serial.println(keye);
    if (keye1 == 'A')
    {
      ac_time[0] = c_time[0];
      ac_time[1] = c_time[1];
      ac_time[2] = c_time[2];
      ac_time[3] = c_time[3];
      c_time[0] = '0';
      c_time[1] = '0';
      c_time[2] = '0';
      c_time[3] = '0';
      atemps_restant = 60000 * (chiffr_saisi(ac_time[0]) * 1000 + chiffr_saisi(ac_time[1]) * 100 + chiffr_saisi(ac_time[2]) * 10 + chiffr_saisi(ac_time[3]));
      lcd.setCursor(2, 0);
      lcd.print(c_time);
      lcd.setCursor(6, 0);
      lcd.print("  ");
      long previous = millis();
      char key1 = '\0';
      do
      {
        key1 = keypad.getKey();
        if (key1)
        {
          //Serial.println(key);
          if (chiffr_saisi(key1) <= 9)
          {
            lcd.setCursor(2, 0);
            if (k == 0)
            {
              previous = millis();
              c_time[3] = key1;
              k = 1;
              lcd.print(c_time);
            }
            else if (k == 1)
            {
              c_time[2] = c_time[3];
              c_time[3] = key1;
              k = k + 1;
              lcd.print(c_time);
              previous = millis();
            }
            else if (k == 2)
            {
              c_time[1] = c_time[2];
              c_time[2] = c_time[3];
              c_time[3] = key1;
              k = 3;
              lcd.print(c_time);
              previous = millis();
            }
            else if (k == 3)
            {
              c_time[0] = c_time[1];
              c_time[1] = c_time[2];
              c_time[2] = c_time[3];
              c_time[3] = key1;
              k = 4;
              lcd.print(c_time);
              previous = millis();
            }
          }
          if (key1 == 'D')
          {
            c_time[0] = '0';
            c_time[1] = '0';
            c_time[2] = '0';
            c_time[3] = '0';
            lcd.setCursor(2, 0);
            lcd.print(c_time);
            key1 = 0;
            k = 0;
          }
        }

      } while (millis() - previous < 20000 && key1 != '#');
      k = 0;
      if (key1 == '#')
      {
        //Serial.print(chiffr_saisi(key));
        temps_prog = 60000 * (chiffr_saisi(c_time[0]) * 1000 + chiffr_saisi(c_time[1]) * 100 + chiffr_saisi(c_time[2]) * 10 + chiffr_saisi(c_time[3]));
        temps_restant = temps_prog;
        temps_ecoule = 0;
        temps_prog_eeprom = temps_prog;
        temps_ecoule_eeprom = temps_ecoule;
        Save_eeprom(0, temps_prog_eeprom);
        Save_eeprom(1, temps_ecoule_eeprom);
        affiche_rpm((((float)temps_ecoule_eeprom) / ((float)temps_prog_eeprom)) * 100);
        ac_time[0] = c_time[0];
        ac_time[1] = c_time[1];
        ac_time[2] = c_time[2];
        ac_time[3] = c_time[3];
        atemps_restant = temps_restant;
        lcd.setCursor(6, 0);
        lcd.print("mn");
        k = 0;
      }
      else
      {
        c_time[0] = ac_time[0];
        c_time[1] = ac_time[1];
        c_time[2] = ac_time[2];
        c_time[3] = ac_time[3];
        k = 0;
      }
    }
  }
}
void affiche(int Tm)
{
  lcd.setCursor(6, 0);
  lcd.print("mn");
  if (Tm >= 0 && Tm < 10)
  {
    lcd.setCursor(2, 0);
    lcd.print("000");
    lcd.setCursor(5, 0);
    lcd.print(Tm);
  }
  else if (Tm < 100)
  {
    lcd.setCursor(2, 0);
    lcd.print("00");
    lcd.setCursor(4, 0);
    lcd.print(Tm);
  }
  else if (Tm < 1000)
  {
    lcd.setCursor(2, 0);
    lcd.print("0");
    lcd.setCursor(3, 0);
    lcd.print(Tm);
  }
  else if (Tm < 10000)
  {
    lcd.setCursor(2, 0);
    lcd.print(Tm);
  }
}
int chiffr_saisi (char c)
{
  if (c == '0')
  {
    return 0;
  }
  else if (c == '1') return 1;
  else if (c == '2') return 2;
  else if (c == '3') return 3;
  else if (c == '4') return 4;
  else if (c == '5') return 5;
  else if (c == '6') return 6;
  else if (c == '7') return 7;
  else if (c == '8') return 8;
  else if (c == '9') return 9;
  else return 10;


}
float Calcul_temp_celsius()
{
  byte i;
  byte present = 0;
  byte type_s;
  byte data[12];
  byte addr[8];
  float celsius, fahrenheit;

  while ( !ds.search(addr)) {
    //Serial.println("No more addresses.");
    //Serial.println();
    ds.reset_search();
    delay(250);
    //return;
  }

  //Serial.print("ROM =");
  for ( i = 0; i < 8; i++) {
    //Serial.write(' ');
    //Serial.print(addr[i], HEX);
  }

  while (OneWire::crc8(addr, 7) != addr[7]) {
    //Serial.println("CRC is not valid!");
    //return;
  }
  //Serial.println();

  // the first ROM byte indicates which chip
  switch (addr[0]) {
    case 0x10:
      //Serial.println("  Chip = DS18S20");  // or old DS1820
      type_s = 1;
      break;
    case 0x28:
      //Serial.println("  Chip = DS18B20");
      type_s = 0;
      break;
    case 0x22:
      //Serial.println("  Chip = DS1822");
      type_s = 0;
      break;
      /*default:
        //Serial.println("Device is not a DS18x20 family device.");
        return;*/
  }

  ds.reset();
  ds.select(addr);
  ds.write(0x44, 1);        // start conversion, with parasite power on at the end

  delay(1000);     // maybe 750ms is enough, maybe not
  // we might do a ds.depower() here, but the reset will take care of it.

  present = ds.reset();
  ds.select(addr);
  ds.write(0xBE);         // Read Scratchpad

  //Serial.print("  Data = ");
  //Serial.print(present, HEX);
  //Serial.print(" ");
  for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = ds.read();
    //Serial.print(data[i], HEX);
    //Serial.print(" ");
  }
  /*Serial.print(" CRC=");
    Serial.print(OneWire::crc8(data, 8), HEX);
    Serial.println();*/

  // Convert the data to actual temperature
  // because the result is a 16 bit signed integer, it should
  // be stored to an "int16_t" type, which is always 16 bits
  // even when compiled on a 32 bit processor.
  int16_t raw = (data[1] << 8) | data[0];
  if (type_s) {
    raw = raw << 3; // 9 bit resolution default
    if (data[7] == 0x10) {
      // "count remain" gives full 12 bit resolution
      raw = (raw & 0xFFF0) + 12 - data[6];
    }
  } else {
    byte cfg = (data[4] & 0x60);
    // at lower res, the low bits are undefined, so let's zero them
    if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
    else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
    else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
    //// default is 12 bit resolution, 750 ms conversion time
  }
  celsius = (float)raw / 16.0;
  fahrenheit = celsius * 1.8 + 32.0;
  return celsius;
}
float Calcul_temp_celsius_M()
{
  byte i;
  byte present = 0;
  byte type_s;
  byte data[12];
  byte addr[8];
  float celsius, fahrenheit;

  while ( !ds.search(addr)) {
    //Serial.println("No more addresses.");
    //Serial.println();
    ds.reset_search();
    delay(10);


    //return;
  }

  //Serial.print("ROM =");
  for ( i = 0; i < 8; i++) {
    //Serial.write(' ');
    //Serial.print(addr[i], HEX);
  }

  while (OneWire::crc8(addr, 7) != addr[7]) {
    //Serial.println("CRC is not valid!");
    //return;
  }
  //Serial.println();

  // the first ROM byte indicates which chip
  switch (addr[0]) {
    case 0x10:
      //Serial.println("  Chip = DS18S20");  // or old DS1820
      type_s = 1;
      break;
    case 0x28:
      //Serial.println("  Chip = DS18B20");
      type_s = 0;
      break;
    case 0x22:
      //Serial.println("  Chip = DS1822");
      type_s = 0;
      break;
      /*default:
        //Serial.println("Device is not a DS18x20 family device.");
        return;*/
  }

  ds.reset();
  ds.select(addr);
  ds.write(0x44, 1);        // start conversion, with parasite power on at the end

  delay(500);     // maybe 750ms is enough, maybe not



  // we might do a ds.depower() here, but the reset will take care of it.

  present = ds.reset();
  ds.select(addr);
  ds.write(0xBE);         // Read Scratchpad

  //Serial.print("  Data = ");
  //Serial.print(present, HEX);
  //Serial.print(" ");
  for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = ds.read();
    //Serial.print(data[i], HEX);
    //Serial.print(" ");
  }
  /*Serial.print(" CRC=");
    Serial.print(OneWire::crc8(data, 8), HEX);
    Serial.println();*/

  // Convert the data to actual temperature
  // because the result is a 16 bit signed integer, it should
  // be stored to an "int16_t" type, which is always 16 bits
  // even when compiled on a 32 bit processor.
  int16_t raw = (data[1] << 8) | data[0];
  if (type_s) {
    raw = raw << 3; // 9 bit resolution default
    if (data[7] == 0x10) {
      // "count remain" gives full 12 bit resolution
      raw = (raw & 0xFFF0) + 12 - data[6];
    }
  } else {
    byte cfg = (data[4] & 0x60);
    // at lower res, the low bits are undefined, so let's zero them
    if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
    else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
    else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
    //// default is 12 bit resolution, 750 ms conversion time
  }
  celsius = (float)raw / 16.0;
  fahrenheit = celsius * 1.8 + 32.0;
  return celsius;
}
void affiche_rpm(int p)
{
  //int xcv=(float)(560/160)*analogRead(A0);
  lcd.setCursor(9, 0);
  lcd.print("rpm=");
  if (p >= 0 && p < 10)
  {
    lcd.setCursor(13, 0);
    lcd.print("00");
    lcd.setCursor(15, 0);
    lcd.print(p);
  }
  else if (p < 100)
  {
    lcd.setCursor(13, 0);
    lcd.print("0");
    lcd.setCursor(14, 0);
    lcd.print(p);
  }
  else if (p < 1000)
  {
    lcd.setCursor(13, 0);
    lcd.print(p);
  }
}
void Save_eeprom(int a, long temps)
{
  int temps_tab[4] = {0};
  int j = 0;
  temps_tab[3] = temps % 100;
  temps_tab[2] = ((temps - temps_tab[3]) / 100) % 100;
  temps_tab[1] = ((temps - temps_tab[3] - (temps_tab[2] * 100)) / 10000) % 100;
  temps_tab[0] = ((temps - temps_tab[3] - (temps_tab[2] * 100) - (temps_tab[1] * 10000)) / 1000000) % 100;

  for (int i = 4 * a; i < (4 + 4 * a); i++)
  {
    EEPROM.write(i, temps_tab[j]);
    j = j + 1;
  }
  /*EEPROM.write(0,temps_tab[0]);
    EEPROM.write(1,temps_tab[1]);
    EEPROM.write(2,temps_tab[2]);
    EEPROM.write(3,temps_tab[3]);*/

}
long Read_eeprom(int a)
{
  long temps_tab[4] = {0};
  long temps = 0;
  int j = 0;
  for (int i = 4 * a; i < 4 * (a + 1); i++)
  {
    temps_tab[j] = EEPROM.read(i);
    j = j + 1;
  }/*
  temps_tab[0]=EEPROM.read(0);
  temps_tab[1]=EEPROM.read(1);
  temps_tab[2]=EEPROM.read(2);
  temps_tab[3]=EEPROM.read(3);*/
  temps = (temps_tab[0] * 1000000) + (temps_tab[1] * 10000) + (temps_tab[2] * 100) + temps_tab[3];
  return temps;
}
int read_eeprom_temp()
{
  int temp = EEPROM.read(8);
  return temp;
}
void save_eeprom_temp(int temp)
{
  EEPROM.write(8, temp);
}

void start_shake(int i)//demarrage quand i=1
{
  if (i == 0)
  {
    digitalWrite(turnOn, LOW);
  }
  else
  {
    digitalWrite(turnOn, HIGH);
  }
}
void start_fan_bulb(int i)//demarrage quand i=1;
{
  if (i == 0)
  {
    digitalWrite(fan, LOW);
    digitalWrite(bulb, HIGH);
  }
  else
  {
    digitalWrite(fan, HIGH);
    digitalWrite(bulb, LOW);
  }
}
float N(int sensor)
{
  if (sensor == 420) return 80;
  if (sensor == 480) return 100;
  if (sensor == 540) return 120;
  if (sensor == 600) return 140;
  if (sensor == 660) return 160;
  if (sensor == 720) return 180;
  if (sensor == 780) return 200;
  if (sensor == 840) return 220;
  if (sensor == 900) return 240;
  if (sensor == 960) return 260;
}
