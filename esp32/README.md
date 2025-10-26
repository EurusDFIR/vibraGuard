# VibraGuard - ESP32 Hardware Setup

## Giai đoạn 1: Lõi phần cứng (Hardware Core)

### Mục tiêu

Thiết bị hoạt động độc lập - phát hiện rung động và kích hoạt còi cảnh báo.

### Linh kiện cần thiết

1. ESP32 Development Board
2. Cảm biến rung động SW-420
3. Buzzer (còi) - 3.3V hoặc 5V
4. Transistor NPN (S8050, 2N2222, hoặc tương tự)
5. Điện trở 1K Ohm
6. Dây nối (jumper wires)
7. Breadboard (không bắt buộc)

### Sơ đồ kết nối

#### Cảm biến SW-420

```
SW-420 VCC  -> ESP32 3.3V
SW-420 GND  -> ESP32 GND
SW-420 DO   -> ESP32 GPIO 13
```

#### Mạch lái còi Buzzer (quan trọng!)

```
ESP32 GPIO 12 -> Điện trở 1K Ohm -> Transistor Base (B)
Transistor Emitter (E) -> GND chung
Transistor Collector (C) -> Buzzer (-)
Buzzer (+) -> 5V (hoặc 3.3V)
```

**Lưu ý:** Không nối trực tiếp GPIO vào Buzzer! Phải dùng Transistor để bảo vệ ESP32.

### Cách upload code

1. Mở Arduino IDE
2. Cài đặt board ESP32 (nếu chưa có):
   - File > Preferences > Additional Board Manager URLs
   - Thêm: `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`
   - Tools > Board > Boards Manager > Tìm "ESP32" và cài đặt
3. Chọn board:
   - Tools > Board > ESP32 Arduino > ESP32 Dev Module
4. Chọn COM port của ESP32
5. Upload file `vibraguard_hardware_core.ino`

### Kiểm thử

1. Cấp nguồn cho ESP32 (qua USB hoặc nguồn ngoài)
2. Mở Serial Monitor (115200 baud)
3. Gõ nhẹ hoặc rung cảm biến SW-420
4. **Kết quả mong đợi:**
   - Serial Monitor hiển thị "⚠️ VIBRATION DETECTED!"
   - Còi Buzzer kêu lên

### Troubleshooting

**Còi không kêu:**

- Kiểm tra lại kết nối Transistor (Base, Emitter, Collector)
- Đảm bảo điện trở 1K Ohm được nối đúng
- Kiểm tra cực (+) và (-) của Buzzer
- Thử đổi Transistor khác

**Cảm biến không phát hiện rung:**

- Kiểm tra độ nhạy của SW-420 (có vít điều chỉnh)
- Thử xoay vít trên module SW-420 để tăng độ nhạy
- Kiểm tra kết nối DO với GPIO 13

**ESP32 reset liên tục:**

- Kiểm tra nguồn cấp cho ESP32
- Có thể buzzer tiêu thụ quá nhiều dòng, thử dùng nguồn ngoài mạnh hơn
