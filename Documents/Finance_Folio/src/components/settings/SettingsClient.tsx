"use client";

import { useState } from "react";
import { useForm } from "react-hook-form";
import { zodResolver } from "@hookform/resolvers/zod";
import { accountSchema } from "@/lib/validators";
import { createAccount, deleteAccount, adjustAccountBalance } from "@/actions/accounts";
import { updateProfile, changePassword, resetAllStatistics } from "@/actions/settings";
import { 
  Plus, 
  Trash2, 
  Wallet, 
  User, 
  Building,
  CreditCard,
  RefreshCw,
  ChevronDown,
  ChevronUp,
  Settings as SettingsIcon,
  ShieldAlert,
  CheckCircle,
  Save,
  Key,
  X
} from "lucide-react";

interface SettingsClientProps {
  initialAccounts: any[];
  user: any;
}

const getAccountIcon = (type: string) => {
  const t = (type || "").toLowerCase();
  if (t.includes("wallet") || t.includes("pay")) return Wallet;
  if (t.includes("saving") || t.includes("bank")) return Building;
  return CreditCard;
};

export default function SettingsClient({ initialAccounts, user }: SettingsClientProps) {
  // Global State
  const [expandedSection, setExpandedSection] = useState<string | null>("profile");
  const [loading, setLoading] = useState(false);
  const [message, setMessage] = useState<{ text: string, type: 'error' | 'success' } | null>(null);

  // Bank Accounts State
  const [accounts, setAccounts] = useState(initialAccounts);
  const [showAccountForm, setShowAccountForm] = useState(false);
  const [showSyncForm, setShowSyncForm] = useState(false);
  const [syncAccountId, setSyncAccountId] = useState("");
  const [syncBalance, setSyncBalance] = useState<number>(0);

  // Forms
  const accountForm = useForm({
    resolver: zodResolver(accountSchema),
    defaultValues: { accountName: "", accountType: "", openingBalance: 0 },
  });

  const [fullName, setFullName] = useState(user.fullName || "");
  const [dob, setDob] = useState(user.dob ? new Date(user.dob).toISOString().split('T')[0] : "");
  const [profilePic, setProfilePic] = useState(user.profilePic || "");
  const [currentPass, setCurrentPass] = useState("");
  const [newPass, setNewPass] = useState("");

  const toggleSection = (section: string) => {
    setExpandedSection(expandedSection === section ? null : section);
    setMessage(null);
  };

  // Profile Action
  const handleUpdateProfile = async () => {
    setLoading(true);
    setMessage(null);
    const res = await updateProfile({ 
      fullName, 
      dob: dob || null, 
      profilePic: profilePic || null 
    });
    if (res.success) {
      setMessage({ text: "Profile updated successfully!", type: "success" });
    } else {
      setMessage({ text: res.error || "Update failed", type: "error" });
    }
    setLoading(false);
  };

  // Password Action
  const handleChangePassword = async () => {
    setLoading(true);
    setMessage(null);
    const res = await changePassword({ current: currentPass, newPass });
    if (res.success) {
      setMessage({ text: "Password changed successfully!", type: "success" });
      setCurrentPass("");
      setNewPass("");
    } else {
      setMessage({ text: res.error || "Password change failed", type: "error" });
    }
    setLoading(false);
  };

  // Reset Statistics Action (Password + Confirm)
  const handleResetStatistics = async () => {
    const password = window.prompt("SECURITY CHECK: Please enter your login password to authorize this action:");
    if (!password) {
      alert("Reset cancelled: Password required.");
      return;
    }
    
    const confirmText = window.prompt("FINAL CONFIRMATION: All your data will be permanently wiped. Type 'CONFIRM' to execute hard reset:");
    if (confirmText !== "CONFIRM" && confirmText !== "confirm") {
      alert("Reset cancelled: Confirmation text did not match.");
      return;
    }

    setLoading(true);
    const res = await resetAllStatistics(password);
    if (res.success) {
      alert("All statistics have been reset successfully.");
      window.location.reload(); // Hard reload to fetch clean data
    } else {
      alert(res.error || "Failed to reset statistics");
    }
    setLoading(false);
  };

  // Bank Actions
  const onAddAccount = async (data: any) => {
    setLoading(true);
    const res = await createAccount(data);
    if (res.success && res.data) {
      setAccounts((prev) => [...prev, res.data]);
      accountForm.reset();
      setShowAccountForm(false);
    } else {
      setMessage({ text: res.error || "Failed to create account", type: "error" });
    }
    setLoading(false);
  };

  const handleSyncBalance = async () => {
    if (!syncAccountId) return;
    setLoading(true);
    const res = await adjustAccountBalance(syncAccountId, syncBalance);
    if (res.success && res.data) {
      setAccounts((prev) => prev.map((a) => (a.id === syncAccountId ? res.data : a)));
      setShowSyncForm(false);
      setSyncAccountId("");
      setSyncBalance(0);
    } else {
      setMessage({ text: res.error || "Failed to sync", type: "error" });
    }
    setLoading(false);
  };

  const onDeleteAccount = async (id: string) => {
    if (!confirm("Archive this account?")) return;
    const res = await deleteAccount(id);
    if (res.success) {
      setAccounts((prev) => prev.filter((a) => a.id !== id));
    }
  };

  return (
    <div className="space-y-6 max-w-4xl mx-auto">
      {/* Header */}
      <div className="mb-8">
        <h1 className="text-3xl font-extrabold tracking-tight text-slate-100 flex items-center gap-3">
          <SettingsIcon className="w-8 h-8 text-violet-400" />
          Settings
        </h1>
        <p className="text-slate-400 mt-2">Manage your profile, connected bank accounts, and security settings.</p>
      </div>

      {message && (
        <div className={`p-4 rounded-xl border flex items-center gap-3 ${
          message.type === 'error' ? 'bg-red-500/10 border-red-500/20 text-red-400' : 'bg-emerald-500/10 border-emerald-500/20 text-emerald-400'
        }`}>
          {message.type === 'error' ? <ShieldAlert className="w-5 h-5" /> : <CheckCircle className="w-5 h-5" />}
          <p className="text-sm font-medium">{message.text}</p>
        </div>
      )}

      {/* 1. Profile Settings */}
      <div className="glass-card rounded-2xl overflow-hidden border border-slate-800 transition-all">
        <button 
          onClick={() => toggleSection("profile")}
          className="w-full flex items-center justify-between p-6 bg-slate-900/50 hover:bg-slate-900/80 transition-colors"
        >
          <div className="flex items-center gap-3">
            <div className="w-10 h-10 rounded-full bg-violet-500/20 text-violet-400 flex items-center justify-center">
              <User className="w-5 h-5" />
            </div>
            <div className="text-left">
              <h2 className="font-bold text-slate-200">Profile Settings</h2>
              <p className="text-xs text-slate-500">Edit user info, name, and email</p>
            </div>
          </div>
          {expandedSection === "profile" ? <ChevronUp className="text-slate-400" /> : <ChevronDown className="text-slate-400" />}
        </button>

        {expandedSection === "profile" && (
          <div className="p-6 border-t border-slate-800">
            <div className="max-w-md space-y-4">
              {/* Profile Image Preview */}
              <div className="flex items-center gap-4 mb-2">
                <div className="w-16 h-16 rounded-full overflow-hidden bg-slate-800 border border-slate-700 flex items-center justify-center shrink-0">
                  {profilePic ? (
                    <img 
                      src={profilePic} 
                      alt="Profile" 
                      className="w-full h-full object-cover"
                      onError={(e) => {
                        (e.target as HTMLImageElement).src = "https://www.gravatar.com/avatar/00000000000000000000000000000000?d=mp&f=y";
                      }}
                    />
                  ) : (
                    <User className="w-8 h-8 text-slate-600" />
                  )}
                </div>
                <div>
                  <h3 className="text-sm font-bold text-slate-200">Profile Photo</h3>
                  <p className="text-xs text-slate-500">Provide an image URL or raw base64 string</p>
                </div>
              </div>

              {/* User ID and Email */}
              <div className="grid grid-cols-2 gap-4">
                <div>
                  <label className="text-xs font-semibold text-slate-400 block mb-1">User ID (Unchangeable)</label>
                  <input 
                    type="text" 
                    value={user.customId || "FGAT0001"} 
                    disabled 
                    className="w-full px-4 py-2.5 bg-slate-900/50 border border-slate-800 rounded-xl text-slate-400 text-sm font-mono cursor-not-allowed"
                  />
                </div>
                <div>
                  <label className="text-xs font-semibold text-slate-400 block mb-1">Email Address</label>
                  <input 
                    type="text" 
                    value={user.email} 
                    disabled 
                    className="w-full px-4 py-2.5 bg-slate-900/50 border border-slate-800 rounded-xl text-slate-500 text-sm cursor-not-allowed"
                  />
                </div>
              </div>

              {/* Name and DOB */}
              <div>
                <label className="text-xs font-semibold text-slate-400 block mb-1">Display Name</label>
                <input 
                  type="text" 
                  value={fullName}
                  onChange={(e) => setFullName(e.target.value)}
                  className="w-full px-4 py-2.5 bg-slate-950 border border-slate-800 rounded-xl focus:border-violet-500 focus:outline-none text-slate-200 text-sm"
                />
              </div>

              <div>
                <label className="text-xs font-semibold text-slate-400 block mb-1">Date of Birth</label>
                <input 
                  type="date" 
                  value={dob}
                  onChange={(e) => setDob(e.target.value)}
                  className="w-full px-4 py-2.5 bg-slate-950 border border-slate-800 rounded-xl focus:border-violet-500 focus:outline-none text-slate-200 text-sm"
                />
              </div>

              <div>
                <label className="text-xs font-semibold text-slate-400 block mb-1">Profile Photo (URL or Base64)</label>
                <input 
                  type="text" 
                  value={profilePic}
                  onChange={(e) => setProfilePic(e.target.value)}
                  placeholder="https://example.com/photo.jpg"
                  className="w-full px-4 py-2.5 bg-slate-950 border border-slate-800 rounded-xl focus:border-violet-500 focus:outline-none text-slate-200 text-sm"
                />
              </div>

              <button 
                onClick={handleUpdateProfile}
                disabled={loading}
                className="flex items-center gap-2 px-5 py-2.5 bg-violet-600 hover:bg-violet-500 text-white rounded-xl text-sm font-medium transition-colors w-full justify-center"
              >
                <Save className="w-4 h-4" />
                {loading ? "Saving..." : "Save Profile Details"}
              </button>
            </div>
          </div>
        )}
      </div>

      {/* 2. Bank Accounts */}
      <div className="glass-card rounded-2xl overflow-hidden border border-slate-800 transition-all">
        <button 
          onClick={() => toggleSection("banks")}
          className="w-full flex items-center justify-between p-6 bg-slate-900/50 hover:bg-slate-900/80 transition-colors"
        >
          <div className="flex items-center gap-3">
            <div className="w-10 h-10 rounded-full bg-cyan-500/20 text-cyan-400 flex items-center justify-center">
              <Building className="w-5 h-5" />
            </div>
            <div className="text-left">
              <h2 className="font-bold text-slate-200">Bank Accounts</h2>
              <p className="text-xs text-slate-500">Manage available bank accounts and balances</p>
            </div>
          </div>
          {expandedSection === "banks" ? <ChevronUp className="text-slate-400" /> : <ChevronDown className="text-slate-400" />}
        </button>

        {expandedSection === "banks" && (
          <div className="p-6 border-t border-slate-800">
            <div className="flex gap-3 mb-6">
              <button
                onClick={() => setShowAccountForm(!showAccountForm)}
                className="flex items-center gap-2 px-4 py-2 bg-slate-800 hover:bg-slate-700 text-slate-200 rounded-lg text-xs font-medium transition-colors"
              >
                <Plus className="w-4 h-4" /> Add Account
              </button>
              <button
                onClick={() => setShowSyncForm(!showSyncForm)}
                className="flex items-center gap-2 px-4 py-2 bg-slate-900 border border-slate-700 hover:bg-slate-800 text-slate-300 rounded-lg text-xs font-medium transition-colors"
              >
                <RefreshCw className="w-4 h-4" /> Sync Balance
              </button>
            </div>

            {/* Forms for Bank Accounts */}
            {showAccountForm && (
              <form onSubmit={accountForm.handleSubmit(onAddAccount)} className="mb-6 p-4 rounded-xl border border-cyan-500/20 bg-cyan-950/10 grid grid-cols-1 md:grid-cols-3 gap-4 items-end">
                <div>
                  <label className="text-xs font-semibold text-slate-400 mb-1 block">Account Name</label>
                  <input {...accountForm.register("accountName")} placeholder="SBI Savings" className="w-full px-3 py-2 bg-slate-950 border border-slate-800 rounded-lg text-sm text-slate-200 focus:border-cyan-500 focus:outline-none" />
                </div>
                <div>
                  <label className="text-xs font-semibold text-slate-400 mb-1 block">Type</label>
                  <select {...accountForm.register("accountType")} className="w-full px-3 py-2 bg-slate-950 border border-slate-800 rounded-lg text-sm text-slate-200 focus:border-cyan-500 focus:outline-none">
                    <option value="bank">Bank Account</option>
                    <option value="wallet">Digital Wallet</option>
                    <option value="cash">Cash</option>
                  </select>
                </div>
                <div className="flex gap-2">
                  <button type="submit" className="flex-1 py-2 bg-cyan-600 hover:bg-cyan-500 text-white rounded-lg text-sm font-medium">Add</button>
                  <button type="button" onClick={() => setShowAccountForm(false)} className="px-3 py-2 bg-slate-800 rounded-lg"><X className="w-4 h-4" /></button>
                </div>
              </form>
            )}

            {showSyncForm && (
              <div className="mb-6 p-4 rounded-xl border border-slate-700 bg-slate-800/20 grid grid-cols-1 md:grid-cols-3 gap-4 items-end">
                <div>
                  <label className="text-xs font-semibold text-slate-400 mb-1 block">Select Account</label>
                  <select value={syncAccountId} onChange={(e) => setSyncAccountId(e.target.value)} className="w-full px-3 py-2 bg-slate-950 border border-slate-800 rounded-lg text-sm text-slate-200 focus:border-slate-500 focus:outline-none">
                    <option value="">Choose...</option>
                    {accounts.map(a => <option key={a.id} value={a.id}>{a.accountName}</option>)}
                  </select>
                </div>
                <div>
                  <label className="text-xs font-semibold text-slate-400 mb-1 block">Actual Balance (₹)</label>
                  <input type="number" value={syncBalance} onChange={(e) => setSyncBalance(Number(e.target.value))} className="w-full px-3 py-2 bg-slate-950 border border-slate-800 rounded-lg text-sm text-slate-200 focus:border-slate-500 focus:outline-none" />
                </div>
                <div className="flex gap-2">
                  <button onClick={handleSyncBalance} className="flex-1 py-2 bg-slate-700 hover:bg-slate-600 text-white rounded-lg text-sm font-medium">Sync</button>
                  <button type="button" onClick={() => setShowSyncForm(false)} className="px-3 py-2 bg-slate-800 rounded-lg"><X className="w-4 h-4" /></button>
                </div>
              </div>
            )}

            {/* List */}
            <div className="grid grid-cols-1 md:grid-cols-2 gap-4">
              {accounts.map((acc) => {
                const Icon = getAccountIcon(acc.accountType);
                return (
                  <div key={acc.id} className="flex items-center justify-between p-4 rounded-xl border border-slate-800 bg-slate-900/30">
                    <div className="flex items-center gap-3">
                      <div className="w-10 h-10 rounded-xl bg-slate-800 flex items-center justify-center border border-slate-700 text-slate-400">
                        <Icon className="w-5 h-5" />
                      </div>
                      <div>
                        <h3 className="font-semibold text-slate-200 text-sm">{acc.accountName}</h3>
                        <p className="text-xs text-slate-500 capitalize">{acc.accountType}</p>
                      </div>
                    </div>
                    <div className="text-right flex items-center gap-4">
                      <div>
                        <p className="font-bold text-slate-200">₹{acc.currentBalance.toLocaleString("en-IN", { minimumFractionDigits: 2 })}</p>
                        <p className="text-[10px] text-slate-500 uppercase">Balance</p>
                      </div>
                      <button onClick={() => onDeleteAccount(acc.id)} className="text-slate-600 hover:text-red-400 transition-colors">
                        <Trash2 className="w-4 h-4" />
                      </button>
                    </div>
                  </div>
                );
              })}
            </div>
          </div>
        )}
      </div>

      {/* 3. Account Settings */}
      <div className="glass-card rounded-2xl overflow-hidden border border-red-900/30 transition-all">
        <button 
          onClick={() => toggleSection("account-security")}
          className="w-full flex items-center justify-between p-6 bg-slate-900/50 hover:bg-slate-900/80 transition-colors"
        >
          <div className="flex items-center gap-3">
            <div className="w-10 h-10 rounded-full bg-red-500/10 text-red-400 flex items-center justify-center">
              <ShieldAlert className="w-5 h-5" />
            </div>
            <div className="text-left">
              <h2 className="font-bold text-slate-200">Account Settings</h2>
              <p className="text-xs text-slate-500">Security, password, and destructive actions</p>
            </div>
          </div>
          {expandedSection === "account-security" ? <ChevronUp className="text-slate-400" /> : <ChevronDown className="text-slate-400" />}
        </button>

        {expandedSection === "account-security" && (
          <div className="p-6 border-t border-slate-800 space-y-8">
            
            {/* Password Reset */}
            <div className="max-w-md space-y-4">
              <h3 className="text-sm font-bold text-slate-200 border-b border-slate-800 pb-2">Reset Password</h3>
              <div>
                <label className="text-xs font-semibold text-slate-400 block mb-1">Current Password</label>
                <input 
                  type="password" 
                  value={currentPass}
                  onChange={(e) => setCurrentPass(e.target.value)}
                  className="w-full px-4 py-2.5 bg-slate-950 border border-slate-800 rounded-xl focus:border-slate-500 focus:outline-none text-slate-200 text-sm"
                />
              </div>
              <div>
                <label className="text-xs font-semibold text-slate-400 block mb-1">New Password</label>
                <input 
                  type="password" 
                  value={newPass}
                  onChange={(e) => setNewPass(e.target.value)}
                  className="w-full px-4 py-2.5 bg-slate-950 border border-slate-800 rounded-xl focus:border-slate-500 focus:outline-none text-slate-200 text-sm"
                />
              </div>
              <button 
                onClick={handleChangePassword}
                disabled={loading}
                className="flex items-center gap-2 px-5 py-2.5 bg-slate-800 hover:bg-slate-700 text-slate-200 rounded-xl text-sm font-medium transition-colors"
              >
                <Key className="w-4 h-4" />
                Change Password
              </button>
            </div>

            {/* Factory Reset */}
            <div className="max-w-md pt-6">
              <h3 className="text-sm font-bold text-red-400 border-b border-red-900/30 pb-2 mb-4">Danger Zone</h3>
              <div className="p-4 rounded-xl bg-red-950/20 border border-red-900/50">
                <h4 className="text-red-400 font-bold text-sm mb-1">Reset Statistics</h4>
                <p className="text-xs text-slate-400 mb-4 line-height-relaxed">
                  Permanently delete all expenses, incomes, investments, and history. Your account balances will be reset to 0. This cannot be undone.
                </p>
                <button 
                  onClick={handleResetStatistics}
                  disabled={loading}
                  className="w-full py-2.5 bg-red-600/10 hover:bg-red-600/20 text-red-500 border border-red-500/30 rounded-lg text-sm font-bold transition-colors"
                >
                  Factory Reset Statistics
                </button>
              </div>
            </div>

          </div>
        )}
      </div>
    </div>
  );
}
