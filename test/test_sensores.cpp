/**
 * @file test_sensores.cpp
 * @brief Programa de diagnóstico para probar sensores individualmente
 * 
 * Este programa te permite verificar las lecturas directas de cada sensor
 * y diagnosticar problemas de conexión.
 */

#include <Arduino.h>

// Pines según pin_config.h
#define PH_PIN 32
#define TDS_PIN 33
#define LDR_PIN 35

void setup() {
    Serial.begin(115200);
    delay(2000);
    
    Serial.println("\n\n");
    Serial.println("====================================");
    Serial.println("   DIAGNÓSTICO DE SENSORES ESP32");
    Serial.println("====================================\n");
    
    // Configurar pines analógicos
    analogReadResolution(12); // 12-bit: 0-4095
    analogSetPinAttenuation(PH_PIN, ADC_11db);   // 0-3.3V
    analogSetPinAttenuation(TDS_PIN, ADC_11db);
    analogSetPinAttenuation(LDR_PIN, ADC_11db);
    
    Serial.println("Configuración completada:");
    Serial.println("- Resolución ADC: 12 bits (0-4095)");
    Serial.println("- Atenuación: 11dB (rango 0-3.3V)");
    Serial.println("- VREF: 3.3V");
    Serial.println("\n");
}

void loop() {
    Serial.println("========== LECTURA DE SENSORES ==========");
    Serial.print("Tiempo: ");
    Serial.print(millis() / 1000);
    Serial.println(" segundos");
    Serial.println();
    
    // Test Sensor pH (GPIO32)
    Serial.println("--- SENSOR pH (PH4502C) ---");
    Serial.print("Pin: GPIO");
    Serial.println(PH_PIN);
    
    int phADC = analogRead(PH_PIN);
    float phVoltage = (phADC * 3.3) / 4096.0;
    
    Serial.print("  ADC Raw: ");
    Serial.print(phADC);
    Serial.print(" / 4095 (");
    Serial.print((phADC * 100.0) / 4095.0, 1);
    Serial.println("%)");
    
    Serial.print("  Voltaje: ");
    Serial.print(phVoltage, 3);
    Serial.println(" V");
    
    if (phADC < 50) {
        Serial.println("  ⚠️ ADVERTENCIA: ADC muy bajo - Posible cortocircuito a GND");
    } else if (phADC > 4000) {
        Serial.println("  ⚠️ ADVERTENCIA: ADC muy alto - Posible desconexión o corto a VCC");
    } else if (phADC > 100 && phADC < 4000) {
        Serial.println("  ✓ Sensor conectado - Lectura en rango válido");
    }
    
    Serial.println();
    
    // Test Sensor TDS (GPIO33)
    Serial.println("--- SENSOR TDS (SEN0244) ---");
    Serial.print("Pin: GPIO");
    Serial.println(TDS_PIN);
    
    int tdsADC = analogRead(TDS_PIN);
    float tdsVoltage = (tdsADC * 3.3) / 4096.0;
    
    Serial.print("  ADC Raw: ");
    Serial.print(tdsADC);
    Serial.print(" / 4095 (");
    Serial.print((tdsADC * 100.0) / 4095.0, 1);
    Serial.println("%)");
    
    Serial.print("  Voltaje: ");
    Serial.print(tdsVoltage, 3);
    Serial.println(" V");
    
    if (tdsADC < 150) {
        Serial.println("  ❌ ERROR: Sensor desconectado o en cortocircuito");
        Serial.println("     - Verifica conexiones");
        Serial.println("     - Verifica alimentación 5V del sensor");
    } else if (tdsADC > 4000) {
        Serial.println("  ❌ ERROR: Señal fuera de rango");
    } else {
        Serial.println("  ✓ Sensor conectado - Lectura en rango válido");
        // Estimación aproximada de TDS
        float tdsEstimado = (tdsVoltage * 133.42) / 2.0 * 1000;
        Serial.print("  TDS estimado: ");
        Serial.print(tdsEstimado, 0);
        Serial.println(" ppm");
    }
    
    Serial.println();
    
    // Test Sensor LDR (GPIO35)
    Serial.println("--- SENSOR LUZ (LDR) ---");
    Serial.print("Pin: GPIO");
    Serial.println(LDR_PIN);
    
    int ldrADC = analogRead(LDR_PIN);
    float ldrVoltage = (ldrADC * 3.3) / 4096.0;
    
    Serial.print("  ADC Raw: ");
    Serial.print(ldrADC);
    Serial.print(" / 4095 (");
    Serial.print((ldrADC * 100.0) / 4095.0, 1);
    Serial.println("%)");
    
    Serial.print("  Voltaje: ");
    Serial.print(tdsVoltage, 3);
    Serial.println(" V");
    
    if (ldrADC < 100) {
        Serial.println("  Estado: Muy Oscuro");
    } else if (ldrADC < 1000) {
        Serial.println("  Estado: Oscuro");
    } else if (ldrADC < 2500) {
        Serial.println("  Estado: Luz Ambiente");
    } else {
        Serial.println("  Estado: Muy Luminoso");
    }
    
    Serial.println();
    Serial.println("==========================================");
    Serial.println();
    
    delay(2000); // Esperar 2 segundos entre lecturas
}
