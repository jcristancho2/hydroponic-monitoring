#include "LDRSensor.h"

LDRSensor::LDRSensor(uint8_t pin)
    : pin(pin), rawValue(0), voltage(0.0f), lightLevel(DARK),
      lastUpdate(0), updateInterval(1000), sunThreshold(3000),
      darkThreshold(500), lowThreshold(1500), mediumThreshold(2500), brightThreshold(3500)
{
}

void LDRSensor::begin()
{
    analogSetPinAttenuation(pin, ADC_11db);
    Serial.printf("LDRSensor: Inicializado en pin %d\n", pin);
}

void LDRSensor::update()
{
    rawValue = analogRead(pin);
    voltage = (rawValue * VREF) / ADC_RES;
    calculateLightLevel();
    lastUpdate = millis();
}

void LDRSensor::calculateLightLevel()
{
    if (rawValue < darkThreshold)
    {
        lightLevel = DARK;
    }
    else if (rawValue < lowThreshold)
    {
        lightLevel = LOW_LIGHT;
    }
    else if (rawValue < mediumThreshold)
    {
        lightLevel = MEDIUM_LIGHT;
    }
    else if (rawValue < brightThreshold)
    {
        lightLevel = BRIGHT_LIGHT;
    }
    else
    {
        lightLevel = VERY_BRIGHT;
    }
}

String LDRSensor::getLightLevelString() const
{
    switch (lightLevel)
    {
    case DARK:
        return "Oscuro";
    case LOW_LIGHT:
        return "Poca luz";
    case MEDIUM_LIGHT:
        return "Luz media";
    case BRIGHT_LIGHT:
        return "Luz intensa";
    case VERY_BRIGHT:
        return "Muy brillante";
    default:
        return "Desconocido";
    }
}

void LDRSensor::setThresholds(int dark, int low, int medium, int bright)
{
    darkThreshold = dark;
    lowThreshold = low;
    mediumThreshold = medium;
    brightThreshold = bright;

    Serial.printf("LDRSensor: Umbrales actualizados - Oscuro: %d, Bajo: %d, Medio: %d, Alto: %d\n",
                  dark, low, medium, bright);
}

bool LDRSensor::shouldUpdate()
{
    return (millis() - lastUpdate >= updateInterval);
}