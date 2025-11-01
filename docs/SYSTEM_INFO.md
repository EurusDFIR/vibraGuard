# 🔗 THÔNG TIN HỆ THỐNG - VIBRAGUARD

## 🌐 IP & URL

### ☁️ GCP Production Server

```
IP Server:      34.87.133.103
Zone:           asia-southeast1-b
Project:        zero-dawn-473705
Machine Type:   e2-small (2 vCPU, 2GB RAM)
```

### 📡 API Endpoints

```
Backend API:    http://34.87.133.103:8080
WebSocket:      ws://34.87.133.103:8080/ws
MQTT Broker:    tcp://34.87.133.103:1883
```

### 🔑 Credentials

```
Database:
  Host:     postgres (internal Docker network)
  Port:     5432
  User:     myuser
  Password: mypassword
  Database: vibraguard_db

MQTT:
  Port:     1883
  Auth:     None (no authentication)
```

---

## 📱 CẤU HÌNH CLIENT

### Frontend Web

**File:** `frontend/src/App.jsx`

```javascript
const BACKEND_URL = "http://34.87.133.103:8080";
```

**Run:** `npm run dev`  
**Access:** `http://localhost:5173`

### Mobile App

**File:** `vibraguard_app/lib/main.dart`

```dart
static const String backendUrl = 'http://34.87.133.103:8080';
```

**Run:** `flutter run -d emulator-5554`

### ESP32

**File:** `esp32/vibraguard_ai_final/vibraguard_ai_final.ino`

```cpp
const char *MQTT_SERVER = "34.87.133.103";
const char *WIFI_SSID = "LE HUNG";          // ⚠️ Đổi khi demo
const char *WIFI_PASSWORD = "123456789";    // ⚠️ Đổi khi demo
```

---

## 🔧 LỆNH QUẢN LÝ GCP

### Start/Stop VM

```bash
# Start
gcloud compute instances start vibraguard-server --zone=asia-southeast1-b

# Stop (tiết kiệm chi phí)
gcloud compute instances stop vibraguard-server --zone=asia-southeast1-b

# Status
gcloud compute instances list
```

### SSH vào Server

```bash
gcloud compute ssh vibraguard-server --zone=asia-southeast1-b
```

### Docker Commands (trên server)

```bash
# Status
cd ~/vibraguard-prod
docker-compose ps

# Logs
docker-compose logs -f backend
docker-compose logs -f mosquitto

# Restart
docker-compose restart
docker-compose restart backend

# Stop/Start
docker-compose down
docker-compose up -d
```

---

## 🧪 TEST COMMANDS

### Backend API

```bash
# Status
curl http://34.87.133.103:8080/api/control/status

# Events
curl http://34.87.133.103:8080/api/events

# ARM
curl -X POST http://34.87.133.103:8080/api/control/arm

# DISARM
curl -X POST http://34.87.133.103:8080/api/control/disarm
```

### MQTT Test

```bash
# Subscribe (Listen)
mosquitto_sub -h 34.87.133.103 -t vibra_guard/#

# Publish (Send)
mosquitto_pub -h 34.87.133.103 -t vibra_guard/sensor -m "test message"
```

---

## 💰 CHI PHÍ (Ước tính)

```
VM e2-small (Singapore):  ~$15/tháng (24/7)
Storage (10GB):           ~$0.40/tháng
Network Egress:           ~$1-5/tháng (tùy traffic)
Static IP:                ~$3/tháng (khi VM stop)

Tổng:                     ~$20-25/tháng
```

**Tiết kiệm:** Stop VM khi không dùng (chỉ mất phí IP tĩnh ~$3/tháng)

---

## 📂 FILES QUAN TRỌNG

```
GCP_HUONG_DAN_CHAY.md          - Hướng dẫn chạy chi tiết
GCP_DEPLOYMENT_CHECKLIST.md    - Checklist triển khai
KHOI_DONG_NHANH.md              - Khởi động nhanh
guided6.md                      - Hướng dẫn deploy GCP
```

---

## ⚠️ LƯU Ý

1. **IP tĩnh:** `34.87.133.103` sẽ không đổi (đã reserve)
2. **WiFi ESP32:** Phải đổi SSID/Password khi demo ở nơi khác
3. **Firewall:** Port 1883 (MQTT) và 8080 (Backend) đã mở
4. **Auto-start:** Services tự động khởi động khi VM restart

---

**Cập nhật:** 31/10/2025  
**Version:** 1.0 Production
