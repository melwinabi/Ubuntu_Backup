"use client";

import Link from "next/link";
import { usePathname } from "next/navigation";
import { 
  LayoutDashboard, 
  ArrowUpRight, 
  ArrowDownRight, 
  LineChart, 
  CreditCard, 
  Gauge, 
  Coins, 
  PiggyBank,
  Settings
} from "lucide-react";

const NAV_ITEMS = [
  { href: "/dashboard", label: "Dashboard", icon: LayoutDashboard },
  { href: "/income", label: "Income", icon: ArrowUpRight },
  { href: "/expenses", label: "Expenses", icon: ArrowDownRight },
  { href: "/investments", label: "Investments", icon: LineChart },
  { href: "/credit-cards", label: "Credit Cards", icon: CreditCard },
  { href: "/credit-score", label: "Credit Score", icon: Gauge },
  { href: "/loans", label: "Loans Given", icon: Coins },
  { href: "/networth", label: "Net Worth", icon: PiggyBank },
  { href: "/settings", label: "Settings", icon: Settings },
];

export default function SidebarNav() {
  const pathname = usePathname();

  return (
    <nav className="space-y-1">
      {NAV_ITEMS.map((item) => {
        const Icon = item.icon;
        const isActive = pathname === item.href || (item.href !== "/dashboard" && pathname.startsWith(item.href));

        return (
          <Link
            key={item.href}
            href={item.href}
            className={`flex items-center gap-3 px-4 py-3 rounded-xl text-sm font-medium transition-all duration-200 group ${
              isActive
                ? "bg-violet-600/10 border border-violet-500/20 text-violet-400"
                : "text-slate-400 hover:bg-slate-900/50 hover:text-slate-200 border border-transparent"
            }`}
          >
            <Icon className={`w-4 h-4 transition-transform group-hover:scale-110 ${
              isActive ? "text-violet-400" : "text-slate-400 group-hover:text-slate-300"
            }`} />
            <span>{item.label}</span>
          </Link>
        );
      })}
    </nav>
  );
}
