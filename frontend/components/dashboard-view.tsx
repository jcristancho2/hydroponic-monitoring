"use client";

import { useEffect, useState } from "react";
import { Card } from "@/components/ui/card";
import { Badge } from "@/components/ui/badge";
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
} from "lucide-react";

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
                  // Aquí se podría agregar una notificación push o toast
                  console.warn(alertMsg);
                }
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
            />
            <PumpCard
              title="Bomba pH- (Ácido)"
              status={data?.actuadores?.bomba_sustrato?.estado === 1}
              description="Dosificación para reducir pH"
            />
            <PumpCard
              title="Bomba pH+ (Base)"
              status={data?.actuadores?.bomba_solucion?.estado === 1}
              description="Dosificación para aumentar pH"
            />
          </div>
        </div>
      </div>
    </div>
  );
}
