# 📱 VibraGuard Flutter Mobile App - Hướng dẫn Hoàn chỉnh

## 🎯 Tổng quan

Flutter app cho phép bạn:

- 📡 Nhận cảnh báo real-time từ ESP32 qua WebSocket
- 📜 Xem lịch sử tất cả các sự kiện
- 🎛️ Điều khiển ARM/DISARM từ xa
- 📲 Nhận push notifications trên điện thoại

---

## 🚀 Hướng dẫn Tạo và Chạy Flutter App

### Bước 1: Tạo Flutter Project

```bash
cd r:\_Projects\Eurus_Workspace\vibraGuard
flutter create vibraguard_app
cd vibraguard_app
```

### Bước 2: Cài đặt Dependencies

Mở file `pubspec.yaml` và thêm dependencies:

```yaml
dependencies:
  flutter:
    sdk: flutter
  http: ^1.1.0
  stomp_dart_client: ^2.0.0
  flutter_local_notifications: ^17.0.0
```

Sau đó chạy:

```bash
flutter pub get
```

### Bước 3: Cấu hình Android Permissions

Mở `android/app/src/main/AndroidManifest.xml` và thêm:

```xml
<manifest xmlns:android="http://schemas.android.com/apk/res/android">
    <!-- Thêm các permissions này -->
    <uses-permission android:name="android.permission.INTERNET"/>
    <uses-permission android:name="android.permission.ACCESS_NETWORK_STATE"/>
    <uses-permission android:name="android.permission.POST_NOTIFICATIONS"/>
    <uses-permission android:name="android.permission.VIBRATE"/>

    <application
        ...
```

### Bước 4: Copy Code vào lib/main.dart

Copy toàn bộ code từ file `FLUTTER_MAIN_DART.md` vào `lib/main.dart`

**QUAN TRỌNG**: Thay đổi IP backend trong code:

```dart
// Dòng 23-24 trong main.dart
static const String backendUrl = 'http://192.168.1.11:8080'; // Thay IP của bạn
static const String wsUrl = 'ws://192.168.1.11:8080/ws';     // Thay IP của bạn
```

### Bước 5: Chạy App

**Option 1: Chạy trên Emulator Android**

```bash
flutter emulators --launch <emulator_id>
flutter run
```

**Option 2: Chạy trên điện thoại Android thật**

1. Bật Developer Mode và USB Debugging
2. Kết nối điện thoại qua USB
3. Chạy: `flutter run`

**Option 3: Build APK để cài đặt**

```bash
flutter build apk --release
```

APK output: `build/app/outputs/flutter-apk/app-release.apk`

---

## 🧪 Testing Workflow

### 1. Kiểm tra Kết nối

- Mở app
- Xem góc phải trên: phải hiển thị "🟢 Connected"
- Nếu "🔴 Disconnected": Kiểm tra IP backend

### 2. Test Real-time Alert

- Tạo rung động mạnh trên ESP32 (máy khoan, gõ mạnh)
- **App phải**:
  - Hiển thị alert mới trong "🚨 Cảnh báo Real-time"
  - Hiển thị push notification trên thanh thông báo
  - Rung điện thoại (nếu có)

### 3. Test Remote Control

- Tạo attack → Còi ESP32 hú
- Bấm nút **"DISARM"** trên app
- Còi phải tắt ngay lập tức
- Status badge đổi thành "🔕 DISARMED"

### 4. Test History

- Kéo xuống để refresh (Pull to refresh)
- Xem danh sách các sự kiện cũ từ database

---

## 🎨 UI Components

### 1. AppBar

- Tiêu đề: "🛡️ VibraGuard"
- Status badges: Connection + ARM/DISARM status

### 2. Control Panel (Card)

- Nút **ARM**: Kích hoạt hệ thống (màu cam)
- Nút **DISARM**: Tắt báo động (màu xanh)

### 3. Real-time Alerts (Card)

- Hiển thị 5 alerts mới nhất
- Thông tin: Device ID, Severity, Sensor Value, AI Confidence, Timestamp
- Màu đỏ để highlight

### 4. Event History (Card)

- Hiển thị tất cả sự kiện từ database
- ListTile với avatar (ID), title (Device Name), subtitle (timestamp + sensor)
- Severity badge với màu sắc tương ứng

---

## 🔧 Troubleshooting

### Lỗi: WebSocket không kết nối

**Nguyên nhân**: IP backend sai hoặc backend chưa chạy

**Giải pháp**:

1. Kiểm tra backend đang chạy: `http://192.168.1.11:8080`
2. Ping từ điện thoại: Mở browser trên điện thoại, vào URL backend
3. Kiểm tra cùng mạng WiFi: Điện thoại và máy backend phải cùng mạng
4. Thay đổi IP trong `main.dart`:
   ```dart
   static const String backendUrl = 'http://<IP_MÁY_BẠN>:8080';
   ```

### Lỗi: Không nhận được notifications

**Nguyên nhân**: Chưa cấp quyền notifications

**Giải pháp**:

1. Vào Settings → Apps → VibraGuard → Notifications
2. Bật "Allow notifications"

### Lỗi: Không build được APK

**Nguyên nhân**: Thiếu dependencies hoặc Gradle outdated

**Giải pháp**:

```bash
flutter clean
flutter pub get
flutter doctor
flutter build apk --release
```

### Lỗi: "Unhandled Exception: SocketException"

**Nguyên nhân**: Backend không thể truy cập

**Giải pháp**:

1. Kiểm tra firewall trên máy backend (cho phép port 8080)
2. Kiểm tra Docker đã chạy chưa:
   ```bash
   docker ps
   # Phải thấy mosquitto và postgres
   ```
3. Test MQTT từ điện thoại: Dùng MQTT client app (MQTT Explorer)

---

## 📊 API Endpoints Used

### REST API

- `GET http://192.168.1.11:8080/api/events` - Lấy lịch sử
- `POST http://192.168.1.11:8080/api/control/arm` - ARM
- `POST http://192.168.1.11:8080/api/control/disarm` - DISARM
- `GET http://192.168.1.11:8080/api/control/status` - Status

### WebSocket

- Connect: `ws://192.168.1.11:8080/ws`
- Subscribe: `/topic/alerts`
- Protocol: STOMP over WebSocket

---

## 🌟 Features Checklist

- ✅ Real-time WebSocket connection
- ✅ STOMP protocol support
- ✅ Local push notifications
- ✅ Pull-to-refresh history
- ✅ Material Design 3 UI
- ✅ Dark theme
- ✅ Responsive layout
- ✅ Error handling
- ✅ Connection status monitoring
- ✅ ARM/DISARM controls

---

## 📱 Screenshots (Expected)

### Home Screen

- Top: AppBar với status badges
- Middle: Control Panel (ARM/DISARM buttons)
- Bottom: Alerts section + History section

### When Alert Received

- Red card xuất hiện trong Alerts section
- Push notification trên thanh thông báo
- Điện thoại rung

### When Disarmed

- Status badge đổi màu xám "🔕 DISARMED"
- Snackbar hiển thị "✅ System disarmed successfully!"

---

## 🎯 Next Steps (Optional Enhancements)

1. **Background Service**: App nhận alerts ngay cả khi đóng
2. **Firebase Cloud Messaging**: Push notifications qua internet (không cần cùng WiFi)
3. **Charts**: Biểu đồ thống kê attacks theo thời gian
4. **Multi-device**: Quản lý nhiều ESP32 devices
5. **User Authentication**: Login/Logout
6. **Settings Screen**: Thay đổi backend URL trong app

---

## 📚 Dependencies Documentation

- **http**: https://pub.dev/packages/http
- **stomp_dart_client**: https://pub.dev/packages/stomp_dart_client
- **flutter_local_notifications**: https://pub.dev/packages/flutter_local_notifications

---

## 🎉 Kết luận

Khi hoàn thành, bạn sẽ có app mobile hoàn chỉnh với:

- Real-time alerts từ ESP32
- Push notifications trên điện thoại
- Remote control ARM/DISARM
- Lịch sử sự kiện đầy đủ

**Happy Coding! 📱🚀**
