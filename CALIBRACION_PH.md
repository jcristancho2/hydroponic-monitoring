# 📊 GUÍA COMPLETA DE CALIBRACIÓN DEL SENSOR pH

## 🎯 ¿Por qué calibrar?

El sensor de pH necesita calibración porque:
- Cada sensor es único (variaciones de fabricación)
- El electrodo se degrada con el tiempo
- La temperatura afecta las lecturas
- Sin calibración: **pH = 20.89** ❌ (como estás viendo ahora)
- Con calibración: **pH = 6.5** ✅ (valor correcto)

---

## 📦 ¿Qué necesitas?

### **Opción A: Calibración Profesional** ⭐ (Recomendado)
```
✅ Solución buffer pH 7.0 (neutro) - OBLIGATORIO
✅ Solución buffer pH 4.0 (ácido) - Recomendado
✅ Agua destilada para enjuague
✅ Vaso o recipiente limpio
```

**Dónde comprar:**
- Tiendas de hidroponía
- Amazon / MercadoLibre
- Farmacias grandes
- **Precio:** ~$5-10 USD por sobre

### **Opción B: Calibración Casera** 💡 (Aproximada)
Si NO tienes soluciones buffer, puedes usar:

**Buffer pH 7 (neutro):**
```
- Agua destilada pura = ~pH 7.0
- O mezclar agua de grifo + bicarbonato de sodio
```

**Buffer pH 4 (ácido):**
```
- Vinagre blanco diluido (1:4) = ~pH 4-5
- Jugo de limón diluido (1:3) = ~pH 4
```

⚠️ **NOTA:** Los valores caseros son aproximados, no precisos.

---

## 🔧 PROCESO DE CALIBRACIÓN

### **PASO 1: Preparar el Monitor Serial**

1. Conecta el ESP32 por USB
2. Abre el monitor serial:

```powershell
cd C:\Users\ANDRES\Desktop\clone\hydroponic-monitoring
.venv\Scripts\activate
pio device monitor --port COM4 --baud 115200
```

3. Verás mensajes como:
```
pH: 20.89 (0.000V) [No calibrado]
```

### **PASO 2: Verificar Comandos Disponibles**

Escribe en el monitor serial:
```
HELP
```

Deberías ver:
```
=== COMANDOS DISPONIBLES ===
Calibración pH:
  PHCAL,7    - Calibrar punto neutro (pH 7)
  PHCAL,4    - Calibrar punto ácido (pH 4)
  PHCAL,10   - Calibrar punto básico (pH 10)
  PHSAVE     - Guardar calibración
  PHRESET    - Restablecer calibración
...
```

### **PASO 3: Calibración en pH 7 (OBLIGATORIO)** ⭐

Este es el punto más importante:

1. **Preparar el sensor:**
   - Limpiar el electrodo con agua destilada
   - Secar suavemente con papel absorbente
   - NO frotar el bulbo de vidrio

2. **Sumergir en buffer pH 7:**
   ```
   - Verter solución buffer pH 7 en un vaso limpio
   - Sumergir COMPLETAMENTE el bulbo del electrodo
   - Agitar suavemente para eliminar burbujas
   - Esperar 30 segundos a que se estabilice
   ```

3. **Ejecutar comando de calibración:**
   ```
   PHCAL,7
   ```

4. **Verificar resultado:**
   ```
   PHSensor: Calibrado V@7=2.4856 V
   ```
   
   ✅ **Voltaje normal:** 2.2V - 2.8V
   ❌ **Si ves 0.0V:** Sensor desconectado
   ❌ **Si ves >3.0V:** Cable o conexión incorrecta

### **PASO 4: Calibración en pH 4 o pH 10 (Recomendado)**

Esto mejora la precisión en todo el rango:

**Para pH ácido (4.0):**

1. **Limpiar el electrodo:**
   ```
   - Enjuagar con agua destilada
   - Secar suavemente
   ```

2. **Sumergir en buffer pH 4:**
   ```
   - Usar solución buffer pH 4
   - Sumergir completamente el electrodo
   - Esperar 30 segundos
   ```

3. **Calibrar:**
   ```
   PHCAL,4
   ```

4. **Verificar:**
   ```
   PHSensor: Calibrado slope=0.1850 V/pH (buffer 4)
   ```
   
   ✅ **Pendiente normal:** 0.15 - 0.20 V/pH

### **PASO 5: Guardar la Calibración**

¡IMPORTANTE! La calibración se guarda en EEPROM:

```
PHSAVE
```

Respuesta:
```
PHSensor: Calibración guardada en EEPROM
```

Ahora la calibración se mantiene aunque desconectes el ESP32.

---

## 🧪 VERIFICAR CALIBRACIÓN

### **Test 1: Ver voltaje actual**

Monitorea la salida serial:
```
pH: 6.85 (2.486V) [Calibrado]
```

**Valores de voltaje esperados:**
- pH 7.0 → ~2.5V
- pH 6.0 → ~2.7V
- pH 8.0 → ~2.3V
- pH 4.0 → ~3.0V
- pH 10.0 → ~1.9V

### **Test 2: Probar con agua de grifo**

```
1. Sumergir sensor en agua de grifo
2. Esperar 10 segundos
3. Leer valor en monitor serial
4. Debe dar: pH 6.5 - 8.5 (normal para agua de grifo)
```

### **Test 3: Probar con vinagre**

```
1. Sumergir en vinagre
2. Debe dar: pH 2.5 - 3.5
```

---

## 🔄 COMANDOS DE CALIBRACIÓN

### **Tabla Resumen:**

| Comando | Descripción | Cuándo usar |
|---------|-------------|-------------|
| `PHCAL,7` | Calibrar pH neutro | **SIEMPRE primero** |
| `PHCAL,4` | Calibrar pH ácido | Después de pH 7 |
| `PHCAL,10` | Calibrar pH básico | Después de pH 7 |
| `PHSAVE` | Guardar en EEPROM | **Después de calibrar** |
| `PHRESET` | Resetear calibración | Si algo sale mal |
| `PHEEPRCLR` | Borrar EEPROM | Reinicio completo |

### **Ejemplo de Sesión Completa:**

```bash
# Paso 1: Verificar comandos
HELP

# Paso 2: Calibrar pH 7 (sensor en buffer pH 7)
PHCAL,7
# Respuesta: PHSensor: Calibrado V@7=2.4856 V

# Paso 3: Calibrar pH 4 (sensor en buffer pH 4)
PHCAL,4
# Respuesta: PHSensor: Calibrado slope=0.1850 V/pH (buffer 4)

# Paso 4: GUARDAR (¡IMPORTANTE!)
PHSAVE
# Respuesta: PHSensor: Calibración guardada en EEPROM

# Paso 5: Verificar
# Ahora deberías ver en el monitor:
# pH: 6.85 (2.486V) [Calibrado] ✅
```

---

## ❌ PROBLEMAS COMUNES

### **Problema 1: "pH: 20.89" (valor imposible)**

**Causa:** Sensor desconectado o no calibrado

**Solución:**
```bash
# Opción A: Verificar conexión física
- GPIO32 → señal Po del PH4502C
- 5V → VCC
- GND → GND

# Opción B: Forzar calibración con valores por defecto
PHRESET
PHSAVE
```

### **Problema 2: "Voltaje: 0.000V"**

**Causa:** Sensor totalmente desconectado

**Solución:**
1. Verificar cable a GPIO32
2. Verificar alimentación 5V del sensor
3. Buscar LED en el módulo PH4502C (debe estar encendido)

### **Problema 3: "pH no cambia al cambiar líquido"**

**Causa:** Electrodo sucio o dañado

**Solución:**
1. Limpiar electrodo con agua destilada
2. Sumergir 30 min en solución KCl 3M (si tienes)
3. Si es muy viejo, reemplazar electrodo

### **Problema 4: "Lecturas inestables"**

**Causa:** Burbujas de aire, temperatura o agitación

**Solución:**
1. Eliminar burbujas del bulbo
2. Esperar a que temperatura se estabilice
3. No agitar el líquido durante lectura
4. Calibrar con temperatura actual:
   ```
   SETT,25.0    # Establecer temperatura actual
   ```

---

## 🎓 CONSEJOS PROFESIONALES

### **1. Mantenimiento del electrodo:**
```
✅ Guardar en solución KCl 3M (si tienes)
✅ O en agua destilada con una gota de buffer pH 7
❌ NUNCA secar completamente
❌ NUNCA tocar el bulbo de vidrio con los dedos
```

### **2. Frecuencia de calibración:**
```
- Uso intensivo: Cada semana
- Uso normal: Cada 2-4 semanas
- Después de almacenamiento: Siempre
```

### **3. Orden correcto de calibración:**
```
1º → pH 7 (OBLIGATORIO)
2º → pH 4 o pH 10 (mejora precisión)
3º → PHSAVE (OBLIGATORIO)
```

### **4. Calibración de 3 puntos (Óptimo):**
```
PHCAL,7      # Neutro
PHCAL,4      # Ácido
PHCAL,10     # Básico
PHSAVE       # Guardar
```

---

## 🚀 INICIO RÁPIDO (Sin soluciones buffer)

Si NO tienes soluciones buffer y necesitas algo YA:

### **Calibración mínima funcional:**

```bash
# 1. Resetear calibración
PHRESET

# 2. Usar valores por defecto (aproximados)
# El sistema ya tiene valores razonables:
# - V@pH7 = 2.50V
# - Pendiente = 0.18 V/pH

# 3. Guardar
PHSAVE

# 4. Probar con agua de grifo
# Debería dar pH 6-8 (aproximado)
```

**Precisión:** ±1.0 pH (suficiente para hidropónicos)

---

## 📞 SIGUIENTE PASO

**¿Qué quieres hacer?**

**A.** 🔬 **Tengo soluciones buffer** → Calibrar ahora (te guío paso a paso)

**B.** 🏠 **NO tengo buffer** → Usar calibración por defecto o casera

**C.** 🛠️ **Problemas con el sensor** → Diagnosticar hardware

**D.** 📊 **Ya calibré** → Verificar en el dashboard web

**Responde con la letra para continuar.**

---

## 📚 REFERENCIAS

**Documentación técnica:**
- Sensor PH4502C: Rango 0-14 pH
- Voltaje de salida: 0-3.3V
- Precisión después de calibración: ±0.1 pH
- Tiempo de respuesta: <60 segundos

**Fórmula de cálculo:**
```
pH = 7.0 + (V₇ - Vmedido) / pendiente
```

Donde:
- V₇ = voltaje en pH 7
- Vmedido = voltaje actual del sensor  
- pendiente = cambio de voltaje por unidad de pH (~0.18 V/pH)
