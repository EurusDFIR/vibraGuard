# 📊 DATABASE ACCESS GUIDE

## Thông tin Database

**Database:** PostgreSQL 15  
**Location:** GCP VM (34.87.133.103)  
**Container Name:** postgres

### Credentials

```
Host: 34.87.133.103
Port: 5432
Database: vibraguard_db
Username: vibraguard_user
Password: vibraguard_pass
```

---

## 🔌 Cách 1: Kết nối từ GCP VM (Khuyến nghị)

### Bước 1: SSH vào GCP VM

```bash
gcloud compute ssh vibraguard-server --zone=asia-southeast1-b
```

### Bước 2: Kết nối PostgreSQL

```bash
docker exec -it postgres psql -U vibraguard_user -d vibraguard_db
```

### Bước 3: Thoát khi xong

```sql
\q
```

---

## 💻 Cách 2: Kết nối từ máy Local

### Bước 1: Mở port PostgreSQL trên GCP

```bash
gcloud compute firewall-rules create allow-postgres \
  --allow tcp:5432 \
  --source-ranges 0.0.0.0/0 \
  --description "Allow PostgreSQL access"
```

### Bước 2: Kết nối bằng psql

```bash
psql -h 34.87.133.103 -p 5432 -U vibraguard_user -d vibraguard_db
# Nhập password: vibraguard_pass
```

---

## 🖥️ Cách 3: Dùng GUI Tool (DBeaver hoặc pgAdmin)

### DBeaver (Khuyến nghị)

1. **Download:** https://dbeaver.io/download/
2. **New Connection** → PostgreSQL
3. **Điền thông tin:**
   - Host: `34.87.133.103`
   - Port: `5432`
   - Database: `vibraguard_db`
   - Username: `myuser`
   - Password: `mypassword`
4. **Test Connection** → **Finish**

### pgAdmin

1. **Download:** https://www.pgadmin.org/download/
2. **Add New Server:**
   - Name: `VibraGuard GCP`
   - Host: `34.87.133.103`
   - Port: `5432`
   - Database: `vibraguard_db`
   - Username: `vibraguard_user`
   - Password: `vibraguard_pass`

---

## 📝 Các câu lệnh SQL hữu ích

### 1. Xem tất cả các bảng

```sql
\dt
-- Hoặc
SELECT table_name FROM information_schema.tables
WHERE table_schema = 'public';
```

### 2. Xem cấu trúc bảng

```sql
-- Xem cấu trúc bảng vibration_events
\d vibration_events

-- Xem chi tiết tất cả columns
SELECT column_name, data_type, character_maximum_length, is_nullable
FROM information_schema.columns
WHERE table_name = 'vibration_events';
```

### 3. Kiểm tra dữ liệu

#### Đếm số lượng events

```sql
SELECT COUNT(*) as total_events FROM vibration_events;
```

#### Xem 10 events mới nhất

```sql
SELECT * FROM vibration_events
ORDER BY timestamp DESC
LIMIT 10;
```

#### Xem events theo device

```sql
SELECT device_id, COUNT(*) as event_count
FROM vibration_events
GROUP BY device_id;
```

#### Xem events hôm nay

```sql
SELECT * FROM vibration_events
WHERE DATE(timestamp) = CURRENT_DATE
ORDER BY timestamp DESC;
```

### 4. Lọc theo severity

```sql
-- Xem các critical alerts
SELECT * FROM vibration_events
WHERE severity = 'CRITICAL'
ORDER BY timestamp DESC;

-- Đếm theo severity
SELECT severity, COUNT(*) as count
FROM vibration_events
GROUP BY severity;
```

### 5. Xem với confidence cao

```sql
SELECT * FROM vibration_events
WHERE ai_confidence >= 90
ORDER BY timestamp DESC
LIMIT 20;
```

### 6. Thống kê theo ngày

```sql
SELECT
    DATE(timestamp) as date,
    COUNT(*) as total_events,
    AVG(ai_confidence) as avg_confidence
FROM vibration_events
GROUP BY DATE(timestamp)
ORDER BY date DESC;
```

### 7. Xem devices đã đăng ký

```sql
SELECT * FROM devices;

-- Nếu có bảng devices, xem chi tiết
SELECT
    d.device_id,
    d.device_name,
    COUNT(v.id) as total_events
FROM devices d
LEFT JOIN vibration_events v ON d.device_id = v.device_id
GROUP BY d.device_id, d.device_name;
```

### 8. Xóa dữ liệu test (Cẩn thận!)

```sql
-- Xóa tất cả events (NGUY HIỂM)
DELETE FROM vibration_events;

-- Xóa events cũ hơn 30 ngày
DELETE FROM vibration_events
WHERE timestamp < NOW() - INTERVAL '30 days';

-- Xóa events từ device test cụ thể
DELETE FROM vibration_events
WHERE device_id = 'test_device_001';
```

---

## 🔍 Kiểm tra Database Health

### 1. Kiểm tra kích thước database

```sql
SELECT pg_size_pretty(pg_database_size('vibraguard_db')) as database_size;
```

### 2. Kiểm tra kích thước bảng

```sql
SELECT
    table_name,
    pg_size_pretty(pg_total_relation_size(quote_ident(table_name))) as size
FROM information_schema.tables
WHERE table_schema = 'public'
ORDER BY pg_total_relation_size(quote_ident(table_name)) DESC;
```

### 3. Xem các connections hiện tại

```sql
SELECT * FROM pg_stat_activity
WHERE datname = 'vibraguard_db';
```

### 4. Kiểm tra indexes

```sql
SELECT
    tablename,
    indexname,
    indexdef
FROM pg_indexes
WHERE schemaname = 'public'
ORDER BY tablename, indexname;
```

---

## 🛠️ Backup & Restore

### Backup Database (từ GCP VM)

```bash
# SSH vào VM
gcloud compute ssh vibraguard-server --zone=asia-southeast1-b

# Backup
docker exec postgres pg_dump -U vibraguard_user vibraguard_db > backup_$(date +%Y%m%d).sql

# Download về máy local
gcloud compute scp vibraguard-server:~/backup_*.sql ./ --zone=asia-southeast1-b
```

### Restore Database

```bash
# Upload file backup lên VM
gcloud compute scp backup_20250131.sql vibraguard-server:~/ --zone=asia-southeast1-b

# SSH vào VM
gcloud compute ssh vibraguard-server --zone=asia-southeast1-b

# Restore
docker exec -i postgres psql -U vibraguard_user vibraguard_db < backup_20250131.sql
```

---

## 📋 Schema Example (Dự kiến)

```sql
-- Bảng vibration_events
CREATE TABLE IF NOT EXISTS vibration_events (
    id BIGSERIAL PRIMARY KEY,
    device_id VARCHAR(255) NOT NULL,
    timestamp TIMESTAMP NOT NULL DEFAULT NOW(),
    vibration_type VARCHAR(50),
    severity VARCHAR(20),
    ai_confidence DECIMAL(5,2),
    ax DECIMAL(10,4),
    ay DECIMAL(10,4),
    az DECIMAL(10,4),
    gx DECIMAL(10,4),
    gy DECIMAL(10,4),
    gz DECIMAL(10,4),
    temperature DECIMAL(5,2),
    processed BOOLEAN DEFAULT FALSE,
    created_at TIMESTAMP DEFAULT NOW()
);

-- Index cho tìm kiếm nhanh
CREATE INDEX idx_device_timestamp ON vibration_events(device_id, timestamp DESC);
CREATE INDEX idx_severity ON vibration_events(severity);
CREATE INDEX idx_timestamp ON vibration_events(timestamp DESC);
```

---

## 🚨 Troubleshooting

### Không kết nối được từ máy local

```bash
# 1. Kiểm tra firewall rule đã tạo chưa
gcloud compute firewall-rules list | grep postgres

# 2. Kiểm tra PostgreSQL container có chạy không
gcloud compute ssh vibraguard-server --zone=asia-southeast1-b
docker ps | grep postgres

# 3. Kiểm tra logs
docker logs postgres
```

### Database quá chậm

```sql
-- Chạy VACUUM để tối ưu
VACUUM ANALYZE vibration_events;

-- Reindex
REINDEX TABLE vibration_events;
```

### Quên password

```bash
# Check trong docker-compose.yml trên VM
gcloud compute ssh vibraguard-server --zone=asia-southeast1-b
cat docker-compose.yml | grep POSTGRES
```

---

## 📌 Quick Reference

| Command         | Description               |
| --------------- | ------------------------- |
| `\l`            | List all databases        |
| `\dt`           | List all tables           |
| `\d table_name` | Describe table structure  |
| `\du`           | List all users            |
| `\q`            | Quit psql                 |
| `\?`            | Help                      |
| `\timing`       | Show query execution time |

---

## 🎯 Next Steps

1. **Kết nối vào database**: Chọn 1 trong 3 cách trên
2. **Kiểm tra schema**: Dùng `\dt` và `\d vibration_events`
3. **Test insert data**: Thử gửi data từ ESP32 hoặc API
4. **Xem data**: Dùng các SQL queries ở trên
5. **Setup monitoring**: Theo dõi kích thước DB và performance

---

_Last updated: 2025-01-31_
