OK, bạn đã sẵn sàng ở nhánh `feature/wifimanager` và đã cài thư viện. Đây là kế hoạch "cấy ghép" chi tiết:

## 🚀 Kế hoạch Tích hợp WiFiManager

### Giai đoạn 1: Sửa đổi Biến toàn cục (Global Variables)

Mở file `vibraguard_ai_final.ino` của bạn.

1.  **Thêm thư viện:** Đảm bảo dòng này đã có ở đầu file:
    ```cpp
    #include <WiFiManager.h>
    ```
2.  **Sửa biến MQTT Server:** Tìm các dòng cấu hình MQTT của bạn.
    - **Tìm dòng này:**
      ```cpp
      const char *MQTT_SERVER = "192.168.1.11"; // IP máy chạy Mosquitto
      ```
    - **Sửa thành:**
      ```cpp
      const char *MQTT_SERVER_DEFAULT = "192.168.1.11"; // IP mặc định
      char MQTT_SERVER_BUFFER[40]; // Buffer để lưu IP MQTT mới
      ```
3.  **Sửa biến Trạng thái Wi-Fi:** Tìm dòng này:
    ```cpp
    bool wifiConnected = false;
    ```
    - **Sửa thành (để an toàn hơn):**
    <!-- end list -->
    ```cpp
    bool wifiConnected = false; // Vẫn giữ, nhưng WiFiManager sẽ quản lý chính
    ```

---

### Giai đoạn 2: Thay thế `setup()` (Phần quan trọng nhất)

1.  Đi đến hàm `setup()`.

2.  **Xóa** dòng gọi `connectWiFi();` (Thường ở gần cuối `setup()`).

3.  **Dán** đoạn code `WiFiManager` này vào vị trí của `connectWiFi()`:

    ```cpp
    // === KHỞI TẠO WIFI MANAGER (Nâng cao) ===
    WiFiManager wm;

    // Tạo ô nhập liệu cho IP MQTT Broker
    WiFiManagerParameter custom_mqtt_server("mqtt_ip", "MQTT Server IP", MQTT_SERVER_DEFAULT, 40);
    wm.addParameter(&custom_mqtt_server);

    Serial.println("\nKhoi tao WiFi Manager (voi MQTT IP)...");
    wm.setConfigPortalTimeout(180); // Chờ 180s ở trang cấu hình

    bool res = wm.autoConnect("VibraGuard-Setup"); // Tên Access Point

    if(!res) {
        Serial.println("❌ Cau hinh WiFi that bai (Timeout)! Khoi dong lai...");
        delay(3000);
        ESP.restart();
    }
    else {
        // Nếu kết nối thành công:
        Serial.println("\n✅ WiFi da ket noi!");
        Serial.print("   IP Address: ");
        Serial.println(WiFi.localIP());
        wifiConnected = true; // Đặt cờ báo đã kết nối

        // Lấy giá trị IP MQTT người dùng đã nhập và lưu vào buffer
        strncpy(MQTT_SERVER_BUFFER, custom_mqtt_server.getValue(), sizeof(MQTT_SERVER_BUFFER) - 1);
        MQTT_SERVER_BUFFER[sizeof(MQTT_SERVER_BUFFER) - 1] = '\0'; // Đảm bảo kết thúc chuỗi

        Serial.print("   MQTT Server IP (da luu): ");
        Serial.println(MQTT_SERVER_BUFFER);
    }
    // =====================================
    ```

4.  **Sửa lại `client.setServer()`:** Ngay sau đoạn code `WiFiManager` trên, tìm dòng cấu hình MQTT:

    - **Tìm dòng này:**
      ```cpp
      client.setServer(MQTT_SERVER, MQTT_PORT);
      ```
    - **Sửa thành (để dùng IP mới):**
      ```cpp
      client.setServer(MQTT_SERVER_BUFFER, MQTT_PORT);
      ```

5.  **Sửa lại `printSystemStatus()` (Tùy chọn nhưng nên làm):** Trong `setup()`, tìm dòng `Serial.printf("   MQTT Server: %s:%d\n", MQTT_SERVER, MQTT_PORT);`

    - **Sửa thành:**
      ```cpp
      Serial.printf("   MQTT Server: %s:%d\n", MQTT_SERVER_BUFFER, MQTT_PORT);
      ```

---

### Giai đoạn 3: Dọn dẹp `loop()` và Xóa hàm cũ

1.  **Sửa `loop()`:**
    - Tìm khối `if` kiểm tra Wi-Fi:
      ```cpp
      // 1. Kiểm tra WiFi định kỳ
      if (currentTime - lastWiFiCheck >= WIFI_CHECK_INTERVAL)
      {
          checkWiFiConnection();
          lastWiFiCheck = currentTime;
      }
      ```
    - **Xóa bỏ** toàn bộ khối `if` đó.
    - **Sửa khối MQTT:** Sửa điều kiện `if` của MQTT để nó tự kiểm tra `WiFi.status()`:
      - **Tìm dòng:**
        ```cpp
        if (wifiConnected && currentTime - lastMQTTCheck >= MQTT_CHECK_INTERVAL)
        ```
      - **Sửa thành:**
        ```cpp
        if (WiFi.status() == WL_CONNECTED && currentTime - lastMQTTCheck >= MQTT_CHECK_INTERVAL)
        ```
2.  **Xóa Hàm Cũ:**
    - Cuộn xuống cuối file và **xóa hoàn toàn** hai hàm:
      - `void connectWiFi()`
      - `void checkWiFiConnection()`

---

### Giai đoạn 4: Biên dịch và Test

1.  **Biên dịch (Verify):** Nhấn "Verify". Đảm bảo không có lỗi biên dịch.
2.  **Nạp (Upload):** Nạp code mới lên ESP32.
3.  **Test Lần 1 (Cấu hình):**
    - Mở Serial Monitor (để xem log).
    - Lấy điện thoại/laptop, kết nối vào Wi-Fi `VibraGuard-Setup`.
    - Trang cấu hình sẽ tự mở.
    - Chọn Wi-Fi `LE HUNG`, nhập mật khẩu.
    - Nhập IP MQTT `192.168.1.11` (hoặc IP mới của bạn).
    - Nhấn "Save".
    - Quan sát Serial Monitor xem nó có kết nối Wi-Fi và MQTT thành công không.
4.  **Test Lần 2 (Tự động):**
    - Nhấn nút Reset (RST/EN) trên ESP32.
    - Quan sát Serial Monitor. Lần này nó phải **tự động kết nối** vào `LE HUNG` mà không cần bạn làm gì cả.

Bạn đã hoàn tất\!
