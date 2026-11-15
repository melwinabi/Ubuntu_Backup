"use client"

import { useState, useEffect } from "react"
import { signIn } from "next-auth/react"
import { useRouter } from "next/navigation"
import { Mail, Loader2 } from "lucide-react"

export default function LoginPage() {
  const [activeTab, setActiveTab] = useState<"user" | "admin">("user")
  const [email, setEmail] = useState("")
  const [otp, setOtp] = useState("")
  const [otpSent, setOtpSent] = useState(false)
  const [loading, setLoading] = useState(false)
  const [error, setError] = useState("")
  const [qrCodeUrl, setQrCodeUrl] = useState("")
  const [tempSecret, setTempSecret] = useState("")
  const [totpRequired, setTotpRequired] = useState(false)
  const [isSettingUpTotp, setIsSettingUpTotp] = useState(false)
  const router = useRouter()

  const [mounted, setMounted] = useState(false)

  useEffect(() => {
    setMounted(true)
    // Check for Auth errors in URL
    const params = new URLSearchParams(window.location.search)
    const authError = params.get("error")
    if (authError) {
      if (authError === "OAuthCallback") {
        setError("Google Login failed: Invalid Client Secret in .env file.")
      } else {
        setError(`Authentication Error: ${authError}`)
      }
      setLoading(false)
    }
  }, [])

  const handleSendOtp = async (e: React.FormEvent) => {
    e.preventDefault()
    setLoading(true)
    setError("")
    try {
      // First, check if TOTP is already set up
      const checkRes = await fetch(`/api/auth/totp/setup?email=${email}`)
      const checkData = await checkRes.json()

      if (checkData.isSet) {
        // TOTP is enabled, go straight to TOTP entry
        setTotpRequired(true)
        setLoading(false)
        return
      }

      // If not set up, send Email OTP for bootstrap
      const res = await fetch("/api/auth/otp", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ email })
      })
      const data = await res.json()
      if (res.ok) {
        setOtpSent(true)
      } else {
        setError(data.error || "Failed to send OTP.")
      }
    } catch (err) {
      setError("An error occurred.")
    } finally {
      setLoading(false)
    }
  }

  const handleVerifyEmailOtp = async (e: React.FormEvent) => {
    e.preventDefault()
    setLoading(true)
    setError("")

    try {
      // Check if TOTP is already set up for this email
      const checkRes = await fetch(`/api/auth/totp/setup?email=${email}`)
      const checkData = await checkRes.json()

      if (checkData.isSet) {
        setTotpRequired(true)
        setOtpSent(false) // Swap to TOTP mode
      } else {
        // Not set up, show QR code
        const setupRes = await fetch("/api/auth/totp/setup", {
          method: "POST",
          headers: { "Content-Type": "application/json" },
          body: JSON.stringify({ email })
        })
        const setupData = await setupRes.json()
        setQrCodeUrl(setupData.qrCodeUrl)
        setTempSecret(setupData.secret)
        setIsSettingUpTotp(true)
        setOtpSent(false)
      }
    } catch (err) {
      setError("Verification failed.")
    } finally {
      setLoading(false)
    }
  }

  const handleCompleteTotpSetup = async (e: React.FormEvent) => {
    e.preventDefault()
    setLoading(true)
    setError("")

    try {
      const res = await fetch("/api/auth/totp/verify", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ email, secret: tempSecret, token: otp })
      })
      if (res.ok) {
        // Setup successful, now login
        handleAdminLogin()
      } else {
        const data = await res.json()
        setError(data.error || "Invalid TOTP code.")
      }
    } catch (err) {
      setError("Setup failed.")
    } finally {
      setLoading(false)
    }
  }

  const handleAdminLogin = async (e?: React.FormEvent) => {
    if (e) e.preventDefault()
    setLoading(true)
    setError("")
    
    const result = await signIn("admin-otp", {
      email,
      otp,
      redirect: false,
    })

    if (result?.error) {
      setError("Invalid code or expired.")
      setLoading(false)
    } else {
      router.push("/admin/dashboard")
    }
  }

  const handleUserLogin = async () => {
    if (email === "asmelwin3@gmail.com") {
      setLoading(true)
      await signIn("user-dummy", { email, callbackUrl: "/dashboard" })
      return
    }
    setLoading(true)
    await signIn("google", { callbackUrl: "/dashboard" })
  }

  if (!mounted) return <div className="flex-center" style={{ minHeight: "100vh" }}><Loader2 className="spinner" /></div>

  return (
    <div className="flex-center">
      <div className="glass-container card animate-fade-in" style={{ maxWidth: isSettingUpTotp ? "500px" : "400px" }}>
        <div style={{ textAlign: "center", marginBottom: "2rem" }}>
          <h1 style={{ fontSize: "2rem", fontWeight: "bold", color: "var(--primary)" }}>Smart-HUB</h1>
          <p style={{ color: "var(--text-secondary)" }}>Home Automation Management</p>
        </div>

        <div style={{ display: "flex", gap: "1rem", marginBottom: "2rem" }}>
          <button 
            className={`btn ${activeTab === "user" ? "btn-primary" : "btn-outline"}`}
            onClick={() => { setActiveTab("user"); setError(""); setOtpSent(false); setIsSettingUpTotp(false); setTotpRequired(false); }}
          >
            User Login
          </button>
          <button 
            className={`btn ${activeTab === "admin" ? "btn-primary" : "btn-outline"}`}
            onClick={() => { setActiveTab("admin"); setError(""); setOtpSent(false); setIsSettingUpTotp(false); setTotpRequired(false); }}
          >
            Admin Login
          </button>
        </div>

        {activeTab === "user" ? (
          <div>
            <p style={{ textAlign: "center", marginBottom: "1.5rem", color: "var(--text-secondary)" }}>
              Access your personal device dashboard
            </p>
            <div className="input-group" style={{ marginBottom: "1.5rem" }}>
              <div style={{ position: "relative" }}>
                <Mail size={18} style={{ position: "absolute", left: "12px", top: "50%", transform: "translateY(-50%)", color: "var(--text-secondary)" }} />
                <input 
                  type="email" 
                  placeholder="Enter your Gmail" 
                  value={email}
                  onChange={(e) => setEmail(e.target.value)}
                  style={{ paddingLeft: "40px" }}
                />
              </div>
            </div>
            <button 
              className="btn btn-primary" 
              onClick={handleUserLogin}
              style={{ display: "flex", alignItems: "center", justifyContent: "center", gap: "10px" }}
              disabled={loading}
            >
              <img src="https://authjs.dev/img/providers/google.svg" width="20" />
              {loading ? <Loader2 className="spinner" /> : "Login with Google"}
            </button>
          </div>
        ) : (
          <div>
            {!otpSent && !isSettingUpTotp && !totpRequired ? (
              <form onSubmit={handleSendOtp}>
                <div className="input-group">
                  <label>Admin Gmail Address</label>
                  <div style={{ position: "relative" }}>
                    <Mail size={18} style={{ position: "absolute", left: "12px", top: "50%", transform: "translateY(-50%)", color: "var(--text-secondary)" }} />
                    <input 
                      type="email" 
                      placeholder="admin@domain.com" 
                      value={email}
                      onChange={(e) => setEmail(e.target.value)}
                      style={{ paddingLeft: "40px" }}
                      required
                    />
                  </div>
                </div>
                {error && <p className="error-text" style={{ marginBottom: "1rem" }}>{error}</p>}
                <button type="submit" className="btn btn-primary" disabled={loading}>
                  {loading ? <Loader2 className="spinner" /> : "Verify Identity"}
                </button>
              </form>
            ) : otpSent ? (
              <form onSubmit={handleVerifyEmailOtp}>
                <div className="input-group">
                  <label>Enter Email OTP</label>
                  <input 
                    type="text" 
                    placeholder="000000" 
                    value={otp}
                    onChange={(e) => setOtp(e.target.value)}
                    maxLength={6}
                    required
                    autoFocus
                  />
                  <p style={{ fontSize: "0.75rem", color: "var(--text-secondary)", marginTop: "0.5rem" }}>
                    Verify email to proceed with TOTP
                  </p>
                </div>
                {error && <p className="error-text" style={{ marginBottom: "1rem" }}>{error}</p>}
                <button type="submit" className="btn btn-primary" disabled={loading}>
                  {loading ? <Loader2 className="spinner" /> : "Verify Email"}
                </button>
              </form>
            ) : isSettingUpTotp ? (
              <div style={{ textAlign: "center" }}>
                <h3 style={{ marginBottom: "1rem" }}>Setup Google Authenticator</h3>
                <p style={{ fontSize: "0.875rem", color: "var(--text-secondary)", marginBottom: "1.5rem" }}>
                  Scan this QR code in your Google Authenticator app, then enter the 6-digit code.
                </p>
                <img src={qrCodeUrl} alt="QR Code" style={{ background: "white", padding: "10px", borderRadius: "8px", margin: "0 auto 1.5rem" }} />
                <form onSubmit={handleCompleteTotpSetup}>
                  <div className="input-group">
                    <input 
                      type="text" 
                      placeholder="Enter 6-digit App Code" 
                      value={otp}
                      onChange={(e) => setOtp(e.target.value)}
                      maxLength={6}
                      required
                      autoFocus
                      style={{ textAlign: "center", fontSize: "1.25rem", letterSpacing: "4px" }}
                    />
                  </div>
                  {error && <p className="error-text" style={{ marginBottom: "1rem" }}>{error}</p>}
                  <button type="submit" className="btn btn-primary" disabled={loading}>
                    {loading ? <Loader2 className="spinner" /> : "Verify & Enable TOTP"}
                  </button>
                </form>
              </div>
            ) : totpRequired ? (
              <form onSubmit={handleAdminLogin}>
                <div className="input-group">
                  <label>Google Authenticator Code</label>
                  <input 
                    type="text" 
                    placeholder="000 000" 
                    value={otp}
                    onChange={(e) => setOtp(e.target.value)}
                    maxLength={6}
                    required
                    autoFocus
                    style={{ textAlign: "center", fontSize: "1.5rem", letterSpacing: "8px" }}
                  />
                </div>
                {error && <p className="error-text" style={{ marginBottom: "1rem" }}>{error}</p>}
                <button type="submit" className="btn btn-primary" disabled={loading}>
                  {loading ? <Loader2 className="spinner" /> : "Login Now"}
                </button>
                <button 
                  type="button" 
                  className="btn btn-outline" 
                  style={{ marginTop: "1rem" }}
                  onClick={() => setTotpRequired(false)}
                >
                  Back
                </button>
              </form>
            ) : null}
          </div>
        )}
      </div>
    </div>
  )
}
