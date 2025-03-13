#include <LiquidCrystal.h> // Arduino LCD Library

// Set up RGB pins
#define BLUE 4
#define GREEN 5
#define RED 6

// set up LCD / Temp Reader
int tempPin = 0;
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

unsigned long startTime = 0;
bool isReadingTemp = false;
bool randColor = true;
float lastTempF = 0;
const float TEMP_THRESHOLD = 3.0; // Used to detect significant shifts in temp

void setup() { //sets up LCD & RGB
  lcd.begin(16, 2);

  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);
  lcd.print("Booting Up...");
  
  // Waits 10 seconds to read initial temperature to establish a room temperature baseline
  delay(10000);
  lastTempF = readTemperature();

  lcd.clear();
  lcd.print("Touch to");
  lcd.setCursor(0, 1);
  lcd.print("Read Emotion");
  defaultColor(randColor);
}

void loop() { // Main loop that happens when a shift in tempurature is detected
  float currentTempF = readTemperature();

  // Check for significant shift
  if (!isReadingTemp && abs(currentTempF - lastTempF) >= TEMP_THRESHOLD) { // if isReading Temp is false and a shift and temp is detected, move onto reading temps and showing the emotion / color
    isReadingTemp = true;
    startTime = millis();
    lcd.clear();
    lcd.print("Tempurature");
    lcd.setCursor(0, 1);
    lcd.print("Detected!");
    delay(1000);
    lcd.clear();
  }
  
  if (isReadingTemp) {
    lcd.clear();
    unsigned long elapsedTime = millis() - startTime;

    if (elapsedTime < 10000) {
      // First 20 seconds: read and display temperature
      lcd.print("Reading...");
      lcd.setCursor(0, 1);
      lcd.print("Temp: ??.??");
    }
    else if (elapsedTime < 20000) {
      // Next 10 seconds: display color based on temperature
      lcd.clear();
      randColor = false;
      defaultColor(randColor);
      displayTemperature(currentTempF);
      displayEmotion(currentTempF);
      delay(10000);
    } 
    else {
      // After 30 seconds, reset and monitor for another shift (after waiting 15 to settle to room temp)
      isReadingTemp = false;
      lcd.clear();
      lcd.print("Resetting");
      lcd.setCursor(0, 1);
      lcd.print("Tempurature");
      setColor(100, 100, 100);
      delay(15000);
      randColor = true;
      lcd.clear();
      lcd.print("Touch to");
      lcd.setCursor(0, 1);
      lcd.print("Read Emotion");
      defaultColor(randColor);
      lastTempF = currentTempF; // Update baseline
    }
  } 
  else {
    // Continuously update lastTempF while monitoring
    lastTempF = currentTempF;
  }
}

float readTemperature() { // reads tempurature and converts analog input to Kelvin to Celcius to Farenheit
  int tempReading = analogRead(tempPin);
  
  double tempK = log(10000.0 * ((1024.0 / tempReading - 1)));
  tempK = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * tempK * tempK)) * tempK);
  float tempC = tempK - 273.15;
  float tempF = (tempC * 9.0) / 5.0 + 32.0;
  return tempF;
}

void defaultColor(bool color) { // secondary loop that cycles through RGB values to 

  int redValue;
  int greenValue;
  int blueValue;

  while(color){
    #define delayTime 10 // fading time between colors

    redValue = 255;
    greenValue = 0;
    blueValue = 0;

    for(int i = 0; i < 255; i += 1) { // Cycles through RGB values like a rainbow
      redValue -= 1;
      greenValue += 1;
      analogWrite(RED, redValue);
      analogWrite(GREEN, greenValue);
      delay(delayTime);
      
      // Check for temp shift and breaks if detected
      float currentTempF = readTemperature();
      if (abs(currentTempF - lastTempF) >= TEMP_THRESHOLD) {
        color = false;
        break;
      }
    }

    if (!color) break; // breaks out of while loop if change in temp is found

    redValue = 0;
    greenValue = 255;
    blueValue = 0;

    for(int i = 0; i < 255; i += 1) {
      greenValue -= 1;
      blueValue += 1;
      analogWrite(GREEN, greenValue);
      analogWrite(BLUE, blueValue);
      delay(delayTime);
      
      float currentTempF = readTemperature();
      if (abs(currentTempF - lastTempF) >= TEMP_THRESHOLD) {
        color = false;
        break;
      }
    }

    if (!color) break; // breaks out of while loop if change in temp is found

    redValue = 0;
    greenValue = 0;
    blueValue = 255;

    for(int i = 0; i < 255; i += 1) {
      blueValue -= 1;
      redValue += 1;
      analogWrite(BLUE, blueValue);
      analogWrite(RED, redValue);
      delay(delayTime);
      
      float currentTempF = readTemperature();
      if (abs(currentTempF - lastTempF) >= TEMP_THRESHOLD) {
        color = false;
        break;
      }
    }
  }
}


void displayTemperature(float tempF) { // method for checking displaying temp 
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.setCursor(6, 0);
  lcd.print(tempF);
  lcd.print(" F ");
}

void displayEmotion(float tempF) { // displays color associated with emotion and emotion
  lcd.setCursor(0, 1);
  lcd.print("Emotion: ");

  if (tempF >= 90.01) {
    setColor(255, 0, 0); // Red
    lcd.setCursor(9, 1);
    lcd.print("STRESSED");
  }
  else if (tempF <= 90.00 && tempF > 86.01) {
    setColor(175, 0, 250); // Purple
    lcd.setCursor(9, 1);
    lcd.print("NERVOUS ");
  }
  else if (tempF <= 86.00 && tempF > 82.01) {
    setColor(0, 255, 0); // Green
    lcd.setCursor(9, 1);
    lcd.print("CONTENT ");
  }
  else if (tempF <= 82.00 && tempF > 78.01) {
    setColor(249, 208, 64); // Orange-yellow
    lcd.setCursor(9, 1);
    lcd.print("CALM   "); 
  }
  else if (tempF <= 78.00) {
    setColor(0, 0, 255); // Blue
    lcd.setCursor(9, 1);
    lcd.print("SAD    ");
  }
}

void setColor(int red, int green, int blue) { // makes setting temp colors easier
  analogWrite(RED, red);
  analogWrite(GREEN, green);
  analogWrite(BLUE, blue);
}