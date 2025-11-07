#include <Arduino.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <EEPROM.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"
#include "pin_config.h"     // Configuración de pines centralizada
#include "network_config.h" // Configuración de red centralizada

// Incluir nuestros módulos
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

// Objetos de nuestros módulos
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
const unsigned long SENSOR_INTERVAL = 500;     // 500ms para sensores
const unsigned long FIREBASE_INTERVAL = 10000; // 10s para Firebase
const unsigned long SERIAL_INTERVAL = 5000;    // 5s para salida serial

// Variables para simulación cuando no hay sensores físicos
bool useSimulation = false;
float prev_ph = 6.5;
float prev_tds = 500.0;

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

  // DATOS DE DIAGNÓSTICO
  ok &= Firebase.RTDB.setString(&fbData, "/hydroponic_data/diagnostico/chip", "ESP32-D0WD-V3");
  ok &= Firebase.RTDB.setString(&fbData, "/hydroponic_data/diagnostico/mac", mac);
  ok &= Firebase.RTDB.setFloat(&fbData, "/hydroponic_data/diagnostico/senal", WiFi.RSSI());
  ok &= Firebase.RTDB.setString(&fbData, "/hydroponic_data/diagnostico/ip", WiFi.localIP().toString());
  ok &= Firebase.RTDB.setString(&fbData, "/hydroponic_data/diagnostico/estado", "Conectado");
  ok &= Firebase.RTDB.setInt(&fbData, "/hydroponic_data/diagnostico/timestamp", millis());

  // DATOS DE SENSORES REALES O SIMULADOS
  float ph_value, tds_value;
  int nivel_liquido_pct, nivel_tranque;

  if (useSimulation)
  {
    // Simulación cuando no hay sensores físicos
    prev_ph += random(-20, 21) / 10.0;
    if (prev_ph < 5.5)
      prev_ph = 5.5;
    if (prev_ph > 7.5)
      prev_ph = 7.5;
    ph_value = prev_ph;

    prev_tds += random(-50, 51);
    if (prev_tds < 0)
      prev_tds = 0;
    if (prev_tds > 1000)
      prev_tds = 1000;
    tds_value = prev_tds;

    nivel_liquido_pct = random(0, 101);
    nivel_tranque = random(10, 100);

    Serial.println("MODO SIMULACIÓN ACTIVO");
  }
  else
  {
    // Datos reales de los sensores
    ph_value = phSensor.getFilteredPH();
    tds_value = tdsSensor.getTDSValue();

    // Para compatibilidad con frontend - usar valores simulados para nivel general
    // Solo monitoreamos niveles de tanques de dosificación pH+ y pH-
    nivel_liquido_pct = random(60, 90); // Simular nivel general del sistema
    nivel_tranque = random(70, 95);     // Para compatibilidad con dashboard
  }

  // Enviar datos de sensores
  ok &= Firebase.RTDB.setFloat(&fbData, "/hydroponic_data/sensores/ph4502c/ph", ph_value);
  ok &= Firebase.RTDB.setFloat(&fbData, "/hydroponic_data/sensores/sen0244/tds", tds_value);
  ok &= Firebase.RTDB.setInt(&fbData, "/hydroponic_data/sensores/sen0205/nivel_liquido", nivel_liquido_pct);
  ok &= Firebase.RTDB.setInt(&fbData, "/hydroponic_data/sensores/ultrasonico/nivel_tranque", nivel_tranque);

  // Estados de las bombas
  int bomba_agua = pumpController.isCirculationOn() ? 1 : 0;
  int bomba_sustrato = pumpController.isPumpMinusOn() ? 1 : 0;
  int bomba_solucion = pumpController.isPumpPlusOn() ? 1 : 0;

  ok &= Firebase.RTDB.setInt(&fbData, "/hydroponic_data/actuadores/bomba_agua/estado", bomba_agua);
  ok &= Firebase.RTDB.setInt(&fbData, "/hydroponic_data/actuadores/bomba_sustrato/estado", bomba_sustrato);
  ok &= Firebase.RTDB.setInt(&fbData, "/hydroponic_data/actuadores/bomba_solucion/estado", bomba_solucion);

  // Datos adicionales del sistema real
  ok &= Firebase.RTDB.setBool(&fbData, "/hydroponic_data/sensores/tds_conectado", tdsSensor.isConnected());
  ok &= Firebase.RTDB.setBool(&fbData, "/hydroponic_data/sensores/ph_calibrado", phSensor.isCalibrationValid());
  ok &= Firebase.RTDB.setString(&fbData, "/hydroponic_data/sistema/modo", useSimulation ? "simulacion" : "real");

  // Estados de sensores de nivel (solo tanques de dosificación)
  if (!useSimulation)
  {
    ok &= Firebase.RTDB.setBool(&fbData, "/hydroponic_data/sensores/nivel_ph_minus/estado", levelSensors.isLevelOK("pH-"));
    ok &= Firebase.RTDB.setBool(&fbData, "/hydroponic_data/sensores/nivel_ph_plus/estado", levelSensors.isLevelOK("pH+"));
  }

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
  Serial.println("\n=== ESTADO DEL SISTEMA HIDROPÓNICO ===");

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

  // Estado de niveles de tanques de dosificación
  bool nivelMinus = levelSensors.isLevelOK("pH-");
  bool nivelPlus = levelSensors.isLevelOK("pH+");
  Serial.printf("Niveles - pH-: %s | pH+: %s\n",
                nivelMinus ? "OK" : "BAJO",
                nivelPlus ? "OK" : "BAJO");

  // Estado de bombas
  Serial.printf("Bombas - Circulación: %s | pH-: %s | pH+: %s\n",
                pumpController.isCirculationOn() ? "ON" : "OFF",
                pumpController.isPumpMinusOn() ? "ON" : "OFF",
                pumpController.isPumpPlusOn() ? "ON" : "OFF");

  // Estado de control
  if (pumpController.getCurrentDoseState() == PumpController::DOSING)
  {
    const char *tipoStr = (pumpController.getCurrentDoseType() == PumpController::DOSE_PLUS) ? "pH+" : "pH-";
    Serial.printf("Dosificación activa: %s (Pulso: %lums, Sesión: %lums)\n",
                  tipoStr,
                  pumpController.getElapsedPulse(),
                  pumpController.getElapsedSession());
  }
  else
  {
    Serial.println("Estado de dosificación: IDLE");
  }

  Serial.printf("Modo: %s\n", useSimulation ? "SIMULACIÓN" : "SENSORES REALES");
  Serial.println("=====================================\n");
}

void setup()
{
  Serial.begin(115200);
  delay(2000);

  Serial.println("\n========================================");
  Serial.println("    🚀 SISTEMA HIDROPÓNICO MODULAR 🚀    ");
  Serial.println("========================================");

  // Inicializar EEPROM para calibraciones
  EEPROM.begin(512);

  // Inicializar sensores
  Serial.println("Inicializando sensores...");
  phSensor.begin();
  tdsSensor.begin();
  ldrSensor.begin();

  // Configurar sensores de nivel SEN0205 para tanques de dosificación
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
    Serial.println("Sin WiFi - Funcionando en modo local");
    useSimulation = true; // Usar simulación si no hay WiFi
    return;
  }

  // Configurar Firebase
  Serial.println("\nConfigurando Firebase...");
  config.database_url = DATABASE_URL;
  config.host = DATABASE_HOST;
  config.signer.tokens.legacy_token = DATABASE_SECRET;
  config.token_status_callback = tokenStatusCallback;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  // Esperar conexión Firebase
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
    Serial.println("\n✅ Firebase conectado");
    Serial.println("🔄 Sistema funcionando con sensores reales + Firebase");
    useSimulation = false;
    enviarDatos(); // Envío inicial
  }
  else
  {
    Serial.println("\n❌ Timeout Firebase - Modo local");
    Serial.println(fbData.errorReason().c_str());
    useSimulation = true;
  }

  Serial.println("\n✅ Sistema inicializado completamente");
  Serial.println("Escribe HELP para ver comandos disponibles\n");
}

void loop()
{
  unsigned long now = millis();

  // Procesar comandos seriales
  serialCommands.processCommands();

  // Actualizar sensores
  if (now - lastSensorUpdate >= SENSOR_INTERVAL)
  {
    lastSensorUpdate = now;

    if (!useSimulation)
    {
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

      // Control automático de pH
      bool nivelMinus = levelSensors.isLevelOK("pH-");
      bool nivelPlus = levelSensors.isLevelOK("pH+");
      pumpController.update(phSensor.getFilteredPH(), nivelMinus, nivelPlus);
    }
  }

  // Actualizar Firebase
  if (now - lastFirebaseUpdate >= FIREBASE_INTERVAL)
  {
    lastFirebaseUpdate = now;
    if (WiFi.status() == WL_CONNECTED && (Firebase.ready() || useSimulation))
    {
      enviarDatos();
    }
  }

  // Salida por Serial
  if (now - lastSerialOutput >= SERIAL_INTERVAL)
  {
    lastSerialOutput = now;
    if (!useSimulation)
    {
      imprimirEstadoSistema();
    }
  }
}
