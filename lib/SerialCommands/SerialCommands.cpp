#include "SerialCommands.h"
#include "PHSensor.h"
#include "PumpController.h"
#include "TDSSensor.h"

SerialCommands::SerialCommands() : phSensor(nullptr), pumpController(nullptr), tdsSesor(nullptr)
{
}

void SerialCommands::begin(PHSensor *phSensor, PumpController *pumpController, TDSSensor *tdsSesor)
{
    this->phSensor = phSensor;
    this->pumpController = pumpController;
    this->tdsSesor = tdsSesor;

    Serial.println("SerialCommands: Inicializado");
    printHelp();
}

void SerialCommands::processCommands()
{
    if (!Serial.available())
        return;

    String command = Serial.readStringUntil('\n');
    command.trim();
    command.toUpperCase();

    processCommand(command);
}

void SerialCommands::processCommand(String cmd)
{
    if (cmd.startsWith("PHCAL"))
    {
        if (!phSensor)
        {
            Serial.println("Error: PHSensor no inicializado");
            return;
        }

        int comma = cmd.indexOf(',');
        if (comma < 0)
        {
            Serial.println("Uso: PHCAL,7 | PHCAL,4 | PHCAL,10");
            return;
        }

        float target = cmd.substring(comma + 1).toFloat();
        if (target != 4.0f && target != 7.0f && target != 10.0f)
        {
            Serial.println("Solo pH 4, 7 o 10 permitidos");
            return;
        }

        phSensor->calibratePoint(target);
    }
    else if (cmd == "PHSAVE")
    {
        if (phSensor)
        {
            phSensor->saveCalibration();
        }
    }
    else if (cmd == "PHRESET")
    {
        if (phSensor)
        {
            phSensor->resetCalibration();
        }
    }
    else if (cmd == "PHEEPRCLR")
    {
        if (phSensor)
        {
            phSensor->clearEEPROM();
        }
    }
    else if (cmd.startsWith("SETT,"))
    {
        float temp = cmd.substring(5).toFloat();
        if (temp > -10 && temp < 80)
        {
            if (phSensor)
                phSensor->setTemperature(temp);
            if (tdsSesor)
                tdsSesor->setTemperature(temp);
            Serial.printf("Temperatura establecida: %.2f °C\n", temp);
        }
        else
        {
            Serial.println("Temperatura fuera de rango (-10 a 80°C)");
        }
    }
    else if (cmd == "PPLUS,ON")
    {
        if (pumpController)
        {
            pumpController->forcePumpPlus(true);
        }
    }
    else if (cmd == "PPLUS,OFF")
    {
        if (pumpController)
        {
            pumpController->forcePumpPlus(false);
        }
    }
    else if (cmd == "PMINUS,ON")
    {
        if (pumpController)
        {
            pumpController->forcePumpMinus(true);
        }
    }
    else if (cmd == "PMINUS,OFF")
    {
        if (pumpController)
        {
            pumpController->forcePumpMinus(false);
        }
    }
    else if (cmd == "RELCFG,LOW")
    {
        if (pumpController)
        {
            pumpController->setRelayLogic(true);
            Serial.println("Relés configurados: LOW = ON");
        }
    }
    else if (cmd == "RELCFG,HIGH")
    {
        if (pumpController)
        {
            pumpController->setRelayLogic(false);
            Serial.println("Relés configurados: HIGH = ON");
        }
    }
    else if (cmd == "RESET")
    {
        Serial.println("\n⚠️ COMANDO DE REINICIO RECIBIDO");
        Serial.println("Reiniciando ESP32 en 1 segundo...");
        delay(1000);
        ESP.restart();
    }
    else if (cmd == "EMERGENCY" || cmd == "EMERGENCY,ON")
    {
        if (pumpController)
        {
            pumpController->emergencyStop();
        }
    }
    else if (cmd == "EMERGENCY,OFF" || cmd == "RESUME")
    {
        if (pumpController)
        {
            pumpController->emergencyResume();
        }
    }
    else if (cmd == "HELP")
    {
        printHelp();
    }
    else
    {
        Serial.println("Comando no reconocido. Escribe HELP para ver comandos disponibles.");
    }
}

void SerialCommands::printHelp()
{
    Serial.println("\n=== COMANDOS DISPONIBLES ===");
    Serial.println("Calibración pH:");
    Serial.println("  PHCAL,7    - Calibrar punto neutro (pH 7)");
    Serial.println("  PHCAL,4    - Calibrar punto ácido (pH 4)");
    Serial.println("  PHCAL,10   - Calibrar punto básico (pH 10)");
    Serial.println("  PHSAVE     - Guardar calibración");
    Serial.println("  PHRESET    - Restablecer calibración");
    Serial.println("  PHEEPRCLR  - Borrar EEPROM");
    Serial.println();
    Serial.println("Configuración:");
    Serial.println("  SETT,25.5  - Establecer temperatura");
    Serial.println("  RELCFG,LOW - Relés activos en LOW");
    Serial.println("  RELCFG,HIGH- Relés activos en HIGH");
    Serial.println();
    Serial.println("Control manual:");
    Serial.println("  PPLUS,ON   - Encender bomba pH+");
    Serial.println("  PPLUS,OFF  - Apagar bomba pH+");
    Serial.println("  PMINUS,ON  - Encender bomba pH-");
    Serial.println("  PMINUS,OFF - Apagar bomba pH-");
    Serial.println();
    Serial.println("Sistema:");
    Serial.println("  RESET      - Reiniciar ESP32");
    Serial.println("  EMERGENCY  - Activar parada de emergencia");
    Serial.println("  RESUME     - Desactivar parada de emergencia");
    Serial.println("  HELP       - Mostrar esta ayuda");
    Serial.println("===============================\n");
}