#ifndef PUMP_CONTROLLER_H
#define PUMP_CONTROLLER_H

#include <Arduino.h>

class PumpController
{
public:
    enum DoseType
    {
        NONE,
        DOSE_MINUS,
        DOSE_PLUS
    };
    enum DoseState
    {
        IDLE,
        DOSING
    };

    struct Config
    {
        float phMin = 5.8f;                  // pH mínimo
        float phMax = 6.8f;                  // pH máximo
        float phLowHyst = 6.0f;              // Histéresis baja
        float phHighHyst = 6.6f;             // Histéresis alta
        unsigned long doseOnMs = 10000;      // 10s por pulso
        unsigned long maxSessionMs = 600000; // 10 min máximo
        unsigned long recheckDelayMs = 0;    // Sin delay entre pulsos
        bool relayActiveLow = true;          // true: LOW=ON, false: HIGH=ON
    };

    // Constructor
    PumpController(uint8_t relayCirc, uint8_t relayPhMinus, uint8_t relayPhPlus);

    // Inicialización
    void begin();
    void begin(const Config &config);

    // Control automático
    void update(float ph, bool levelMinusOK, bool levelPlusOK);

    // Control manual
    void forcePumpMinus(bool on);
    void forcePumpPlus(bool on);
    void forceCirculation(bool on);

    // Estado
    DoseType getCurrentDoseType() const { return doseType; }
    DoseState getCurrentDoseState() const { return doseState; }
    bool isPumpMinusOn() const;
    bool isPumpPlusOn() const;
    bool isCirculationOn() const;

    // Configuración
    void setConfig(const Config &config) { this->config = config; }
    Config getConfig() const { return config; }
    void setRelayLogic(bool activeLow) { config.relayActiveLow = activeLow; }

    // Información de timing
    unsigned long getElapsedPulse() const;
    unsigned long getElapsedSession() const;

private:
    uint8_t relayCircPin, relayMinusPin, relayPlusPin;
    Config config;
    DoseType doseType;
    DoseState doseState;
    unsigned long doseStamp;
    unsigned long sessionStart;

    void relayWrite(uint8_t pin, bool on);
    void stopAllDosing();
};

#endif // PUMP_CONTROLLER_H