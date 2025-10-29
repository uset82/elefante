# Tutorial: Cómo crear tu propio Serial Plotter para STM32

## 📚 Tabla de Contenidos
1. [Introducción](#introducción)
2. [Prerequisites](#prerequisites)
3. [Configuración del Proyecto](#configuración-del-proyecto)
4. [Estructura de Archivos](#estructura-de-archivos)
5. [Código HTML](#código-html)
6. [Código CSS](#código-css)
7. [Código JavaScript](#código-javascript)
8. [Cómo Usar](#cómo-usar)
9. [Troubleshooting](#troubleshooting)

---

## 📖 Introducción

En este tutorial, vamos a crear un **serial plotter en el navegador** que lee datos en tiempo real desde tu placa STM32F767ZI Nucleo y los visualiza en gráficos interactivos.

### ¿Qué vamos a construir?
- ✅ Una aplicación web que se conecta directamente al puerto serial de tu STM32
- ✅ 4 gráficos en tiempo real para temperatura, potenciómetro, servo y motor
- ✅ Sin necesidad de software adicional (solo Chrome/Edge)
- ✅ Visualización fluida a 60 FPS con SmoothieCharts

### Tecnologías utilizadas
- **Vite** - Build tool moderno y rápido
- **SmoothieChart.js** - Biblioteca para gráficos en tiempo real
- **WebSerial API** - API del navegador para comunicación serial
- **Vanilla JavaScript** - Sin frameworks pesados

---

## 🔧 Prerequisites

Antes de comenzar, necesitas tener instalado:

1. **Node.js y npm** 
   - Descarga desde: https://nodejs.org/
   - Verifica instalación: `node --version` y `npm --version`

2. **Chrome o Edge browser**
   - Necesario para WebSerial API
   - Firefox no soporta WebSerial todavía

3. **STM32 con código funcionando**
   - Tu placa debe enviar datos por serial a 115200 baudios
   - Formato: `Temp: -33.0°C | Pot: 570 | Servo: 25° | Motor: 0 | OFF`

---

## 🏗️ Configuración del Proyecto

### Paso 1: Crear carpeta del proyecto

```powershell
# En Windows PowerShell
cd D:\HVL2025\microcontroller\tarea3
mkdir fan_plotter
cd fan_plotter
```

### Paso 2: Inicializar proyecto npm

```powershell
npm init -y
```

Esto crea un archivo `package.json` con la configuración básica.

### Paso 3: Instalar dependencias

```powershell
# Instalar Vite como herramienta de desarrollo
npm install --save-dev vite

# Instalar SmoothieChart para gráficos
npm install smoothie
```

### Paso 4: Modificar package.json

Abre `package.json` y modifícalo así:

```json
{
  "name": "fan_plotter",
  "version": "1.0.0",
  "description": "Real-time serial plotter for STM32 Mini Fan Project",
  "main": "index.js",
  "type": "module",
  "scripts": {
    "dev": "vite",
    "build": "vite build",
    "preview": "vite preview"
  },
  "keywords": ["stm32", "serial", "plotter"],
  "author": "Tu Nombre",
  "license": "ISC",
  "devDependencies": {
    "vite": "^7.1.11"
  },
  "dependencies": {
    "smoothie": "^1.36.1"
  }
}
```

**Importante:** El campo `"type": "module"` permite usar imports ES6.

---

## 📁 Estructura de Archivos

Crea la siguiente estructura:

```
fan_plotter/
├── index.html          # Página principal
├── package.json        # Configuración npm
├── src/
│   ├── main.js         # Lógica JavaScript
│   └── style.css       # Estilos
└── README.md          # Documentación
```

Crea la carpeta `src`:

```powershell
mkdir src
```

---

## 📄 Código HTML

Crea el archivo `index.html` en la raíz del proyecto:

```html
<!DOCTYPE html>
<html lang="en">
  <head>
    <meta charset="UTF-8" />
    <meta name="viewport" content="width=device-width, initial-scale=1.0" />
    <title>Fan Control Serial Plotter</title>
  </head>
  <body>
    <div id="app">
      <h1>🌀 Mini Fan Project - Real-Time Serial Plotter</h1>
      <p>STM32F767ZI Nucleo | TMP36 Sensor | Servo Motor | DC Motor</p>
      
      <button id="connect">🔌 Connect to Serial Port</button>
      <button id="disconnect" disabled>❌ Disconnect</button>
      
      <div id="status">Status: <span id="statusText">Disconnected</span></div>
      
      <div class="charts">
        <div class="chart-container">
          <h2>🌡️ Temperature (°C)</h2>
          <canvas id="chartTemp" width="800" height="200"></canvas>
          <p id="currentTemp">Current: -- °C</p>
        </div>
        
        <div class="chart-container">
          <h2>🎛️ Potentiometer (ADC Value)</h2>
          <canvas id="chartPot" width="800" height="200"></canvas>
          <p id="currentPot">Current: -- ADC</p>
        </div>
        
        <div class="chart-container">
          <h2>🔄 Servo Angle (degrees)</h2>
          <canvas id="chartServo" width="800" height="200"></canvas>
          <p id="currentServo">Current: -- °</p>
        </div>
        
        <div class="chart-container">
          <h2>⚡ Motor Speed (PWM)</h2>
          <canvas id="chartMotor" width="800" height="200"></canvas>
          <p id="currentMotor">Current: -- PWM</p>
        </div>
      </div>
    </div>
    <script type="module" src="/src/main.js"></script>
  </body>
</html>
```

### Explicación del HTML:

1. **Botones de conexión**: `connect` y `disconnect` para controlar el puerto serial
2. **Status display**: Muestra si está conectado o no
3. **4 contenedores de gráficos**: Uno para cada señal (temp, pot, servo, motor)
4. **Canvas elements**: Donde se dibujan los gráficos
5. **Current value displays**: Muestran el valor actual de cada señal
6. **Script module**: Importa el JavaScript con `type="module"`

---

## 🎨 Código CSS

Crea el archivo `src/style.css`:

```css
body {
  margin: 0;
  padding: 20px;
  font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
  background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
  color: #333;
  min-height: 100vh;
}

#app {
  max-width: 1200px;
  margin: 0 auto;
  background: white;
  border-radius: 20px;
  padding: 30px;
  box-shadow: 0 20px 60px rgba(0,0,0,0.3);
}

h1 {
  color: #667eea;
  text-align: center;
  margin-bottom: 10px;
  font-size: 2.5em;
}

p {
  text-align: center;
  color: #666;
  margin-bottom: 30px;
}

button {
  padding: 15px 30px;
  font-size: 16px;
  border: none;
  border-radius: 10px;
  cursor: pointer;
  transition: all 0.3s ease;
  font-weight: bold;
  margin: 10px;
}

#connect {
  background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
  color: white;
}

#connect:hover:not(:disabled) {
  transform: translateY(-2px);
  box-shadow: 0 5px 20px rgba(102, 126, 234, 0.4);
}

#disconnect {
  background: #e74c3c;
  color: white;
}

#disconnect:hover:not(:disabled) {
  background: #c0392b;
  transform: translateY(-2px);
}

button:disabled {
  opacity: 0.5;
  cursor: not-allowed;
}

#status {
  text-align: center;
  padding: 15px;
  border-radius: 10px;
  background: #f8f9fa;
  margin: 20px 0;
  font-size: 18px;
  font-weight: bold;
}

#statusText {
  color: #e74c3c;
}

#statusText.connected {
  color: #27ae60;
}

.charts {
  margin-top: 30px;
}

.chart-container {
  margin-bottom: 40px;
  padding: 20px;
  border-radius: 15px;
  background: #f8f9fa;
  box-shadow: 0 5px 15px rgba(0,0,0,0.1);
}

.chart-container h2 {
  color: #667eea;
  margin-top: 0;
  margin-bottom: 15px;
  font-size: 1.5em;
}

.chart-container p {
  text-align: left;
  font-size: 16px;
  margin-top: 10px;
  color: #333;
  font-weight: bold;
}

canvas {
  width: 100%;
  height: auto;
  display: block;
  border-radius: 10px;
  background: white;
}
```

### Puntos clave del CSS:

- **Gradient background**: Fondo morado degradado
- **Card design**: Contenedor blanco con sombra y bordes redondeados
- **Hover effects**: Animaciones suaves en los botones
- **Responsive canvas**: Los gráficos se adaptan al ancho de la pantalla
- **Color coding**: Rojo para desconectado, verde para conectado

---

## 💻 Código JavaScript

Crea el archivo `src/main.js`:

```javascript
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

// Global variables
let port = null;
let reader = null;
let latestValues = {
  temp: 0,
  pot: 0,
  servo: 0,
  motor: 0
};

// Create charts with different colors
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
    fillStyle: color + "20", // Add transparency
  });

  return series;
}

// Create all charts
const sTemp = makeChart("chartTemp", "#e74c3c", -50, 50, "Temperature");
const sPot = makeChart("chartPot", "#3498db", 0, 4200, "Potentiometer");
const sServo = makeChart("chartServo", "#2ecc71", 0, 180, "Servo");
const sMotor = makeChart("chartMotor", "#f39c12", 0, 255, "Motor");

// Update charts periodically
setInterval(() => {
  const now = Date.now();
  sTemp.append(now, latestValues.temp);
  sPot.append(now, latestValues.pot);
  sServo.append(now, latestValues.servo);
  sMotor.append(now, latestValues.motor);

  // Update current value displays
  currentTemp.textContent = `Current: ${latestValues.temp.toFixed(1)} °C`;
  currentPot.textContent = `Current: ${latestValues.pot} ADC`;
  currentServo.textContent = `Current: ${latestValues.servo}°`;
  currentMotor.textContent = `Current: ${latestValues.motor} PWM`;
}, 100);

// Connect button handler
btnConnect.addEventListener("click", async () => {
  try {
    // Request serial port
    port = await navigator.serial.requestPort();
    await port.open({ baudRate: 115200 });

    statusText.textContent = "Connected";
    statusText.classList.add("connected");
    btnConnect.disabled = true;
    btnDisconnect.disabled = false;

    // Start reading data
    readSerialData();
  } catch (err) {
    alert("Error connecting to serial port: " + err.message);
    console.error(err);
  }
});

// Disconnect button handler
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

// Read serial data function
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

        // Accumulate and split on newline
        buffer += decoder.decode(value, { stream: true });
        const lines = buffer.split(/\r?\n/);
        buffer = lines.pop() ?? "";

        for (const line of lines) {
          if (!line) continue;

          // Parse the line format
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

// Parse the status line from STM32
function parseStatusLine(line) {
  try {
    // Format: "Temp: -33.0°C | Pot: 570 | Servo: 25° | Motor: 0 | OFF"
    
    const tempMatch = line.match(/Temp:\s*(-?\d+\.?\d*)°C/);
    const potMatch = line.match(/Pot:\s*(\d+)/);
    const servoMatch = line.match(/Servo:\s*(\d+)°/);
    const motorMatch = line.match(/Motor:\s*(\d+)/);

    if (tempMatch) {
      latestValues.temp = parseFloat(tempMatch[1]);
    }
    if (potMatch) {
      latestValues.pot = parseInt(potMatch[1]);
    }
    if (servoMatch) {
      latestValues.servo = parseInt(servoMatch[1]);
    }
    if (motorMatch) {
      latestValues.motor = parseInt(motorMatch[1]);
    }

    console.log("Parsed:", latestValues);
  } catch (err) {
    console.error("Error parsing line:", line, err);
  }
}
```

### Explicación del código JavaScript:

#### 1. **Imports**
```javascript
import { SmoothieChart, TimeSeries } from "smoothie";
```
- Importa las clases necesarias de SmoothieChart

#### 2. **makeChart() Function**
```javascript
function makeChart(canvasId, color, minValue, maxValue, label)
```
- Crea un gráfico con configuración personalizada
- Retorna un objeto `TimeSeries` donde agregaremos datos

#### 3. **WebSerial Connection**
```javascript
port = await navigator.serial.requestPort();
await port.open({ baudRate: 115200 });
```
- `requestPort()`: Muestra diálogo para seleccionar puerto
- `open()`: Abre la conexión con el baudrate especificado

#### 4. **Reading Data**
```javascript
const decoder = new TextDecoder();
buffer += decoder.decode(value, { stream: true });
const lines = buffer.split(/\r?\n/);
```
- Decodifica bytes a texto
- Acumula en buffer hasta encontrar línea completa
- Divide por saltos de línea

#### 5. **Parsing Data**
```javascript
const tempMatch = line.match(/Temp:\s*(-?\d+\.?\d*)°C/);
```
- Usa expresiones regulares para extraer valores
- Convierte strings a números con `parseFloat()` / `parseInt()`

#### 6. **Updating Charts**
```javascript
setInterval(() => {
  sTemp.append(now, latestValues.temp);
}, 100);
```
- Agrega datos cada 100ms (10 Hz)
- `append()` añade punto al gráfico con timestamp

---

## 🚀 Cómo Usar

### 1. Inicia el servidor de desarrollo

```powershell
npm run dev
```

Verás algo como:
```
  VITE v7.1.11  ready in 279 ms

  ➜  Local:   http://localhost:5173/
  ➜  Network: use --host to expose
```

### 2. Abre el navegador

- Ve a `http://localhost:5173/`
- Debe funcionar en Chrome o Edge

### 3. Conecta tu STM32

- Asegúrate de que tu placa está conectada por USB
- El código debe estar corriendo y enviando datos

### 4. Haz clic en "Connect to Serial Port"

- Selecciona "STMicroelectronics STLink Virtual COM Port"
- Si no aparece, verifica que:
  - La placa esté conectada
  - Los drivers estén instalados
  - No haya otro programa usando el puerto serial

### 5. ¡Observa los datos!

- Los gráficos comenzarán a actualizarse automáticamente
- Gira el potenciómetro → El ángulo del servo cambia
- Toca el sensor de temperatura → La temperatura sube

---

## 🐛 Troubleshooting

### ❌ Error: "WebSerial API not supported"

**Solución:**
- Usa Chrome o Edge (versión actualizada)
- Firefox no soporta WebSerial todavía

### ❌ No aparece ningún puerto al hacer clic en "Connect"

**Solución:**
- Verifica que la placa esté conectada por USB
- Revisa en Device Manager (Administrador de Dispositivos)
- Reinstala drivers ST-Link

### ❌ Los gráficos no se actualizan

**Solución:**
- Abre la consola del navegador (F12)
- Busca mensajes de error
- Verifica que el STM32 esté enviando datos correctamente
- Confirma que el baudrate sea 115200 en ambos lados

### ❌ Error: "Failed to open serial port"

**Solución:**
- Cierra cualquier otro programa que use el puerto serial:
  - PlatformIO Serial Monitor
  - Arduino IDE
  - PuTTY
  - Tera Term
- Solo un programa puede usar el puerto a la vez

### ❌ Temperatura muestra -33°C constantemente

**Solución:**
- Este es un problema de hardware, no del plotter
- Verifica las conexiones del TMP36:
  - Pin izquierdo → 3.3V
  - Pin central → PA3
  - Pin derecho → GND
- Toca el sensor con tu dedo → Debe cambiar el valor

---

## 📊 Formato de Datos

El plotter espera datos en este formato:

```
Temp: -33.0°C | Pot: 570 | Servo: 25° | Motor: 0 | OFF
```

### Si quieres cambiar el formato:

Modifica la función `parseStatusLine()` en `main.js`:

```javascript
function parseStatusLine(line) {
  // Para formato CSV: "25.5,1024,90,128"
  const values = line.split(',');
  latestValues.temp = parseFloat(values[0]);
  latestValues.pot = parseInt(values[1]);
  latestValues.servo = parseInt(values[2]);
  latestValues.motor = parseInt(values[3]);
}
```

Y actualiza tu código STM32 para enviar datos en formato CSV:

```cpp
void printStatus(float temp, int pot, int servo, int motor) {
  Serial.print(temp, 1);
  Serial.print(",");
  Serial.print(pot);
  Serial.print(",");
  Serial.print(servo);
  Serial.print(",");
  Serial.println(motor);
}
```

---

## 🎯 Próximos Pasos

### Mejoras opcionales:

1. **Agregar controles de pausa**
   ```javascript
   let isPaused = false;
   btnPause.addEventListener("click", () => {
     isPaused = !isPaused;
   });
   ```

2. **Exportar datos a CSV**
   ```javascript
   function exportToCSV() {
     let csv = "Time,Temp,Pot,Servo,Motor\n";
     // ... agregar datos
     const blob = new Blob([csv], { type: 'text/csv' });
     const url = URL.createObjectURL(blob);
     // ... descargar
   }
   ```

3. **Agregar alertas de umbral**
   ```javascript
   if (latestValues.temp > 40) {
     alert("¡Temperatura muy alta!");
   }
   ```

4. **Cambiar colores de gráficos dinámicamente**
   ```javascript
   if (latestValues.motor > 200) {
     // Cambiar color a rojo cuando motor está al máximo
   }
   ```

---

## 📖 Recursos Adicionales

- **WebSerial API Docs**: https://developer.mozilla.org/en-US/docs/Web/API/Web_Serial_API
- **SmoothieChart Docs**: http://smoothiecharts.org/
- **Vite Guide**: https://vitejs.dev/guide/
- **Regex Tutorial**: https://regexr.com/

---

## 👨‍💻 Créditos

Tutorial creado para ELE201 - Assignment 3  
STM32F767ZI Nucleo Board Mini Fan Project  
October 2025

---

## 💡 Tips Finales

1. **Consola de desarrollador es tu amiga**: Presiona F12 para ver los datos parseados
2. **Ajusta la velocidad de gráficos**: Cambia `millisPerPixel` en `makeChart()`
3. **Guarda screenshots para tu reporte**: Muestra los gráficos funcionando
4. **Experimenta con colores**: Cambia los códigos de color hex en `makeChart()`
5. **Comparte tu proyecto**: Sube a GitHub para tu portafolio

---

¡Felicidades! 🎉 Ahora tienes tu propio serial plotter personalizado funcionando en el navegador.
