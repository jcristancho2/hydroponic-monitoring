#include <Arduino.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

// CONFIGURACIÓN WIFI
const char* WIFI_SSID = "Prueba123";
const char* WIFI_PASSWORD = "1234567890";

// CONFIGURACIÓN FIREBASE
#define API_KEY "AIzaSyAtJSvqWiw0qRB4wwDLRahKVEzH_qcwYj8"
#define DATABASE_URL "https://proyecto-hidroponico-9ea9d-default-rtdb.firebaseio.com/"
#define DATABASE_SECRET "h9ppcrM5LEnlcw88bdhItk24hEBb9rpK7Nbi24zs"
#define DATABASE_HOST "proyecto-hidroponico-9ea9d-default-rtdb.firebaseio.com"

FirebaseData fbData;
FirebaseAuth auth;
FirebaseConfig config;

// Variables para simulación (valores previos para variaciones suaves)
float prev_ph = 6.5;
float prev_tds = 500.0;
int prev_nivel_liquido = 1;
int prev_nivel_tranque = 50;
int prev_bomba_agua = 0;
int prev_bomba_sustrato = 0;
int prev_bomba_solucion = 0;

void conectarWiFi() {
  Serial.println("\nConectando a WiFi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  int intentos = 0;
  while (WiFi.status() != WL_CONNECTED && intentos < 20) {
    delay(500); Serial.print("."); intentos++;
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConectado a WiFi");
    Serial.print("IP: "); Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nFallo WiFi. Reinicia la placa.");
  }
}

void enviarDatos() {
  if (!Firebase.ready()) {
    Serial.println("Firebase no listo");
    return;
  }

  Serial.println("--- Enviando datos simulados ---");

  String mac = WiFi.macAddress();
  bool ok = true;

  // DATOS DE DIAGNÓSTICO (siempre)
  ok &= Firebase.RTDB.setString(&fbData, "/hydroponic_data/diagnostico/chip", "ESP32-D0WD-V3");
  ok &= Firebase.RTDB.setString(&fbData, "/hydroponic_data/diagnostico/mac", mac);
  ok &= Firebase.RTDB.setFloat(&fbData, "/hydroponic_data/diagnostico/senal", WiFi.RSSI());
  ok &= Firebase.RTDB.setString(&fbData, "/hydroponic_data/diagnostico/ip", WiFi.localIP().toString());
  ok &= Firebase.RTDB.setString(&fbData, "/hydroponic_data/diagnostico/estado", "Conectado");
  ok &= Firebase.RTDB.setInt(&fbData, "/hydroponic_data/diagnostico/timestamp", millis());

  // SIMULACIÓN DE SENSORES (valores random realistas)
  
  // 1. PH4502C - pH (rango hidroponía: 5.5-7.5, variación ±0.2)
  prev_ph += random(-20, 21) / 10.0;  // Variación -0.2 a +0.2
  if (prev_ph < 5.5) prev_ph = 5.5;
  if (prev_ph > 7.5) prev_ph = 7.5;
  ok &= Firebase.RTDB.setFloat(&fbData, "/hydroponic_data/sensores/ph4502c/ph", prev_ph);
  Serial.printf("pH simulado: %.1f\n", prev_ph);

  // 2. SEN0244 - TDS (0-1000 ppm, variación ±50)
  prev_tds += random(-50, 51);
  if (prev_tds < 0) prev_tds = 0;
  if (prev_tds > 1000) prev_tds = 1000;
  ok &= Firebase.RTDB.setFloat(&fbData, "/hydroponic_data/sensores/sen0244/tds", prev_tds);
  Serial.printf("TDS simulado: %.0f ppm\n", prev_tds);

  // 3. SEN0205 - Nivel de líquido (binario 0/1, o %: 0-100)
  // Simulamos como porcentaje (0-100) para más realismo
  int nivel_liquido_pct = random(0, 101);
  ok &= Firebase.RTDB.setInt(&fbData, "/hydroponic_data/sensores/sen0205/nivel_liquido", nivel_liquido_pct);
  Serial.printf("Nivel líquido simulado: %d%%\n", nivel_liquido_pct);

  // 4. Sensor ultrasónico - Nivel de agua en tranque (10-100 cm, variación ±5)
  prev_nivel_tranque += random(-5, 6);
  if (prev_nivel_tranque < 10) prev_nivel_tranque = 10;
  if (prev_nivel_tranque > 100) prev_nivel_tranque = 100;
  ok &= Firebase.RTDB.setInt(&fbData, "/hydroponic_data/sensores/ultrasonico/nivel_tranque", prev_nivel_tranque);
  Serial.printf("Nivel tranque simulado: %d cm\n", prev_nivel_tranque);

  // 5. Moto bombas - Estados (0=off, 1=on, random cada vez)
  prev_bomba_agua = random(0, 2);  // 0 o 1
  prev_bomba_sustrato = random(0, 2);
  prev_bomba_solucion = random(0, 2);
  
  ok &= Firebase.RTDB.setInt(&fbData, "/hydroponic_data/actuadores/bomba_agua/estado", prev_bomba_agua);
  ok &= Firebase.RTDB.setInt(&fbData, "/hydroponic_data/actuadores/bomba_sustrato/estado", prev_bomba_sustrato);
  ok &= Firebase.RTDB.setInt(&fbData, "/hydroponic_data/actuadores/bomba_solucion/estado", prev_bomba_solucion);
  
  Serial.printf("Bombas: Agua=%d, Sustrato=%d, Solución=%d\n", prev_bomba_agua, prev_bomba_sustrato, prev_bomba_solucion);

  if (ok) {
    Serial.println("Datos simulados enviados correctamente");
  } else {
    Serial.printf("Error: %s (HTTP: %d)\n", fbData.errorReason().c_str(), fbData.httpCode());
  }
}

void setup() {
  Serial.begin(115200);
  delay(2000);

  Serial.println("\n========================================");
  Serial.println("    🚀 SISTEMA HIDROPÓNICO SIMULADO 🚀    ");
  Serial.println("========================================");

  conectarWiFi();
  if (WiFi.status() != WL_CONNECTED) return;

  Serial.println("\nConfigurando Firebase...");

  config.database_url = DATABASE_URL;
  config.host = DATABASE_HOST;
  config.signer.tokens.legacy_token = DATABASE_SECRET;
  config.token_status_callback = tokenStatusCallback;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  Serial.println("Esperando Firebase...");
  int espera = 0;
  while (!Firebase.ready() && espera < 30) {
    delay(500); Serial.print("."); espera++;
  }

  if (Firebase.ready()) {
    Serial.println("\n✅ Firebase conectado (con simulación de sensores)");
    enviarDatos();  // Envío inicial
  } else {
    Serial.println("\n❌ Timeout Firebase");
    Serial.println(fbData.errorReason().c_str());
  }
}

void loop() {
  static unsigned long ultimo = 0;
  if (millis() - ultimo > 10000 && Firebase.ready()) {  // Cada 10 segundos
    enviarDatos();
    ultimo = millis();
  }
}
