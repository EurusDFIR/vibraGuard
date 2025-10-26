/*
 * =======================================================
 * PHƯƠNG ÁN C.2: (SILENT MODE for Edge Impulse)
 * =======================================================
 * Xóa bỏ mọi Serial.println() trong setup() để
 * edge-impulse-data-forwarder không bị nhầm lẫn.
 *
 * Nối dây (Vẫn như cũ):
 * - SDA -> GP8
 * - SCL -> GP9
 */

#include <MPU6050_tockn.h>
#include <Wire.h>

MPU6050 mpu(Wire);

void setup() {
    Serial.begin(115200);
    
    // Khởi tạo I2C với SDA=GP8, SCL=GP9
    Wire.begin(8, 9); 
    
    mpu.begin();
    
    // Tự hiệu chỉnh (im lặng, không in gì ra)
    mpu.calcGyroOffsets(); 
}

void loop() {
    // 1. Cập nhật dữ liệu
    mpu.update(); 
    
    // 2. Lấy dữ liệu
    float accX = mpu.getAccX();
    float accY = mpu.getAccY();
    float accZ = mpu.getAccZ();

    // 3. In ra theo format Edge Impulse
    // Format: x,y,z (không có khoảng trắng)
    Serial.print(accX);
    Serial.print(",");
    Serial.print(accY);
    Serial.print(",");
    Serial.println(accZ);
    
    delay(100); // 10 mẫu/giây
}