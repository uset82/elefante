#include <Arduino.h>
#include <DHT.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED display configuration
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Function declarations
void outputPlotterData();
void diagnoseDHT11();

/*
  Elephant Watering System - Pump & Sensor Test with Fan Plotter Integration
  
  Hardware setup:
    - Pin D8 → Relay IN (5V relay module)
    - Pin A0 → Soil moisture sensor AOUT
    - Pin D2 → DHT11 sensor data (single-wire). VCC→5V, GND→GND. Use 10k pull‑up to 5V if your module does not include one.
    - OLED Display Module 128X64 I2C 12864 LCD
      - PIN SCL: I2C serial clock A5
      - PIN SDA: I2C serial DATA A4
    
  Serial output format for fan plotter:
    "Temp: X | Pot: Y | Servo: Z | Motor: W"
    - Temp: DHT11 temperature in °C
    - Pot: Soil moisture sensor raw ADC (0–1023)
    - Servo: Pump status (0°=OFF, 180°=ON)
    - Motor: Pump PWM equivalent (0=OFF, 255=ON)
    
  Relay module wiring:
    - Arduino D8 → Relay IN
    - Arduino 5V → Relay VCC
    - Arduino GND → Relay GND
    - Pump+ → Relay COM
    - External supply+ → Relay NO
    - Pump- → External supply GND
    - Common GND between Arduino and external supply
    
  Soil sensor wiring:
    - Sensor VCC → Arduino 3.3V or 5V
    - Sensor GND → Arduino GND
    - Sensor AOUT → Arduino A0
    
  DHT11 sampling:
    - Read every ~2s (sensor refresh/latency). Avoid tight polling.
    
  Pin reservations for future:
    - A4/A5: I2C (RTC, LCD)
    - D9/D10: Servo library
*/

const bool USE_RELAY = true;         // Using relay module
const int RELAY_PIN = 8;             // Digital pin for relay IN
const int SOIL_SENSOR_PIN = A0;      // Analog pin for soil moisture sensor

// Relay polarity: many modules are ACTIVE LOW (LOW = ON). Set this based on your board.
const bool RELAY_ACTIVE_LOW = true;   // set to false if your relay is ACTIVE HIGH
inline void writeRelay(bool on) {
  if (!USE_RELAY) return;
  digitalWrite(RELAY_PIN, (RELAY_ACTIVE_LOW ? (on ? LOW : HIGH) : (on ? HIGH : LOW)));
}

// Soil moisture thresholds (adjust based on calibration)
const int DRY_THRESHOLD = 400;       // Above this = dry soil (needs water) - LOWERED FOR TESTING
const int WET_THRESHOLD = 200;       // Below this = wet soil (no water needed)
// Add smoothing and stability to avoid rapid relay toggling
float soilAvg = 0;
bool soilAvgInitialized = false;
unsigned long drySince = 0;
unsigned long wetSince = 0;
const unsigned long DRY_STABLE_TIME = 1000;   // ms with dry readings required before turning ON - FASTER FOR TESTING
const unsigned long WET_STABLE_TIME = 1000;   // ms with wet readings required before turning OFF - FASTER FOR TESTING
const unsigned long MAX_ON_TIME = 10000;      // safety: max ON duration if soil never reaches wet

// Variables for pump control and timing
bool pumpState = false;
unsigned long lastPumpToggle = 0;
const unsigned long PUMP_ON_TIME = 3000;   // 3 seconds ON
const unsigned long PUMP_OFF_TIME = 5000;  // 5 seconds OFF

// Variables for data output timing
unsigned long lastDataOutput = 0;
const unsigned long DATA_OUTPUT_INTERVAL = 100;  // 100ms = 10Hz for smooth plotting

// DHT11 sampling variables (declared before use)
unsigned long lastDhtRead = 0;
const unsigned long DHT_READ_INTERVAL = 1000; // Read every 1 second (faster response)
float currentTempC = 22.0;
float currentHumidity = 50.0;
bool dhtInitialized = false;

// Add DHT11 configuration
const int DHT_PIN = 2; // Digital pin for DHT11 data
#define DHTTYPE DHT11
DHT dht(DHT_PIN, DHTTYPE);

void setup() {
  Serial.begin(9600);  // Match the baud rate expected by fan plotter
  
  pinMode(RELAY_PIN, OUTPUT);
  writeRelay(false);      // Pump OFF initially (respects relay polarity)
  
  // Initial status message
  Serial.println("Elephant Watering System - Fan Plotter Mode");
  Serial.println("============================================");

  // Initialize OLED
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.println("Elephant System");
  display.println("Initializing...");
  display.display();

  // Initialize DHT11 with longer delay for stability
  dht.begin();
  Serial.println("Initializing DHT11 sensor...");
  delay(3000);  // Give DHT11 more time to stabilize

  // Try initial reading
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  if (!isnan(t) && !isnan(h)) {
    currentTempC = t;
    currentHumidity = h;
    Serial.print("DHT11 ready - Temp: ");
    Serial.print(t);
    Serial.print("°C, Humidity: ");
    Serial.print(h);
    Serial.println("%");
    dhtInitialized = true;
  } else {
    Serial.println("DHT11 initialization failed - check wiring!");
  }
}

void loop() {
  unsigned long currentTime = millis();

  // DHT11 periodic sampling with better error handling
  if (currentTime - lastDhtRead >= DHT_READ_INTERVAL) {
    float t = dht.readTemperature();
    float h = dht.readHumidity();

    // Better error handling and debugging
    if (!isnan(t) && !isnan(h)) {
      currentTempC = t;
      currentHumidity = h;
      if (!dhtInitialized) {
        Serial.println("DHT11 initialized successfully!");
        dhtInitialized = true;
      }
    } else {
      Serial.print("DHT11 read error - Temp: ");
      Serial.print(isnan(t) ? "NaN" : String(t));
      Serial.print(", Humidity: ");
      Serial.println(isnan(h) ? "NaN" : String(h));
    }
    lastDhtRead = currentTime;
  }

  // Handle pump control using soil moisture with hysteresis and minimum durations
  int soilValue = analogRead(SOIL_SENSOR_PIN);
  // smooth the signal using simple low-pass filter
  if (!soilAvgInitialized) { soilAvg = soilValue; soilAvgInitialized = true; }
  else { soilAvg = soilAvg * 0.9f + soilValue * 0.1f; }

  // track how long readings stay dry/wet (stability)
  if (soilAvg > DRY_THRESHOLD) {
    if (drySince == 0) drySince = currentTime;
    wetSince = 0;
  } else if (soilAvg < WET_THRESHOLD) {
    if (wetSince == 0) wetSince = currentTime;
    drySince = 0;
  } else {
    // in the middle band, reset timers
    drySince = 0;
    wetSince = 0;
  }

  bool dryStable = (drySince > 0) && ((currentTime - drySince) >= DRY_STABLE_TIME);
  bool wetStable = (wetSince > 0) && ((currentTime - wetSince) >= WET_STABLE_TIME);

  if (!pumpState && dryStable && (currentTime - lastPumpToggle >= PUMP_OFF_TIME)) {
    // Turn pump ON when soil is dry and OFF period elapsed
    pumpState = true;
    writeRelay(true);
    lastPumpToggle = currentTime;
    Serial.println(">> Pump ON (dryStable)");
  } else if (pumpState && wetStable && (currentTime - lastPumpToggle >= PUMP_ON_TIME)) {
    // Turn pump OFF when soil is wet and ON period elapsed
    pumpState = false;
    writeRelay(false);
    lastPumpToggle = currentTime;
    Serial.println(">> Pump OFF (wetStable)");
  } else if (pumpState && (currentTime - lastPumpToggle >= MAX_ON_TIME)) {
    // safety cutoff: don't run indefinitely if sensor fails to report wet
    pumpState = false;
    writeRelay(false);
    lastPumpToggle = currentTime;
    Serial.println(">> Pump OFF (safety MAX_ON_TIME)");
  }
  
  // Output data for fan plotter at regular intervals
  if (currentTime - lastDataOutput >= DATA_OUTPUT_INTERVAL) {
    outputPlotterData();
    lastDataOutput = currentTime;
  }
}

// DHT11 variables moved above for proper scope
void outputPlotterData() {
  // Read soil moisture sensor
  int soilValue = analogRead(SOIL_SENSOR_PIN);

  // Use measured temperature from DHT11
  float tempC = currentTempC;

  // Map pump state to servo-like value for visualization
  int servoValue = pumpState ? 180 : 0;  // 180° = ON, 0° = OFF

  // Map pump state to motor PWM equivalent for visualization
  int motorValue = pumpState ? 255 : 0;  // 255 = full speed, 0 = off

  // Output in fan plotter format: "Temp: X | Pot: Y | Servo: Z | Motor: W"
  Serial.print("Temp: ");
  Serial.print(tempC, 1);
  Serial.print("°C | Pot: ");
  Serial.print(soilValue);
  Serial.print(" | Servo: ");
  Serial.print(servoValue);
  Serial.print("° | Motor: ");
  Serial.print(motorValue);
  // Append humidity percentage from DHT11
  Serial.print(" | Humidity: ");
  Serial.print(currentHumidity, 0);
  Serial.print("%");
 
   if (pumpState) {
     Serial.println(" | ON");
   } else {
     Serial.println(" | OFF");
   }

  // Update OLED display
  display.clearDisplay();
  display.setCursor(0,0);
  display.print("Temp: ");
  display.print(tempC, 1);
  display.println(" C");
  display.print("Soil: ");
  display.println(soilValue);
  display.print("Pump: ");
  display.println(pumpState ? "ON" : "OFF");
  display.print("Hum: ");
  display.print(currentHumidity, 0);
  display.println(" %");
  display.display();
}

// DHT11 diagnostic function
void diagnoseDHT11() {
  Serial.println("\n=== DHT11 Diagnostic ===");
  for (int i = 0; i < 5; i++) {
    float t = dht.readTemperature();
    float h = dht.readHumidity();
    Serial.print("Reading ");
    Serial.print(i + 1);
    Serial.print(": Temp=");
    if (isnan(t)) {
      Serial.print("ERROR");
    } else {
      Serial.print(t);
      Serial.print("°C");
    }
    Serial.print(", Humidity=");
    if (isnan(h)) {
      Serial.print("ERROR");
    } else {
      Serial.print(h);
      Serial.print("%");
    }
    Serial.println();
    delay(1000);
  }
  Serial.println("======================\n");
}
