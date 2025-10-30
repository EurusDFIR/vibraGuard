import { useState, useEffect } from 'react'
import SockJS from 'sockjs-client'
import Stomp from 'stompjs'
import axios from 'axios'
import { ToastContainer, toast } from 'react-toastify'
import 'react-toastify/dist/ReactToastify.css'
import './App.css'

// Cấu hình Backend URL
const BACKEND_URL = 'http://localhost:8080'
const WS_URL = `${BACKEND_URL}/ws`

function App() {
  const [alerts, setAlerts] = useState([])
  const [history, setHistory] = useState([])
  const [systemStatus, setSystemStatus] = useState('UNKNOWN')
  const [connected, setConnected] = useState(false)
  const [stompClient, setStompClient] = useState(null)

  // Kết nối WebSocket
  useEffect(() => {
    let client = null
    let isSubscribed = false

    console.log('🔌 Connecting to WebSocket...')
    const socket = new SockJS(WS_URL)
    client = Stomp.over(socket)

    client.connect(
      {},
      (frame) => {
        console.log('✅ WebSocket connected:', frame)
        setConnected(true)
        isSubscribed = true

        // Subscribe vào topic /topic/alerts
        client.subscribe('/topic/alerts', (message) => {
          const alert = JSON.parse(message.body)
          console.log('🚨 Alert received:', alert)
          
          // 🛡️ Ngăn duplicate: Kiểm tra alert đã tồn tại chưa (dựa trên eventId)
          setAlerts((prev) => {
            // Nếu alert với eventId này đã tồn tại, bỏ qua
            if (prev.some(a => a.eventId === alert.eventId)) {
              console.log('⚠️ Duplicate alert ignored:', alert.eventId)
              return prev
            }
            return [alert, ...prev]
          })
          
          // 🆕 Tự động thêm vào history table (không cần reload)
          const newHistoryEvent = {
            id: alert.eventId,
            device: {
              deviceName: alert.deviceId // Sẽ hiển thị deviceId nếu không có deviceName
            },
            eventTimestamp: alert.timestamp,
            sensorValue: alert.sensorValue,
            severity: alert.severity
          }
          setHistory((prev) => {
            // Ngăn duplicate trong history table
            if (prev.some(e => e.id === alert.eventId)) {
              return prev
            }
            return [newHistoryEvent, ...prev]
          })
          
          // Hiển thị notification trên browser
          if (Notification.permission === 'granted') {
            new Notification('🚨 VibraGuard Alert!', {
              body: `Device ${alert.deviceId} detected vibration! (${(alert.confidence * 100).toFixed(1)}% confidence)`,
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

    // Request notification permission
    if (Notification.permission === 'default') {
      Notification.requestPermission()
    }

    // Cleanup: Disconnect khi component unmount
    return () => {
      console.log('🔌 Cleaning up WebSocket connection...')
      if (client && client.connected) {
        client.disconnect(() => {
          console.log('✅ WebSocket disconnected')
        })
      }
    }
  }, []) // Empty dependency - chỉ chạy 1 lần khi mount

  // Lấy lịch sử sự kiện
  useEffect(() => {
    fetchHistory()
    fetchSystemStatus()
  }, [])

  const fetchHistory = async () => {
    try {
      const response = await axios.get(`${BACKEND_URL}/api/events`)
      console.log('📜 History fetched:', response.data)
      setHistory(response.data)
    } catch (error) {
      console.error('❌ Error fetching history:', error)
    }
  }

  const fetchSystemStatus = async () => {
    try {
      const response = await axios.get(`${BACKEND_URL}/api/control/status`)
      console.log('📊 Status fetched:', response.data)
      setSystemStatus(response.data.systemStatus)
    } catch (error) {
      console.error('❌ Error fetching status:', error)
    }
  }

  const handleArm = async () => {
    try {
      const response = await axios.post(`${BACKEND_URL}/api/control/arm`)
      console.log('🔔 ARM response:', response.data)
      setSystemStatus('ARMED')
      toast.success('✅ Hệ thống đã được kích hoạt!', {
        position: "top-right",
        autoClose: 3000,
      })
    } catch (error) {
      console.error('❌ Error arming system:', error)
      toast.error('❌ Lỗi khi kích hoạt hệ thống!', {
        position: "top-right",
        autoClose: 3000,
      })
    }
  }

  const handleDisarm = async () => {
    try {
      const response = await axios.post(`${BACKEND_URL}/api/control/disarm`)
      console.log('🔕 DISARM response:', response.data)
      setSystemStatus('DISARMED')
      toast.success('✅ Báo động đã được tắt!', {
        position: "top-right",
        autoClose: 3000,
      })
    } catch (error) {
      console.error('❌ Error disarming system:', error)
      toast.error('❌ Lỗi khi tắt báo động!', {
        position: "top-right",
        autoClose: 3000,
      })
    }
  }

  const handlePanicButton = async () => {
    try {
      const response = await axios.post(`${BACKEND_URL}/api/control/sound_alarm`)
      console.log('🚨 SOUND_ALARM response:', response.data)
      toast.warning('🚨 Còi báo động thủ công đã được kích hoạt!', {
        position: "top-right",
        autoClose: 5000,
      })
    } catch (error) {
      console.error('❌ Error activating manual alarm:', error)
      toast.error('❌ Lỗi khi kích hoạt còi thủ công!', {
        position: "top-right",
        autoClose: 3000,
      })
    }
  }

  const formatTimestamp = (timestamp) => {
    return new Date(timestamp).toLocaleString('vi-VN')
  }

  return (
    <div className="app">
      <header className="app-header">
        <h1>🛡️ VibraGuard Security System</h1>
        <div className="status-bar">
          <span className={`status-badge ${connected ? 'connected' : 'disconnected'}`}>
            {connected ? '🟢 Connected' : '🔴 Disconnected'}
          </span>
          <span className={`status-badge ${systemStatus === 'ARMED' ? 'armed' : 'disarmed'}`}>
            {systemStatus === 'ARMED' ? '🔔 ARMED' : '🔕 DISARMED'}
          </span>
        </div>
      </header>

      <main className="main-content">
        {/* Control Panel */}
        <section className="control-panel">
          <h2>⚙️ Điều khiển</h2>
          <div className="button-group">
            <button className="btn btn-arm" onClick={handleArm}>
              🔔 Kích hoạt (ARM)
            </button>
            <button className="btn btn-disarm" onClick={handleDisarm}>
              🔕 Tắt báo động (DISARM)
            </button>
            <button className="btn btn-panic" onClick={handlePanicButton} title="Bật còi báo động thủ công">
              🚨 Kêu Còi Ngay (Panic)
            </button>
            <button className="btn btn-refresh" onClick={fetchHistory}>
              🔄 Làm mới
            </button>
          </div>
        </section>

        {/* Real-time Alerts */}
        <section className="alerts-section">
          <h2>🚨 Cảnh báo Real-time ({alerts.length})</h2>
          <div className="alerts-list">
            {alerts.length === 0 ? (
              <p className="empty-message">Không có cảnh báo mới</p>
            ) : (
              alerts.map((alert, index) => (
                <div key={index} className="alert-card">
                  <div className="alert-icon">🚨</div>
                  <div className="alert-content">
                    <h3>Device: {alert.deviceId}</h3>
                    <p>Severity: <span className="severity">{alert.severity}</span></p>
                    <p>Sensor Value: {alert.sensorValue}</p>
                    {alert.confidence && <p>AI Confidence: {(alert.confidence * 100).toFixed(1)}%</p>}
                    <p className="timestamp">{formatTimestamp(alert.timestamp)}</p>
                  </div>
                </div>
              ))
            )}
          </div>
        </section>

        {/* Event History */}
        <section className="history-section">
          <h2>📜 Lịch sử Sự kiện ({history.length})</h2>
          <div className="history-table-container">
            <table className="history-table">
              <thead>
                <tr>
                  <th>ID</th>
                  <th>Device</th>
                  <th>Thời gian</th>
                  <th>Sensor</th>
                  <th>Mức độ</th>
                </tr>
              </thead>
              <tbody>
                {history.length === 0 ? (
                  <tr>
                    <td colSpan="5" className="empty-message">Chưa có sự kiện nào</td>
                  </tr>
                ) : (
                  history.map((event) => (
                    <tr key={event.id}>
                      <td>{event.id}</td>
                      <td>{event.deviceName || event.deviceId || 'Unknown'}</td>
                      <td>{formatTimestamp(event.eventTimestamp)}</td>
                      <td>{event.sensorValue}</td>
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
        </section>
      </main>

      <footer className="app-footer">
        <p>© 2025 VibraGuard AI Security System | 95.1% AI Accuracy</p>
      </footer>
      
      {/* Toast Container */}
      <ToastContainer />
    </div>
  )
}

export default App
