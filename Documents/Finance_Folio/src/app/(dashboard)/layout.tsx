import { getServerSession } from "next-auth";
import { authOptions } from "@/lib/auth";
import { redirect } from "next/navigation";
import Link from "next/link";
import { 
  LayoutDashboard, 
  ArrowUpRight, 
  ArrowDownRight, 
  LineChart, 
  CreditCard, 
  Gauge, 
  Coins, 
  PiggyBank, 
  LogOut,
  User,
  Menu
} from "lucide-react";
import SidebarNav from "@/components/SidebarNav";
import UserProfileCard from "@/components/UserProfileCard";

export default async function DashboardLayout({
  children,
}: {
  children: React.ReactNode;
}) {
  const session = await getServerSession(authOptions);

  if (!session || !session.user) {
    redirect("/login");
  }

  return (
    <div className="flex h-screen w-full overflow-hidden bg-slate-950">
      {/* Sidebar - Desktop */}
      <aside className="hidden md:flex flex-col w-64 border-r border-slate-900 bg-slate-900/20 backdrop-blur-md shrink-0">
        {/* Logo */}
        <div className="h-16 flex items-center px-6 border-b border-slate-900 gap-2">
          <div className="flex items-center justify-center w-8 h-8 rounded-lg bg-violet-600/20 border border-violet-500/30 text-violet-400">
            <PiggyBank className="w-5 h-5" />
          </div>
          <span className="font-bold text-lg bg-gradient-to-r from-violet-400 to-indigo-400 bg-clip-text text-transparent">
            Finance Folio
          </span>
        </div>

        {/* Navigation */}
        <div className="flex-1 overflow-y-auto px-4 py-6">
          <SidebarNav />
        </div>

        {/* User Card & Logout */}
        <div className="p-4 border-t border-slate-900 bg-slate-950/40">
          <UserProfileCard user={session.user} />
        </div>
      </aside>

      {/* Main Content Area */}
      <div className="flex flex-col flex-1 overflow-hidden">
        {/* Header - Mobile & Action Area */}
        <header className="h-16 border-b border-slate-900 bg-slate-950/20 backdrop-blur-md flex items-center justify-between px-6 z-20">
          {/* Mobile menu trigger */}
          <div className="flex items-center gap-3 md:hidden">
            <div className="flex items-center justify-center w-8 h-8 rounded-lg bg-violet-600/20 border border-violet-500/30 text-violet-400">
              <PiggyBank className="w-5 h-5" />
            </div>
            <span className="font-bold text-base bg-gradient-to-r from-violet-400 to-indigo-400 bg-clip-text text-transparent">
              Finance Folio
            </span>
          </div>

          <div className="hidden md:flex items-center gap-1.5 text-sm text-slate-400">
            <span>Welcome back,</span>
            <span className="font-medium text-slate-200">{session.user.name}</span>
          </div>

          {/* Mobile Drawer Trigger (simplifying for now with simple buttons) */}
          <div className="md:hidden flex items-center gap-4">
            <UserProfileCard user={session.user} isMobileCompact />
          </div>
        </header>

        {/* Scrollable Content */}
        <main className="flex-1 overflow-y-auto bg-slate-950 p-6 md:p-8 relative">
          <div className="absolute top-0 right-1/4 w-[400px] h-[400px] bg-violet-600/5 rounded-full blur-[120px] pointer-events-none" />
          <div className="max-w-7xl mx-auto space-y-8 animate-fade-in relative z-10">
            {children}
          </div>
        </main>
      </div>
    </div>
  );
}
