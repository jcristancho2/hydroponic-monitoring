"use client"

import { Card } from "@/components/ui/card"
import { Badge } from "@/components/ui/badge"
import { CheckCircle, XCircle, AlertTriangle, Wifi, WifiOff } from "lucide-react"

interface SystemStatusProps {
  isOnline: boolean
  isSimulation: boolean
  phCalibrated: boolean
  tdsConnected: boolean
  lastUpdate: string
}

export function SystemStatus({ 
  isOnline, 
  isSimulation, 
  phCalibrated, 
  tdsConnected,
  lastUpdate 
}: SystemStatusProps) {
  return (
    <Card className="border-border/50 bg-card/50 p-4 backdrop-blur sm:p-6">
      <div className="space-y-4">
        <h3 className="text-lg font-semibold">Estado del Sistema</h3>
        
        <div className="grid gap-3 sm:grid-cols-2 lg:grid-cols-4">
          {/* Connection Status */}
          <div className="flex items-center space-x-2">
            {isOnline ? (
              <Wifi className="h-5 w-5 text-green-500" />
            ) : (
              <WifiOff className="h-5 w-5 text-red-500" />
            )}
            <div>
              <p className="text-sm font-medium">Conexión</p>
              <p className={`text-xs ${isOnline ? 'text-green-600' : 'text-red-600'}`}>
                {isOnline ? 'En línea' : 'Desconectado'}
              </p>
            </div>
          </div>

          {/* Operating Mode */}
          <div className="flex items-center space-x-2">
            {isSimulation ? (
              <AlertTriangle className="h-5 w-5 text-orange-500" />
            ) : (
              <CheckCircle className="h-5 w-5 text-green-500" />
            )}
            <div>
              <p className="text-sm font-medium">Modo</p>
              <p className={`text-xs ${isSimulation ? 'text-orange-600' : 'text-green-600'}`}>
                {isSimulation ? 'Simulación' : 'Sensores reales'}
              </p>
            </div>
          </div>

          {/* pH Sensor Status */}
          <div className="flex items-center space-x-2">
            {phCalibrated ? (
              <CheckCircle className="h-5 w-5 text-green-500" />
            ) : (
              <XCircle className="h-5 w-5 text-red-500" />
            )}
            <div>
              <p className="text-sm font-medium">pH Sensor</p>
              <p className={`text-xs ${phCalibrated ? 'text-green-600' : 'text-red-600'}`}>
                {phCalibrated ? 'Calibrado' : 'No calibrado'}
              </p>
            </div>
          </div>

          {/* TDS Sensor Status */}
          <div className="flex items-center space-x-2">
            {tdsConnected ? (
              <CheckCircle className="h-5 w-5 text-green-500" />
            ) : (
              <XCircle className="h-5 w-5 text-red-500" />
            )}
            <div>
              <p className="text-sm font-medium">TDS Sensor</p>
              <p className={`text-xs ${tdsConnected ? 'text-green-600' : 'text-red-600'}`}>
                {tdsConnected ? 'Conectado' : 'Desconectado'}
              </p>
            </div>
          </div>
        </div>

        {/* Last Update */}
        <div className="pt-2 border-t border-border/50">
          <p className="text-xs text-muted-foreground">
            Última actualización: {lastUpdate}
          </p>
        </div>
      </div>
    </Card>
  )
}