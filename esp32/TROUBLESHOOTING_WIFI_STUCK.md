# 🔧 Hướng dẫn Khắc phục ESP32 C3 - WiFi Stuck với SSID cũ

## ⚠️ Vấn đề

ESP32 C3 vẫn hiển thị SSID cũ "Eurus" mặc dù code đã thay đổi thành "LE HUNG"

## 🔍 Nguyên nhân

1. **Firmware cache:** ESP32 đang chạy firmware cũ đã compile trước đó
2. **Flash memory chưa xóa:** Code cũ vẫn còn trong flash
3. **Compile không thành công:** Arduino IDE có thể compile nhầm file cũ
4. **Wrong board selected:** Board selection không đúng

## ✅ Giải pháp (Theo thứ tự)

### Bước 1: Xóa hoàn toàn Flash Memory (QUAN TRỌNG NHẤT!)

#### Trên Windows:

```bash
# 1. Tìm COM port của ESP32 (ví dụ: COM3, COM4...)
# Check trong Arduino IDE -> Tools -> Port

# 2. Xóa toàn bộ flash
# Tải esptool.py hoặc dùng từ Arduino
C:\Users\YOUR_USERNAME\AppData\Local\Arduino15\packages\esp32\tools\esptool_py\<version>\esptool.exe --chip esp32c3 --port COM3 erase_flash

# Hoặc dùng esptool.py
python -m esptool --chip esp32c3 --port COM3 erase_flash
```

#### Sau khi xóa flash:

ESP32 sẽ hiển thị:

```
Erasing flash (this may take a while)...
Chip erase completed successfully
```

### Bước 2: Verify Code trong Arduino IDE

1. **Mở file:** `vibraguard_ai_final.ino`
2. **Kiểm tra dòng 38-39:**
   ```cpp
   const char *WIFI_SSID = "LE HUNG";
   const char *WIFI_PASSWORD = "123456789";
   ```
3. **Kiểm tra message mới trong setup():**
   ```cpp
   Serial.println("VibraGuard AI - FIRMWARE MOI - NOV 2 2025");
   Serial.println("    WIFI: LE HUNG - PASSWORD: 123456789");
   ```

### Bước 3: Clean Build trong Arduino IDE

1. **Sketch** → **Export compiled Binary** (để force recompile)
2. Xóa folder build cache:
   - Windows: `C:\Users\YOUR_USERNAME\AppData\Local\Temp\arduino_build_*`
   - Delete tất cả folders có tên `arduino_build_`
3. Đóng và mở lại Arduino IDE

### Bước 4: Upload Code với Settings đúng

**Board Settings:**

```
Board: "ESP32C3 Dev Module"
Upload Speed: "115200"
CPU Frequency: "160MHz"
Flash Frequency: "80MHz"
Flash Mode: "QIO"
Flash Size: "4MB (32Mb)"
Partition Scheme: "Default 4MB with spiffs"
Core Debug Level: "None"
Erase All Flash Before Sketch Upload: "Enabled"  ⬅️ QUAN TRỌNG!
```

**Cách enable "Erase All Flash":**

- Tools → Erase All Flash Before Sketch Upload → **Enabled**

### Bước 5: Upload và Monitor

1. **Upload code:**
   - Click nút Upload (hoặc Ctrl+U)
   - Đợi "Hard resetting via RTS pin..."
2. **Mở Serial Monitor:**

   - Baud rate: **115200**
   - Bạn PHẢI thấy:

   ```
   ========================================
   VibraGuard AI - FIRMWARE MOI - NOV 2 2025
       WIFI: LE HUNG - PASSWORD: 123456789
       *** KIEM TRA XEM DONG NAY CO HIEN RA KHONG ***
   ========================================
   ```

3. **Kiểm tra WiFi connect:**
   ```
   ========================================
   🔌 BẮT ĐẦU KẾT NỐI WIFI
      SSID: LE HUNG
      Password: 123456789
   ========================================
   ```

### Bước 6: Hardware Reset

Nếu vẫn không work:

1. **Giữ nút BOOT** trên ESP32 C3
2. **Nhấn nút RESET** (trong khi vẫn giữ BOOT)
3. **Thả RESET**, đợi 1 giây
4. **Thả BOOT**
5. ESP32 vào **bootloader mode**
6. Upload code ngay lập tức

## 🎯 Checklist

- [ ] Xóa flash memory hoàn toàn (`erase_flash`)
- [ ] Xóa Arduino build cache
- [ ] Verify code có đúng "LE HUNG" không
- [ ] Enable "Erase All Flash Before Sketch Upload"
- [ ] Board settings đúng (ESP32C3 Dev Module)
- [ ] Upload speed 115200
- [ ] Serial Monitor baud rate 115200
- [ ] Thấy message "FIRMWARE MOI - NOV 2 2025"
- [ ] Thấy "SSID: LE HUNG" trong log

## 🔍 Debug Tips

### Nếu vẫn thấy "Eurus":

1. **Kiểm tra file đang mở:**

   - File → Recent → Có nhiều file `.ino` không?
   - Đảm bảo đang mở file đúng!

2. **Kiểm tra thư mục:**

   ```
   r:\_Projects\Eurus_Workspace\vibraGuard\esp32\vibraguard_ai_final\
   ```

   Có file `.ino` nào khác không?

3. **Search toàn bộ project:**
   - Ctrl+Shift+F trong VS Code
   - Search "Eurus"
   - Xem có file nào khác chứa "Eurus" không

### Nếu compile error:

```
Error compiling for board ESP32C3 Dev Module
```

→ Cài lại ESP32 board package:

- Tools → Board → Boards Manager
- Search "esp32"
- Uninstall → Install lại version mới nhất

## 🚀 Phương án Cuối cùng (Nuclear Option)

Nếu tất cả đều fail:

1. **Uninstall Arduino IDE** hoàn toàn
2. **Xóa folders:**
   - `C:\Users\YOUR_USERNAME\AppData\Local\Arduino15`
   - `C:\Users\YOUR_USERNAME\Documents\Arduino`
3. **Reinstall Arduino IDE**
4. **Cài ESP32 board package**
5. **Cài Edge Impulse library**
6. **Upload code mới**

## 📝 Expected Output (Đúng)

```
========================================
VibraGuard AI - FIRMWARE MOI - NOV 2 2025
    WIFI: LE HUNG - PASSWORD: 123456789
    *** KIEM TRA XEM DONG NAY CO HIEN RA KHONG ***
========================================
AI Model: 95.1% Accuracy
  Attack:  93.1%
  Normal:  95.8%
  Noise:   98.8%
========================================
✅ Buzzer configured
✅ I2C initialized (SDA=8, SCL=9)
🔄 Initializing MPU6050... Done!
🔄 Calibrating Gyro (keep still)... Done!

========================================
🔌 BẮT ĐẦU KẾT NỐI WIFI
   SSID: LE HUNG
   Password: 123456789
========================================
.....
✅ WiFi Connected!
📡 IP Address: 192.168.1.xxx
📶 Signal Strength: -45 dBm
```

## ⚠️ Warning

Nếu sau khi làm TẤT CẢ các bước trên mà vẫn thấy "Eurus":
→ **ESP32 có thể bị lỗi hardware** hoặc **có 2 ESP32 cùng COM port**

Check:

- Có 2 ESP32 cắm vào máy tính không?
- Thử đổi USB port khác
- Thử ESP32 trên máy tính khác

---

**Last Updated:** November 2, 2025  
**Tested On:** ESP32-C3 Super Mini
