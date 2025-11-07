# Módulos del Sistema Hidropónico

Este documento explica los módulos creados a partir del `codigoB` para ser utilizados en el sistema hidropónico.

## Estructura de Módulos

### 📊 PHSensor (`lib/PHSensor/`)

Maneja el sensor de pH PH4502C con calibración automática y filtrado de señal.

**Características:**

- Calibración en 3 puntos (pH 4, 7, 10)
- Filtro IIR para suavizar lecturas
- Almacenamiento en EEPROM
- Compensación por temperatura
- Detección automática de errores

**Uso básico:**

```cpp
PHSensor phSensor(32, 0); // Pin 32, EEPROM addr 0
phSensor.begin();
phSensor.update();
float ph = phSensor.getFilteredPH();
```

**Comandos de calibración:**

- `PHCAL,7` - Calibrar punto neutro
- `PHCAL,4` - Calibrar punto ácido
- `PHCAL,10` - Calibrar punto básico
- `PHSAVE` - Guardar calibración

### 💧 TDSSensor (`lib/TDSSensor/`)

Maneja el sensor TDS SEN0244 con detección automática de conexión.

**Características:**

- Detección automática de sensor conectado
- Compensación por temperatura
- Integración con librería GravityTDS

**Uso básico:**

```cpp
TDSSensor tdsSensor(33); // Pin 33
tdsSensor.begin();
tdsSensor.update();
float tds = tdsSensor.getTDSValue();
bool conectado = tdsSensor.isConnected();
```

### ⚙️ PumpController (`lib/PumpController/`)

Controla las bombas de dosificación de pH con lógica de pulsos.

**Características:**

- Control por pulsos de 10 segundos
- Histéresis para evitar oscilaciones
- Seguridad por tiempo máximo
- Control manual y automático
- Configuración de lógica de relés

**Uso básico:**

```cpp
PumpController pumps(23, 25, 26); // Circulación, pH-, pH+
pumps.begin();
pumps.update(ph_actual, nivel_minus_ok, nivel_plus_ok);
```

**Control manual:**

- `PPLUS,ON/OFF` - Control bomba pH+
- `PMINUS,ON/OFF` - Control bomba pH-
- `RELCFG,LOW/HIGH` - Configurar lógica de relés

### 📏 LevelSensor (`lib/LevelSensor/`)

Maneja sensores de nivel de líquido SEN0205 para tanques de dosificación únicamente.

**Características:**

- Soporte para múltiples sensores
- Configuración de lógica (HIGH/LOW = OK)
- Identificación por nombre
- **Configuración simplificada**: Solo tanques pH+ y pH-

**Uso básico:**

```cpp
MultiLevelSensor levels;
levels.addSensor(32, true, "pH-");        // Tanque ácido
levels.addSensor(33, true, "pH+");        // Tanque base
levels.begin();
bool ok = levels.isLevelOK("pH-");
```

**Nota**: El sistema ya no monitorea el tanque principal, solo los tanques de dosificación.

### 🌞 LDRSensor (`lib/LDRSensor/`)

Maneja sensor de luz LDR con clasificación automática.

**Características:**

- Clasificación en 5 niveles de luz
- Umbrales configurables
- Detección de sol directo

**Uso básico:**

```cpp
LDRSensor ldr(35); // Pin 35
ldr.begin();
ldr.update();
String nivel = ldr.getLightLevelString();
bool sol = ldr.isBrightSun();
```

### 💻 SerialCommands (`lib/SerialCommands/`)

Maneja comandos por puerto serie para configuración y control.

**Comandos disponibles:**

- **Calibración pH:** `PHCAL,7` `PHCAL,4` `PHCAL,10` `PHSAVE` `PHRESET`
- **Configuración:** `SETT,25.5` `RELCFG,LOW` `LVLCFG,HIGH`
- **Control manual:** `PPLUS,ON` `PMINUS,OFF`
- **Ayuda:** `HELP`

## Integración en main.cpp

El nuevo `main.cpp` integra todos los módulos y mantiene la funcionalidad Firebase:

### Características principales:

1. **Modo dual:** Sensores reales + simulación
2. **Control automático de pH** con histéresis
3. **Comandos seriales** para calibración y control
4. **Envío a Firebase** con datos reales
5. **Monitoreo completo** del sistema

### Configuración de pines

El sistema ahora usa archivos de configuración centralizados en `include/`:

**`include/pin_config.h`** - Configuración de pines del ESP32:

```cpp
// Sensores analógicos (ADC1 - WiFi compatible)
#define PH_PIN 32         // Sensor pH PH4502C
#define TDS_PIN 33        // Sensor TDS/Conductividad SEN0244
#define LDR_PIN 35        // Fotoresistencia LDR

// Sensores digitales de nivel SEN0205 (Solo dosificación)
#define LVL_PH_MINUS 18   // Tanque pH- (ácido)
#define LVL_PH_PLUS 21    // Tanque pH+ (base)

// Control de bombas (relés)
#define RELAY_CIRC 23     // Bomba principal/circulación
#define RELAY_PH_MINUS 25 // Bomba pH- (ácido)
#define RELAY_PH_PLUS 26  // Bomba pH+ (base)
```

**`include/network_config.h`** - Configuración WiFi y Firebase:

```cpp
const char* WIFI_SSID = "Prueba123";
const char* WIFI_PASSWORD = "1234567890";
#define DATABASE_URL "https://proyecto-hidroponico-9ea9d-default-rtdb.firebaseio.com/"
```

## Instalación y Uso

### 1. Subir el firmware:

```cmd
pio run --target upload
```

### 2. Monitorear el sistema:

```cmd
pio device monitor --port COM3 --baud 115200
```

### 3. Calibrar el sensor de pH:

1. Sumerge el sensor en buffer pH 7
2. Envía: `PHCAL,7`
3. Sumerge en buffer pH 4 o 10
4. Envía: `PHCAL,4` o `PHCAL,10`
5. Guarda: `PHSAVE`

### 4. Configurar temperatura:

```
SETT,25.5
```

### 5. Control manual de bombas:

```
PPLUS,ON    # Encender bomba pH+
PMINUS,OFF  # Apagar bomba pH-
```

## Ventajas de la Modularización

1. **Reutilizable:** Cada módulo es independiente
2. **Mantenible:** Código organizado y documentado
3. **Escalable:** Fácil agregar nuevos sensores
4. **Testeable:** Cada módulo se puede probar por separado
5. **Configurable:** Parámetros ajustables en tiempo real

## Próximos Pasos

- [ ] Implementar control PID más avanzado
- [ ] Crear interfaz web para configuración
- [ ] Implementar alertas por SMS/email

## Soporte

Para obtener ayuda, ejecuta `HELP` en el monitor serie o revisa la documentación de cada módulo en sus archivos `.h` correspondientes.
