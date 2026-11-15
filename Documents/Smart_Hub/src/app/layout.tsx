import { Inter } from 'next/font/google'
import './globals.css'
import { ReactNode } from 'react'

const inter = Inter({ subsets: ['latin'] })

export const metadata = {
  title: 'Smart-HUB | Home Automation',
  description: 'Secure management for W1 series devices',
}

import Providers from '@/components/Providers'

export default function RootLayout({ children }: { children: ReactNode }) {
  return (
    <html lang="en">
      <body className={inter.className} suppressHydrationWarning>
        <Providers>{children}</Providers>
      </body>
    </html>
  )
}

