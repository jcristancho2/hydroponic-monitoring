#include "TDSSensor.h"

TDSSensor::TDSSensor(uint8_t pin)
    : pin(pin), tdsValue(0.0f), connected(false), initialized(false),
      rawADC(0), temperature(25.0f), lastUpdate(0), updateInterval(1000)
{
}

void TDSSensor::begin()
{
    analogSetPinAttenuation(pin, ADC_11db);

    gravityTds.setPin(pin);
    gravityTds.setAref(3.3);
    gravityTds.setAdcRange(4096);
    gravityTds.begin();

    initialized = true;
    Serial.println("TDSSensor: Inicializado correctamente");
}

void TDSSensor::update()
{
    if (!initialized)
    {
        Serial.println("TDSSensor: Error - No inicializado");
        return;
    }

    rawADC = analogRead(pin);
    checkConnection();

    if (connected)
    {
        gravityTds.setTemperature(temperature);
        gravityTds.update();
        tdsValue = gravityTds.getTdsValue();
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