//Time - TimeRTC
#include <TimeLib.h>
#include <Wire.h>
#include <DS1307RTC.h>

//LiquidCrystal - HelloWorld
#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

//buzzer to arduino pin 9
const int buzzer = 9;

//alarm button to pin 6
const int buttonAlarmPin = 6;

//vibration sensor to 0
const int vibrationPin = 0;

//variables for snooze mode
int alarmOff = LOW;
int alarmState = HIGH;      // the current state of the output pin
int gotUp = LOW;

//variable for reading the button status
int alarmReading;           // the current reading from the input pin
int alarmPrevious = LOW;  

//set up the alarm and snooze time
int alarmH = 7;
int alarmM = 30;
int snoozeGap = 1;

int buzzerState = LOW;

int vibrationState;
int countVibration=0;

long time = 0;         // the last time the output pin was toggled
long alarmDebounce = 50;   // the debounce time, increase if the output flickers


void setup() {
  Serial.begin(9600);
  while (!Serial) ; // wait until Arduino Serial Monitor 
  setTime(7, 29, 50, 25, 3, 2017);
  lcd.begin(16, 2);  // set up the LCD's number of columns and rows
  pinMode(buzzer, OUTPUT); // Set buzzer - pin 9 as an output
  pinMode(buttonAlarmPin, INPUT);
  
}

void loop() {
  
    digitalClockDisplay();

    // ring the alarm when it's time
    if(gotUp == LOW && hour() == alarmH && minute() == alarmM && second() == 0){
      alarmM += snoozeGap;
      //Serial.println("alarm minutes: ");
      //Serial.println(alarmM);
      alarmState = HIGH;
      lcd.setCursor(0,1);
      lcd.print("Get up, sweetie!");
      buzzerState = HIGH;
      //Serial.println(buzzerState);
      activateBuzzer();
    }
    
    // when the alarm is on, track the button is pressed or not
    if(buzzerState == HIGH){
      //Serial.println("tracking alarm state");
      trackAlarmState();
    }

    //when the button is pressed
    if(alarmState == LOW && alarmOff == LOW){
      alarmOff = HIGH;
      buzzerState = LOW;
      deactivateBuzzer();
      lcd.clear();
      digitalClockDisplay();
      lcd.setCursor(0,1);
      lcd.print("Good job!");
      delay(3000);
      lcd.clear();
    }

    //after the button is pressed, start tracking vibration
    if(alarmOff == HIGH && gotUp == LOW){
      //Serial.println("track vibration");
      trackVibration();
    }
    //check if the user got up by the number of vibration detected
    if(gotUp == LOW && hour() == alarmH && minute() == alarmM - snoozeGap && second() == 55){
      //Serial.println(countVibration);
      if(countVibration > 500) {
        gotUp = HIGH;
        lcd.setCursor(0, 1);
        lcd.print("You did it!");
      }else{
        lcd.setCursor(0, 1);
        lcd.print("Come On!");
          alarmOff = LOW;
          alarmState = HIGH;
      }
      //Serial.println("Got up?");
      //Serial.println(gotUp);
    }
}

//display time on lcd screen
void digitalClockDisplay(){
  if(hour()<=9){
    lcd.setCursor(0, 0);
    lcd.print(0);
    lcd.setCursor(1, 0);
    lcd.print(hour());
  }else{
    lcd.setCursor(0, 0);
    lcd.print(hour());
  }

  lcd.setCursor(2,0);
  lcd.print(":");

    if(minute()<=9){
    lcd.setCursor(3, 0);
    lcd.print(0);
    lcd.setCursor(4, 0);
    lcd.print(minute());
  }else{
    lcd.setCursor(3, 0);
    lcd.print(minute());
  }
  lcd.setCursor(5,0);
  lcd.print(":");
  
  if(second()<=9){
    lcd.setCursor(6, 0);
    lcd.print(0);
    lcd.setCursor(7, 0);
    lcd.print(second());
  }else{
    lcd.setCursor(6,0);
    lcd.print(second());
  }
  
//  lcd.setCursor(6,0);
//  lcd.print(day());
//  lcd.setCursor(8,0);
//  lcd.print("/");
//  lcd.setCursor(9,0);
//  lcd.print(month());
}

//track the button state for turning off alarm
//retrieve from Arduino - Switch https://www.arduino.cc/en/tutorial/switch
void trackAlarmState(){
  alarmReading = digitalRead(buttonAlarmPin);
  //Serial.println(alarmReading);
  // if the input just went from LOW and HIGH and we've waited long enough
  // to ignore any noise on the circuit, toggle the output pin and remember
  // the time
  if (alarmReading == HIGH && alarmPrevious == LOW && millis() - time > alarmDebounce) {
    if (alarmState == HIGH){
      alarmState = LOW;
      Serial.println("LOW");
    }else{
      alarmState = HIGH;
      Serial.println("HIGH");
    }
    time = millis();    
  }
  alarmPrevious = alarmReading;
}

// trigger the buzzer
void activateBuzzer(){
  tone(buzzer, 500);
}
void deactivateBuzzer(){
  noTone(buzzer);
}

//calculate the number of vibration detected
void trackVibration(){
  if(digitalRead(vibrationPin) == 0 ) countVibration ++;
}
