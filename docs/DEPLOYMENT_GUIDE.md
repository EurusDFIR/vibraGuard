# 🚀 VibraGuard - Hướng dẫn Triển khai Hoàn chỉnh

## ✅ Đã Hoàn Thành

### Backend Enhancements

1. **WebSocket Support** ✅

   - Endpoint: `ws://localhost:8080/ws`
   - Topic: `/topic/alerts` (broadcast real-time alerts)
   - SockJS fallback support

2. **Control API** ✅

   - `POST /api/control/arm` - Kích hoạt hệ thống
   - `POST /api/control/disarm` - Tắt còi báo động
   - `GET /api/control/status` - Trạng thái hệ thống

3. **Real-time Alert Broadcasting** ✅
   - Khi ESP32 gửi MQTT alert → Backend lưu DB → Gửi WebSocket alert đến tất cả clients

### React Web App

1. **Complete UI** ✅

   - Real-time alerts display
   - Event history table
   - ARM/DISARM control buttons
   - System status badges

2. **WebSocket Integration** ✅

   - Auto-connect on page load
   - Subscribe to `/topic/alerts`
   - Browser push notifications

3. **REST API Integration** ✅
   - Fetch event history
   - Send control commands
   - Check system status

## 📋 Hướng dẫn Chạy Hệ thống

### Bước 1: Khởi động Backend

```bash
cd backend
mvn clean install
mvn spring-boot:run
```

**Kiểm tra**: Backend chạy tại http://localhost:8080

### Bước 2: Khởi động React Web App

```bash
cd frontend
npm install  # (nếu chưa cài)
npm run dev
```

**Truy cập**: http://localhost:5173

### Bước 3: Upload Code Arduino

1. Mở `esp32/vibraguard_ai_final/vibraguard_ai_final.ino`
2. Kiểm tra cấu hình WiFi và MQTT:
   ```cpp
   const char *WIFI_SSID = "LE HUNG";
   const char *MQTT_SERVER = "192.168.1.11";  // IP máy chạy Mosquitto
   ```
3. Upload lên ESP32C3
4. Mở Serial Monitor (115200 baud)

### Bước 4: Test Hoàn chỉnh

1. **Kiểm tra kết nối**:

   - Web UI hiển thị: 🟢 Connected + 🔔 ARMED
   - Serial Monitor: "WebSocket connected"

2. **Test Attack Detection**:

   - Tạo rung động mạnh (máy khoan/gõ mạnh)
   - **ESP32**: Còi hú, gửi MQTT alert
   - **Web UI**: Alert mới xuất hiện ngay lập tức
   - **Browser**: Thông báo push notification

3. **Test Remote Control**:

   - Bấm nút "🔕 Tắt báo động (DISARM)" trên Web
   - **ESP32**: Còi phải tắt ngay lập tức
   - Serial Monitor: "📨 MQTT Message [vibra_guard/control/ESP32_CUA_SO_01]: DISARM"

4. **Test Event History**:
   - Bấm nút "🔄 Làm mới"
   - Xem danh sách tất cả các attack đã xảy ra

## 🎯 Tính năng Đã Hoàn thành

### ESP32 (AI + IoT)

- ✅ MPU6050 sensor reading (50Hz)
- ✅ Edge Impulse AI inference (95.1% accuracy)
- ✅ Smart attack detection (80% confidence threshold)
- ✅ Buzzer control (continuous alarm)
- ✅ MQTT alerts to backend
- ✅ Remote ARM/DISARM via MQTT

### Backend (Spring Boot)

- ✅ MQTT subscriber (vibra_guard/sensor)
- ✅ PostgreSQL database storage
- ✅ REST API (events, control, status)
- ✅ WebSocket real-time broadcasting
- ✅ MQTT publisher (control commands)

### Frontend (React)

- ✅ WebSocket real-time alerts
- ✅ Event history display
- ✅ ARM/DISARM control buttons
- ✅ System status monitoring
- ✅ Browser push notifications
- ✅ Responsive design

## 📱 Flutter Mobile App (Coming Next)

Để xây dựng Flutter app, chạy lệnh:

```bash
cd vibraGuard
flutter create vibraguard_app
cd vibraguard_app
flutter pub add http stomp_dart_client flutter_local_notifications
```

Sau đó tạo UI tương tự React app nhưng có thêm:

- Push notifications trên mobile
- Material Design UI
- Background service (optional)

## 🛠️ Troubleshooting

### WebSocket không kết nối được

- Kiểm tra backend đã chạy chưa: `http://localhost:8080`
- Kiểm tra CORS settings trong backend
- Xem console log trên browser (F12)

### ESP32 không gửi MQTT

- Kiểm tra WiFi connection trên Serial Monitor
- Ping MQTT broker: `ping 192.168.1.11`
- Test MQTT:
  ```bash
  mosquitto_sub -h 192.168.1.11 -t "vibra_guard/sensor" -v
  ```

### Không nhận được WebSocket alerts

- Kiểm tra backend logs xem có gửi alerts không
- Xem console log trên browser
- Test manually:
  ```bash
  mosquitto_pub -h 192.168.1.11 -t "vibra_guard/sensor" -m '{"deviceId":"ESP32_CUA_SO_01","timestamp":1234567890,"ai_triggered":true,"confidence":0.95}'
  ```

## 🎉 Kết quả Mong đợi

Khi hoàn thành, bạn sẽ có:

1. **ESP32**: Phát hiện attack real-time với AI 95.1% accuracy
2. **Web UI**: Hiển thị alerts ngay lập tức không cần F5
3. **Remote Control**: Tắt còi từ xa qua Web/Mobile
4. **Database**: Lưu trữ toàn bộ lịch sử
5. **Notifications**: Push notifications trên browser và mobile

## 📚 Tài liệu Tham khảo

- **Backend API**: http://localhost:8080/swagger-ui.html (nếu có Swagger)
- **Edge Impulse Dashboard**: https://studio.edgeimpulse.com/
- **React Docs**: https://react.dev/
- **Flutter Docs**: https://docs.flutter.dev/

---

**Happy Building! 🚀**

Nếu cần hỗ trợ thêm, tham khảo:

- `guided.md` - Hướng dẫn Phase 1-2 (Backend + ESP32)
- `guided2.md` - Hướng dẫn Edge Impulse AI training
- `guided3.md` - Hướng dẫn Frontend development
