# 🚀 KHỞI ĐỘNG NHANH - VIBRAGUARD

## ⚡ CHẠY HỆ THỐNG

```bash
# Backend
cd backend && mvn spring-boot:run

# Frontend Web
cd frontend && npm run dev

# Mobile Flutter
cd vibraguard_app && flutter run -d emulator-5554
```

## 📝 FILE QUAN TRỌNG

### Backend URL

**Web:** `frontend/src/App.jsx` → dòng 14

```javascript
const BACKEND_URL = "http://localhost:8080";
```

**Mobile:** `vibraguard_app/lib/main.dart` → dòng 28

```dart
static const String backendUrl = 'http://10.0.2.2:8080';  // Emulator
// Thiết bị thật: 'http://192.168.1.x:8080'
```

### Đổi Màu/UI

- **Web:** `frontend/src/App.css`
- **Mobile:** `vibraguard_app/lib/main.dart`

### Config Backend

`backend/src/main/resources/application.properties`

## 🔧 LỖI THƯỜNG GẶP

**Flutter build failed:**

```bash
# Sửa pubspec.yaml: flutter_local_notifications: ^17.0.0
flutter clean && flutter pub get && flutter run
```

**Mobile không connect backend:**

- Emulator: `http://10.0.2.2:8080`
- Thiết bị thật: `http://192.168.1.x:8080` (IP máy tính)

**MQTT IP thay đổi (WiFi đổi):**

- Nếu MQTT trên cùng máy Backend → Dùng `localhost:1883` (đã fix)
- Nếu MQTT trên máy khác → Đặt IP tĩnh cho máy đó trong Router

**Port 8080 đã dùng:**

```bash
# Windows: netstat -ano | findstr :8080 → taskkill /PID xxx /F
# Linux: lsof -ti:8080 | xargs kill -9
```

## 📍 API ENDPOINTS

```
GET  /api/control/status
GET  /api/events
POST /api/control/arm
POST /api/control/disarm
POST /api/control/sound_alarm
WS   /ws
```

## ✅ NHANH

- Backend chạy → `http://localhost:8080`
- Web chạy → `http://localhost:5173`
- Mobile: Nhấn `r` để reload, `R` để restart
