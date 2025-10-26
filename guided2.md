Chào bạn, đây là kế hoạch và mục tiêu rõ ràng cho dự án **"VibraGuard AI"** (phiên bản nâng cấp) của bạn.

### Tên dự án: VibraGuard AI

### 1. Mục tiêu tổng thể

Mục tiêu của dự án là xây dựng một hệ thống an ninh IoT **thông minh** và **hoàn chỉnh** (full-stack). Hệ thống này sử dụng **AI tại biên (Edge AI)** để phân tích rung động tại chỗ, loại bỏ báo động giả và chỉ kích hoạt cảnh báo (còi tại chỗ, thông báo qua web/app) khi phát hiện một vụ tấn công thực sự.

---

### 2. Các mục tiêu chi tiết (Key Objectives)

1.  **Chống Báo động giả (Core Objective):** Đây là mục tiêu đổi mới lớn nhất.

    - Thay thế cảm biến ON/OFF (SW-420) bằng cảm biến gia tốc 3 trục (MPU-6050).
    - Sử dụng TinyML (thông qua Edge Impulse) để huấn luyện một mô hình AI chạy _trực tiếp trên ESP32_.
    - **Kết quả:** Hệ thống phải tự phân biệt được "tấn công" (khoan, đập, cạy) với "rung động bình thường" (gõ cửa, xe tải, mưa).

2.  **Tích hợp Full-Stack (Backend Integration):**

    - Đảm bảo thiết bị ESP32-C3 (đã có AI) kết nối và gửi tin nhắn MQTT (đến Mosquitto) _chỉ khi_ AI xác nhận là "tấn công".
    - Backend (Spring Boot) phải nhận được tin nhắn này và lưu vào CSDL (PostgreSQL).

3.  **Hoàn thiện Giao diện Người dùng (Frontend Completion):**

    - Hoàn thành **bắt buộc** cả Web (React) và App (Flutter) theo yêu cầu đề bài.
    - Các giao diện này phải có khả năng nhận cảnh báo real-time (qua WebSocket).

4.  **Điều khiển Hai chiều (Remote Control):**
    - Người dùng phải có khả năng tắt báo động (DISARM) từ Web hoặc App.
    - Lệnh này phải được gửi qua Backend (REST API) -> MQTT -> đến ESP32 để tắt còi.

---

### 3. Kế hoạch hành động 4 giai đoạn

Đây là lộ trình "best practice" để hoàn thành mục tiêu trên.

#### Giai đoạn 1: Xây dựng "Bộ não AI" (Core AI)

- **Mục tiêu:** Tạo ra mô hình AI phân loại rung động.
- **Hành động:**
  1.  Nối dây ESP32 + MPU-6050.
  2.  Sử dụng nền tảng **Edge Impulse** để thu thập dữ liệu rung động (mô phỏng `attack`, `normal`, `noise`).
  3.  Huấn luyện mô hình AI trên web Edge Impulse.
  4.  Tải mô hình (dưới dạng thư viện Arduino) về máy.

#### Giai đoạn 2: Tích hợp AI lên Thiết bị (Device AI)

- **Mục tiêu:** Biến ESP32 thành một thiết bị thông minh, tự ra quyết định.
- **Hành động:**
  1.  Viết code **`vibraguard_ai_integration.ino`** (như bản tôi đã cung cấp).
  2.  Code này sẽ đọc MPU-6050, đưa dữ liệu vào "não" AI.
  3.  Dùng kết quả của AI (`attack_score`) để quyết định kích hoạt báo động.
  4.  Test độc lập: Đảm bảo mô phỏng "attack" thì còi kêu, "normal" thì còi im lặng.

#### Giai đoạn 3: Kết nối Toàn hệ thống (Full-Stack Connection)

- **Mục tiêu:** Liên kết thiết bị AI với backend đã có.
- **Hành động:**
  1.  Tích hợp code WiFi và MQTT (từ bản SW-420 cũ) vào code AI ở Giai đoạn 2.
  2.  Khi AI phát hiện "attack", ESP32 sẽ gửi tin nhắn MQTT (lên topic `vibra_guard/sensor`).
  3.  Backend (Spring Boot) nhận tin, lưu vào Postgres.
  4.  Backend đẩy tin qua WebSocket (tới topic `/topic/alerts`).

#### Giai đoạn 4: Hoàn thiện Giao diện & Điều khiển (UI/UX)

- **Mục tiêu:** Hoàn thành yêu cầu đề bài (Web/App) và vòng lặp điều khiển.
- **Hành động:**
  1.  Xây dựng Web (React) và App (Flutter).
  2.  Kết nối Web/App vào WebSocket (để nhận cảnh báo ở GĐ 3).
  3.  Tạo nút "DISARM" trên Web/App, gọi API Spring Boot.
  4.  Spring Boot publish lệnh "DISARM" lên MQTT (topic `vibra_guard/control/...`).
  5.  ESP32 (trong `mqttCallback()`) nhận lệnh và tắt còi.
      Chào bạn, đây là bản kế hoạch tổng hợp **đầy đủ và duy nhất** để bạn thực hiện dự án **VibraGuard AI** (sử dụng MPU-6050 và TinyML). Bạn không cần quay lại tìm bất cứ thông tin nào.

Kế hoạch này bao gồm 4 phần: Nối dây, Thu thập dữ liệu, Huấn luyện AI, và Code tích hợp cuối cùng.

---

### Phần 1: Sơ đồ Nối dây (Hardware)

Bạn sẽ sử dụng I2C (cho MPU-6050) và 1 chân Digital (cho còi).

- **Cảm biến MPU-6050:**

  - `VCC` (cảm biến) -\> **3V3(OUT)** (trên ESP32)
  - `GND` (cảm biến) -\> **G** (trên ESP32)
  - `SDA` (cảm biến) -\> **GP4** (chân số `4` trên ESP32)
  - `SCL` (cảm biến) -\> **GP5** (chân số `5` trên ESP32)

- **Mạch Còi (Transistor S8050):**

  - `GP1` (chân số `1` trên ESP32) -\> Điện trở 1K Ohm -\> Chân **Base (B)** (chân giữa S8050)
  - `G` (chân `GND` trên ESP32) -\> Chân **Emitter (E)** (chân phải S8050)
  - `5V` (trên ESP32) -\> Chân dương (`+`) của Còi Buzzer
  - Chân âm (`-`) của Còi Buzzer -\> Chân **Collector (C)** (chân trái S8050)

---

### Phần 2: Thu thập Dữ liệu (Dạy AI)

Mục tiêu của bước này là "ghi âm" lại các kiểu rung động để dạy cho AI.

1.  **Cài đặt:**

    - Trong Arduino IDE, cài 2 thư viện: `Adafruit MPU6050` và `Adafruit BusIO`.
    - Trên máy tính, cài `Edge Impulse CLI` (nếu chưa): `npm install -g edge-impulse-cli`

2.  **Nạp code Thu thập:** Nạp code sau vào ESP32. Code này chỉ dùng để _gửi dữ liệu thô_ lên máy tính.

    **File: `data_forwarder.ino`**

    ```cpp
    #include <Adafruit_MPU6050.h>
    #include <Adafruit_Sensor.h>
    #include <Wire.h>

    Adafruit_MPU6050 mpu;
    int sample_rate_ms = 100; // Thu thập 10 mẫu/giây (1000ms / 100ms)

    void setup() {
        Serial.begin(115200);
        // Khởi tạo I2C với SDA=GP4, SCL=GP5
        Wire.begin(4, 5);

        if (!mpu.begin()) {
            Serial.println("Loi! Khong tim thay MPU6050.");
            while (1) { delay(10); }
        }
        Serial.println("MPU6050 san sang. Bat dau stream du lieu...");
    }

    void loop() {
        sensors_event_t a, g, temp;
        mpu.getEvent(&a, &g, &temp);

        // In 3 trục X, Y, Z theo định dạng Edge Impulse
        Serial.print(a.acceleration.x);
        Serial.print(",");
        Serial.print(a.acceleration.y);
        Serial.print(",");
        Serial.println(a.acceleration.z);

        delay(sample_rate_ms);
    }
    ```

3.  **Thu thập:**

    - Gắn ESP32 (đã nạp code) vào cửa.
    - Mở terminal (trên máy tính) và chạy: `edge-impulse-data-forwarder`
    - Đăng nhập và chọn dự án.
    - Trên web Edge Impulse, vào "Data acquisition", đặt nhãn (Label) và nhấn "Start sampling":
      - **Nhãn `normal` (3-5 phút):** Gõ cửa, đóng cửa, vỗ tay, bật nhạc.
      - **Nhãn `attack` (3-5 phút):** Mô phỏng khoan, đập (búa cao su), cạy cửa.
      - **Nhãn `noise` (1-2 phút):** Không làm gì cả.

---

### Phần 3: Huấn luyện AI (Trên Web Edge Impulse)

Sau khi có dữ liệu, bạn huấn luyện "bộ não" trên web.

1.  **Create Impulse (Tạo não):**
    - **Input block:** `Time-series data` (3-axis).
    - **Processing block:** `Spectral Analysis` (Phân tích quang phổ).
    - **Learning block:** `Classification (Keras)` (AI phân loại).
    - Nhấn "Save Impulse".
2.  **Generate features:** Vào tab "Spectral features" -\> nhấn "Generate features".
3.  **Train model:** Vào tab "NN Classifier" -\> nhấn "Start training". Chờ vài phút.
4.  **Deployment (Tải "não" về):**
    - Vào tab "Deployment".
    - Chọn "Arduino library".
    - Nhấn "Build" và tải file `.zip` về.
5.  **Import vào Arduino:** Mở Arduino IDE -\> `Sketch` -\> `Include Library` -\> `Add .ZIP Library...` -\> Chọn file `.zip` bạn vừa tải.

---

### Phần 4: Code Tích hợp AI (Bản Hoàn Chỉnh)

Đây là code cuối cùng, kết hợp mọi thứ. Code này sẽ **thay thế hoàn toàn** code SW-420 của bạn. Bạn chỉ cần copy, dán, và nạp.

**Hãy thay `<Your-Project-Name-Here>` bằng tên thư viện AI bạn vừa import (ví dụ: `ei_vibraguard_ai_arduino`).**

**File: `vibraguard_ai_integration.ino`**

```cpp
/*
 * =======================================================
 * VibraGuard AI - FINAL INTEGRATION CODE
 * =======================================================
 * - Board: ESP32-C3 Super Mini
 * - Sensor: MPU-6050 (I2C: GP4, GP5)
 * - Buzzer: GP1 (qua Transistor)
 * - AI: Edge Impulse TinyML
 * - Comms: WiFi + MQTT
 */

// --- Thư viện Mạng ---
#include <WiFi.h>
#include <PubSubClient.h>

// --- Thư viện Cảm biến & AI ---
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <ei-vibraguard-ai-arduino.h> // <--- THAY TÊN NÀY! (Tên file .h trong thư viện .zip)

// ===== CẤU HÌNH WIFI =====
const char *ssid = "LE HUNG";       // Tên WiFi của bạn
const char *password = "123456789"; // Mật khẩu WiFi

// ===== CẤU HÌNH MQTT =====
const char *mqtt_server = "192.168.1.12"; // IP máy tính chạy Mosquitto
const int mqtt_port = 1883;
const char *mqtt_topic = "vibra_guard/sensor";
const char *device_id = "ESP32_CUA_SO_01";

// ===== ĐỊNH NGHĨA CHÂN =====
const int BUZZER_PIN = 1; // Điều khiển Buzzer qua Transistor (Nối vào GP1)
// Chân I2C (SDA=4, SCL=5) được định nghĩa trong Wire.begin()

// ===== BIẾN TRẠNG THÁI =====
bool isAlarmActive = false; // Trạng thái báo động
WiFiClient espClient;
PubSubClient client(espClient);

// ===== BIẾN CẢM BIẾN & AI =====
Adafruit_MPU6050 mpu;
// Các biến này lấy từ code ví dụ của Edge Impulse
static bool use_continuous_mode = false;
float buffer[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE] = { 0 };
size_t buf_idx = 0;

// ----- KHAI BÁO HÀM -----
void connectWiFi();
void reconnectMQTT();
void sendVibrationAlert();
void mqttCallback(char *topic, byte *payload, unsigned int length);

// =======================
//        SETUP
// =======================
void setup()
{
    Serial.begin(115200);
    // Khởi tạo I2C với SDA=GP4, SCL=GP5
    Wire.begin(4, 5);

    Serial.println("\n\n========================================");
    Serial.println("VibraGuard AI System - Starting...");
    Serial.println("========================================");

    // Khởi tạo MPU6050
    if (!mpu.begin()) {
        Serial.println("Loi! Khong tim thay MPU6050.");
        while (1) { delay(10); }
    }
    Serial.println("MPU6050 AI Sensor detected!");

    // Cấu hình còi
    pinMode(BUZZER_PIN, OUTPUT);
    digitalWrite(BUZZER_PIN, LOW); // Đảm bảo còi tắt

    // Kết nối mạng
    connectWiFi();
    client.setServer(mqtt_server, mqtt_port);
    client.setCallback(mqttCallback);

    Serial.println("System Ready - AI Monitoring started...");
}

// =======================
//        MAIN LOOP
// =======================
void loop()
{
    // 1. Duy trì kết nối MQTT
    if (!client.connected()) {
        reconnectMQTT();
    }
    client.loop(); // Rất quan trọng, phải gọi liên tục

    // 2. Đọc dữ liệu cảm biến
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    // 3. Nạp dữ liệu vào bộ đệm của AI
    buffer[buf_idx++] = a.acceleration.x;
    buffer[buf_idx++] = a.acceleration.y;
    buffer[buf_idx++] = a.acceleration.z;

    // 4. Khi bộ đệm đầy, yêu cầu AI phân tích
    if (buf_idx >= EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE) {
        buf_idx = 0; // Reset bộ đệm

        // Tạo tín hiệu từ bộ đệm
        signal_t signal;
        numpy::signal_from_buffer(buffer, EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE, &signal);

        // Chạy phân loại (AI "suy nghĩ")
        ei_impulse_result_t result = { 0 };
        run_classifier(&signal, &result, false);

        // 5. Lấy kết quả phân loại
        float normal_score = 0.0;
        float attack_score = 0.0;
        float noise_score = 0.0;

        for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++) {
            if (strcmp(result.classification[ix].label, "normal") == 0) {
                normal_score = result.classification[ix].value;
            }
            if (strcmp(result.classification[ix].label, "attack") == 0) {
                attack_score = result.classification[ix].value;
            }
            if (strcmp(result.classification[ix].label, "noise") == 0) {
                noise_score = result.classification[ix].value;
            }
        }

        // In kết quả ra Serial để debug
        Serial.printf("AI Result: Normal(%.2f) - Attack(%.2f) - Noise(%.2f)\n",
                      normal_score, attack_score, noise_score);

        // 6. Ra quyết định THÔNG MINH
        // Ngưỡng: Chắc chắn > 80% là "tấn công" VÀ nó không phải là "bình thường"
        if (attack_score > 0.80 && attack_score > normal_score && !isAlarmActive) {

            // --- KÍCH HOẠT BÁO ĐỘNG ---
            Serial.println("\n⚠️  AI DETECTED ATTACK! ALARM ACTIVATED!");
            isAlarmActive = true;
            sendVibrationAlert(); // Gửi MQTT lên backend
        }
    }

    // 7. Điều khiển còi (Logic "chốt")
    // Còi sẽ kêu liên tục cho đến khi nhận được lệnh DISARM
    if (isAlarmActive) {
        digitalWrite(BUZZER_PIN, HIGH);
    } else {
        digitalWrite(BUZZER_PIN, LOW);
    }

    delay(10); // Delay nhỏ cho hệ thống
}

// =======================
//  CÁC HÀM MẠNG (Giữ nguyên)
// =======================

void connectWiFi() {
    Serial.print("\n🔌 Connecting to WiFi: ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        delay(500);
        Serial.print(".");
        attempts++;
    }
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\n✅ WiFi Connected!");
        Serial.print("📡 IP Address: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("\n❌ WiFi Connection Failed!");
    }
}

void reconnectMQTT() {
    while (!client.connected()) {
        Serial.print("\n🔄 Attempting MQTT connection...");
        String clientId = "ESP32-" + String(device_id);
        if (client.connect(clientId.c_str())) {
            Serial.println(" ✅ Connected to MQTT Broker!");
            String controlTopic = "vibra_guard/control/" + String(device_id);
            client.subscribe(controlTopic.c_str());
            Serial.print("📥 Subscribed to: ");
            Serial.println(controlTopic);
        } else {
            Serial.print(" ❌ Failed, rc=");
            Serial.print(client.state());
            Serial.println(" - Retrying in 5 seconds...");
            delay(5000);
        }
    }
}

void sendVibrationAlert() {
    String payload = "{";
    payload += "\"deviceId\":\"" + String(device_id) + "\",";
    payload += "\"timestamp\":" + String(millis()) + ",";
    payload += "\"sensorValue\":1,";
    payload += "\"ai_triggered\":true"; // Thêm dữ liệu cho "xịn"
    payload += "}";

    boolean success = client.publish(mqtt_topic, payload.c_str());
    if (success) {
        Serial.println("📤 MQTT Message Sent (AI TRIGGER):");
        Serial.println("   Payload: " + payload);
    } else {
        Serial.println("❌ Failed to send MQTT message!");
    }
}

void mqttCallback(char *topic, byte *payload, unsigned int length) {
    Serial.print("\n📨 Message received [");
    Serial.print(topic);
    Serial.print("]: ");
    String message;
    for (int i = 0; i < length; i++) {
        message += (char)payload[i];
    }
    Serial.println(message);

    // Xử lý lệnh DISARM (Giai đoạn 5)
    if (message.indexOf("DISARM") >= 0) {
        Serial.println("🔕 Disarm command received - ALARM OFF");
        isAlarmActive = false; // Tắt trạng thái báo động
        digitalWrite(BUZZER_PIN, LOW); // Tắt còi ngay lập tức
    }
}
```
