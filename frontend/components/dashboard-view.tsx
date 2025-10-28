"use client"

import { useEffect, useState } from "react"
import { Card } from "@/components/ui/card"
import { Badge } from "@/components/ui/badge"
import { SensorCard } from "@/components/sensor-card"
import { PumpCard } from "@/components/pump-card"
import { ChartCard } from "@/components/chart-card"
import { ThemeToggle } from "@/components/theme-toggle"
import { Activity, Droplets, Beaker, Waves } from "lucide-react"

interface HydroponicData {
  diagnostico?: {
    estado: string
    chip: string
    mac: string
    senal: number
    ip: string
    timestamp: number
  }
  sensores?: {
    ph4502c?: { ph: number }
    sen0244?: { tds: number }
    sen0205?: { nivel_liquido: number }
    ultrasonico?: { nivel_tranque: number }
  }
  actuadores?: {
    bomba_agua?: { estado: number }
    bomba_sustrato?: { estado: number }
    bomba_solucion?: { estado: number }
  }
}

export function DashboardView() {
  const [data, setData] = useState<HydroponicData | null>(null)
  const [phHistory, setPhHistory] = useState<Array<{ time: string; value: number }>>([])
  const [tdsHistory, setTdsHistory] = useState<Array<{ time: string; value: number }>>([])

  useEffect(() => {
    const initFirebase = async () => {
      const { initializeApp } = await import("firebase/app")
      const { getDatabase, ref, onValue } = await import("firebase/database")

      const firebaseConfig = {
        databaseURL: "https://proyecto-hidroponico-9ea9d-default-rtdb.firebaseio.com",
      }

      const app = initializeApp(firebaseConfig)
      const db = getDatabase(app)
      const dataRef = ref(db, "/hydroponic_data")

      onValue(dataRef, (snapshot) => {
        const newData = snapshot.val()
        if (newData) {
          setData(newData)

          if (newData.sensores?.ph4502c?.ph) {
            const now = new Date().toLocaleTimeString("es-ES", {
              hour: "2-digit",
              minute: "2-digit",
            })
            setPhHistory((prev) => {
              const updated = [...prev, { time: now, value: newData.sensores.ph4502c.ph }]
              return updated.slice(-20)
            })
          }

          if (newData.sensores?.sen0244?.tds) {
            const now = new Date().toLocaleTimeString("es-ES", {
              hour: "2-digit",
              minute: "2-digit",
            })
            setTdsHistory((prev) => {
              const updated = [...prev, { time: now, value: newData.sensores.sen0244.tds }]
              return updated.slice(-20)
            })
          }
        }
      })
    }

    initFirebase()
  }, [])

  const isOnline = data?.diagnostico?.estado === "Conectado"
  const lastUpdate = data?.diagnostico?.timestamp ? new Date(data.diagnostico.timestamp).toLocaleString("es-ES") : "--"

  return (
    <div className="min-h-screen bg-background p-3 sm:p-4 md:p-6 lg:p-8">
      <div className="mx-auto max-w-7xl space-y-4 sm:space-y-6">
        {/* Header */}
        <header className="flex flex-col gap-3 sm:gap-4 md:flex-row md:items-center md:justify-between">
          <div>
            <h1 className="text-2xl font-bold tracking-tight text-foreground sm:text-3xl">Sistema Hidropónico</h1>
            <p className="text-sm text-muted-foreground sm:text-base">Monitoreo en tiempo real - Cultivo de lechuga</p>
          </div>
          <div className="flex items-center gap-2 sm:gap-3">
            <ThemeToggle />
            <Badge
              variant={isOnline ? "default" : "destructive"}
              className="px-2 py-1 text-xs sm:px-3 sm:py-1.5 sm:text-sm"
            >
              <div
                className={`mr-1.5 h-2 w-2 rounded-full ${isOnline ? "bg-green-500" : "bg-red-500"} animate-pulse sm:mr-2`}
              />
              <span className="hidden sm:inline">{isOnline ? "Sistema Conectado" : "Sistema Desconectado"}</span>
              <span className="sm:hidden">{isOnline ? "Conectado" : "Desconectado"}</span>
            </Badge>
          </div>
        </header>

        {/* System Info Card */}
        <Card className="border-border/50 bg-card/50 p-4 backdrop-blur sm:p-6">
          <div className="grid gap-3 sm:grid-cols-2 sm:gap-4 lg:grid-cols-4">
            <div>
              <p className="text-xs text-muted-foreground sm:text-sm">Chip</p>
              <p className="font-mono text-xs font-medium sm:text-sm">{data?.diagnostico?.chip || "--"}</p>
            </div>
            <div>
              <p className="text-xs text-muted-foreground sm:text-sm">Dirección IP</p>
              <p className="font-mono text-xs font-medium sm:text-sm">{data?.diagnostico?.ip || "--"}</p>
            </div>
            <div>
              <p className="text-xs text-muted-foreground sm:text-sm">Señal WiFi</p>
              <p className="font-mono text-xs font-medium sm:text-sm">{data?.diagnostico?.senal || "--"} dBm</p>
            </div>
            <div>
              <p className="text-xs text-muted-foreground sm:text-sm">Última actualización</p>
              <p className="font-mono text-xs font-medium sm:text-sm">{lastUpdate}</p>
            </div>
          </div>
        </Card>

        {/* Sensors Grid */}
        <div className="grid gap-3 sm:grid-cols-2 sm:gap-4 lg:grid-cols-4 lg:gap-6">
          <SensorCard
            title="pH"
            value={data?.sensores?.ph4502c?.ph?.toFixed(1) || "--"}
            unit="pH"
            icon={<Beaker className="h-4 w-4 sm:h-5 sm:w-5" />}
            status={
              data?.sensores?.ph4502c?.ph
                ? data.sensores.ph4502c.ph >= 5.5 && data.sensores.ph4502c.ph <= 7.5
                  ? "optimal"
                  : "warning"
                : "unknown"
            }
            description="PH4502C"
            optimalRange="5.5 - 7.5"
          />
          <SensorCard
            title="TDS"
            value={data?.sensores?.sen0244?.tds?.toFixed(0) || "--"}
            unit="ppm"
            icon={<Droplets className="h-4 w-4 sm:h-5 sm:w-5" />}
            status={
              data?.sensores?.sen0244?.tds
                ? data.sensores.sen0244.tds >= 400 && data.sensores.sen0244.tds <= 800
                  ? "optimal"
                  : "warning"
                : "unknown"
            }
            description="SEN0244"
            optimalRange="400 - 800 ppm"
          />
          <SensorCard
            title="Nivel Líquido"
            value={data?.sensores?.sen0205?.nivel_liquido?.toString() || "--"}
            unit="%"
            icon={<Waves className="h-4 w-4 sm:h-5 sm:w-5" />}
            status={
              data?.sensores?.sen0205?.nivel_liquido
                ? data.sensores.sen0205.nivel_liquido > 30
                  ? "optimal"
                  : "warning"
                : "unknown"
            }
            description="SEN0205"
            optimalRange="> 30%"
          />
          <SensorCard
            title="Nivel Tranque"
            value={data?.sensores?.ultrasonico?.nivel_tranque?.toString() || "--"}
            unit="cm"
            icon={<Activity className="h-4 w-4 sm:h-5 sm:w-5" />}
            status={
              data?.sensores?.ultrasonico?.nivel_tranque
                ? data.sensores.ultrasonico.nivel_tranque > 20
                  ? "optimal"
                  : "warning"
                : "unknown"
            }
            description="Ultrasónico"
            optimalRange="> 20 cm"
          />
        </div>

        {/* Charts */}
        <div className="grid gap-4 sm:gap-6 lg:grid-cols-2">
          <ChartCard
            title="Historial de pH"
            data={phHistory}
            dataKey="value"
            color="#22c55e"
            optimalMin={5.5}
            optimalMax={7.5}
          />
          <ChartCard
            title="Historial de TDS"
            data={tdsHistory}
            dataKey="value"
            color="#3b82f6"
            unit="ppm"
          />
        </div>

        {/* Pumps */}
        <div>
          <h2 className="mb-3 text-lg font-semibold sm:mb-4 sm:text-xl">Actuadores</h2>
          <div className="grid gap-3 sm:gap-4 md:grid-cols-3 lg:gap-6">
            <PumpCard title="Bomba de Agua" status={data?.actuadores?.bomba_agua?.estado === 1} />
            <PumpCard title="Bomba de Sustrato" status={data?.actuadores?.bomba_sustrato?.estado === 1} />
            <PumpCard title="Bomba de Solución" status={data?.actuadores?.bomba_solucion?.estado === 1} />
          </div>
        </div>
      </div>
    </div>
  )
}
