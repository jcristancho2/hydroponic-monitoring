#ifndef LEVEL_SENSOR_H
#define LEVEL_SENSOR_H

#include <Arduino.h>

class LevelSensor
{
public:
    // Constructores
    LevelSensor(); // Constructor por defecto
    LevelSensor(uint8_t pin, bool highMeansOK = true);

    // Inicialización
    void begin();

    // Lectura
    bool isLevelOK();
    int getRawReading();

    // Configuración
    void setLogic(bool highMeansOK) { this->highMeansOK = highMeansOK; }
    bool getLogic() const { return highMeansOK; }

private:
    uint8_t pin;
    bool highMeansOK; // true: HIGH = nivel OK, false: LOW = nivel OK
};

// Clase para múltiples sensores de nivel
class MultiLevelSensor
{
public:
    struct SensorConfig
    {
        uint8_t pin;
        bool highMeansOK;
        String name;
    };

    MultiLevelSensor();

    // Configuración
    void addSensor(uint8_t pin, bool highMeansOK, const String &name);
    void begin();

    // Lectura
    bool isLevelOK(uint8_t index);
    bool isLevelOK(const String &name);
    int getRawReading(uint8_t index);
    int getRawReading(const String &name);

    // Información
    uint8_t getSensorCount() const { return sensorCount; }
    String getSensorName(uint8_t index);

private:
    static constexpr uint8_t MAX_SENSORS = 8;
    LevelSensor sensors[MAX_SENSORS];
    String sensorNames[MAX_SENSORS];
    uint8_t sensorCount;

    int findSensorByName(const String &name);
};

#endif // LEVEL_SENSOR_H