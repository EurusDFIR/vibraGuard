# 🔍 VIBRAGUARD SYSTEM HEALTH CHECK

**Date:** October 31, 2025 - 22:52
**GCP Server:** 34.87.133.103 (vibraguard-server)

---

## ✅ SYSTEM STATUS: FULLY OPERATIONAL

### 1. Docker Services

| Service        | Status     | Port | Uptime    |
| -------------- | ---------- | ---- | --------- |
| Backend        | ✅ Running | 8080 | 3 minutes |
| PostgreSQL     | ✅ Running | 5432 | 3+ hours  |
| Mosquitto MQTT | ✅ Running | 1883 | 3+ hours  |

### 2. Backend API

- **Endpoint:** `http://34.87.133.103:8080/api/control/status`
- **Status:** ✅ Operational
- **Response:**
  ```json
  {
    "devicesCount": 1,
    "systemStatus": "ARMED",
    "timestamp": 1761900640295
  }
  ```

### 3. Twilio Integration

- **Status:** ✅ Initialized Successfully
- **Recipients:** 1 phone number (+84399354603)
- **Features:**
  - SMS Alerts: ✅ Configured
  - Voice Calls: ✅ Configured
  - TwiML URL: ✅ Set
- **Note:** ⚠️ Phone number needs to be verified in Twilio Console for Trial account

### 4. MQTT Message Flow

- **Test Message Sent:** ✅ Success
- **Backend Processing:** ✅ Success
- **Database Entry:** ✅ Created (Event ID: 11)
- **Attack Detection:** ✅ Working (95% confidence detected)
- **Alert Triggered:** ✅ Twilio alert attempted

### 5. Database Status

**Latest Events:**
| ID | Device ID | Confidence | Timestamp | Severity |
|----|-----------|------------|-----------|----------|
| 11 | ESP32_CUA_SO_01 | 95% | 2025-10-31 08:51:08 | WARNING |
| 10 | ESP32_CUA_SO_01 | 98% | 2025-10-31 08:47:27 | WARNING |
| 9 | ESP32_CUA_SO_01 | 98% | 2025-10-31 08:39:00 | WARNING |

### 6. WebSocket Alerts

- **Endpoint:** `ws://34.87.133.103:8080/ws`
- **Topic:** `/topic/alerts`
- **Status:** ✅ Configured and sending alerts

---

## 🧪 TESTING GUIDE

### A. Test Web Interface

1. Mở file: `test_websocket_health.html` trong browser
2. Kết nối tự động đến WebSocket
3. Gửi MQTT message để trigger alert
4. Xem alert hiển thị real-time

### B. Test Mobile App (Flutter)

1. Đảm bảo đang ở branch: `feature/migrate_GCP`
2. Chạy: `flutter run` trong `vibraguard_app/`
3. App sẽ connect đến: `http://34.87.133.103:8080`
4. Kiểm tra:
   - Dashboard hiển thị 1 device
   - System status: ARMED
   - WebSocket connection: Connected
   - Control buttons hoạt động

### C. Test Twilio SMS/Voice (Real Device)

**Prerequisites:**

- ✅ Verify phone number +84399354603 tại: https://console.twilio.com/us1/develop/phone-numbers/manage/verified
- ⚠️ Currently using Trial account - number must be verified first

**Test Steps:**

1. Run test script:
   ```bash
   python test_twilio_trigger.py
   ```
2. Backend sẽ gửi:

   - 📱 SMS đến +84399354603
   - ☎️ Voice call đến +84399354603

3. Check điện thoại để xem:
   - SMS content: "🚨 VibraGuard ALERT! Device: ESP32_CUA_SO_01, AI Confidence: 98.2%"
   - Voice call với message tự động

### D. Test Full ESP32 Integration

1. Flash ESP32 với firmware mới nhất
2. ESP32 sẽ gửi data đến: `34.87.133.103:1883` (MQTT)
3. Topic: `vibra_guard/sensor`
4. Format:
   ```json
   {
     "deviceId": "ESP32_CUA_SO_01",
     "timestamp": 1761900000000,
     "sensorValue": 95,
     "aiTriggered": true,
     "confidence": 0.95,
     "scores": {
       "attack": 95.0,
       "normal": 3.5,
       "noise": 1.5
     }
   }
   ```

---

## 📊 EXPECTED BEHAVIOR

### When Attack Detected (confidence >= 80%):

1. ✅ Event saved to PostgreSQL database
2. ✅ WebSocket alert sent to all connected clients
3. ✅ Twilio SMS sent to +84399354603
4. ✅ Twilio Voice call to +84399354603
5. ✅ Log entries created in backend

### WebSocket Alert Format:

```json
{
  "eventId": 11,
  "deviceId": "ESP32_CUA_SO_01",
  "timestamp": "2025-10-31T08:51:08.349611904",
  "severity": "WARNING",
  "sensorValue": 95,
  "confidence": 0.95,
  "aiTriggered": true
}
```

---

## 🔗 Quick Access URLs

| Service        | URL                                          | Status |
| -------------- | -------------------------------------------- | ------ |
| Backend API    | http://34.87.133.103:8080                    | ✅     |
| Backend Status | http://34.87.133.103:8080/api/control/status | ✅     |
| MQTT Broker    | mqtt://34.87.133.103:1883                    | ✅     |
| PostgreSQL     | 34.87.133.103:5432                           | ✅     |
| WebSocket      | ws://34.87.133.103:8080/ws                   | ✅     |

---

## ⚠️ IMPORTANT NOTES

1. **Twilio Trial Limitation:**

   - SMS và Voice call chỉ gửi đến verified numbers
   - Verify số tại: https://console.twilio.com/us1/develop/phone-numbers/manage/verified
   - Hoặc nâng cấp lên Twilio paid account

2. **Flutter Mobile App:**

   - URL backend đã cập nhật trong code
   - Không cần thay đổi gì thêm
   - Chạy trực tiếp: `flutter run`

3. **Frontend React:**

   - Chưa được update lên branch này
   - Cần merge hoặc update URL nếu muốn test

4. **Feature Branch:**
   - Hiện đang ở: `feature/migrate_GCP`
   - Merge vào main sau khi test thành công

---

## ✅ SYSTEM READY FOR TESTING

Tất cả components đã sẵn sàng. Bạn có thể bắt đầu test:

1. ✅ Web interface (test_websocket_health.html)
2. ✅ Mobile app (Flutter)
3. ✅ Twilio SMS/Voice (sau khi verify số)
4. ✅ ESP32 hardware integration

**Next Steps:**

1. Verify phone number in Twilio Console
2. Test mobile app với Flutter
3. Test WebSocket với browser
4. Test ESP32 hardware nếu có
