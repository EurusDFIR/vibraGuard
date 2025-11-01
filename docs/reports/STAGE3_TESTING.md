# Giai đoạn 3: Tích hợp MQTT - Hướng dẫn

## Mục tiêu

Kết nối ESP32 → MQTT Broker → Backend → Database

## Chuẩn bị

### 1. Kiểm tra Infrastructure

Đảm bảo Mosquitto và PostgreSQL đang chạy:

```bash
docker ps
```

Phải thấy 2 containers: `vibraguard-mosquitto-1` và `vibraguard-postgres-1`

### 2. Thêm dữ liệu thiết bị vào Database

```bash
docker exec -i vibraguard-postgres-1 psql -U myuser -d vibraguard_db < backend/src/main/resources/init-data.sql
```

Hoặc dùng DBeaver/pgAdmin để chạy script `init-data.sql`

### 3. Cài đặt thư viện Arduino

Trong Arduino IDE:

- Tools > Manage Libraries
- Tìm và cài đặt: **PubSubClient** by Nick O'Leary

## Cấu hình ESP32

### 1. Cấu hình WiFi và MQTT

Mở file `vibraguard_mqtt_integration.ino` và sửa các thông tin sau:

```cpp
// WiFi
const char* ssid = "YOUR_WIFI_SSID";        // Tên WiFi
const char* password = "YOUR_WIFI_PASSWORD"; // Mật khẩu WiFi

// MQTT
const char* mqtt_server = "192.168.1.100";   // IP máy tính chạy Mosquitto
const char* device_id = "ESP32_CUA_SO_01";   // Phải khớp với device_id trong DB
```

### 2. Tìm IP máy tính

**Windows:**

```bash
ipconfig
```

Tìm dòng "IPv4 Address" (ví dụ: 192.168.1.100)

**Mac/Linux:**

```bash
ifconfig
```

### 3. Upload code lên ESP32

1. Chọn Board: **ESP32 Dev Module**
2. Chọn COM Port
3. Click Upload
4. Mở Serial Monitor (115200 baud)

## Khởi chạy Backend

### 1. Build backend (nếu chưa build)

```bash
cd backend
mvn clean install -DskipTests
```

### 2. Chạy Spring Boot

```bash
mvn spring-boot:run
```

Hoặc:

```bash
java -jar target/vibraguard-backend-0.0.1-SNAPSHOT.jar
```

Backend sẽ chạy tại `http://localhost:8080`

## Kiểm thử End-to-End

### Bước 1: Kiểm tra Backend logs

Sau khi khởi động, bạn sẽ thấy:

```
✓ Connected to MQTT Broker
✓ Subscribed to topic: vibra_guard/sensor
```

### Bước 2: Kiểm tra ESP32 Serial Monitor

Sau khi upload, bạn sẽ thấy:

```
✅ WiFi Connected!
📡 IP Address: 192.168.x.x
✅ Connected to MQTT Broker!
📥 Subscribed to: vibra_guard/control/ESP32_CUA_SO_01
System Ready - Monitoring vibration...
```

### Bước 3: Tạo rung động

Rung hoặc gõ nhẹ cảm biến SW-420

**Kết quả mong đợi:**

**Trên ESP32 Serial Monitor:**

```
⚠️  VIBRATION DETECTED!
📤 MQTT Message Sent:
   Topic: vibra_guard/sensor
   Payload: {"deviceId":"ESP32_CUA_SO_01","timestamp":12345,"sensorValue":1}
```

**Trên Backend logs:**

```
📨 Received MQTT message: {"deviceId":"ESP32_CUA_SO_01","timestamp":12345,"sensorValue":1}
📊 Parsed sensor data: deviceId=ESP32_CUA_SO_01, timestamp=12345
✅ Vibration event saved: ID=1, Device=ESP32_CUA_SO_01, Time=2025-10-26T13:45:30
```

**Còi Buzzer:** Phải kêu trong 500ms

### Bước 4: Kiểm tra Database

**Cách 1: Dùng REST API**

```bash
curl http://localhost:8080/api/events
```

**Cách 2: Dùng Postman**

- GET `http://localhost:8080/api/events`

**Cách 3: Dùng DBeaver/pgAdmin**

```sql
SELECT * FROM vibration_events ORDER BY event_timestamp DESC LIMIT 10;
```

**Kết quả:** Phải thấy sự kiện mới với deviceId = "ESP32_CUA_SO_01"

## Kiểm tra bằng MQTT Explorer (Optional)

1. Tải và cài đặt [MQTT Explorer](http://mqtt-explorer.com/)
2. Kết nối:
   - Host: `localhost`
   - Port: `1883`
3. Subscribe topic: `vibra_guard/sensor`
4. Rung cảm biến → Sẽ thấy message xuất hiện trong MQTT Explorer

## Troubleshooting

### ESP32 không kết nối được WiFi

- Kiểm tra lại SSID và Password
- Đảm bảo ESP32 trong vùng phủ sóng WiFi
- Thử reset ESP32 (nút RST)

### ESP32 không kết nối được MQTT Broker

- Kiểm tra IP máy tính có đúng không
- Đảm bảo firewall không chặn port 1883
- Windows: Tắt firewall tạm thời để test
- Kiểm tra Mosquitto có đang chạy: `docker ps`

### Backend không nhận được MQTT message

- Kiểm tra Backend logs có lỗi không
- Kiểm tra topic có khớp không:
  - ESP32 publish: `vibra_guard/sensor`
  - Backend subscribe: `vibra_guard/sensor`
- Restart Backend để reconnect MQTT

### Không thấy dữ liệu trong Database

- Kiểm tra device_id trong ESP32 có khớp với DB không
- Chạy lại script `init-data.sql` để thêm thiết bị
- Kiểm tra Backend logs có lỗi database không

### Lỗi "Device not found"

Backend sẽ tự động tạo device mới nếu chưa có trong DB.
Nhưng tốt nhất là thêm device trước bằng script `init-data.sql`

## Flow hoàn chỉnh

```
┌─────────┐      ┌──────────┐      ┌─────────┐      ┌──────────┐
│  ESP32  │─────▶│ Mosquitto│─────▶│ Backend │─────▶│ Database │
│ +Sensor │MQTT  │  Broker  │MQTT  │ Spring  │ JPA  │PostgreSQL│
└─────────┘      └──────────┘      └─────────┘      └──────────┘
    │                                     │
    └─── Buzzer (còi kêu)                 │
                                          ▼
                                   [REST API]
                                   GET /api/events
```

## Bước tiếp theo

Sau khi Giai đoạn 3 hoạt động tốt, chuyển sang:

- **Giai đoạn 4:** Tạo WebSocket để đẩy cảnh báo real-time đến Frontend
- **Giai đoạn 5:** Thêm tính năng điều khiển từ xa (tắt còi qua Web/App)
