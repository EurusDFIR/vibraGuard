# VibraGuard - Hệ thống An ninh Chống trộm

## Tổng quan dự án

**VibraGuard** là hệ thống an ninh full-stack phát hiện rung động, kích hoạt còi cảnh báo và gửi thông báo real-time đến Web/Mobile.

### Kiến trúc

- **Device:** ESP32 + Cảm biến SW-420 + Buzzer
- **Broker:** Mosquitto MQTT
- **Database:** PostgreSQL
- **Backend:** Spring Boot (Java 17)
- **Frontend:** React Web + Flutter Mobile (chưa triển khai)

---

## Tiến độ triển khai

### ✅ Giai đoạn 0: Dựng hạ tầng (HOÀN THÀNH)

- [x] Tạo `docker-compose.yml`
- [x] Khởi chạy Mosquitto MQTT Broker (port 1883)
- [x] Khởi chạy PostgreSQL (port 5432)

**Kiểm tra:**

```bash
docker ps
# Phải thấy 2 containers: vibraguard-mosquitto-1 và vibraguard-postgres-1
```

---

### ✅ Giai đoạn 1: Lõi phần cứng (HOÀN THÀNH)

- [x] Tạo code Arduino cho ESP32
- [x] Đọc cảm biến SW-420 (GPIO 13)
- [x] Điều khiển Buzzer qua Transistor (GPIO 12)
- [x] Tạo README hướng dẫn lắp mạch

**File:** `esp32/vibraguard_hardware_core.ino`

**Kiểm tra:**

1. Upload code lên ESP32
2. Rung cảm biến → Còi phải kêu

---

### ✅ Giai đoạn 2: Lõi Backend & DB (HOÀN THÀNH)

- [x] Tạo Spring Boot project với Maven
- [x] Cấu hình kết nối PostgreSQL
- [x] Tạo Entity Models (Device, VibrationEvent)
- [x] Tạo Repositories
- [x] Tạo Service layer
- [x] Tạo REST API Controller
- [x] Tạo script dữ liệu mẫu

**Cấu trúc:**

```
backend/
├── src/main/java/com/vibraguard/
│   ├── VibraGuardApplication.java
│   ├── controller/VibrationEventController.java
│   ├── model/Device.java, VibrationEvent.java
│   ├── repository/DeviceRepository.java, VibrationEventRepository.java
│   ├── service/VibrationEventService.java
│   └── dto/VibrationEventDTO.java
└── pom.xml
```

**Chạy Backend:**

```bash
cd backend
mvn spring-boot:run
```

**API Endpoints:**

- `GET http://localhost:8080/api/events` - Lấy tất cả sự kiện

**Thêm dữ liệu mẫu:**

```bash
docker exec -i vibraguard-postgres-1 psql -U myuser -d vibraguard_db < backend/src/main/resources/init-data.sql
```

---

### ✅ Giai đoạn 3: Tích hợp MQTT (HOÀN THÀNH)

- [x] Thêm dependency `spring-integration-mqtt` vào `pom.xml`
- [x] Cấu hình MQTT trong `application.properties`
- [x] Tạo `MqttConfig` với Spring Integration
- [x] Tạo `MqttSubscriberService` để subscribe topic `vibra_guard/sensor`
- [x] Parse JSON và lưu vào Database
- [x] Tạo ESP32 code với WiFi và MQTT (`vibraguard_mqtt_integration.ino`)
- [x] Kết nối ESP32 với MQTT Broker
- [x] Gửi JSON payload khi phát hiện rung động

**Files:**

- Backend: `config/MqttConfig.java`, `service/MqttSubscriberService.java`
- ESP32: `esp32/vibraguard_mqtt_integration.ino`
- Hướng dẫn: `STAGE3_TESTING.md`

**Kiểm tra:**

1. Chạy Backend: `mvn spring-boot:run`
2. Upload code ESP32 với WiFi và MQTT config
3. Rung cảm biến → Còi kêu + Gửi MQTT
4. Check logs Backend: Phải thấy "Vibration event saved"
5. Check API: `curl http://localhost:8080/api/events`

**Flow hoàn chỉnh:**

```
ESP32 (rung) → Buzzer kêu → MQTT Publish → Backend Subscribe → Save DB → REST API
```

---

### 🔜 Giai đoạn 4: Giao diện Real-time (TIẾP THEO)

**Mục tiêu:** Hiển thị cảnh báo ngay lập tức trên Web/App

**Backend:**

- [ ] Thêm `spring-boot-starter-websocket`
- [ ] Tạo WebSocket config
- [ ] Đẩy sự kiện mới qua WebSocket (`/topic/alerts`)

**Frontend:**

- [ ] Tạo React app
- [ ] Gọi API `GET /api/events` để load lịch sử
- [ ] Subscribe WebSocket để nhận cảnh báo real-time

---

### 🔜 Giai đoạn 5: Vòng lặp điều khiển

**Mục tiêu:** Tắt còi từ xa qua Web/App

**Backend:**

- [ ] Tạo API `POST /api/control/{deviceId}`
- [ ] Tạo `MqttPublisherService`
- [ ] Publish lệnh đến topic `vibra_guard/control/{deviceId}`

**ESP32:**

- [ ] Subscribe topic `vibra_guard/control/ESP32_CUA_SO_01`
- [ ] Nhận lệnh `DISARM` → Tắt còi

**Frontend:**

- [ ] Thêm nút "Tắt báo động"
- [ ] Gọi API control

---

## Hướng dẫn chạy toàn bộ hệ thống

### 1. Khởi động Infrastructure

```bash
docker-compose up -d
```

### 2. Chạy Backend

```bash
cd backend
mvn spring-boot:run
```

### 3. Upload code ESP32

- Mở Arduino IDE
- Mở file `esp32/vibraguard_hardware_core.ino`
- Chọn board ESP32 và COM port
- Upload

### 4. Kiểm tra

- Rung cảm biến → Còi kêu
- Gọi API: `curl http://localhost:8080/api/events`

---

## Công nghệ sử dụng

### Backend

- Spring Boot 3.1.5
- Spring Data JPA
- PostgreSQL Driver
- Lombok
- Maven

### Hardware

- ESP32
- SW-420 Vibration Sensor
- Buzzer
- Transistor NPN (S8050/2N2222)

### Infrastructure

- Docker & Docker Compose
- Mosquitto MQTT Broker
- PostgreSQL 14

---

## Liên hệ & Đóng góp

Dự án đang trong giai đoạn phát triển. Mọi đóng góp và phản hồi đều được hoan nghênh!

---

**Cập nhật lần cuối:** 26/10/2025
**Phiên bản:** 0.1.0-SNAPSHOT
