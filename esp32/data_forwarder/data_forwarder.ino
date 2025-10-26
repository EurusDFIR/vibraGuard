/*
 * VibraGuard AI - Data Collection (Phase 1)
 * Mục tiêu: Thu thập dữ liệu rung động từ MPU-6050 để huấn luyện AI
 *
 * Phần cứng:
 * - ESP32-C3 Super Mini
 * - MPU-6050 (I2C)
 *
 * Sơ đồ nối dây:
 * - MPU-6050 VCC -> 3V3 (ESP32)
 * - MPU-6050 GND -> GND (ESP32)
 * - MPU-6050 SDA -> GP4 (ESP32)
 * - MPU-6050 SCL -> GP5 (ESP32)
 *
 * Cách sử dụng:
 * 1. Nạp code này vào ESP32
 * 2. Chạy: edge-impulse-data-forwarder
 * 3. Thu thập dữ liệu với các nhãn:
 *    - "normal": Gõ cửa, đóng cửa, vỗ tay (3-5 phút)
 *    - "attack": Mô phỏng khoan, đập, cạy (3-5 phút)
 *    - "noise": Không làm gì (1-2 phút)
 */

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

Adafruit_MPU6050 mpu;
int sample_rate_ms = 100; // Thu thập 10 mẫu/giây (1000ms / 100ms)

// Cấu hình địa chỉ I2C (thay đổi nếu cần)
#define MPU6050_ADDRESS 0x68 // Thay thành 0x69 nếu debug tool phát hiện ở đó

void setup()
{
    Serial.begin(115200);

    // Khởi tạo I2C với SDA=GP4, SCL=GP5
    Wire.begin(4, 5);

    Serial.println("\n========================================");
    Serial.println("VibraGuard AI - Data Forwarder");
    Serial.println("========================================");

    if (!mpu.begin(MPU6050_ADDRESS))
    {
        Serial.println("❌ Loi! Khong tim thay MPU6050.");
        Serial.println("   Kiem tra lai day ket noi:");
        Serial.println("   - VCC -> 3V3");
        Serial.println("   - GND -> GND");
        Serial.println("   - SDA -> GP4");
        Serial.println("   - SCL -> GP5");
        Serial.print("   - Dia chi I2C dang test: 0x");
        Serial.println(MPU6050_ADDRESS, HEX);
        Serial.println("   💡 Neu khong phai 0x68, thay doi MPU6050_ADDRESS thanh 0x69");
        while (1)
        {
            delay(10);
        }
    }

    Serial.println("✅ MPU6050 san sang!");
    Serial.println("📊 Bat dau stream du lieu...");
    Serial.println("   Format: X, Y, Z (acceleration)");
    Serial.println("========================================\n");

    // Cấu hình MPU6050
    mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
    mpu.setGyroRange(MPU6050_RANGE_500_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

    delay(1000); // Cho cảm biến ổn định
}

void loop()
{
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    // In 3 trục gia tốc X, Y, Z theo định dạng Edge Impulse
    // Format: x,y,z (dấu phẩy, không có khoảng trắng)
    Serial.print(a.acceleration.x);
    Serial.print(",");
    Serial.print(a.acceleration.y);
    Serial.print(",");
    Serial.println(a.acceleration.z);

    delay(sample_rate_ms);
}
