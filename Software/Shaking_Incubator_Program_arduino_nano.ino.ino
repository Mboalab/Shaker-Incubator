/*Example sketch to control a stepper motor with A4988/DRV8825 stepper motor driver and Arduino without a library. More info: https://www.makerguides.com */

// Define stepper motor connections and steps per revolution:
#define dirPin 12
#define stepPin 11
#define enablepin 4
#define command_Mot 10
#define stepsPerRevolution 200
#define NOT_AN_INTERRUPT -1
#define motorInterfaceType 1
#define d 50
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <AccelStepper.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
// Create a new instance of the AccelStepper class:
//AccelStepper stepper = AccelStepper(motorInterfaceType, stepPin, dirPin);
int freq = 600;
int nb_int = 0;
void setup() {
  // Declare pins as output:
  lcd.init();                      // initialize the lcd
  lcd.init();
  lcd.backlight();
  // Set the speed in steps per second:
  // stepper.setMaxSpeed(3000);
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(enablepin, OUTPUT);
  pinMode(command_Mot, INPUT);
  pinMode(12, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  lcd.clear();
  Serial.begin(9600);
  for (int a = 0; a < 5; a++)
  {
    jeu_de_led();
  }
  //  initialisation_interrupt();
}


int frq = 0;
int a = 0;
int b = 0;
int sensorValue = 0;
float rpm = 0;
float dd = 0;
void loop() {
  // Set the spinning direction clockwise:
  digitalWrite(dirPin, LOW);
  if (Serial.available())
  {

    char c = Serial.read();

    if (c == '0')
    {
      a = 1;
      digitalWrite(9, LOW);
      digitalWrite(8, LOW);
      digitalWrite(7, LOW);
      digitalWrite(6, LOW);
      digitalWrite(5, LOW);
    }
    else if (c == '1')
    {
      a = 1;
      digitalWrite(9, HIGH);
      digitalWrite(8, LOW);
      digitalWrite(7, LOW);
      digitalWrite(6, LOW);
      digitalWrite(5, LOW);
    }
    else if (c == '2')
    {
      a = 1;
      digitalWrite(9, HIGH);
      digitalWrite(8, HIGH);
      digitalWrite(7, LOW);
      digitalWrite(6, LOW);
      digitalWrite(5, LOW);
    }
    else if (c == '3')
    {
      a = 1;
      digitalWrite(9, HIGH);
      digitalWrite(8, HIGH);
      digitalWrite(7, HIGH);
      digitalWrite(6, LOW);
      digitalWrite(5, LOW);
    }
    else if (c == '4')
    {
      a = 1;
      digitalWrite(9, HIGH);
      digitalWrite(8, HIGH);
      digitalWrite(7, HIGH);
      digitalWrite(6, HIGH);
      digitalWrite(5, LOW);
    }
    else if (c == '5')
    {
      a = 1;
      digitalWrite(9, HIGH);
      digitalWrite(8, HIGH);
      digitalWrite(7, HIGH);
      digitalWrite(6, HIGH);
      digitalWrite(5, HIGH);
    }
    /*else if (c == '*')
      {
      jeu_de_led();
      }*/
  }
  else
  {

    if (a < 1)jeu_de_led();
  }
  if (digitalRead(command_Mot) == HIGH)
  {
    
    b = 1;
    sensorValue = analogRead(A0);
    sensorValue = sensorValue - sensorValue % 60;
    rpm = N(sensorValue);
   // Serial.println(rpm);
    dd = ((float)60000000 / (float)(400 * rpm)) - 125;
    if (rpm > 70)
    {
      digitalWrite(enablepin, LOW);
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(dd);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(dd);
    }
    else
    {
      digitalWrite(enablepin, HIGH);
    }

  }
  else
  {
    if (b == 1)
    {
      sensorValue = analogRead(A0);
      sensorValue = sensorValue - sensorValue % 60;
      rpm = N(sensorValue);
      dd = ((float)60000000 / (float)(400 * rpm)) - 125;
      while (rpm > 80)
      {
        
        for (int j = 1; j <= 200; j++)
        {
          digitalWrite(enablepin, LOW);
          digitalWrite(stepPin, HIGH);
          delayMicroseconds(dd);
          digitalWrite(stepPin, LOW);
          delayMicroseconds(dd);
        }
        rpm=rpm-20;
        dd = ((float)60000000 / (float)(400 * rpm)) - 125;
      }
    }
    digitalWrite(enablepin, HIGH);
    b = 0;
  }

  /*
    if(digitalRead(command_Mot)==HIGH)
    {
    digitalWrite(12, HIGH);
    sensorValue = analogRead(A0);
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(map(sensorValue,550,1023,600,2000));
    digitalWrite(stepPin, LOW);
    delayMicroseconds(map(sensorValue,550,1023,600,2000));
    }
    else
    {
    digitalWrite(12, LOW);
    }*/



}
void initialisation_interrupt()
{
  attachInterrupt(digitalPinToInterrupt(2), update_freq, RISING); //Procedure d'initialisaton des interruptions
  //attachInterrupt(digitalPinToInterrupt(3),AbsenceTensionAvale,CHANGE);
}
void update_freq()
{
  long long previous = micros();
  while (micros() - previous < 1000000) {}
  if (freq == 500)freq = 2500;
  else
    freq = freq - 200;
  Serial.print("freq=");
  Serial.println(freq);

  // delayMicroseconds(1000000);

}
void jeu_de_led()
{
  //Allumer gauche vers droite
  digitalWrite(9, HIGH);
  digitalWrite(8, LOW);
  digitalWrite(7, LOW);
  digitalWrite(6, LOW);
  digitalWrite(5, LOW);
  delay(d);
  digitalWrite(9, HIGH);
  digitalWrite(8, HIGH);
  digitalWrite(7, LOW);
  digitalWrite(6, LOW);
  digitalWrite(5, LOW);
  delay(d);
  digitalWrite(9, HIGH);
  digitalWrite(8, HIGH);
  digitalWrite(7, HIGH);
  digitalWrite(6, LOW);
  digitalWrite(5, LOW);
  delay(d);
  digitalWrite(9, HIGH);
  digitalWrite(8, HIGH);
  digitalWrite(7, HIGH);
  digitalWrite(6, HIGH);
  digitalWrite(5, LOW);
  delay(d);
  digitalWrite(9, HIGH);
  digitalWrite(8, HIGH);
  digitalWrite(7, HIGH);
  digitalWrite(6, HIGH);
  digitalWrite(5, HIGH);
  delay(d);
  //eteindre gauche vers droite
  digitalWrite(9, LOW);
  digitalWrite(8, HIGH);
  digitalWrite(7, HIGH);
  digitalWrite(6, HIGH);
  digitalWrite(5, HIGH);
  delay(d);
  digitalWrite(9, LOW);
  digitalWrite(8, LOW);
  digitalWrite(7, HIGH);
  digitalWrite(6, HIGH);
  digitalWrite(5, HIGH);
  delay(d);
  digitalWrite(9, LOW);
  digitalWrite(8, LOW);
  digitalWrite(7, LOW);
  digitalWrite(6, HIGH);
  digitalWrite(5, HIGH);
  delay(d);
  digitalWrite(9, LOW);
  digitalWrite(8, LOW);
  digitalWrite(7, LOW);
  digitalWrite(6, LOW);
  digitalWrite(5, HIGH);
  delay(d);
  digitalWrite(9, LOW);
  digitalWrite(8, LOW);
  digitalWrite(7, LOW);
  digitalWrite(6, LOW);
  digitalWrite(5, LOW);
  delay(d);
  //Allumer droite vers gauche
  digitalWrite(5, HIGH);
  digitalWrite(6, LOW);
  digitalWrite(7, LOW);
  digitalWrite(8, LOW);
  digitalWrite(9, LOW);
  delay(d);
  digitalWrite(5, HIGH);
  digitalWrite(6, HIGH);
  digitalWrite(7, LOW);
  digitalWrite(8, LOW);
  digitalWrite(9, LOW);
  delay(d);
  digitalWrite(5, HIGH);
  digitalWrite(6, HIGH);
  digitalWrite(7, HIGH);
  digitalWrite(8, LOW);
  digitalWrite(9, LOW);
  delay(d);
  digitalWrite(5, HIGH);
  digitalWrite(6, HIGH);
  digitalWrite(7, HIGH);
  digitalWrite(8, HIGH);
  digitalWrite(9, LOW);
  delay(d);
  digitalWrite(9, HIGH);
  digitalWrite(8, HIGH);
  digitalWrite(7, HIGH);
  digitalWrite(6, HIGH);
  digitalWrite(5, HIGH);
  delay(d);
  //eteindre de la droite vers la gauche
  digitalWrite(5, LOW);
  digitalWrite(6, HIGH);
  digitalWrite(7, HIGH);
  digitalWrite(8, HIGH);
  digitalWrite(9, HIGH);
  delay(d);
  digitalWrite(5, LOW);
  digitalWrite(6, LOW);
  digitalWrite(7, HIGH);
  digitalWrite(8, HIGH);
  digitalWrite(9, HIGH);
  delay(d);
  digitalWrite(5, LOW);
  digitalWrite(6, LOW);
  digitalWrite(7, LOW);
  digitalWrite(8, HIGH);
  digitalWrite(9, HIGH);
  delay(d);
  digitalWrite(5, LOW);
  digitalWrite(6, LOW);
  digitalWrite(7, LOW);
  digitalWrite(8, LOW);
  digitalWrite(9, HIGH);
  delay(d);
  digitalWrite(5, LOW);
  digitalWrite(6, LOW);
  digitalWrite(7, LOW);
  digitalWrite(8, LOW);
  digitalWrite(9, LOW);
  delay(d);
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
