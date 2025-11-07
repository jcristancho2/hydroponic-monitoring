#ifndef LDR_SENSOR_H
#define LDR_SENSOR_H

#include <Arduino.h>

class LDRSensor
{
public:
    enum LightLevel
    {
        DARK,
        LOW_LIGHT,
        MEDIUM_LIGHT,
        BRIGHT_LIGHT,
        VERY_BRIGHT
    };

    // Constructor
    LDRSensor(uint8_t pin);

    // Inicialización
    void begin();

    // Actualización
    void update();

    // Getters
    int getRawValue() const { return rawValue; }
    float getVoltage() const { return voltage; }
    LightLevel getLightLevel() const { return lightLevel; }
    String getLightLevelString() const;
    bool isBrightSun() const { return rawValue >= sunThreshold; }

    // Configuración
    void setSunThreshold(int threshold) { sunThreshold = threshold; }
    void setUpdateInterval(unsigned long interval) { updateInterval = interval; }
    void setThresholds(int dark, int low, int medium, int bright);

    // Control de timing
    bool shouldUpdate();

private:
    uint8_t pin;
    int rawValue;
    float voltage;
    LightLevel lightLevel;
    unsigned long lastUpdate;
    unsigned long updateInterval;
    int sunThreshold;

    // Umbrales para niveles de luz
    int darkThreshold;
    int lowThreshold;
    int mediumThreshold;
    int brightThreshold;

    static constexpr float VREF = 3.3f;
    static constexpr float ADC_RES = 4095.0f;

    void calculateLightLevel();
};

#endif // LDR_SENSOR_H