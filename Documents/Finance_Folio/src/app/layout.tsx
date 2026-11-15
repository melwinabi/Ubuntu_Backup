import type { Metadata } from "next";
import { Geist, Geist_Mono } from "next/font/google";
import "./globals.css";
import { Providers } from "@/components/Providers";

const geistSans = Geist({
  variable: "--font-geist-sans",
  subsets: ["latin"],
});

const geistMono = Geist_Mono({
  variable: "--font-geist-mono",
  subsets: ["latin"],
});

export const metadata: Metadata = {
  title: "Finance Folio - Personal Finance & Portfolio Tracker",
  description: "Track your income, expenses, investments, loans, credit cards, credit score, and net worth in one beautiful dashboard.",
};

export default function RootLayout({
  children,
}: Readonly<{
  children: React.ReactNode;
}>) {
  return (
    <html lang="en" className="h-full bg-slate-950 text-slate-100">
      <body className={`${geistSans.variable} ${geistMono.variable} min-h-full font-sans antialiased bg-slate-950 text-slate-100`}>
        <Providers>{children}</Providers>
      </body>
    </html>
  );
}

