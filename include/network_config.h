#ifndef NETWORK_CONFIG_H
#define NETWORK_CONFIG_H

/**
 * @file network_config.h
 * @brief Configuración de red y servicios en la nube
 *
 * Este archivo centraliza la configuración de WiFi y Firebase
 * para facilitar cambios de red y credenciales.
 */

// ============================================================================
// CONFIGURACIÓN WIFI
// ============================================================================

/**
 * @brief Nombre de la red WiFi
 * @note Cambiar por el SSID de tu red
 */
const char *WIFI_SSID = "Prueba123";

/**
 * @brief Contraseña de la red WiFi
 * @note Cambiar por la contraseña de tu red
 */
const char *WIFI_PASSWORD = "1234567890";

/**
 * @brief Timeout para conexión WiFi (ms)
 * @note Tiempo máximo para intentar conectar
 */
#define WIFI_TIMEOUT 20000

/**
 * @brief Intervalo para reconexión WiFi (ms)
 * @note Tiempo entre intentos de reconexión
 */
#define WIFI_RECONNECT_INTERVAL 30000

// ============================================================================
// CONFIGURACIÓN FIREBASE
// ============================================================================

/**
 * @brief API Key de Firebase
 * @note Obtener desde la configuración del proyecto Firebase
 */
#define API_KEY "AIzaSyAtJSvqWiw0qRB4wwDLRahKVEzH_qcwYj8"

/**
 * @brief URL de la base de datos Firebase
 * @note Formato: https://proyecto-xxxx-default-rtdb.firebaseio.com/
 */
#define DATABASE_URL "https://proyecto-hidroponico-9ea9d-default-rtdb.firebaseio.com/"

/**
 * @brief Database Secret de Firebase
 * @note Secret para autenticación legacy
 */
#define DATABASE_SECRET "h9ppcrM5LEnlcw88bdhItk24hEBb9rpK7Nbi24zs"

/**
 * @brief Host de la base de datos Firebase
 * @note Host sin protocolo https://
 */
#define DATABASE_HOST "proyecto-hidroponico-9ea9d-default-rtdb.firebaseio.com"

/**
 * @brief Email del usuario Firebase (opcional)
 * @note Para autenticación si se requiere
 */
// #define USER_EMAIL "usuario@email.com"

/**
 * @brief Contraseña del usuario Firebase (opcional)
 * @note Para autenticación si se requiere
 */
// #define USER_PASSWORD "password123"

/**
 * @brief Path base en Firebase
 * @note Ruta donde se almacenarán los datos
 */
#define FIREBASE_PATH "/datos"

/**
 * @brief Intervalo de envío de datos (ms)
 * @note Frecuencia de actualización de datos en Firebase
 */
#define DATA_SEND_INTERVAL 2000

// ============================================================================
// CONFIGURACIÓN DE TIMEOUTS
// ============================================================================

/**
 * @brief Timeout para operaciones Firebase (ms)
 */
#define FIREBASE_TIMEOUT 10000

/**
 * @brief Número máximo de reintentos para Firebase
 */
#define FIREBASE_MAX_RETRIES 3

/**
 * @brief Intervalo entre reintentos Firebase (ms)
 */
#define FIREBASE_RETRY_INTERVAL 5000

// ============================================================================
// CONFIGURACIÓN DE DEBUGGING
// ============================================================================

/**
 * @brief Habilitar debug de WiFi
 * @note Comentar para desactivar mensajes de debug WiFi
 */
#define DEBUG_WIFI

/**
 * @brief Habilitar debug de Firebase
 * @note Comentar para desactivar mensajes de debug Firebase
 */
#define DEBUG_FIREBASE

/**
 * @brief Habilitar debug de datos
 * @note Comentar para desactivar mensajes de debug de datos
 */
#define DEBUG_DATA

// ============================================================================
// CONFIGURACIÓN DE SEGURIDAD
// ============================================================================

/**
 * @brief Habilitar verificación SSL
 * @note Recomendado mantener habilitado para seguridad
 */
#define ENABLE_SSL_VERIFICATION true

/**
 * @brief Certificado CA (opcional)
 * @note Para verificación SSL personalizada
 */
// #define FIREBASE_CA_CERT "-----BEGIN CERTIFICATE-----\n...\n-----END CERTIFICATE-----"

#endif // NETWORK_CONFIG_H