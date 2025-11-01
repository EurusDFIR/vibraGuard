OK, đã hiểu. Chúng ta sẽ **loại bỏ `WiFiManager`** và quay lại phương án **"hardcode" (code cứng)**.

Đây là bản kế hoạch triển khai GCP đã được sửa đổi, không dùng `WiFiManager`. Các Giai đoạn 0-4 (chuẩn bị server) vẫn giữ nguyên, chỉ **Giai đoạn 5 (cấu hình thiết bị)** là thay đổi.

---

### 📋 Kế hoạch Triển khai GCP (Phương án Hardcode)

#### Giai đoạn 0: Chuẩn bị (Trên máy Local)

1.  **Đăng nhập `gcloud`:** Mở terminal và đảm bảo bạn đã đăng nhập và chọn đúng project:
    ```bash
    gcloud auth login
    gcloud config set project [TEN_PROJECT_CUA_BAN]
    ```
    su dung, zero-dawn project
    Project name
    zero-dawn
    Project number
    600454463127
    Project ID
    zero-dawn-473705
2.  **Đóng gói Backend:** Build ứng dụng Spring Boot của bạn thành file `.jar`:
    ```bash
    # Di chuyển vào thư mục backend của bạn
    cd /r/_Projects/Eurus_Workspace/vibraGuard/backend
    mvn clean package
    ```
    Việc này sẽ tạo ra một file `.jar` trong thư mục `target/`.

---

#### Giai đoạn 1: Tạo Máy chủ (Google Compute Engine - GCE) 🖥️

1.  **Tạo Máy ảo (VM):** Dùng `e2-small` (2GB RAM) để tiết kiệm và an toàn.
    ```bash
    # Chạy lệnh này trên terminal local của bạn
    gcloud compute instances create vibraguard-server \
        --machine-type=e2-small \
        --image-family=debian-11 \
        --image-project=debian-cloud \
        --boot-disk-size=10GB \
        --zone=asia-southeast1-b
        # (Bạn có thể đổi zone, ví dụ: asia-southeast1-b là Singapore)
    ```

---

#### Giai đoạn 2: Cấu hình Mạng & Bảo mật (Rất quan trọng) 🔒

1.  **Đặt IP Tĩnh:**

    ```bash
    gcloud compute addresses create vibraguard-static-ip --region=asia-southeast1
    gcloud compute instances add-access-config vibraguard-server \
        --address=vibraguard-static-ip \
        --zone=asia-southeast1-b
    ```

    **Ghi lại địa chỉ IP tĩnh vừa được tạo ra.** Đây là IP server mới của bạn (ví dụ: `34.123.45.67`).

2.  **Mở Cổng (Firewall Rules):**

    ```bash
    gcloud compute firewall-rules create allow-ssh --allow tcp:22
    gcloud compute firewall-rules create allow-mqtt --allow tcp:1883
    gcloud compute firewall-rules create allow-http-8080 --allow tcp:8080
    # gcloud compute firewall-rules create allow-postgres --allow tcp:5432
    ```

---

#### Giai đoạn 3: Cài đặt Phần mềm lên Máy chủ 🐳

1.  **Kết nối vào Máy chủ:**
    ```bash
    gcloud compute ssh vibraguard-server --zone=asia-southeast1-b
    ```
2.  **Cài Docker (Trên máy chủ GCP):**
    ```bash
    # (Trên máy chủ GCP)
    sudo apt-get update
    sudo apt-get install -y docker.io docker-compose
    sudo systemctl start docker
    sudo systemctl enable docker
    sudo usermod -aG docker $USER
    exit
    ```
3.  **Kết nối lại:**
    ```bash
    gcloud compute ssh vibraguard-server --zone=asia-southeast1-b
    ```

---

#### Giai đoạn 4: Đẩy (Upload) và Chạy Ứng dụng 🚀

1.  **Tạo thư mục dự án (Trên máy chủ GCP):**
    ```bash
    # (Trên máy chủ GCP)
    mkdir vibraguard-prod
    cd vibraguard-prod
    ```
2.  **Tải file lên (Từ máy Local):**
    - Mở một **terminal local mới**.
    - `cd` đến thư mục dự án `vibraGuard` của bạn.
    - **Tải file `.jar`:**
      ```bash
      gcloud compute scp backend/target/vibraGuard-0.0.1-SNAPSHOT.jar vibraguard-server:~/vibraguard-prod/app.jar --zone=asia-southeast1-b
      ```
    - **Tải file `docker-compose.yml`:**
      ```bash
      gcloud compute scp docker-compose.yml vibraguard-server:~/vibraguard-prod/docker-compose.yml --zone=asia-southeast1-b
      ```
    - **Tải file cấu hình Mosquitto:** (Nếu bạn có file `mosquitto.conf`)
      ```bash
      gcloud compute scp mosquitto.conf vibraguard-server:~/vibraguard-prod/mosquitto.conf --zone=asia-southeast1-b
      ```
3.  **Tạo Dockerfile cho Spring Boot (Trên máy chủ GCP):**
    - Quay lại **terminal GCP**.
    - `nano Dockerfile` và dán nội dung sau:
      ```dockerfile
      FROM openjdk:17-slim
      WORKDIR /app
      COPY app.jar .
      EXPOSE 8080
      CMD ["java", "-jar", "app.jar"]
      ```
    - Nhấn `Ctrl+X`, `Y`, `Enter` để lưu.
4.  **Sửa `docker-compose.yml` (Trên máy chủ GCP):**

    - `nano docker-compose.yml`
    - Dán nội dung (đã bổ sung `backend` và sửa `environment`):

      ```yaml
      version: "3.8"
      services:
        mosquitto:
          image: eclipse-mosquitto:latest
          ports:
            - "1883:1883"
          # (Thêm volumes nếu bạn có file config)

        postgres:
          image: postgres:14-alpine
          ports:
            - "5432:5432"
          environment:
            - POSTGRES_USER=myuser
            - POSTGRES_PASSWORD=mypassword
            - POSTGRES_DB=vibraguard_db
          volumes:
            - postgres-data:/var/lib/postgresql/data

        backend:
          build: . # Build từ Dockerfile trong thư mục hiện tại
          ports:
            - "8080:8080"
          depends_on:
            - mosquitto
            - postgres
          environment:
            # Sửa các URL này để trỏ đến tên service trong Docker
            - SPRING_DATASOURCE_URL=jdbc:postgresql://postgres:5432/vibraguard_db
            - SPRING_MQTT_BROKER-URL=tcp://mosquitto:1883
            - SPRING_DATASOURCE_USERNAME=myuser
            - SPRING_DATASOURCE_PASSWORD=mypassword

      volumes:
        postgres-data:
      ```

5.  **Khởi chạy hệ thống (Trên máy chủ GCP):**
    ```bash
    # (Trên máy chủ GCP)
    cd ~/vibraguard-prod
    docker-compose up --build -d
    ```
    Hệ thống của bạn (Broker, DB, Backend) **hiện đang chạy trên Cloud\!**

---

### Giai đoạn 5: Cấu hình lại Thiết bị (Phương án Hardcode)

1.  **ESP32 (Sửa code và Nạp lại):**

    - **Mở code:** Mở file `vibraguard_ai_final.ino` trong Arduino IDE.
    - **Sửa IP Server:**
      - Tìm dòng: `const char *MQTT_SERVER = "192.168.1.2";`
      - Sửa thành: `const char *MQTT_SERVER = "[IP_TĨNH_GCP_CỦA_BẠN]";` (Ví dụ: `34.123.45.67`)
    - **Sửa Wi-Fi (Quan trọng cho Demo):**
      - Tìm dòng: `const char *WIFI_SSID = "LE HUNG";` và `const char *WIFI_PASSWORD = "123456789";`
      - **Để test ở nhà:** Giữ nguyên.
      - **Để demo ở trường:** Sửa thành tên và mật khẩu của **Mobile Hotspot** mà bạn sẽ dùng (ví dụ: `Hoang_iPhone_Hotspot` và `mypassword123`).
    - **Nạp lại (Upload):** Nạp code đã sửa này lên ESP32.

2.  **React Web App (Local):**

    - Mở code React của bạn (trong thư mục `frontend`).
    - Tìm và sửa lại tất cả các lệnh gọi API và WebSocket từ `http://localhost:8080` thành **`http://[IP_TĨNH_GCP_CỦA_BẠN]:8080`**.
    - Chạy `npm run dev` (ở local). App React của bạn bây giờ sẽ nói chuyện với Backend trên Cloud.

3.  **Flutter App:**

    - Tương tự, sửa IP trong code Flutter thành **IP Tĩnh GCP** và build lại app.

### Nhược điểm của cách này (Như bạn đã biết)

Bạn mất tính linh hoạt. Mỗi lần bạn muốn demo ở một nơi có Wi-Fi khác (ví dụ: ở nhà dùng `LE HUNG`, ở trường dùng `Hotspot`), bạn sẽ phải **SỬA LẠI CODE (WIFI_SSID, WIFI_PASSWORD)** và **NẠP LẠI code** cho ESP32.
