#include "PumpController.h"

PumpController::PumpController(uint8_t relayCirc, uint8_t relayPhMinus, uint8_t relayPhPlus)
    : relayCircPin(relayCirc), relayMinusPin(relayPhMinus), relayPlusPin(relayPhPlus),
      doseType(NONE), doseState(IDLE), doseStamp(0), sessionStart(0), emergencyMode(false)
{
}

void PumpController::begin()
{
    Config defaultConfig;
    begin(defaultConfig);
}

void PumpController::begin(const Config &config)
{
    this->config = config;

    pinMode(relayCircPin, OUTPUT);
    pinMode(relayMinusPin, OUTPUT);
    pinMode(relayPlusPin, OUTPUT);

    // Configuración inicial: circulación ON, dosificación OFF
    relayWrite(relayCircPin, true);
    relayWrite(relayMinusPin, false);
    relayWrite(relayPlusPin, false);

    Serial.println("PumpController: Inicializado - Circulación ON, dosificación OFF");
}

void PumpController::update(float ph, bool levelMinusOK, bool levelPlusOK)
{
    // Si está en modo emergencia, no ejecutar control automático
    if (emergencyMode)
    {
        return;
    }

    unsigned long now = millis();

    switch (doseState)
    {
    case IDLE:
    {
        // ¿Necesita subir pH?
        if (ph < config.phMin && levelPlusOK)
        {
            doseType = DOSE_PLUS;
            relayWrite(relayPlusPin, true);
            relayWrite(relayMinusPin, false);
            doseStamp = now;
            sessionStart = now;
            doseState = DOSING;
            Serial.printf("PumpController: pH+ ON - pH=%.2f < MIN=%.2f, Nivel=%s\n",
                          ph, config.phMin, levelPlusOK ? "OK" : "BAJO");
        }
        // ¿Necesita bajar pH?
        else if (ph > config.phMax && levelMinusOK)
        {
            doseType = DOSE_MINUS;
            relayWrite(relayMinusPin, true);
            relayWrite(relayPlusPin, false);
            doseStamp = now;
            sessionStart = now;
            doseState = DOSING;
            Serial.printf("PumpController: pH- ON - pH=%.2f > MAX=%.2f, Nivel=%s\n",
                          ph, config.phMax, levelMinusOK ? "OK" : "BAJO");
        }
        else
        {
            // Alertas de nivel bajo
            if (ph < config.phMin && !levelPlusOK)
            {
                Serial.printf("PumpController: ALERTA - pH+ bloqueado. pH=%.2f < MIN=%.2f pero Nivel=BAJO\n",
                              ph, config.phMin);
            }
            if (ph > config.phMax && !levelMinusOK)
            {
                Serial.printf("PumpController: ALERTA - pH- bloqueado. pH=%.2f > MAX=%.2f pero Nivel=BAJO\n",
                              ph, config.phMax);
            }
        }
        break;
    }

    case DOSING:
    {
        // Seguridad: tiempo máximo de sesión
        if (now - sessionStart >= config.maxSessionMs)
        {
            stopAllDosing();
            Serial.println("PumpController: ALERTA - Tiempo máximo alcanzado. Apagado por seguridad.");
            break;
        }

        // ¿Terminó el pulso?
        if (now - doseStamp >= config.doseOnMs)
        {
            bool objetivoAlcanzado = false;

            if (doseType == DOSE_PLUS)
            {
                objetivoAlcanzado = (ph >= config.phLowHyst);
                if (!levelPlusOK)
                {
                    Serial.println("PumpController: Depósito pH+ BAJO durante dosificación");
                    objetivoAlcanzado = true;
                }
            }
            else if (doseType == DOSE_MINUS)
            {
                objetivoAlcanzado = (ph <= config.phHighHyst);
                if (!levelMinusOK)
                {
                    Serial.println("PumpController: Depósito pH- BAJO durante dosificación");
                    objetivoAlcanzado = true;
                }
            }

            if (objetivoAlcanzado)
            {
                stopAllDosing();
                Serial.println("PumpController: Objetivo alcanzado → OFF");
            }
            else
            {
                // Continuar con otro pulso
                doseStamp = now + config.recheckDelayMs;
                Serial.printf("PumpController: Continúa %s otros 10s\n",
                              (doseType == DOSE_PLUS) ? "pH+" : "pH-");
            }
        }
        break;
    }
    }
}

void PumpController::forcePumpMinus(bool on)
{
    relayWrite(relayMinusPin, on);
    if (on)
    {
        doseType = DOSE_MINUS;
        doseState = DOSING;
        doseStamp = millis();
        sessionStart = doseStamp;
        relayWrite(relayPlusPin, false);
        Serial.println("PumpController: MANUAL - pH- ON");
    }
    else
    {
        if (doseType == DOSE_MINUS)
        {
            doseType = NONE;
            doseState = IDLE;
        }
        Serial.println("PumpController: MANUAL - pH- OFF");
    }
}

void PumpController::forcePumpPlus(bool on)
{
    relayWrite(relayPlusPin, on);
    if (on)
    {
        doseType = DOSE_PLUS;
        doseState = DOSING;
        doseStamp = millis();
        sessionStart = doseStamp;
        relayWrite(relayMinusPin, false);
        Serial.println("PumpController: MANUAL - pH+ ON");
    }
    else
    {
        if (doseType == DOSE_PLUS)
        {
            doseType = NONE;
            doseState = IDLE;
        }
        Serial.println("PumpController: MANUAL - pH+ OFF");
    }
}

void PumpController::forceCirculation(bool on)
{
    relayWrite(relayCircPin, on);
    Serial.printf("PumpController: Circulación %s\n", on ? "ON" : "OFF");
}

bool PumpController::isPumpMinusOn() const
{
    int state = digitalRead(relayMinusPin);
    return config.relayActiveLow ? (state == LOW) : (state == HIGH);
}

bool PumpController::isPumpPlusOn() const
{
    int state = digitalRead(relayPlusPin);
    return config.relayActiveLow ? (state == LOW) : (state == HIGH);
}

bool PumpController::isCirculationOn() const
{
    int state = digitalRead(relayCircPin);
    return config.relayActiveLow ? (state == LOW) : (state == HIGH);
}

bool PumpController::isPumpMinusActive() const
{
    // Retorna true si pH- está siendo controlado (independiente del pulso)
    if (doseState != DOSING)
        return false;
    return doseType == DOSE_MINUS;
}

bool PumpController::isPumpPlusActive() const
{
    // Retorna true si pH+ está siendo controlado (independiente del pulso)
    if (doseState != DOSING)
        return false;
    return doseType == DOSE_PLUS;
}

bool PumpController::isDosingActive() const
{
    // Retorna true si hay alguna dosificación en progreso
    return (doseState == DOSING);
}

unsigned long PumpController::getElapsedPulse() const
{
    if (doseState != DOSING)
        return 0;
    unsigned long elapsed = millis() - doseStamp;
    return (elapsed >= config.doseOnMs) ? config.doseOnMs : elapsed;
}

unsigned long PumpController::getElapsedSession() const
{
    if (doseState != DOSING)
        return 0;
    return millis() - sessionStart;
}

void PumpController::relayWrite(uint8_t pin, bool on)
{
    if (config.relayActiveLow)
    {
        digitalWrite(pin, on ? LOW : HIGH);
    }
    else
    {
        digitalWrite(pin, on ? HIGH : LOW);
    }
}

void PumpController::stopAllDosing()
{
    relayWrite(relayMinusPin, false);
    relayWrite(relayPlusPin, false);
    doseType = NONE;
    doseState = IDLE;
}

void PumpController::emergencyStop()
{
    if (emergencyMode)
        return; // Ya está en emergencia

    emergencyMode = true;
    // Detener todas las bombas inmediatamente
    relayWrite(relayCircPin, false);
    relayWrite(relayMinusPin, false);
    relayWrite(relayPlusPin, false);
    doseType = NONE;
    doseState = IDLE;
    Serial.println("🚨🚨🚨 MODO EMERGENCIA ACTIVADO - TODAS LAS BOMBAS DETENIDAS 🚨🚨🚨");
}

void PumpController::emergencyResume()
{
    if (!emergencyMode)
        return; // No está en emergencia

    emergencyMode = false;
    // Restaurar circulación (las bombas de dosificación quedan OFF)
    relayWrite(relayCircPin, true);
    doseType = NONE;
    doseState = IDLE;
    Serial.println("✅ MODO EMERGENCIA DESACTIVADO - Sistema restaurado");
}