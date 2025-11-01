# 🚀 HƯỚNG DẪN CHẠY HỆ THỐNG (SAU KHI DEPLOY GCP)

## 📍 THÔNG TIN SERVER

- **IP Server:** `34.87.133.103`
- **Zone:** `asia-southeast1-b`
- **Backend API:** `http://34.87.133.103:8080`
- **MQTT Broker:** `tcp://34.87.133.103:1883`

---

## ⚡ KHỞI ĐỘNG NHANH

### 🔥 Server GCP đang chạy rồi!

Server GCP đã được cấu hình tự động khởi động. **KHÔNG CẦN KHỞI ĐỘNG LẠI.**

Chỉ cần chạy **Frontend Web** và **Mobile App** ở local:

```bash
# 1. Frontend Web (React)
cd frontend
npm run dev
# → http://localhost:5173

# 2. Mobile App (Flutter)
cd vibraguard_app
flutter run -d emulator-5554
```

---

## 🔧 QUẢN LÝ SERVER GCP

### Kiểm tra trạng thái Services

```bash
gcloud compute ssh vibraguard-server --zone=asia-southeast1-b \
  --command="cd vibraguard-prod && docker-compose ps"
```

### Xem Logs Backend

```bash
gcloud compute ssh vibraguard-server --zone=asia-southeast1-b \
  --command="cd vibraguard-prod && docker-compose logs -f backend"
```

### Restart Services (Nếu cần)

```bash
gcloud compute ssh vibraguard-server --zone=asia-southeast1-b \
  --command="cd vibraguard-prod && docker-compose restart"
```

### Stop Services

```bash
gcloud compute ssh vibraguard-server --zone=asia-southeast1-b \
  --command="cd vibraguard-prod && docker-compose down"
```

### Start Services

```bash
gcloud compute ssh vibraguard-server --zone=asia-southeast1-b \
  --command="cd vibraguard-prod && docker-compose up -d"
```

---

## 🌐 CẤU HÌNH ĐÃ THAY ĐỔI

### ✅ Frontend Web

**File:** `frontend/src/App.jsx` (dòng 14)

```javascript
const BACKEND_URL = "http://34.87.133.103:8080"; // ← IP GCP
```

### ✅ Mobile App

**File:** `vibraguard_app/lib/main.dart` (dòng 28)

```dart
static const String backendUrl = 'http://34.87.133.103:8080';  // ← IP GCP
```

### ⚠️ ESP32 (CẦN SỬA)

**File:** `esp32/vibraguard_ai_final.ino`

```cpp
const char *MQTT_SERVER = "34.87.133.103";  // ← ĐỔI IP GCP
const char *WIFI_SSID = "YOUR_WIFI";        // ← ĐỔI WIFI
const char *WIFI_PASSWORD = "YOUR_PASS";     // ← ĐỔI PASS
```

**Sau đó:** Nạp lại code vào ESP32

---

## 🧪 KIỂM TRA HỆ THỐNG

### Test Backend API

```bash
curl http://34.87.133.103:8080/api/control/status
# Kết quả: {"devicesCount":0,"systemStatus":"ARMED",...}
```

### Test MQTT

```bash
# Subscribe (Lắng nghe)
mosquitto_sub -h 34.87.133.103 -t vibra_guard/#

# Publish (Gửi thử)
mosquitto_pub -h 34.87.133.103 -t vibra_guard/sensor -m "test"
```

### Test Frontend

Mở browser: `http://localhost:5173`

### Test Mobile

Chạy app trên emulator hoặc thiết bị thật

---

## 📊 WORKFLOW DEMO

### 🎯 Demo ở Trường/Nơi khác

1. **Bật Mobile Hotspot** trên điện thoại
2. **Sửa ESP32 code:** WiFi SSID/Password → Hotspot của bạn
3. **Nạp lại** code vào ESP32
4. **Chạy Frontend & Mobile** ở local (kết nối WiFi bình thường)
5. ESP32 sẽ gửi data lên GCP Server
6. Frontend/Mobile sẽ nhận data từ GCP Server

### 🏠 Demo ở Nhà

Tương tự, nhưng dùng WiFi nhà.

---

## 💰 QUẢN LÝ CHI PHÍ

### Tắt VM khi không dùng (Tiết kiệm tiền)

```bash
gcloud compute instances stop vibraguard-server --zone=asia-southeast1-b
```

### Bật lại VM

```bash
gcloud compute instances start vibraguard-server --zone=asia-southeast1-b
```

### Xem chi phí

https://console.cloud.google.com/billing

---

## 🔄 CẬP NHẬT CODE MỚI

### Nếu sửa Backend

```bash
# 1. Build lại JAR
cd backend
mvn clean package

# 2. Upload lên GCP
gcloud compute scp backend/target/vibraguard-backend-0.0.1-SNAPSHOT.jar \
  vibraguard-server:vibraguard-prod/app.jar --zone=asia-southeast1-b

# 3. Restart Backend
gcloud compute ssh vibraguard-server --zone=asia-southeast1-b \
  --command="cd vibraguard-prod && docker-compose restart backend"
```

### Nếu sửa Frontend/Mobile

Chỉ cần chạy lại ở local, không cần làm gì thêm.

---

## ❌ LỖI THƯỜNG GẶP

**Frontend/Mobile không connect được:**

- Kiểm tra IP: `34.87.133.103:8080`
- Kiểm tra VM đang chạy: `gcloud compute instances list`
- Kiểm tra firewall: Port 8080 và 1883 đã mở chưa

**ESP32 không connect:**

- Kiểm tra WiFi SSID/Password
- Kiểm tra IP MQTT Server: `34.87.133.103`
- Kiểm tra ESP32 có kết nối WiFi thành công không

**VM tốn tiền:**

- Stop VM khi không dùng
- Hoặc xóa VM: `gcloud compute instances delete vibraguard-server --zone=asia-southeast1-b`

---

## 📞 API ENDPOINTS

```
GET  http://34.87.133.103:8080/api/control/status
GET  http://34.87.133.103:8080/api/events
POST http://34.87.133.103:8080/api/control/arm
POST http://34.87.133.103:8080/api/control/disarm
POST http://34.87.133.103:8080/api/control/sound_alarm
WS   ws://34.87.133.103:8080/ws
```

---

## ✅ TÓM TẮT

**Lần sau chỉ cần:**

1. ✅ Mở Frontend: `cd frontend && npm run dev`
2. ✅ Mở Mobile: `cd vibraguard_app && flutter run`
3. ✅ Nạp ESP32 với WiFi/IP mới (nếu cần)
4. ✅ Server GCP đã sẵn sàng!

**Không cần:** Khởi động Backend, MQTT, Database (đã chạy trên GCP)
