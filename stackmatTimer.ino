#include<Wire.h>
#include <LiquidCrystal_I2C.h>

//setting up lcd
const int en = 2, rw = 1, rs = 0, d4 = 4, d5 = 5, d6 = 6, d7 = 7, bl = 3;
const int i2cAddress = 0x27;
LiquidCrystal_I2C lcd(i2cAddress, en, rw, rs, d4, d5, d6, d7, bl, POSITIVE);


// button pins
int b1 = 2;
int b2 = 3;
int r = 4;

//timing values
int timerReady = 0;
int timerNotRunning = 1;

//times
unsigned long start = 0;
unsigned long done = 0;
float endTime = 0.00;
float oldTime = 0.00;
unsigned long timeHeld = 0;

// led pins 
int redPin = 5;
int greenPin = 6;
int bluePin = 7;

void setup() {
pinMode(b1, INPUT);
pinMode(b2,INPUT);
pinMode(r,INPUT);

pinMode(redPin, OUTPUT);
pinMode(greenPin, OUTPUT);
pinMode(bluePin, OUTPUT);

Serial.begin(9600);
lcd.begin(16,2);

// default settings for lights and time:
lcd.setCursor(5,0);
lcd.print("0.00");
digitalWrite(redPin, HIGH);
}


//=====================================================================================================================================================

  
void convertTime(float input){ // funtion to convert time to minutes:seconds.centiseconds
  
  if(input > 600){
       
   
        lcd.print("DNF          ");
   
    }else if (input < 60){
      //Serial.println(input);
      lcd.print(input);
    }else{
  
  int tSecs = int(input); // total seconds
  int secs = tSecs % 60;  // seconds that will be displayed
  float centiSecs = input - tSecs;
  float secondPart = secs + centiSecs; // includes both seconds and centiseconds for easier display 
  int mins = (tSecs - secs)/60;

  // convert to strings in order to display

  String sSecondPart = String(secondPart);
  String sMins = String(mins);
  if (tSecs % 60 < 10){
    String output1 = sMins + ":" + "0" + sSecondPart;
  
    lcd.print(output1);
  }else{
  String output2 = sMins + ":" + sSecondPart + "      "; // time that will be displayed

  lcd.print(output2);
  }
   }
}


//==================================================================================================================================================


//==================================================================================================================================================


void loop() {

lcd.setCursor(5,0);
  
int b1Val = digitalRead(b1);
int b2Val = digitalRead(b2);
int rVal = digitalRead(r);

if(rVal == 1){
  //Serial.println("0.00");
  lcd.clear();
  lcd.setCursor(5,0);
  lcd.print("0.00");
  
  timerNotRunning = 1;
  timerReady = 0;
  oldTime = 0.00;

  digitalWrite(redPin, HIGH);
  digitalWrite(greenPin, LOW);
  digitalWrite(bluePin, LOW);
}

if(b1Val == 1 && b2Val == 1){  //timer is about to start or is stopped
  if(timerNotRunning == 1){    //timer is about to start
    timeHeld = millis();
    timerNotRunning = 0;
    timerReady = 1;
   digitalWrite(redPin, LOW);
   digitalWrite(greenPin, HIGH);
   digitalWrite(bluePin, LOW);
      }
  
  if (timerNotRunning == 0 && timerReady == 0){  //timer is stopped
    done = millis();
    endTime = (done/1000.00 - start/1000.00) + oldTime;
   
      convertTime(endTime);

    Serial.println(endTime);
    timerNotRunning = 1;
    timerReady = 0; 
    oldTime = endTime;

    
    
    digitalWrite(greenPin, LOW);
    digitalWrite(redPin, HIGH);
    digitalWrite(bluePin, LOW);
    delay(1000);
      }
    
    }
else if(b1Val == 1 || b2Val == 1){
      if(millis() - timeHeld < 150){
        timerReady = 0;
        timerNotRunning = 1;
        digitalWrite(greenPin, LOW);
        digitalWrite(redPin, HIGH);
        digitalWrite(bluePin, LOW); 
   
 }
 
    else if(timerNotRunning == 0 && timerReady == 1){ //time starts recording
      start = millis();
      timerReady = 0;
      delay(100);
      digitalWrite(bluePin, HIGH);
      digitalWrite(greenPin, LOW);
    }
 
  
      }
    
while(timerNotRunning == 0 && timerReady == 0){
  if(b1Val == 1 && b2Val == 1){
    break;
  }
  digitalWrite(bluePin, HIGH);
  digitalWrite(greenPin, LOW);
  digitalWrite(redPin, LOW);
  float currentTime = (millis()/1000.00)-(start/1000.00)+ (oldTime);
    convertTime(currentTime);
   break;
   
 }
}
