import { useState, useEffect } from 'react'
import SockJS from 'sockjs-client'
import Stomp from 'stompjs'
import axios from 'axios'
import { ToastContainer, toast } from 'react-toastify'
import 'react-toastify/dist/ReactToastify.css'
import {
  LineChart, Line, BarChart, Bar, PieChart, Pie, Cell,
  XAxis, YAxis, CartesianGrid, Tooltip, Legend, ResponsiveContainer, Area, AreaChart
} from 'recharts'
import './App.css'

// Cấu hình Backend URL
const BACKEND_URL = 'http://34.87.133.103:8080'
const WS_URL = `${BACKEND_URL}/ws`

// Colors
const COLORS = ['#ff4757', '#ffa502', '#2ed573', '#1e90ff', '#5f27cd']

function App() {
  const [alerts, setAlerts] = useState([])
  const [history, setHistory] = useState([])
  const [systemStatus, setSystemStatus] = useState('UNKNOWN')
  const [connected, setConnected] = useState(false)
  const [devicesCount, setDevicesCount] = useState(0)
  const [stompClient, setStompClient] = useState(null)
  const [activeTab, setActiveTab] = useState('dashboard')
  
  // Statistics
  const [stats, setStats] = useState({
    totalEvents: 0,
    todayEvents: 0,
    criticalEvents: 0,
    avgConfidence: 0,
    hourlyData: [],
    severityData: [],
    deviceData: []
  })

  // WebSocket Connection
  useEffect(() => {
    let client = null
    console.log('🔌 Connecting to WebSocket...')
    const socket = new SockJS(WS_URL)
    client = Stomp.over(socket)

    client.connect(
      {},
      (frame) => {
        console.log('✅ WebSocket connected')
        setConnected(true)

        client.subscribe('/topic/alerts', (message) => {
          const alert = JSON.parse(message.body)
          console.log('🚨 Alert received:', alert)
          
          setAlerts((prev) => {
            if (prev.some(a => a.eventId === alert.eventId)) return prev
            return [alert, ...prev].slice(0, 10) // Keep only 10 most recent
          })
          
          const newHistoryEvent = {
            id: alert.eventId,
            deviceId: alert.deviceId,
            deviceName: alert.deviceId,
            eventTimestamp: alert.timestamp,
            sensorValue: alert.sensorValue,
            severity: alert.severity,
            confidence: alert.confidence
          }
          
          setHistory((prev) => {
            if (prev.some(e => e.id === alert.eventId)) return prev
            return [newHistoryEvent, ...prev]
          })
          
          // Toast notification
          toast.error(`🚨 Attack Detected! Device: ${alert.deviceId}`, {
            position: "top-right",
            autoClose: 5000,
          })
          
          // Browser notification
          if (Notification.permission === 'granted') {
            new Notification('🚨 VibraGuard Alert!', {
              body: `Device ${alert.deviceId} detected attack! (${(alert.confidence * 100).toFixed(1)}% confidence)`,
              icon: '/vite.svg'
            })
          }
        })
      },
      (error) => {
        console.error('❌ WebSocket error:', error)
        setConnected(false)
      }
    )

    setStompClient(client)

    if (Notification.permission === 'default') {
      Notification.requestPermission()
    }

    return () => {
      if (client && client.connected) {
        client.disconnect()
      }
    }
  }, [])

  // Fetch initial data
  useEffect(() => {
    fetchHistory()
    fetchSystemStatus()
    const interval = setInterval(() => {
      fetchSystemStatus()
    }, 5000)
    
    // 🔍 DEBUG: Test keyboard shortcut for panic
    const handleKeyPress = (e) => {
      if (e.key === 'p' && e.ctrlKey) {
        console.log('🔥 CTRL+P PRESSED - Testing Panic!')
        handlePanicButton()
      }
    }
    window.addEventListener('keydown', handleKeyPress)
    
    return () => {
      clearInterval(interval)
      window.removeEventListener('keydown', handleKeyPress)
    }
  }, [])

  // Calculate statistics when history changes
  useEffect(() => {
    calculateStats()
  }, [history])

  const calculateStats = () => {
    if (history.length === 0) return

    const today = new Date().toDateString()
    const todayEvents = history.filter(e => 
      new Date(e.eventTimestamp).toDateString() === today
    ).length

    const criticalEvents = history.filter(e => 
      e.severity === 'CRITICAL' || e.severity === 'WARNING'
    ).length

    const avgConf = history.reduce((sum, e) => sum + (e.confidence || 0), 0) / history.length

    // Hourly data (last 24 hours)
    const hourlyMap = {}
    history.forEach(e => {
      const hour = new Date(e.eventTimestamp).getHours()
      hourlyMap[hour] = (hourlyMap[hour] || 0) + 1
    })
    const hourlyData = Array.from({ length: 24 }, (_, i) => ({
      hour: `${i}:00`,
      events: hourlyMap[i] || 0
    }))

    // Severity distribution
    const severityMap = {}
    history.forEach(e => {
      severityMap[e.severity] = (severityMap[e.severity] || 0) + 1
    })
    const severityData = Object.entries(severityMap).map(([name, value]) => ({
      name, value
    }))

    // Device distribution
    const deviceMap = {}
    history.forEach(e => {
      const device = e.deviceName || e.deviceId
      deviceMap[device] = (deviceMap[device] || 0) + 1
    })
    const deviceData = Object.entries(deviceMap).map(([name, value]) => ({
      name, value
    }))

    setStats({
      totalEvents: history.length,
      todayEvents,
      criticalEvents,
      avgConfidence: avgConf,
      hourlyData,
      severityData,
      deviceData
    })
  }

  const fetchHistory = async () => {
    try {
      const response = await axios.get(`${BACKEND_URL}/api/events`)
      setHistory(response.data)
      console.log('📜 History fetched:', response.data)
    } catch (error) {
      console.error('❌ Error fetching history:', error)
      toast.error('Failed to fetch history')
    }
  }

  const fetchSystemStatus = async () => {
    try {
      const response = await axios.get(`${BACKEND_URL}/api/control/status`)
      setSystemStatus(response.data.systemStatus)
      setDevicesCount(response.data.devicesCount)
    } catch (error) {
      console.error('❌ Error fetching status:', error)
    }
  }

  const handleArm = async () => {
    try {
      console.log('🔒 ARM button clicked - sending request...')
      const response = await axios.post(`${BACKEND_URL}/api/control/arm`)
      console.log('✅ ARM response:', response.data)
      toast.success('✅ System ARMED')
      fetchSystemStatus()
    } catch (error) {
      console.error('❌ ARM error:', error)
      toast.error('❌ Failed to ARM system')
    }
  }

  const handleDisarm = async () => {
    try {
      console.log('🔓 DISARM button clicked - sending request...')
      const response = await axios.post(`${BACKEND_URL}/api/control/disarm`)
      console.log('✅ DISARM response:', response.data)
      toast.success('✅ System DISARMED')
      fetchSystemStatus()
    } catch (error) {
      console.error('❌ DISARM error:', error)
      toast.error('❌ Failed to DISARM system')
    }
  }

  const handlePanicButton = async () => {
    try {
      console.log('🚨 Panic button clicked - sending request...')
      const response = await axios.post(`${BACKEND_URL}/api/control/sound_alarm`)
      console.log('✅ Panic response:', response.data)
      toast.warning('🚨 PANIC ALARM ACTIVATED!')
    } catch (error) {
      console.error('❌ Panic button error:', error)
      console.error('Error details:', error.response?.data)
      toast.error(`❌ Failed to activate alarm: ${error.message}`)
    }
  }

  const formatTimestamp = (timestamp) => {
    return new Date(timestamp).toLocaleString('vi-VN', {
      year: 'numeric',
      month: '2-digit',
      day: '2-digit',
      hour: '2-digit',
      minute: '2-digit',
      second: '2-digit'
    })
  }

  return (
    <div className="app-container">
      {/* Header */}
      <header className="app-header">
        <div className="header-content">
          <div className="logo-section">
            <h1>🛡️ VibraGuard AI</h1>
            <p className="subtitle">Smart Security Monitoring System</p>
          </div>
          <div className="status-section">
            <div className={`status-badge ${connected ? 'connected' : 'disconnected'}`}>
              <span className="status-dot"></span>
              {connected ? 'Connected' : 'Disconnected'}
            </div>
            <div className={`system-status ${systemStatus.toLowerCase()}`}>
              {systemStatus === 'ARMED' ? '🔔' : '🔕'} {systemStatus}
            </div>
          </div>
        </div>
      </header>

      {/* Navigation Tabs */}
      <nav className="nav-tabs">
        <button 
          className={`tab ${activeTab === 'dashboard' ? 'active' : ''}`}
          onClick={() => setActiveTab('dashboard')}
        >
          📊 Dashboard
        </button>
        <button 
          className={`tab ${activeTab === 'events' ? 'active' : ''}`}
          onClick={() => setActiveTab('events')}
        >
          📜 Events History
        </button>
        <button 
          className={`tab ${activeTab === 'analytics' ? 'active' : ''}`}
          onClick={() => setActiveTab('analytics')}
        >
          📈 Analytics
        </button>
        <button 
          className={`tab ${activeTab === 'control' ? 'active' : ''}`}
          onClick={() => setActiveTab('control')}
        >
          ⚙️ Control Panel
        </button>
      </nav>

      <main className="main-content">
        {/* DASHBOARD TAB */}
        {activeTab === 'dashboard' && (
          <div className="dashboard-view">
            {/* Stats Cards */}
            <div className="stats-grid">
              <div className="stat-card">
                <div className="stat-icon">📊</div>
                <div className="stat-content">
                  <h3>Total Events</h3>
                  <p className="stat-value">{stats.totalEvents}</p>
                  <span className="stat-label">All time</span>
                </div>
              </div>
              
              <div className="stat-card">
                <div className="stat-icon">📅</div>
                <div className="stat-content">
                  <h3>Today's Events</h3>
                  <p className="stat-value">{stats.todayEvents}</p>
                  <span className="stat-label">Last 24 hours</span>
                </div>
              </div>
              
              <div className="stat-card">
                <div className="stat-icon">⚠️</div>
                <div className="stat-content">
                  <h3>Critical Alerts</h3>
                  <p className="stat-value critical">{stats.criticalEvents}</p>
                  <span className="stat-label">High priority</span>
                </div>
              </div>
              
              <div className="stat-card">
                <div className="stat-icon">🎯</div>
                <div className="stat-content">
                  <h3>Avg Confidence</h3>
                  <p className="stat-value">{(stats.avgConfidence * 100).toFixed(1)}%</p>
                  <span className="stat-label">AI accuracy</span>
                </div>
              </div>
            </div>

            {/* Real-time Alerts */}
            <section className="section-card">
              <h2>🚨 Recent Alerts ({alerts.length})</h2>
              <div className="alerts-grid">
                {alerts.length === 0 ? (
                  <p className="empty-message">No recent alerts</p>
                ) : (
                  alerts.map((alert, index) => (
                    <div key={index} className="alert-item">
                      <div className="alert-indicator"></div>
                      <div className="alert-details">
                        <h4>{alert.deviceId}</h4>
                        <p className="alert-severity">{alert.severity}</p>
                        {alert.confidence && (
                          <p className="alert-confidence">
                            Confidence: {(alert.confidence * 100).toFixed(1)}%
                          </p>
                        )}
                        <p className="alert-time">{formatTimestamp(alert.timestamp)}</p>
                      </div>
                    </div>
                  ))
                )}
              </div>
            </section>

            {/* Charts Row */}
            <div className="charts-row">
              <section className="chart-card">
                <h3>📈 Events by Hour (Last 24h)</h3>
                <ResponsiveContainer width="100%" height={250}>
                  <AreaChart data={stats.hourlyData}>
                    <defs>
                      <linearGradient id="colorEvents" x1="0" y1="0" x2="0" y2="1">
                        <stop offset="5%" stopColor="#1e90ff" stopOpacity={0.8}/>
                        <stop offset="95%" stopColor="#1e90ff" stopOpacity={0}/>
                      </linearGradient>
                    </defs>
                    <CartesianGrid strokeDasharray="3 3" />
                    <XAxis dataKey="hour" />
                    <YAxis />
                    <Tooltip />
                    <Area type="monotone" dataKey="events" stroke="#1e90ff" fillOpacity={1} fill="url(#colorEvents)" />
                  </AreaChart>
                </ResponsiveContainer>
              </section>

              <section className="chart-card">
                <h3>🎯 Severity Distribution</h3>
                <ResponsiveContainer width="100%" height={250}>
                  <PieChart>
                    <Pie
                      data={stats.severityData}
                      cx="50%"
                      cy="50%"
                      labelLine={false}
                      label={({ name, percent }) => `${name}: ${(percent * 100).toFixed(0)}%`}
                      outerRadius={80}
                      fill="#8884d8"
                      dataKey="value"
                    >
                      {stats.severityData.map((entry, index) => (
                        <Cell key={`cell-${index}`} fill={COLORS[index % COLORS.length]} />
                      ))}
                    </Pie>
                    <Tooltip />
                  </PieChart>
                </ResponsiveContainer>
              </section>
            </div>
          </div>
        )}

        {/* EVENTS HISTORY TAB */}
        {activeTab === 'events' && (
          <div className="events-view">
            <div className="section-header">
              <h2>📜 Events History ({history.length})</h2>
              <button className="btn-refresh" onClick={fetchHistory}>
                🔄 Refresh
              </button>
            </div>
            
            <div className="table-container">
              <table className="data-table">
                <thead>
                  <tr>
                    <th>ID</th>
                    <th>Device</th>
                    <th>Timestamp</th>
                    <th>Sensor</th>
                    <th>Confidence</th>
                    <th>Severity</th>
                  </tr>
                </thead>
                <tbody>
                  {history.length === 0 ? (
                    <tr>
                      <td colSpan="6" className="empty-message">No events recorded</td>
                    </tr>
                  ) : (
                    history.map((event) => (
                      <tr key={event.id}>
                        <td>{event.id}</td>
                        <td>{event.deviceName || event.deviceId}</td>
                        <td>{formatTimestamp(event.eventTimestamp)}</td>
                        <td>{event.sensorValue}</td>
                        <td>
                          {event.confidence ? 
                            <span className="confidence-badge">
                              {(event.confidence * 100).toFixed(1)}%
                            </span> 
                            : 'N/A'
                          }
                        </td>
                        <td>
                          <span className={`severity-badge ${event.severity.toLowerCase()}`}>
                            {event.severity}
                          </span>
                        </td>
                      </tr>
                    ))
                  )}
                </tbody>
              </table>
            </div>
          </div>
        )}

        {/* ANALYTICS TAB */}
        {activeTab === 'analytics' && (
          <div className="analytics-view">
            <h2>📈 System Analytics</h2>
            
            <div className="charts-grid">
              <section className="chart-card full-width">
                <h3>📊 Event Trends (24 Hours)</h3>
                <ResponsiveContainer width="100%" height={300}>
                  <LineChart data={stats.hourlyData}>
                    <CartesianGrid strokeDasharray="3 3" />
                    <XAxis dataKey="hour" />
                    <YAxis />
                    <Tooltip />
                    <Legend />
                    <Line type="monotone" dataKey="events" stroke="#1e90ff" strokeWidth={2} />
                  </LineChart>
                </ResponsiveContainer>
              </section>

              <section className="chart-card">
                <h3>🔍 Device Activity</h3>
                <ResponsiveContainer width="100%" height={300}>
                  <BarChart data={stats.deviceData}>
                    <CartesianGrid strokeDasharray="3 3" />
                    <XAxis dataKey="name" />
                    <YAxis />
                    <Tooltip />
                    <Bar dataKey="value" fill="#2ed573" />
                  </BarChart>
                </ResponsiveContainer>
              </section>

              <section className="chart-card">
                <h3>⚠️ Severity Breakdown</h3>
                <ResponsiveContainer width="100%" height={300}>
                  <BarChart data={stats.severityData}>
                    <CartesianGrid strokeDasharray="3 3" />
                    <XAxis dataKey="name" />
                    <YAxis />
                    <Tooltip />
                    <Bar dataKey="value" fill="#ff4757" />
                  </BarChart>
                </ResponsiveContainer>
              </section>
            </div>
          </div>
        )}

        {/* CONTROL PANEL TAB */}
        {activeTab === 'control' && (
          <div className="control-view">
            <h2>⚙️ System Control</h2>
            
            <div className="control-grid">
              <div className="control-card">
                <h3>🔔 System Status</h3>
                <div className="status-display">
                  <div className={`status-indicator ${systemStatus.toLowerCase()}`}>
                    {systemStatus}
                  </div>
                  <p>Connected Devices: <strong>{devicesCount}</strong></p>
                </div>
                
                <div className="button-group">
                  <button 
                    className="btn btn-arm" 
                    onClick={handleArm}
                    style={{ position: 'relative', zIndex: 10 }}
                  >
                    🔔 ARM System
                  </button>
                  <button 
                    className="btn btn-disarm" 
                    onClick={handleDisarm}
                    style={{ position: 'relative', zIndex: 10 }}
                  >
                    🔕 DISARM System
                  </button>
                  <button 
                    className="btn btn-panic" 
                    onClick={() => {
                      console.log('🔥 PANIC BUTTON CLICKED!')
                      handlePanicButton()
                    }}
                    style={{ 
                      position: 'relative', 
                      zIndex: 10,
                      pointerEvents: 'auto',
                      cursor: 'pointer'
                    }}
                  >
                    🚨 PANIC ALARM
                  </button>
                </div>
              </div>

              <div className="control-card">
                <h3>📊 Quick Stats</h3>
                <div className="quick-stats">
                  <div className="stat-row">
                    <span>Total Events:</span>
                    <strong>{stats.totalEvents}</strong>
                  </div>
                  <div className="stat-row">
                    <span>Today:</span>
                    <strong>{stats.todayEvents}</strong>
                  </div>
                  <div className="stat-row">
                    <span>Critical:</span>
                    <strong className="critical">{stats.criticalEvents}</strong>
                  </div>
                  <div className="stat-row">
                    <span>Devices:</span>
                    <strong>{devicesCount}</strong>
                  </div>
                </div>
              </div>
            </div>
          </div>
        )}
      </main>

      <footer className="app-footer">
        <p>© 2025 VibraGuard AI Security System | 95.1% AI Accuracy | Edge Impulse TinyML</p>
      </footer>
      
      <ToastContainer />
    </div>
  )
}

export default App
