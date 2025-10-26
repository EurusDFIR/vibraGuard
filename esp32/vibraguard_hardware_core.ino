/*
 * VibraGuard - Hardware Core (Giai đoạn 1)
 * Mục tiêu: Thiết bị hoạt động độc lập (rung là kêu)
 *
 * Sơ đồ kết nối:
 * - SW-420 Digital Out (DO) -> GPIO 13
 * - GPIO 12 -> Điện trở 1K Ohm -> Transistor NPN Base (B)
 * - Transistor Emitter (E) -> GND
 * - Transistor Collector (C) -> Buzzer (-)
 * - Buzzer (+) -> 5V hoặc 3.3V
 */

// Định nghĩa chân
const int SENSOR_PIN = 13; // SW-420 Digital Output
const int BUZZER_PIN = 12; // Điều khiển Buzzer qua Transistor

void setup()
{
    // Khởi tạo Serial để debug
    Serial.begin(115200);
    Serial.println("VibraGuard - Hardware Core Started");

    // Cấu hình chân
    pinMode(SENSOR_PIN, INPUT);  // Chân đọc cảm biến rung
    pinMode(BUZZER_PIN, OUTPUT); // Chân điều khiển còi

    // Tắt còi ban đầu
    digitalWrite(BUZZER_PIN, LOW);

    Serial.println("System Ready - Monitoring vibration...");
}

void loop()
{
    // Đọc trạng thái cảm biến rung
    int sensorState = digitalRead(SENSOR_PIN);

    if (sensorState == HIGH)
    {
        // Phát hiện rung động!
        Serial.println("⚠️ VIBRATION DETECTED!");
        digitalWrite(BUZZER_PIN, HIGH); // Bật còi
        delay(100);                     // Giữ còi kêu một chút
    }
    else
    {
        // Không có rung động
        digitalWrite(BUZZER_PIN, LOW); // Tắt còi
    }

    delay(50); // Delay nhỏ để tránh đọc quá nhanh
}
