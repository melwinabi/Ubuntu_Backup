"use client"

import { useState, useEffect } from "react"
import { signOut, useSession } from "next-auth/react"
import { 
  Search, Monitor, Cpu, LogOut, Loader2, ArrowLeft, 
  UserPlus, ChevronRight, LayoutGrid, Plus, Save, CheckCircle2,
  Settings, Database, Network, Activity
} from "lucide-react"

type DeviceData = {
  id: number
  deviceName: string | null
  subSeries: { subSeriesName: string }
  status: string
  lastSeen: string
  widgets: { widgetName: string; mqttTopic: string }[]
}

type Series = { id: number; seriesName: string }
type SubSeries = { id: number; subSeriesName: string }

export default function AdminDashboard() {
  const { data: session, status } = useSession()
  const [configUserEmail, setConfigUserEmail] = useState("")
  const [isConfigured, setIsConfigured] = useState(false)
  const [selectedSeriesId, setSelectedSeriesId] = useState<number | null>(null)
  
  // Device Search States
  const [searchDeviceId, setSearchDeviceId] = useState("")
  const [deviceResult, setDeviceResult] = useState<DeviceData | null>(null)
  
  // Add Device Workflow States
  const [isAddingDevice, setIsAddingDevice] = useState(false)
  const [workflowStep, setWorkflowStep] = useState(1)
  const [allSeries, setAllSeries] = useState<Series[]>([])
  const [allSubSeries, setAllSubSeries] = useState<SubSeries[]>([])
  
  // Creation Form State
  const [newDevice, setNewDevice] = useState({
    seriesId: "",
    subSeriesId: "",
    subSeriesName: "",
    deviceId: "",
    deviceName: "",
    serverId: "",
    mqttTopic: "",
    selectedWidgets: [] as string[],
    widgetTopics: {} as Record<string, string>
  })

  const [loading, setLoading] = useState(false)
  const [error, setError] = useState("")

  useEffect(() => {
    if (session?.user?.email) {
      setConfigUserEmail(session.user.email)
    }
  }, [session])

  useEffect(() => {
    if (isAddingDevice && workflowStep === 1) {
      fetchSeries()
    }
  }, [isAddingDevice, workflowStep])

  const fetchSeries = async () => {
    try {
      const res = await fetch("/api/series")
      if (res.ok) setAllSeries(await res.json())
    } catch (err) { console.error(err) }
  }

  const fetchSubSeries = async (id: string) => {
    try {
      const res = await fetch(`/api/sub-series?seriesId=${id}`)
      if (res.ok) setAllSubSeries(await res.json())
    } catch (err) { console.error(err) }
  }

  const handleConfigSubmit = (e: React.FormEvent) => {
    e.preventDefault()
    if (configUserEmail) setIsConfigured(true)
  }

  const handleSearch = async (e: React.FormEvent) => {
    e.preventDefault()
    if (!searchDeviceId) return
    setLoading(true)
    setError("")
    setDeviceResult(null)
    try {
      const res = await fetch(`/api/devices?id=${searchDeviceId}`)
      if (res.ok) setDeviceResult(await res.json())
      else setError("Device not found.")
    } catch (err) { setError("Failed to fetch device.") }
    finally { setLoading(false) }
  }

  const handleCreateDevice = async () => {
    setLoading(true)
    setError("")
    try {
      const widgetsPayload = newDevice.selectedWidgets.map(w => ({
        name: w,
        topic: newDevice.widgetTopics[w]
      }))

      const res = await fetch("/api/devices", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({
          ...newDevice,
          widgets: widgetsPayload
        })
      })

      if (res.ok) {
        setIsAddingDevice(false)
        setWorkflowStep(1)
        alert("Device created successfully!")
      } else {
        const data = await res.json()
        setError(data.error || "Failed to create device.")
      }
    } catch (err) { setError("An error occurred.") }
    finally { setLoading(false) }
  }

  if (status === "loading") return <div className="flex-center" style={{ minHeight: "100vh" }}><Loader2 className="spinner" /></div>
  if (status === "unauthenticated" || (session?.user as any)?.role !== "admin") {
    return <div className="flex-center" style={{ minHeight: "100vh" }}>Unauthorized. Redirecting...</div>
  }

  const isStepValid = (step: number): boolean => {
    switch(step) {
      case 1: return true
      case 2: return !!newDevice.seriesId
      case 3: return isStepValid(2) && !!newDevice.subSeriesId
      case 4: 
        const isSwitch = newDevice.subSeriesName.toLowerCase().includes("switch")
        return isStepValid(3) && !!newDevice.deviceId && !!newDevice.serverId && (isSwitch ? !!newDevice.mqttTopic : !!newDevice.deviceName)
      case 5: return isStepValid(4) && newDevice.selectedWidgets.length > 0
      default: return false
    }
  }

  const handleStepClick = (step: number) => {
    if (isStepValid(step)) {
      setWorkflowStep(step)
    }
  }

  const availableWidgetsList = [
    "Temperature", "Humidity", "Gas", "Light", "Relay", 
    "Push Button", "Motion Sensor", "Voltage", "Current"
  ]

  const renderWorkflow = () => {
    const isSwitch = newDevice.subSeriesName.toLowerCase().includes("switch")
    const stepsCount = isSwitch ? 3 : 5

    return (
      <div className="workflow-container">
        {/* Visual Stepper */}
        <div className="stepper">
          {[1, 2, 3, 4, 5].map((step) => {
            if (isSwitch && step > 3) return null
            const active = workflowStep === step
            const valid = isStepValid(step)
            return (
              <div 
                key={step} 
                className={`step-item ${active ? 'active' : ''} ${valid ? 'valid' : 'disabled'}`}
                onClick={() => handleStepClick(step)}
              >
                <div className="step-number">{step}</div>
                <div className="step-label">
                  {step === 1 && "Series"}
                  {step === 2 && "Model"}
                  {step === 3 && (isSwitch ? "Finish" : "Info")}
                  {step === 4 && "Widgets"}
                  {step === 5 && "MQTT"}
                </div>
                {step < stepsCount && <div className="step-connector" />}
              </div>
            )
          })}
        </div>

        <div className="glass-container workflow-card">
          {(() => {
            switch(workflowStep) {
      case 1:
        return (
          <div className="animate-fade-in">
            <h2 className="step-title">Step 1: Select Device Series</h2>
            <div className="input-group">
              <label>Device Series</label>
              <select 
                value={newDevice.seriesId} 
                onChange={(e) => {
                  const id = e.target.value
                  setNewDevice({...newDevice, seriesId: id})
                  fetchSubSeries(id)
                }}
                className="select-field"
              >
                <option value="">Select Series</option>
                {allSeries.map(s => <option key={s.id} value={s.id}>{s.seriesName}</option>)}
              </select>
            </div>
            <button 
              className="btn btn-primary" 
              disabled={!newDevice.seriesId}
              onClick={() => setWorkflowStep(2)}
            >
              Next
            </button>
          </div>
        )
      case 2:
        return (
          <div className="animate-fade-in">
            <h2 className="step-title">Step 2: Select Sub-Series</h2>
            <div style={{ display: "grid", gap: "1rem" }}>
              {allSubSeries.map(ss => (
                <div 
                  key={ss.id} 
                  className={`glass-container selectable-card ${String(newDevice.subSeriesId) === String(ss.id) ? "active" : ""}`}
                  style={{ 
                    border: String(newDevice.subSeriesId) === String(ss.id) ? "2px solid var(--primary)" : "1px solid var(--glass-border)",
                    transform: String(newDevice.subSeriesId) === String(ss.id) ? "scale(1.02)" : "scale(1)"
                  }}
                  onClick={() => {
                    setNewDevice({...newDevice, subSeriesId: ss.id.toString(), subSeriesName: ss.subSeriesName});
                    // Auto-advance after small delay for visual feedback
                    setTimeout(() => setWorkflowStep(3), 300);
                  }}
                >
                  <Cpu size={24} style={{ color: String(newDevice.subSeriesId) === String(ss.id) ? "var(--primary)" : "inherit" }} />
                  <span style={{ fontWeight: String(newDevice.subSeriesId) === String(ss.id) ? "bold" : "normal" }}>
                    {ss.subSeriesName}
                  </span>
                </div>
              ))}
            </div>
            <div style={{ display: "flex", gap: "1rem", marginTop: "2rem" }}>
              <button className="btn btn-outline" onClick={() => setWorkflowStep(1)}>Back</button>
              <button 
                className="btn btn-primary" 
                disabled={!newDevice.subSeriesId}
                onClick={() => setWorkflowStep(3)}
              >
                Next
              </button>
            </div>
          </div>
        )
      case 3:
        const subName = newDevice.subSeriesName.toLowerCase()
        const isSwitch = subName.includes("switch")
        return (
          <div className="animate-fade-in">
            <h2 className="step-title">Step 3: {isSwitch ? "Configuration" : "Device Info"}</h2>
            <div className="input-grid">
              <div className="input-group">
                <label>Device ID</label>
                <input type="number" placeholder="e.g. 1" value={newDevice.deviceId} onChange={e => setNewDevice({...newDevice, deviceId: e.target.value})} />
              </div>
              <div className="input-group">
                <label>Server ID</label>
                <input type="text" placeholder="server_01" value={newDevice.serverId} onChange={e => setNewDevice({...newDevice, serverId: e.target.value})} />
              </div>
              {isSwitch ? (
                <div className="input-group" style={{ gridColumn: "span 2" }}>
                  <label>MQTT Topic</label>
                  <input type="text" placeholder="w1/switch/1" value={newDevice.mqttTopic} onChange={e => setNewDevice({...newDevice, mqttTopic: e.target.value})} />
                </div>
              ) : (
                <div className="input-group" style={{ gridColumn: "span 2" }}>
                  <label>Device Name (Location)</label>
                  <input type="text" placeholder="Living Room Hub" value={newDevice.deviceName} onChange={e => setNewDevice({...newDevice, deviceName: e.target.value})} />
                </div>
              )}
            </div>
            {error && <p className="error-text">{error}</p>}
            <div style={{ display: "flex", gap: "1rem", marginTop: "2rem" }}>
              <button className="btn btn-outline" onClick={() => setWorkflowStep(2)}>Back</button>
              <button 
                className="btn btn-primary" 
                disabled={!newDevice.deviceId || !newDevice.serverId || (isSwitch ? !newDevice.mqttTopic : !newDevice.deviceName)}
                onClick={() => isSwitch ? handleCreateDevice() : setWorkflowStep(4)}
              >
                {isSwitch ? "SAVE" : "NEXT"}
              </button>
            </div>
          </div>
        )
      case 4:
        return (
          <div className="animate-fade-in">
            <h2 className="step-title">Step 4: Select Widgets</h2>
            <div className="checkbox-grid">
              {availableWidgetsList.map(w => (
                <label key={w} className="checkbox-item">
                  <input 
                    type="checkbox" 
                    checked={newDevice.selectedWidgets.includes(w)}
                    onChange={(e) => {
                      const selected = e.target.checked 
                        ? [...newDevice.selectedWidgets, w]
                        : newDevice.selectedWidgets.filter(item => item !== w)
                      setNewDevice({...newDevice, selectedWidgets: selected})
                    }}
                  />
                  <span>{w}</span>
                </label>
              ))}
            </div>
            <div style={{ display: "flex", gap: "1rem", marginTop: "2rem" }}>
              <button className="btn btn-outline" onClick={() => setWorkflowStep(3)}>Back</button>
              <button 
                className="btn btn-primary" 
                disabled={newDevice.selectedWidgets.length === 0}
                onClick={() => setWorkflowStep(5)}
              >
                NEXT
              </button>
            </div>
          </div>
        )
      case 5:
        return (
          <div className="animate-fade-in">
            <h2 className="step-title">Step 5: Enter MQTT Topics</h2>
            <div style={{ display: "grid", gap: "1rem" }}>
              {newDevice.selectedWidgets.map(w => (
                <div key={w} className="input-group">
                  <label>{w}:</label>
                  <input 
                    type="text" 
                    placeholder={`e.g. w1/hub/${newDevice.deviceId}/${w.toLowerCase().replace(" ", "")}`}
                    value={newDevice.widgetTopics[w] || ""}
                    onChange={(e) => setNewDevice({
                      ...newDevice, 
                      widgetTopics: {...newDevice.widgetTopics, [w]: e.target.value}
                    })}
                  />
                </div>
              ))}
            </div>
            {error && <p className="error-text">{error}</p>}
            <div style={{ display: "flex", gap: "1rem", marginTop: "2rem" }}>
              <button className="btn btn-outline" onClick={() => setWorkflowStep(4)}>Back</button>
              <button 
                className="btn btn-primary" 
                disabled={Object.keys(newDevice.widgetTopics).length < newDevice.selectedWidgets.length}
                onClick={handleCreateDevice}
              >
                {loading ? <Loader2 className="spinner" /> : <><Save size={18} /> SAVE DEVICE</>}
              </button>
            </div>
          </div>
        )
            }
          })()}
        </div>
      </div>
    )
  }

  return (
    <div className="dashboard-layout">
      {/* Sidebar */}
      <aside className="sidebar">
        <h2 style={{ fontSize: "1.5rem", fontWeight: "bold", color: "var(--primary)", marginBottom: "2rem" }}>Smart-HUB</h2>
        <nav style={{ flex: 1 }}>
          <div 
            onClick={() => { setIsAddingDevice(false); setSelectedSeriesId(null); }}
            className={`nav-item ${!isAddingDevice && !selectedSeriesId ? "active" : ""}`}
          >
            <Database size={20} />
            <span>Overview</span>
          </div>
          <div 
            onClick={() => { setIsAddingDevice(true); setWorkflowStep(1); }}
            className={`nav-item ${isAddingDevice ? "active" : ""}`}
            style={{ color: "var(--success)" }}
          >
            <Plus size={20} />
            <span>Add Device</span>
          </div>
        </nav>
        <button className="btn btn-outline" onClick={() => signOut({ callbackUrl: "/", redirect: true })} style={{ justifyContent: "flex-start" }}>
          <LogOut size={20} />
          Logout
        </button>
      </aside>

      {/* Main Content */}
      <main className="main-content">
        {isAddingDevice ? (
          renderWorkflow()
        ) : (
          <div className="animate-fade-in">
            <header style={{ marginBottom: "3rem", display: "flex", justifyContent: "space-between", alignItems: "flex-end" }}>
              <div>
                <h1 style={{ fontSize: "2.5rem", marginBottom: "0.5rem" }}>System Management</h1>
                <p style={{ color: "var(--text-secondary)" }}>Managing infrastructure for: <strong>{configUserEmail}</strong></p>
              </div>
              <button className="btn btn-primary" style={{ width: "auto" }} onClick={() => { setIsAddingDevice(true); setWorkflowStep(1); }}>
                <Plus size={18} /> Add New Device
              </button>
            </header>

            <div className="stats-grid">
              <div className="glass-container stat-card">
                <Network className="stat-icon" />
                <div><p>Active Series</p><h3>W1_Series</h3></div>
              </div>
              <div className="glass-container stat-card">
                <Activity className="stat-icon" />
                <div><p>System Status</p><h3 style={{ color: "var(--success)" }}>Operational</h3></div>
              </div>
            </div>

            <div className="search-section">
              <h2 style={{ marginBottom: "1rem" }}>Quick Search</h2>
              <form onSubmit={handleSearch} className="glass-container search-bar">
                <Search size={20} />
                <input type="number" placeholder="Enter Device ID to inspect..." value={searchDeviceId} onChange={e => setSearchDeviceId(e.target.value)} />
                <button type="submit" className="btn btn-primary" style={{ width: "auto" }} disabled={loading}>
                  {loading ? <Loader2 className="spinner" /> : "Inspect"}
                </button>
              </form>
            </div>

            {error && <div className="error-box">{error}</div>}

            {deviceResult && (
              <div className="glass-container result-card animate-fade-in">
                <div className="result-header">
                  <div>
                    <h2>{deviceResult.deviceName || deviceResult.subSeries.subSeriesName}</h2>
                    <p>ID: #{deviceResult.id} | {deviceResult.subSeries.subSeriesName}</p>
                  </div>
                  <span className={`status-badge ${deviceResult.status.toLowerCase()}`}>{deviceResult.status}</span>
                </div>
                {deviceResult.widgets.length > 0 && (
                  <div className="result-widgets">
                    <h3>Configured Widgets</h3>
                    <div className="widget-pills">
                      {deviceResult.widgets.map(w => (
                        <span key={w.widgetName} className="widget-pill">
                          {w.widgetName}
                          <small>{w.mqttTopic}</small>
                        </span>
                      ))}
                    </div>
                  </div>
                )}
              </div>
            )}
          </div>
        )}
      </main>

      <style jsx>{`
        .nav-item {
          display: flex;
          align-items: center;
          gap: 0.75rem;
          padding: 0.75rem 1rem;
          border-radius: 8px;
          color: var(--text-secondary);
          cursor: pointer;
          transition: all 0.2s;
          margin-bottom: 0.5rem;
        }
        .nav-item:hover { background: var(--glass-bg); color: var(--text-primary); }
        .nav-item.active { background: var(--glass-bg); color: var(--primary); font-weight: 600; }
        
        .workflow-card { padding: 3rem; max-width: 600px; margin: 0 auto; }
        .step-title { margin-bottom: 2rem; font-size: 1.5rem; color: var(--primary); }
        
        .select-field {
          width: 100%;
          padding: 0.75rem;
          background: var(--glass-bg);
          border: 1px solid var(--glass-border);
          border-radius: 8px;
          color: white;
          outline: none;
        }
        
        .selectable-card {
          padding: 1.5rem;
          display: flex;
          align-items: center;
          gap: 1.5rem;
          cursor: pointer;
          transition: all 0.2s;
        }
        .selectable-card:hover { border-color: var(--primary); }
        .selectable-card.active { border-color: var(--primary); background: var(--glass-bg); }
        
        .input-grid { display: grid; grid-template-columns: 1fr 1fr; gap: 1rem; }
        .checkbox-grid { display: grid; grid-template-columns: 1fr 1fr; gap: 1rem; }
        .checkbox-item {
          display: flex;
          align-items: center;
          gap: 0.75rem;
          padding: 0.75rem;
          background: var(--glass-bg);
          border-radius: 8px;
          cursor: pointer;
        }

        /* Stepper Styles */
        .stepper {
          display: flex;
          justify-content: space-between;
          margin-bottom: 2rem;
          padding: 0 1rem;
        }
        .step-item {
          display: flex;
          flex-direction: column;
          align-items: center;
          gap: 0.5rem;
          position: relative;
          flex: 1;
          cursor: pointer;
          transition: opacity 0.2s;
        }
        .step-item.disabled { cursor: not-allowed; opacity: 0.5; }
        .step-number {
          width: 32px;
          height: 32px;
          border-radius: 50%;
          background: var(--glass-bg);
          border: 1px solid var(--glass-border);
          display: flex;
          align-items: center;
          justify-content: center;
          font-weight: bold;
          font-size: 0.875rem;
          z-index: 2;
        }
        .step-item.active .step-number { background: var(--primary); border-color: var(--primary); color: white; }
        .step-item.valid:not(.active) .step-number { border-color: var(--primary); color: var(--primary); }
        .step-label { font-size: 0.75rem; color: var(--text-secondary); text-transform: uppercase; letter-spacing: 0.05em; }
        .step-item.active .step-label { color: var(--primary); font-weight: bold; }
        .step-connector {
          position: absolute;
          top: 16px;
          left: calc(50% + 16px);
          right: calc(-50% + 16px);
          height: 2px;
          background: var(--glass-border);
          z-index: 1;
        }
        .step-item.valid .step-connector { background: var(--primary); }
        
        .stats-grid { display: grid; grid-template-columns: 1fr 1fr; gap: 1.5rem; margin-bottom: 3rem; }
        .stat-card { padding: 1.5rem; display: flex; align-items: center; gap: 1.5rem; }
        .stat-icon { color: var(--primary); font-size: 32px; }
        
        .search-bar { padding: 0.75rem 1.5rem; display: flex; align-items: center; gap: 1rem; }
        .search-bar input { flex: 1; background: none; border: none; color: white; outline: none; font-size: 1rem; }
        
        .result-card { margin-top: 2rem; padding: 2rem; }
        .result-header { display: flex; justify-content: space-between; align-items: flex-start; margin-bottom: 2rem; }
        .widget-pills { display: flex; flex-wrap: wrap; gap: 0.75rem; margin-top: 1rem; }
        .widget-pill {
          padding: 0.5rem 1rem;
          background: var(--glass-bg);
          border: 1px solid var(--glass-border);
          border-radius: 999px;
          display: flex;
          flex-direction: column;
          font-size: 0.875rem;
        }
        .widget-pill small { font-size: 0.65rem; color: var(--text-secondary); }
        
        .error-box { padding: 1rem; background: rgba(239, 68, 68, 0.1); border: 1px solid var(--error); border-radius: 8px; color: var(--error); margin-top: 2rem; }
      `}</style>
    </div>
  )
}
