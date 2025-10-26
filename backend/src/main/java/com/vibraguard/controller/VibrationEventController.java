package com.vibraguard.controller;

import com.vibraguard.dto.VibrationEventDTO;
import com.vibraguard.service.VibrationEventService;
import lombok.RequiredArgsConstructor;
import lombok.extern.slf4j.Slf4j;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;

import java.util.List;

@RestController
@RequestMapping("/api/events")
@RequiredArgsConstructor
@Slf4j
@CrossOrigin(origins = "*") // Cho phép CORS từ frontend
public class VibrationEventController {

    private final VibrationEventService eventService;

    /**
     * GET /api/events
     * Lấy danh sách tất cả các sự kiện rung động
     */
    @GetMapping
    public ResponseEntity<List<VibrationEventDTO>> getAllEvents() {
        log.info("GET /api/events - Fetching all events");
        List<VibrationEventDTO> events = eventService.getAllEvents();
        return ResponseEntity.ok(events);
    }

    /**
     * GET /api/events/device/{deviceId}
     * Lấy các sự kiện của một thiết bị cụ thể
     */
    @GetMapping("/device/{deviceId}")
    public ResponseEntity<List<VibrationEventDTO>> getEventsByDevice(@PathVariable Long deviceId) {
        log.info("GET /api/events/device/{} - Fetching events for device", deviceId);
        List<VibrationEventDTO> events = eventService.getEventsByDeviceId(deviceId);
        return ResponseEntity.ok(events);
    }
}
