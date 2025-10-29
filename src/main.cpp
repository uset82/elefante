#include <Arduino.h>
#include <DHT.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

/*
Pin Connections (GPIO Numbers for RP2350 Zero):

Stepper Motor (28BYJ-48 via ULN2003):
- IN1: GPIO 0 (Stepper coil 1)
- IN2: GPIO 1 (Stepper coil 2)
- IN3: GPIO 2 (Stepper coil 3)
- IN4: GPIO 3 (Stepper coil 4)

Position Button: GPIO 10 (Sets stepper positions, active low with pull-up)
Stop Button: GPIO 11 (Stops all operations, active low with pull-up)

Relay (5V module replaced with L293D for DC pump): Enable1 GPIO 12, Input1 GPIO 13, Input2 GPIO 14

Capacitive Soil Moisture Sensor: GPIO 26 (A0, analog input for soil moisture level)

DHT11 (Temp & Humidity): GPIO 8 (Digital input for DHT11 data)

OLED SSD1306 I2C (128x64):
- SDA: GPIO 4 (I2C data line)
- SCL: GPIO 5 (I2C clock line)

Power/Ground:
- VCC (5V from RP2350): For L293D VCC1, DHT11
- VCC (3.3V from RP2350): For OLED, soil sensor
- External 5V Supply (2A+): For ULN2003 VCC, L293D VCC2
- Add 470µF capacitor across the external 5V supply (+ to +, - to GND)
- GND: Connect external supply GND to ULN2003 GND, L293D GND, RP2350 GND
*/

// Stepper motor pins (connected to ULN2003 IN1-IN4)
#define IN1 0
#define IN2 1
#define IN3 2
#define IN4 3

// Button pins (changed to avoid I2C conflict)
#define BUTTON_PIN 10
#define STOP_BUTTON_PIN 11

// Relay replaced with L293D for pump
#define PUMP_ENABLE_PIN 12  // GPIO 12 for L293D enable1
#define PUMP_IN1_PIN 13     // GPIO 13 for L293D input1
#define PUMP_IN2_PIN 14     // GPIO 14 for L293D input2
#define SOIL_SENSOR_PIN A0      // Analog pin for soil moisture sensor (GPIO 26)
#define DHT_PIN 8               // Digital pin for DHT11 data (GPIO 8)
#define DHTTYPE DHT11
DHT dht(DHT_PIN, DHTTYPE);

// OLED display (I2C default pins: SDA GPIO 4, SCL GPIO 5)
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Pump control with L293D (PWM soft start to reduce inrush)
inline void writePump(bool on) {
  if (on) {
    digitalWrite(PUMP_IN1_PIN, HIGH);
    digitalWrite(PUMP_IN2_PIN, LOW);
    // Ramp up enable with PWM
    for (int i = 0; i <= 255; i += 10) {
      analogWrite(PUMP_ENABLE_PIN, i);
      delay(20);
    }
  } else {
    analogWrite(PUMP_ENABLE_PIN, 0);
  }
}

// Soil moisture thresholds
const int DRY_THRESHOLD = 400;
const int WET_THRESHOLD = 200;
float soilAvg = 0;
bool soilAvgInitialized = false;
unsigned long drySince = 0;
unsigned long wetSince = 0;
const unsigned long DRY_STABLE_TIME = 1000;
const unsigned long WET_STABLE_TIME = 1000;
const unsigned long MAX_ON_TIME = 10000;

// Pump control
bool pumpState = false;
unsigned long lastPumpToggle = 0;
const unsigned long PUMP_ON_TIME = 3000;
const unsigned long PUMP_OFF_TIME = 5000;

// DHT11 variables
unsigned long lastDhtRead = 0;
const unsigned long DHT_READ_INTERVAL = 1000;
float currentTempC = 22.0;
float currentHumidity = 50.0;
bool dhtInitialized = false;

// OLED update
unsigned long lastOledUpdate = 0;
const unsigned long OLED_UPDATE_INTERVAL = 500;

// Step sequence for 28BYJ-48 (half-step for smoother motion)
int stepSequence[8][4] = {
  {1, 0, 0, 0},
  {1, 1, 0, 0},
  {0, 1, 0, 0},
  {0, 1, 1, 0},
  {0, 0, 1, 0},
  {0, 0, 1, 1},
  {0, 0, 0, 1},
  {1, 0, 0, 1}
};

int stepDelay = 5; // Delay between steps in ms (increased for smoother motion and more torque)
long currentStep = 0; // Current step position
long position1 = -1; // First marked position
long position2 = -1; // Second marked position
unsigned long buttonPressTime = 0; // Time when button was pressed
bool waitingAfterFirstPress = false; // State: waiting 10 seconds after first press
bool oscillating = false; // State: oscillating between positions
int direction = 1; // 1 for forward, -1 for backward
bool stopped = false; // Flag to stop everything
int printCounter = 0; // Counter for printing position

void setup() {
  // Set stepper pins as outputs
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // Set button pins as input with pull-up
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(STOP_BUTTON_PIN, INPUT_PULLUP);

  // Pump pins (L293D)
  pinMode(PUMP_ENABLE_PIN, OUTPUT);
  pinMode(PUMP_IN1_PIN, OUTPUT);
  pinMode(PUMP_IN2_PIN, OUTPUT);
  writePump(false); // Pump OFF initially

  // Initialize DHT11
  dht.begin();
  delay(2000); // Stabilize

  // Initialize OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  // Initialize serial for feedback
  Serial.begin(115200);
  Serial.println("Rotating table with watering system setup.");
}

float getAngle(long steps) {
  // Assuming 4096 steps per revolution (half-step)
  return (steps % 4096) * 360.0 / 4096.0;
}

void stepMotor(int dir) {
  int step = (currentStep % 8 + 8) % 8; // Ensure positive modulo
  if (dir == -1) {
    step = (7 - step) % 8; // Reverse sequence for backward
  }
  digitalWrite(IN1, stepSequence[step][0]);
  digitalWrite(IN2, stepSequence[step][1]);
  digitalWrite(IN3, stepSequence[step][2]);
  digitalWrite(IN4, stepSequence[step][3]);
  delay(stepDelay);
  currentStep += dir;

  // Print position every 100 steps for feedback
  printCounter++;
  if (printCounter >= 100) {
    Serial.print("Current Step: ");
    Serial.print(currentStep);
    Serial.print(" | Angle: ");
    Serial.println(getAngle(currentStep));
    printCounter = 0;
  }
}

void loop() {
  if (stopped) {
    Serial.println("System stopped. Power cycle to restart.");
    while (1) {
      delay(1000);
    }
  }

  unsigned long currentTime = millis();

  // DHT11 reading
  if (currentTime - lastDhtRead >= DHT_READ_INTERVAL) {
    float t = dht.readTemperature();
    float h = dht.readHumidity();
    if (!isnan(t) && !isnan(h)) {
      currentTempC = t;
      currentHumidity = h;
      if (!dhtInitialized) {
        Serial.println("DHT11 initialized.");
        dhtInitialized = true;
      }
    }
    lastDhtRead = currentTime;
  }

  // Soil moisture and pump control
  int soilValue = analogRead(SOIL_SENSOR_PIN);
  if (!soilAvgInitialized) {
    soilAvg = soilValue;
    soilAvgInitialized = true;
  } else {
    soilAvg = soilAvg * 0.9f + soilValue * 0.1f;
  }

  if (soilAvg > DRY_THRESHOLD) {
    if (drySince == 0) drySince = currentTime;
    wetSince = 0;
  } else if (soilAvg < WET_THRESHOLD) {
    if (wetSince == 0) wetSince = currentTime;
    drySince = 0;
  } else {
    drySince = 0;
    wetSince = 0;
  }

  bool dryStable = (drySince > 0) && ((currentTime - drySince) >= DRY_STABLE_TIME);
  bool wetStable = (wetSince > 0) && ((currentTime - wetSince) >= WET_STABLE_TIME);

  if (!pumpState && dryStable && (currentTime - lastPumpToggle >= PUMP_OFF_TIME)) {
    pumpState = true;
    writePump(true);
    lastPumpToggle = currentTime;
    Serial.println("Pump ON");
  } else if (pumpState && wetStable && (currentTime - lastPumpToggle >= PUMP_ON_TIME)) {
    pumpState = false;
    writePump(false);
    lastPumpToggle = currentTime;
    Serial.println("Pump OFF");
  } else if (pumpState && (currentTime - lastPumpToggle >= MAX_ON_TIME)) {
    pumpState = false;
    writePump(false);
    lastPumpToggle = currentTime;
    Serial.println("Pump OFF (safety)");
  }

  // OLED update
  if (currentTime - lastOledUpdate >= OLED_UPDATE_INTERVAL) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.printf("Temp: %.1f C\nHum: %.0f %%\nSoil: %d\nPump: %s\nStep: %ld\nAng: %.1f",
                   currentTempC, currentHumidity, (int)soilAvg, pumpState ? "ON" : "OFF", currentStep, getAngle(currentStep));
    display.display();

    // Output for fan plotter
    Serial.print("Temp: ");
    Serial.print(currentTempC);
    Serial.println(" C");
    Serial.print("Humidity: ");
    Serial.print(currentHumidity);
    Serial.println(" %");
    Serial.print("Pot: ");
    Serial.println((int)soilAvg);
    Serial.print("Servo: ");
    Serial.print(getAngle(currentStep));
    Serial.println("°");
    Serial.print("Motor: ");
    Serial.print(pumpState ? 255 : 0);
    Serial.print(" | ");
    Serial.println(pumpState ? "ON" : "OFF");

    lastOledUpdate = currentTime;
  }

  // Check stop button
  if (digitalRead(STOP_BUTTON_PIN) == LOW) {
    Serial.println("Stop button pressed!");
    oscillating = false;
    waitingAfterFirstPress = false;
    position1 = -1;
    position2 = -1;
    stopped = true;
    // Stop motor
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
    Serial.println("Stopping system...");
    delay(200); // Debounce
  }

  // Check button press (active low)
  if (digitalRead(BUTTON_PIN) == LOW) {
    if (!waitingAfterFirstPress && !oscillating) {
      // First press: mark position1
      position1 = currentStep;
      buttonPressTime = millis();
      waitingAfterFirstPress = true;
      Serial.print("Position 1 set at Step: ");
      Serial.print(position1);
      Serial.print(" | Angle: ");
      Serial.println(getAngle(position1));
      delay(200); // Debounce
    } else if (waitingAfterFirstPress && millis() - buttonPressTime >= 10000) {
      // Second press after 10 seconds: mark position2
      position2 = currentStep;
      oscillating = true;
      waitingAfterFirstPress = false;
      Serial.print("Position 2 set at Step: ");
      Serial.print(position2);
      Serial.print(" | Angle: ");
      Serial.println(getAngle(position2));
      Serial.println("Starting oscillation between positions.");
      delay(200); // Debounce
    }
  }

  // Stepper motor control
  if (oscillating) {
    // Oscillate between position1 and position2
    long target = (direction == 1) ? position2 : position1;
    if (currentStep != target) {
      stepMotor(direction);
    } else {
      // Reached target, reverse direction
      direction = -direction;
      Serial.print("Reached ");
      Serial.print(direction == 1 ? "Position 2" : "Position 1");
      Serial.println(", reversing direction.");
      delay(500); // Pause at position
    }
  } else if (waitingAfterFirstPress) {
    // Continue rotating after first press, but wait 10 seconds before allowing second press
    if (millis() - buttonPressTime >= 10000) {
      // Continue rotating
      stepMotor(1);
    } else {
      Serial.print("Waiting... ");
      Serial.print((10000 - (millis() - buttonPressTime)) / 1000);
      Serial.println(" seconds left.");
      delay(1000); // Print every second
    }
  } else {
    // Initial continuous rotation
    stepMotor(1);
    delay(1); // Small delay to reduce CPU load and power spikes
  }
}