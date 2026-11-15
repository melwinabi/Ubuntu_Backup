"use client";

import { signOut } from "next-auth/react";
import { LogOut, User } from "lucide-react";

interface UserProfileCardProps {
  user: {
    name?: string | null;
    email?: string | null;
  };
  isMobileCompact?: boolean;
}

export default function UserProfileCard({ user, isMobileCompact = false }: UserProfileCardProps) {
  const name = user.name || "User";
  const email = user.email || "";
  const initials = name
    .split(" ")
    .map((n) => n[0])
    .join("")
    .slice(0, 2)
    .toUpperCase();

  if (isMobileCompact) {
    return (
      <button
        onClick={() => signOut({ callbackUrl: "/login" })}
        className="flex items-center justify-center w-8 h-8 rounded-lg bg-slate-900 border border-slate-800 text-slate-400 hover:text-red-400 hover:border-red-500/20 hover:bg-red-500/5 transition-all"
        title="Sign Out"
      >
        <LogOut className="w-4 h-4" />
      </button>
    );
  }

  return (
    <div className="flex items-center justify-between w-full gap-3">
      <div className="flex items-center gap-3 min-w-0">
        <div className="w-10 h-10 rounded-xl bg-violet-600/10 border border-violet-500/20 text-violet-400 flex items-center justify-center font-bold text-sm shrink-0">
          {initials || <User className="w-4 h-4" />}
        </div>
        <div className="min-w-0 flex-1">
          <p className="text-sm font-semibold text-slate-200 truncate leading-none mb-1">{name}</p>
          <p className="text-xs text-slate-500 truncate leading-none">{email}</p>
        </div>
      </div>
      <button
        onClick={() => signOut({ callbackUrl: "/login" })}
        className="p-2 rounded-lg text-slate-500 hover:text-red-400 hover:bg-red-500/5 border border-transparent hover:border-red-500/10 transition-all shrink-0"
        title="Sign Out"
      >
        <LogOut className="w-4.5 h-4.5" />
      </button>
    </div>
  );
}
