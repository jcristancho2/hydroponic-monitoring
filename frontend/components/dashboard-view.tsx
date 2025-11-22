"use client";

import { useEffect, useState } from "react";
import { Card } from "@/components/ui/card";
import { Badge } from "@/components/ui/badge";
import { Button } from "@/components/ui/button";
import { SensorCard } from "@/components/sensor-card";
import { PumpCard } from "@/components/pump-card";
import { ChartCard } from "@/components/chart-card";
import { SystemStatus } from "@/components/system-status";
import { ThemeToggle } from "@/components/theme-toggle";
import {
  Droplets,
  Beaker,
  CheckCircle,
  XCircle,
  Sun,
  Download,
} from "lucide-react";
import { getDatabase, ref, set } from "firebase/database";

interface HydroponicData {
  diagnostico?: {
    estado: string;
    chip: string;
    mac: string;
    senal: number;
    ip: string;
    timestamp: number;
  };
  sensores?: {
    ph4502c?: { ph: number };
    sen0244?: { tds: number };
    sen0205?: { nivel_liquido: number };
    ultrasonico?: { nivel_tranque: number };
    ldr?: {
      valor_bruto: number;
      nivel_luz: string;
      exposicion_solar_hoy_segundos?: number;
      tiempo_restante_segundos?: number;
      exposicion_activa?: boolean;
    };
    tds_conectado?: boolean;
    ph_calibrado?: boolean;
    nivel_ph_minus?: { estado: boolean };
    nivel_ph_plus?: { estado: boolean };
  };
  actuadores?: {
    bomba_agua?: { estado: number };
    bomba_sustrato?: { estado: number };
    bomba_solucion?: { estado: number };
  };
  sistema?: {
    modo?: string;
  };
}

export function DashboardView() {
  const [data, setData] = useState<HydroponicData | null>(null);
  const [phHistory, setPhHistory] = useState<
    Array<{ time: string; value: number }>
  >([]);
  const [tdsHistory, setTdsHistory] = useState<
    Array<{ time: string; value: number }>
  >([]);
  const [isSimulationMode, setIsSimulationMode] = useState(false);
  const [isFirebaseConnected, setIsFirebaseConnected] = useState(false);
  const [lastAlert, setLastAlert] = useState<string | null>(null);
  const [ldrAlert, setLdrAlert] = useState<string | null>(null);
  const [isDownloading, setIsDownloading] = useState(false);

  useEffect(() => {
    const initFirebase = async () => {
      const { initializeApp } = await import("firebase/app");
      const { getDatabase, ref, onValue } = await import("firebase/database");

      const firebaseConfig = {
        databaseURL:
          "https://proyecto-hidroponico-9ea9d-default-rtdb.firebaseio.com",
      };

      const app = initializeApp(firebaseConfig);
      const db = getDatabase(app);
      const dataRef = ref(db, "/hydroponic_data");

      onValue(
        dataRef,
        (snapshot) => {
          const newData = snapshot.val();
          if (newData) {
            setData(newData);
            setIsFirebaseConnected(true);

            // Detectar modo de simulación
            setIsSimulationMode(newData.sistema?.modo === "simulacion");

            // Verificar alertas críticas
            if (!isSimulationMode) {
              if (
                !newData.sensores?.nivel_ph_minus?.estado ||
                !newData.sensores?.nivel_ph_plus?.estado
              ) {
                const alertMsg = "⚠️ Nivel bajo en tanques de dosificación";
                if (lastAlert !== alertMsg) {
                  setLastAlert(alertMsg);
                  console.warn(alertMsg);
                }
              }
            }

            // Verificar alertas de luz (LDR)
            if (newData.sensores?.ldr?.nivel_luz) {
              const nivelLuz = newData.sensores.ldr.nivel_luz.toLowerCase();
              if (nivelLuz === "muy bajo" || nivelLuz === "bajo") {
                const alertMsg =
                  "💡 Alerta: Luz muy baja detectada - Verifica la iluminación";
                if (ldrAlert !== alertMsg) {
                  setLdrAlert(alertMsg);
                  console.warn(alertMsg);
                }
              } else {
                setLdrAlert(null);
              }
            }

            // Solo agregar al historial si el sensor está realmente conectado y calibrado
            if (
              newData.sensores?.ph4502c?.ph &&
              newData.sensores?.ph_calibrado
            ) {
              const now = new Date().toLocaleTimeString("es-ES", {
                hour: "2-digit",
                minute: "2-digit",
              });
              setPhHistory((prev) => {
                const updated = [
                  ...prev,
                  { time: now, value: newData.sensores.ph4502c.ph },
                ];
                return updated.slice(-30); // Aumentado para más historia
              });
            }

            // Solo agregar al historial si el sensor TDS está conectado
            if (
              newData.sensores?.sen0244?.tds &&
              newData.sensores?.tds_conectado
            ) {
              const now = new Date().toLocaleTimeString("es-ES", {
                hour: "2-digit",
                minute: "2-digit",
              });
              setTdsHistory((prev) => {
                const updated = [
                  ...prev,
                  { time: now, value: newData.sensores.sen0244.tds },
                ];
                return updated.slice(-30); // Aumentado para más historia
              });
            }
          } else {
            setIsFirebaseConnected(false);
          }
        },
        (error) => {
          console.error("Error conectando a Firebase:", error);
          setIsFirebaseConnected(false);
        }
      );
    };

    initFirebase();
  }, []);

  const setBombaCommand = (
    bomba: "bomba_agua" | "bomba_sustrato" | "bomba_solucion",
    estado: 0 | 1
  ) => {
    const db = getDatabase();
    set(ref(db, `/hydroponic_data/comandos/${bomba}`), estado);
  };

  // Función para descargar datos en formato Excel/CSV
  const handleDownloadData = async () => {
    try {
      setIsDownloading(true);
      const { initializeApp } = await import("firebase/app");
      const { getDatabase, ref, get } = await import("firebase/database");

      const firebaseConfig = {
        databaseURL:
          "https://proyecto-hidroponico-9ea9d-default-rtdb.firebaseio.com",
      };

      const app = initializeApp(firebaseConfig);
      const db = getDatabase(app);

      // Obtener datos del historial
      const phRef = ref(db, "/hydroponic_data/historial/ph");
      const tdsRef = ref(db, "/hydroponic_data/historial/tds");
      const ldrRef = ref(db, "/hydroponic_data/historial/ldr");

      const [phSnapshot, tdsSnapshot, ldrSnapshot] = await Promise.all([
        get(phRef),
        get(tdsRef),
        get(ldrRef),
      ]);

      const phData = phSnapshot.val() || {};
      const tdsData = tdsSnapshot.val() || {};
      const ldrData = ldrSnapshot.val() || {};

      // Combinar datos por timestamp
      const allTimestamps = new Set([
        ...Object.keys(phData),
        ...Object.keys(tdsData),
        ...Object.keys(ldrData),
      ]);

      const sortedTimestamps = Array.from(allTimestamps)
        .map(Number)
        .sort((a, b) => a - b);

      // Crear contenido CSV
      let csvContent = "Timestamp,Fecha y Hora,pH,TDS,LDR\n";

      sortedTimestamps.forEach((timestamp) => {
        const date = new Date(timestamp);
        const formattedDate = date.toLocaleString("es-ES");
        const ph = phData[timestamp] || "";
        const tds = tdsData[timestamp] || "";
        const ldr = ldrData[timestamp] || "";

        csvContent += `${timestamp},"${formattedDate}",${ph},${tds},${ldr}\n`;
      });

      // Crear blob y descargar
      const blob = new Blob([csvContent], { type: "text/csv;charset=utf-8;" });
      const link = document.createElement("a");
      const url = URL.createObjectURL(blob);

      const filename = `datos_hidroponico_${
        new Date().toISOString().split("T")[0]
      }.csv`;
      link.setAttribute("href", url);
      link.setAttribute("download", filename);
      link.style.visibility = "hidden";

      document.body.appendChild(link);
      link.click();
      document.body.removeChild(link);

      console.log("Datos descargados exitosamente");
    } catch (error) {
      console.error("Error al descargar datos:", error);
    } finally {
      setIsDownloading(false);
    }
  };

  // Función para convertir segundos a formato HH:MM
  const secondsToHHMM = (seconds: number) => {
    const hours = Math.floor(seconds / 3600);
    const minutes = Math.floor((seconds % 3600) / 60);
    return `${hours}h ${minutes}m`;
  };

  const isOnline = data?.diagnostico?.estado === "Conectado";
  const lastUpdate = data?.diagnostico?.timestamp
    ? new Date(data.diagnostico.timestamp).toLocaleString("es-ES")
    : "--";

  return (
    <div className="min-h-screen bg-background p-3 sm:p-4 md:p-6 lg:p-8">
      <div className="mx-auto max-w-7xl space-y-4 sm:space-y-6">
        {/* Header */}
        <header className="flex flex-col gap-3 sm:gap-4 md:flex-row md:items-center md:justify-between">
          <div>
            <h1 className="text-2xl font-bold tracking-tight text-foreground sm:text-3xl">
              Sistema Hidropónico
            </h1>
            <p className="text-sm text-muted-foreground sm:text-base">
              Monitoreo en tiempo real - Cultivo de lechuga
              <span
                className={`ml-2 text-xs px-2 py-1 rounded ${
                  isFirebaseConnected
                    ? "bg-green-100 dark:bg-green-900 text-green-800 dark:text-green-200"
                    : "bg-red-100 dark:bg-red-900 text-red-800 dark:text-red-200"
                }`}
              >
                {isFirebaseConnected
                  ? "FIREBASE CONECTADO"
                  : "FIREBASE DESCONECTADO"}
              </span>
            </p>
          </div>
          <div className="flex items-center gap-2 sm:gap-3">
            <ThemeToggle />
            <Button
              onClick={handleDownloadData}
              disabled={isDownloading}
              variant="outline"
              size="sm"
              className="gap-2"
            >
              <Download className="h-4 w-4" />
              <span className="hidden sm:inline">
                {isDownloading ? "Descargando..." : "Descargar"}
              </span>
              <span className="sm:hidden">{isDownloading ? "..." : "D"}</span>
            </Button>
            <Badge
              variant={isOnline ? "default" : "destructive"}
              className="px-2 py-1 text-xs sm:px-3 sm:py-1.5 sm:text-sm"
            >
              <div
                className={`mr-1.5 h-2 w-2 rounded-full ${
                  isOnline ? "bg-green-500" : "bg-red-500"
                } animate-pulse sm:mr-2`}
              />
              <span className="hidden sm:inline">
                {isOnline ? "Sistema Conectado" : "Sistema Desconectado"}
              </span>
              <span className="sm:hidden">
                {isOnline ? "Conectado" : "Desconectado"}
              </span>
            </Badge>
          </div>
        </header>

        {/* System Status */}
        <SystemStatus
          isOnline={isOnline}
          isSimulation={isSimulationMode}
          phCalibrated={data?.sensores?.ph_calibrado || false}
          tdsConnected={data?.sensores?.tds_conectado || false}
          lastUpdate={lastUpdate}
        />

        {/* Alerts Section */}
        {(lastAlert || ldrAlert) && (
          <div className="space-y-3">
            {lastAlert && (
              <Card className="border-orange-200 bg-orange-50 dark:bg-orange-950 dark:border-orange-800 p-4">
                <p className="text-sm font-medium text-orange-800 dark:text-orange-200">
                  {lastAlert}
                </p>
              </Card>
            )}
            {ldrAlert && (
              <Card className="border-yellow-200 bg-yellow-50 dark:bg-yellow-950 dark:border-yellow-800 p-4">
                <p className="text-sm font-medium text-yellow-800 dark:text-yellow-200">
                  {ldrAlert}
                </p>
              </Card>
            )}
          </div>
        )}

        {/* Luz Solar - Recomendación y Temporizador */}
        {data?.sensores?.ldr?.nivel_luz && (
          <Card className="border-blue-200 bg-blue-50 dark:bg-blue-950 dark:border-blue-800 p-4">
            <div className="flex items-start gap-3">
              <Sun className="h-5 w-5 text-blue-600 dark:text-blue-400 mt-0.5 shrink-0" />
              <div className="flex-1">
                <h3 className="font-semibold text-blue-900 dark:text-blue-100 text-sm">
                  ☀️ Recomendación de Exposición Solar
                </h3>
                <p className="text-xs text-blue-800 dark:text-blue-200 mt-1">
                  Se recomienda una exposición solar{" "}
                  <span className="font-bold">máxima de 6 horas diarias</span>{" "}
                  para el cultivo de lechuga. Nivel de luz detectado
                  actualmente:{" "}
                  <span className="font-semibold">
                    {data.sensores.ldr.nivel_luz}
                  </span>
                </p>

                {/* Solar Exposure Timer */}
                <div className="mt-3 grid grid-cols-2 gap-3">
                  <div className="bg-white dark:bg-blue-900 rounded p-2">
                    <p className="text-xs text-blue-700 dark:text-blue-300 font-medium">
                      Exposición Hoy
                    </p>
                    <p className="text-sm font-bold text-blue-900 dark:text-blue-100">
                      {data.sensores.ldr.exposicion_solar_hoy_segundos
                        ? secondsToHHMM(
                            data.sensores.ldr.exposicion_solar_hoy_segundos
                          )
                        : "0h 0m"}
                    </p>
                  </div>
                  <div
                    className={`rounded p-2 ${
                      data.sensores.ldr.tiempo_restante_segundos &&
                      data.sensores.ldr.tiempo_restante_segundos > 0
                        ? "bg-white dark:bg-blue-900"
                        : "bg-red-100 dark:bg-red-900"
                    }`}
                  >
                    <p
                      className={`text-xs font-medium ${
                        data.sensores.ldr.tiempo_restante_segundos &&
                        data.sensores.ldr.tiempo_restante_segundos > 0
                          ? "text-blue-700 dark:text-blue-300"
                          : "text-red-700 dark:text-red-300"
                      }`}
                    >
                      Tiempo Restante
                    </p>
                    <p
                      className={`text-sm font-bold ${
                        data.sensores.ldr.tiempo_restante_segundos &&
                        data.sensores.ldr.tiempo_restante_segundos > 0
                          ? "text-blue-900 dark:text-blue-100"
                          : "text-red-900 dark:text-red-100"
                      }`}
                    >
                      {data.sensores.ldr.tiempo_restante_segundos &&
                      data.sensores.ldr.tiempo_restante_segundos > 0
                        ? secondsToHHMM(
                            data.sensores.ldr.tiempo_restante_segundos
                          )
                        : "⚠️ Límite"}
                    </p>
                  </div>
                </div>

                {/* Solar Exposure Status */}
                <p className="text-xs text-blue-800 dark:text-blue-200 mt-2">
                  Estado:{" "}
                  {data.sensores.ldr.exposicion_activa
                    ? "🟢 Exposición activa"
                    : "⚫ Sin exposición"}
                </p>
              </div>
            </div>
          </Card>
        )}

        {/* System Info Card */}
        <Card className="border-border/50 bg-card/50 p-4 backdrop-blur sm:p-6">
          <h3 className="text-lg font-semibold mb-3">
            Información del Hardware
          </h3>
          <div className="grid gap-3 sm:grid-cols-2 sm:gap-4 lg:grid-cols-4">
            <div>
              <p className="text-xs text-muted-foreground sm:text-sm">Chip</p>
              <p className="font-mono text-xs font-medium sm:text-sm">
                {data?.diagnostico?.chip || "--"}
              </p>
            </div>
            <div>
              <p className="text-xs text-muted-foreground sm:text-sm">
                Dirección IP
              </p>
              <p className="font-mono text-xs font-medium sm:text-sm">
                {data?.diagnostico?.ip || "--"}
              </p>
            </div>
            <div>
              <p className="text-xs text-muted-foreground sm:text-sm">
                MAC Address
              </p>
              <p className="font-mono text-xs font-medium sm:text-sm">
                {data?.diagnostico?.mac || "--"}
              </p>
            </div>
            <div>
              <p className="text-xs text-muted-foreground sm:text-sm">
                Señal WiFi
              </p>
              <p className="font-mono text-xs font-medium sm:text-sm">
                {data?.diagnostico?.senal || "--"} dBm
              </p>
            </div>
          </div>
        </Card>

        {/* Sensors Grid */}
        <div className="grid gap-3 sm:grid-cols-2 sm:gap-4 lg:gap-6">
          <SensorCard
            title="pH"
            value={
              data?.sensores?.ph_calibrado && data?.sensores?.ph4502c?.ph
                ? data.sensores.ph4502c.ph.toFixed(1)
                : "--"
            }
            unit="pH"
            icon={<Beaker className="h-4 w-4 sm:h-5 sm:w-5" />}
            status={
              data?.sensores?.ph_calibrado && data?.sensores?.ph4502c?.ph
                ? data.sensores.ph4502c.ph >= 5.5 &&
                  data.sensores.ph4502c.ph <= 7.5
                  ? "optimal"
                  : "warning"
                : "unknown"
            }
            description={`PH4502C ${
              data?.sensores?.ph_calibrado
                ? "(Calibrado)"
                : "(No calibrado - Sin datos)"
            }`}
            optimalRange="5.5 - 7.5"
          />
          <SensorCard
            title="TDS/Conductividad"
            value={
              data?.sensores?.tds_conectado && data?.sensores?.sen0244?.tds
                ? data.sensores.sen0244.tds.toFixed(0)
                : "--"
            }
            unit="ppm"
            icon={<Droplets className="h-4 w-4 sm:h-5 sm:w-5" />}
            status={
              data?.sensores?.tds_conectado && data?.sensores?.sen0244?.tds
                ? data.sensores.sen0244.tds >= 400 &&
                  data.sensores.sen0244.tds <= 1200
                  ? "optimal"
                  : "warning"
                : "unknown"
            }
            description={`SEN0244 ${
              data?.sensores?.tds_conectado
                ? "(Conectado)"
                : "(Desconectado - Sin datos)"
            }`}
            optimalRange="400 - 1200 ppm"
          />
          <SensorCard
            title="Luz Ambiente"
            value={data?.sensores?.ldr?.valor_bruto?.toString() || "--"}
            unit="Valor"
            icon={<Sun className="h-4 w-4 sm:h-5 sm:w-5" />}
            status={
              data?.sensores?.ldr?.nivel_luz
                ? data.sensores.ldr.nivel_luz.toLowerCase() === "alto" ||
                  data.sensores.ldr.nivel_luz.toLowerCase() === "muy alto"
                  ? "optimal"
                  : data.sensores.ldr.nivel_luz.toLowerCase() === "medio"
                  ? "warning"
                  : "warning"
                : "unknown"
            }
            description={`LDR - ${
              data?.sensores?.ldr?.nivel_luz || "No detectado"
            }`}
            optimalRange="Alto"
          />
        </div>

        {/* Dosing Tank Levels - Only shown in real mode */}
        {!isSimulationMode && (
          <div>
            <h2 className="mb-3 text-lg font-semibold sm:mb-4 sm:text-xl">
              Niveles de Tanques de Dosificación
            </h2>
            <div className="grid gap-3 sm:grid-cols-2 sm:gap-4 lg:gap-6">
              <Card className="p-4 sm:p-6">
                <div className="flex items-center justify-between">
                  <div className="flex items-center space-x-3">
                    <div className="flex h-10 w-10 items-center justify-center rounded-lg bg-red-100 dark:bg-red-900">
                      <Droplets className="h-5 w-5 text-red-600 dark:text-red-400" />
                    </div>
                    <div>
                      <h3 className="font-semibold">Tanque pH- (Ácido)</h3>
                      <p className="text-sm text-muted-foreground">SEN0205</p>
                    </div>
                  </div>
                  <div className="flex items-center space-x-2">
                    {data?.sensores?.nivel_ph_minus?.estado ? (
                      <CheckCircle className="h-5 w-5 text-green-500" />
                    ) : (
                      <XCircle className="h-5 w-5 text-red-500" />
                    )}
                    <span
                      className={`text-sm font-medium ${
                        data?.sensores?.nivel_ph_minus?.estado
                          ? "text-green-600"
                          : "text-red-600"
                      }`}
                    >
                      {data?.sensores?.nivel_ph_minus?.estado ? "OK" : "BAJO"}
                    </span>
                  </div>
                </div>
              </Card>

              <Card className="p-4 sm:p-6">
                <div className="flex items-center justify-between">
                  <div className="flex items-center space-x-3">
                    <div className="flex h-10 w-10 items-center justify-center rounded-lg bg-blue-100 dark:bg-blue-900">
                      <Droplets className="h-5 w-5 text-blue-600 dark:text-blue-400" />
                    </div>
                    <div>
                      <h3 className="font-semibold">Tanque pH+ (Base)</h3>
                      <p className="text-sm text-muted-foreground">SEN0205</p>
                    </div>
                  </div>
                  <div className="flex items-center space-x-2">
                    {data?.sensores?.nivel_ph_plus?.estado ? (
                      <CheckCircle className="h-5 w-5 text-green-500" />
                    ) : (
                      <XCircle className="h-5 w-5 text-red-500" />
                    )}
                    <span
                      className={`text-sm font-medium ${
                        data?.sensores?.nivel_ph_plus?.estado
                          ? "text-green-600"
                          : "text-red-600"
                      }`}
                    >
                      {data?.sensores?.nivel_ph_plus?.estado ? "OK" : "BAJO"}
                    </span>
                  </div>
                </div>
              </Card>
            </div>
          </div>
        )}

        {/* Charts - Only show if sensors are connected */}
        <div className="grid gap-4 sm:gap-6 lg:grid-cols-2">
          <div>
            {data?.sensores?.ph_calibrado && phHistory.length > 0 ? (
              <ChartCard
                title="Historial de pH"
                data={phHistory}
                dataKey="value"
                color="#22c55e"
                optimalMin={5.5}
                optimalMax={7.5}
              />
            ) : (
              <Card className="border-border/50 bg-card/50 p-6 backdrop-blur">
                <h3 className="mb-4 text-lg font-semibold">Historial de pH</h3>
                <div className="flex h-[250px] items-center justify-center text-muted-foreground">
                  <div className="text-center">
                    <Beaker className="mx-auto h-12 w-12 mb-4 opacity-50" />
                    <p className="text-sm">Sensor pH no calibrado</p>
                    <p className="text-xs">
                      Conecta y calibra el sensor para ver datos
                    </p>
                  </div>
                </div>
              </Card>
            )}
          </div>
          <div>
            {data?.sensores?.tds_conectado && tdsHistory.length > 0 ? (
              <ChartCard
                title="Historial de TDS"
                data={tdsHistory}
                dataKey="value"
                color="#3b82f6"
                unit="ppm"
              />
            ) : (
              <Card className="border-border/50 bg-card/50 p-6 backdrop-blur">
                <h3 className="mb-4 text-lg font-semibold">Historial de TDS</h3>
                <div className="flex h-[250px] items-center justify-center text-muted-foreground">
                  <div className="text-center">
                    <Droplets className="mx-auto h-12 w-12 mb-4 opacity-50" />
                    <p className="text-sm">Sensor TDS no conectado</p>
                    <p className="text-xs">Conecta el sensor para ver datos</p>
                  </div>
                </div>
              </Card>
            )}
          </div>
        </div>

        {/* Pumps */}
        <div>
          <h2 className="mb-3 text-lg font-semibold sm:mb-4 sm:text-xl">
            Actuadores y Bombas
          </h2>
          <div className="grid gap-3 sm:gap-4 md:grid-cols-3 lg:gap-6">
            <PumpCard
              title="Bomba Principal/Circulación"
              status={data?.actuadores?.bomba_agua?.estado === 1}
              description="Control de flujo principal del sistema"
              bombaKey="bomba_agua"
            />
            <PumpCard
              title="Bomba pH- (Ácido)"
              status={data?.actuadores?.bomba_sustrato?.estado === 1}
              description="Dosificación para reducir pH"
              bombaKey="bomba_sustrato"
            />
            <PumpCard
              title="Bomba pH+ (Base)"
              status={data?.actuadores?.bomba_solucion?.estado === 1}
              description="Dosificación para aumentar pH"
              bombaKey="bomba_solucion"
            />
          </div>
        </div>

        {/* Manual Control - Only show in real mode */}
        {!isSimulationMode && (
          <div>
            <h2 className="mb-3 text-lg font-semibold sm:mb-4 sm:text-xl">
              Control Manual de Bombas
            </h2>
            <div className="grid gap-3 sm:gap-4 md:grid-cols-3 lg:gap-6 mt-4">
              <div>
                <button
                  className="px-4 py-2 bg-green-600 text-white rounded mr-2"
                  onClick={() => setBombaCommand("bomba_agua", 1)}
                >
                  Encender Bomba Agua
                </button>
                <button
                  className="px-4 py-2 bg-red-600 text-white rounded"
                  onClick={() => setBombaCommand("bomba_agua", 0)}
                >
                  Apagar Bomba Agua
                </button>
              </div>
              <div>
                <button
                  className="px-4 py-2 bg-green-600 text-white rounded mr-2"
                  onClick={() => setBombaCommand("bomba_sustrato", 1)}
                >
                  Encender Bomba pH-
                </button>
                <button
                  className="px-4 py-2 bg-red-600 text-white rounded"
                  onClick={() => setBombaCommand("bomba_sustrato", 0)}
                >
                  Apagar Bomba pH-
                </button>
              </div>
              <div>
                <button
                  className="px-4 py-2 bg-green-600 text-white rounded mr-2"
                  onClick={() => setBombaCommand("bomba_solucion", 1)}
                >
                  Encender Bomba pH+
                </button>
                <button
                  className="px-4 py-2 bg-red-600 text-white rounded"
                  onClick={() => setBombaCommand("bomba_solucion", 0)}
                >
                  Apagar Bomba pH+
                </button>
              </div>
            </div>
          </div>
        )}
      </div>
    </div>
  );
}
