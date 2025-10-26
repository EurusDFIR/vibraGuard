# VibraGuard AI - I2C Debug Tool

## Mục đích:

Debug kết nối I2C giữa ESP32 và MPU6050 khi gặp lỗi "Khong tim thay MPU6050"

## Cách sử dụng:

1. **Nạp code:**

   - Mở Arduino IDE
   - Mở file: `esp32/i2c_debug/i2c_debug.ino`
   - Board: ESP32C3 Dev Module
   - Upload

2. **Xem kết quả:**
   - Mở Serial Monitor (115200 baud)
   - Đọc output để chẩn đoán vấn đề

## Kết quả mong đợi:

### ✅ Thành công:

```
🔍 Scanning I2C bus...
   Expected MPU6050 address: 0x68 or 0x69
   ✅ Device found at address 0x68
✅ Found 1 device(s) on I2C bus

🔧 Testing MPU6050...
   ✅ MPU6050 detected at 0x68!
   🎉 Ready for data collection!
```

### ❌ Thất bại:

```
🔍 Scanning I2C bus...
   Expected MPU6050 address: 0x68 or 0x69
   ❌ No I2C devices found!

🔧 Troubleshooting:
   1. Kiểm tra nguồn điện...
```

## Các vấn đề thường gặp:

### 1. **Không tìm thấy thiết bị I2C:**

- **Nguyên nhân:** Nguồn điện, dây nối, module hỏng
- **Giải pháp:**
  - Đo điện áp VCC (phải = 3.3V)
  - Kiểm tra dây SDA/SCL
  - Thử swap GP4 ↔ GP5
  - Thêm tụ điện 100nF giữa VCC-GND

### 2. **MPU6050 ở địa chỉ 0x69:**

- **Nguyên nhân:** Một số module MPU6050 dùng địa chỉ khác
- **Giải pháp:** Cập nhật code data_forwarder.ino để dùng 0x69

### 3. **ESP32 không scan được:**

- **Nguyên nhân:** Cấu hình I2C sai
- **Giải pháp:** Kiểm tra Wire.begin(4, 5) đúng chưa

## Tiếp theo:

Sau khi debug thành công, quay lại:

1. Nạp `data_forwarder.ino`
2. Chạy `edge-impulse-data-forwarder`
3. Thu thập dữ liệu AI
