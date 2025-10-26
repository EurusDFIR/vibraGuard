/*
 * =======================================================
 * PHƯƠNG ÁN C: Sửa lỗi thư viện tockn
 * =======================================================
 * Lỗi biên dịch trước là do tôi trộn lẫn 2 thư viện.
 * Code này chỉ dùng 100% thư viện "MPU6050_tockn".
 *
 * Nối dây (Vẫn như cũ - ĐÃ ĐÚNG):
 * - SDA -> GP8
 * - SCL -> GP9
 */

#include <MPU6050_tockn.h> // <--- Chỉ dùng thư viện này
#include <Wire.h>

MPU6050 mpu(Wire);

void setup() {
    Serial.begin(115200);
    
    // Khởi tạo I2C với SDA=GP8, SCL=GP9
    Wire.begin(8, 9); 
    
    mpu.begin();
    
    Serial.println("\n========================================");
    Serial.println("VibraGuard AI - Data Forwarder (tockn lib)");
    Serial.println("========================================");
    Serial.println("Dang kiem tra ket noi (dung thu vien tockn)...");

    // Yêu cầu cảm biến tự hiệu chỉnh
    // Đây là cách tốt nhất để "test" kết nối.
    // Nếu nó đứng im ở đây -> Lỗi phần cứng (hàn/đứt dây)
    Serial.println("Dang tu hieu chinh Gyro... Vui long giu IM cam bien.");
    mpu.calcGyroOffsets(); 
    
    Serial.println("✅✅✅ THANH CONG! Ket noi I2C thanh cong!");
    Serial.println("📊 Bat dau stream du lieu (X, Y, Z)...");
}

void loop() {
    // 1. Cập nhật dữ liệu (BẮT BUỘC phải gọi hàm này)
    mpu.update(); 
    
    // 2. Lấy dữ liệu (Dùng hàm chuẩn của tockn)
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
    
    delay(100); // 10 mẫu/giây (như Edge Impulse cần)
}