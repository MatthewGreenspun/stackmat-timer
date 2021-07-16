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

LiquidCrystal_I2C lcd(I2C_ADDRESS, EN, RW, RS, D4, D5, D6, D7, BL, POSITIVE);

#define B1 2
#define B2 3
#define RESET 4

// timing values
bool timerReady = false;
bool timerNotRunning = true;

// times
unsigned long start = 0;
unsigned long done = 0;
float endTime = 0.00;
float oldTime = 0.00;
unsigned long timeHeld = 0;

// led pins
#define RED_PIN 5
#define GREEN_PIN 6
#define BLUE_PIN 7

void setup() {
  pinMode(B1, INPUT);
  pinMode(B2, INPUT);
  pinMode(RESET, INPUT);

  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);

  lcd.begin(16, 2);

  // default settings for lights and time:
  lcd.setCursor(5, 0);
  lcd.print("0.00");
  digitalWrite(RED_PIN, HIGH);
}

void convertTime(
    float input) { // funtion to convert time to minutes:seconds.centiseconds

  if (input > 600) {

    lcd.print("DNF          ");

  } else if (input < 60) {
    // Serial.println(input);
    lcd.print(input);
  } else {

    int tSecs = int(input); // total seconds
    int secs = tSecs % 60;  // seconds that will be displayed
    float centiSecs = input - tSecs;
    float secondPart =
        secs +
        centiSecs; // includes both seconds and centiseconds for easier display
    int mins = (tSecs - secs) / 60;

    // convert to strings in order to display

    String sSecondPart = String(secondPart);
    String sMins = String(mins);
    if (tSecs % 60 < 10) {
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

    timerNotRunning = true;
    timerReady = false;
    oldTime = 0.00;

    digitalWrite(RED_PIN, HIGH);
    digitalWrite(GREEN_PIN, LOW);
    digitalWrite(BLUE_PIN, LOW);
  }

  if (b1Val && b2Val) { // timer is about to start or is stopped
    if (timerNotRunning) {   // timer is about to start
      timeHeld = millis();
      timerNotRunning = false;
      timerReady = true;
      digitalWrite(RED_PIN, LOW);
      digitalWrite(GREEN_PIN, HIGH);
      digitalWrite(BLUE_PIN, LOW);
    }

    if (!timerNotRunning && !timerReady) { // timer is stopped
      done = millis();
      endTime = (done / 1000.00 - start / 1000.00) + oldTime;

      convertTime(endTime);

      Serial.println(endTime);
      timerNotRunning = true;
      timerReady = true;
      oldTime = endTime;

      digitalWrite(GREEN_PIN, LOW);
      digitalWrite(RED_PIN, HIGH);
      digitalWrite(BLUE_PIN, LOW);
      delay(1000);
    }

  } else if (b1Val || b2Val ) {
    if (millis() - timeHeld < 150) {
      timerReady = false;
      timerNotRunning = true;
      digitalWrite(GREEN_PIN, LOW);
      digitalWrite(RED_PIN, HIGH);
      digitalWrite(BLUE_PIN, LOW);

    }

    else if (!timerNotRunning && timerReady ) { // time starts recording
      start = millis();
      timerReady = false;
      delay(100);
      digitalWrite(BLUE_PIN, HIGH);
      digitalWrite(GREEN_PIN, LOW);
    }
  }

  while (!timerNotRunning && !timerReady) {
    if (b1Val && b2Val ) {
      break;
    }
    digitalWrite(BLUE_PIN, HIGH);
    digitalWrite(GREEN_PIN, LOW);
    digitalWrite(RED_PIN, LOW);
    float currentTime = (millis() / 1000.00) - (start / 1000.00) + (oldTime);
    convertTime(currentTime);
    break;
  }
}
