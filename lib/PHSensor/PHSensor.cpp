#include "PHSensor.h"

PHSensor::PHSensor(uint8_t pin, int eepromAddr)
    : pin(pin), eepromAddr(eepromAddr), temperature(25.0f),
      phFiltered(7.0f), phInstant(7.0f), lastVoltage(0.0f),
      filterAlpha(0.25f), dividerK(1.0f)
{

    // Valores por defecto de calibración
    calibration.v_at_ph7 = 2.50f;
    calibration.v_per_ph_25 = 0.18f;
    calibration.valid = false;
}

void PHSensor::begin()
{
    analogReadResolution(12);
    analogSetPinAttenuation(pin, ADC_11db);
    loadCalibration();
    sanitizeCalibration();
}

void PHSensor::update()
{
    lastVoltage = readVoltageMedianAvg(10);
    float modulVoltage = lastVoltage * dividerK;
    phInstant = computePH(modulVoltage, temperature);
    phFiltered = filterAlpha * phInstant + (1.0f - filterAlpha) * phFiltered;
}

float PHSensor::readVoltageMedianAvg(uint8_t nSamples)
{
    float buf[20];
    if (nSamples > 20)
        nSamples = 20;

    // Tomar muestras
    for (uint8_t i = 0; i < nSamples; i++)
    {
        int adc = analogRead(pin);
        buf[i] = (adc * VREF) / ADC_RES;
        delay(8);
    }

    // Ordenar (inserción)
    for (uint8_t i = 1; i < nSamples; i++)
    {
        float key = buf[i];
        int j = i - 1;
        while (j >= 0 && buf[j] > key)
        {
            buf[j + 1] = buf[j];
            j--;
        }
        buf[j + 1] = key;
    }

    // Promedio de la parte central
    uint8_t start = nSamples / 4;
    uint8_t end = nSamples - start;
    float sum = 0.0f;
    for (uint8_t i = start; i < end; i++)
    {
        sum += buf[i];
    }
    return sum / float(end - start);
}

float PHSensor::computePH(float voltage, float tempC)
{
    voltage = clamp(voltage, 0.0f, 5.0f);

    float slope25 = (isfinite(calibration.v_per_ph_25) && calibration.v_per_ph_25 > MIN_SLOPE)
                        ? calibration.v_per_ph_25
                        : 0.18f;
    float v7 = isfinite(calibration.v_at_ph7) ? calibration.v_at_ph7 : 2.50f;

    // Compensación por temperatura
    float slope_T = slope25 * ((tempC + 273.15f) / 298.15f);
    if (slope_T < MIN_SLOPE)
        slope_T = MIN_SLOPE;

    return 7.0f + (v7 - voltage) / slope_T;
}

void PHSensor::calibratePoint(float targetPH, float temperature)
{
    float voltageNow = readVoltageMedianAvg();
    float moduleVoltage = voltageNow * dividerK;

    if (abs(targetPH - 7.0f) < 0.01f)
    {
        // Calibración punto neutro
        calibration.v_at_ph7 = moduleVoltage;
        calibration.valid = true;
        Serial.printf("PHSensor: Calibrado V@7=%.4f V\n", moduleVoltage);
    }
    else
    {
        // Calibración pendiente
        if (!isfinite(calibration.v_at_ph7))
        {
            Serial.println("PHSensor: Error - Primero calibra el punto pH 7");
            return;
        }

        float slope25 = (calibration.v_at_ph7 - moduleVoltage) / (targetPH - 7.0f);
        if (slope25 < 0)
            slope25 = -slope25;

        if (slope25 < MIN_SLOPE || slope25 > MAX_SLOPE)
        {
            Serial.printf("PHSensor: Error - Pendiente fuera de rango: %.3f V/pH\n", slope25);
            return;
        }

        calibration.v_per_ph_25 = slope25;
        calibration.valid = true;
        Serial.printf("PHSensor: Calibrado slope=%.4f V/pH (buffer %.0f)\n", slope25, targetPH);
    }
}

void PHSensor::saveCalibration()
{
    EEPROM.put(eepromAddr, calibration);
    EEPROM.commit();
    Serial.println("PHSensor: Calibración guardada en EEPROM");
}

void PHSensor::loadCalibration()
{
    EEPROM.get(eepromAddr, calibration);
}

void PHSensor::resetCalibration()
{
    calibration.v_at_ph7 = 2.50f;
    calibration.v_per_ph_25 = 0.18f;
    calibration.valid = false;
    saveCalibration();
    Serial.println("PHSensor: Calibración restablecida a valores por defecto");
}

void PHSensor::clearEEPROM()
{
    for (int i = 0; i < sizeof(Calibration); i++)
    {
        EEPROM.write(eepromAddr + i, 0xFF);
    }
    EEPROM.commit();
    Serial.println("PHSensor: EEPROM de calibración borrada");
}

void PHSensor::sanitizeCalibration()
{
    bool bad = false;
    if (!isfinite(calibration.v_at_ph7) ||
        calibration.v_at_ph7 < MIN_V_AT_7 ||
        calibration.v_at_ph7 > MAX_V_AT_7)
        bad = true;

    if (!isfinite(calibration.v_per_ph_25) ||
        calibration.v_per_ph_25 < MIN_SLOPE ||
        calibration.v_per_ph_25 > MAX_SLOPE)
        bad = true;

    if (bad)
    {
        calibration.v_at_ph7 = 2.50f;
        calibration.v_per_ph_25 = 0.18f;
        calibration.valid = false;
        Serial.println("PHSensor: Calibración inválida. Usando valores por defecto.");
    }
}

float PHSensor::clamp(float x, float lo, float hi)
{
    if (x < lo)
        return lo;
    if (x > hi)
        return hi;
    return x;
}