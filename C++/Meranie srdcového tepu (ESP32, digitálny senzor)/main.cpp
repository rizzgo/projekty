/**************************************************************************
 Projekt IMP - Meranie srdcového tepu

 Richard Seipel - xseipe00

 **************************************************************************/

#include <SPI.h>
#include <Wire.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "MAX30105.h"
#include "heartRate.h"


/*
 DISPLAY
 **************************************************************************/

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for SSD1306 display connected using software SPI (default case):
#define OLED_MOSI  23
#define OLED_CLK   18
#define OLED_DC    13
#define OLED_CS    5
#define OLED_RESET 12

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
  OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

uint16_t width;
uint16_t height;

/*
 HEART-RATE SENSOR
 **************************************************************************/

MAX30105 particleSensor;

const byte RATE_SIZE = 6; //Increase this for more averaging. 4 is good.
byte rates[RATE_SIZE]; //Array of heart rates
byte rateSpot = 0;
long lastBeat = 0; //Time at which the last beat occurred

float beatsPerMinute;
int beatAvg;
int calibrated = false;


/*
 DISPLAY FUNCTIONS
 **************************************************************************/

void printCenter(String text, int line=0) {
  int16_t x1;
  int16_t y1;
  display.getTextBounds(text, 0, 0, &x1, &y1, &width, &height);
  display.setCursor((SCREEN_WIDTH - width) / 2, (SCREEN_HEIGHT - height) / 2 + line*16);
  display.print(text);
}

void displayHeartRate(int beatAvg) {
  display.clearDisplay();
  printCenter((String)beatAvg+" BPM");
  display.display();
}

void displayNoFinger() {
  display.clearDisplay();
  printCenter("Place your index finger on the sensor.");
  display.display();
}

void displayCalibration() {
  display.clearDisplay();
  printCenter("calib", -1);
  printCenter("ration",0);
  printCenter("...", 1);
  display.display();
}


/*
 SETUP
 **************************************************************************/

void setup() {
  // Setup display
  Serial.begin(9600);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  
  Serial.end();

  // Setup sensor
  Serial.begin(115200);

  // Initialize sensor
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed
  {
    Serial.println("MAX30105 was not found. Please check wiring/power. ");
    while (1);
  }

  particleSensor.setup(); //Configure sensor with these settings
  particleSensor.setPulseAmplitudeRed(0x0A); //Turn Red LED to low to indicate sensor is running
  particleSensor.setPulseAmplitudeGreen(0); //Turn off Green LED
}


/*
 PROGRAM LOOP
 **************************************************************************/

void loop() {
  long irValue = particleSensor.getIR();
  long delta;

  if (checkForBeat(irValue))
  {
    //We sensed a beat!
    delta = millis() - lastBeat;
    lastBeat = millis();

    beatsPerMinute = 60 / (delta / 1000.0);

    if (20 < beatsPerMinute && beatsPerMinute < 255)
    {
      rates[rateSpot++] = (byte)beatsPerMinute; //Store this reading in the array
      rateSpot %= RATE_SIZE; //Wrap variable

      //Take average of readings
      beatAvg = 0;
      for (byte x = 0 ; x < RATE_SIZE ; x++)
        beatAvg += rates[x];
      beatAvg /= RATE_SIZE;
    }
  }

  if (!calibrated && beatAvg != 0){
    calibrated = true;
  }

  if (irValue < 50000) {
    displayNoFinger();
    beatAvg = 0;
    calibrated = false;
  }
  else if (!calibrated) {
    displayCalibration();
  }
  else {
    displayHeartRate(beatAvg);
  }
}
