"use client"

import { Card } from "@/components/ui/card"
import { Area, AreaChart, CartesianGrid, ResponsiveContainer, Tooltip, XAxis, YAxis, ReferenceLine } from "recharts"
import { useEffect, useState } from "react"

interface ChartCardProps {
  title: string
  data: Array<{ time: string; value: number }>
  dataKey: string
  color: string
  unit?: string
  optimalMin?: number
  optimalMax?: number
}

export function ChartCard({ title, data, dataKey, color, unit = "", optimalMin, optimalMax }: ChartCardProps) {
  const [isDark, setIsDark] = useState(false)

  useEffect(() => {
    // Detectar si el tema es oscuro
    const checkTheme = () => {
      const isDarkMode = document.documentElement.classList.contains('dark')
      setIsDark(isDarkMode)
    }

    // Verificar al montar
    checkTheme()

    // Observar cambios en el tema
    const observer = new MutationObserver(checkTheme)
    observer.observe(document.documentElement, {
      attributes: true,
      attributeFilter: ['class']
    })

    return () => observer.disconnect()
  }, [])

  // Colores para los ejes según el tema
  const axisColor = isDark ? "#ffffff" : "hsl(var(--foreground))"
  const axisLineColor = isDark ? "rgba(255, 255, 255, 0.2)" : "hsl(var(--border))"

  return (
    <Card className="border-border/50 bg-card/50 p-6 backdrop-blur">
      <h3 className="mb-4 text-lg font-semibold text-center sm:text-center">{title}</h3>
      <ResponsiveContainer width="100%" height={250}>
        <AreaChart data={data}>
          <defs>
            <linearGradient id={`gradient-${dataKey}`} x1="0" y1="0" x2="0" y2="1">
              <stop offset="5%" stopColor={color} stopOpacity={0.3} />
              <stop offset="95%" stopColor={color} stopOpacity={0} />
            </linearGradient>
          </defs>
          <CartesianGrid strokeDasharray="3 3" stroke="hsl(var(--border))" opacity={0.3} />
          <XAxis 
            dataKey="time" 
            stroke={axisColor} 
            fontSize={12} 
            tickLine={false}
            tick={{ fill: axisColor }}
            axisLine={{ stroke: axisLineColor }}
          />
          <YAxis 
            stroke={axisColor} 
            fontSize={12} 
            tickLine={false}
            tick={{ fill: axisColor }}
            axisLine={{ stroke: axisLineColor }}
          />
          <Tooltip
            contentStyle={{
              backgroundColor: "hsl(var(--card))",
              border: "1px solid hsl(var(--border))",
              borderRadius: "8px",
              padding: "8px 12px",
            }}
            labelStyle={{ color: "hsl(var(--foreground))" }}
            formatter={(value: number) => [`${value.toFixed(2)} ${unit}`, title]}
          />
          {optimalMin && (
            <ReferenceLine y={optimalMin} stroke="hsl(var(--chart-3))" strokeDasharray="3 3" opacity={0.5} />
          )}
          {optimalMax && (
            <ReferenceLine y={optimalMax} stroke="hsl(var(--chart-3))" strokeDasharray="3 3" opacity={0.5} />
          )}
          <Area type="monotone" dataKey={dataKey} stroke={color} strokeWidth={2} fill={`url(#gradient-${dataKey})`} />
        </AreaChart>
      </ResponsiveContainer>
    </Card>
  )
}
