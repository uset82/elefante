# Elephant Watering System — Project Description

This project is an Arduino‑based automatic plant watering system with real‑time monitoring on a web plotter. It reads air temperature and humidity (DHT11), soil moisture (analog sensor), and controls a mini water pump via a relay, only watering when the soil is dry and stable.

## Goals
- Keep soil slightly moist without overwatering.
- Prevent relay/pump chattering via smoothing and stability checks.
- Visualize live metrics (temperature, humidity, soil moisture, servo angle, pump PWM/status) in a browser.

## Hardware
- `Arduino Uno`
- `DHT11` sensor (air temperature, humidity) on pin `D2`
- Soil moisture sensor analog output on pin `A0`
- Relay module driving a mini pump on pin `D8`
- Optional servo (angle reporting) and external 5–12V supply for the pump

## Wiring (essential)
- DHT11: `VCC` → 5V, `GND` → GND, `DATA` → `D2`
- Soil sensor: `AOUT` → `A0`, `VCC` → 5V (or 3.3V per sensor), `GND` → GND
- Relay: `IN` → `D8`, `VCC` → 5V, `GND` → GND; pump powered through relay contacts

## Firmware Logic (elefant/src/main.cpp)
- Samples DHT11 roughly every ~2 seconds.
- Reads soil moisture from `A0` each loop and applies a low‑pass filter: `soilAvg = 0.9*soilAvg + 0.1*soilValue`.
- Uses hysteresis thresholds (e.g., `DRY_THRESHOLD`, `WET_THRESHOLD`) and stability timers (`drySince`, `wetSince`) so the pump only turns ON after soil is consistently dry, and turns OFF after soil is consistently wet.
- Enforces minimum ON/OFF durations and a safety `MAX_ON_TIME` cutoff to avoid rapid toggling and long continuous runs.

> See the constants and exact timings in `elefant/src/main.cpp` for current values.

## Serial Output Format
Printed at ~100 ms cadence for the plotter:
```
Temp: <C> | Pot: <ADC> | Servo: <deg> | Motor: <PWM> | Humidity: <pct>% | ON|OFF
```
- Example: `Temp: 23.4 | Pot: 612 | Servo: 90 | Motor: 255 | Humidity: 55% | ON`

## Web Plotter (fan_plotter)
- Connect to the device via Web Serial at `9600` baud.
- Charts: Temperature (°C), Humidity (%), Soil Moisture (ADC), Soil Moisture (% derived), Servo (°), Motor/Pump PWM.
- Displays current values and pump status.

### Soil Moisture Percent (derived)
The plotter maps `A0` ADC to moisture % using a simple linear mapping (tunable):
- `DRY ≈ 1023` (air)
- `WET ≈ 300` (sensor fully wet)
You can calibrate by measuring your sensor in air and in water, then update mapping in `fan_plotter/src/main.js`.

## Usage
1. Upload firmware to the Uno (PlatformIO or Arduino IDE). Close other apps using the COM port during upload.
2. Open the plotter (`npm run dev` in `fan_plotter`, then visit `http://localhost:5173/`).
3. Click “Connect to Serial Port”, select the Uno (COM4 typical on Windows), and observe charts.
4. Test dry/wet behavior: lift probe (dry) should turn pump ON after stability; immerse probe (wet) should turn pump OFF after stability.

## Safety & Tips
- Never power the pump directly from the Uno. Use the relay/power supply rated for the pump.
- Add a diode or proper relay/motor driver to protect from inductive kick if using a transistor.
- Keep sensors and wiring dry where needed; ensure good common ground.
- If relay toggles rapidly, increase stability times, adjust hysteresis, or increase smoothing factor in `main.cpp`.

## Future Improvements
- Replace resistive moisture sensors with capacitive ones for long‑term stability (resistive probes can corrode over time).
- Add scheduling (watering windows) or volume control (time‑based dosing).
- Integrate a simple “Flowchart” viewer (`System Flowchart/flowchart.html`) for documentation.

## Folder Structure (key paths)
- Firmware: `elefant/src/main.cpp`
- Plotter: `fan_plotter/index.html`, `fan_plotter/src/main.js`
- Docs: `System Flowchart/` (Mermaid diagram + viewer), `data/` (this file and other assets)

## References
- Automated plant watering ideas and rationale: [Automated plant watering ideas](https://chatgpt.com/share/68fb8dd4-b8a8-8006-bf74-2072b81ea16a)