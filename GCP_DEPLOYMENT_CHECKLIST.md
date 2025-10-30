# ✅ BÁO CÁO KIỂM TRA HỆ THỐNG - GCP DEPLOYMENT

**Ngày kiểm tra:** 31/10/2025  
**Server GCP IP:** `34.87.133.103`  
**Project:** zero-dawn-473705  
**Zone:** asia-southeast1-b

---

## 📊 TỔNG QUAN HỆ THỐNG

### ☁️ Server GCP

```
✅ VM Instance: vibraguard-server (e2-small)
✅ IP Tĩnh: 34.87.133.103
✅ Services đang chạy:
   - MQTT Broker (Mosquitto): Port 1883
   - PostgreSQL Database: Port 5432
   - Spring Boot Backend: Port 8080
```

### 🔍 KIỂM TRA CẤU HÌNH

#### ✅ 1. Frontend Web (React)

**File:** `frontend/src/App.jsx` (dòng 14)

```javascript
const BACKEND_URL = 'http://34.87.133.103:8080' ✅ ĐÚNG
const WS_URL = `${BACKEND_URL}/ws` ✅ ĐÚNG
```

**Trạng thái:** ✅ **ĐÃ CẬP NHẬT IP GCP**

---

#### ✅ 2. Mobile App (Flutter)

**File:** `vibraguard_app/lib/main.dart` (dòng 38)

```dart
static const String backendUrl = 'http://34.87.133.103:8080'; ✅ ĐÚNG
```

**Trạng thái:** ✅ **ĐÃ CẬP NHẬT IP GCP**

---

#### ✅ 3. ESP32 (Arduino)

**File:** `esp32/vibraguard_ai_final/vibraguard_ai_final.ino` (dòng 43)

```cpp
const char *MQTT_SERVER = "34.87.133.103"; ✅ ĐÚNG
const int MQTT_PORT = 1883; ✅ ĐÚNG
const char *WIFI_SSID = "LE HUNG"; ⚠️ CẦN ĐỔI KHI DEMO
const char *WIFI_PASSWORD = "123456789"; ⚠️ CẦN ĐỔI KHI DEMO
```

**Trạng thái:** ✅ **ĐÃ CẬP NHẬT IP GCP**  
**Lưu ý:** ⚠️ Cần đổi WiFi SSID/Password khi demo ở nơi khác

---

#### ✅ 4. Backend Config (application.properties)

**File:** `backend/src/main/resources/application.properties`

```properties
# Config LOCAL (không ảnh hưởng GCP)
spring.datasource.url=jdbc:postgresql://localhost:5432/vibraguard_db
mqtt.broker.url=tcp://localhost:1883
```

**Trạng thái:** ✅ **OK** (Local config, không ảnh hưởng GCP)

**GCP Config** (được override bởi Docker environment):

```yaml
# File: docker-compose.yml trên GCP
SPRING_DATASOURCE_URL=jdbc:postgresql://postgres:5432/vibraguard_db ✅
MQTT_BROKER_URL=tcp://mosquitto:1883 ✅
```

---

## 🎯 KẾT QUẢ KIỂM TRA

| Component        | IP/URL                      | Status     | Ghi chú            |
| ---------------- | --------------------------- | ---------- | ------------------ |
| **GCP Backend**  | `34.87.133.103:8080`        | ✅ Running | API tested OK      |
| **GCP MQTT**     | `34.87.133.103:1883`        | ✅ Running | Port open          |
| **GCP Database** | `postgres:5432` (internal)  | ✅ Running | In Docker network  |
| **Frontend**     | `http://34.87.133.103:8080` | ✅ Updated | URL đã đổi         |
| **Mobile**       | `http://34.87.133.103:8080` | ✅ Updated | URL đã đổi         |
| **ESP32**        | `34.87.133.103:1883`        | ✅ Updated | MQTT Server đã đổi |

---

## 🧪 TEST HOẠT ĐỘNG

### ✅ Test Backend API

```bash
$ curl http://34.87.133.103:8080/api/control/status
{"devicesCount":0,"systemStatus":"ARMED","timestamp":1761823974712}
```

**Kết quả:** ✅ **THÀNH CÔNG**

### ✅ Test Container Status

```bash
$ docker-compose ps
NAME                    STATUS    PORTS
vibraguard-backend      Up        0.0.0.0:8080->8080/tcp
vibraguard-mosquitto    Up        0.0.0.0:1883->1883/tcp
vibraguard-postgres     Up        0.0.0.0:5432->5432/tcp
```

**Kết quả:** ✅ **TẤT CẢ SERVICES ĐANG CHẠY**

---

## 📝 CHECKLIST TRƯỚC KHI DEMO

### Frontend Web

- [ ] Đã chạy `npm run dev`
- [ ] Mở browser tại `http://localhost:5173`
- [ ] Test kết nối Backend: Check status badge "Connected"

### Mobile App

- [ ] Đã chạy `flutter run -d emulator-5554`
- [ ] Test kết nối Backend: Check connection badge
- [ ] Test ARM/DISARM buttons
- [ ] Test Panic button

### ESP32

- [ ] **Đổi WiFi SSID/Password** nếu demo ở nơi khác
- [ ] Nạp lại code sau khi đổi WiFi
- [ ] Kiểm tra Serial Monitor: ESP32 kết nối WiFi thành công
- [ ] Kiểm tra Serial Monitor: ESP32 kết nối MQTT thành công
- [ ] Test gửi data: Rung thiết bị, xem log MQTT

### GCP Server

- [ ] Kiểm tra VM đang chạy: `gcloud compute instances list`
- [ ] Kiểm tra services: `docker-compose ps`
- [ ] Xem logs nếu cần: `docker-compose logs -f backend`

---

## ⚠️ LƯU Ý QUAN TRỌNG

### 1. WiFi ESP32

```cpp
// File: esp32/vibraguard_ai_final/vibraguard_ai_final.ino

// Demo ở nhà:
const char *WIFI_SSID = "LE HUNG";
const char *WIFI_PASSWORD = "123456789";

// Demo ở trường/nơi khác:
// const char *WIFI_SSID = "TEN_WIFI_MOI";
// const char *WIFI_PASSWORD = "PASS_WIFI_MOI";
```

**⚠️ Nhớ nạp lại code sau khi đổi!**

### 2. Mobile Hotspot (Nếu dùng)

Nếu dùng hotspot điện thoại:

1. Bật hotspot trên điện thoại
2. Đổi ESP32 WiFi thành tên hotspot
3. Nạp lại code
4. Frontend/Mobile vẫn dùng WiFi bình thường (không cần hotspot)

### 3. Chi phí GCP

- VM e2-small: ~$13-15/tháng nếu chạy 24/7
- **Tiết kiệm:** Stop VM khi không dùng

```bash
gcloud compute instances stop vibraguard-server --zone=asia-southeast1-b
```

---

## 🔄 WORKFLOW HOÀN CHỈNH

### Khởi động Demo

```bash
# 1. Kiểm tra GCP Server (nếu cần)
gcloud compute instances list

# 2. Chạy Frontend
cd frontend
npm run dev
# → http://localhost:5173

# 3. Chạy Mobile
cd vibraguard_app
flutter run -d emulator-5554

# 4. Nạp ESP32 (nếu đổi WiFi)
# Open Arduino IDE → Upload code
```

### Trong quá trình Demo

1. **Frontend/Mobile:** Hiển thị dashboard, events
2. **ESP32:** Rung thiết bị → AI phát hiện attack
3. **MQTT:** ESP32 gửi alert lên GCP
4. **Backend:** Lưu event vào DB, broadcast WebSocket
5. **Frontend/Mobile:** Hiển thị alert real-time
6. **Test Control:** ARM/DISARM/Panic buttons

---

## 🎉 TÓM TẮT

### ✅ HOÀN THÀNH

- [x] Deploy Backend, MQTT, Database lên GCP
- [x] Cấu hình IP tĩnh: `34.87.133.103`
- [x] Cấu hình Firewall: Port 1883, 8080
- [x] Cập nhật Frontend Web → GCP IP
- [x] Cập nhật Mobile App → GCP IP
- [x] Cập nhật ESP32 → GCP IP
- [x] Test API thành công
- [x] Tất cả services đang chạy

### 📌 NEXT STEPS

1. ✅ Hệ thống sẵn sàng demo
2. ⚠️ Nhớ đổi WiFi ESP32 khi demo ở nơi khác
3. 💡 Có thể stop VM để tiết kiệm chi phí khi không dùng

---

## 📞 LIÊN HỆ KHI GẶP VẤN ĐỀ

### Backend không hoạt động

```bash
# Xem logs
gcloud compute ssh vibraguard-server --zone=asia-southeast1-b \
  --command="cd vibraguard-prod && docker-compose logs backend"

# Restart
gcloud compute ssh vibraguard-server --zone=asia-southeast1-b \
  --command="cd vibraguard-prod && docker-compose restart backend"
```

### Frontend/Mobile không connect

- Kiểm tra IP: `34.87.133.103:8080`
- Test API: `curl http://34.87.133.103:8080/api/control/status`
- Kiểm tra VM đang chạy: `gcloud compute instances list`

### ESP32 không connect

- Kiểm tra WiFi SSID/Password
- Kiểm tra IP MQTT: `34.87.133.103`
- Xem Serial Monitor để debug

---

## 📊 SO SÁNH LOCAL vs GCP

| Thành phần      | Local (Development)     | GCP (Production)               |
| --------------- | ----------------------- | ------------------------------ |
| **Backend URL** | `http://localhost:8080` | `http://34.87.133.103:8080` ✅ |
| **MQTT Broker** | `tcp://localhost:1883`  | `tcp://34.87.133.103:1883` ✅  |
| **Database**    | localhost:5432          | postgres:5432 (Docker) ✅      |
| **Frontend**    | Local (npm run dev)     | Local (npm run dev) ✅         |
| **Mobile**      | Local (flutter run)     | Local (flutter run) ✅         |
| **ESP32 WiFi**  | WiFi nhà                | WiFi nhà/Hotspot ⚠️            |
| **ESP32 MQTT**  | Local IP                | GCP IP: 34.87.133.103 ✅       |

### ✅ Đã cập nhật cho GCP:

- ✅ Frontend Web: IP GCP
- ✅ Mobile App: IP GCP
- ✅ ESP32: IP GCP
- ✅ Backend: Chạy trên GCP
- ✅ MQTT: Chạy trên GCP
- ✅ Database: Chạy trên GCP

### ⚠️ Cần lưu ý:

- ESP32 cần đổi WiFi SSID/Password khi demo ở nơi khác
- Frontend/Mobile chạy local nhưng connect tới GCP Server

---

**✅ HỆ THỐNG ĐÃ SẴN SÀNG CHO PRODUCTION!**
