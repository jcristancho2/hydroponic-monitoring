// ========================================
// CONFIGURACIÓN FIREBASE Y WIFI
// ========================================
// Instrucciones para configurar:
//
// 1. CONFIGURACIÓN WIFI:
//    - Reemplaza "TU_NOMBRE_DE_RED" con el nombre de tu red WiFi
//    - Reemplaza "TU_CONTRASEÑA" con la contraseña de tu red
//
// 2. CONFIGURACIÓN FIREBASE:
//    - Ve a https://console.firebase.google.com/
//    - Crea un nuevo proyecto o selecciona uno existente
//    - Ve a "Project Settings" > "General" > "Your apps"
//    - Si no tienes una app, crea una nueva "Web app"
//    - Copia el "API Key" y pegalo en lugar de "TU_API_KEY"
//    - Ve a "Realtime Database" en el menú lateral
//    - Crea una base de datos (puedes empezar en modo test)
//    - Copia la URL de la base de datos y pegala en "TU_DATABASE_URL"
//      (Ejemplo: https://tu-proyecto-default-rtdb.firebaseio.com/)
//
// 3. REGLAS DE FIREBASE (Opcional para testing):
//    En "Realtime Database" > "Rules", puedes usar estas reglas temporales:
//    {
//      "rules": {
//        ".read": true,
//        ".write": true
//      }
//    }
//    ⚠️ NOTA: Estas reglas son solo para testing, para producción usa reglas más seguras
//
// ========================================

#ifndef CONFIG_H
#define CONFIG_H

// Configuración WiFi, la red wifi se recomenda de 2.4 GHz
#define WIFI_SSID "Prueba123"        // Cambiar por tu red WiFi (ej: "MiWiFi")
#define WIFI_PASSWORD "1234567890"       // Cambiar por tu contraseña

//conexion de base de datos con firebase
// a api key se encuentra en la configuracion del proyecto de firebase como clave api web
#define API_KEY "AIzaSyAtJSvqWiw0qRB4wwDLRahKVEzH_qcwYj8"                
#define DATABASE_URL "https://proyecto-hidroponico-9ea9d-default-rtdb.firebaseio.com/"

#endif