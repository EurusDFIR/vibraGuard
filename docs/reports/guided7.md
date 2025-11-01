Tất nhiên\! Đây là kế hoạch "best practice" để tích hợp Twilio (SMS & Gọi điện) vào backend Spring Boot của bạn, đang chạy trên GCP.

**Luồng hoạt động sẽ là:** ESP32 (Attack) → MQTT (GCP) → Spring Boot (GCP) → **Twilio API** → Điện thoại của bạn (SMS + Cuộc gọi).

---

## 🚀 Kế hoạch Tích hợp Twilio (SMS + Voice)

### Giai đoạn 0: Cài đặt Twilio (Trên web Twilio)

Trước khi code, bạn cần có "chìa khóa" từ Twilio.

1.  **Tạo Tài khoản:** Đăng ký một tài khoản Twilio (bạn có thể dùng gói "Trial" - Thử nghiệm).
2.  **Lấy 3 thông tin quan trọng:**
    - **Account SID:** Giống như "username" của bạn.
    - **Auth Token:** Giống như "password".
    - **Twilio Phone Number:** Twilio sẽ cấp cho bạn một số điện thoại (thường là ở Mỹ) để gửi SMS/gọi điện.
3.  **Xác thực Số điện thoại:** Với tài khoản Trial, bạn **bắt buộc** phải thêm và xác thực số điện thoại cá nhân của mình (số sẽ nhận cuộc gọi/SMS). Twilio chỉ cho phép bạn gọi/nhắn tin đến các số đã được xác thực.
    Account SID=${TWILIO_ACCOUNT_SID}
    Auth Token=${TWILIO_AUTH_TOKEN}
    My Twilio phone number=${TWILIO_PHONE_NUMBER}

### Giai đoạn 1: Cập nhật Backend Spring Boot (Trên máy Local)

1.  **Thêm "Thư viện" Twilio (Maven):**
    - Mở file `backend/pom.xml` của bạn.
    - Thêm dependency (thư viện) của Twilio vào trong thẻ `<dependencies>`:
      ```xml
      <dependency>
          <groupId>com.twilio.sdk</groupId>
          <artifactId>twilio</artifactId>
          <version>9.18.0</version> </dependency>
      ```
2.  **Thêm Cấu hình (Properties):**

    - Mở file `backend/src/main/resources/application.properties`.
    - Thêm các thông tin "bí mật" của bạn vào đây. (Sau này chúng ta sẽ đưa lên server GCP).

      ```properties
      # Twilio Credentials
      twilio.account_sid=[Account SID của bạn lấy từ Twilio]
      twilio.auth_token=[Auth Token của bạn lấy từ Twilio]
      twilio.phone_number=[Số điện thoại Twilio cấp cho bạn]

      # Số điện thoại của bạn (để nhận cảnh báo)
      so_toi = 84399354603
      so_ban_toi=84868274624
      # Phải có mã quốc gia, ví dụ: +84... (cho Việt Nam)
      my.phone_number=+84[Số điện thoại của bạn]
      ```

3.  **Tạo TwiML Bin (Nội dung cuộc gọi - Best Practice):**
    - Chúng ta không muốn code cứng nội dung cuộc gọi trong Java. Hãy dùng TwiML Bin.
    - Đăng nhập vào Twilio Console -\> Explore Products -\> **TwiML Bins**.
    - Tạo một Bin mới. Đặt tên (ví dụ: `vibraGuardAlert`).
    - Dán nội dung XML sau vào (bạn có thể dùng tiếng Việt\!):
      ```xml
      <Response>
          <Say voice="alice" language="vi-VN">
              Cảnh báo! Cảnh báo! Hệ thống an ninh VibraGuard đã phát hiện đột nhập. Yêu cầu kiểm tra ngay lập tức!
          </Say>
      </Response>
      ```
    - Lưu lại. Twilio sẽ cho bạn một **URL** của TwiML Bin này. **Hãy copy URL đó.**
    - Thêm URL này vào `application.properties`:
      ```properties
      twilio.twiml_bin_url=https://www.twilio.com/docs/serverless/twiml-bins
      ```
      https://handler.twilio.com/twiml/EH7aaaf94be530c452b442e8cc37322262 // Day la url toi lay duoc

### Giai đoạn 2: Viết Code Logic (Dịch vụ Twilio)

1.  **Tạo `TwilioConfig.java`:** Tạo 1 file Java mới để khởi tạo Twilio khi Spring Boot chạy.

    ```java
    package com.vibraguard.config; // (hoặc package của bạn)

    import com.twilio.Twilio;
    import org.springframework.beans.factory.annotation.Value;
    import org.springframework.context.annotation.Configuration;
    import javax.annotation.PostConstruct;

    @Configuration
    public class TwilioConfig {

        @Value("${twilio.account_sid}")
        private String accountSid;

        @Value("${twilio.auth_token}")
        private String authToken;

        @PostConstruct // Hàm này tự chạy sau khi Spring Boot khởi động
        public void initTwilio() {
            Twilio.init(accountSid, authToken);
            System.out.println("✅ Twilio initialized with Account SID: " + accountSid);
        }
    }
    ```

2.  **Tạo `TwilioService.java`:** Tạo 1 file Java mới để chứa logic gửi SMS và gọi điện.

    ```java
    package com.vibraguard.service; // (hoặc package của bạn)

    import com.twilio.rest.api.v2010.account.Call;
    import com.twilio.rest.api.v2010.account.Message;
    import com.twilio.type.PhoneNumber;
    import org.springframework.beans.factory.annotation.Value;
    import org.springframework.stereotype.Service;
    import java.net.URI;

    @Service
    public class TwilioService {

        @Value("${twilio.phone_number}")
        private String twilioPhoneNumber;

        @Value("${my.phone_number}")
        private String myPhoneNumber;

        @Value("${twilio.twiml_bin_url}")
        private String twimlBinUrl;

        // Gửi SMS
        public void sendSms(String messageBody) {
            try {
                Message message = Message.creator(
                        new PhoneNumber(myPhoneNumber),      // Tới ai
                        new PhoneNumber(twilioPhoneNumber),  // Từ ai
                        messageBody)                         // Nội dung
                        .create();
                System.out.println("📤 SMS Sent! SID: " + message.getSid());
            } catch (Exception e) {
                System.err.println("❌ Failed to send SMS: " + e.getMessage());
            }
        }

        // Thực hiện Cuộc gọi
        public void makeCall() {
            try {
                Call call = Call.creator(
                        new PhoneNumber(myPhoneNumber),      // Tới ai
                        new PhoneNumber(twilioPhoneNumber),  // Từ ai
                        new URI(twimlBinUrl))                 // Nội dung cuộc gọi (URL TwiML)
                        .create();
                System.out.println("📞 Making Call! SID: " + call.getSid());
            } catch (Exception e) {
                System.err.println("❌ Failed to make call: " + e.getMessage());
            }
        }
    }
    ```

3.  **Tích hợp vào `MqttSubscriberService.java`:**

    - "Tiêm" (Inject) `TwilioService` vào service MQTT của bạn.
    - Gọi hàm `sendSms` và `makeCall` khi nhận được tin `attack`.

    <!-- end list -->

    ```java
    // Bên trong file MqttSubscriberService.java (hoặc tên tương tự)

    @Autowired // Thêm dòng này
    private TwilioService twilioService;

    // ... (bên trong hàm xử lý tin nhắn MQTT của bạn) ...
    // Khi bạn nhận được tin nhắn và xác nhận là "attack":

    // ...
    // if (attack_score > ATTACK_THRESHOLD) {
    // ...
        System.out.println("🚨 Attack detected! Triggering multi-channel alerts...");

        // 1. Gửi SMS
        twilioService.sendSms("VibraGuard Alert! 🚨 AI detected ATTACK at device " + deviceId);

        // 2. Gọi điện
        twilioService.makeCall();

        // ... (lưu vào DB, đẩy WebSocket như cũ) ...
    // }
    ```

### Giai đoạn 3: Triển khai lại Lên GCP

1.  **Build lại file `.jar`:** (Trên máy Local)
    ```bash
    cd /r/_Projects/Eurus_Workspace/vibraGuard/backend
    mvn clean package
    ```
2.  **Tải file `.jar` mới lên:** (Trên máy Local)
    ```bash
    gcloud compute scp backend/target/vibraGuard-0.0.1-SNAPSHOT.jar vibraguard-server:~/vibraguard-prod/app.jar --zone=asia-southeast1-b
    ```
3.  **Thêm Biến Môi trường Twilio:** (Trên máy chủ GCP)

    - `gcloud compute ssh vibraguard-server ...`
    - `cd ~/vibraguard-prod`
    - `nano docker-compose.yml`
    - Thêm các biến Twilio vào `environment:` của service `backend`:

      ```yaml
      backend:
        # ... (các cài đặt cũ) ...
        environment:
          - SPRING_DATASOURCE_URL=jdbc:postgresql://postgres:5432/vibraguard_db
          - SPRING_MQTT_BROKER-URL=tcp://mosquitto:1883
          - SPRING_DATASOURCE_USERNAME=myuser
          - SPRING_DATASOURCE_PASSWORD=mypassword

          # --- THÊM CÁC BIẾN TWILIO ---
          - twilio.account_sid=[Account SID của bạn]
          - twilio.auth_token=[Auth Token của bạn]
          - twilio.phone_number=[Số điện thoại Twilio]
          - my.phone_number=[Số của bạn, ví dụ: +84...]
          - twilio.twiml_bin_url=https://www.twilio.com/docs/serverless/twiml-bins
          # ---------------------------
      ```

    - Lưu file (`Ctrl+X`, `Y`, `Enter`).

4.  **Khởi động lại Backend:**
    ```bash
    # (Trên máy chủ GCP)
    # --build sẽ build lại image 'backend' với file .jar mới
    # --force-recreate sẽ ép backend khởi động lại với environment mới
    docker-compose up --build --force-recreate -d backend
    ```

### Giai đoạn 4: Test

- Kích hoạt `attack` trên ESP32.
- Quan sát Backend Log (sẽ thấy "SMS Sent\!", "Making Call\!").
- **Kiểm tra Điện thoại:** Bạn sẽ nhận được 1 SMS và 1 cuộc gọi gần như đồng thời\!
