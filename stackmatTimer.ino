#include <LiquidCrystal_I2C.h>
#include <Wire.h>

#define EN 2
#define RW 1
#define RS 0
#define D4 4
#define D5 5
#define D6 6
#define D7 7
#define BL 3
#define I2C_ADDRESS 0x27

#define B1 2
#define B2 3
#define RESET 4

#define RED_PIN 5
#define GREEN_PIN 6
#define BLUE_PIN 7
enum Color {RED, BLUE, GREEN};

LiquidCrystal_I2C lcd(I2C_ADDRESS, EN, RW, RS, D4, D5, D6, D7, BL, POSITIVE);
bool timerReady = false;
bool timerRunning = false;

unsigned long start = 0;
unsigned long timeHeld = 0;

void setup() {
  pinMode(B1, INPUT);
  pinMode(B2, INPUT);
  pinMode(RESET, INPUT);

  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);

  lcd.begin(16, 2);
  Serial.begin(9600);

  // default settings for lights and time:
  lcd.setCursor(5, 0);
  lcd.print("0.00");
  setColor(RED);
}

void setColor(Color color) {
  if (color == RED)
    digitalWrite(RED_PIN, HIGH);
  else
    digitalWrite(RED_PIN, LOW);
  if (color == BLUE)
    digitalWrite(BLUE_PIN, HIGH);
  else
    digitalWrite(BLUE_PIN, LOW);
  if (color == GREEN)
    digitalWrite(GREEN_PIN, HIGH);
  else
    digitalWrite(GREEN_PIN, LOW);
}

void convertTime(float time) { // funtion to convert time to minutes:seconds.centiseconds
  if (time > 600) {
    lcd.print("DNF          ");
  } else if (time < 60) {
    // Serial.println(input);
    lcd.print(time);
  } else {

    int totalSeconds = (int)time;
    int secs = totalSeconds % 60;  // seconds that will be displayed
    float centiSecs = time - totalSeconds;
    float secondPart =
        secs +
        centiSecs; // includes both seconds and centiseconds for easier display
    int mins = (totalSeconds - secs) / 60;

    // convert to strings in order to display

    String sSecondPart = String(secondPart);
    String sMins = String(mins);
    if (totalSeconds % 60 < 10) {
      String output1 = sMins + ":" + "0" + sSecondPart;

      lcd.print(output1);
    } else {
      String output2 =
          sMins + ":" + sSecondPart + "      "; // time that will be displayed

      lcd.print(output2);
    }
  }
}

void loop() {
  lcd.setCursor(5, 0);

  int b1Val = digitalRead(B1);
  int b2Val = digitalRead(B2);
  int rVal = digitalRead(RESET);

  if (rVal) {
    lcd.clear();
    lcd.setCursor(5, 0);
    lcd.print("0.00");
    setColor(RED);

    timerReady = false;
    timerRunning = false;
  }

  if (b1Val && b2Val) {
    if (timerRunning) { //timer stopped
      float endTime = (((float)millis()) / 1000.0) - (((float)start) / 1000.0);
      convertTime(endTime);
      timerReady = false;
      timerRunning = false;
      setColor(RED);
      delay(200);
    } else if (!timerRunning && timerReady && millis() - timeHeld > 300) {
      setColor(GREEN);
    } else if(!timerReady){ //hands placed on timer
      timeHeld = millis();
      timerRunning = false;
      timerReady = true;
    }

  } else if ((b1Val || b2Val) && millis() - timeHeld <= 300) {
    // if someone takes off their hands 300ms after puting their hands down the timer resets
    timerReady = false;
    timerRunning = false;
    setColor(RED);

  } else if(!b1Val || !b2Val){
    if(timerReady){ //timer started
      start = millis();
      timerReady = false;
      timerRunning = true;
      delay(100);
      setColor(BLUE);
    } else if(!timerReady && timerRunning){
      float now = (((float)millis()) / 1000.0) - (((float)start) / 1000.0);
      convertTime(now);
    }
  } 
}
