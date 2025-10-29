/*
  Pump Test – Arduino Uno
  Hardware options:
    - MOSFET low-side driver (recommended): logic-level N-MOSFET, series gate resistor ~100 Ω,
      10 kΩ gate pulldown, flyback diode across pump (cathode to +, anode to -).
    - Relay module (on/off only): 5 V relay board with optocoupler preferred.

  Pin plan (keeps future features free):
    - D3  → PWM to MOSFET gate (pump speed control)
    - D8  → Relay IN (if using a relay instead of MOSFET)
    - A0  → Soil sensor (reserved)
    - A4/A5 → I2C (RTC / I2C LCD reserved)
    - D9/D10 → reserved for Servo library (avoid analogWrite on these)

  Select driver type by toggling USE_RELAY.
*/

const bool USE_RELAY = false;        // set true if testing with a 5 V relay module

const int PUMP_PWM_PIN = 3;          // PWM-capable pin for MOSFET gate
const int RELAY_PIN    = 8;          // digital pin for relay IN

void setup() {
  if (USE_RELAY) {
    pinMode(RELAY_PIN, OUTPUT);
    digitalWrite(RELAY_PIN, LOW);    // pump OFF
  } else {
    pinMode(PUMP_PWM_PIN, OUTPUT);
    analogWrite(PUMP_PWM_PIN, 0);    // start at 0% duty
  }
}

void loop() {
  if (USE_RELAY) {
    // Simple on/off pulses
    digitalWrite(RELAY_PIN, HIGH);   // pump ON
    delay(3000);
    digitalWrite(RELAY_PIN, LOW);    // pump OFF
    delay(3000);
  } else {
    // Ramp speed up and down with PWM
    for (int duty = 0; duty <= 255; duty += 5) {
      analogWrite(PUMP_PWM_PIN, duty);
      delay(50);
    }
    for (int duty = 255; duty >= 0; duty -= 5) {
      analogWrite(PUMP_PWM_PIN, duty);
      delay(50);
    }
    delay(1000);
  }
}