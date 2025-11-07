#include "LevelSensor.h"

// Implementación de LevelSensor simple
LevelSensor::LevelSensor() : pin(255), highMeansOK(true)
{
    // Constructor por defecto con pin inválido
}

LevelSensor::LevelSensor(uint8_t pin, bool highMeansOK)
    : pin(pin), highMeansOK(highMeansOK)
{
}

void LevelSensor::begin()
{
    if (pin == 255)
    {
        Serial.println("LevelSensor: Error - Pin no configurado");
        return;
    }
    pinMode(pin, INPUT);
    Serial.printf("LevelSensor: Pin %d inicializado (lógica: %s = OK)\n",
                  pin, highMeansOK ? "HIGH" : "LOW");
}

bool LevelSensor::isLevelOK()
{
    if (pin == 255)
        return false; // Pin no configurado
    int reading = digitalRead(pin);
    return highMeansOK ? (reading == HIGH) : (reading == LOW);
}

int LevelSensor::getRawReading()
{
    if (pin == 255)
        return LOW; // Pin no configurado
    return digitalRead(pin);
}

// Implementación de MultiLevelSensor
MultiLevelSensor::MultiLevelSensor() : sensorCount(0)
{
}

void MultiLevelSensor::addSensor(uint8_t pin, bool highMeansOK, const String &name)
{
    if (sensorCount >= MAX_SENSORS)
    {
        Serial.printf("MultiLevelSensor: Error - Máximo %d sensores permitidos\n", MAX_SENSORS);
        return;
    }

    sensors[sensorCount] = LevelSensor(pin, highMeansOK);
    sensorNames[sensorCount] = name;
    sensorCount++;

    Serial.printf("MultiLevelSensor: Sensor '%s' agregado en pin %d\n", name.c_str(), pin);
}

void MultiLevelSensor::begin()
{
    for (uint8_t i = 0; i < sensorCount; i++)
    {
        sensors[i].begin();
    }
    Serial.printf("MultiLevelSensor: %d sensores inicializados\n", sensorCount);
}

bool MultiLevelSensor::isLevelOK(uint8_t index)
{
    if (index >= sensorCount)
    {
        Serial.printf("MultiLevelSensor: Error - Índice %d fuera de rango\n", index);
        return false;
    }
    return sensors[index].isLevelOK();
}

bool MultiLevelSensor::isLevelOK(const String &name)
{
    int index = findSensorByName(name);
    if (index < 0)
    {
        Serial.printf("MultiLevelSensor: Error - Sensor '%s' no encontrado\n", name.c_str());
        return false;
    }
    return sensors[index].isLevelOK();
}

int MultiLevelSensor::getRawReading(uint8_t index)
{
    if (index >= sensorCount)
    {
        Serial.printf("MultiLevelSensor: Error - Índice %d fuera de rango\n", index);
        return LOW;
    }
    return sensors[index].getRawReading();
}

int MultiLevelSensor::getRawReading(const String &name)
{
    int index = findSensorByName(name);
    if (index < 0)
    {
        Serial.printf("MultiLevelSensor: Error - Sensor '%s' no encontrado\n", name.c_str());
        return LOW;
    }
    return sensors[index].getRawReading();
}

String MultiLevelSensor::getSensorName(uint8_t index)
{
    if (index >= sensorCount)
    {
        return "INVALID";
    }
    return sensorNames[index];
}

int MultiLevelSensor::findSensorByName(const String &name)
{
    for (uint8_t i = 0; i < sensorCount; i++)
    {
        if (sensorNames[i].equals(name))
        {
            return i;
        }
    }
    return -1;
}