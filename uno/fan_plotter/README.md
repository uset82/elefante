# 🌀 Mini Fan Project - Real-Time Serial Plotter

Web-based serial plotter for visualizing data from the STM32F767ZI Mini Fan Project in real-time.

## 📋 Features

- **Real-time visualization** of 4 data streams:
  - 🌡️ Temperature sensor (TMP36)
  - 🎛️ Potentiometer ADC value
  - 🔄 Servo motor angle
  - ⚡ DC motor PWM speed

- **WebSerial API** for direct browser-to-serial communication
- **SmoothieChart** for smooth, responsive graphs
- **Color-coded charts** for easy data identification

## 🚀 Quick Start

### Prerequisites

- Node.js and npm installed
- STM32F767ZI Nucleo board connected via USB
- Chrome or Edge browser (WebSerial API support required)

### Installation

```bash
# Navigate to project folder
cd fan_plotter

# Install dependencies (already done)
npm install

# Start development server
npm run dev
```

The app will open at `http://localhost:5173` (or another port if 5173 is busy).

## 📖 How to Use

1. **Upload the STM32 code** to your Nucleo board (from `exercise2/src/main.cpp`)

2. **Open the web plotter** in Chrome or Edge browser

3. **Click "Connect to Serial Port"** button

4. **Select your STM32 board** from the list (usually "STMicroelectronics STLink Virtual COM Port")

5. **Watch the data flow!** The charts will start updating in real-time

6. **Test the system:**
   - Rotate the potentiometer → Servo angle changes
   - Touch the TMP36 sensor → Temperature changes (motor should activate above 20°C)

## 🎨 Chart Colors

- 🔴 **Red** - Temperature (°C)
- 🔵 **Blue** - Potentiometer (ADC 0-4095)
- 🟢 **Green** - Servo Angle (0-180°)
- 🟠 **Orange** - Motor Speed (PWM 0-255)

## 📊 Data Format

The STM32 sends data in this format:
```
Temp: -33.0°C | Pot: 570 | Servo: 25° | Motor: 0 | OFF
```

The plotter automatically parses this format and extracts the values.

## 🔧 Technical Details

- **Framework:** Vite (fast modern build tool)
- **Chart Library:** SmoothieChart.js (60 FPS canvas-based charts)
- **Communication:** WebSerial API (Chrome/Edge only)
- **Baud Rate:** 115200
- **Update Rate:** 10 Hz (100ms intervals)

## 🐛 Troubleshooting

### "Connect to Serial Port" button doesn't work
- ✅ Use Chrome or Edge browser (Firefox doesn't support WebSerial yet)
- ✅ Make sure your board is connected via USB
- ✅ Close any other serial monitor (Arduino IDE, PlatformIO, etc.)

### Charts not updating
- ✅ Check that the STM32 code is running (LED should blink)
- ✅ Verify baud rate is 115200 in both STM32 code and web plotter
- ✅ Look at browser console (F12) for any error messages

### Temperature showing -33°C
- ✅ Check TMP36 wiring: Left pin = 3.3V, Middle = PA3, Right = GND
- ✅ Touch the sensor with your finger - value should increase
- ✅ Verify PA3 connection with multimeter

## 📁 Project Structure

```
fan_plotter/
├── index.html          # Main HTML page
├── package.json        # npm dependencies
├── src/
│   ├── main.js         # JavaScript logic (WebSerial + charts)
│   └── style.css       # Styling
└── README.md          # This file
```

## 🎓 Assignment Notes

This plotter helps with:
- **Debugging** sensor readings in real-time
- **Testing** motor response to temperature changes
- **Demonstrating** the full system functionality
- **Documentation** - take screenshots for your report!

## 📝 Author

ELE201 - Assignment 3: Mini Fan Project  
STM32F767ZI Nucleo Board  
October 2025

---

💡 **Tip:** Press F12 in your browser to open Developer Tools and see the parsed data in the console!
