#ifndef PH_SENSOR_H
#define PH_SENSOR_H

#include <Arduino.h>
#include <EEPROM.h>

class PHSensor
{
public:
    struct Calibration
    {
        float v_at_ph7;    // Voltaje real del módulo a pH 7 (V)
        float v_per_ph_25; // Pendiente V/pH @25°C (positiva)
        bool valid;
    };

    // Constructor
    PHSensor(uint8_t pin, int eepromAddr = 0);

    // Inicialización
    void begin();

    // Lectura de pH
    void update();
    float getFilteredPH() const { return phFiltered; }
    float getInstantPH() const { return phInstant; }
    float getVoltage() const { return lastVoltage; }

    // Calibración
    void calibratePoint(float targetPH, float temperature = 25.0f);
    void saveCalibration();
    void loadCalibration();
    void resetCalibration();
    void clearEEPROM();
    bool isCalibrationValid() const { return calibration.valid; }

    // Configuración
    void setTemperature(float temp) { temperature = temp; }
    void setFilterAlpha(float alpha) { filterAlpha = alpha; }
    void setDividerK(float k) { dividerK = k; }

    // Obtener calibración
    Calibration getCalibration() const { return calibration; }

private:
    uint8_t pin;
    int eepromAddr;
    float temperature;
    float phFiltered;
    float phInstant;
    float lastVoltage;
    float filterAlpha;
    float dividerK;
    Calibration calibration;

    // Constantes
    static constexpr float VREF = 3.3f;
    static constexpr float ADC_RES = 4095.0f;
    static constexpr float MIN_SLOPE = 0.05f;
    static constexpr float MAX_SLOPE = 0.40f;
    static constexpr float MIN_V_AT_7 = 0.2f;
    static constexpr float MAX_V_AT_7 = 4.0f;

    // Métodos privados
    float readVoltageMedianAvg(uint8_t nSamples = 10);
    float computePH(float voltage, float tempC);
    void sanitizeCalibration();
    float clamp(float x, float lo, float hi);
};

#endif // PH_SENSOR_H