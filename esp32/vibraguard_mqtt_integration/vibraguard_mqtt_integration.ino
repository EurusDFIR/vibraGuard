/*
 * VibraGuard - MQTT Integration (Giai đoạn 3)
 * Mục tiêu: Kết nối ESP32 với MQTT Broker và gửi dữ liệu lên Backend
 *
 * Thư viện cần cài đặt trong Arduino IDE:
 * 1. PubSubClient by Nick O'Leary
 *
 * Sơ đồ kết nối:
 * - SW-420 Digital Out (DO) -> GPIO 13
 * - GPIO 12 -> Điện trở 1K Ohm -> Transistor NPN Base (B)
 * - Transistor Emitter (E) -> GND
 * - Transistor Collector (C) -> Buzzer (-)
 * - Buzzer (+) -> 5V hoặc 3.3V
 */

#include <WiFi.h>
#include <PubSubClient.h>

// ===== CẤU HÌNH WIFI =====
const char *ssid = "LE HUNG";       // Thay bằng tên WiFi của bạn
const char *password = "123456789"; // Thay bằng mật khẩu WiFi

// ===== CẤU HÌNH MQTT =====
const char *mqtt_server = "192.168.1.2"; // Thay bằng IP máy tính chạy Mosquitto
const int mqtt_port = 1883;
const char *mqtt_topic = "vibra_guard/sensor";
const char *device_id = "ESP32_CUA_SO_01"; // ID thiết bị (phải khớp với DB)

// MQTT Keepalive settings
const unsigned long MQTT_KEEP_ALIVE = 15; // Giảm xuống 15 giây để match Mosquitto
const unsigned long MQTT_TIMEOUT = 5000;  // 5 giây timeout cho operations

// ===== ĐỊNH NGHĨA CHÂN =====
const int SENSOR_PIN = 2; // SW-420 Digital Output
const int BUZZER_PIN = 1; // Điều khiển Buzzer qua Transistor

// ===== BIẾN TOÀN CỤC =====
WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastVibrationTime = 0;
const unsigned long VIBRATION_COOLDOWN = 2000; // 2 giây cooldown giữa các lần gửi

// ===== BIẾN CHO BUZZER =====
bool buzzerActive = false;
unsigned long buzzerStartTime = 0;
const unsigned long BUZZER_DURATION = 5000; // 5 giây kêu còi liên tục

// ===== BIẾN CHO RECONNECT =====
unsigned long lastWiFiCheck = 0;
unsigned long lastMQTTCheck = 0;
unsigned long lastMQTTReconnectAttempt = 0;
const unsigned long WIFI_CHECK_INTERVAL = 10000; // Kiểm tra WiFi mỗi 10 giây
const unsigned long MQTT_CHECK_INTERVAL = 5000;  // Kiểm tra MQTT mỗi 5 giây
unsigned long mqttReconnectDelay = 1000;         // Delay ban đầu 1 giây
const unsigned long MQTT_MAX_DELAY = 30000;      // Delay tối đa 30 giây

// ===== TRẠNG THÁI HỆ THỐNG =====
bool wifiConnected = false;
bool mqttConnected = false;

// MQTT timeout tracking
unsigned long lastMQTTMessageTime = 0;
const unsigned long MQTT_TIMEOUT_MS = 60000; // Tăng lên 60 giây để không can thiệp MQTT keepalive

void checkMQTTTimeout()
{
    if (mqttConnected && (millis() - lastMQTTMessageTime > MQTT_TIMEOUT_MS))
    {
        Serial.println("⚠️  MQTT timeout detected - forcing reconnect");
        mqttConnected = false;
        client.disconnect();
    }
}

void setup()
{
    // Khởi tạo Serial
    Serial.begin(115200);
    Serial.println("\n\n========================================");
    Serial.println("VibraGuard - MQTT Integration Started");
    Serial.println("========================================");

    // Cấu hình chân
    pinMode(SENSOR_PIN, INPUT);
    pinMode(BUZZER_PIN, OUTPUT);
    digitalWrite(BUZZER_PIN, LOW);

    // Kết nối WiFi
    connectWiFi();

    // Cấu hình MQTT với keepalive
    client.setServer(mqtt_server, mqtt_port);
    client.setCallback(mqttCallback);
    client.setKeepAlive(MQTT_KEEP_ALIVE);
    client.setSocketTimeout(MQTT_TIMEOUT);

    Serial.println("System Ready - Monitoring vibration...");
}

void loop()
{
    unsigned long currentTime = millis();

    // Kiểm tra và reconnect WiFi
    if (currentTime - lastWiFiCheck >= WIFI_CHECK_INTERVAL)
    {
        checkWiFiConnection();
        lastWiFiCheck = currentTime;
    }

    // Kiểm tra và reconnect MQTT (chỉ khi WiFi đã kết nối)
    if (wifiConnected && currentTime - lastMQTTCheck >= MQTT_CHECK_INTERVAL)
    {
        checkMQTTConnection();
        lastMQTTCheck = currentTime;
    }

    // Kiểm tra MQTT timeout
    if (mqttConnected)
    {
        checkMQTTTimeout();
    }

    // Xử lý MQTT messages (luôn gọi client.loop() nếu WiFi connected)
    if (wifiConnected)
    {
        client.loop();
    }

    // Đọc trạng thái cảm biến rung
    int sensorState = digitalRead(SENSOR_PIN);

    // Quản lý buzzer - kiểm tra xem có cần tắt không
    if (buzzerActive && (currentTime - buzzerStartTime >= BUZZER_DURATION))
    {
        digitalWrite(BUZZER_PIN, LOW);
        buzzerActive = false;
        Serial.println("🔕 Buzzer auto-off after timeout");
    }

    if (sensorState == HIGH)
    {
        // Kiểm tra cooldown để tránh spam MQTT messages
        if (currentTime - lastVibrationTime > VIBRATION_COOLDOWN)
        {
            lastVibrationTime = currentTime;

            // Phát hiện rung động!
            Serial.println("\n⚠️  VIBRATION DETECTED!");

            // Bật còi liên tục (nếu chưa active)
            if (!buzzerActive)
            {
                digitalWrite(BUZZER_PIN, HIGH);
                buzzerActive = true;
                buzzerStartTime = currentTime;
                Serial.println("🔊 Buzzer activated for 5 seconds!");
            }

            // Gửi tin nhắn MQTT (chỉ khi MQTT đã kết nối)
            if (mqttConnected)
            {
                sendVibrationAlert();
            }
            else
            {
                Serial.println("⚠️  MQTT not connected - Cannot send alert!");
            }
        }
    }

    delay(50);
}

void connectWiFi()
{
    Serial.print("\n🔌 Connecting to WiFi: ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20)
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
        Serial.println("\n📶 WiFi connection lost! Attempting to reconnect...");
        wifiConnected = false;
        mqttConnected = false; // MQTT cũng sẽ mất khi WiFi mất

        connectWiFi();
    }
    else if (!wifiConnected)
    {
        // WiFi đã kết nối nhưng flag chưa được set
        wifiConnected = true;
        Serial.println("✅ WiFi status confirmed");
        printWiFiInfo();
    }
    else
    {
        // Kiểm tra WiFi signal quality định kỳ
        static unsigned long lastSignalCheck = 0;
        if (millis() - lastSignalCheck > 30000)
        { // Mỗi 30 giây
            printWiFiInfo();
            lastSignalCheck = millis();
        }
    }
}

void printWiFiInfo()
{
    Serial.print("📊 WiFi Info - IP: ");
    Serial.print(WiFi.localIP());
    Serial.print(" | RSSI: ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");
}

void checkMQTTConnection()
{
    unsigned long currentTime = millis();

    // Debug: In ra state hiện tại
    static unsigned long lastDebugPrint = 0;
    if (millis() - lastDebugPrint > 10000)
    { // Mỗi 10 giây
        Serial.print("🔍 MQTT State - Connected: ");
        Serial.print(client.connected() ? "YES" : "NO");
        Serial.print(" | Flag: ");
        Serial.println(mqttConnected ? "YES" : "NO");
        lastDebugPrint = millis();
    }

    // Chỉ thử reconnect nếu đã đủ thời gian delay
    if (!client.connected() && (currentTime - lastMQTTReconnectAttempt >= mqttReconnectDelay))
    {
        Serial.print("\n🔄 Attempting MQTT connection...");

        String clientId = "ESP32-";
        clientId += String(device_id);

        // Thử connect với timeout
        bool connectResult = false;
        unsigned long connectStart = millis();

        // Non-blocking connect attempt
        if (client.connect(clientId.c_str()))
        {
            connectResult = true;
        }

        if (connectResult && client.connected())
        {
            Serial.println(" ✅ Connected to MQTT Broker!");
            mqttConnected = true;
            lastMQTTMessageTime = millis(); // Reset timeout counter

            // Subscribe to control topic
            String controlTopic = "vibra_guard/control/" + String(device_id);
            if (client.subscribe(controlTopic.c_str()))
            {
                Serial.print("📥 Subscribed to: ");
                Serial.println(controlTopic);
            }
            else
            {
                Serial.println("❌ Failed to subscribe to control topic!");
            }

            // Reset reconnect delay khi thành công
            mqttReconnectDelay = 1000;
        }
        else
        {
            int errorCode = client.state();
            Serial.print(" ❌ Failed, rc=");
            Serial.print(errorCode);

            // Phân tích lỗi chi tiết hơn
            switch (errorCode)
            {
            case -4:
                Serial.print(" (connection timeout)");
                break;
            case -3:
                Serial.print(" (connection lost)");
                break;
            case -2:
                Serial.print(" (connect failed)");
                break;
            case -1:
                Serial.print(" (malformed packet)");
                break;
            case 1:
                Serial.print(" (protocol violation)");
                break;
            case 2:
                Serial.print(" (client id rejected)");
                break;
            case 3:
                Serial.print(" (server unavailable)");
                break;
            case 4:
                Serial.print(" (bad credentials)");
                break;
            case 5:
                Serial.print(" (not authorized)");
                break;
            default:
                Serial.print(" (unknown)");
                break;
            }

            // Kiểm tra WiFi status khi MQTT fail
            Serial.print(" | WiFi Status: ");
            switch (WiFi.status())
            {
            case WL_CONNECTED:
                Serial.print("Connected");
                break;
            case WL_NO_SHIELD:
                Serial.print("No Shield");
                break;
            case WL_IDLE_STATUS:
                Serial.print("Idle");
                break;
            case WL_NO_SSID_AVAIL:
                Serial.print("No SSID");
                break;
            case WL_SCAN_COMPLETED:
                Serial.print("Scan Done");
                break;
            case WL_CONNECT_FAILED:
                Serial.print("Connect Failed");
                break;
            case WL_CONNECTION_LOST:
                Serial.print("Connection Lost");
                break;
            case WL_DISCONNECTED:
                Serial.print("Disconnected");
                break;
            default:
                Serial.print("Unknown");
                break;
            }

            Serial.print(" - Retrying in ");
            Serial.print(mqttReconnectDelay / 1000);
            Serial.println(" seconds...");

            mqttConnected = false;

            // Exponential backoff
            mqttReconnectDelay = min(mqttReconnectDelay * 2, MQTT_MAX_DELAY);
        }

        lastMQTTReconnectAttempt = currentTime;
    }
    else if (client.connected() && !mqttConnected)
    {
        // MQTT đã kết nối nhưng flag chưa được set
        mqttConnected = true;
        Serial.println("✅ MQTT status confirmed");
    }
    else if (!client.connected() && mqttConnected)
    {
        // MQTT vừa mất kết nối
        mqttConnected = false;
        Serial.println("📡 MQTT connection lost!");
        mqttReconnectDelay = 1000; // Reset delay để reconnect ngay
    }
}

void sendVibrationAlert()
{
    if (!mqttConnected || !client.connected())
    {
        Serial.println("❌ Cannot send MQTT message - Not connected!");
        return;
    }

    // Tạo JSON payload
    String payload = "{";
    payload += "\"deviceId\":\"" + String(device_id) + "\",";
    payload += "\"timestamp\:" + String(millis()) + ",";
    payload += "\"sensorValue\":1";
    payload += "}";

    // Publish to MQTT với retry
    int retryCount = 0;
    const int MAX_RETRIES = 3;
    boolean success = false;

    while (retryCount < MAX_RETRIES && !success)
    {
        if (client.connected())
        {
            success = client.publish(mqtt_topic, payload.c_str());
            if (!success)
            {
                retryCount++;
                if (retryCount < MAX_RETRIES)
                {
                    Serial.print("⚠️  Publish failed, retrying... (");
                    Serial.print(retryCount);
                    Serial.print("/");
                    Serial.print(MAX_RETRIES);
                    Serial.println(")");
                    delay(100 * retryCount); // Delay tăng dần
                }
            }
        }
        else
        {
            Serial.println("❌ MQTT disconnected during publish attempt!");
            mqttConnected = false;
            break;
        }
    }

    if (success)
    {
        Serial.println("📤 MQTT Message Sent:");
        Serial.println("   Topic: " + String(mqtt_topic));
        Serial.println("   Payload: " + payload);
        lastMQTTMessageTime = millis(); // Cập nhật thời gian gửi thành công
    }
    else
    {
        Serial.println("❌ Failed to send MQTT message after retries!");
        mqttConnected = false; // Mark as disconnected để trigger reconnect
    }
}

void mqttCallback(char *topic, byte *payload, unsigned int length)
{
    Serial.print("\n📨 Message received [");
    Serial.print(topic);
    Serial.print("]: ");

    String message;
    for (int i = 0; i < length; i++)
    {
        message += (char)payload[i];
    }
    Serial.println(message);

    // Xử lý lệnh điều khiển
    if (message.indexOf("DISARM") >= 0)
    {
        Serial.println("🔕 Disarm command received - Buzzer OFF immediately");
        digitalWrite(BUZZER_PIN, LOW);
        buzzerActive = false;
    }
    else if (message.indexOf("ARM") >= 0)
    {
        Serial.println("🔔 Arm command received - System active");
        // Có thể thêm logic arm/disarm sau
    }
    else if (message.indexOf("BUZZER_OFF") >= 0)
    {
        Serial.println("🔇 Buzzer OFF command received");
        digitalWrite(BUZZER_PIN, LOW);
        buzzerActive = false;
    }
}
