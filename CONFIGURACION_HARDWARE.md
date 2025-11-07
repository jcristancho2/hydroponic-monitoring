# Configuración de Hardware y Red

Este documento explica cómo configurar los pines del ESP32 y las credenciales de red para el sistema hidropónico.

## 📌 Configuración de Pines (`include/pin_config.h`)

### Pines de Sensores Analógicos (ADC1)

Los sensores analógicos **DEBEN** usar pines ADC1 para compatibilidad con WiFi:

```cpp
#define PH_PIN 32         // Sensor pH PH4502C
#define TDS_PIN 33        // Sensor TDS SEN0244
#define LDR_PIN 35        // Sensor de luz LDR
```

**Pines ADC1 disponibles:** 32, 33, 34, 35, 36, 39

### Pines de Sensores Digitales

Los sensores de nivel SEN0205 pueden usar cualquier pin GPIO:

```cpp
#define LVL_PH_MINUS 18   // Sensor nivel tanque pH-
#define LVL_PH_PLUS 21    // Sensor nivel tanque pH+
```

### Pines de Control (Relés)

Las bombas se controlan mediante relés en cualquier pin GPIO:

```cpp
#define RELAY_CIRC 23     // Bomba circulación
#define RELAY_PH_MINUS 25 // Bomba pH- (ácido)
#define RELAY_PH_PLUS 26  // Bomba pH+ (base)
```

### Validación Automática

El archivo incluye validación automática para evitar conflictos de pines:

- ✅ Verifica que no hay pines duplicados
- ✅ Evita pines problemáticos en boot
- ✅ Documenta pinout completo del ESP32

## 🌐 Configuración de Red (`include/network_config.h`)

### Credenciales WiFi

```cpp
const char* WIFI_SSID = "TU_RED_WIFI";
const char* WIFI_PASSWORD = "TU_PASSWORD";
```

### Configuración Firebase

```cpp
#define API_KEY "tu-api-key-de-firebase"
#define DATABASE_URL "https://tu-proyecto-default-rtdb.firebaseio.com/"
#define DATABASE_SECRET "tu-database-secret"
#define DATABASE_HOST "tu-proyecto-default-rtdb.firebaseio.com"
```

### Timeouts y Configuración

```cpp
#define WIFI_TIMEOUT 20000              // 20 segundos
#define DATA_SEND_INTERVAL 2000         // 2 segundos
#define FIREBASE_TIMEOUT 10000          // 10 segundos
```

## 🔧 Cómo Modificar la Configuración

### 1. Cambiar Pines de Sensores

Edita `include/pin_config.h`:

```cpp
// Ejemplo: Cambiar sensor pH del pin 32 al pin 34
#define PH_PIN 34  // Nuevo pin para sensor pH
```

### 2. Cambiar Credenciales WiFi

Edita `include/network_config.h`:

```cpp
const char* WIFI_SSID = "MiNuevaRed";
const char* WIFI_PASSWORD = "MiNuevoPassword";
```

### 3. Cambiar Configuración Firebase

Edita `include/network_config.h`:

```cpp
#define DATABASE_URL "https://mi-nuevo-proyecto-default-rtdb.firebaseio.com/"
```

## ⚠️ Pines a Evitar

### Pines Problemáticos en Boot

- **Pin 0**: GPIO0 (Boot/Flash)
- **Pin 2**: GPIO2 (Boot mode)
- **Pin 5**: GPIO5 (SPI SS Flash)
- **Pin 12**: GPIO12 (Boot voltage)
- **Pin 15**: GPIO15 (SPI SS Flash)

### Pines Solo de Entrada

- **Pin 34**: GPIO34 (solo entrada)
- **Pin 35**: GPIO35 (solo entrada)
- **Pin 36**: GPIO36 (solo entrada)
- **Pin 39**: GPIO39 (solo entrada)

### ADC2 (Incompatible con WiFi)

- Pines 0, 2, 4, 12, 13, 14, 15, 25, 26, 27

## 📋 Pinout de Referencia ESP32

```
        ESP32 DEVKIT V1
    ┌─────────────────────────┐
    │ EN                   D23│ GPIO23
    │ VP (GPIO36)          D22│ GPIO22
    │ VN (GPIO39)          TX0│ GPIO1
    │ D34                  RX0│ GPIO3
    │ D35                  D21│ GPIO21
    │ D32                  D19│ GPIO19
    │ D33                  D18│ GPIO18
    │ D25                   D5│ GPIO5
    │ D26                  TX2│ GPIO17
    │ D27                  RX2│ GPIO16
    │ D14                   D4│ GPIO4
    │ D12                   D2│ GPIO2
    │ D13                  D15│ GPIO15
    │ GND                  GND│
    │ VIN                  3V3│
    └─────────────────────────┘
```

## 🚀 Expansión Futura

El archivo `pin_config.h` incluye secciones comentadas para sensores adicionales:

```cpp
// #define TEMP_SENSOR_PIN 4      // Sensor temperatura DS18B20
// #define FLOW_SENSOR_PIN 5      // Sensor de flujo
// #define MAIN_VALVE_PIN 12      // Válvula principal
// #define BUZZER_PIN 4           // Buzzer de alarma
```

Para habilitar funciones futuras, simplemente descomenta y asigna los pines necesarios.

## 🔍 Debug y Monitoreo

El archivo `network_config.h` incluye flags de debug:

```cpp
#define DEBUG_WIFI      // Debug conexión WiFi
#define DEBUG_FIREBASE  // Debug operaciones Firebase
#define DEBUG_DATA      // Debug datos enviados
```

Comenta cualquier línea para desactivar el debug correspondiente.

## ✅ Verificación de Configuración

Antes de compilar, verifica:

1. **Pines únicos**: Ningún pin se usa para múltiples funciones
2. **ADC1 para analógicos**: Sensores pH, TDS y LDR en pines ADC1
3. **Credenciales correctas**: WiFi y Firebase configurados
4. **Sin pines problemáticos**: Evitar pines que causan problemas en boot

El sistema incluye validación automática que evitará la compilación si hay conflictos.
