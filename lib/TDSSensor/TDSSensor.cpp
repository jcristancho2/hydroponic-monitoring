#include "TDSSensor.h"

TDSSensor::TDSSensor(uint8_t pin)
    : pin(pin), tdsValue(0.0f), connected(false), initialized(false),
      rawADC(0), temperature(25.0f), lastUpdate(0), updateInterval(1000)
{
}

void TDSSensor::begin()
{
    analogReadResolution(12); // Asegurar resolución de 12 bits
    analogSetPinAttenuation(pin, ADC_11db);

    gravityTds.setPin(pin);
    gravityTds.setAref(3.3);
    gravityTds.setAdcRange(4096);
    gravityTds.begin();

    // Leer un valor inicial para verificar que funciona
    rawADC = analogRead(pin);
    float initialVoltage = (rawADC * 3.3f) / 4096.0f;
    
    initialized = true;
    Serial.printf("TDSSensor: Inicializado correctamente (ADC inicial: %d, Voltaje: %.3fV)\n", 
                  rawADC, initialVoltage);
}

void TDSSensor::update()
{
    if (!initialized)
    {
        Serial.println("TDSSensor: Error - No inicializado");
        return;
    }

    rawADC = analogRead(pin);
    bool wasConnected = connected;
    checkConnection();

    // Diagnóstico cuando cambia el estado de conexión
    if (wasConnected != connected)
    {
        if (connected)
        {
            Serial.printf("TDSSensor: Sensor conectado (ADC: %d)\n", rawADC);
        }
        else
        {
            Serial.printf("TDSSensor: Sensor desconectado (ADC: %d)\n", rawADC);
            if (rawADC < MIN_CONNECTED_ADC)
            {
                Serial.printf("  → ADC muy bajo (< %d). Verificar:\n", MIN_CONNECTED_ADC);
                Serial.println("     - Sensor tiene alimentación 5V?");
                Serial.println("     - Cable de señal conectado a GPIO33?");
                Serial.println("     - Sensor sumergido en agua?");
            }
            else if (rawADC > MAX_CONNECTED_ADC)
            {
                Serial.printf("  → ADC muy alto (> %d). Verificar:\n", MAX_CONNECTED_ADC);
                Serial.println("     - Cable de señal desconectado?");
                Serial.println("     - Cortocircuito a VCC?");
            }
        }
    }

    if (connected)
    {
        gravityTds.setTemperature(temperature);
        gravityTds.update();
        float rawTds = gravityTds.getTdsValue();
        
        // Validar que el valor TDS sea válido (no NaN, no infinito, y positivo)
        if (isfinite(rawTds) && rawTds >= 0.0f && rawTds <= 2000.0f)
        {
            tdsValue = rawTds;
        }
        else
        {
            // Si el valor es inválido, intentar calcular manualmente
            float voltage = (rawADC * 3.3f) / 4096.0f;
            
            // Validar voltaje antes de calcular
            if (voltage > 0.0f && voltage < 3.3f)
            {
                // Fórmula alternativa para TDS basada en voltaje
                // Compensación por temperatura
                float compensationCoefficient = 1.0f + 0.02f * (temperature - 25.0f);
                float compensationVoltage = voltage / compensationCoefficient;
                
                // Fórmula polinómica del sensor SEN0244
                float calculatedTds = (133.42f * compensationVoltage * compensationVoltage * compensationVoltage 
                                     - 255.86f * compensationVoltage * compensationVoltage 
                                     + 857.39f * compensationVoltage) * 0.5f;
                
                if (isfinite(calculatedTds) && calculatedTds >= 0.0f)
                {
                    tdsValue = calculatedTds;
                }
                else
                {
                    Serial.printf("TDSSensor: Error - TDS calculado inválido (%.2f). ADC: %d, Voltaje: %.3fV\n", 
                                 calculatedTds, rawADC, voltage);
                    tdsValue = 0.0f;
                }
            }
            else
            {
                Serial.printf("TDSSensor: Error - Voltaje inválido (%.3fV). ADC: %d\n", voltage, rawADC);
                tdsValue = 0.0f;
            }
        }
    }
    else
    {
        tdsValue = 0.0f;
    }

    lastUpdate = millis();
}

void TDSSensor::checkConnection()
{
    connected = (rawADC > MIN_CONNECTED_ADC && rawADC < MAX_CONNECTED_ADC);
}

void TDSSensor::setTemperature(float temp)
{
    temperature = temp;
}

bool TDSSensor::shouldUpdate()
{
    return (millis() - lastUpdate >= updateInterval);
}