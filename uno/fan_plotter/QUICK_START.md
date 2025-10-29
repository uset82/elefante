# 🚀 Quick Start Guide - Serial Plotter

## ✅ El servidor ya está corriendo!

Tu serial plotter está activo en:
👉 **http://localhost:5173/**

---

## 📋 Pasos para usar:

### 1. Abre el navegador
- Abre **Chrome** o **Edge** (no Firefox)
- Ve a: http://localhost:5173/

### 2. Conecta tu STM32
- Verifica que tu placa Nucleo esté conectada por USB
- Asegúrate de que el código esté corriendo (LED parpadeando)
- **IMPORTANTE**: Cierra el Serial Monitor de PlatformIO si está abierto

### 3. Haz clic en "Connect to Serial Port"
- Selecciona: **STMicroelectronics STLink Virtual COM Port**
- Si no aparece, desconecta y reconecta el USB

### 4. ¡Disfruta!
- Los gráficos comenzarán a actualizarse automáticamente
- 🌡️ Rojo = Temperatura
- 🔵 Azul = Potenciómetro  
- 🟢 Verde = Servo
- 🟠 Naranja = Motor

---

## 🧪 Prueba el sistema:

1. **Gira el potenciómetro** → El ángulo del servo cambia (24-25°)
2. **Toca el sensor TMP36** con tus dedos → La temperatura debe subir
3. **Calienta el sensor** → El motor debería activarse cuando temp > 20°C

---

## 🐛 Problemas comunes:

### No aparece el puerto serial?
```powershell
# Verifica que el monitor serial NO esté abierto
# En VSCode, cierra la terminal que dice "PlatformIO: Monitor"
```

### Los gráficos no se actualizan?
- Presiona **F12** para abrir Developer Tools
- Ve a la pestaña **Console**
- Deberías ver: `Parsed: {temp: -33.6, pot: 567, servo: 24, motor: 0}`

### Dice "WebSerial not supported"?
- ❌ Estás usando Firefox → Usa Chrome o Edge
- ✅ Chrome versión 89+ o Edge versión 89+

---

## 🎯 Comandos útiles:

### Para detener el servidor:
```powershell
# Presiona Ctrl+C en la terminal donde corre npm run dev
```

### Para reiniciar el servidor:
```powershell
cd D:\HVL2025\microcontroller\tarea3\fan_plotter
npm run dev
```

### Para construir versión de producción:
```powershell
npm run build
# Genera archivos en carpeta dist/
```

---

## 📸 Screenshots para tu reporte:

1. Abre el plotter en el navegador
2. Conecta el STM32
3. Deja correr por 30 segundos
4. Presiona **Windows + Shift + S** para capturar pantalla
5. Guarda la imagen para tu documentación

---

## 🔗 Recursos:

- **Tutorial completo**: `D:\HVL2025\microcontroller\tarea3\TUTORIAL_SERIAL_PLOTTER.md`
- **README del proyecto**: `D:\HVL2025\microcontroller\tarea3\fan_plotter\README.md`
- **Código fuente**: `D:\HVL2025\microcontroller\tarea3\fan_plotter\src\`

---

## 💡 Pro Tips:

1. **Abre la consola del navegador (F12)** para ver los datos parseados en tiempo real
2. **Usa el scroll del mouse** sobre los gráficos para hacer zoom
3. **Cambia los colores** editando los valores hex en `src/main.js`
4. **Exporta datos**: Copia los valores de la consola para análisis

---

## ✨ Estado actual del sistema:

✅ Servidor Vite corriendo en puerto 5173  
✅ Archivos HTML, CSS, JS creados  
✅ Dependencias instaladas (smoothie, vite)  
✅ Tutorial completo disponible  

**Todo listo para usar! 🎉**

---

**Creado por**: ELE201 Assignment 3  
**Fecha**: October 21, 2025  
**Placa**: STM32F767ZI Nucleo Board
