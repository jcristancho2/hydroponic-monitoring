#ifndef SERIAL_COMMANDS_H
#define SERIAL_COMMANDS_H

#include <Arduino.h>

// Forward declarations
class PHSensor;
class PumpController;
class TDSSensor;
class LevelSensor;
class LDRSensor;

class SerialCommands
{
public:
    // Constructor
    SerialCommands();

    // Inicialización con referencias a los módulos
    void begin(PHSensor *phSensor, PumpController *pumpController, TDSSensor *tdsSesor);

    // Procesamiento de comandos
    void processCommands();

private:
    PHSensor *phSensor;
    PumpController *pumpController;
    TDSSensor *tdsSesor;

    void processCommand(String command);
    void printHelp();
};

#endif // SERIAL_COMMANDS_H