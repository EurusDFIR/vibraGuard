<div align="center">

# 🛡️ VibraGuard AI

### Intelligent Intrusion Detection with Edge AI

_An AIoT security system that uses TinyML to detect break-in attempts through vibration analysis_

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Platform](https://img.shields.io/badge/platform-ESP32--C3-blue.svg)](https://www.espressif.com/en/products/socs/esp32-c3)
[![AI Accuracy](https://img.shields.io/badge/AI%20Accuracy-95.1%25-brightgreen.svg)](docs/reports/TESTING_FINAL_REPORT.md)
[![Inference](https://img.shields.io/badge/Inference-1ms-orange.svg)](docs/SYSTEM_ARCHITECTURE.md)
[![Backend](https://img.shields.io/badge/Backend-Spring%20Boot%203.1.5-green.svg)](backend/)
[![Cloud](https://img.shields.io/badge/Cloud-GCP-4285F4.svg)](https://cloud.google.com/)

[Features](#-key-features) •
[Architecture](#-system-architecture) •
[Quick Start](#-quick-start) •
[Documentation](#-documentation) •
[Contributing](#-contributing)

</div>

---

## 📖 About

**VibraGuard AI** is a cutting-edge security system that solves the chronic **false alarm problem** of traditional vibration sensors. Instead of simple threshold-based detection, VibraGuard uses **TinyML (Tiny Machine Learning)** running directly on an ESP32-C3 microcontroller to intelligently classify vibrations as:

- **Attack** 🚨 - Break-in attempts (drilling, smashing, prying)
- **Normal** ✅ - Harmless events (door knocks, wind, footsteps)
- **Noise** 🔇 - Environmental interference (passing vehicles, distant machinery)

By processing AI inference **at the edge** (1ms latency), VibraGuard achieves:

- **95.1% accuracy** (Attack: 93.1%, Normal: 95.8%, Noise: 98.8%)
- **<2 second** end-to-end response time
- **>90% reduction** in false alarms vs traditional sensors

---

## ✨ Key Features

### 🧠 Edge AI Processing

- **TensorFlow Lite** model trained on Edge Impulse
- **1ms inference time** on ESP32-C3 (RISC-V @ 160MHz)
- **1.4KB RAM** footprint, **45.1KB Flash**
- **Spectral Analysis (FFT)** for frequency-domain feature extraction

### 🔗 Full-Stack IoT Architecture

- **ESP32-C3 Super Mini** with MPU-6050 (6-axis accelerometer/gyro)
- **MQTT** protocol (Eclipse Mosquitto broker)
- **Spring Boot 3.1.5** backend (Java 17)
- **PostgreSQL 14** database
- **React + Vite** web dashboard
- **Flutter 3.9** mobile app (Android/iOS)

### ☁️ Cloud Deployment

- **Google Cloud Platform** (Compute Engine e2-small)
- **Docker Compose** orchestration (3 services)
- **24/7 uptime** with auto-restart policies
- **Static IP** (34.87.133.103)

### 📢 Multi-Channel Alerts

- **Local Buzzer** - Immediate on-device alarm
- **WebSocket (STOMP)** - Real-time push to web/mobile
- **SMS** - Twilio API integration
- **Voice Call** - TwiML automated phone calls

### 🎮 Remote Control

- **ARM/DISARM** system via web or mobile app
- **Event history** with confidence scores
- **Real-time status** monitoring

---

## 🏗️ System Architecture

```
┌─────────────────────────────────────────────────────────────────────┐
│                         VibraGuard AI System                        │
└─────────────────────────────────────────────────────────────────────┘

┌──────────────┐     MQTT      ┌──────────────┐     WebSocket    ┌──────────┐
│   ESP32-C3   │───────────────▶│  Spring Boot │◀─────────────────│  React   │
│  + MPU-6050  │  vibra_guard/ │   Backend    │   /topic/alerts  │ Dashboard│
│  + TinyML AI │    sensor     │              │                  └──────────┘
└──────┬───────┘               │ ┌──────────┐ │
       │                       │ │PostgreSQL│ │     REST API     ┌──────────┐
       │ Buzzer GP1            │ └──────────┘ │◀─────────────────│  Flutter │
       │                       │              │   /api/control   │   App    │
       ▼                       │ ┌──────────┐ │                  └──────────┘
   🔊 Local                    │ │Mosquitto │ │
     Alert                     │ │  Broker  │ │     Twilio API   ┌──────────┐
                               │ └──────────┘ │─────────────────▶│   📱     │
                               │              │   SMS + Voice    │  Phone   │
                               └──────────────┘                  └──────────┘
```

For detailed architecture diagrams, see [SYSTEM_ARCHITECTURE.md](docs/SYSTEM_ARCHITECTURE.md).

---

## 🚀 Quick Start

### Prerequisites

- **Hardware:** ESP32-C3 Super Mini, MPU-6050 sensor, Buzzer, NPN transistor (S8050)
- **Software:** Arduino IDE 1.8+ / PlatformIO, Docker, Node.js 18+, Java 17+
- **Cloud:** Google Cloud SDK (optional, for deployment)

### 1. ESP32 Setup

```bash
# 1. Flash firmware
cd esp32/vibraguard_ai_final
# Open .ino in Arduino IDE
# Select Board: ESP32C3 Dev Module
# Upload

# 2. Configure WiFi (in code)
const char* WIFI_SSID = "YourNetwork";
const char* WIFI_PASSWORD = "YourPassword";
const char* MQTT_SERVER = "34.87.133.103";  # Or your server IP
```

### 2. Backend Setup (Local)

```bash
# Start infrastructure
docker-compose up -d

# Build backend
cd backend
mvn clean package
java -jar target/app.jar
```

Backend will be available at `http://localhost:8080`

### 3. Frontend Setup

```bash
# Web Dashboard
cd frontend
npm install
npm run dev
# Open http://localhost:5174

# Mobile App
cd vibraguard_app
flutter pub get
flutter run
```

### 4. Test the System

```bash
# Run integration tests
cd tests/integration
pip install -r requirements.txt
python test_system_complete.py
```

---

## 📚 Documentation

| Document                                          | Description                           |
| ------------------------------------------------- | ------------------------------------- |
| [📐 Architecture](docs/SYSTEM_ARCHITECTURE.md)    | System design with 8 Mermaid diagrams |
| [🚀 Deployment](docs/DEPLOYMENT_GUIDE.md)         | Step-by-step GCP deployment           |
| [🗄️ Database](docs/DBEAVER_CONNECTION_GUIDE.md)   | PostgreSQL setup & 19 SQL queries     |
| [🧪 Testing](docs/TESTING_GUIDE.md)               | Test suites and validation            |
| [📱 Flutter Guide](docs/guides/FLUTTER_GUIDE.md)  | Mobile app development                |
| [🌐 WebSocket](docs/WEBSOCKET_TROUBLESHOOTING.md) | Real-time communication               |
| [📊 Final Report](docs/reports/Report_final.md)   | Complete academic report              |

---

## 🛠️ Tech Stack

### Edge Device

- **MCU:** ESP32-C3 Super Mini (RISC-V, 160MHz, WiFi)
- **Sensor:** MPU-6050 (6-axis, I2C, 50Hz sampling)
- **AI:** Edge Impulse TinyML (TensorFlow Lite for Microcontrollers)

### Backend

- **Framework:** Spring Boot 3.1.5 (Java 17)
- **Database:** PostgreSQL 14-alpine
- **Broker:** Eclipse Mosquitto (MQTT)
- **Container:** Docker Compose

### Frontend

- **Web:** React 18 + Vite 4
- **Mobile:** Flutter 3.9.2 (Dart 3.0)

### Cloud

- **Platform:** Google Cloud Platform
- **Compute:** Compute Engine e2-small (2 vCPUs, 2GB RAM)
- **Networking:** VPC, Firewall, Static IP

### Alerts

- **SMS/Voice:** Twilio API SDK 10.6.5
- **Real-time:** WebSocket (STOMP)
- **Local:** Buzzer (PWM)

---

## 📊 Performance Metrics

| Metric                    | Value  | Notes                                      |
| ------------------------- | ------ | ------------------------------------------ |
| **AI Accuracy**           | 95.1%  | Attack: 93.1%, Normal: 95.8%, Noise: 98.8% |
| **Inference Time**        | 1ms    | Edge processing on ESP32-C3                |
| **RAM Usage**             | 1.4KB  | TensorFlow Lite model                      |
| **Flash Usage**           | 45.1KB | Optimized with quantization                |
| **End-to-End Latency**    | <2s    | Detection → Alert delivery                 |
| **False Alarm Reduction** | >90%   | vs traditional threshold sensors           |
| **Uptime**                | 99.9%  | Docker auto-restart on GCP                 |

---

## 🗂️ Project Structure

```
vibraGuard/
├── 📄 README.md                    # This file
├── 📄 LICENSE                      # MIT License
├── 📄 CONTRIBUTING.md              # Contribution guidelines
├── 📄 CODE_OF_CONDUCT.md           # Community standards
├── 🐳 docker-compose.yml           # Infrastructure orchestration
├── 📡 mosquitto.conf               # MQTT broker config
│
├── 🖥️ backend/                     # Spring Boot backend
│   ├── src/main/java/
│   ├── src/main/resources/
│   ├── pom.xml
│   └── Dockerfile
│
├── 🌐 frontend/                    # React web dashboard
│   ├── src/
│   ├── public/
│   ├── package.json
│   └── vite.config.js
│
├── 📱 vibraguard_app/              # Flutter mobile app
│   ├── lib/
│   ├── android/
│   ├── ios/
│   └── pubspec.yaml
│
├── 🔌 esp32/                       # ESP32-C3 firmware
│   └── vibraguard_ai_final/
│       └── vibraguard_ai_final.ino
│
├── 📚 docs/                        # Documentation
│   ├── DEPLOYMENT_GUIDE.md
│   ├── SYSTEM_ARCHITECTURE.md
│   ├── DBEAVER_CONNECTION_GUIDE.md
│   ├── guides/
│   │   ├── FLUTTER_GUIDE.md
│   │   ├── QUICKSTART.md
│   │   └── GCP_DEPLOYMENT_CHECKLIST.md
│   └── reports/
│       └── Report_final.md
│
├── 🧪 tests/                       # Test suites
│   ├── mqtt/
│   │   ├── test_mqtt_python.py
│   │   └── test_mqtt_websocket.sh
│   ├── backend/
│   │   ├── test_control_command.py
│   │   └── test_control_panel.html
│   ├── websocket/
│   │   ├── test_websocket.html
│   │   └── test_websocket_health.html
│   └── integration/
│       ├── test_system_complete.py
│       ├── test_db_connection.py
│       └── test_twilio_trigger.py
│
└── 🛠️ scripts/                     # Utility scripts
    └── setup/
```

---

## 🧪 Testing

```bash
# Unit tests (Backend)
cd backend
mvn test

# Integration tests
cd tests/integration
python test_system_complete.py

# MQTT connectivity
cd tests/mqtt
python test_mqtt_python.py

# WebSocket health
cd tests/websocket
# Open test_websocket_health.html in browser

# Database connection
cd tests/integration
python test_db_connection.py
```

---

## 🚢 Deployment

### Local Development

```bash
docker-compose up -d
cd backend && mvn spring-boot:run
cd frontend && npm run dev
```

### GCP Production

See [DEPLOYMENT_GUIDE.md](docs/DEPLOYMENT_GUIDE.md) for step-by-step instructions.

```bash
# Quick deploy
gcloud compute instances create vibraguard-server \
  --machine-type=e2-small \
  --zone=asia-southeast1-b

# Upload and build
scp -r . vibraguard-server:~/vibraguard
gcloud compute ssh vibraguard-server
cd vibraguard && docker-compose up --build -d
```

---

## 🤝 Contributing

We welcome contributions! Please see [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines.

### Quick Contribution Steps

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'feat(esp32): add WiFi manager'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

See [CODE_OF_CONDUCT.md](CODE_OF_CONDUCT.md) for community guidelines.

---

## 📜 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

---

## 🙏 Acknowledgments

- **Edge Impulse** - TinyML platform for model training
- **Espressif** - ESP32-C3 hardware and SDK
- **Spring Team** - Spring Boot framework
- **Twilio** - SMS/Voice API
- **Google Cloud** - Cloud infrastructure

---

## 📞 Contact & Support

- **GitHub Issues:** [Report bugs or request features](https://github.com/EurusDFIR/vibraGuard/issues)
- **Email:** vibraguard@example.com
- **Documentation:** [Full docs](docs/)
- **Report:** [Academic report](docs/reports/Report_final.md)

---

## 🎯 Roadmap

- [x] Edge AI with 95.1% accuracy
- [x] Full-stack IoT implementation
- [x] GCP deployment
- [x] Multi-channel alerts (Buzzer + SMS + Voice + WebSocket)
- [ ] WiFiManager integration (hardcoded WiFi currently)
- [ ] I2S audio output (MAX98357A + Speaker)
- [ ] Secondary alarm system (tamper detection)
- [ ] ESP32-CAM integration (photo capture on attack)
- [ ] Mobile app on Play Store / App Store
- [ ] Support for multiple materials (door, safe, window)

---

## 📈 Project Status

**Current Version:** 1.0.0  
**Status:** ✅ Production Ready  
**Last Updated:** November 2025

---

<div align="center">

**Made with ❤️ by the VibraGuard AI Team**

[⬆ Back to Top](#️-vibraguard-ai)

</div>
