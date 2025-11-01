# Hướng dẫn Kết nối DBeaver với PostgreSQL trên GCP

> ✅ **Trạng thái:** Firewall và Database đã được cấu hình hoàn chỉnh!  
> 🚀 **Hành động:** Bạn chỉ cần mở DBeaver và kết nối (Bước 3)

---

## 📋 Thông tin Kết nối

| **Thông số** | **Giá trị**                     |
| ------------ | ------------------------------- |
| **Host**     | `34.87.133.103` (GCP Static IP) |
| **Port**     | `5432`                          |
| **Database** | `vibraguard_db`                 |
| **Username** | `vibraguard_user`               |
| **Password** | `vibraguard_pass`               |
| **Driver**   | PostgreSQL                      |

> ⚠️ **Lưu ý:** Username/Password trên VM **KHÁC** với file `docker-compose.yml` local!

---

## 🔧 Bước 1: Cài đặt DBeaver

### Windows:

1. Tải DBeaver Community Edition: https://dbeaver.io/download/
2. Chạy file installer `.exe`
3. Cài đặt với cấu hình mặc định

### Linux/Mac:

```bash
# Ubuntu/Debian
sudo snap install dbeaver-ce

# macOS (Homebrew)
brew install --cask dbeaver-community
```

---

## 🌐 Bước 2: Kiểm tra Firewall GCP (✅ Đã hoàn thành!)

**Firewall đã được cấu hình sẵn! Bạn không cần làm gì thêm.**

### Xác nhận Firewall Rule:

```bash
# Kiểm tra rule hiện tại (optional)
gcloud compute firewall-rules describe allow-postgres-external
```

**Kết quả đã verify:**

```yaml
name: allow-postgres-external
allowed:
  - IPProtocol: tcp
    ports: ["5432"]
sourceRanges: ["0.0.0.0/0"] # Cho phép từ mọi IP
direction: INGRESS
disabled: false
```

### Trạng thái Hệ thống:

✅ **Firewall:** Port 5432 đã mở (`allow-postgres-external`)  
✅ **PostgreSQL Container:** Đang chạy và bind `0.0.0.0:5432`  
✅ **Database:** `vibraguard_db` có 2 bảng (`devices`, `vibration_events`)  
✅ **Dữ liệu:** 25 events đã được lưu

**Bạn có thể bỏ qua phần này và chuyển sang Bước 3 ngay!**

---

## 🔌 Bước 3: Kết nối DBeaver

### 3.1. Tạo Connection mới

1. Mở DBeaver
2. Click **Database** → **New Database Connection** (hoặc `Ctrl+Shift+N`)
3. Chọn **PostgreSQL**
4. Click **Next**

### 3.2. Cấu hình Connection

**Main Tab:**

```
Host: 34.87.133.103
Port: 5432
Database: vibraguard_db
Username: vibraguard_user
Password: vibraguard_pass
```

**Screenshot tham khảo:**

```
┌─────────────────────────────────────────────┐
│ Connection Settings                          │
├─────────────────────────────────────────────┤
│ Host:     [34.87.133.103            ]       │
│ Port:     [5432                     ]       │
│ Database: [vibraguard_db            ]       │
│ Username: [vibraguard_user          ]       │
│ Password: [••••••••••••••••         ]       │
│                                              │
│ ☑ Show all databases                        │
│ ☐ Use SSH Tunnel                            │
└─────────────────────────────────────────────┘
```

### 3.3. Test Connection

1. Click **Test Connection...**
2. **Nếu thành công:** Thấy popup `Connected (Connection time: XX ms)`
3. **Nếu thất bại:** Xem phần **Troubleshooting** bên dưới
4. Click **Finish** để lưu connection

---

## ✅ Bước 4: Kiểm tra Database hoạt động

### 4.1. Mở SQL Editor

- Right-click vào connection `vibraguard_db` → **SQL Editor** → **New SQL Script**

### 4.2. Truy vấn Cơ bản

#### **1. Kiểm tra các bảng trong database**

```sql
-- Liệt kê tất cả bảng
SELECT table_name
FROM information_schema.tables
WHERE table_schema = 'public';
```

**Kết quả mong đợi:**

```
table_name
-----------------------
devices
vibration_events
```

---

#### **2. Kiểm tra cấu trúc bảng `devices`**

```sql
-- Xem schema của bảng devices
SELECT
    column_name,
    data_type,
    character_maximum_length,
    is_nullable,
    column_default
FROM information_schema.columns
WHERE table_name = 'devices'
ORDER BY ordinal_position;
```

**Kết quả mong đợi:**

```
column_name     | data_type | is_nullable | column_default
----------------+-----------+-------------+--------------------
id              | bigint    | NO          | nextval(...)
device_id       | varchar   | NO          | NULL
location        | varchar   | YES         | NULL
status          | varchar   | YES         | NULL
last_heartbeat  | timestamp | YES         | NULL
created_at      | timestamp | YES         | CURRENT_TIMESTAMP
```

---

#### **3. Kiểm tra cấu trúc bảng `vibration_events`**

```sql
-- Xem schema của bảng vibration_events
\d vibration_events

-- Hoặc dùng SQL standard:
SELECT
    column_name,
    data_type,
    is_nullable,
    column_default
FROM information_schema.columns
WHERE table_name = 'vibration_events'
ORDER BY ordinal_position;
```

**Kết quả thực tế:**

```
column_name      | data_type                      | is_nullable | column_default
-----------------+--------------------------------+-------------+--------------------
id               | bigint                         | NO          | nextval(...)
event_timestamp  | timestamp(6) without time zone | NO          |
notes            | varchar(500)                   | YES         | NULL
sensor_value     | integer                        | YES         | NULL
severity         | varchar(255)                   | YES         | NULL
device_id        | bigint                         | NO          | (FK to devices.id)
```

> ⚠️ **Lưu ý:** Bảng này có cấu trúc KHÁC với guide ban đầu:
>
> - Dùng `event_timestamp` (không phải `timestamp`)
> - Dùng `notes` (chứa "AI Attack Detected - Confidence: XX%")
> - Không có các cột riêng: `attack_score`, `normal_score`, `noise_score`, `predicted_label`, `confidence`
> - `device_id` là **foreign key** (bigint) trỏ đến `devices.id`

---

### 4.3. Truy vấn Dữ liệu Thực tế

#### **4. Xem tất cả thiết bị đã đăng ký**

```sql
SELECT * FROM devices ORDER BY id DESC;
```

**Kết quả thực tế:**

```
id | device_name      | location    | is_armed | created_at          | updated_at
---+------------------+-------------+----------+---------------------+---------------------
1  | ESP32_CUA_SO_01  | Window #1   | true     | 2025-10-30 14:20:10 | 2025-10-31 15:02:51
```

---

#### **5. Xem tất cả sự kiện rung động (Mới nhất trước)**

```sql
SELECT
    id,
    device_id,
    event_timestamp,
    notes,
    severity,
    sensor_value
FROM vibration_events
ORDER BY event_timestamp DESC
LIMIT 20;
```

**Kết quả thực tế:**

```
id | device_id | event_timestamp         | notes                                  | severity | sensor_value
---+-----------+-------------------------+----------------------------------------+----------+--------------
25 | 1         | 2025-10-31 15:02:51.729 | AI Attack Detected - Confidence: 85.2% | WARNING  | 1
24 | 1         | 2025-10-31 14:39:17.374 | AI Attack Detected - Confidence: 99.6% | WARNING  | 1
23 | 1         | 2025-10-31 14:30:09.694 | AI Attack Detected - Confidence: 96.5% | WARNING  | 1
```

**Kết quả thực tế:**

```
id | device_id | event_timestamp         | notes                                  | severity | sensor_value
---+-----------+-------------------------+----------------------------------------+----------+--------------
25 | 1         | 2025-10-31 15:02:51.729 | AI Attack Detected - Confidence: 85.2% | WARNING  | 1
24 | 1         | 2025-10-31 14:39:17.374 | AI Attack Detected - Confidence: 99.6% | WARNING  | 1
23 | 1         | 2025-10-31 14:30:09.694 | AI Attack Detected - Confidence: 96.5% | WARNING  | 1
22 | 1         | 2025-10-31 09:02:14.409 | AI Attack Detected - Confidence: 99.6% | WARNING  | 1
21 | 1         | 2025-10-31 09:00:33.619 | AI Attack Detected - Confidence: 98.2% | WARNING  | 98
```

---

#### **6. Chỉ xem các sự kiện ATTACK (Trích xuất Confidence từ notes)**

```sql
SELECT
    id,
    device_id,
    event_timestamp,
    notes,
    severity,
    -- Trích xuất giá trị confidence từ chuỗi notes
    CAST(
        SUBSTRING(notes FROM 'Confidence: ([0-9.]+)%')
        AS NUMERIC
    ) AS confidence_percent
FROM vibration_events
WHERE notes LIKE '%AI Attack Detected%'
ORDER BY event_timestamp DESC;
```

**Kết quả:**

```
id | device_id | event_timestamp         | notes                                  | severity | confidence_percent
---+-----------+-------------------------+----------------------------------------+----------+-------------------
25 | 1         | 2025-10-31 15:02:51.729 | AI Attack Detected - Confidence: 85.2% | WARNING  | 85.2
24 | 1         | 2025-10-31 14:39:17.374 | AI Attack Detected - Confidence: 99.6% | WARNING  | 99.6
23 | 1         | 2025-10-31 14:30:09.694 | AI Attack Detected - Confidence: 96.5% | WARNING  | 96.5
```

---

#### **7. Thống kê số lượng attack theo confidence range**

```sql
SELECT
    CASE
        WHEN CAST(SUBSTRING(notes FROM 'Confidence: ([0-9.]+)%') AS NUMERIC) >= 95 THEN '95-100%'
        WHEN CAST(SUBSTRING(notes FROM 'Confidence: ([0-9.]+)%') AS NUMERIC) >= 90 THEN '90-95%'
        WHEN CAST(SUBSTRING(notes FROM 'Confidence: ([0-9.]+)%') AS NUMERIC) >= 85 THEN '85-90%'
        WHEN CAST(SUBSTRING(notes FROM 'Confidence: ([0-9.]+)%') AS NUMERIC) >= 80 THEN '80-85%'
        ELSE '<80%'
    END AS confidence_range,
    COUNT(*) AS total_attacks,
    AVG(CAST(SUBSTRING(notes FROM 'Confidence: ([0-9.]+)%') AS NUMERIC)) AS avg_confidence
FROM vibration_events
WHERE notes LIKE '%AI Attack Detected%'
GROUP BY confidence_range
ORDER BY confidence_range DESC;
```

**Kết quả mẫu:**

```
confidence_range | total_attacks | avg_confidence
-----------------+---------------+----------------
95-100%          | 12            | 97.8
90-95%           | 6             | 92.3
85-90%           | 5             | 87.1
80-85%           | 2             | 83.5
```

---

#### **8. Xem sự kiện trong 24 giờ qua**

```sql
SELECT
    id,
    device_id,
    event_timestamp,
    notes,
    severity
FROM vibration_events
WHERE event_timestamp >= NOW() - INTERVAL '24 hours'
ORDER BY event_timestamp DESC;
```

---

#### **9. JOIN với bảng devices để xem thông tin chi tiết**

```sql
SELECT
    ve.id,
    d.device_name,
    d.location,
    ve.event_timestamp,
    ve.notes,
    ve.severity,
    CAST(SUBSTRING(ve.notes FROM 'Confidence: ([0-9.]+)%') AS NUMERIC) AS confidence
FROM vibration_events ve
JOIN devices d ON ve.device_id = d.id
WHERE ve.notes LIKE '%AI Attack Detected%'
ORDER BY ve.event_timestamp DESC
LIMIT 10;
```

**Kết quả:**

```
id | device_name      | location  | event_timestamp         | notes                                  | severity | confidence
---+------------------+-----------+-------------------------+----------------------------------------+----------+-----------
25 | ESP32_CUA_SO_01  | Window #1 | 2025-10-31 15:02:51.729 | AI Attack Detected - Confidence: 85.2% | WARNING  | 85.2
24 | ESP32_CUA_SO_01  | Window #1 | 2025-10-31 14:39:17.374 | AI Attack Detected - Confidence: 99.6% | WARNING  | 99.6
```

---

#### **10. Tính tổng số attack và attack rate**

```sql
SELECT
    COUNT(*) AS total_events,
    SUM(CASE WHEN notes LIKE '%AI Attack Detected%' THEN 1 ELSE 0 END) AS total_attacks,
    ROUND(
        100.0 * SUM(CASE WHEN notes LIKE '%AI Attack Detected%' THEN 1 ELSE 0 END) / COUNT(*),
        2
    ) AS attack_rate_percent
FROM vibration_events;
```

**Kết quả mẫu:**

```
total_events | total_attacks | attack_rate_percent
-------------+---------------+--------------------
25           | 25            | 100.00
```

> ⚠️ Nếu attack_rate = 100%, có thể do chỉ lưu attack events (normal/noise không lưu DB).

---

#### **11. Tìm device có nhiều attack nhất**

```sql
SELECT
    d.device_name,
    d.location,
    COUNT(*) AS attack_count,
    MAX(ve.event_timestamp) AS last_attack_time
FROM vibration_events ve
JOIN devices d ON ve.device_id = d.id
WHERE ve.notes LIKE '%AI Attack Detected%'
GROUP BY d.device_name, d.location
ORDER BY attack_count DESC;
```

---

#### **12. Xem xu hướng attack theo giờ trong ngày**

```sql
SELECT
    EXTRACT(HOUR FROM event_timestamp) AS hour_of_day,
    COUNT(*) AS attack_count
FROM vibration_events
WHERE notes LIKE '%AI Attack Detected%'
GROUP BY hour_of_day
ORDER BY hour_of_day;
```

**Kết quả mẫu:**

```
hour_of_day | attack_count
------------+--------------
9           | 3
14          | 2
15          | 1
```

---

#### **13. Attack với confidence thấp (có thể false alarm)**

```sql
SELECT
    id,
    event_timestamp,
    notes,
    CAST(SUBSTRING(notes FROM 'Confidence: ([0-9.]+)%') AS NUMERIC) AS confidence
FROM vibration_events
WHERE notes LIKE '%AI Attack Detected%'
  AND CAST(SUBSTRING(notes FROM 'Confidence: ([0-9.]+)%') AS NUMERIC) < 85.0
ORDER BY confidence ASC;
```

---

#### **14. Báo cáo hàng ngày**

```sql
SELECT
    DATE(event_timestamp) AS date,
    COUNT(*) AS total_events,
    COUNT(CASE WHEN notes LIKE '%AI Attack Detected%' THEN 1 END) AS attacks,
    ROUND(AVG(CAST(SUBSTRING(notes FROM 'Confidence: ([0-9.]+)%') AS NUMERIC)), 2) AS avg_confidence
FROM vibration_events
WHERE event_timestamp >= NOW() - INTERVAL '7 days'
GROUP BY DATE(event_timestamp)
ORDER BY date DESC;
```

**Kết quả mẫu:**

```
date       | total_events | attacks | avg_confidence
-----------+--------------+---------+----------------
2025-10-31 | 18           | 18      | 94.23
2025-10-30 | 7            | 7       | 91.85
```

---

## 🚨 Troubleshooting (Xử lý Lỗi)

### Lỗi 1: "Connection refused" / "Connection timed out"

**Nguyên nhân:** Firewall chặn port 5432

**Giải pháp:**

```bash
# SSH vào GCP VM
gcloud compute ssh vibraguard-server --zone=asia-southeast1-b

# Kiểm tra PostgreSQL đang chạy
docker ps | grep postgres

# Kiểm tra port đang listen
sudo netstat -tlnp | grep 5432

# Nếu không thấy, restart Docker Compose
cd /home/[YOUR_USER]/vibraguard
docker-compose restart postgres

# Kiểm tra firewall rule
gcloud compute firewall-rules list | grep 5432
```

---

### Lỗi 2: "Authentication failed for user 'myuser'"

**Nguyên nhân:** Sai username/password

**Giải pháp:**

```bash
# SSH vào VM, kiểm tra docker-compose.yml
cat docker-compose.yml | grep -A 5 postgres

# Hoặc kiểm tra biến môi trường container
docker exec vibraguard-postgres-1 env | grep POSTGRES
```

---

### Lỗi 3: "Database 'vibraguard_db' does not exist"

**Nguyên nhân:** Database chưa được tạo

**Giải pháp:**

```bash
# SSH vào VM
docker exec -it vibraguard-postgres-1 psql -U myuser -c "\l"

# Nếu không thấy vibraguard_db, tạo thủ công:
docker exec -it vibraguard-postgres-1 psql -U myuser -c "CREATE DATABASE vibraguard_db;"

# Hoặc restart toàn bộ để Spring Boot tự tạo
docker-compose restart backend
```

---

### Lỗi 4: "SSL connection required"

**Nguyên nhân:** PostgreSQL yêu cầu SSL (không phải lỗi trong setup này)

**Giải pháp trong DBeaver:**

- Tab **SSL** → Chọn `disable` hoặc `allow`

---

### Lỗi 5: Bảng rỗng (không có dữ liệu)

**Nguyên nhân:** ESP32 chưa gửi dữ liệu hoặc Backend chưa lưu

**Giải pháp:**

```bash
# Kiểm tra log Backend
docker logs vibraguard-backend-1 --tail 50

# Kiểm tra ESP32 đã gửi MQTT chưa
docker logs vibraguard-mosquitto-1 --tail 50

# Test MQTT bằng MQTT Explorer:
# - Host: 34.87.133.103
# - Port: 1883
# - Subscribe: vibra_guard/#
```

---

## 📊 Screenshot Minh chứng

### 1. Connection thành công

![DBeaver Connected](image_e85084.png)

### 2. Xem bảng vibration_events

![Events Table](image_to_be_captured.png)

### 3. Xem attack events

![Attack Events](image_to_be_captured.png)

---

## 📚 Tài liệu Tham khảo

- **DBeaver Documentation:** https://dbeaver.io/docs/
- **PostgreSQL JDBC Driver:** https://jdbc.postgresql.org/
- **GCP Firewall Rules:** https://cloud.google.com/vpc/docs/firewalls

---

## ✅ Checklist Kiểm tra

- [ ] Đã mở port 5432 trên GCP Firewall
- [ ] DBeaver kết nối thành công (`Test Connection` pass)
- [ ] Thấy 2 bảng: `devices`, `vibration_events`
- [ ] Bảng `vibration_events` có dữ liệu (ít nhất 1 dòng)
- [ ] Truy vấn SELECT thành công
- [ ] Thấy attack events với confidence >= 0.80
- [ ] Đã export CSV báo cáo (nếu cần)

---

**Tác giả:** VibraGuard AI Team  
**Ngày cập nhật:** 01/11/2025  
**Phiên bản:** 1.0
