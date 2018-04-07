/* Sweep
 by BARRAGAN <http://barraganstudio.com>
 This example code is in the public domain.

 modified 8 Nov 2013
 by Scott Fitzgerald
 http://www.arduino.cc/en/Tutorial/Sweep
*/

#include <Servo.h>
#include <EEPROM.h>
#include  "virtuabotixRTC.h"
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <OneWire.h> 
#include <DallasTemperature.h>

Servo myservo;  
#define ONE_WIRE_BUS 2 
#define SERVO_BUS 9
#define b1_pin 10
#define b2_pin 11
int eeAddress = 0;
String storeDate = "";
//int size =  
int previousState_plus = HIGH;
int previsosState_reset = HIGH;
int pos = 0;    // variable to store the servo position
LiquidCrystal_I2C lcd(0x3f,20,4);
OneWire oneWire(ONE_WIRE_BUS); 
DallasTemperature sensors(&oneWire); 
virtuabotixRTC myRTC(6, 7, 8);
void setup()
{
//  myRTC.setDS1302Time(0, 40,20,0,0,0,0);     
  myservo.attach(SERVO_BUS);  // attaches the servo on pin 9 to the servo object
  myRTC.updateTime();
  Serial.begin(9600);
  pinMode(b1_pin,INPUT_PULLUP);
  pinMode(b2_pin,INPUT_PULLUP);
  lcd.init();                      // initialize the lcd 
  lcd.init();
  // Print a message to the LCD.
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("TankTemp:");
  lcd.setCursor(14,0);
  lcd.print((char)223);
  lcd.setCursor(15,0);
  lcd.print("c");
  lcd.setCursor(0,1);
  lcd.print("Last fed:");
  storeDate = getLastFed();
  if (storeDate == ""){
    storeDate = "--:--"
  }
  lcd.setCursor(9,1);
  lcd.print(storeDate);
//  lcd.print("--");
//  lcd.setCursor(11,1);
//  lcd.print(":");
//  lcd.setCursor(12,1);
//  lcd.print("--");
  
  // Start up the library 
  sensors.begin(); 
}
boolean needfeed(){
  myRTC.updateTime();
//  Serial.println(myRTC.hours%12);
//  Serial.println(myRTC.minutes);
//  Serial.println(myRTC.seconds);
  if((myRTC.hours%10==0)&&myRTC.minutes==0&&myRTC.seconds==0){
    return true;
  }
  return false;
}


void feed(String feeedtime){
  lcd.setCursor(9,1);
  lcd.print(feedtime);
//  int hour = myRTC.hours;
//  if (hour < 10) lcd.print("0"+String(myRTC.hours));
//  else lcd.print(String(myRTC.hours));
//  lcd.setCursor(12,1);
//  int minutes = myRTC.minutes;
//  if (minutes < 10) lcd.print("0"+String(myRTC.minutes));
//  else lcd.print(myRTC.minutes);

  for (pos = 0; pos <= 90; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(5);                       // waits 15ms for the servo to reach the position
  }
  for (pos = 90; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(5);                       // waits 15ms for the servo to reach the position
  }
}
void showTemp(){
  sensors.requestTemperatures();
  lcd.setCursor(9,0);
  lcd.print(sensors.getTempCByIndex(0));
  delay(1000); 
}

void loop()
{
  int currentState_plus = digitalRead(b1_pin); // press = 0
  int currentState_reset = digitalRead(b2_pin);
  if(needfeed()){
   
    String date = "";
    int hour = myRTC.hours;
    if (hour < 10) date+=("0"+String(myRTC.hours));
    else date+=(String(myRTC.hours));
    date+=":";
    int minutes = myRTC.minutes;
    if (minutes < 10) date+=("0"+String(myRTC.minutes));
    else date+=(myRTC.minutes);
    
    if(currentState_plus== HIGH && previousState_plus == LOW) {
    EEPROM.put(eeAddress, date);
    Serial.println(date);
    } 
    feed(date);
  }
  showTemp();
  previousState_plus = currentState_plus;
  previousState_reset = currentState_reset;
}

String getLastFed(){
  String lastFed = "00:00";
  int eeAddress = sizeof(lastFed);
  EEPROM.get(eeAddress,lastFed);
  return lastFed;
  }


