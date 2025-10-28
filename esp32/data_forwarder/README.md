# VibraGuard AI - Data Collection Guide

## Bước 1: Chuẩn bị

### Phần cứng đã nối dây:

- ✅ MPU-6050 VCC -> ESP32 3V3
- ✅ MPU-6050 GND -> ESP32 GND
- ✅ MPU-6050 SDA -> ESP32 GP4
- ✅ MPU-6050 SCL -> ESP32 GP5

### Phần mềm đã cài:

- ✅ Arduino IDE với thư viện: `Adafruit MPU6050`, `Adafruit BusIO`
- ✅ Edge Impulse CLI: `edge-impulse-data-forwarder` (version 1.35.1)

---

## Bước 2: Nạp Code Thu thập

1. Mở Arduino IDE
2. Mở file: `esp32/data_forwarder/data_forwarder.ino`
3. Chọn board: **ESP32C3 Dev Module**
4. Chọn port COM tương ứng
5. Nhấn **Upload**

### Kiểm tra:

- Mở Serial Monitor (115200 baud)
- Phải thấy: `✅ MPU6050 san sang!`
- Thấy dòng số liên tục: `x,y,z` (ví dụ: `0.12,-0.34,9.81`)

---

## Bước 3: Thu thập Dữ liệu

### 3.1. Kết nối với Edge Impulse:

```bash
cd r:/_Projects/Eurus_Workspace/vibraGuard
edge-impulse-data-forwarder
```

- Đăng nhập tài khoản Edge Impulse
- Chọn hoặc tạo project mới (đặt tên: `vibraguard-ai`)
- CLI sẽ phát hiện ESP32 và bắt đầu forward dữ liệu

### 3.2. Thu thập từng loại dữ liệu:

#### 📊 **Label: `normal`** (3-5 phút)

Mục tiêu: Dạy AI nhận biết rung động bình thường

**Hành động:**

- Gõ cửa nhẹ (5-10 lần)
- Đóng/mở cửa bình thường
- Vỗ tay gần cửa
- Bật nhạc/radio gần cửa
- Để người đi qua gần cửa

**Trên web Edge Impulse:**

- Vào `Data acquisition`
- Label: `normal`
- Sample length: 10000 ms (10 giây)
- Nhấn `Start sampling` (lặp lại nhiều lần)

---

#### ⚠️ **Label: `attack`** (3-5 phút)

Mục tiêu: Dạy AI nhận biết tấn công thực sự

**Hành động:**

- Mô phỏng khoan (dùng búa cao su đập liên tục)
- Đập mạnh vào cửa/tường
- Cạy cửa (dùng dụng cụ kim loại)
- Lắc mạnh khung cửa

**Trên web Edge Impulse:**

- Label: `attack`
- Sample length: 10000 ms
- Nhấn `Start sampling` (lặp lại nhiều lần)

---

#### 🔇 **Label: `noise`** (1-2 phút)

Mục tiêu: Dạy AI bỏ qua nhiễu nền

**Hành động:**

- Không làm gì cả (để yên cửa)
- Chỉ có rung động môi trường tự nhiên

**Trên web Edge Impulse:**

- Label: `noise`
- Sample length: 10000 ms
- Nhấn `Start sampling` (2-3 lần)

---

## Bước 4: Kiểm tra Dữ liệu

Trên web Edge Impulse:

1. Vào tab **Data acquisition**
2. Kiểm tra:
   - Training data: ~70% (ví dụ: 20 samples)
   - Test data: ~30% (ví dụ: 10 samples)
3. Phân bố nhãn:
   - `normal`: 40-50%
   - `attack`: 40-50%
   - `noise`: 10-20%

---

## Tiếp theo:

Sau khi có đủ dữ liệu, chuyển sang **Phase 2: Train AI Model**

Sẽ thực hiện:

1. Create Impulse (Spectral Analysis)
2. Generate features
3. Train Neural Network
4. Deploy Arduino library

---

## Troubleshooting:

### Lỗi: "Khong tim thay MPU6050"

**Bước 1: Chạy debug tool**

1. Nạp file: `esp32/i2c_debug/i2c_debug.ino`
2. Xem Serial Monitor để chẩn đoán chính xác
3. Sửa theo hướng dẫn trong `esp32/i2c_debug/README.md`

**Bước 2: Kiểm tra cơ bản**

- Kiểm tra lại dây nối
- Thử swap SDA/SCL (đổi GP4 <-> GP5)
- Đo điện áp VCC (phải ~3.3V)
- **Nếu debug tool báo địa chỉ 0x69:** Thay đổi `#define MPU6050_ADDRESS 0x68` thành `0x69` trong code

### edge-impulse-data-forwarder không thấy thiết bị

- Đóng Serial Monitor trước khi chạy CLI
- Thử port COM khác
- Restart ESP32

### Dữ liệu không ổn định (nhảy lung tung)

- Kiểm tra nguồn điện ESP32
- Thêm tụ điện 100nF giữa VCC và GND của MPU6050
- Rút ngắn dây I2C
