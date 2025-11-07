# 🔍 GUÍA DE DIAGNÓSTICO - Sistema Hidropónico

## ❌ PROBLEMA DETECTADO: Sensores sin datos

Según el monitor serial:
- ❌ **pH: 20.89** - Valor imposible (sensor desconectado)
- ❌ **TDS: 0 ppm** - Sensor desconectado
- ⚠️ **LDR: 0** - Sin luz (puede ser normal)

---

## 🔧 SOLUCIÓN 1: Verificar Conexiones Físicas

### **SENSOR pH (PH4502C)**

**Conexiones requeridas:**
```
ESP32 GPIO32 (Pin ADC) ──→ Po (señal) del PH4502C
ESP32 GND             ──→ GND del sensor
Fuente 5V             ──→ VCC del sensor
```

**¿Cómo verificar?**
1. ✅ El LED del sensor PH4502C debe estar ENCENDIDO (rojo/verde)
2. ✅ Con un multímetro, medir entre Po y GND: debería dar 1.5V - 3.0V
3. ✅ Verificar que el cable esté bien conectado a GPIO32

**Si no tienes el sensor físicamente:**
- Comentar las líneas relacionadas con pH en el código

---

### **SENSOR TDS (SEN0244)**

**Conexiones requeridas:**
```
ESP32 GPIO33 (Pin ADC) ──→ Señal (cable amarillo/naranja)
ESP32 GND             ──→ GND (cable negro)
Fuente 5V             ──→ VCC (cable rojo)
```

**¿Cómo verificar?**
1. ✅ Con multímetro entre señal y GND: debería dar 0.5V - 3.0V
2. ✅ El sensor debe estar sumergido en agua para dar lectura válida
3. ✅ Verificar que esté conectado a GPIO33

**Valores normales:**
- **Agua destilada**: 0-50 ppm
- **Agua de grifo**: 100-400 ppm  
- **Solución nutritiva**: 800-1500 ppm

---

### **SENSOR LDR (Luz)**

**Conexiones:**
```
ESP32 GPIO35 ──→ Señal del LDR
```

Este sensor puede dar 0 si está en la oscuridad (es normal).

---

## 🔧 SOLUCIÓN 2: Ejecutar Programa de Diagnóstico

He creado un programa especial de prueba: `test/test_sensores.cpp`

**Para ejecutarlo:**

1. **Detener el monitor actual**: Presiona `Ctrl+C` en el terminal

2. **Temporalmente, renombrar el main.cpp**:
```powershell
mv src\main.cpp src\main.cpp.backup
mv test\test_sensores.cpp src\main.cpp
```

3. **Compilar y subir**:
```powershell
pio run --target upload
```

4. **Abrir monitor**:
```powershell
pio device monitor --port COM4 --baud 115200
```

5. **Ver diagnóstico detallado** - Te mostrará:
   - Valores ADC crudos (0-4095)
   - Voltajes exactos
   - Estado de cada sensor
   - Sugerencias de problemas

6. **Restaurar el código original**:
```powershell
mv src\main.cpp test\test_sensores.cpp
mv src\main.cpp.backup src\main.cpp
```

---

## 🔧 SOLUCIÓN 3: Modo de Prueba Sin Sensores

Si NO tienes los sensores físicamente conectados, puedes modificar el código para usar valores de prueba:

**Editar `src/main.cpp`** y agregar al inicio del `loop()`:

```cpp
void loop() {
    // ===== MODO DE PRUEBA SIN SENSORES =====
    // Descomentar estas líneas si NO tienes sensores conectados
    
    // Simular pH normal
    // phFiltered = 6.5 + (random(-10, 10) / 100.0);
    
    // Simular TDS normal  
    // tdsValue = 1000 + random(-50, 50);
    
    // =======================================
    
    // ... resto del código ...
}
```

---

## 📊 SOLUCIÓN 4: Verificar Dashboard Web

**El dashboard web SÍ está recibiendo datos de Firebase**, pero muestra los valores incorrectos de los sensores.

Para verificar el dashboard:

1. **Abrir otra terminal PowerShell**

2. **Ir al directorio frontend**:
```powershell
cd c:\Users\ANDRES\Desktop\clone\hydroponic-monitoring\frontend
```

3. **Instalar dependencias** (si no está hecho):
```powershell
npm install
```

4. **Ejecutar el servidor de desarrollo**:
```powershell
npm run dev
```

5. **Abrir navegador en**:
```
http://localhost:3000
```

Deberías ver los datos que el ESP32 está enviando a Firebase (aunque sean incorrectos por los sensores desconectados).

---

## 🎯 RESUMEN DE ACCIONES

### ✅ **Acción Inmediata**

1. **Verificar conexiones físicas** de los sensores (ver arriba)
2. **Verificar alimentación 5V** para los sensores
3. **Ejecutar programa de diagnóstico** para ver valores crudos

### ✅ **Si NO tienes sensores**

1. Modificar código para usar valores simulados
2. O comentar las líneas de lectura de sensores

### ✅ **Para el Dashboard**

El dashboard funciona correctamente, solo necesita que el ESP32 envíe datos válidos.

---

## 📞 SIGUIENTE PASO

**¿Qué quieres hacer?**

A. 🔌 Verificar conexiones físicas (te guío paso a paso)
B. 🧪 Ejecutar programa de diagnóstico  
C. 🎮 Activar modo simulación (sin sensores físicos)
D. 🌐 Ver el dashboard web
E. ❓ Otro problema

**Responde con la letra de la opción que prefieres.**
