# ⚡ Quick DBeaver Setup - 3 Phút

## ✅ Trạng thái Hệ thống

**Firewall:** ✅ Port 5432 đã mở (`allow-postgres-external`)  
**PostgreSQL:** ✅ Đang chạy trên GCP VM  
**Database:** ✅ `vibraguard_db` có 25 events  
**Bạn cần làm:** Chỉ kết nối DBeaver (không cần SSH hay setup gì thêm!)

---

## ��� Bước 1: Mở DBeaver

1. Tải và cài DBeaver: https://dbeaver.io/download/
2. Mở DBeaver
3. Click **Database** → **New Database Connection** (Ctrl+Shift+N)
4. Chọn **PostgreSQL** → Click **Next**

---

## ��� Bước 2: Nhập Thông tin

**Main Tab:**
```
Host:     34.87.133.103
Port:     5432
Database: vibraguard_db
Username: vibraguard_user
Password: vibraguard_pass
```

**SSL Tab:**
```
SSL Mode: disable
```

---

## ��� Bước 3: Test & Connect

1. Click **Test Connection...**
2. Nếu thấy "Connected" → Click **Finish**
3. Done! ✅

---

## ��� Truy vấn Đầu tiên

Mở SQL Editor và chạy:

```sql
-- Xem tất cả attack events
SELECT 
    id,
    event_timestamp,
    notes,
    severity
FROM vibration_events
ORDER BY event_timestamp DESC
LIMIT 10;
```

**Kết quả mong đợi:**
```
id | event_timestamp         | notes                                  | severity
---+-------------------------+----------------------------------------+----------
25 | 2025-10-31 15:02:51.729 | AI Attack Detected - Confidence: 85.2% | WARNING
24 | 2025-10-31 14:39:17.374 | AI Attack Detected - Confidence: 99.6% | WARNING
```

---

## ��� Nếu Lỗi

**Lỗi: "Connection refused"**
```bash
# SSH vào VM kiểm tra PostgreSQL
gcloud compute ssh vibraguard-server --zone=asia-southeast1-b \
  --command="docker ps | grep postgres"

# Nếu không thấy container, restart:
gcloud compute ssh vibraguard-server --zone=asia-southeast1-b \
  --command="cd ~/vibraguard-prod && docker-compose restart postgres"
```

**Lỗi: "Authentication failed"**
- Double-check username: `vibraguard_user` (KHÔNG phải `myuser`)
- Double-check password: `vibraguard_pass` (KHÔNG phải `mypassword`)

---

## ��� Xem Thêm

- **Full Guide:** `DBEAVER_CONNECTION_GUIDE.md` (19 truy vấn SQL)
- **Test Script:** `python test_db_connection.py` (cần cài `psycopg2-binary`)

---

**Thời gian:** < 3 phút  
**Verified:** 2025-11-01  
**Status:** ✅ Ready to use
