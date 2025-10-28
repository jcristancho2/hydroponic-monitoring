"use client"

import { useState } from "react"
import { LandingPage } from "@/components/landing-page"
import { DashboardView } from "@/components/dashboard-view"

export default function Home() {
  const [showDashboard, setShowDashboard] = useState(false)

  if (!showDashboard) {
    return <LandingPage onEnter={() => setShowDashboard(true)} />
  }

  return <DashboardView />
}
