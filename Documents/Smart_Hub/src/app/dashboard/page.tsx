"use client"

import { useState, useEffect } from "react"
import { useSession, signOut } from "next-auth/react"
import { useRouter } from "next/navigation"
import { 
  Loader2, LogOut, Home, Search, Tv, 
  Thermometer, Droplets, Flame, Lightbulb, 
  ToggleRight, MousePointer2, Radio, Zap, Activity, Info, Settings
} from "lucide-react"

import { useMqtt } from "@/hooks/use-mqtt"

type Widget = {
  id: number
  widgetName: string
  mqttTopic: string
}

type Device = {
  id: number
  deviceName: string | null
  mqttTopic: string | null
  widgets: Widget[]
}

type Series = { id: number; seriesName: string }

export default function UserDashboard() {
  const { data: session, status } = useSession()
  const router = useRouter()
  
  // State for user-specific devices
  const [userDevices, setUserDevices] = useState<Device[]>([])
  const [deviceData, setDeviceData] = useState<Device | null>(null)
  const [loading, setLoading] = useState(true)
  const [isModalOpen, setIsModalOpen] = useState(false)

  // State for the Search/Add Modal
  const [allSeries, setAllSeries] = useState<Series[]>([])
  const [selectedSeriesId, setSelectedSeriesId] = useState("")
  const [availableDevices, setAvailableDevices] = useState<Device[]>([])
  const [selectedDeviceId, setSelectedDeviceId] = useState("")
  const [addingDevice, setAddingDevice] = useState(false)

  // MQTT Hook logic - include root topic and widget topics
  const rootTopic = deviceData?.mqttTopic ? [deviceData.mqttTopic] : []
  const widgetTopics = deviceData?.widgets.map(w => w.mqttTopic) || []
  const combinedTopics = Array.from(new Set([...rootTopic, ...widgetTopics]))
  
  const brokerUrl = "wss://mymqtt.uvfarms.in"
  const { messages, status: mqttStatus, publish } = useMqtt(brokerUrl, combinedTopics)

  useEffect(() => {
    if (session?.user?.id) {
      fetchUserDevices()
      fetchSeries()
    }
  }, [session])

  useEffect(() => {
    if (selectedSeriesId) {
      fetchAvailableDevices(selectedSeriesId)
    } else {
      setAvailableDevices([])
      setSelectedDeviceId("")
    }
  }, [selectedSeriesId])

  useEffect(() => {
    if (status === "unauthenticated") {
      router.push("/")
    }
  }, [status, router])

  const fetchUserDevices = async () => {
    try {
      const res = await fetch("/api/user/devices")
      if (res.ok) setUserDevices(await res.json())
    } catch (err) { console.error(err) }
    finally { setLoading(false) }
  }

  const fetchSeries = async () => {
    try {
      const res = await fetch("/api/series")
      if (res.ok) setAllSeries(await res.json())
    } catch (err) { console.error(err) }
  }

  const fetchAvailableDevices = async (seriesId: string) => {
    try {
      const res = await fetch(`/api/devices?seriesId=${seriesId}`)
      if (res.ok) setAvailableDevices(await res.json())
    } catch (err) { console.error(err) }
  }

  const handleAddDevice = async () => {
    if (!selectedDeviceId) return
    setAddingDevice(true)
    try {
      const res = await fetch("/api/user/devices", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ deviceId: selectedDeviceId })
      })
      if (res.ok) {
        await fetchUserDevices()
        setIsModalOpen(false)
        setSelectedDeviceId("")
        setSelectedSeriesId("")
      }
    } catch (err) { console.error(err) }
    finally { setAddingDevice(false) }
  }

  const handleOpenDevice = (device: Device) => {
    setDeviceData(device)
  }

  const [confirmUnlink, setConfirmUnlink] = useState<number | null>(null)

  const handleRemoveDevice = async (deviceId: number) => {
    setLoading(true)
    try {
      const res = await fetch(`/api/user/devices?deviceId=${deviceId}`, { method: "DELETE" })
      if (res.ok) {
        if (deviceData?.id === deviceId) setDeviceData(null)
        await fetchUserDevices()
      } else {
        const errorData = await res.json()
        alert(`Failed to remove device: ${errorData.details || errorData.error}`)
      }
    } catch (error) {
      console.error("Error removing device:", error)
    } finally {
      setLoading(false)
      setConfirmUnlink(null)
    }
  }

  const [editingWidgetId, setEditingWidgetId] = useState<string | number | null>(null)
  const [newTopic, setNewTopic] = useState("")

  const handleUpdateTopic = async (widget: any, isMainSwitch: boolean) => {
    try {
      if (isMainSwitch) {
        const res = await fetch("/api/devices", {
          method: "PATCH",
          headers: { "Content-Type": "application/json" },
          body: JSON.stringify({ id: deviceData?.id, mqttTopic: newTopic })
        })
        if (res.ok) {
           const updated = await fetch(`/api/devices?id=${deviceData?.id}`).then(r => r.json())
           setDeviceData(updated)
           fetchUserDevices()
        }
      } else {
        const res = await fetch("/api/widgets", {
          method: "PATCH",
          headers: { "Content-Type": "application/json" },
          body: JSON.stringify({ id: widget.id, mqttTopic: newTopic })
        })
        if (res.ok) {
           const updated = await fetch(`/api/devices?id=${deviceData?.id}`).then(r => r.json())
           setDeviceData(updated)
        }
      }
      setEditingWidgetId(null)
    } catch (err) { console.error(err) }
  }

  const handleDeleteWidget = async (id: number) => {
    if (!confirm("Are you sure you want to delete this widget?")) return
    try {
      const res = await fetch(`/api/widgets?id=${id}`, { method: "DELETE" })
      if (res.ok) {
        const updated = await fetch(`/api/devices?id=${deviceData?.id}`).then(r => r.json())
        setDeviceData(updated)
      }
    } catch (err) { console.error(err) }
  }

  const renderWidget = (widget: any) => {
    const isMainSwitch = widget.id === 0 && widget.widgetName === "Main Switch"
    const type = widget.widgetName.toLowerCase()
    const liveValue = messages[widget.mqttTopic] || "..."
    const isEditing = editingWidgetId === widget.id
    
    const containerStyle = {
      padding: "1.5rem",
      display: "flex",
      flexDirection: "column" as const,
      alignItems: "center",
      gap: "1rem",
      minWidth: "200px",
      position: "relative" as const
    }

    const toggleSwitch = (currentValue: string, topic: string) => {
      const isCurrentlyOn = currentValue === "1" || currentValue.toLowerCase() === "on"
      const nextValue = isCurrentlyOn ? "0" : "1"
      publish(topic, nextValue)
    }

    if (isEditing) {
      return (
        <div key={widget.id} className="glass-container" style={containerStyle}>
          <h4 style={{ fontSize: "0.875rem", color: "var(--text-secondary)" }}>Edit {widget.widgetName}</h4>
          <input 
            className="input-field"
            value={newTopic}
            onChange={(e) => setNewTopic(e.target.value)}
            placeholder="New MQTT Topic"
            style={{ fontSize: "0.875rem", padding: "0.5rem" }}
          />
          <div style={{ display: "flex", gap: "0.5rem", width: "100%" }}>
            <button className="btn btn-primary" style={{ flex: 1, padding: "0.5rem" }} onClick={() => handleUpdateTopic(widget, isMainSwitch)}>Save</button>
            <button className="btn btn-outline" style={{ flex: 1, padding: "0.5rem" }} onClick={() => setEditingWidgetId(null)}>Cancel</button>
          </div>
          {!isMainSwitch && (
            <button className="btn" style={{ background: "#ef4444", color: "white", width: "100%", padding: "0.5rem", marginTop: "0.5rem" }} onClick={() => handleDeleteWidget(widget.id)}>
              Delete Widget
            </button>
          )}
        </div>
      )
    }

    const renderHeader = () => (
      <div style={{ position: "absolute", top: "10px", right: "10px", display: "flex", gap: "5px" }}>
        <button 
          onClick={() => { setEditingWidgetId(widget.id); setNewTopic(widget.mqttTopic); }}
          style={{ background: "rgba(255,255,255,0.05)", border: "none", color: "var(--text-secondary)", cursor: "pointer", padding: "5px", borderRadius: "50%", display: "flex", alignItems: "center", justifyContent: "center" }}
          title="Edit Widget"
        >
          <Settings size={14} />
        </button>
      </div>
    )

    const baseWidget = (icon: any, content: any) => (
      <div key={widget.id} className="glass-container" style={containerStyle}>
        {renderHeader()}
        {icon}
        <p style={{ fontSize: "0.875rem", fontWeight: "500" }}>{widget.widgetName}</p>
        {content}
      </div>
    )

    if (type === "temperature") {
      return baseWidget(<Thermometer size={32} color="#f87171" />, <div style={{ fontSize: "1.5rem", fontWeight: "bold" }}>{liveValue}°C</div>)
    }
    if (type === "humidity") {
      return baseWidget(<Droplets size={32} color="#60a5fa" />, <div style={{ fontSize: "1.5rem", fontWeight: "bold" }}>{liveValue}%</div>)
    }
    if (type === "gas") {
      return baseWidget(<Flame size={32} color="#fbbf24" />, <div style={{ fontSize: "1.5rem", fontWeight: "bold" }}>{liveValue}</div>)
    }
    if (type === "light" || type === "relay" || type === "switch" || isMainSwitch) {
      const isOn = liveValue === "1" || liveValue.toLowerCase() === "on"
      return baseWidget(
        <Lightbulb size={32} color={isOn ? "#fde047" : "#4b5563"} />,
        <button 
          className={`btn ${isOn ? "btn-primary" : "btn-outline"}`} 
          style={{ padding: "0.25rem 0.5rem", width: "auto" }}
          onClick={() => toggleSwitch(liveValue, widget.mqttTopic)}
        >
          <ToggleRight /> {isOn ? "ON" : "OFF"}
        </button>
      )
    }
    if (type === "push button") {
      return baseWidget(
        <MousePointer2 size={32} color="#a855f7" />,
        <button className="btn btn-primary" style={{ padding: "0.5rem", width: "auto" }} onClick={() => publish(widget.mqttTopic, "1")}>Press</button>
      )
    }
    if (type === "motion sensor") {
      const isMotion = liveValue.toLowerCase() === "motion" || liveValue === "1"
      return baseWidget(
        <Radio size={32} color={isMotion ? "#ec4899" : "#4b5563"} />,
        <div style={{ color: isMotion ? "var(--error)" : "var(--success)", display: "flex", alignItems: "center", gap: "0.5rem", fontSize: "0.875rem" }}>
          <Activity size={16} /> {isMotion ? "Motion" : "No Motion"}
        </div>
      )
    }
    if (type === "voltage" || type === "current") {
      return baseWidget(<Zap size={32} color="#fca5a5" />, <div style={{ fontSize: "1.5rem", fontWeight: "bold" }}>{liveValue}{type === "voltage" ? "V" : "A"}</div>)
    }

    return baseWidget(<Info size={32} />, <div style={{ fontSize: "1rem" }}>{liveValue}</div>)
  }

  if (status === "loading") return <div className="flex-center" style={{ minHeight: "100vh" }}><Loader2 className="spinner" /></div>
  if (!session) return null

  return (
    <div className="main-container" style={{ padding: "2rem" }}>
      <header style={{ display: "flex", justifyContent: "space-between", alignItems: "center", marginBottom: "3rem" }}>
        <div style={{ display: "flex", alignItems: "center", gap: "1rem" }}>
          <Home size={32} color="var(--primary)" />
          <h1 style={{ fontSize: "2rem" }}>My Dashboard</h1>
        </div>
        <div style={{ display: "flex", alignItems: "center", gap: "1.5rem" }}>
          <div style={{ display: "flex", alignItems: "center", gap: "0.5rem", fontSize: "0.875rem" }}>
            <div style={{ 
              width: "8px", 
              height: "8px", 
              borderRadius: "50%", 
              background: combinedTopics.length === 0 ? "var(--text-secondary)" : (mqttStatus === "connected" ? "var(--success)" : "var(--error)") 
            }} />
            <span style={{ color: "var(--text-secondary)" }}>
              {combinedTopics.length === 0 ? "Select a Device" : (mqttStatus === "connected" ? "Live Connected" : "Connecting...")}
            </span>
          </div>
          <button className="btn" style={{ background: "var(--primary)", color: "white", width: "auto", display: "flex", gap: "0.5rem" }} onClick={() => setIsModalOpen(true)}>
            <Search size={18} /> Add Device
          </button>
          <button className="btn btn-outline" onClick={() => signOut({ callbackUrl: "/", redirect: true })} style={{ width: "auto" }}>
            <LogOut size={18} /> Logout
          </button>
        </div>
      </header>

      {/* Device View (Widgets) */}
      {deviceData ? (
        <div className="animate-fade-in">
          <button 
            onClick={() => setDeviceData(null)}
            style={{ background: "none", border: "none", color: "var(--text-secondary)", cursor: "pointer", display: "flex", alignItems: "center", gap: "0.5rem", marginBottom: "2rem" }}
          >
            <Home size={18} /> Back to My Devices
          </button>
          
          <div style={{ marginBottom: "3rem", display: "flex", justifyContent: "space-between", alignItems: "center" }}>
            <div>
              <h2 style={{ fontSize: "2.5rem" }}>{deviceData?.deviceName || "Device ID: #" + deviceData?.id}</h2>
              <p style={{ color: "var(--text-secondary)" }}>Managing IoT Node</p>
            </div>
            <button className="btn" style={{ background: "#ef4444", color: "white", width: "auto" }} onClick={() => handleRemoveDevice(deviceData.id)}>
              Unlink Device
            </button>
          </div>

          <div style={{ display: "flex", flexWrap: "wrap", gap: "1.5rem" }}>
            {deviceData?.widgets.length === 0 && deviceData?.mqttTopic ? (
               renderWidget({
                 id: 0,
                 widgetName: "Main Switch",
                 mqttTopic: deviceData.mqttTopic
               })
            ) : (
              deviceData?.widgets.map(renderWidget)
            )}
          </div>
        </div>
      ) : (
        /* Grid View (My Devices) */
        <div className="animate-fade-in">
          <h2 style={{ marginBottom: "2rem" }}>My Devices</h2>
          
          {loading ? (
            <div className="flex-center" style={{ height: "40vh" }}><Loader2 className="spinner" /></div>
          ) : userDevices.length === 0 ? (
            <div className="flex-center" style={{ height: "40vh", flexDirection: "column", gap: "1.5rem" }}>
              <Tv size={64} style={{ opacity: 0.2 }} />
              <div style={{ textAlign: "center" }}>
                <h3 style={{ marginBottom: "0.5rem" }}>No devices added yet</h3>
                <p style={{ color: "var(--text-secondary)" }}>Click the "Add Device" button above to get started.</p>
              </div>
            </div>
          ) : (
            <div style={{ display: "grid", gridTemplateColumns: "repeat(auto-fill, minmax(280px, 1fr))", gap: "1.5rem" }}>
              {userDevices.map(device => (
                <div 
                  key={device.id} 
                  className="glass-container card animate-fade-in" 
                  style={{ padding: "1.5rem", cursor: "pointer", transition: "transform 0.2s" }}
                  onClick={() => handleOpenDevice(device)}
                  onMouseEnter={(e) => e.currentTarget.style.transform = "translateY(-5px)"}
                  onMouseLeave={(e) => e.currentTarget.style.transform = "translateY(0)"}
                >
                  <div style={{ display: "flex", alignItems: "center", gap: "1rem", marginBottom: "1rem" }}>
                    <div style={{ background: "var(--primary-light)", padding: "0.75rem", borderRadius: "12px" }}>
                      <Tv size={24} color="var(--primary)" />
                    </div>
                    <div>
                      <h3 style={{ fontSize: "1.125rem" }}>{device.deviceName || "Untitled Device"}</h3>
                      <p style={{ fontSize: "0.875rem", color: "var(--text-secondary)" }}>ID: #{device.id}</p>
                    </div>
                  </div>
                  <div style={{ display: "flex", justifyContent: "space-between", alignItems: "center" }}>
                    <span style={{ fontSize: "0.875rem", color: "var(--text-secondary)" }}>{device.widgets.length} Widgets</span>
                    <div style={{ display: "flex", alignItems: "center", gap: "0.5rem", fontSize: "0.75rem", color: "var(--success)" }}>
                      <div style={{ width: "6px", height: "6px", borderRadius: "50%", background: "var(--success)" }} />
                      Online
                    </div>
                  </div>
                </div>
              ))}
            </div>
          )}
        </div>
      )}

      {/* Add Device Modal */}
      {isModalOpen && (
        <div className="modal-overlay" style={{ position: "fixed", inset: 0, background: "rgba(0,0,0,0.8)", display: "flex", alignItems: "center", justifyContent: "center", zIndex: 1000, padding: "1rem" }}>
          <div className="glass-container card animate-fade-in" style={{ padding: "2.5rem", width: "100%", maxWidth: "450px", position: "relative" }}>
             <button title="Close" onClick={() => setIsModalOpen(false)} style={{ position: "absolute", top: "1.5rem", right: "1.5rem", background: "none", border: "none", color: "var(--text-secondary)", cursor: "pointer" }}>
               <LogOut size={20} />
             </button>
             
             <h2 style={{ textAlign: "center", marginBottom: "2rem" }}>Add New Device</h2>
             
             <div className="input-group">
               <label>Select Series</label>
               <select 
                 value={selectedSeriesId} 
                 onChange={(e) => setSelectedSeriesId(e.target.value)}
                 className="select-field"
               >
                 <option value="">Select Series</option>
                 {allSeries.map(s => <option key={s.id} value={s.id.toString()}>{s.seriesName}</option>)}
               </select>
             </div>

             <div className="input-group">
               <label>Select Device ID</label>
               <select 
                 value={selectedDeviceId} 
                 onChange={(e) => setSelectedDeviceId(e.target.value)}
                 disabled={!selectedSeriesId}
                 className="select-field"
                 style={{ opacity: selectedSeriesId ? 1 : 0.5 }}
               >
                 <option value="">Select Device ID</option>
                 {availableDevices.map(d => <option key={d.id} value={d.id.toString()}>ID: #{d.id} {d.deviceName ? `(${d.deviceName})` : ""}</option>)}
               </select>
             </div>

             <button 
               className="btn btn-primary" 
               style={{ marginTop: "1rem" }}
               disabled={!selectedDeviceId || addingDevice}
               onClick={handleAddDevice}
             >
               {addingDevice ? <Loader2 className="spinner" /> : "Add to Dashboard"}
             </button>
          </div>
        </div>
      )}
      {/* Confirmation Modal */}
      {confirmUnlink !== null && (
        <div className="modal-overlay" onClick={() => setConfirmUnlink(null)}>
          <div className="modal-content glass" onClick={e => e.stopPropagation()}>
            <h2 className="modal-title">Remove Device</h2>
            <p style={{ marginBottom: "20px", color: "rgba(255,255,255,0.7)" }}>
              Are you sure you want to remove this device from your dashboard?
            </p>
            <div style={{ display: "flex", gap: "10px", justifyContent: "flex-end" }}>
              <button 
                className="btn" 
                style={{ background: "rgba(255,255,255,0.1)", color: "white", width: "auto" }}
                onClick={() => setConfirmUnlink(null)}
              >
                Cancel
              </button>
              <button 
                id="confirm-unlink-btn"
                className="btn" 
                style={{ background: "#ef4444", color: "white", width: "auto" }}
                onClick={() => handleRemoveDevice(confirmUnlink)}
              >
                {loading ? <Loader2 className="spinner" /> : "Remove"}
              </button>
            </div>
          </div>
        </div>
      )}
    </div>
  )
}
