'use client'

import * as React from 'react'
import {
  ThemeProvider as NextThemesProvider,
  type ThemeProviderProps,
} from 'next-themes'

import { ReactNode } from 'react';

export function ThemeProvider({
  children,
  ...props
}: ThemeProviderProps & { children: ReactNode }) {
  return <NextThemesProvider {...props}>{children}</NextThemesProvider>;
}
