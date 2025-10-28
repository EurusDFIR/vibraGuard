package com.vibraguard.controller;

import com.vibraguard.service.ControlService;
import lombok.RequiredArgsConstructor;
import lombok.extern.slf4j.Slf4j;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;

import java.util.HashMap;
import java.util.Map;

/**
 * Controller xử lý các lệnh điều khiển hệ thống
 * - ARM: Kích hoạt báo động
 * - DISARM: Tắt báo động
 * - STATUS: Kiểm tra trạng thái hiện tại
 */
@RestController
@RequestMapping("/api/control")
@RequiredArgsConstructor
@Slf4j
@CrossOrigin(origins = "*")
public class ControlController {

    private final ControlService controlService;

    /**
     * POST /api/control/arm
     * Gửi lệnh ARM đến tất cả thiết bị ESP32
     */
    @PostMapping("/arm")
    public ResponseEntity<Map<String, Object>> armSystem(@RequestParam(required = false) String deviceId) {
        log.info("🔔 ARM command received");

        try {
            if (deviceId != null && !deviceId.isEmpty()) {
                controlService.sendArmCommand(deviceId);
            } else {
                controlService.sendArmCommandToAll();
            }

            Map<String, Object> response = new HashMap<>();
            response.put("status", "success");
            response.put("message", "System armed successfully");
            response.put("systemStatus", "ARMED");
            response.put("timestamp", System.currentTimeMillis());

            return ResponseEntity.ok(response);
        } catch (Exception e) {
            log.error("❌ Error arming system: {}", e.getMessage());
            Map<String, Object> response = new HashMap<>();
            response.put("status", "error");
            response.put("message", e.getMessage());
            return ResponseEntity.internalServerError().body(response);
        }
    }

    /**
     * POST /api/control/disarm
     * Gửi lệnh DISARM đến tất cả thiết bị ESP32 (tắt còi)
     */
    @PostMapping("/disarm")
    public ResponseEntity<Map<String, Object>> disarmSystem(@RequestParam(required = false) String deviceId) {
        log.info("🔕 DISARM command received");

        try {
            if (deviceId != null && !deviceId.isEmpty()) {
                controlService.sendDisarmCommand(deviceId);
            } else {
                controlService.sendDisarmCommandToAll();
            }

            Map<String, Object> response = new HashMap<>();
            response.put("status", "success");
            response.put("message", "System disarmed successfully");
            response.put("systemStatus", "DISARMED");
            response.put("timestamp", System.currentTimeMillis());

            return ResponseEntity.ok(response);
        } catch (Exception e) {
            log.error("❌ Error disarming system: {}", e.getMessage());
            Map<String, Object> response = new HashMap<>();
            response.put("status", "error");
            response.put("message", e.getMessage());
            return ResponseEntity.internalServerError().body(response);
        }
    }

    /**
     * POST /api/control/sound_alarm
     * Bật còi báo động thủ công (Panic Button)
     */
    @PostMapping("/sound_alarm")
    public ResponseEntity<Map<String, Object>> soundAlarm(@RequestParam(required = false) String deviceId) {
        log.info("🚨 SOUND_ALARM command received (Manual Panic Button)");

        try {
            if (deviceId != null && !deviceId.isEmpty()) {
                controlService.sendSoundAlarmCommand(deviceId);
            } else {
                controlService.sendSoundAlarmCommandToAll();
            }

            Map<String, Object> response = new HashMap<>();
            response.put("status", "success");
            response.put("message", "Manual alarm activated successfully");
            response.put("timestamp", System.currentTimeMillis());

            return ResponseEntity.ok(response);
        } catch (Exception e) {
            log.error("❌ Error activating manual alarm: {}", e.getMessage());
            Map<String, Object> response = new HashMap<>();
            response.put("status", "error");
            response.put("message", e.getMessage());
            return ResponseEntity.internalServerError().body(response);
        }
    }

    /**
     * GET /api/control/status
     * Lấy trạng thái hiện tại của hệ thống
     */
    @GetMapping("/status")
    public ResponseEntity<Map<String, Object>> getSystemStatus() {
        log.info("📊 System status requested");

        Map<String, Object> response = new HashMap<>();
        response.put("systemStatus", controlService.getSystemStatus());
        response.put("timestamp", System.currentTimeMillis());
        response.put("devicesCount", controlService.getActiveDevicesCount());

        return ResponseEntity.ok(response);
    }
}
