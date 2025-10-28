# 🛡️ VibraGuard - AI-Powered IoT Security System

[![AI Accuracy](https://img.shields.io/badge/AI%20Accuracy-95.1%25-brightgreen)](https://github.com/EurusDFIR/vibraGuard)
[![Edge Impulse](https://img.shields.io/badge/Edge%20Impulse-TinyML-blue)](https://edgeimpulse.com)
[![Spring Boot](https://img.shields.io/badge/Spring%20Boot-3.1.5-green)](https://spring.io)
[![React](https://img.shields.io/badge/React-18-61dafb)](https://react.dev)
[![Flutter](https://img.shields.io/badge/Flutter-Ready-02569B)](https://flutter.dev)

## 📖 Tổng quan

**VibraGuard** là hệ thống bảo mật IoT hoàn chỉnh sử dụng **AI TinyML** (Edge Impulse) để phát hiện tấn công với độ chính xác **95.1%**. Hệ thống kết hợp ESP32-C3, cảm biến MPU-6050, MQTT, WebSocket real-time và giao diện Web/Mobile hiện đại.

### ✨ Tính năng nổi bật

- 🧠 **AI Detection**: Edge Impulse TinyML với 95.1% accuracy
- ⚡ **Real-time**: WebSocket alerts < 100ms latency
- 🔔 **Smart Alarm**: Buzzer tự động với AI filtering
- 📱 **Multi-platform**: React Web + Flutter Mobile
- 🌐 **Remote Control**: ARM/DISARM từ xa qua MQTT
- 💾 **Data Persistence**: PostgreSQL với lịch sử đầy đủ
- 🐳 **Docker Ready**: Easy deployment với docker-compose

---

## 🏗️ Kiến trúc Hệ thống

```
┌─────────────────────────────────────────────────────────────┐
│         ESP32-C3 + MPU6050 (AI 95.1% Accuracy)              │
│  [Sensor 50Hz] → [AI Inference 1ms] → [MQTT Pub/Sub]       │
└─────────────────────────────────────────────────────────────┘
                          ↓ MQTT
┌─────────────────────────────────────────────────────────────┐
│           Mosquitto Broker (Docker) Port 1883               │
└─────────────────────────────────────────────────────────────┘
                          ↓
┌─────────────────────────────────────────────────────────────┐
│              Spring Boot Backend (WebSocket)                │
│  [MQTT Sub] → [PostgreSQL] → [WebSocket Broadcast]         │
│  [REST API] → [MQTT Pub] → [Control Commands]              │
└─────────────────────────────────────────────────────────────┘
              ↓ WebSocket          ↓ REST API
┌───────────────────┐    ┌────────────────────────┐
│   React Web       │    │  Flutter Mobile        │
│  Real-time UI     │    │  Push Notifications    │
└───────────────────┘    └────────────────────────┘
```

---

## 🚀 Quickstart (5 phút)

### Prerequisites

- Docker & Docker Compose
- Maven & Java 17
- Node.js & npm
- Arduino IDE (với ESP32 support)
- Flutter SDK (optional)

### 1️⃣ Khởi động Infrastructure

```bash
docker-compose up -d
docker ps  # Kiểm tra mosquitto + postgres đang chạy
```

### 2️⃣ Khởi động Backend

```bash
cd backend
mvn spring-boot:run
# Backend sẽ chạy tại http://localhost:8080
```

### 3️⃣ Khởi động React Web App

```bash
cd frontend
npm install  # Lần đầu tiên
npm run dev  # http://localhost:5173
```

### 4️⃣ Upload Code ESP32

1. Mở Arduino IDE
2. File → Open → `esp32/vibraguard_ai_final/vibraguard_ai_final.ino`
3. Cấu hình WiFi + MQTT IP (đầu file)
4. Upload lên ESP32-C3
5. Mở Serial Monitor (115200 baud)

### 5️⃣ Test Hệ thống

- **Tạo rung động mạnh** (máy khoan, gõ mạnh)
- **ESP32**: Còi hú + Serial hiển thị AI scores
- **Web UI**: Alert mới xuất hiện ngay lập tức (< 100ms)
- **Browser**: Push notification popup
- **Bấm DISARM**: Còi tắt ngay

✅ **Hệ thống hoạt động hoàn hảo!**

---

## 📊 AI Model Performance

| Metric               | Value                       |
| -------------------- | --------------------------- |
| **Overall Accuracy** | 95.1% (int8), 97% (float32) |
| **Attack Precision** | 93.1% (230/247)             |
| **Normal Precision** | 95.8% (230/240)             |
| **Noise Precision**  | 98.8% (84/85)               |
| **Inference Time**   | 1ms                         |
| **RAM Usage**        | 1.4KB                       |
| **Flash Usage**      | 15.2KB                      |
| **False Positive**   | 4.2%                        |
| **False Negative**   | 6.9%                        |

**Training Platform**: Edge Impulse  
**Sampling Rate**: 50Hz (20ms delay)  
**Window Size**: 2000ms  
**Features**: FFT 128, Raw Data, Spectral Analysis

---

## 🛠️ Technology Stack

### Embedded

- **MCU**: ESP32-C3 Super Mini
- **Sensor**: MPU-6050 (6-axis IMU)
- **AI Framework**: Edge Impulse TinyML
- **Language**: C++ (Arduino)
- **Protocol**: MQTT (PubSubClient)

### Backend

- **Framework**: Spring Boot 3.1.5
- **Language**: Java 17
- **Database**: PostgreSQL 14
- **MQTT**: Spring Integration MQTT
- **WebSocket**: Spring WebSocket (STOMP)
- **Build Tool**: Maven

### Frontend

- **Web**: React 18 + Vite
- **Mobile**: Flutter (Dart)
- **WebSocket**: SockJS + StompJS (React), stomp_dart_client (Flutter)
- **HTTP Client**: Axios (React), http (Flutter)
- **Notifications**: Browser API (React), flutter_local_notifications (Flutter)

### Infrastructure

- **Containerization**: Docker + Docker Compose
- **MQTT Broker**: Eclipse Mosquitto
- **Database**: PostgreSQL 14

---

## � Project Structure

```
vibraGuard/
├── backend/                      # Spring Boot Backend
│   ├── src/main/java/com/vibraguard/
│   │   ├── config/              # MQTT + WebSocket config
│   │   ├── controller/          # REST API + Control
│   │   ├── service/             # Business logic
│   │   ├── model/               # JPA entities
│   │   ├── repository/          # Data access
│   │   └── dto/                 # Data transfer objects
│   └── pom.xml
├── frontend/                     # React Web App
│   ├── src/
│   │   ├── App.jsx              # Main component
│   │   └── App.css              # Styling
│   └── package.json
├── esp32/                        # Arduino ESP32 Code
│   ├── vibraguard_ai_final/     # 🎯 Production code (AI + MQTT)
│   ├── vibraguard_mqtt_integration/  # Legacy SW-420 version
│   ├── data_forwarder/          # Edge Impulse data collection
│   └── i2c_debug/               # MPU6050 debugging
├── docs/                         # Documentation
│   └── best_practices.md        # AI training guide
├── docker-compose.yml            # Mosquitto + PostgreSQL
├── mosquitto.conf                # MQTT broker config
├── DEPLOYMENT_GUIDE.md           # 📖 Full deployment guide
├── FLUTTER_GUIDE.md              # 📱 Flutter app guide
├── QUICKSTART_ALL.md             # ⚡ 5-minute quickstart
└── SYSTEM_SUMMARY.md             # 📋 Complete system summary
```

---

## 📡 API Endpoints

### REST API

| Method | Endpoint                        | Description                  |
| ------ | ------------------------------- | ---------------------------- |
| `GET`  | `/api/events`                   | Lấy danh sách tất cả sự kiện |
| `GET`  | `/api/events/device/{deviceId}` | Lấy sự kiện theo device      |
| `POST` | `/api/control/arm`              | Kích hoạt báo động           |
| `POST` | `/api/control/disarm`           | Tắt còi (gửi MQTT DISARM)    |
| `GET`  | `/api/control/status`           | Trạng thái hệ thống          |

### WebSocket

- **Endpoint**: `ws://localhost:8080/ws`
- **Protocol**: STOMP over WebSocket
- **Topic**: `/topic/alerts` (broadcast real-time alerts)

### MQTT Topics

| Topic                            | Direction       | Payload | Description                   |
| -------------------------------- | --------------- | ------- | ----------------------------- |
| `vibra_guard/sensor`             | ESP32 → Backend | JSON    | Attack alerts với AI scores   |
| `vibra_guard/control/{deviceId}` | Backend → ESP32 | Text    | Control commands (ARM/DISARM) |

**Example Payload (sensor)**:

```json
{
  "deviceId": "ESP32_CUA_SO_01",
  "timestamp": 1234567890,
  "ai_triggered": true,
  "scores": {
    "attack": 0.95,
    "normal": 0.03,
    "noise": 0.02
  },
  "confidence": 0.95
}
```

---

## 🎯 Development Roadmap

### ✅ Phase 1: Hardware & Infrastructure (Completed)

- [x] ESP32-C3 + MPU6050 hardware setup
- [x] Docker infrastructure (Mosquitto + PostgreSQL)
- [x] Basic vibration detection

### ✅ Phase 2: Backend & Database (Completed)

- [x] Spring Boot REST API
- [x] PostgreSQL integration
- [x] MQTT subscriber
- [x] Data persistence

### ✅ Phase 3: AI Integration (Completed)

- [x] Edge Impulse training (95.1% accuracy)
- [x] ESP32 AI inference (1ms)
- [x] Smart attack/normal/noise classification
- [x] Data collection at 50Hz

### ✅ Phase 4: Real-time Communication (Completed)

- [x] WebSocket broadcasting
- [x] MQTT publisher (control commands)
- [x] Remote ARM/DISARM

### ✅ Phase 5: Frontend Development (Completed)

- [x] React Web App với real-time alerts
- [x] Flutter Mobile App với push notifications
- [x] Responsive UI design
- [x] Browser notifications

### 🚀 Phase 6: Future Enhancements (Planned)

- [ ] Firebase Cloud Messaging (push qua internet)
- [ ] Multi-device management dashboard
- [ ] Historical data analytics & charts
- [ ] User authentication & authorization
- [ ] Firmware OTA updates
- [ ] ML model A/B testing
- [ ] Battery-powered mode optimization

---

## 📖 Documentation

| File                                             | Description                     |
| ------------------------------------------------ | ------------------------------- |
| [QUICKSTART_ALL.md](QUICKSTART_ALL.md)           | ⚡ 5-minute quickstart guide    |
| [DEPLOYMENT_GUIDE.md](DEPLOYMENT_GUIDE.md)       | 📖 Full deployment instructions |
| [FLUTTER_GUIDE.md](FLUTTER_GUIDE.md)             | 📱 Flutter app setup guide      |
| [SYSTEM_SUMMARY.md](SYSTEM_SUMMARY.md)           | 📋 Complete system overview     |
| [guided.md](guided.md)                           | 📘 Phase 1-2: Backend + ESP32   |
| [guided2.md](guided2.md)                         | 📗 Phase 3: Edge Impulse AI     |
| [guided3.md](guided3.md)                         | 📙 Phase 4: Frontend            |
| [STAGE3_TESTING.md](STAGE3_TESTING.md)           | 🧪 MQTT testing guide           |
| [docs/best_practices.md](docs/best_practices.md) | 🎓 AI training best practices   |

---

## 🧪 Testing Guide

### Test 1: Attack Detection + Real-time Alert

1. **Trigger**: Tạo rung động mạnh (máy khoan/gõ mạnh)
2. **ESP32**: Còi hú + Serial hiển thị "🚨 ATTACK DETECTED!"
3. **Web UI**: Alert mới xuất hiện < 100ms
4. **Backend**: Logs "Vibration event saved"
5. **Database**: New row trong `vibration_events` table

### Test 2: Remote Control (DISARM)

1. **Web UI**: Bấm nút "🔕 Tắt báo động"
2. **Backend**: Publish MQTT "DISARM"
3. **ESP32**: Nhận message → Tắt còi ngay
4. **Serial**: "🔕 DISARM Command Received"

### Test 3: AI Filtering (Normal Activity)

1. **Trigger**: Gõ nhẹ, đóng cửa bình thường
2. **ESP32**: AI scores: Normal > Attack
3. **Còi**: Im lặng (không trigger alarm)
4. **Web UI**: Không có alert mới

### Test 4: WebSocket Real-time

1. **Open Web UI**: Kiểm tra "🟢 Connected"
2. **Trigger Attack**: Tạo rung động
3. **Verify**: Alert xuất hiện KHÔNG CẦN F5

---

## 🛠️ Troubleshooting

### ❌ WebSocket không kết nối

**Triệu chứng**: "🔴 Disconnected" trên Web UI

**Giải pháp**:

```bash
# Kiểm tra backend chạy
curl http://localhost:8080/api/control/status

# Clear browser cache
Ctrl + Shift + R
```

### ❌ ESP32 không kết nối MQTT

**Triệu chứng**: Serial "❌ Failed! rc=-2"

**Giải pháp**:

```bash
# Kiểm tra Mosquitto
docker ps | grep mosquitto

# Test MQTT
mosquitto_sub -h localhost -t "vibra_guard/sensor" -v

# Kiểm tra firewall (Windows)
netsh advfirewall firewall add rule name="MQTT" dir=in action=allow protocol=TCP localport=1883
```

### ❌ AI không phát hiện attack

**Triệu chứng**: Gõ mạnh nhưng không alert

**Giải pháp**:

1. Kiểm tra MPU6050 wiring (SDA=GP8, SCL=GP9)
2. Xem Serial có "🧠 AI:" outputs không
3. Tăng cường độ rung động (dùng máy khoan)
4. Check threshold trong code: `ATTACK_THRESHOLD = 0.80`

---

## 📊 Performance Metrics

### System Latency

- **ESP32 AI Inference**: 1ms
- **MQTT Publish**: < 50ms
- **Backend Processing**: < 10ms
- **WebSocket Broadcast**: < 5ms
- **Web UI Render**: < 10ms
- **Total End-to-End**: < 100ms ⚡

### Resource Usage

- **ESP32 RAM**: 1.4KB (AI model)
- **ESP32 Flash**: 15.2KB (AI model)
- **Backend Memory**: ~200MB (Spring Boot)
- **Database Size**: ~10MB (10,000 events)
- **Network Traffic**: ~500 bytes/alert

---

## 🤝 Contributing

Contributions are welcome! Please follow these guidelines:

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

---

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

---

## 👥 Authors

- **EurusDFIR** - _Initial work_ - [GitHub](https://github.com/EurusDFIR)

---

## 🙏 Acknowledgments

- **Edge Impulse** - TinyML platform
- **Spring Boot** - Backend framework
- **React** - Web framework
- **Flutter** - Mobile framework
- **Eclipse Mosquitto** - MQTT broker
- **Arduino Community** - ESP32 support

---

## 📞 Contact & Support

- **GitHub Issues**: [Create an issue](https://github.com/EurusDFIR/vibraGuard/issues)
- **Documentation**: See `docs/` folder
- **Email**: (Add your email here)

---

**Built with ❤️ using AI, IoT, and Modern Web Technologies**

**Last Updated**: October 29, 2025  
**Version**: 1.0.0 (Production Ready) 🎉
