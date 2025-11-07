# Frontend - Actualizaciones Sistema Hidropónico

## 🚀 Nuevas Características

### ✅ Detección de Modo de Operación

- **Modo Real**: Muestra datos de sensores físicos conectados
- **Modo Simulación**: Indica cuando el ESP32 está en modo de prueba
- Badge visual que indica el modo activo

### ✅ Monitoreo de Estados de Sensores

- **Sensor pH**: Indica si está calibrado o no
- **Sensor TDS**: Muestra estado de conexión
- **Sensores de Nivel**: Solo visibles en modo real para tanques pH+ y pH-

### ✅ Información Extendida del Sistema

- Dirección MAC del ESP32
- Estado de conexión WiFi mejorado
- Información detallada del hardware

### ✅ Bombas con Descripciones

- **Bomba Principal**: Control de flujo del sistema
- **Bomba pH-**: Dosificación de ácido para reducir pH
- **Bomba pH+**: Dosificación de base para aumentar pH

### ✅ Niveles de Tanques de Dosificación

- Monitoreo específico de tanques pH+ y pH- (solo en modo real)
- Indicadores visuales de estado OK/BAJO
- Sensores SEN0205 dedicados

### ✅ Alertas y Notificaciones

- Detección automática de niveles bajos
- Alertas en consola para debugging
- Base preparada para notificaciones push

## 📊 Estructura de Datos Actualizada

```typescript
interface HydroponicData {
  diagnostico?: {
    estado: string;
    chip: string;
    mac: string; // ⬅️ NUEVO
    senal: number;
    ip: string;
    timestamp: number;
  };
  sensores?: {
    ph4502c?: { ph: number };
    sen0244?: { tds: number };
    sen0205?: { nivel_liquido: number };
    ultrasonico?: { nivel_tranque: number };
    tds_conectado?: boolean; // ⬅️ NUEVO
    ph_calibrado?: boolean; // ⬅️ NUEVO
    nivel_ph_minus?: { estado: boolean }; // ⬅️ NUEVO
    nivel_ph_plus?: { estado: boolean }; // ⬅️ NUEVO
  };
  actuadores?: {
    bomba_agua?: { estado: number };
    bomba_sustrato?: { estado: number };
    bomba_solucion?: { estado: number };
  };
  sistema?: {
    modo?: string; // ⬅️ NUEVO: "real" | "simulacion"
  };
}
```

## 🎨 Nuevos Componentes

### `SystemStatus`

- Estado visual del sistema completo
- Conexión, modo, calibración y sensores
- Timestamp de última actualización

### `PumpCard` (Actualizado)

- Soporte para descripciones opcionales
- Mejor información contextual

## 🔄 Compatibilidad

### ✅ Retrocompatibilidad

- Funciona con datos anteriores
- Manejo graceful de campos faltantes
- Fallbacks para información no disponible

### ✅ Datos en Tiempo Real

- Actualización automática vía Firebase
- Historia extendida (30 puntos vs 20)
- Mejor gestión de estados

## 🛠️ Configuración Firebase

El frontend se conecta automáticamente a:

```
https://proyecto-hidroponico-9ea9d-default-rtdb.firebaseio.com/
```

### Estructura esperada:

```
/hydroponic_data/
├── diagnostico/
├── sensores/
├── actuadores/
└── sistema/        ⬅️ NUEVO
```

## 📱 Responsive Design

- ✅ Móviles (sm): Layout optimizado
- ✅ Tablets (md): Grid reorganizado
- ✅ Desktop (lg): Vista completa
- ✅ Modo oscuro/claro

## 🚀 Próximas Mejoras

- [ ] Notificaciones push para alertas
- [ ] Control manual de bombas desde UI
- [ ] Historial persistente en localStorage
- [ ] Exportar datos a CSV
- [ ] Configuración de umbrales desde UI
- [ ] Comandos de calibración desde dashboard
