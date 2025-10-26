-- Script khởi tạo dữ liệu mẫu cho VibraGuard
-- Chạy script này sau khi Spring Boot đã tạo các bảng

-- Thêm thiết bị mẫu
INSERT INTO devices (device_id, device_name, location, status, created_at, updated_at)
VALUES 
    ('ESP32_CUA_SO_01', 'Cảm biến cửa sổ phòng khách', 'Phòng khách - Cửa sổ chính', 'ACTIVE', NOW(), NOW()),
    ('ESP32_CUA_CHINH', 'Cảm biến cửa chính', 'Cửa ra vào chính', 'ACTIVE', NOW(), NOW()),
    ('ESP32_CUA_SAU', 'Cảm biến cửa sau', 'Cửa sau nhà bếp', 'ACTIVE', NOW(), NOW())
ON CONFLICT (device_id) DO NOTHING;

-- Note: Các sự kiện sẽ được tạo tự động từ ESP32 và MQTT
