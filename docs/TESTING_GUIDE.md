# 🧪 VibraGuard - Test Script

## Test môi trường và kết nối

### 1. Test Docker Infrastructure

```bash
# Kiểm tra containers đang chạy
docker ps

# Expected output:
# - vibraguard-mosquitto-1 (port 1883)
# - vibraguard-postgres-1 (port 5432)

# Test Mosquitto MQTT
mosquitto_sub -h localhost -t "vibra_guard/#" -v

# Test PostgreSQL
docker exec -it vibraguard-postgres-1 psql -U myuser -d vibraguard_db -c "\dt"
```

### 2. Test Backend APIs

```bash
# Test status endpoint
curl http://localhost:8080/api/control/status

# Test events endpoint
curl http://localhost:8080/api/events

# Test ARM command
curl -X POST http://localhost:8080/api/control/arm

# Test DISARM command
curl -X POST http://localhost:8080/api/control/disarm
```

### 3. Test MQTT Publishing (Simulate ESP32)

```bash
# Publish fake attack alert
mosquitto_pub -h localhost -t "vibra_guard/sensor" -m '{
  "deviceId": "ESP32_CUA_SO_01",
  "timestamp": 1234567890,
  "ai_triggered": true,
  "confidence": 0.95,
  "scores": {
    "attack": 0.95,
    "normal": 0.03,
    "noise": 0.02
  }
}'

# Check backend logs - should see:
# "📨 Received MQTT message"
# "✅ Vibration event saved"
# "📡 WebSocket alert sent to /topic/alerts"

# Check database
docker exec -it vibraguard-postgres-1 psql -U myuser -d vibraguard_db -c "SELECT * FROM vibration_events ORDER BY id DESC LIMIT 5;"
```

### 4. Test WebSocket Connection

```bash
# Install wscat (WebSocket client)
npm install -g wscat

# Connect to WebSocket (manual test)
wscat -c ws://localhost:8080/ws

# After connection, subscribe to /topic/alerts
# (This is complex with STOMP protocol, better use Web UI)
```

**Better**: Open http://localhost:5173 and check "🟢 Connected" status

### 5. Test React Web App

```bash
# Start React dev server
cd frontend
npm run dev

# Open browser: http://localhost:5173
# Verify:
# - Status badges: "🟢 Connected" + "🔔 ARMED"
# - Control buttons work
# - History table displays events

# Test real-time alert:
# 1. Keep Web UI open
# 2. Publish MQTT (step 3 above)
# 3. Alert should appear instantly (< 1 second)
```

### 6. Test Remote Control

```bash
# Scenario: ESP32 đang hú còi

# Method 1: Via Web UI
# - Open http://localhost:5173
# - Click "🔕 Tắt báo động (DISARM)"
# - ESP32 should stop buzzer

# Method 2: Via curl
curl -X POST http://localhost:8080/api/control/disarm

# Method 3: Via MQTT directly
mosquitto_pub -h localhost -t "vibra_guard/control/ESP32_CUA_SO_01" -m "DISARM"

# Check ESP32 Serial Monitor:
# Should see: "📨 MQTT Message [...]: DISARM"
# Should see: "🔕 DISARM Command Received"
```

---

## 🎯 Complete End-to-End Test

### Prerequisites

1. ✅ Docker containers running
2. ✅ Backend running (`mvn spring-boot:run`)
3. ✅ React Web running (`npm run dev`)
4. ✅ ESP32 uploaded and connected
5. ✅ Serial Monitor open (115200 baud)
6. ✅ Web UI open (http://localhost:5173)

### Test Flow

#### Step 1: Verify Connections

```bash
# Terminal 1: Monitor MQTT
mosquitto_sub -h localhost -t "vibra_guard/#" -v

# Terminal 2: Monitor Backend logs
cd backend && mvn spring-boot:run

# Browser: Check Web UI status
# - Should see: "🟢 Connected"

# ESP32 Serial Monitor:
# - Should see: "✅ WiFi Connected"
# - Should see: "✅ MQTT Connected"
```

#### Step 2: Test Attack Detection

**Action**: Tạo rung động mạnh (máy khoan, búa)

**Expected Results**:

1. **ESP32 Serial Monitor**:

```
🧠 AI: Attack=95.00% | Normal=3.00% | Noise=2.00% | Time=1ms

🚨🚨🚨 ATTACK DETECTED! 🚨🚨🚨
   Confidence: 95.0%
   Attack > Normal by 92.0%

📤 MQTT Alert Sent:
   {"deviceId":"ESP32_CUA_SO_01","timestamp":...}
```

2. **ESP32 Hardware**: Còi HÚ LIÊN TỤC

3. **Terminal 1 (MQTT Monitor)**:

```
vibra_guard/sensor {"deviceId":"ESP32_CUA_SO_01",...}
```

4. **Terminal 2 (Backend Logs)**:

```
📨 Received MQTT message: {...}
✅ Vibration event saved: ID=1, Device=ESP32_CUA_SO_01
📡 WebSocket alert sent to /topic/alerts: deviceId=ESP32_CUA_SO_01
```

5. **Web UI (http://localhost:5173)**:

- Alert mới xuất hiện NGAY LẬP TỨC (< 1 giây)
- Card màu đỏ với thông tin:
  - Device: ESP32_CUA_SO_01
  - Severity: WARNING
  - AI Confidence: 95.0%
  - Timestamp: (current time)

6. **Browser Notification**: Popup "🚨 VibraGuard Alert!"

7. **Database**:

```bash
docker exec -it vibraguard-postgres-1 psql -U myuser -d vibraguard_db -c "SELECT * FROM vibration_events ORDER BY id DESC LIMIT 1;"

# Should see new row with device_id = ESP32_CUA_SO_01
```

**✅ PASS if ALL 7 conditions met**

#### Step 3: Test Remote DISARM

**Action**: Bấm nút "🔕 Tắt báo động" trên Web UI

**Expected Results**:

1. **Web UI**: Snackbar "✅ Báo động đã được tắt!"

2. **Web UI Status**: Badge đổi thành "🔕 DISARMED"

3. **Backend Logs**:

```
🔕 DISARM command received
📤 MQTT command sent: topic=vibra_guard/control/ESP32_CUA_SO_01, command=DISARM
```

4. **Terminal 1 (MQTT Monitor)**:

```
vibra_guard/control/ESP32_CUA_SO_01 DISARM
```

5. **ESP32 Serial Monitor**:

```
📨 MQTT Message [vibra_guard/control/ESP32_CUA_SO_01]: DISARM
🔕 DISARM Command Received
   Alarm deactivated!
```

6. **ESP32 Hardware**: Còi TẮT NGAY LẬP TỨC

**✅ PASS if ALL 6 conditions met**

#### Step 4: Test Normal Activity (AI Filtering)

**Action**: Gõ nhẹ, đóng cửa bình thường

**Expected Results**:

1. **ESP32 Serial Monitor**:

```
🧠 AI: Attack=5.00% | Normal=92.00% | Noise=3.00% | Time=1ms
(Không có "ATTACK DETECTED" message)
```

2. **ESP32 Hardware**: Còi IM LẶNG

3. **Web UI**: KHÔNG có alert mới

4. **Backend Logs**: KHÔNG có "Vibration event saved"

**✅ PASS if AI correctly filters normal activity**

#### Step 5: Test Event History

**Action**: Bấm nút "🔄 Làm mới" trên Web UI

**Expected Results**:

1. **Web UI**: Bảng "📜 Lịch sử Sự kiện" refresh

2. **Table Content**: Hiển thị tất cả attacks từ database

3. **Compare with Database**:

```bash
docker exec -it vibraguard-postgres-1 psql -U myuser -d vibraguard_db -c "SELECT COUNT(*) FROM vibration_events;"
```

**✅ PASS if Web UI count matches database count**

---

## 📊 Test Checklist

### Infrastructure Tests

- [ ] Docker containers running
- [ ] Mosquitto accepting connections (port 1883)
- [ ] PostgreSQL accepting connections (port 5432)
- [ ] Database tables created

### Backend Tests

- [ ] Spring Boot starts successfully
- [ ] MQTT subscriber connected
- [ ] WebSocket endpoint available
- [ ] REST API endpoints responding
- [ ] MQTT publisher can send commands

### Frontend Tests

- [ ] React dev server running
- [ ] WebSocket connection established
- [ ] Can fetch event history
- [ ] Can send ARM command
- [ ] Can send DISARM command
- [ ] Real-time alerts display
- [ ] Browser notifications work

### ESP32 Tests

- [ ] WiFi connection successful
- [ ] MQTT connection successful
- [ ] MPU6050 sensor working
- [ ] AI inference running (1ms)
- [ ] Buzzer control working
- [ ] Can receive control commands
- [ ] Can publish sensor data

### Integration Tests

- [ ] End-to-end attack detection (< 100ms)
- [ ] Remote DISARM works
- [ ] AI filtering works (normal activity)
- [ ] Event history accurate
- [ ] WebSocket real-time updates
- [ ] Database persistence

---

## 🐛 Common Issues & Solutions

### Issue 1: WebSocket "🔴 Disconnected"

**Solution**:

```bash
# Check backend running
curl http://localhost:8080/api/control/status

# Check browser console (F12)
# Look for WebSocket errors

# Try hard refresh
Ctrl + Shift + R
```

### Issue 2: MQTT Connection Failed (ESP32)

**Solution**:

```bash
# Check Mosquitto running
docker ps | grep mosquitto

# Test MQTT from PC
mosquitto_sub -h localhost -t "test" -v

# Check ESP32 IP in same network
ping <ESP32_IP>

# Update MQTT_SERVER IP in Arduino code
```

### Issue 3: No Real-time Alerts

**Solution**:

```bash
# Check backend logs for:
# "📡 WebSocket alert sent"

# Check Web UI console for:
# "🚨 Alert received"

# Test manually:
mosquitto_pub -h localhost -t "vibra_guard/sensor" -m '{"deviceId":"ESP32_CUA_SO_01","timestamp":123,"ai_triggered":true,"confidence":0.95}'
```

### Issue 4: AI Not Detecting Attack

**Solution**:

```bash
# Check MPU6050 wiring
# - SDA → GP8
# - SCL → GP9
# - VCC → 3.3V
# - GND → GND

# Check Serial Monitor for "🧠 AI:" outputs

# Increase vibration intensity (use drill)

# Check threshold in code:
# const float ATTACK_THRESHOLD = 0.80;
```

---

## 📈 Performance Benchmarks

Run these tests to measure system performance:

### Latency Test

```bash
# Method: Measure time from ESP32 publish to Web UI display

# 1. Sync clocks
date

# 2. Trigger attack and note ESP32 timestamp
# 3. Note Web UI alert timestamp
# 4. Calculate difference

# Expected: < 100ms
```

### Throughput Test

```bash
# Method: Rapid-fire MQTT publishes

for i in {1..100}; do
  mosquitto_pub -h localhost -t "vibra_guard/sensor" -m "{\"deviceId\":\"TEST\",\"timestamp\":$i}"
  sleep 0.1
done

# Check:
# - All 100 events saved to database
# - Backend handles load without crashes
# - Web UI displays all alerts
```

### Stress Test

```bash
# Method: Multiple concurrent connections

# Terminal 1: Backend
cd backend && mvn spring-boot:run

# Terminal 2-5: Multiple MQTT publishers
for i in {1..4}; do
  (while true; do
    mosquitto_pub -h localhost -t "vibra_guard/sensor" -m "{\"deviceId\":\"ESP$i\",\"timestamp\":$(date +%s)}"
    sleep 1
  done) &
done

# Monitor:
# - Backend memory usage
# - Database connection pool
# - WebSocket connection stability
```

---

## ✅ Test Sign-off

**System Ready for Production when**:

- [x] All Infrastructure Tests pass
- [x] All Backend Tests pass
- [x] All Frontend Tests pass
- [x] All ESP32 Tests pass
- [x] All Integration Tests pass
- [x] Latency < 100ms
- [x] Throughput > 10 events/sec
- [x] No memory leaks in 1 hour stress test

**Tested by**: ******\_******  
**Date**: ******\_******  
**Version**: 1.0.0  
**Status**: ✅ APPROVED / ❌ NEEDS WORK

---

**Happy Testing! 🧪🎯**
