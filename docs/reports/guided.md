Tuyệt vời. Đây là bản kế hoạch chi tiết cho dự án của bạn, tuân thủ các "best practice" và tích hợp các lựa chọn công nghệ bạn đã xác nhận (Mosquitto, Postgres, v.v.).

### Tên dự án: **VibraGuard**

(Ghép từ "Vibration" - Rung động và "Guard" - Người bảo vệ)

### Mục tiêu dự án

Xây dựng một hệ thống an ninh chống trộm full-stack, có khả năng phát hiện rung động tại chỗ (qua ESP32), hú còi cảnh báo tức thì, đồng thời gửi cảnh báo real-time đến giao diện Web (React) và Mobile (Flutter) thông qua một backend (Spring Boot) và broker (Mosquitto).

### Kiến trúc hệ thống

1.  **Device (ESP32):** Rung -\> Kêu còi -\> Gửi tin MQTT (topic `vibra_guard/sensor`).
2.  **Broker (Mosquitto):** Nhận và chuyển tiếp tin MQTT.
3.  **Backend (Spring Boot):**
    - Subcribe `vibra_guard/sensor` (để nhận dữ liệu).
    - Lưu sự kiện vào **PostgreSQL**.
    - Đẩy cảnh báo (qua WebSocket) đến Frontend.
    - Cung cấp REST API (lấy lịch sử, gửi lệnh).
    - Publish lệnh (MQTT) đến `vibra_guard/control/{id}`.
4.  **Frontend (React/Flutter):**
    - Nhận cảnh báo (qua WebSocket).
    - Gọi REST API (xem lịch sử, gửi lệnh).

---

### Giai đoạn 0: Dựng hạ tầng (Infrastructure)

**Mục tiêu:** Khởi chạy Broker và Database bằng Docker.
**Best Practice:** Sử dụng `docker-compose` để quản lý cả hai dịch vụ.

1.  **Tạo file `docker-compose.yml`:**

    ```yaml
    version: "3.8"
    services:
      # MQTT Broker
      mosquitto:
        image: eclipse-mosquitto:latest
        ports:
          - "1883:1883" # Port MQTT chính
        volumes:
          - mosquitto-data:/mosquitto/data
          - mosquitto-log:/mosquitto/log

      # Database
      postgres:
        image: postgres:14-alpine
        ports:
          - "5432:5432" # Port Postgres
        environment:
          - POSTGRES_USER=myuser
          - POSTGRES_PASSWORD=mypassword
          - POSTGRES_DB=vibraguard_db # Đặt tên DB theo dự án
        volumes:
          - postgres-data:/var/lib/postgresql/data

    volumes:
      mosquitto-data:
      mosquitto-log:
      postgres-data:
    ```

2.  **Khởi chạy:**
    Mở terminal, `cd` vào thư mục chứa file và chạy: `docker-compose up -d`
3.  **Kiểm tra:**
    - Dùng tool như DBeaver kết nối tới `localhost:5432` (user/pass/db ở trên).
    - Dùng tool như MQTT Explorer hoặc MQTTX kết nối tới `localhost:1883`.

---

### Giai đoạn 1: Lõi phần cứng (Hardware Core)

**Mục tiêu:** Thiết bị hoạt động độc lập (rung là kêu).

1.  **Lắp mạch (Best Practice):**

    - Nối chân `DO` (Digital Out) của SW-420 vào `GPIO 13` (ví dụ).
    - **Quan trọng (Mạch lái còi):**
      - Nối `GPIO 12` (ví dụ) qua điện trở 1K Ohm vào cực **Base (B)** của Transistor NPN (S8050, 2N2222).
      - Nối cực **Emitter (E)** của Transistor vào `GND` chung.
      - Nối cực **Collector (C)** của Transistor vào chân âm (-) của còi Buzzer.
      - Nối chân dương (+) của Buzzer vào nguồn (ví dụ 5V hoặc 3.3V).

2.  **Code (Arduino C++):**

    - Viết code `setup()` để set `GPIO 13` là `INPUT` và `GPIO 12` là `OUTPUT`.
    - Trong `loop()`, đọc `digitalRead(13)`. Nếu là `HIGH` (rung động), thì `digitalWrite(12, HIGH)`. Ngược lại `digitalWrite(12, LOW)`.

3.  **Kiểm thử:** Cấp nguồn cho ESP32. Rung cảm biến. Còi _phải_ kêu.

---

### Giai đoạn 2: Lõi Backend & DB (Backend Core)

**Mục tiêu:** Xây dựng API và logic để lưu trữ.

1.  **Thiết kế DB (PostgreSQL):**
    - Bảng `devices` (id, device_name, location, status)
    - Bảng `vibration_events` (id, device_id (khóa ngoại), event_timestamp)
2.  **Khởi tạo dự án Spring Boot (start.spring.io):**
    - Dependencies: `Spring Web`, `Spring Data JPA`, `PostgreSQL Driver`.
3.  **Cấu hình (`application.properties`):**
    ```properties
    spring.datasource.url=jdbc:postgresql://localhost:5432/vibraguard_db
    spring.datasource.username=myuser
    spring.datasource.password=mypassword
    spring.jpa.hibernate.ddl-auto=update
    ```
4.  **Tạo Model & Repository:**
    - Tạo 2 class Entity `Device` và `VibrationEvent` (dùng `@Entity`).
    - Tạo 2 interface `DeviceRepository` và `EventRepository` (extends `JpaRepository`).
5.  **Tạo REST API (Controller):**
    - Tạo `EventController` với `GET /api/events` (lấy danh sách sự kiện).
6.  **Kiểm thử:** Chạy Spring Boot. Dùng Postman gọi `GET /api/events` (sẽ trả về mảng rỗng `[]`).

---

### Giai đoạn 3: Tích hợp MQTT (Device-to-Backend)

**Mục tiêu:** Đưa dữ liệu từ ESP32 lên Database.

1.  **Backend (Spring Boot):**
    - Thêm dependency `spring-integration-mqtt`.
    - Tạo một `@Service` tên `MqttSubscriberService`.
    - Cấu hình service này để kết nối `tcp://localhost:1883` và subcribe topic `vibra_guard/sensor`.
    - Khi nhận được tin nhắn (payload):
      1.  Parse chuỗi JSON (dùng Jackson ObjectMapper).
      2.  Tạo đối tượng `VibrationEvent` mới.
      3.  Dùng `eventRepository.save(newEvent)` để lưu vào DB.
2.  **Device (ESP32):**
    - Thêm thư viện `PubSubClient`.
    - Thêm code kết nối Wi-Fi và MQTT (trỏ đến IP máy tính của bạn, port 1883).
    - Trong logic phát hiện rung (Giai đoạn 1), ngoài việc `digitalWrite(12, HIGH)`, thêm:
      ```cpp
      String payload = "{\"deviceId\": \"ESP32_CUA_SO_01\", \"timestamp\": " + String(millis()) + "}";
      client.publish("vibra_guard/sensor", payload.c_tr());
      ```
3.  **Kiểm thử (Quan trọng):**
    - Rung cảm biến.
    - Check log Spring Boot xem có nhận được tin MQTT không.
    - Check DB (bằng DBeaver) hoặc gọi API `GET /api/events` (bằng Postman) -\> Phải thấy sự kiện mới.

---

### Giai đoạn 4: Giao diện Real-time (Backend-to-Frontend)

**Mục tiêu:** Hiển thị cảnh báo ngay lập tức trên Web/App.

1.  **Backend (Spring Boot):**
    - Thêm dependency `spring-boot-starter-websocket`.
    - Tạo `WebSocketConfig` để kích hoạt STOMP.
    - Nâng cấp `MqttSubscriberService`: Sau khi `eventRepository.save(newEvent)`, dùng `SimpMessagingTemplate` để đẩy `newEvent` này đến topic WebSocket (ví dụ `/topic/alerts`).
2.  **Frontend (ReactJS & Flutter):**
    - **Tải lịch sử (Load):** Khi mở app/web, dùng `axios` (React) hoặc `http` (Flutter) gọi API `GET /api/events` để hiển thị các sự kiện cũ.
    - **Nhận Real-time (Subscribe):**
      - React: Dùng `SockJS` và `Stomp.js` để kết nối WebSocket của Spring Boot và subcribe `/topic/alerts`.
      - Flutter: Dùng `stomp_dart_client` để làm tương tự.
    - Khi nhận được tin nhắn mới qua WebSocket, cập nhật `state` để thêm sự kiện mới vào đầu danh sách (UI tự động cập nhật).
3.  **Kiểm thử:**
    - Mở Web/App. Rung cảm biến.
    - Còi kêu, và Web/App _phải_ hiển thị cảnh báo mới ngay lập tức mà không cần F5.

---

### Giai đoạn 5: Vòng lặp điều khiển (Frontend-to-Device)

**Mục tiêu:** Cho phép người dùng tắt còi từ xa.

1.  **Backend (Spring Boot):**
    - Tạo API mới, ví dụ `POST /api/control/{deviceId}` với body `{"command": "DISARM"}`.
    - Tạo `MqttPublisherService`. Khi API này được gọi, service sẽ publish tin nhắn JSON (ví dụ `{"command": "DISARM"}`) đến topic MQTT `vibra_guard/control/{deviceId}`.
2.  **Frontend (React & Flutter):**
    - Thêm nút "Tắt báo động" vào giao diện.
    - Khi nhấn nút, gọi API `POST /api/control/ESP32_CUA_SO_01` (ví dụ) với body.
3.  **Device (ESP32):**
    - Nâng cấp code: Trong `setup()`, subcribe topic `vibra_guard/control/ESP32_CUA_SO_01`.
    - Viết hàm `callback()` (hàm xử lý tin nhắn MQTT đến).
    - Trong hàm `callback()`, nếu nhận được tin nhắn chứa `"command": "DISARM"`:
      - `digitalWrite(12, LOW);` // Tắt còi Buzzer.
4.  **Kiểm thử (Toàn hệ thống):**
    1.  Rung cảm biến -\> Còi kêu, Web/App báo động.
    2.  Bấm nút "Tắt báo động" trên Web hoặc App.
    3.  Còi _phải_ ngừng kêu.
