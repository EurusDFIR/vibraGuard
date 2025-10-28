/*
 * =======================================================
 * VibraGuard AI - FINAL INTEGRATION CODE
 * =======================================================
 * Version: 1.0 - Production Ready
 * Accuracy: 95.1% (Attack: 93.1%, Normal: 95.8%, Noise: 98.8%)
 *
 * Hardware:
 * - Board: ESP32-C3 Super Mini
 * - Sensor: MPU-6050 (I2C: GP8, GP9)
 * - Buzzer: GP1 (via NPN Transistor S8050)
 * - AI: Edge Impulse TinyML (1ms inference, 1.4KB RAM)
 *
 * Features:
 * - Edge AI: 95.1% accuracy attack detection
 * - MQTT: Real-time alerts to backend
 * - WiFi: Auto-reconnect with status monitoring
 * - Remote Control: DISARM via MQTT commands
 * - Smart Buzzer: Continuous alarm until disarmed
 *
 * Wiring:
 * MPU-6050:
 *   VCC -> 3V3, GND -> GND
 *   SDA -> GP8, SCL -> GP9
 * Buzzer Circuit:
 *   GP1 -> 1K Ohm -> Transistor Base
 *   Transistor Emitter -> GND
 *   Transistor Collector -> Buzzer(-)
 *   Buzzer(+) -> 5V
 */

// ===== THÔNG SỐ CẤU HÌNH =====
// Thay đổi các giá trị này theo môi trường của bạn

// WiFi
const char *WIFI_SSID = "LE HUNG";
const char *WIFI_PASSWORD = "123456789";

// MQTT Broker
const char *MQTT_SERVER = "192.168.1.11"; // IP máy chạy Mosquitto
const int MQTT_PORT = 1883;
const char *DEVICE_ID = "ESP32_CUA_SO_01";

// Ngưỡng AI (0.0 - 1.0)
const float ATTACK_THRESHOLD = 0.85; // 85% confidence để kích hoạt alarm
const float CERTAINTY_MARGIN = 0.1;  // Attack phải cao hơn Normal ít nhất 10%

// Buzzer
const int BUZZER_PIN = 1;
const unsigned long BUZZER_MAX_DURATION = 300000; // 5 phút tự động tắt

// MQTT Topics
const char *TOPIC_SENSOR = "vibra_guard/sensor";
const char *TOPIC_CONTROL_PREFIX = "vibra_guard/control/";

// ===== KẾT THÚC CẤU HÌNH =====

#include <WiFi.h>
#include <PubSubClient.h>
#include <MPU6050_tockn.h>
#include <Wire.h>

// Import thư viện AI từ Edge Impulse
#include <vibraGuard_inferencing.h>

// ===== BIẾN TOÀN CỤC =====
WiFiClient espClient;
PubSubClient client(espClient);
MPU6050 mpu(Wire);

// AI Buffer
static float buffer[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE] = {0};
static size_t buf_idx = 0;

// System State
bool isSystemArmed = false; // ✅ Thêm biến để kiểm soát ARM/DISARM
bool isAlarmActive = false;
bool wifiConnected = false;
bool mqttConnected = false;
unsigned long alarmStartTime = 0;

// Timing
unsigned long lastWiFiCheck = 0;
unsigned long lastMQTTCheck = 0;
unsigned long lastAIProcess = 0;
const unsigned long WIFI_CHECK_INTERVAL = 10000;
const unsigned long MQTT_CHECK_INTERVAL = 5000;
const unsigned long AI_PROCESS_INTERVAL = 100; // 10 lần/giây để đọc sensor

// Statistics
unsigned long totalInferences = 0;
unsigned long attacksDetected = 0;
unsigned long falseAlarms = 0;

// ===== KHAI BÁO HÀM =====
void connectWiFi();
void checkWiFiConnection();
void reconnectMQTT();
void checkMQTTConnection();
void sendVibrationAlert(float attack_score, float normal_score, float noise_score);
void mqttCallback(char *topic, byte *payload, unsigned int length);
void processAI();
void controlBuzzer();
void printSystemStatus();

// =======================
//        SETUP
// =======================
void setup()
{
    Serial.begin(115200);
    delay(1000);

    Serial.println("\n\n========================================");
    Serial.println("VibraGuard AI - Production Version 1.0");
    Serial.println("========================================");
    Serial.println("AI Model: 95.1% Accuracy");
    Serial.println("  Attack:  93.1%");
    Serial.println("  Normal:  95.8%");
    Serial.println("  Noise:   98.8%");
    Serial.println("========================================");

    // Cấu hình Buzzer
    pinMode(BUZZER_PIN, OUTPUT);
    digitalWrite(BUZZER_PIN, LOW);
    Serial.println("✅ Buzzer configured");

    // Khởi tạo I2C với SDA=GP8, SCL=GP9
    Wire.begin(8, 9);
    Serial.println("✅ I2C initialized (SDA=8, SCL=9)");

    // Khởi tạo MPU6050
    Serial.print("🔄 Initializing MPU6050... ");
    mpu.begin();
    Serial.println("Done!");

    Serial.print("🔄 Calibrating Gyro (keep still)... ");
    mpu.calcGyroOffsets(true); // true = verbose
    Serial.println("Done!");

    // Kết nối WiFi
    connectWiFi();

    // Cấu hình MQTT
    String controlTopic = String(TOPIC_CONTROL_PREFIX) + String(DEVICE_ID);
    client.setServer(MQTT_SERVER, MQTT_PORT);
    client.setCallback(mqttCallback);
    client.setKeepAlive(60);

    // In thông tin hệ thống
    Serial.println("\n========================================");
    Serial.println("System Configuration:");
    Serial.printf("  Device ID: %s\n", DEVICE_ID);
    Serial.printf("  MQTT Server: %s:%d\n", MQTT_SERVER, MQTT_PORT);
    Serial.printf("  Attack Threshold: %.0f%%\n", ATTACK_THRESHOLD * 100);
    Serial.printf("  AI Buffer Size: %d samples\n", EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE);
    Serial.println("========================================");

    Serial.println("\n🎯 System Ready - AI Monitoring Active");
    Serial.println("Waiting for vibrations...\n");
}

// =======================
//        MAIN LOOP
// =======================
void loop()
{
    unsigned long currentTime = millis();

    // 1. Kiểm tra WiFi định kỳ
    if (currentTime - lastWiFiCheck >= WIFI_CHECK_INTERVAL)
    {
        checkWiFiConnection();
        lastWiFiCheck = currentTime;
    }

    // 2. Kiểm tra MQTT định kỳ
    if (wifiConnected && currentTime - lastMQTTCheck >= MQTT_CHECK_INTERVAL)
    {
        checkMQTTConnection();
        lastMQTTCheck = currentTime;
    }

    // 3. Xử lý MQTT messages
    if (mqttConnected)
    {
        client.loop();
    }

    // 4. Xử lý AI (đọc sensor + inference)
    if (currentTime - lastAIProcess >= AI_PROCESS_INTERVAL)
    {
        processAI();
        lastAIProcess = currentTime;
    }

    // 5. Điều khiển buzzer
    controlBuzzer();

    // 6. In status định kỳ (mỗi 30 giây)
    static unsigned long lastStatusPrint = 0;
    if (currentTime - lastStatusPrint >= 30000)
    {
        printSystemStatus();
        lastStatusPrint = currentTime;
    }
}

// =======================
//     WiFi FUNCTIONS
// =======================
void connectWiFi()
{
    Serial.print("\n🔌 Connecting to WiFi: ");
    Serial.println(WIFI_SSID);

    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 30)
    {
        delay(500);
        Serial.print(".");
        attempts++;
    }

    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.println("\n✅ WiFi Connected!");
        Serial.print("📡 IP Address: ");
        Serial.println(WiFi.localIP());
        Serial.print("📶 Signal Strength: ");
        Serial.print(WiFi.RSSI());
        Serial.println(" dBm");
        wifiConnected = true;
    }
    else
    {
        Serial.println("\n❌ WiFi Connection Failed!");
        wifiConnected = false;
    }
}

void checkWiFiConnection()
{
    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("\n⚠️  WiFi connection lost! Reconnecting...");
        wifiConnected = false;
        mqttConnected = false;
        connectWiFi();
    }
}

// =======================
//     MQTT FUNCTIONS
// =======================
void reconnectMQTT()
{
    if (!wifiConnected)
        return;

    Serial.print("\n🔄 Connecting to MQTT... ");
    String clientId = "ESP32-" + String(DEVICE_ID);
    String controlTopic = String(TOPIC_CONTROL_PREFIX) + String(DEVICE_ID);

    if (client.connect(clientId.c_str()))
    {
        Serial.println("✅ Connected!");

        // Subscribe to control topic
        if (client.subscribe(controlTopic.c_str()))
        {
            Serial.print("📥 Subscribed to: ");
            Serial.println(controlTopic);
            mqttConnected = true;
        }
        else
        {
            Serial.println("❌ Failed to subscribe!");
            mqttConnected = false;
        }
    }
    else
    {
        Serial.print("❌ Failed! rc=");
        Serial.println(client.state());
        mqttConnected = false;
    }
}

void checkMQTTConnection()
{
    if (!client.connected())
    {
        mqttConnected = false;
        reconnectMQTT();
    }
}

void sendVibrationAlert(float attack_score, float normal_score, float noise_score)
{
    if (!mqttConnected)
    {
        Serial.println("⚠️  Cannot send MQTT - Not connected");
        return;
    }

    String payload = "{";
    payload += "\"deviceId\":\"" + String(DEVICE_ID) + "\",";
    payload += "\"timestamp\":" + String(millis()) + ",";
    payload += "\"ai_triggered\":true,";
    payload += "\"scores\":{";
    payload += "\"attack\":" + String(attack_score, 3) + ",";
    payload += "\"normal\":" + String(normal_score, 3) + ",";
    payload += "\"noise\":" + String(noise_score, 3);
    payload += "},";
    payload += "\"confidence\":" + String(attack_score, 3);
    payload += "}";

    if (client.publish(TOPIC_SENSOR, payload.c_str()))
    {
        Serial.println("📤 MQTT Alert Sent:");
        Serial.println("   " + payload);
        attacksDetected++;
    }
    else
    {
        Serial.println("❌ MQTT publish failed!");
    }
}

void mqttCallback(char *topic, byte *payload, unsigned int length)
{
    Serial.print("\n📨 MQTT Message [");
    Serial.print(topic);
    Serial.print("]: ");

    String message;
    for (unsigned int i = 0; i < length; i++)
    {
        message += (char)payload[i];
    }
    Serial.println(message);

    // Xử lý lệnh DISARM
    if (message.indexOf("DISARM") >= 0 || message.indexOf("disarm") >= 0)
    {
        Serial.println("🔕 DISARM Command Received");
        isSystemArmed = false; // ✅ Tắt hệ thống
        isAlarmActive = false;
        digitalWrite(BUZZER_PIN, LOW);
        Serial.println("   Alarm deactivated!");
    }
    // Xử lý lệnh ARM
    else if (message.indexOf("ARM") >= 0 || message.indexOf("arm") >= 0)
    {
        Serial.println("🔔 ARM Command Received");
        isSystemArmed = true; // ✅ Kích hoạt hệ thống
        Serial.println("   System armed and monitoring");
    }
    // Xử lý lệnh STATUS
    else if (message.indexOf("STATUS") >= 0 || message.indexOf("status") >= 0)
    {
        printSystemStatus();
    }
}

// =======================
//     AI PROCESSING
// =======================
void processAI()
{
    // 1. Đọc dữ liệu cảm biến
    mpu.update();
    float accX = mpu.getAccX();
    float accY = mpu.getAccY();
    float accZ = mpu.getAccZ();

    // 2. Thêm vào buffer
    buffer[buf_idx++] = accX;
    buffer[buf_idx++] = accY;
    buffer[buf_idx++] = accZ;

    // 3. Khi buffer đầy, chạy AI inference
    if (buf_idx >= EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE)
    {
        buf_idx = 0;
        totalInferences++;

        // Tạo signal từ buffer
        signal_t signal;
        numpy::signal_from_buffer(buffer, EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE, &signal);

        // Chạy classifier
        ei_impulse_result_t result = {0};
        EI_IMPULSE_ERROR res = run_classifier(&signal, &result, false);

        if (res != EI_IMPULSE_OK)
        {
            Serial.printf("❌ AI Error: %d\n", res);
            return;
        }

        // Lấy kết quả
        float attack_score = 0.0;
        float normal_score = 0.0;
        float noise_score = 0.0;

        for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++)
        {
            if (strcmp(result.classification[ix].label, "attack") == 0)
            {
                attack_score = result.classification[ix].value;
            }
            else if (strcmp(result.classification[ix].label, "normal") == 0)
            {
                normal_score = result.classification[ix].value;
            }
            else if (strcmp(result.classification[ix].label, "noise") == 0)
            {
                noise_score = result.classification[ix].value;
            }
        }

        // Debug output
        Serial.printf("🧠 AI: Attack=%.2f%% | Normal=%.2f%% | Noise=%.2f%% | Time=%dms\n",
                      attack_score * 100, normal_score * 100, noise_score * 100,
                      result.timing.classification);

        // ✅ Quyết định kích hoạt alarm (CHỈ KHI ĐÃ ARM)
        if (isSystemArmed &&
            attack_score > ATTACK_THRESHOLD &&
            attack_score > (normal_score + CERTAINTY_MARGIN) &&
            !isAlarmActive)
        {
            Serial.println("\n🚨🚨🚨 ATTACK DETECTED! 🚨🚨🚨");
            Serial.printf("   Confidence: %.1f%%\n", attack_score * 100);
            Serial.printf("   Attack > Normal by %.1f%%\n",
                          (attack_score - normal_score) * 100);

            isAlarmActive = true;
            alarmStartTime = millis();
            sendVibrationAlert(attack_score, normal_score, noise_score);
        }
    }
}

// =======================
//   BUZZER CONTROL
// =======================
void controlBuzzer()
{
    if (isAlarmActive)
    {
        // Tự động tắt sau thời gian tối đa
        if (millis() - alarmStartTime >= BUZZER_MAX_DURATION)
        {
            Serial.println("⏰ Auto-disarm: Maximum duration reached");
            isAlarmActive = false;
            digitalWrite(BUZZER_PIN, LOW);
            return;
        }

        // Bật buzzer
        digitalWrite(BUZZER_PIN, HIGH);
    }
    else
    {
        // Tắt buzzer
        digitalWrite(BUZZER_PIN, LOW);
    }
}

// =======================
//     SYSTEM STATUS
// =======================
void printSystemStatus()
{
    Serial.println("\n========================================");
    Serial.println("System Status:");
    Serial.printf("  WiFi: %s", wifiConnected ? "✅ Connected" : "❌ Disconnected");
    if (wifiConnected)
    {
        Serial.printf(" (%d dBm)\n", WiFi.RSSI());
    }
    else
    {
        Serial.println();
    }
    Serial.printf("  MQTT: %s\n", mqttConnected ? "✅ Connected" : "❌ Disconnected");
    Serial.printf("  System: %s\n", isSystemArmed ? "🔔 ARMED" : "🔕 DISARMED"); // ✅ Thêm dòng này
    Serial.printf("  Alarm: %s\n", isAlarmActive ? "🚨 ACTIVE" : "✅ Inactive");
    Serial.printf("  Uptime: %lu seconds\n", millis() / 1000);
    Serial.println("\nStatistics:");
    Serial.printf("  Total Inferences: %lu\n", totalInferences);
    Serial.printf("  Attacks Detected: %lu\n", attacksDetected);
    Serial.printf("  Free Heap: %d bytes\n", ESP.getFreeHeap());
    Serial.println("========================================\n");
}
