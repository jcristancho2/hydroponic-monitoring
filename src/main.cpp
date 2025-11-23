#include <Arduino.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <EEPROM.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"
#include "pin_config.h"
#include "network_config.h"

// Incluir nuestros mudulos
#include "PHSensor.h"
#include "TDSSensor.h"
#include "PumpController.h"
#include "LevelSensor.h"
#include "LDRSensor.h"
#include "SerialCommands.h"

// Objetos Firebase
FirebaseData fbData;
FirebaseAuth auth;
FirebaseConfig config;

// Objetos de nuestros modulos
PHSensor phSensor(PH_PIN, 0); // EEPROM addr 0
TDSSensor tdsSensor(TDS_PIN);
PumpController pumpController(RELAY_CIRC, RELAY_PH_MINUS, RELAY_PH_PLUS);
MultiLevelSensor levelSensors;
LDRSensor ldrSensor(LDR_PIN);
SerialCommands serialCommands;

// Timing
unsigned long lastSensorUpdate = 0;
unsigned long lastFirebaseUpdate = 0;
unsigned long lastSerialOutput = 0;
unsigned long lastCommandCheck = 0;
const unsigned long SENSOR_INTERVAL = 500;     // 500ms para sensores
const unsigned long FIREBASE_INTERVAL = 10000; // 10s para Firebase
const unsigned long SERIAL_INTERVAL = 5000;    // 5s para salida serial
const unsigned long COMMAND_CHECK_INTERVAL = 2000; // 2s para verificar comandos


// Monitoreo de exposición solar
unsigned long solarExposureStartTime = 0;        // Inicio de exposición solar
unsigned long totalSolarExposureToday = 0;       // Total acumulado hoy (en segundos)
bool isSolarExposure = false;                    // Bandera de exposición activa
const int SOLAR_THRESHOLD = 500;                 // Umbral de LDR para considerar luz solar
const unsigned long SOLAR_RESET_TIME = 86400000; // 24 horas en ms para resetear contador

void conectarWiFi()
{
  Serial.println("\nConectando a WiFi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  int intentos = 0;
  while (WiFi.status() != WL_CONNECTED && intentos < 20)
  {
    delay(500);
    Serial.print(".");
    intentos++;
  }
  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("\nConectado a WiFi");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
  }
  else
  {
    Serial.println("\nFallo WiFi. Reinicia la placa.");
  }
}

void enviarDatos()
{
  if (!Firebase.ready())
  {
    Serial.println("Firebase no listo");
    return;
  }

  Serial.println("--- Enviando datos a Firebase ---");

  String mac = WiFi.macAddress();
  bool ok = true;

  // DATOS DE DIAGNOSTICO
  ok &= Firebase.RTDB.setString(&fbData, "/hydroponic_data/diagnostico/chip", "ESP32-D0WD-V3");
  ok &= Firebase.RTDB.setString(&fbData, "/hydroponic_data/diagnostico/mac", mac);
  ok &= Firebase.RTDB.setFloat(&fbData, "/hydroponic_data/diagnostico/senal", WiFi.RSSI());
  ok &= Firebase.RTDB.setString(&fbData, "/hydroponic_data/diagnostico/ip", WiFi.localIP().toString());
  ok &= Firebase.RTDB.setString(&fbData, "/hydroponic_data/diagnostico/estado", "Conectado");
  ok &= Firebase.RTDB.setInt(&fbData, "/hydroponic_data/diagnostico/timestamp", millis());

  // DATOS DE SENSORES
  float ph_value, tds_value;
  int nivel_liquido_pct, nivel_tranque;

  // Siempre leer sensores
  ph_value = phSensor.getFilteredPH();
  tds_value = tdsSensor.getTDSValue();

  // Niveles de tanques de dosificacion
  bool nivel_ph_minus = levelSensors.isLevelOK("pH-");
  bool nivel_ph_plus = levelSensors.isLevelOK("pH+");

  // Para compatibilidad con dashboard - usar 0 si no hay sensores de nivel general
  nivel_liquido_pct = 0; // Indicar que no hay sensor
  nivel_tranque = 0;     // Indicar que no hay sensor

  Serial.println("ENVIANDO DATOS DE SENSORES");

  // Valor de la fotoresistencia (LDR) - LEER PRIMERO
  int ldr_value = ldrSensor.getRawValue();
  String ldr_level = ldrSensor.getLightLevelString();

  // Generar timestamp para el historial
  unsigned long dataTimestamp = millis();
  char phHistPath[80];
  char tdsHistPath[80];
  char ldrHistPath[80];
  sprintf(phHistPath, "/hydroponic_data/historial/ph/%lu", dataTimestamp);
  sprintf(tdsHistPath, "/hydroponic_data/historial/tds/%lu", dataTimestamp);
  sprintf(ldrHistPath, "/hydroponic_data/historial/ldr/%lu", dataTimestamp);

  // Enviar datos de sensores (datos actuales)
  ok &= Firebase.RTDB.setFloat(&fbData, "/hydroponic_data/sensores/ph4502c/ph", ph_value);
  ok &= Firebase.RTDB.setFloat(&fbData, "/hydroponic_data/sensores/sen0244/tds", tds_value);
  ok &= Firebase.RTDB.setInt(&fbData, "/hydroponic_data/sensores/sen0205/nivel_liquido", nivel_liquido_pct);
  ok &= Firebase.RTDB.setInt(&fbData, "/hydroponic_data/sensores/ultrasonico/nivel_tranque", nivel_tranque);

  // Guardar historial con timestamp (paths generados con sprintf)
  ok &= Firebase.RTDB.setFloat(&fbData, phHistPath, ph_value);
  ok &= Firebase.RTDB.setFloat(&fbData, tdsHistPath, tds_value);
  ok &= Firebase.RTDB.setInt(&fbData, ldrHistPath, ldr_value);

  // Estados de las bombas
  int bomba_agua = pumpController.isCirculationOn() ? 1 : 0;
  int bomba_sustrato = pumpController.isPumpMinusOn() ? 1 : 0;
  int bomba_solucion = pumpController.isPumpPlusOn() ? 1 : 0;

  ok &= Firebase.RTDB.setInt(&fbData, "/hydroponic_data/actuadores/bomba_agua/estado", bomba_agua);
  ok &= Firebase.RTDB.setInt(&fbData, "/hydroponic_data/actuadores/bomba_sustrato/estado", bomba_sustrato);
  ok &= Firebase.RTDB.setInt(&fbData, "/hydroponic_data/actuadores/bomba_solucion/estado", bomba_solucion);

  // Estado de emergencia
  ok &= Firebase.RTDB.setBool(&fbData, "/hydroponic_data/sistema/emergencia", pumpController.isEmergencyMode());

  // Datos adicionales del sistema real
  ok &= Firebase.RTDB.setBool(&fbData, "/hydroponic_data/sensores/tds_conectado", tdsSensor.isConnected());
  ok &= Firebase.RTDB.setBool(&fbData, "/hydroponic_data/sensores/ph_calibrado", phSensor.isCalibrationValid());
  ok &= Firebase.RTDB.setString(&fbData, "/hydroponic_data/sistema/modo", "conectados");

  // Enviar datos de LDR a sensores en tiempo real
  ok &= Firebase.RTDB.setInt(&fbData, "/hydroponic_data/sensores/ldr/valor_bruto", ldr_value);
  ok &= Firebase.RTDB.setString(&fbData, "/hydroponic_data/sensores/ldr/nivel_luz", ldr_level);

  // Calcular tiempo de exposición solar
  unsigned long currentTime = millis();
  bool hasSolarExposure = (ldr_value > SOLAR_THRESHOLD); // Luz solar detectada

  if (hasSolarExposure && !isSolarExposure)
  {
    // Inicio de exposición solar
    solarExposureStartTime = currentTime;
    isSolarExposure = true;
    Serial.println("☀️ Inicio de exposición solar detectado");
  }
  else if (!hasSolarExposure && isSolarExposure)
  {
    // Fin de exposición solar - agregar tiempo acumulado
    unsigned long exposureTime = (currentTime - solarExposureStartTime) / 1000; // Convertir a segundos
    totalSolarExposureToday += exposureTime;
    isSolarExposure = false;
    Serial.printf("☀️ Exposición solar finalizada. Tiempo: %lu segundos\n", exposureTime);
  }

  // Resetear contador cada 24 horas
  static unsigned long lastResetTime = 0;
  if ((currentTime - lastResetTime) > SOLAR_RESET_TIME)
  {
    totalSolarExposureToday = 0;
    lastResetTime = currentTime;
    Serial.println("🔄 Contador de exposición solar reseteado");
  }

  // Calcular tiempo restante para 6 horas máximo (21600 segundos)
  unsigned long maxSolarExposure = 21600; // 6 horas en segundos
  unsigned long remainingSolarTime = (totalSolarExposureToday < maxSolarExposure)
                                         ? (maxSolarExposure - totalSolarExposureToday)
                                         : 0;

  // Enviar datos de exposición solar a Firebase
  ok &= Firebase.RTDB.setInt(&fbData, "/hydroponic_data/sensores/ldr/exposicion_solar_hoy_segundos", totalSolarExposureToday);
  ok &= Firebase.RTDB.setInt(&fbData, "/hydroponic_data/sensores/ldr/tiempo_restante_segundos", remainingSolarTime);
  ok &= Firebase.RTDB.setBool(&fbData, "/hydroponic_data/sensores/ldr/exposicion_activa", isSolarExposure);

  // Estados de sensores de nivel de tanques de dosificacion
  ok &= Firebase.RTDB.setBool(&fbData, "/hydroponic_data/sensores/nivel_ph_minus/estado", nivel_ph_minus);
  ok &= Firebase.RTDB.setBool(&fbData, "/hydroponic_data/sensores/nivel_ph_plus/estado", nivel_ph_plus);

  if (ok)
  {
    Serial.println("Datos enviados correctamente a Firebase");
  }
  else
  {
    Serial.printf("Error Firebase: %s (HTTP: %d)\n", fbData.errorReason().c_str(), fbData.httpCode());
  }
}

void imprimirEstadoSistema()
{
  Serial.println("\n=== ESTADO DEL SISTEMA HIDROPONICO ===");

  // Estado de sensores
  Serial.printf("pH: %.2f (%.3fV) [%s]\n",
                phSensor.getFilteredPH(),
                phSensor.getVoltage(),
                phSensor.isCalibrationValid() ? "Calibrado" : "No calibrado");

  Serial.printf("TDS: %.0f ppm [%s]\n",
                tdsSensor.getTDSValue(),
                tdsSensor.isConnected() ? "Conectado" : "Desconectado");

  Serial.printf("LDR: %d (%s)\n",
                ldrSensor.getRawValue(),
                ldrSensor.getLightLevelString().c_str());

  // Estado de niveles de tanques de dosificaci├│n
  bool nivelMinus = levelSensors.isLevelOK("pH-");
  bool nivelPlus = levelSensors.isLevelOK("pH+");
  Serial.printf("Niveles - pH-: %s | pH+: %s\n",
                nivelMinus ? "OK" : "BAJO",
                nivelPlus ? "OK" : "BAJO");

  // Estado de bombas
  Serial.printf("Bombas - Circulaci├│n: %s | pH-: %s | pH+: %s\n",
                pumpController.isCirculationOn() ? "ON" : "OFF",
                pumpController.isPumpMinusOn() ? "ON" : "OFF",
                pumpController.isPumpPlusOn() ? "ON" : "OFF");

  // Estado de control
  if (pumpController.getCurrentDoseState() == PumpController::DOSING)
  {
    const char *tipoStr = (pumpController.getCurrentDoseType() == PumpController::DOSE_PLUS) ? "pH+" : "pH-";
    Serial.printf("Dosificaci├│n activa: %s (Pulso: %lums, Sesi├│n: %lums)\n",
                  tipoStr,
                  pumpController.getElapsedPulse(),
                  pumpController.getElapsedSession());
  }
  else
  {
    Serial.println("Estado de dosificaci├│n: IDLE");
  }

  Serial.printf("Modo: SENSORES REALES\n");
  Serial.println("=====================================\n");
}

void setup()
{
  Serial.begin(115200);
  delay(2000);

  Serial.println("\n========================================");
  Serial.println("       ­SISTEMA HIDROPONICO MODULAR ­      ");
  Serial.println("========================================");

  // Inicializar EEPROM para calibraciones
  EEPROM.begin(512);

  // Inicializar sensores
  Serial.println("Inicializando sensores...");
  phSensor.begin();
  tdsSensor.begin();
  ldrSensor.begin();

  // Configurar sensores de nivel SEN0205 para tanques de dosificacion
  levelSensors.addSensor(LVL_PH_MINUS, true, "pH-");
  levelSensors.addSensor(LVL_PH_PLUS, true, "pH+");
  levelSensors.begin();

  // Inicializar controlador de bombas
  Serial.println("Inicializando bombas...");
  pumpController.begin();

  // Inicializar comandos seriales
  serialCommands.begin(&phSensor, &pumpController, &tdsSensor);

  // Conectar WiFi
  conectarWiFi();
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("Sin WiFi - Sistema requiere conexion para datos reales");
    Serial.println("con WIFI REQUERIDO PARA FUNCIONAMIENTO REAL");
    while (true)
    {
      delay(1000);
      Serial.println("Reintentando WiFi...");
      conectarWiFi();
      if (WiFi.status() == WL_CONNECTED)
      {
        Serial.println("WiFi conectado, continuando...");
        break;
      }
    }
  }

  // Configurar Firebase
  Serial.println("\nConfigurando Firebase...");
  config.database_url = DATABASE_URL;
  config.host = DATABASE_HOST;
  config.signer.tokens.legacy_token = DATABASE_SECRET;
  config.token_status_callback = tokenStatusCallback;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  // Esperar conexion Firebase
  Serial.println("Esperando Firebase...");
  int espera = 0;
  while (!Firebase.ready() && espera < 30)
  {
    delay(500);
    Serial.print(".");
    espera++;
  }

  if (Firebase.ready())
  {
    Serial.println("\n Firebase conectado");
    Serial.println("­Sistema funcionando con SENSORES");
    enviarDatos(); // Envio inicial
  }
  else
  {
    Serial.println("\n Firebase requerido para datos reales");
    Serial.println("Reintentando conexion Firebase...");
    while (!Firebase.ready())
    {
      delay(2000);
      Serial.print(".");
      if (Firebase.ready())
      {
        Serial.println("\nFirebase conectado");
        break;
      }
    }
  }

  Serial.println("\nSistema inicializado completamente");
  Serial.println("Escribe HELP para ver comandos disponibles\n");
}

void loop()
{
  unsigned long now = millis();

  // Procesar comandos seriales
  serialCommands.processCommands();

  // Actualizar sensores (siempre en modo real)
  if (now - lastSensorUpdate >= SENSOR_INTERVAL)
  {
    lastSensorUpdate = now;

    // Actualizar sensores reales
    phSensor.update();

    if (tdsSensor.shouldUpdate())
    {
      tdsSensor.update();
    }

    if (ldrSensor.shouldUpdate())
    {
      ldrSensor.update();
    }

    // Control de pH (solo si no está en modo emergencia)
    if (!pumpController.isEmergencyMode())
    {
      bool nivelMinus = levelSensors.isLevelOK("pH-");
      bool nivelPlus = levelSensors.isLevelOK("pH+");
      pumpController.update(phSensor.getFilteredPH(), nivelMinus, nivelPlus);
    }
  }

  // Actualizar Firebase
  if (now - lastFirebaseUpdate >= FIREBASE_INTERVAL)
  {
    lastFirebaseUpdate = now;
    if (WiFi.status() == WL_CONNECTED && Firebase.ready())
    {
      enviarDatos();
    }
  }

  // Salida por Serial
  if (now - lastSerialOutput >= SERIAL_INTERVAL)
  {
    lastSerialOutput = now;
    imprimirEstadoSistema();
  }

  // Verificar comandos desde Firebase
  if (now - lastCommandCheck >= COMMAND_CHECK_INTERVAL)
  {
    lastCommandCheck = now;
    if (WiFi.status() == WL_CONNECTED && Firebase.ready())
    {
      // Leer comando de reinicio desde Firebase
      if (Firebase.RTDB.getBool(&fbData, "/hydroponic_data/comandos/reset"))
      {
        if (fbData.dataType() == "boolean" && fbData.boolData())
        {
          Serial.println("\n⚠️ COMANDO DE REINICIO RECIBIDO DESDE FIREBASE");
          Serial.println("Reiniciando ESP32 en 1 segundo...");
          
          // Limpiar el comando para evitar reinicios múltiples
          Firebase.RTDB.setBool(&fbData, "/hydroponic_data/comandos/reset", false);
          
          delay(1000);
          ESP.restart();
        }
      }

      // Leer comando de emergencia desde Firebase
      if (Firebase.RTDB.getBool(&fbData, "/hydroponic_data/comandos/emergency"))
      {
        if (fbData.dataType() == "boolean")
        {
          bool emergencyCommand = fbData.boolData();
          bool currentEmergency = pumpController.isEmergencyMode();
          
          if (emergencyCommand && !currentEmergency)
          {
            // Activar modo emergencia
            pumpController.emergencyStop();
            // Confirmar en Firebase
            Firebase.RTDB.setBool(&fbData, "/hydroponic_data/sistema/emergencia", true);
          }
          else if (!emergencyCommand && currentEmergency)
          {
            // Desactivar modo emergencia
            pumpController.emergencyResume();
            // Confirmar en Firebase
            Firebase.RTDB.setBool(&fbData, "/hydroponic_data/sistema/emergencia", false);
          }
        }
      }
    }
  }

}
