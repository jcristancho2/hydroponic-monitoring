#ifndef PIN_CONFIG_H
#define PIN_CONFIG_H

/**
 * @file pin_config.h
 * @brief Configuración de pines para el sistema hidropónico
 *
 * Este archivo centraliza todas las asignaciones de pines del ESP32
 * para facilitar la configuración y modificación del hardware.
 *
 * @note ESP32 DEVKIT V1 - 30 pines disponibles
 * @warning Evitar usar pines de solo entrada: 34, 35, 36, 39
 * @warning Evitar pines que causan problemas en boot: 0, 2, 5, 12, 15
 */

// ============================================================================
// CONFIGURACIÓN DE PINES - SENSORES ANALÓGICOS (ADC1 - WiFi compatible)
// ============================================================================

/**
 * @brief Sensor de pH PH4502C
 * @note Requiere pin ADC (12-bit, 0-3.3V)
 * @range Pines ADC1 recomendados: 32, 33, 34, 35, 36, 39
 */
#define PH_PIN 32

/**
 * @brief Sensor TDS SEN0244
 * @note Requiere pin ADC (12-bit, 0-3.3V)
 * @range Pines ADC1 recomendados: 32, 33, 34, 35, 36, 39
 */
#define TDS_PIN 33

/**
 * @brief Sensor de luz LDR
 * @note Requiere pin ADC (12-bit, 0-3.3V)
 * @range Pines ADC1 recomendados: 32, 33, 34, 35, 36, 39
 */
#define LDR_PIN 35

// ============================================================================
// CONFIGURACIÓN DE PINES - SENSORES DIGITALES
// ============================================================================

/**
 * @brief Sensor de nivel SEN0205 - Tanque pH- (ácido)
 * @note Sensor digital de nivel (LOW=OK, HIGH=BAJO)
 * @range Cualquier pin GPIO digital
 */
#define LVL_PH_MINUS 18

/**
 * @brief Sensor de nivel SEN0205 - Tanque pH+ (base)
 * @note Sensor digital de nivel (LOW=OK, HIGH=BAJO)
 * @range Cualquier pin GPIO digital
 */
#define LVL_PH_PLUS 21

// ============================================================================
// CONFIGURACIÓN DE PINES - CONTROL DE BOMBAS (RELÉS)
// ============================================================================

/**
 * @brief Relé para bomba de circulación
 * @note Control digital de bomba (HIGH=ON, LOW=OFF)
 * @range Cualquier pin GPIO digital
 */
#define RELAY_CIRC 23

/**
 * @brief Relé para bomba de dosificación pH- (ácido)
 * @note Control digital de bomba (HIGH=ON, LOW=OFF)
 * @range Cualquier pin GPIO digital
 */
#define RELAY_PH_MINUS 25

/**
 * @brief Relé para bomba de dosificación pH+ (base)
 * @note Control digital de bomba (HIGH=ON, LOW=OFF)
 * @range Cualquier pin GPIO digital
 */
#define RELAY_PH_PLUS 26

// ============================================================================
// CONFIGURACIÓN DE PINES - COMUNICACIÓN (OPCIONAL)
// ============================================================================

/**
 * @brief Pin para LED de estado (opcional)
 * @note LED indicador de estado del sistema
 * @range Cualquier pin GPIO digital
 */
#define STATUS_LED 2 // LED interno del ESP32

/**
 * @brief Pin para buzzer de alarma (opcional)
 * @note Buzzer para alertas sonoras
 * @range Cualquier pin GPIO digital
 */
// #define BUZZER_PIN 4  // Descomentار para habilitar

// ============================================================================
// CONFIGURACIÓN DE PINES - EXPANSIÓN FUTURA
// ============================================================================

/**
 * @brief Pines disponibles para expansión
 * @note Pines libres que pueden usarse para sensores adicionales
 */
// Pines digitales disponibles: 4, 5, 12, 13, 14, 15, 16, 17, 19, 22, 27
// Pines ADC disponibles: 34, 36, 39 (solo entrada)

/**
 * @brief Sensor de temperatura DS18B20 (futuro)
 * @note Para medición independiente de temperatura
 */
// #define TEMP_SENSOR_PIN 4

/**
 * @brief Sensor de flujo (futuro)
 * @note Para monitorear caudal de agua
 */
// #define FLOW_SENSOR_PIN 5

/**
 * @brief Válvula solenoide principal (futuro)
 * @note Control de flujo principal
 */
// #define MAIN_VALVE_PIN 12

// ============================================================================
// VALIDACIÓN DE PINES
// ============================================================================

// Verificar que no hay conflictos de pines
#if (PH_PIN == TDS_PIN) || (PH_PIN == LDR_PIN) || (TDS_PIN == LDR_PIN)
#error "Error: Pines ADC en conflicto. Revisar configuración."
#endif

#if (LVL_PH_MINUS == LVL_PH_PLUS)
#error "Error: Pines de sensores de nivel en conflicto."
#endif

#if (RELAY_CIRC == RELAY_PH_MINUS) || (RELAY_CIRC == RELAY_PH_PLUS) || (RELAY_PH_MINUS == RELAY_PH_PLUS)
#error "Error: Pines de relés en conflicto."
#endif

// ============================================================================
// DOCUMENTACIÓN DE PINES ESP32
// ============================================================================

/**
 * PINOUT ESP32 DEVKIT V1:
 *
 * Lado izquierdo:        Lado derecho:
 * EN                     D23  (GPIO23)
 * VP   (GPIO36) ADC1_0   D22  (GPIO22)
 * VN   (GPIO39) ADC1_3   TX0  (GPIO1)
 * D34  (GPIO34) ADC1_6   RX0  (GPIO3)
 * D35  (GPIO35) ADC1_7   D21  (GPIO21)
 * D32  (GPIO32) ADC1_4   D19  (GPIO19)
 * D33  (GPIO33) ADC1_5   D18  (GPIO18)
 * D25  (GPIO25) ADC2_8   D5   (GPIO5)
 * D26  (GPIO26) ADC2_9   TX2  (GPIO17)
 * D27  (GPIO27) ADC2_7   RX2  (GPIO16)
 * D14  (GPIO14) ADC2_6   D4   (GPIO4)
 * D12  (GPIO12) ADC2_5   D2   (GPIO2)
 * D13  (GPIO13) ADC2_4   D15  (GPIO15)
 * GND                    GND
 * VIN                    3V3
 *
 * NOTAS IMPORTANTES:
 * - ADC1 (pines 32,33,34,35,36,39): Compatible con WiFi
 * - ADC2 (pines 0,2,4,12,13,14,15,25,26,27): NO compatible con WiFi
 * - Pines solo entrada: 34, 35, 36, 39
 * - Pines problemáticos en boot: 0, 2, 5, 12, 15
 * - Pin 2: LED interno
 */

#endif // PIN_CONFIG_H