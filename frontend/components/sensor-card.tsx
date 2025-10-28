import { Card } from "@/components/ui/card"
import { Badge } from "@/components/ui/badge"
import type { ReactNode } from "react"

interface SensorCardProps {
  title: string
  value: string
  unit: string
  icon: ReactNode
  status: "optimal" | "warning" | "unknown"
  description: string
  optimalRange: string
}

export function SensorCard({ title, value, unit, icon, status, description, optimalRange }: SensorCardProps) {
  const statusColors = {
    optimal: "bg-green-500/10 text-green-500 border-green-500/20",
    warning: "bg-yellow-500/10 text-yellow-500 border-yellow-500/20",
    unknown: "bg-muted text-muted-foreground border-border",
  }

  return (
    <Card className="border-border/50 bg-card/50 p-4 backdrop-blur transition-all hover:border-border sm:p-6">
      <div className="flex items-start justify-between">
        <div className="flex items-center gap-2 sm:gap-3">
          <div className={`rounded-lg p-2 ${statusColors[status]} sm:p-2.5`}>{icon}</div>
          <div>
            <p className="text-xs font-medium text-muted-foreground sm:text-sm">{title}</p>
            <p className="text-[10px] text-muted-foreground/70 sm:text-xs">{description}</p>
          </div>
        </div>
        <Badge variant="outline" className={`${statusColors[status]} border text-[10px] sm:text-xs`}>
          {status === "optimal" ? "Óptimo" : status === "warning" ? "Alerta" : "--"}
        </Badge>
      </div>
      <div className="mt-3 sm:mt-4">
        <div className="flex items-baseline gap-1.5 sm:gap-2">
          <span className="text-3xl font-bold tracking-tight sm:text-4xl">{value}</span>
          <span className="text-base text-muted-foreground sm:text-lg">{unit}</span>
        </div>
        <p className="mt-1.5 text-[10px] text-muted-foreground sm:mt-2 sm:text-xs">Rango óptimo: {optimalRange}</p>
      </div>
    </Card>
  )
}
