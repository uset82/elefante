#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

/*
  Simple DHT11 + OLED Test Sketch

  Wiring summary (Arduino Uno):
    - DHT11 data  -> D2
    - DHT11 VCC   -> 5V (or 3.3V if module requires)
    - DHT11 GND   -> GND
    - OLED SDA    -> A4
    - OLED SCL    -> A5
    - OLED VCC    -> 5V (check module specs)
    - OLED GND    -> GND
*/

// DHT11 configuration
constexpr uint8_t DHT_PIN = 2;
#define DHTTYPE DHT11
DHT dht(DHT_PIN, DHTTYPE);

// OLED configuration
constexpr int SCREEN_WIDTH = 128;
constexpr int SCREEN_HEIGHT = 64;
constexpr int OLED_RESET = -1;
constexpr uint8_t OLED_ADDRESS = 0x3C;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Sampling interval
constexpr unsigned long READ_INTERVAL_MS = 2000;

// Latest readings
float lastTemperatureC = NAN;
float lastHumidity = NAN;
unsigned long lastSampleAt = 0;
bool sensorOk = false;

// Heart animation variables
int heartSize = 10;
int heartDirection = 1;
const int heartMin = 5;
const int heartMax = 15;

void showBootScreen();
void updateDisplay();
void logReadings();
void readSensor();
void drawHeart(int x, int y, int size);

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ;  // wait for native USB boards; harmless on Uno
  }

  Serial.println(F("DHT11 + OLED test starting..."));
  Serial.println(F("Pins: DHT->D2, OLED SDA->A4, OLED SCL->A5"));

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    Serial.println(F("ERROR: SSD1306 allocation failed. Check wiring."));
    for (;;) {
      delay(100);
    }
  }

  display.clearDisplay();
  dht.begin();

  showBootScreen();

  // Allow the DHT sensor a moment to stabilize
  delay(1000);
  readSensor();
  updateDisplay();
  logReadings();
}

void loop() {
  const unsigned long now = millis();

  if (now - lastSampleAt >= READ_INTERVAL_MS) {
    readSensor();
    updateDisplay();
    logReadings();
    lastSampleAt = now;
  }

  // Animate heart
  heartSize += heartDirection;
  if (heartSize >= heartMax || heartSize <= heartMin) {
    heartDirection = -heartDirection;
  }
}

void showBootScreen() {
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(F("Elephant Test Rig"));
  display.println(F("DHT11 + OLED"));
  display.println();
  display.println(F("D2  -> DHT data"));
  display.println(F("A4  -> SDA"));
  display.println(F("A5  -> SCL"));
  display.display();
}

void readSensor() {
  float temperature = dht.readTemperature();      // Celsius by default
  float humidity = dht.readHumidity();

  if (isnan(temperature) || isnan(humidity)) {
    sensorOk = false;
    Serial.println(F("WARNING: Failed to read from DHT11"));
  } else {
    sensorOk = true;
    lastTemperatureC = temperature;
    lastHumidity = humidity;
  }
}

void updateDisplay() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(F("ELFY Monitor"));
  display.println(F("-------------"));

  // Draw breathing heart in top right (yellow area)
  drawHeart(100, 9, heartSize);

  // Move cursor to blue area (around row 32 for 64px height)
  display.setCursor(0, 32);

  if (!sensorOk) {
    display.println(F("Sensor error"));
    display.println(F("Check wiring!"));
  } else {
    display.print(F("Temp: "));
    display.print(lastTemperatureC, 1);
    display.println(F(" C"));

    display.print(F("Humidity: "));
    display.print(lastHumidity, 1);
    display.println(F(" %"));
  }

  display.display();
}

void logReadings() {
  if (sensorOk) {
    Serial.print(F("Temperature: "));
    Serial.print(lastTemperatureC, 1);
    Serial.print(F(" C, Humidity: "));
    Serial.print(lastHumidity, 1);
    Serial.println(F(" %"));
  } else {
    Serial.println(F("Sensor error - unable to read values"));
  }
}

void drawHeart(int x, int y, int size) {
  display.fillCircle(x - size/2, y - size/4, size/2, SSD1306_WHITE);
  display.fillCircle(x + size/2, y - size/4, size/2, SSD1306_WHITE);
  display.fillTriangle(x - size, y - size/4, x + size, y - size/4, x, y + size, SSD1306_WHITE);
}
