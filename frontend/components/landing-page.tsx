"use client"

import { Button } from "@/components/ui/button"
import { Leaf, ChevronRight } from "lucide-react"
import { useEffect, useState } from "react"

interface LandingPageProps {
  onEnter: () => void
}

export function LandingPage({ onEnter }: LandingPageProps) {
  const [theme, setTheme] = useState<"light" | "dark">("dark")

  useEffect(() => {
    const savedTheme = localStorage.getItem("theme") as "light" | "dark" | null
    const prefersDark = window.matchMedia("(prefers-color-scheme: dark)").matches

    const initialTheme = savedTheme || (prefersDark ? "dark" : "light")
    setTheme(initialTheme)
    document.documentElement.classList.toggle("dark", initialTheme === "dark")
  }, [])

  const toggleTheme = () => {
    const newTheme = theme === "light" ? "dark" : "light"
    setTheme(newTheme)
    localStorage.setItem("theme", newTheme)
    document.documentElement.classList.toggle("dark", newTheme === "dark")
  }
  return (
    <div className="relative flex min-h-screen items-center justify-center overflow-hidden bg-gradient-to-br from-green-50 via-emerald-50 to-teal-50 dark:from-green-950 dark:via-emerald-950 dark:to-teal-950">
      {/* Animated background elements */}
      <div className="absolute inset-0 overflow-hidden">
        <div className="absolute -left-4 top-1/4 h-72 w-72 animate-blob rounded-full bg-green-300/30 mix-blend-multiply blur-xl filter dark:bg-green-700/20" />
        <div className="animation-delay-2000 absolute right-4 top-1/3 h-72 w-72 animate-blob rounded-full bg-emerald-300/30 mix-blend-multiply blur-xl filter dark:bg-emerald-700/20" />
        <div className="animation-delay-4000 absolute bottom-8 left-1/3 h-72 w-72 animate-blob rounded-full bg-teal-300/30 mix-blend-multiply blur-xl filter dark:bg-teal-700/20" />
      </div>

      {/* Content */}
      <div className="relative z-10 mx-auto max-w-4xl px-4 text-center sm:px-6">
        {/* Logo */}
        <div className="mb-6 flex justify-center sm:mb-8">
          <button
            onClick={toggleTheme}
            className="group cursor-pointer rounded-2xl bg-gradient-to-br from-green-500 to-emerald-600 p-4 shadow-2xl transition-all hover:scale-105 hover:shadow-3xl hover:from-green-600 hover:to-emerald-700 sm:rounded-3xl sm:p-6"
            aria-label="Cambiar tema"
          >
            <Leaf className="h-12 w-12 text-white transition-transform group-hover:rotate-12 sm:h-16 sm:w-16" strokeWidth={2} />
          </button>
        </div>

        {/* Title */}
        <h1 className="mb-3 text-balance text-3xl font-bold tracking-tight text-gray-900 dark:text-white sm:mb-4 sm:text-4xl md:text-5xl lg:text-6xl">
          Sistema Hidropónico
        </h1>
        <p className="mb-6 text-pretty text-base text-gray-600 dark:text-gray-300 sm:mb-8 sm:text-lg md:text-xl lg:text-2xl">
          Monitoreo inteligente para tu cultivo de lechuga
        </p>

        {/* Features */}
        <div className="mb-8 grid gap-3 text-left sm:mb-12 sm:grid-cols-3 sm:gap-4">
          <div className="group relative rounded-xl bg-gradient-to-br from-white via-white to-gray-50/80 p-4 backdrop-blur-sm shadow-[0_8px_30px_rgb(0,0,0,0.12),0_4px_12px_rgb(0,0,0,0.08)] transition-all duration-300 hover:shadow-[0_20px_50px_rgb(0,0,0,0.15),0_8px_25px_rgb(0,0,0,0.1)] hover:-translate-y-2 hover:scale-105 dark:from-gray-800 dark:via-gray-800 dark:to-gray-900/80 dark:shadow-[0_8px_30px_rgb(0,0,0,0.3),0_4px_12px_rgb(0,0,0,0.2)] dark:hover:shadow-[0_20px_50px_rgb(0,0,0,0.4),0_8px_25px_rgb(0,0,0,0.3)] sm:rounded-2xl sm:p-6">
            <div className="absolute inset-0 rounded-xl bg-gradient-to-br from-transparent via-white/10 to-transparent opacity-0 transition-opacity duration-300 group-hover:opacity-100 sm:rounded-2xl"></div>
            <div className="relative z-10">
              <div className="mb-2 text-2xl sm:text-3xl text-center transform transition-transform duration-300 group-hover:scale-110 group-hover:rotate-3">📊</div>
              <h3 className="mb-1 text-sm font-semibold text-gray-900 dark:text-white sm:text-base text-center">Tiempo Real</h3>
              <p className="text-xs text-gray-600 dark:text-gray-400 sm:text-sm">Monitoreo continuo de sensores</p>
            </div>
          </div>
          <div className="group relative rounded-xl bg-gradient-to-br from-white via-white to-gray-50/80 p-4 backdrop-blur-sm shadow-[0_8px_30px_rgb(0,0,0,0.12),0_4px_12px_rgb(0,0,0,0.08)] transition-all duration-300 hover:shadow-[0_20px_50px_rgb(0,0,0,0.15),0_8px_25px_rgb(0,0,0,0.1)] hover:-translate-y-2 hover:scale-105 dark:from-gray-800 dark:via-gray-800 dark:to-gray-900/80 dark:shadow-[0_8px_30px_rgb(0,0,0,0.3),0_4px_12px_rgb(0,0,0,0.2)] dark:hover:shadow-[0_20px_50px_rgb(0,0,0,0.4),0_8px_25px_rgb(0,0,0,0.3)] sm:rounded-2xl sm:p-6">
            <div className="absolute inset-0 rounded-xl bg-gradient-to-br from-transparent via-white/10 to-transparent opacity-0 transition-opacity duration-300 group-hover:opacity-100 sm:rounded-2xl"></div>
            <div className="relative z-10">
              <div className="mb-2 text-2xl sm:text-3xl text-center transform transition-transform duration-300 group-hover:scale-110 group-hover:rotate-3">🌱</div>
              <h3 className="mb-1 text-sm font-semibold text-gray-900 dark:text-white sm:text-base text-center">Control Total</h3>
              <p className="text-xs text-gray-600 dark:text-gray-400 sm:text-sm">Gestión de bombas y actuadores</p>
            </div>
          </div>
          <div className="group relative rounded-xl bg-gradient-to-br from-white via-white to-gray-50/80 p-4 backdrop-blur-sm shadow-[0_8px_30px_rgb(0,0,0,0.12),0_4px_12px_rgb(0,0,0,0.08)] transition-all duration-300 hover:shadow-[0_20px_50px_rgb(0,0,0,0.15),0_8px_25px_rgb(0,0,0,0.1)] hover:-translate-y-2 hover:scale-105 dark:from-gray-800 dark:via-gray-800 dark:to-gray-900/80 dark:shadow-[0_8px_30px_rgb(0,0,0,0.3),0_4px_12px_rgb(0,0,0,0.2)] dark:hover:shadow-[0_20px_50px_rgb(0,0,0,0.4),0_8px_25px_rgb(0,0,0,0.3)] sm:rounded-2xl sm:p-6">
            <div className="absolute inset-0 rounded-xl bg-gradient-to-br from-transparent via-white/10 to-transparent opacity-0 transition-opacity duration-300 group-hover:opacity-100 sm:rounded-2xl"></div>
            <div className="relative z-10">
              <div className="mb-2 text-2xl sm:text-3xl text-center transform transition-transform duration-300 group-hover:scale-110 group-hover:rotate-3">📈</div>
              <h3 className="mb-1 text-sm font-semibold text-gray-900 dark:text-white sm:text-base text-center">Análisis</h3>
              <p className="text-xs text-gray-600 dark:text-gray-400 sm:text-sm">Gráficos y tendencias</p>
            </div>
          </div>
        </div>

        {/* CTA Button */}
        <Button
          onClick={onEnter}
          size="lg"
          className="group relative overflow-hidden bg-gradient-to-r from-green-600 to-emerald-600 px-6 py-5 text-base font-semibold text-white shadow-2xl transition-all hover:scale-105 hover:from-green-700 hover:to-emerald-700 sm:px-8 sm:py-6 sm:text-lg"
        >
          <span className="relative z-10 flex items-center gap-2">
            Acceder
            <ChevronRight className="h-4 w-4 transition-transform group-hover:translate-x-1 sm:h-5 sm:w-5" />
          </span>
          <div className="absolute inset-0 -z-10 bg-gradient-to-r from-green-700 to-emerald-700 opacity-0 transition-opacity group-hover:opacity-100" />
        </Button>
      </div>
    </div>
  )
}
