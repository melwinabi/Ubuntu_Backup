"use client";

import { useState } from "react";
import { signIn } from "next-auth/react";
import { useRouter } from "next/navigation";
import Link from "next/link";
import { useForm } from "react-hook-form";
import { zodResolver } from "@hookform/resolvers/zod";
import { loginSchema } from "@/lib/validators";
import { KeyRound, Mail, AlertTriangle, Eye, EyeOff } from "lucide-react";

export default function LoginPage() {
  const router = useRouter();
  const [error, setError] = useState<string | null>(null);
  const [loading, setLoading] = useState(false);
  const [showPassword, setShowPassword] = useState(false);

  const {
    register,
    handleSubmit,
    formState: { errors },
  } = useForm({
    resolver: zodResolver(loginSchema),
    defaultValues: {
      email: "",
      password: "",
    },
  });

  const onSubmit = async (data: any) => {
    setLoading(true);
    setError(null);

    try {
      const result = await signIn("credentials", {
        redirect: false,
        email: data.email,
        password: data.password,
      });

      if (result?.error) {
        setError(result.error);
      } else {
        router.push("/dashboard");
        router.refresh();
      }
    } catch (err) {
      setError("An unexpected error occurred. Please try again.");
    } finally {
      setLoading(false);
    }
  };

  return (
    <div className="min-h-screen w-full flex items-center justify-center relative overflow-hidden bg-slate-950 p-4">
      {/* Background Ambient Glows */}
      <div className="absolute top-1/4 left-1/4 -translate-x-1/2 -translate-y-1/2 w-[300px] h-[300px] bg-violet-600/15 rounded-full blur-[100px] pointer-events-none" />
      <div className="absolute bottom-1/4 right-1/4 translate-x-1/2 translate-y-1/2 w-[350px] h-[350px] bg-indigo-600/10 rounded-full blur-[120px] pointer-events-none" />

      <div className="w-full max-w-md glass-card rounded-2xl p-8 animate-fade-in relative z-10">
        <div className="text-center mb-8">
          <div className="inline-flex items-center justify-center w-12 h-12 rounded-xl bg-violet-500/10 border border-violet-500/20 text-violet-400 mb-3 glow-primary">
            <KeyRound className="w-6 h-6" />
          </div>
          <h1 className="text-2xl font-bold tracking-tight bg-gradient-to-r from-violet-400 via-indigo-300 to-indigo-400 bg-clip-text text-transparent">
            Welcome back
          </h1>
          <p className="text-sm text-slate-400 mt-1">
            Sign in to manage your personal portfolio
          </p>
        </div>

        {error && (
          <div className="mb-6 p-4 rounded-lg bg-red-500/10 border border-red-500/20 text-red-400 text-sm flex items-start gap-3">
            <AlertTriangle className="w-5 h-5 shrink-0 mt-0.5" />
            <p>{error}</p>
          </div>
        )}

        <form onSubmit={handleSubmit(onSubmit)} className="space-y-5">
          <div className="space-y-1.5">
            <label className="text-xs font-semibold text-slate-300 uppercase tracking-wider">
              Email Address
            </label>
            <div className="relative">
              <span className="absolute inset-y-0 left-0 flex items-center pl-3 text-slate-500">
                <Mail className="w-5 h-5" />
              </span>
              <input
                {...register("email")}
                type="email"
                placeholder="you@example.com"
                className="w-full pl-10 pr-4 py-2.5 bg-slate-900/50 border border-slate-800 rounded-xl focus:border-violet-500 focus:outline-none text-slate-100 placeholder-slate-500 transition-colors"
              />
            </div>
            {errors.email && (
              <p className="text-xs text-red-400">{errors.email.message}</p>
            )}
          </div>

          <div className="space-y-1.5">
            <label className="text-xs font-semibold text-slate-300 uppercase tracking-wider">
              Password
            </label>
            <div className="relative">
              <span className="absolute inset-y-0 left-0 flex items-center pl-3 text-slate-500">
                <KeyRound className="w-5 h-5" />
              </span>
              <input
                {...register("password")}
                type={showPassword ? "text" : "password"}
                placeholder="••••••••"
                className="w-full pl-10 pr-10 py-2.5 bg-slate-900/50 border border-slate-800 rounded-xl focus:border-violet-500 focus:outline-none text-slate-100 placeholder-slate-500 transition-colors"
              />
              <button
                type="button"
                onClick={() => setShowPassword(!showPassword)}
                className="absolute inset-y-0 right-0 flex items-center pr-3 text-slate-500 hover:text-slate-300"
              >
                {showPassword ? <EyeOff className="w-5 h-5" /> : <Eye className="w-5 h-5" />}
              </button>
            </div>
            {errors.password && (
              <p className="text-xs text-red-400">{errors.password.message}</p>
            )}
          </div>

          <button
            type="submit"
            disabled={loading}
            className="w-full py-3 bg-violet-600 hover:bg-violet-500 disabled:opacity-50 text-white font-medium rounded-xl shadow-lg shadow-violet-600/20 transition-all hover:scale-[1.01] active:scale-[0.99] flex items-center justify-center"
          >
            {loading ? (
              <svg className="animate-spin h-5 w-5 text-white" fill="none" viewBox="0 0 24 24">
                <circle className="opacity-25" cx="12" cy="12" r="10" stroke="currentColor" strokeWidth="4" />
                <path className="opacity-75" fill="currentColor" d="M4 12a8 8 0 018-8V0C5.373 0 0 5.373 0 12h4zm2 5.291A7.962 7.962 0 014 12H0c0 3.042 1.135 5.824 3 7.938l3-2.647z" />
              </svg>
            ) : (
              "Sign In"
            )}
          </button>
        </form>

        <p className="text-center text-sm text-slate-400 mt-6">
          Don&apos;t have an account?{" "}
          <Link href="/register" className="text-violet-400 hover:text-violet-300 font-medium transition-colors">
            Register here
          </Link>
        </p>
      </div>
    </div>
  );
}
