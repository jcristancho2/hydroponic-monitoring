#ifndef TDS_SENSOR_H
#define TDS_SENSOR_H

#include <Arduino.h>
#include "GravityTDS.h"

class TDSSensor
{
public:
    // Constructor
    TDSSensor(uint8_t pin);

    // Inicialización
    void begin();

    // Actualización
    void update();

    // Getters
    float getTDSValue() const { return tdsValue; }
    bool isConnected() const { return connected; }
    int getRawADC() const { return rawADC; }

    // Configuración
    void setTemperature(float temp);
    void setUpdateInterval(unsigned long interval) { updateInterval = interval; }

    // Control de timing
    bool shouldUpdate();

private:
    uint8_t pin;
    GravityTDS gravityTds;
    float tdsValue;
    bool connected;
    bool initialized;
    int rawADC;
    float temperature;
    unsigned long lastUpdate;
    unsigned long updateInterval;

    // Constantes para detección de conexión
    static constexpr int MIN_CONNECTED_ADC = 150;
    static constexpr int MAX_CONNECTED_ADC = 4000;

    void checkConnection();
};

#endif // TDS_SENSOR_H