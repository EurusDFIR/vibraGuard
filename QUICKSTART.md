# VibraGuard - Quick Start Guide

## 🚀 Hướng dẫn khởi chạy nhanh

### Bước 1: Khởi động Infrastructure (30 giây)

```bash
cd vibraGuard
docker-compose up -d
docker ps  # Kiểm tra 2 containers đang chạy
```

### Bước 2: Thêm dữ liệu mẫu (10 giây)

```bash
docker exec -i vibraguard-postgres-1 psql -U myuser -d vibraguard_db < backend/src/main/resources/init-data.sql
```

### Bước 3: Chạy Backend (30 giây)

```bash
cd backend
mvn spring-boot:run
```

Đợi đến khi thấy log:

```
✓ Connected to MQTT Broker
✓ Subscribed to topic: vibra_guard/sensor
```

### Bước 4: Cấu hình ESP32 (2 phút)

#### 4.1. Cài thư viện Arduino

- Mở Arduino IDE
- Tools > Manage Libraries
- Tìm và cài: **PubSubClient** by Nick O'Leary

#### 4.2. Cấu hình WiFi và MQTT

Mở file `esp32/vibraguard_mqtt_integration.ino` và sửa:

```cpp
const char* ssid = "TenWiFi";          // ← Sửa đây
const char* password = "MatKhauWiFi";   // ← Sửa đây
const char* mqtt_server = "192.168.1.100"; // ← IP máy tính (dùng ipconfig)
```

#### 4.3. Upload

- Tools > Board > ESP32 Dev Module
- Tools > Port > (Chọn COM port)
- Sketch > Upload
- Tools > Serial Monitor (115200 baud)

### Bước 5: Kiểm thử (1 phút)

✅ **Trên Serial Monitor ESP32:**

```
✅ WiFi Connected!
✅ Connected to MQTT Broker!
System Ready - Monitoring vibration...
```

✅ **Rung cảm biến:**

- Còi kêu 500ms
- Serial Monitor: `⚠️ VIBRATION DETECTED!`
- Serial Monitor: `📤 MQTT Message Sent`

✅ **Trên Backend logs:**

```
📨 Received MQTT message: {...}
✅ Vibration event saved: ID=1
```

✅ **Check API:**

```bash
curl http://localhost:8080/api/events
```

Hoặc mở browser: `http://localhost:8080/api/events`

## 📊 Checklist Kiểm tra

- [ ] Docker containers đang chạy (mosquitto + postgres)
- [ ] Database có dữ liệu thiết bị (3 devices)
- [ ] Backend khởi động thành công
- [ ] Backend kết nối MQTT thành công
- [ ] ESP32 kết nối WiFi
- [ ] ESP32 kết nối MQTT Broker
- [ ] Rung cảm biến → Còi kêu
- [ ] MQTT message được gửi từ ESP32
- [ ] Backend nhận message và lưu DB
- [ ] API trả về dữ liệu sự kiện

## 🔧 Troubleshooting

### ESP32 không kết nối WiFi

```cpp
// Kiểm tra:
1. SSID và password có đúng không?
2. ESP32 trong vùng phủ sóng?
3. Thử reset ESP32 (nút RST)
```

### ESP32 không kết nối MQTT

```cpp
// Kiểm tra:
1. IP máy tính đúng chưa? (dùng ipconfig)
2. Firewall chặn port 1883? (tắt thử)
3. Mosquitto có chạy? (docker ps)
```

### Backend không nhận MQTT

```
// Kiểm tra:
1. Backend logs có lỗi?
2. Restart backend
3. Check topic: ESP32 publish "vibra_guard/sensor"
```

### Không có dữ liệu trong DB

```bash
# Chạy lại script thêm device
docker exec -i vibraguard-postgres-1 psql -U myuser -d vibraguard_db < backend/src/main/resources/init-data.sql

# Hoặc kiểm tra device_id có khớp:
# ESP32: "ESP32_CUA_SO_01"
# DB: SELECT * FROM devices;
```

## 📁 Cấu trúc file quan trọng

```
vibraGuard/
├── docker-compose.yml              ← Infrastructure
├── STAGE3_TESTING.md              ← Hướng dẫn chi tiết
├── esp32/
│   ├── vibraguard_hardware_core.ino    ← Giai đoạn 1 (chỉ còi)
│   └── vibraguard_mqtt_integration.ino ← Giai đoạn 3 (MQTT) ⭐
└── backend/
    ├── pom.xml                         ← Dependencies
    ├── src/main/resources/
    │   ├── application.properties      ← Config MQTT
    │   └── init-data.sql              ← Dữ liệu mẫu
    └── src/main/java/com/vibraguard/
        ├── config/MqttConfig.java          ← MQTT setup ⭐
        └── service/MqttSubscriberService.java ← Xử lý MQTT ⭐
```

## 🎯 Tóm tắt Flow

```
1. Rung cảm biến SW-420
2. ESP32 phát hiện (GPIO 13 = HIGH)
3. ESP32 kích hoạt còi (GPIO 12 = HIGH)
4. ESP32 gửi JSON qua MQTT
5. Mosquitto Broker chuyển tiếp
6. Backend nhận message
7. Backend parse JSON
8. Backend lưu vào PostgreSQL
9. API trả về dữ liệu
```

## ⏭️ Bước tiếp theo

Sau khi hoàn thành Giai đoạn 3, tiếp tục:

- **Giai đoạn 4:** WebSocket + React Frontend (Real-time alerts)
- **Giai đoạn 5:** Remote Control (Tắt còi từ Web/App)

---

**Thời gian hoàn thành:** ~5-10 phút
**Yêu cầu:** Docker, Java 17, Maven, Arduino IDE, ESP32, WiFi
