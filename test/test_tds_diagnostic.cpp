/**
 * @file test_tds_diagnostic.cpp
 * @brief Diagnóstico detallado del sensor TDS SEN0244
 * 
 * Este programa prueba específicamente el sensor TDS para determinar
 * por qué está mostrando "Desconectado"
 */

#include <Arduino.h>

// Pin del sensor TDS según pin_config.h
#define TDS_PIN 33

// Constantes del sensor
#define MIN_CONNECTED_ADC 150
#define MAX_CONNECTED_ADC 4000
#define VREF 3.3
#define ADC_RES 4096.0

void setup() {
    Serial.begin(115200);
    delay(2000);
    
    Serial.println("\n\n");
    Serial.println("=============================================");
    Serial.println("  DIAGNÓSTICO DETALLADO SENSOR TDS SEN0244");
    Serial.println("=============================================\n");
    
    // Configurar pin
    analogReadResolution(12);
    analogSetPinAttenuation(TDS_PIN, ADC_11db);
    
    Serial.println("Configuración:");
    Serial.printf("- Pin GPIO: %d\n", TDS_PIN);
    Serial.println("- Resolución ADC: 12 bits (0-4095)");
    Serial.println("- Atenuación: 11dB (0-3.3V)");
    Serial.println("- VREF: 3.3V");
    Serial.printf("- Rango válido ADC: %d - %d\n", MIN_CONNECTED_ADC, MAX_CONNECTED_ADC);
    Serial.println();
    
    Serial.println("El sensor TDS SEN0244 requiere:");
    Serial.println("  1. Alimentación 5V (VCC)");
    Serial.println("  2. GND conectado");
    Serial.println("  3. Señal conectada a GPIO33");
    Serial.println("  4. Estar SUMERGIDO EN AGUA");
    Serial.println();
    Serial.println("Iniciando lecturas continuas...\n");
}

void loop() {
    // Tomar 20 muestras
    int readings[20];
    int minVal = 4095;
    int maxVal = 0;
    long sumVal = 0;
    
    Serial.println("========================================");
    Serial.printf("Tiempo: %lu segundos\n\n", millis() / 1000);
    
    // Tomar muestras
    for (int i = 0; i < 20; i++) {
        readings[i] = analogRead(TDS_PIN);
        sumVal += readings[i];
        if (readings[i] < minVal) minVal = readings[i];
        if (readings[i] > maxVal) maxVal = readings[i];
        delay(50);
    }
    
    int avgADC = sumVal / 20;
    float avgVoltage = (avgADC * VREF) / ADC_RES;
    
    // Mostrar estadísticas
    Serial.println("📊 ESTADÍSTICAS DE LECTURA:");
    Serial.printf("  Promedio ADC: %d / 4095\n", avgADC);
    Serial.printf("  Mínimo:       %d\n", minVal);
    Serial.printf("  Máximo:       %d\n", maxVal);
    Serial.printf("  Variación:    %d\n", maxVal - minVal);
    Serial.printf("  Voltaje:      %.3f V\n", avgVoltage);
    Serial.println();
    
    // Análisis de estado
    Serial.println("🔍 ANÁLISIS:");
    
    if (avgADC < 50) {
        Serial.println("  ❌ ERROR CRÍTICO: ADC muy bajo");
        Serial.println("     → Posible cortocircuito a GND");
        Serial.println("     → Verificar conexión del cable de señal");
        Serial.println("     → Verificar que el pin GPIO33 esté libre");
        
    } else if (avgADC < MIN_CONNECTED_ADC) {
        Serial.println("  ❌ SENSOR DESCONECTADO (ADC < 150)");
        Serial.println("     POSIBLES CAUSAS:");
        Serial.println("     1. Sensor NO tiene alimentación 5V");
        Serial.println("     2. Cable de señal desconectado");
        Serial.println("     3. Sensor NO está sumergido en agua");
        Serial.println("     4. Sensor dañado");
        Serial.println();
        Serial.println("     VERIFICAR:");
        Serial.println("     ✓ LED del sensor encendido (si tiene)");
        Serial.println("     ✓ Medir con multímetro: VCC = 5V");
        Serial.println("     ✓ Medir señal con multímetro: ~0.5-3.0V");
        Serial.println("     ✓ Sensor DEBE estar en agua");
        
    } else if (avgADC > MAX_CONNECTED_ADC) {
        Serial.println("  ❌ SEÑAL FUERA DE RANGO (ADC > 4000)");
        Serial.println("     → Posible cable desconectado");
        Serial.println("     → Verificar conexión del cable");
        
    } else {
        Serial.println("  ✅ SENSOR CONECTADO - Lectura válida");
        Serial.println();
        
        // Calcular TDS estimado
        // Fórmula del sensor SEN0244: TDS = (voltaje * factor) / 2
        // Factor de compensación para temperatura de 25°C
        float compensationCoefficient = 1.0 + 0.02 * (25.0 - 25.0);
        float compensationVoltage = avgVoltage / compensationCoefficient;
        float tdsValue = (133.42 * compensationVoltage * compensationVoltage * compensationVoltage 
                         - 255.86 * compensationVoltage * compensationVoltage 
                         + 857.39 * compensationVoltage) * 0.5;
        
        Serial.printf("  📈 TDS Estimado: %.0f ppm\n", tdsValue);
        Serial.println();
        
        if (tdsValue < 50) {
            Serial.println("  💧 Agua muy pura (destilada/desionizada)");
        } else if (tdsValue < 200) {
            Serial.println("  💧 Agua de baja mineralización");
        } else if (tdsValue < 500) {
            Serial.println("  💧 Agua potable normal");
        } else if (tdsValue < 1000) {
            Serial.println("  💧 Agua con alta mineralización");
        } else if (tdsValue < 2000) {
            Serial.println("  🌱 Solución nutritiva para hidroponía");
        } else {
            Serial.println("  ⚠️ TDS muy alto - revisar sensor");
        }
    }
    
    // Mostrar muestras individuales (solo primeras 10)
    Serial.println();
    Serial.println("📋 MUESTRAS INDIVIDUALES (primeras 10):");
    Serial.print("  ");
    for (int i = 0; i < 10; i++) {
        Serial.printf("%4d ", readings[i]);
        if ((i + 1) % 5 == 0) {
            Serial.println();
            if (i < 9) Serial.print("  ");
        }
    }
    
    Serial.println();
    Serial.println("========================================");
    Serial.println();
    
    delay(3000); // Esperar 3 segundos entre lecturas
}
