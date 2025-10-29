import { SmoothieChart, TimeSeries } from "smoothie";
import "./style.css";

// Get DOM elements
const btnConnect = document.getElementById("connect");
const btnDisconnect = document.getElementById("disconnect");
const statusText = document.getElementById("statusText");

const currentTemp = document.getElementById("currentTemp");
const currentPot = document.getElementById("currentPot");
const currentServo = document.getElementById("currentServo");
const currentMotor = document.getElementById("currentMotor");
// Add new UI hooks
const currentHumidity = document.getElementById("currentHumidity");

// Global variables
let port = null;
let reader = null;
let latestValues = {
  temp: 0,
  pot: 0,
  servo: 0,
  motor: 0,
  humidity: 0

};
let pumpOn = false;

function makeChart(canvasId, color, minValue, maxValue, label) {
  const chart = new SmoothieChart({
    millisPerPixel: 20,
    interpolation: "linear",
    grid: {
      fillStyle: "#ffffff",
      strokeStyle: "#e0e0e0",
      millisPerLine: 2000,
      verticalSections: 4,
    },
    labels: {
      fillStyle: "#333333",
      fontSize: 12,
    },
    tooltip: true,
    minValue: minValue,
    maxValue: maxValue,
    timestampFormatter: SmoothieChart.timeFormatter,
  });

  const canvas = document.getElementById(canvasId);
  chart.streamTo(canvas, 500);

  const series = new TimeSeries();
  chart.addTimeSeries(series, {
    lineWidth: 3,
    strokeStyle: color,
    fillStyle: color + "20",
  });

  return series;
}

// Create all charts
const sTemp = makeChart("chartTemp", "#e74c3c", -50, 50, "Temperature");
const sPot = makeChart("chartPot", "#3498db", 0, 1023, "Soil Moisture");
// Add new charts
const sHumidity = makeChart("chartHumidity", "#8e44ad", 0, 100, "Humidity");
const sServo = makeChart("chartServo", "#2ecc71", 0, 180, "Servo");
const sMotor = makeChart("chartMotor", "#f39c12", 0, 255, "Motor");



// Update charts periodically
setInterval(() => {
  const now = Date.now();
  sTemp.append(now, latestValues.temp);
  sHumidity.append(now, latestValues.humidity);
  sPot.append(now, latestValues.pot);
  sServo.append(now, latestValues.servo);
  sMotor.append(now, latestValues.motor);

  currentTemp.textContent = `Temperature: ${latestValues.temp.toFixed(1)} \u00B0C`;
  currentHumidity.textContent = `Humidity: ${latestValues.humidity.toFixed(0)} %`;
  currentPot.textContent = `Soil Moisture (ADC): ${latestValues.pot} ADC`;
  currentServo.textContent = `Servo Angle: ${latestValues.servo}\u00B0`;
  currentMotor.textContent = `Motor / Pump PWM: ${latestValues.motor} PWM ${pumpOn ? "(ON)" : "(OFF)"}`;
}, 100);

btnConnect.addEventListener("click", async () => {
  try {
    if (!("serial" in navigator)) {
      alert("This browser does not support Web Serial. Use Chrome or Edge.");
      return;
    }

    const prevPorts = await navigator.serial.getPorts();
    if (prevPorts && prevPorts.length === 1) {
      port = prevPorts[0];
    } else {
      const filters = [
        { usbVendorId: 0x0483 },
        { usbVendorId: 0x1A86 },
        { usbVendorId: 0x10C4 },
        { usbVendorId: 0x2341 },
        { usbVendorId: 0x2E8A }, // Raspberry Pi RP2350
      ];
      port = await navigator.serial.requestPort({ filters });
    }

    await port.open({ baudRate: 115200 });

    statusText.textContent = "Connected";
    statusText.classList.add("connected");
    btnConnect.disabled = true;
    btnDisconnect.disabled = false;

    readSerialData();
  } catch (err) {
    if (err && (err.name === "NotFoundError" || /No port selected/i.test(err.message))) {
      statusText.textContent = "No device selected or detected — check drivers in Device Manager.";
      statusText.classList.remove("connected");
      btnConnect.disabled = false;
      btnDisconnect.disabled = true;
      return;
    }
    if (err && (/Failed to open/i.test(err.message) || err.name === "NetworkError")) {
      statusText.textContent = "Port busy or access denied — close other apps.";
      alert("Port busy or access denied. Close other apps using the COM port.");
      console.error(err);
      return;
    }
    alert("Error connecting to serial port: " + err.message);
    console.error(err);
  }
});

btnDisconnect.addEventListener("click", async () => {
  if (reader) {
    await reader.cancel();
    reader = null;
  }

  if (port) {
    await port.close();
    port = null;
  }

  statusText.textContent = "Disconnected";
  statusText.classList.remove("connected");
  btnConnect.disabled = false;
  btnDisconnect.disabled = true;
});

async function readSerialData() {
  const decoder = new TextDecoder();
  let buffer = "";

  while (port && port.readable) {
    reader = port.readable.getReader();
    try {
      while (true) {
        const { value, done } = await reader.read();
        if (done) break;
        if (!value) continue;

        buffer += decoder.decode(value, { stream: true });
        const lines = buffer.split(/\r?\n/);
        buffer = lines.pop() ?? "";

        for (const line of lines) {
          if (!line) continue;
          parseStatusLine(line);
        }
      }
    } catch (err) {
      console.error("Error reading serial data:", err);
      statusText.textContent = "Error: " + err.message;
      statusText.classList.remove("connected");
    } finally {
      const tail = decoder.decode();
      if (tail) buffer += tail;
      reader.releaseLock();
    }
  }
}

function parseStatusLine(line) {
  try {
    // Updated regex patterns to match the Arduino output format
    const tempMatch = line.match(/Temp:\s*(-?\d+(?:\.\d+)?)°?C?/i);
    const humidityMatch = line.match(/Humidity:\s*(\d+(?:\.\d+)?)%?/i);
    const potMatch = line.match(/Pot:\s*(\d+)/i);
    const servoMatch = line.match(/Servo:\s*(\d+)°?/i);
    const motorMatch = line.match(/Motor:\s*(\d+)/i);
    const statusMatch = line.match(/\|\s*(ON|OFF)\s*$/i);

    if (tempMatch) {
      latestValues.temp = parseFloat(tempMatch[1]);
      console.log("Parsed temp:", latestValues.temp);
    }
    if (humidityMatch) {
      latestValues.humidity = parseFloat(humidityMatch[1]);
      console.log("Parsed humidity:", latestValues.humidity);
    }
    if (potMatch) {
      latestValues.pot = parseInt(potMatch[1], 10);
      console.log("Parsed pot:", latestValues.pot);
    }
    if (servoMatch) {
      latestValues.servo = parseInt(servoMatch[1], 10);
      console.log("Parsed servo:", latestValues.servo);
    }
    if (motorMatch) {
      latestValues.motor = parseInt(motorMatch[1], 10);
      console.log("Parsed motor:", latestValues.motor);
    }
    if (statusMatch) {
      pumpOn = /ON/i.test(statusMatch[1]);
      console.log("Parsed pump status:", pumpOn);
    } else {
      pumpOn = latestValues.motor > 0;
    }

    // Log the full line for debugging
    console.log("Full line:", line);
  } catch (err) {
    console.error("Error parsing line:", line, err);
  }
}
