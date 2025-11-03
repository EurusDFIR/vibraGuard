# 📊 Giải thích Buffer trong VibraGuard AI

## 🤔 Buffer là gì?

**Buffer** (bộ đệm) là một **mảng dữ liệu tạm thời** dùng để lưu trữ các giá trị cảm biến trước khi đưa vào AI xử lý.

---

## 📦 Khai báo Buffer

```cpp
// AI Buffer
static float buffer[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE] = {0};
static size_t buf_idx = 0;
```

### Giải thích:

- **`buffer`**: Mảng chứa dữ liệu gia tốc kế (accelerometer)
- **`EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE`**: Kích thước buffer (do Edge Impulse định nghĩa)
- **`buf_idx`**: Chỉ số hiện tại trong buffer (con trỏ)
- **`static`**: Biến tồn tại suốt chương trình (không mất giữa các lần gọi hàm)

---

## 🔢 Kích thước Buffer

Ví dụ model của bạn:

```
EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE = 450
```

**Nghĩa là:**

- Buffer chứa **450 giá trị số thực (float)**
- Mỗi lần đọc sensor: 3 giá trị (X, Y, Z)
- → Cần đọc **150 lần** để đầy buffer (450 ÷ 3 = 150)

---

## 🔄 Quy trình Hoạt động

### Bước 1: Đọc Sensor

```cpp
void processAI() {
    // 1. Đọc dữ liệu cảm biến MPU6050
    mpu.update();
    float accX = mpu.getAccX();  // Gia tốc trục X
    float accY = mpu.getAccY();  // Gia tốc trục Y
    float accZ = mpu.getAccZ();  // Gia tốc trục Z
```

**Giải thích:**

- `mpu.update()`: Lấy dữ liệu mới từ sensor
- `getAccX/Y/Z()`: Lấy gia tốc 3 trục (đơn vị: g)

### Bước 2: Thêm vào Buffer

```cpp
    // 2. Thêm vào buffer
    buffer[buf_idx++] = accX;  // Vị trí 0, 3, 6, 9...
    buffer[buf_idx++] = accY;  // Vị trí 1, 4, 7, 10...
    buffer[buf_idx++] = accZ;  // Vị trí 2, 5, 8, 11...
```

**Minh họa:**

```
Buffer: [accX1, accY1, accZ1, accX2, accY2, accZ2, accX3, accY3, accZ3, ...]
Vị trí:    0      1      2      3      4      5      6      7      8    ...
```

**`buf_idx++`** nghĩa là:

- Gán giá trị vào vị trí hiện tại
- Sau đó tăng chỉ số lên 1
- Ví dụ: `buf_idx = 0` → gán X → `buf_idx = 1` → gán Y → `buf_idx = 2` → gán Z → `buf_idx = 3`

### Bước 3: Kiểm tra Buffer Đầy

```cpp
    // 3. Khi buffer đầy, chạy AI inference
    if (buf_idx >= EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE) {
        buf_idx = 0;  // Reset về đầu buffer

        // Chạy AI...
    }
}
```

**Khi nào buffer đầy?**

- Khi `buf_idx >= 450` (450 giá trị đã được thêm vào)
- Nghĩa là đã đọc **150 mẫu sensor** (mỗi mẫu = 3 giá trị X, Y, Z)

---

## ⏱️ Thời gian Thu thập Buffer

### Tính toán:

```cpp
const unsigned long AI_PROCESS_INTERVAL = 20; // milliseconds
```

- Mỗi 20ms đọc 1 lần sensor (50 lần/giây)
- Cần 150 mẫu để đầy buffer
- → **Thời gian**: 150 × 20ms = **3000ms = 3 giây**

**Buffer đầy sau 3 giây đọc liên tục!**

### Debug Progress:

```cpp
// Track buffer fill progress
int currentPercent = (buf_idx * 100) / EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE;
Serial.printf("📊 Buffer: %d%%\n", currentPercent);
```

Output:

```
📊 Buffer: 10%
📊 Buffer: 20%
📊 Buffer: 30%
...
📊 Buffer: 90%
🧠 Running AI inference...
```

---

## 🧠 Tại sao cần Buffer?

### 1. **AI cần chuỗi dữ liệu liên tục**

- AI model được train với **chuỗi 150 mẫu** liên tiếp
- Không thể dự đoán chỉ từ 1 mẫu duy nhất
- Cần "nhìn" toàn bộ **pattern rung động** trong 3 giây

### 2. **Phân tích Pattern theo thời gian**

```
Attack Pattern:  [rung mạnh] → [rung mạnh] → [rung mạnh] → [liên tục]
Normal Pattern:  [yên tĩnh] → [rung nhẹ] → [yên tĩnh] → [ngẫu nhiên]
Noise Pattern:   [dao động nhỏ] → [không đều] → [không pattern]
```

AI học cách phân biệt các pattern này!

### 3. **Edge Impulse DSP Processing**

Buffer được xử lý qua **DSP** (Digital Signal Processing):

- Spectral Analysis (phân tích tần số)
- Feature Extraction (trích xuất đặc trưng)
- FFT (Fast Fourier Transform)

---

## 📈 Luồng Dữ liệu Hoàn chỉnh

```
MPU6050 Sensor
    ↓ (mỗi 20ms)
Đọc accX, accY, accZ
    ↓
Thêm vào Buffer
    ↓
Buffer[0..449] đầy (sau 3 giây)
    ↓
Edge Impulse AI Processing
    ↓
Kết quả: Attack: 95%, Normal: 3%, Noise: 2%
    ↓
So sánh với threshold (80%)
    ↓
Nếu Attack >= 80% → Kích hoạt alarm!
```

---

## 🔍 Ví dụ Thực tế

### Scenario 1: Phát hiện Attack

```
Time: 0s → Start filling buffer
📊 Buffer: 10%
📊 Buffer: 20%
...
Time: 3s → Buffer full
🧠 Running AI inference...
🚨 ATTACK DETECTED!
   Attack:  95.0%
   Normal:   3.0%
   Noise:    2.0%
📢 Sending MQTT alert...
🔔 Activating buzzer...
```

### Scenario 2: Normal Vibration

```
Time: 0s → Start filling buffer
Time: 3s → Buffer full
🧠 Running AI inference...
✅ Normal Vibration
   Attack:  12.0%
   Normal:  85.0%
   Noise:    3.0%
🟢 System monitoring continues...
```

---

## ⚙️ Tối ưu Buffer

### Tốc độ đọc sensor:

```cpp
const unsigned long AI_PROCESS_INTERVAL = 20; // 50 Hz (50 lần/giây)
```

**Tại sao 20ms?**

- Quá nhanh (10ms): CPU overload, waste power
- Quá chậm (50ms): Mất pattern quan trọng
- **20ms = sweet spot**: Đủ nhanh để bắt pattern, đủ chậm để CPU xử lý

### Reset Buffer:

```cpp
if (buf_idx >= EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE) {
    buf_idx = 0;  // Quay lại đầu buffer
    // Inference xong → Bắt đầu thu thập buffer mới
}
```

---

## 💡 Tóm tắt

| Thuật ngữ     | Giải thích                                     |
| ------------- | ---------------------------------------------- |
| **Buffer**    | Mảng lưu 450 giá trị sensor (150 mẫu × 3 trục) |
| **buf_idx**   | Vị trí hiện tại trong buffer (0-449)           |
| **Frame**     | 1 chuỗi hoàn chỉnh (450 giá trị = 3 giây data) |
| **Sample**    | 1 lần đọc sensor (X, Y, Z)                     |
| **Inference** | Chạy AI để dự đoán (1ms)                       |

**Buffer = Container chứa data cho AI phân tích!**

---

## 🎯 Kết luận

Buffer giống như **cuốn video camera**:

- Recording 3 giây video rung động
- Khi đủ 3 giây → Đưa vào AI phân tích
- AI xem toàn bộ video → Quyết định: Attack hay Normal?

**Không có buffer = Không có data → AI không chạy được!**

---

**Tác giả:** AI Explanation System  
**Ngày:** November 2, 2025  
**Accuracy:** 95.1% (Edge Impulse TinyML)
