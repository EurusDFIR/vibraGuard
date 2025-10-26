/*
 * VibraGuard AI - I2C Debug Tool
 * Mục tiêu: Debug kết nối I2C với MPU6050
 *
 * Sơ đồ nối dây:
 * - MPU-6050 VCC -> 3V3 (ESP32)
 * - MPU-6050 GND -> GND (ESP32)
 * - MPU-6050 SDA -> GP4 (ESP32)
 * - MPU-6050 SCL -> GP5 (ESP32)
 */

#include <Wire.h>

void setup()
{
    Serial.begin(115200);
    delay(1000);

    Serial.println("\n========================================");
    Serial.println("VibraGuard AI - I2C Debug Tool");
    Serial.println("========================================");

    // Khởi tạo I2C với SDA=GP4, SCL=GP5
    Wire.begin(4, 5);
    Serial.println("✅ I2C initialized (SDA=4, SCL=5)");

    // Test scan I2C bus
    Serial.println("\n🔍 Scanning I2C bus...");
    Serial.println("   Expected MPU6050 address: 0x68 or 0x69");

    byte error, address;
    int nDevices = 0;

    for (address = 1; address < 127; address++)
    {
        Wire.beginTransmission(address);
        error = Wire.endTransmission();

        if (error == 0)
        {
            Serial.print("   ✅ Device found at address 0x");
            if (address < 16)
                Serial.print("0");
            Serial.println(address, HEX);
            nDevices++;
        }
        else if (error == 4)
        {
            Serial.print("   ⚠️  Unknown error at address 0x");
            if (address < 16)
                Serial.print("0");
            Serial.println(address, HEX);
        }
    }

    if (nDevices == 0)
    {
        Serial.println("   ❌ No I2C devices found!");
        Serial.println("\n🔧 Troubleshooting:");
        Serial.println("   1. Kiểm tra nguồn điện:");
        Serial.println("      - VCC phải là 3.3V (đo bằng multimeter)");
        Serial.println("      - GND phải nối chắc chắn");
        Serial.println("   2. Kiểm tra dây I2C:");
        Serial.println("      - SDA: MPU6050 -> GP4 (ESP32)");
        Serial.println("      - SCL: MPU6050 -> GP5 (ESP32)");
        Serial.println("      - Thử swap SDA/SCL nếu không hoạt động");
        Serial.println("   3. Kiểm tra module MPU6050:");
        Serial.println("      - Thử module khác");
        Serial.println("      - Thêm tụ điện 100nF giữa VCC và GND");
        Serial.println("   4. Kiểm tra ESP32:");
        Serial.println("      - Đảm bảo chọn đúng board: ESP32C3 Dev Module");
        Serial.println("      - Thử reset ESP32");
    }
    else
    {
        Serial.print("\n✅ Found ");
        Serial.print(nDevices);
        Serial.println(" device(s) on I2C bus");

        // Test MPU6050 specific
        Serial.println("\n🔧 Testing MPU6050...");
        Wire.beginTransmission(0x68); // Default MPU6050 address
        error = Wire.endTransmission();

        if (error == 0)
        {
            Serial.println("   ✅ MPU6050 detected at 0x68!");
            Serial.println("   🎉 Ready for data collection!");
        }
        else
        {
            Serial.println("   ❌ MPU6050 not responding at 0x68");

            // Try alternative address
            Wire.beginTransmission(0x69);
            error = Wire.endTransmission();

            if (error == 0)
            {
                Serial.println("   ✅ MPU6050 detected at 0x69!");
                Serial.println("   📝 Note: Your MPU6050 uses address 0x69");
                Serial.println("   🔧 Update data_forwarder.ino if needed");
            }
            else
            {
                Serial.println("   ❌ MPU6050 not found at any address");
            }
        }
    }

    Serial.println("\n========================================");
}

void loop()
{
    // Không cần loop, chỉ chạy setup một lần
    delay(1000);
}