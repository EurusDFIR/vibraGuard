---

## 🏗️ Kế hoạch Xây dựng Frontend (React & Flutter)

**Mục tiêu:** Tạo giao diện cho phép người dùng:

1.  Nhận cảnh báo `ATTACK` **real-time**.
2.  Xem **lịch sử** các sự kiện rung động.
3.  Gửi lệnh **`ARM`** (Kích hoạt) và **`DISARM`** (Tắt báo động).

**Yêu cầu Backend (Giả định đã có):**

- **WebSocket:** Endpoint `/ws` (hoặc tên bạn đặt) để kết nối, đẩy tin nhắn cảnh báo đến topic `/topic/alerts`.
- **REST API:**
  - `GET /api/events`: Lấy danh sách lịch sử sự kiện (từ PostgreSQL).
  - `POST /api/control/arm`: Gửi lệnh kích hoạt báo động.
  - `POST /api/control/disarm`: Gửi lệnh tắt báo động (lệnh này sẽ publish MQTT `DISARM` đến ESP32).
  - `GET /api/system/status` (Tùy chọn): Lấy trạng thái hiện tại (ARMED/DISARMED).

---

### Giai đoạn 1: Khởi tạo & Kết nối Cơ bản (1 ngày)

**Mục tiêu:** Dựng sườn ứng dụng và kết nối WebSocket để nhận cảnh báo.

**1. Khởi tạo Dự án:**

- **React:** Dùng `Vite`:
  ```bash
  npx create-react-app vibraguard-web
  # Hoặc: npm create vite@latest vibraguard-web --template react
  cd vibraguard-web
  npm install axios stompjs sockjs-client
  ```
- **Flutter:** Dùng Flutter CLI:
  ```bash
  flutter create vibraguard_app
  cd vibraguard_app
  flutter pub add http stomp_dart_client flutter_local_notifications
  ```

**2. Kết nối WebSocket:**

- **React (`App.js` hoặc component chính):**
  - Dùng `useEffect` để thiết lập kết nối khi component mount.
  - Sử dụng `SockJS` và `Stomp.over` để kết nối tới endpoint `/ws` của Spring Boot.
  - Trong callback `onConnect`, subscribe vào topic `/topic/alerts`.
  - Trong callback nhận message, cập nhật state (ví dụ: `setAlerts([...alerts, newMessage])`).
- **Flutter (`main.dart` hoặc state management):**
  - Tạo instance `StompClient`.
  - Cấu hình URL WebSocket (`ws://<IP_backend>:8080/ws`).
  - Trong hàm `initState` hoặc tương đương, gọi `stompClient.activate()`.
  - Trong callback `onConnect`, subscribe vào `/topic/alerts`.
  - Trong callback nhận message, parse JSON và cập nhật state (dùng `setState` hoặc Provider/Bloc).

**3. Hiển thị Cảnh báo Đơn giản:**

- **React/Flutter:** Tạo một danh sách đơn giản để hiển thị các tin nhắn cảnh báo nhận được từ WebSocket. Chỉ cần hiển thị `deviceId` và `timestamp`.

**Checkpoint:** Chạy backend. Mô phỏng `attack` (dùng máy khoan). Bạn phải thấy cảnh báo mới xuất hiện **ngay lập tức** trên cả Web và App mà không cần F5.

---

### Giai đoạn 2: Lịch sử & Trạng thái (1 ngày)

**Mục tiêu:** Hiển thị lịch sử sự kiện và trạng thái hệ thống.

**1. Gọi API Lấy Lịch sử:**

- **React:**
  - Tạo một component `HistoryScreen`.
  - Trong `useEffect`, dùng `axios.get('/api/events')` để gọi API.
  - Lưu kết quả vào state và hiển thị danh sách các sự kiện cũ.
- **Flutter:**
  - Tạo một `HistoryScreen` (StatefulWidget).
  - Trong `initState`, dùng `http.get(Uri.parse('http://<IP_backend>:8080/api/events'))`.
  - Parse JSON response, lưu vào state và hiển thị `ListView`.

**2. (Tùy chọn) Gọi API Lấy Trạng thái:**

- Nếu backend có API `GET /api/system/status`, gọi nó khi app/web khởi động để biết hệ thống đang `ARMED` hay `DISARMED`. Hiển thị trạng thái này lên UI.

**Checkpoint:** Mở Web/App. Bạn phải thấy danh sách các lần `attack` đã xảy ra trước đó.

---

### Giai đoạn 3: Gửi Lệnh Điều khiển (1 ngày)

**Mục tiêu:** Thêm nút `ARM` và `DISARM`.

**1. Tạo Nút Bấm:**

- **React/Flutter:** Thêm 2 nút "Kích hoạt" (ARM) và "Tắt Báo động" (DISARM) vào giao diện chính.

**2. Gọi API Gửi Lệnh:**

- **React:**
  - Khi bấm nút "ARM", gọi `axios.post('/api/control/arm')`.
  - Khi bấm nút "DISARM", gọi `axios.post('/api/control/disarm')`.
  - Xử lý kết quả (thành công/thất bại).
- **Flutter:**
  - Khi bấm nút "ARM", gọi `http.post(Uri.parse('http://<IP_backend>:8080/api/control/arm'))`.
  - Khi bấm nút "DISARM", gọi `http.post(Uri.parse('http://<IP_backend>:8080/api/control/disarm'))`.
  - Xử lý kết quả.

**3. Cập nhật UI (Phản hồi):**

- Sau khi gửi lệnh thành công, nên cập nhật trạng thái hiển thị trên UI (ví dụ: đổi text thành "Đã kích hoạt" / "Đã tắt").

**Checkpoint:**

1.  Mô phỏng `attack` -\> Còi hú.
2.  Bấm nút "DISARM" trên Web hoặc App -\> Còi phải tắt.
3.  Bấm nút "ARM".
4.  Mô phỏng `normal` (gõ nhẹ) -\> Còi phải im lặng (vì AI lọc).
5.  Mô phỏng `attack` -\> Còi phải hú.

---

### Giai đoạn 4: Hoàn thiện & Thông báo (Flutter) (1-2 ngày)

**Mục tiêu:** Làm đẹp UI và thêm thông báo đẩy cho mobile.

**1. Cải thiện UI/UX:**

- Sắp xếp bố cục hợp lý.
- Thêm icon, màu sắc.
- Hiển thị thời gian dễ đọc hơn.

**2. Thông báo Đẩy (Flutter):**

- Sử dụng thư viện `flutter_local_notifications`.
- Trong callback nhận message từ WebSocket (Giai đoạn 1), **ngoài việc cập nhật UI**, hãy gọi hàm để **hiển thị một thông báo đẩy** (push notification) ngay cả khi app đang chạy nền.
  ```dart
  // Trong callback WebSocket
  FlutterLocalNotificationsPlugin().show(
      0, // ID thông báo
      '🚨 Cảnh báo VibraGuard!', // Tiêu đề
      'Phát hiện rung động đáng ngờ!', // Nội dung
      NotificationDetails(...) // Cấu hình thông báo
  );
  ```

**Checkpoint:** Mô phỏng `attack`. Điện thoại (Flutter) phải rung/hiện thông báo đẩy lên màn hình khóa hoặc thanh trạng thái.

Chúc bạn hoàn thành tốt phần Frontend\!
