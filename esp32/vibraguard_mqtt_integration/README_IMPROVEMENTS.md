# VibraGuard ESP32 - MQTT Integration (Cải Tiến)

## 🚀 Cải Tiến Kết Nối WiFi & MQTT

### ✨ Tính Năng Mới

#### 🔄 **Auto Reconnect WiFi**

- Tự động phát hiện mất kết nối WiFi
- Tự động reconnect mỗi 10 giây
- Hiển thị trạng thái kết nối real-time

#### 🔄 **Smart MQTT Reconnect**

- Chỉ reconnect MQTT khi WiFi đã ổn định
- Exponential backoff: 1s → 2s → 4s → ... → max 30s
- Kiểm tra MQTT connection mỗi 5 giây

#### 📊 **Status Monitoring**

- Theo dõi trạng thái WiFi và MQTT
- Logs chi tiết cho debugging
- Không gửi MQTT khi chưa kết nối

### 📋 Code Cải Tiến

#### Biến Mới

```cpp
// ===== BIẾN CHO RECONNECT =====
unsigned long lastWiFiCheck = 0;
unsigned long lastMQTTCheck = 0;
const unsigned long WIFI_CHECK_INTERVAL = 10000;   // 10 giây
const unsigned long MQTT_CHECK_INTERVAL = 5000;    // 5 giây
unsigned long mqttReconnectDelay = 1000;           // Delay ban đầu
const unsigned long MQTT_MAX_DELAY = 30000;        // Max 30 giây

// ===== TRẠNG THÁI HỆ THỐNG =====
bool wifiConnected = false;
bool mqttConnected = false;
```

#### Logic Mới Trong loop()

```cpp
void loop() {
    unsigned long currentTime = millis();

    // Kiểm tra WiFi mỗi 10 giây
    if (currentTime - lastWiFiCheck >= WIFI_CHECK_INTERVAL) {
        checkWiFiConnection();
        lastWiFiCheck = currentTime;
    }

    // Kiểm tra MQTT mỗi 5 giây (chỉ khi WiFi OK)
    if (wifiConnected && currentTime - lastMQTTCheck >= MQTT_CHECK_INTERVAL) {
        checkMQTTConnection();
        lastMQTTCheck = currentTime;
    }

    // Xử lý MQTT messages
    if (mqttConnected) {
        client.loop();
    }

    // ... phần còn lại của code
}
```

### 🔧 Hàm Mới

#### `checkWiFiConnection()`

```cpp
void checkWiFiConnection() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("\n📶 WiFi connection lost! Reconnecting...");
        wifiConnected = false;
        mqttConnected = false; // MQTT cũng mất khi WiFi mất
        connectWiFi();
    }
}
```

#### `checkMQTTConnection()`

```cpp
void checkMQTTConnection() {
    if (!client.connected()) {
        // Thử reconnect với exponential backoff
        if (client.connect(clientId.c_str())) {
            mqttConnected = true;
            mqttReconnectDelay = 1000; // Reset delay
        } else {
            mqttConnected = false;
            delay(mqttReconnectDelay);
            mqttReconnectDelay = min(mqttReconnectDelay * 2, MQTT_MAX_DELAY);
        }
    }
}
```

### 📊 Logs Cải Tiến

#### Trước (cũ):

```
🔄 Attempting MQTT connection... ❌ Failed, rc=-2 - Retrying in 5 seconds...
🔄 Attempting MQTT connection... ❌ Failed, rc=-2 - Retrying in 5 seconds...
```

#### Sau (mới):

```
📶 WiFi status confirmed
✅ MQTT status confirmed

⚠️ VIBRATION DETECTED!
📤 MQTT Message Sent:
   Topic: vibra_guard/sensor
   Payload: {"deviceId":"ESP32_CUA_SO_01","timestamp":12345,"sensorValue":1}
```

### 🛠️ Cách Test

1. **Upload code mới lên ESP32**
2. **Mở Serial Monitor**
3. **Ngắt WiFi router** → ESP32 sẽ tự động reconnect
4. **Bật lại WiFi** → ESP32 reconnect và MQTT hoạt động lại

### 📈 Performance Improvements

| Metric            | Trước    | Sau            | Cải thiện     |
| ----------------- | -------- | -------------- | ------------- |
| WiFi Reconnect    | Manual   | Auto (10s)     | ✅ Tự động    |
| MQTT Reconnect    | Fixed 5s | Smart backoff  | ✅ Thông minh |
| Connection Status | Không rõ | Real-time logs | ✅ Minh bạch  |
| Error Handling    | Basic    | Comprehensive  | ✅ Hoàn chỉnh |

### 🔧 Troubleshooting

#### ESP32 không reconnect WiFi

```cpp
// Kiểm tra:
1. Router có bị restart không?
2. ESP32 có trong vùng phủ sóng?
3. Thử reset ESP32 (nút RST)
```

#### MQTT reconnect chậm

```cpp
// Bình thường! Exponential backoff giúp:
- Tránh spam server
- Tiết kiệm pin
- Giảm network congestion
```

#### Logs không hiển thị status

```cpp
// Đợi 10-15 giây để system check
// Status sẽ hiển thị định kỳ
```

### 🎯 Kết Luận

Code cải tiến này làm cho VibraGuard **đáng tin cậy hơn** và **tự động phục hồi** khi gặp sự cố mạng. Hệ thống giờ đây có thể hoạt động 24/7 mà không cần can thiệp thủ công.

---

**Cập nhật:** 26/10/2025
**Version:** 1.1.0 (Improved Connectivity)
