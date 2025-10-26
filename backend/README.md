# VibraGuard Backend

## Giai đoạn 2: Lõi Backend & DB

### Yêu cầu

- Java 17 hoặc cao hơn
- Maven 3.6+
- PostgreSQL (đang chạy qua Docker)

### Cấu trúc dự án

```
backend/
├── src/
│   └── main/
│       ├── java/com/vibraguard/
│       │   ├── VibraGuardApplication.java    # Main class
│       │   ├── controller/                   # REST API Controllers
│       │   │   └── VibrationEventController.java
│       │   ├── model/                        # JPA Entities
│       │   │   ├── Device.java
│       │   │   └── VibrationEvent.java
│       │   ├── repository/                   # Data Access Layer
│       │   │   ├── DeviceRepository.java
│       │   │   └── VibrationEventRepository.java
│       │   ├── service/                      # Business Logic
│       │   │   └── VibrationEventService.java
│       │   └── dto/                          # Data Transfer Objects
│       │       └── VibrationEventDTO.java
│       └── resources/
│           ├── application.properties        # Cấu hình
│           └── init-data.sql                 # Dữ liệu mẫu
└── pom.xml                                   # Maven dependencies
```

### Cấu hình Database

File `application.properties` đã được cấu hình để kết nối với PostgreSQL:

```properties
spring.datasource.url=jdbc:postgresql://localhost:5432/vibraguard_db
spring.datasource.username=myuser
spring.datasource.password=mypassword
```

### Khởi chạy ứng dụng

#### 1. Đảm bảo PostgreSQL đang chạy

```bash
docker ps | grep postgres
```

#### 2. Build project

```bash
cd backend
mvn clean install
```

#### 3. Chạy ứng dụng

```bash
mvn spring-boot:run
```

Hoặc:

```bash
java -jar target/vibraguard-backend-0.0.1-SNAPSHOT.jar
```

Ứng dụng sẽ chạy tại: `http://localhost:8080`

### Thêm dữ liệu mẫu

Kết nối vào PostgreSQL và chạy script `init-data.sql`:

**Cách 1: Sử dụng psql**

```bash
docker exec -i vibraguard-postgres-1 psql -U myuser -d vibraguard_db < src/main/resources/init-data.sql
```

**Cách 2: Sử dụng DBeaver hoặc pgAdmin**

- Kết nối tới `localhost:5432`
- Database: `vibraguard_db`
- Username: `myuser`
- Password: `mypassword`
- Copy nội dung `init-data.sql` và Execute

### API Endpoints

#### 1. Lấy tất cả sự kiện

```
GET http://localhost:8080/api/events
```

**Response:**

```json
[
  {
    "id": 1,
    "deviceId": "ESP32_CUA_SO_01",
    "deviceName": "Cảm biến cửa sổ phòng khách",
    "location": "Phòng khách - Cửa sổ chính",
    "eventTimestamp": "2025-10-26T10:30:00",
    "sensorValue": 1,
    "severity": "WARNING",
    "notes": null
  }
]
```

#### 2. Lấy sự kiện theo Device ID

```
GET http://localhost:8080/api/events/device/{deviceId}
```

### Kiểm thử với Postman

1. Mở Postman
2. Tạo request mới:
   - Method: GET
   - URL: `http://localhost:8080/api/events`
   - Click "Send"
3. Nếu chưa có dữ liệu, response sẽ là `[]`
4. Sau khi thêm dữ liệu mẫu, gọi lại API sẽ thấy danh sách thiết bị

### Database Schema

#### Bảng `devices`

| Column      | Type      | Description        |
| ----------- | --------- | ------------------ |
| id          | BIGSERIAL | Primary Key        |
| device_id   | VARCHAR   | Unique device ID   |
| device_name | VARCHAR   | Tên thiết bị       |
| location    | VARCHAR   | Vị trí             |
| status      | VARCHAR   | ACTIVE/INACTIVE    |
| created_at  | TIMESTAMP | Thời gian tạo      |
| updated_at  | TIMESTAMP | Thời gian cập nhật |

#### Bảng `vibration_events`

| Column          | Type      | Description             |
| --------------- | --------- | ----------------------- |
| id              | BIGSERIAL | Primary Key             |
| device_id       | BIGINT    | Foreign Key             |
| event_timestamp | TIMESTAMP | Thời điểm sự kiện       |
| sensor_value    | INTEGER   | Giá trị cảm biến        |
| severity        | VARCHAR   | NORMAL/WARNING/CRITICAL |
| notes           | VARCHAR   | Ghi chú                 |

### Troubleshooting

**Lỗi kết nối database:**

```
Error: Connection refused
```

→ Kiểm tra PostgreSQL có đang chạy không: `docker ps`

**Lỗi port đã được sử dụng:**

```
Error: Port 8080 is already in use
```

→ Đổi port trong `application.properties`: `server.port=8081`

**Build lỗi:**

```
Error: JAVA_HOME not set
```

→ Cài đặt Java 17 và set JAVA_HOME

### Bước tiếp theo

Sau khi Backend hoạt động, chuyển sang **Giai đoạn 3: Tích hợp MQTT** để kết nối với ESP32.
