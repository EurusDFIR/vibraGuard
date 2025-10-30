# 🔧 WEBSOCKET TROUBLESHOOTING GUIDE

## Vấn đề hiện tại

Frontend (React) kết nối được với Backend API nhưng **WebSocket không hoạt động đúng cách**.

### Triệu chứng:

```
🔌 Connecting to WebSocket...
Opening Web Socket...
Web Socket Opened...
>>> CONNECT
accept-version:1.1,1.0
heart-beat:10000,10000
```

WebSocket mở nhưng **không nhận được CONNECTED frame** từ server.

---

## ✅ Những gì đã hoạt động

1. ✅ **REST API hoạt động bình thường**

   ```bash
   curl http://34.87.133.103:8080/api/control/status
   # {"devicesCount":0,"systemStatus":"ARMED","timestamp":...}
   ```

2. ✅ **Backend có WebSocket configuration**

   - File: `backend/src/main/java/com/vibraguard/config/WebSocketConfig.java`
   - Endpoint: `/ws`
   - SockJS enabled với `setAllowedOriginPatterns("*")`

3. ✅ **Dependencies đầy đủ trong pom.xml**

   - `spring-boot-starter-websocket`
   - MQTT integration
   - WebSocket message broker

4. ✅ **Frontend code đúng**
   ```javascript
   const socket = new SockJS("http://34.87.133.103:8080/ws");
   const client = Stomp.over(socket);
   ```

---

## 🔍 Nguyên nhân có thể

### 1. **Backend chưa được rebuild sau khi thêm WebSocket config**

- JAR file trên GCP có thể là version cũ (không có WebSocket)
- Cần rebuild và redeploy

### 2. **CORS/Firewall issue**

- WebSocket cần upgrade HTTP connection
- Có thể bị block ở layer network

### 3. **SockJS endpoint không đúng**

- Backend config: `registry.addEndpoint("/ws").withSockJS()`
- Frontend connect: `new SockJS('/ws')`
- Có thể thiếu SockJS transport protocol

### 4. **Missing WebSocket dependency trong Docker image**

- JAR có thể không include đủ dependencies

---

## 🛠️ Các bước khắc phục

### Bước 1: Kiểm tra Backend version trên GCP

```bash
# SSH vào GCP
gcloud compute ssh vibraguard-server --zone=asia-southeast1-b

# Kiểm tra khi nào rebuild lần cuối
docker exec vibraguard-prod_backend_1 ls -lh /app/app.jar

# Xem logs chi tiết hơn
docker logs vibraguard-prod_backend_1 -f
```

### Bước 2: Rebuild Backend với WebSocket

```bash
# Trên máy local - rebuild JAR
cd backend
mvn clean package -DskipTests

# Upload lên GCP (JAR mới khoảng 60-70MB)
gcloud compute scp target/vibraguard-backend-0.0.1-SNAPSHOT.jar vibraguard-server:~/app.jar --zone=asia-southeast1-b

# SSH vào GCP và restart
gcloud compute ssh vibraguard-server --zone=asia-southeast1-b
cd ~
docker compose down backend
docker compose up -d backend
docker logs vibraguard-prod_backend_1 -f
```

### Bước 3: Test WebSocket với tool

Mở file `test_websocket.html` trong trình duyệt:

```bash
# Windows
start test_websocket.html

# Hoặc dùng VS Code Live Server
# Right-click -> Open with Live Server
```

Quan sát logs trong tool để xem:

- ✅ SockJS connection opened
- ✅ STOMP connected
- ✅ Subscribed to /topic/alerts

### Bước 4: Kiểm tra trong code Backend

Đảm bảo các file sau tồn tại và đúng:

**WebSocketConfig.java**

```java
@Configuration
@EnableWebSocketMessageBroker
public class WebSocketConfig implements WebSocketMessageBrokerConfigurer {

    @Override
    public void configureMessageBroker(MessageBrokerRegistry config) {
        config.enableSimpleBroker("/topic");
        config.setApplicationDestinationPrefixes("/app");
    }

    @Override
    public void registerStompEndpoints(StompEndpointRegistry registry) {
        registry.addEndpoint("/ws")
                .setAllowedOriginPatterns("*")
                .withSockJS();
    }
}
```

**MqttSubscriberService.java** - Phần gửi WebSocket alert

```java
private void sendWebSocketAlert(SensorDataDTO sensorData, VibrationEvent event) {
    try {
        AlertDTO alert = AlertDTO.builder()
            .eventId(event.getId())
            .deviceId(sensorData.getDeviceId())
            // ... other fields
            .build();

        messagingTemplate.convertAndSend("/topic/alerts", alert);
        log.info("📡 WebSocket alert sent to /topic/alerts");
    } catch (Exception e) {
        log.error("❌ Failed to send WebSocket alert", e);
    }
}
```

### Bước 5: Test End-to-End

```bash
# 1. Chạy Frontend
cd frontend
npm run dev
# Mở http://localhost:5173

# 2. Gửi test data từ ESP32 hoặc MQTT test tool
# Hoặc dùng mosquitto_pub
mosquitto_pub -h 34.87.133.103 -t "vibraguard/sensor_data" -m '{
  "deviceId": "ESP32_001",
  "vibrationType": "ABNORMAL",
  "severity": "HIGH",
  "aiConfidence": 95.5,
  "timestamp": "2025-01-31T12:00:00Z",
  "ax": 2.5, "ay": 1.2, "az": 9.8,
  "gx": 0.1, "gy": 0.2, "gz": 0.05
}'

# 3. Xem trong Frontend có nhận được alert không
```

---

## 🐛 Debug với Browser DevTools

### Mở Chrome DevTools (F12)

**1. Network Tab:**

- Filter: `WS` (WebSocket)
- Xem connection status
- Xem frames được gửi/nhận

**2. Console Tab:**

```javascript
// Test manual connection
const socket = new SockJS("http://34.87.133.103:8080/ws");
socket.onopen = () => console.log("✅ Connected");
socket.onerror = (e) => console.error("❌ Error:", e);
socket.onclose = () => console.log("👋 Closed");
```

**3. Application Tab:**

- Check for Service Workers
- Clear cache nếu cần

---

## 📊 Kiểm tra Backend Logs Pattern

Khi WebSocket hoạt động đúng, logs sẽ hiển thị:

```
INFO o.s.w.s.c.WebSocketMessageBrokerStats : WebSocket session count: 1
INFO o.s.w.s.c.WebSocketMessageBrokerStats : STOMP session count: 1
```

Nếu không thấy logs này → Backend chưa nhận được WebSocket connection.

---

## 🚨 Common Issues

### Issue 1: CORS Error

```
Access to XMLHttpRequest at 'http://34.87.133.103:8080/ws/info'
from origin 'http://localhost:5173' has been blocked by CORS policy
```

**Fix:** Đã có `setAllowedOriginPatterns("*")` trong WebSocketConfig

### Issue 2: 404 Not Found on /ws

```
GET http://34.87.133.103:8080/ws/info 404 (Not Found)
```

**Fix:** Backend chưa có WebSocket endpoint, cần rebuild.

### Issue 3: Connection timeout

```
Whoops! Lost connection to http://34.87.133.103:8080/ws
```

**Fix:**

- Check firewall rules
- Check Docker network configuration
- Ensure backend is running

### Issue 4: SockJS transport error

```
WebSocket connection to 'ws://34.87.133.103:8080/ws/...' failed
```

**Fix:** SockJS sẽ fallback sang XHR/polling. Check nếu vẫn không hoạt động.

---

## ✅ Verification Checklist

- [ ] Backend JAR có WebSocket dependency
- [ ] WebSocketConfig.java tồn tại và đúng cú pháp
- [ ] Backend đã được rebuild sau khi thêm WebSocket
- [ ] Docker container restart với JAR mới
- [ ] Firewall cho phép port 8080 (đã có)
- [ ] Frontend kết nối đúng URL
- [ ] Browser DevTools không có lỗi CORS
- [ ] test_websocket.html tool hiển thị "Connected"
- [ ] MQTT có thể gửi message thành công
- [ ] Backend logs hiển thị "WebSocket alert sent"

---

## 🎯 Next Steps

1. **Rebuild Backend** - Đảm bảo WebSocket config được compile
2. **Redeploy to GCP** - Upload JAR mới
3. **Test với tool** - Dùng `test_websocket.html`
4. **Test E2E** - Gửi data từ MQTT → Backend → Frontend
5. **Monitor logs** - Xem backend có send alerts không

---

## 📝 Quick Command Reference

```bash
# Rebuild backend
cd backend && mvn clean package -DskipTests

# Upload to GCP
gcloud compute scp target/*.jar vibraguard-server:~/app.jar --zone=asia-southeast1-b

# Restart backend container
gcloud compute ssh vibraguard-server --zone=asia-southeast1-b
docker compose restart backend

# Watch logs
docker logs vibraguard-prod_backend_1 -f | grep -i "websocket\|stomp\|alert"

# Test MQTT
mosquitto_pub -h 34.87.133.103 -t "vibraguard/sensor_data" -m '{"deviceId":"TEST","vibrationType":"ABNORMAL","severity":"HIGH","aiConfidence":99}'
```

---

_Last updated: 2025-10-31_
_Status: Investigating WebSocket connection issue_
