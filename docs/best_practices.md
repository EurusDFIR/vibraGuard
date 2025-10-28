# VibraGuard AI - Best Practice Guide

## 🎯 Vấn đề: Feature Overlap trong Edge Impulse

### Triệu chứng:

- Biểu đồ Feature Explorer cho thấy các nhãn attack/normal/noise bị **chồng lấn**
- AI không thể phân biệt được giữa "tấn công" và "bình thường"
- Độ chính xác huấn luyện thấp (<70%)

### Nguyên nhân:

**❌ Tần số lấy mẫu quá thấp (10Hz với delay(100))**

Rung động có tần số cao (50-200Hz):

- Khoan: ~80-150Hz
- Đập: ~50-100Hz
- Gõ cửa: ~30-80Hz

Với 10Hz, bạn chỉ "chụp" được 10-20% thông tin thực tế → Mất "dấu vân tay" độc đáo của từng loại rung!

---

## ✅ Giải pháp Best Practice

### 1. Tần số Lấy mẫu (Sampling Rate)

**Quy tắc Nyquist:** Tần số lấy mẫu ≥ 2× tần số tín hiệu

| Loại rung động     | Tần số thực | Tần số lấy mẫu tối thiểu |
| ------------------ | ----------- | ------------------------ |
| Noise (môi trường) | 0-20Hz      | 40Hz                     |
| Normal (gõ cửa)    | 30-80Hz     | 160Hz                    |
| Attack (khoan)     | 80-150Hz    | 300Hz                    |

**Khuyến nghị thực tế:**

- ✅ **50-100Hz**: Đủ cho hầu hết ứng dụng
- ✅ **Code:** `delay(20)` = 50Hz hoặc `delay(10)` = 100Hz
- ⚠️ Trên 100Hz: MPU6050 có thể không kịp xử lý

### 2. Window Size & Overlap

**Trong Edge Impulse - Create Impulse:**

- Window size: `2000ms` (2 giây)
- Window increase: `500ms` (overlap 75%)
- Frequency: `50Hz` hoặc `100Hz`

**Tại sao?**

- 2 giây đủ để capture toàn bộ một cú đập/khoan
- Overlap 75% tạo nhiều training samples hơn
- 50-100Hz capture đủ chi tiết tần số

### 3. Feature Engineering

**Processing Block: Spectral Analysis**

Cấu hình:

```
Frame length: 2 seconds
Frame stride: 0.5 seconds
FFT length: 128 or 256
Filter type: Low-pass (cutoff ~25Hz)
```

**Tại sao dùng Spectral Analysis?**

- Chuyển tín hiệu thời gian → tần số
- Attack có "chữ ký" tần số riêng biệt
- Tách nhiễu tốt hơn raw data

### 4. Data Collection Strategy

**Số lượng samples:**

- Noise: 5-10 samples (10%)
- Normal: 15-20 samples (45%)
- Attack: 15-20 samples (45%)

**Kỹ thuật thu thập:**

**Normal:**

- Gõ cửa (mạnh/nhẹ/trung bình)
- Đóng/mở cửa
- Vỗ tay, bước chân
- Đa dạng hóa!

**Attack:**

- Đập mạnh liên tục (búa cao su)
- Mô phỏng khoan (đập ngắn, nhanh)
- Cạy/lắc mạnh
- Thử nhiều góc độ!

**Noise:**

- Yên lặng hoàn toàn
- Chỉ rung động nền

### 5. Neural Network Configuration

**Best practice cho ESP32:**

```
Input layer: Auto (từ spectral features)
Hidden layers: 20-30 neurons, 1-2 layers
Output layer: 3 (attack, normal, noise)
Epochs: 50-100
Learning rate: 0.001
```

**Tối ưu cho ESP32:**

- Chọn "int8" quantization khi deploy
- RAM usage < 50KB
- Latency < 100ms

---

## 📋 Checklist Thực Hiện

### Phase 1: Sửa Code

- [ ] Thay `delay(100)` → `delay(20)` trong data_forwarder.ino
- [ ] Upload code mới lên ESP32
- [ ] Verify Serial Monitor thấy tốc độ nhanh hơn

### Phase 2: Reset Dữ liệu

- [ ] Vào Edge Impulse → Data acquisition
- [ ] Xóa toàn bộ dữ liệu cũ (10Hz)
- [ ] Verify: 0 samples

### Phase 3: Thu thập lại (50Hz)

- [ ] Chạy: `edge-impulse-data-forwarder`
- [ ] Noise: 5-10 samples × 10s
- [ ] Normal: 15-20 samples × 10s (đa dạng!)
- [ ] Attack: 15-20 samples × 10s (mạnh!)

### Phase 4: Train Model

- [ ] Create Impulse: Window 2000ms, Increase 500ms, 50Hz
- [ ] Spectral Analysis: FFT 128, Low-pass 25Hz
- [ ] Generate features → Verify clusters tách biệt
- [ ] NN Classifier: 20 neurons, 1 layer, 100 epochs
- [ ] Train → Verify accuracy >85%

### Phase 5: Deploy

- [ ] Deployment → Arduino library
- [ ] Download .zip
- [ ] Import vào Arduino IDE
- [ ] Test với code integration

---

## 🎯 Kết quả Mong đợi

### Feature Explorer (sau khi sửa):

```
🔵 Attack: Cụm riêng biệt (góc trên phải)
🟢 Normal: Cụm giữa (tách biệt attack)
🟠 Noise: Cụm dưới trái (xa 2 cụm kia)
```

### Accuracy:

- Training: >85%
- Validation: >80%
- Test: >75%

### Confusion Matrix:

```
           Predicted
         A    N    S
Actual A [90%  8%  2%]
       N [ 5% 92%  3%]
       S [ 2%  3% 95%]
```

---

## 🚨 Troubleshooting

### Vẫn bị overlap sau khi sửa?

**✅ Bạn đang ở đây - Đã cải thiện 70-80% nhưng vẫn có overlap nhẹ giữa Attack và Normal**

#### **Giải pháp nâng cao:**

**1. Thu thập thêm samples "cực đoan"** (QUAN TRỌNG NHẤT!)

- **Normal - Thêm 5-10 samples:**

  - Gõ cửa CỰC MẠNH (giống đập nhưng ngắn hơn)
  - Gõ cửa CỰC NHẸ (gần như chạm)
  - Đóng cửa từ từ vs đóng cửa mạnh
  - Vỗ tay gần/xa cửa
  - Bước chân nặng gần cửa

- **Attack - Thêm 5-10 samples:**
  - Đập liên tục NHANH (mô phỏng khoan tốc độ cao)
  - Đập MẠNH từng cái (búa lớn)
  - Cạy cửa (lắc mạnh liên tục)
  - Thử nhiều vị trí: góc cửa, giữa cửa, khung cửa

**2. Tăng Window Size**

- Thay đổi từ 2000ms → **3000ms** (3 giây)
- Attack thường kéo dài hơn 2 giây
- Capture đủ "pattern" của cả chuỗi đập

**3. Thử FFT cao hơn**

- FFT 128 → **FFT 256**
- Phân tích tần số chi tiết hơn
- Tốn RAM hơn nhưng chính xác hơn

**4. Balance Dataset**

- Đảm bảo: Attack samples = Normal samples
- Hiện tại nếu Normal > Attack → AI bias về Normal
- Kiểm tra: Data Acquisition → Class distribution

**5. Thử thêm Processing Block**

- Ngoài Spectral Analysis, thử thêm **Raw Data**
- Một số features từ raw data (peak, RMS) giúp phân biệt

#### **Quy trình tối ưu (30 phút):**

```
1. Xóa samples "애매" (ambiguous) - giữ lại samples rõ ràng
2. Thu thập thêm 5-10 "extreme samples" cho mỗi class
3. Tăng Window Size lên 3000ms
4. Thử FFT 256
5. Generate features lại
6. Train với 150 epochs
```

**Mục tiêu:**

- Attack và Normal: **tách biệt ít nhất 80%**
- Noise: **tách biệt hoàn toàn 100%**

---

### Accuracy thấp (<70%)?

1. **Thu thập thêm samples** (mỗi loại 20-30)
2. **Đa dạng hóa actions** (nhiều góc độ, lực khác nhau)
3. **Tăng window size** lên 3000ms
4. **Thử FFT 256** thay vì 128
5. **Kiểm tra data quality** trong Raw Data tab
6. **Tăng epochs** lên 150-200
7. **Thêm hidden layer** (20→30 neurons)
8. **Balance dataset** (attack = normal)

### ESP32 crash khi chạy AI?

1. **Giảm FFT** xuống 64
2. **Giảm neurons** xuống 15
3. **Enable int8 quantization**
4. **Giảm window size** xuống 1500ms

---

## 🎓 Tài liệu Tham khảo

1. **Nyquist-Shannon Sampling Theorem**
2. **Edge Impulse Documentation: Spectral Analysis**
3. **MPU6050 Datasheet**: Max sampling 1kHz
4. **Vibration Analysis Standards**: ISO 10816

---

**Thời gian thực hiện:** 30-45 phút
**Độ khó:** ⭐⭐⭐ (Trung bình)
**Kết quả:** AI phân loại chính xác >85%
