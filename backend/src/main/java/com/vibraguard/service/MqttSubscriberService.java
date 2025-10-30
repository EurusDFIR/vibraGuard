package com.vibraguard.service;

import com.fasterxml.jackson.databind.ObjectMapper;
import com.vibraguard.dto.SensorDataDTO;
import com.vibraguard.model.Device;
import com.vibraguard.model.VibrationEvent;
import com.vibraguard.repository.DeviceRepository;
import com.vibraguard.repository.VibrationEventRepository;
import lombok.RequiredArgsConstructor;
import lombok.extern.slf4j.Slf4j;
import org.springframework.integration.annotation.ServiceActivator;
import org.springframework.messaging.Message;
import org.springframework.messaging.simp.SimpMessagingTemplate;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import java.time.LocalDateTime;

@Service
@RequiredArgsConstructor
@Slf4j
public class MqttSubscriberService {

    private final VibrationEventRepository eventRepository;
    private final DeviceRepository deviceRepository;
    private final ObjectMapper objectMapper;
    private final SimpMessagingTemplate messagingTemplate; // WebSocket để gửi alerts

    @ServiceActivator(inputChannel = "mqttInputChannel")
    @Transactional
    public void handleSensorMessage(Message<?> message) {
        try {
            String payload = message.getPayload().toString();
            log.info("📨 Received MQTT message: {}", payload);

            // Parse JSON payload
            SensorDataDTO sensorData = objectMapper.readValue(payload, SensorDataDTO.class);
            log.info("📊 Parsed sensor data: deviceId={}, timestamp={}",
                    sensorData.getDeviceId(), sensorData.getTimestamp());

            // Tìm hoặc tạo thiết bị
            Device device = deviceRepository.findByDeviceId(sensorData.getDeviceId())
                    .orElseGet(() -> {
                        log.warn("⚠️ Device not found: {}. Creating new device.", sensorData.getDeviceId());
                        Device newDevice = new Device();
                        newDevice.setDeviceId(sensorData.getDeviceId());
                        newDevice.setDeviceName("Auto-created: " + sensorData.getDeviceId());
                        newDevice.setLocation("Unknown");
                        newDevice.setStatus("ACTIVE");
                        return deviceRepository.save(newDevice);
                    });

            // Tạo sự kiện rung động
            VibrationEvent event = new VibrationEvent();
            event.setDevice(device);
            event.setSensorValue(sensorData.getSensorValue() != null ? sensorData.getSensorValue() : 1);

            // ⏰ Sử dụng server timestamp thay vì ESP32 timestamp
            // (ESP32 không có RTC, chỉ gửi millis() - uptime since boot)
            event.setEventTimestamp(LocalDateTime.now());

            event.setSeverity("WARNING");
            event.setNotes("AI Attack Detected - Confidence: " +
                    (sensorData.getConfidence() != null ? String.format("%.1f%%", sensorData.getConfidence() * 100)
                            : "N/A"));

            VibrationEvent savedEvent = eventRepository.save(event);
            log.info("✅ Vibration event saved: ID={}, Device={}, Time={}",
                    savedEvent.getId(),
                    device.getDeviceId(),
                    savedEvent.getEventTimestamp());

            // **GỬI ALERT QUA WEBSOCKET** đến tất cả client đang kết nối
            sendWebSocketAlert(sensorData, savedEvent);

        } catch (Exception e) {
            log.error("❌ Error processing MQTT message: {}", e.getMessage(), e);
        }
    }

    /**
     * Gửi thông báo real-time qua WebSocket đến tất cả frontend clients
     * Topic: /topic/alerts
     */
    private void sendWebSocketAlert(SensorDataDTO sensorData, VibrationEvent event) {
        try {
            // Tạo alert message với thông tin chi tiết
            var alertMessage = new java.util.HashMap<String, Object>();
            alertMessage.put("eventId", event.getId());
            alertMessage.put("deviceId", sensorData.getDeviceId());
            alertMessage.put("timestamp", event.getEventTimestamp().toString());
            alertMessage.put("severity", event.getSeverity());
            alertMessage.put("sensorValue", event.getSensorValue());

            // Thêm thông tin AI nếu có
            if (sensorData.getConfidence() != null) {
                alertMessage.put("confidence", sensorData.getConfidence());
            }
            if (sensorData.getAiTriggered() != null) {
                alertMessage.put("aiTriggered", sensorData.getAiTriggered());
            }

            // Gửi đến topic /topic/alerts
            messagingTemplate.convertAndSend("/topic/alerts", alertMessage);
            log.info("📡 WebSocket alert sent to /topic/alerts: deviceId={}", sensorData.getDeviceId());

        } catch (Exception e) {
            log.error("❌ Failed to send WebSocket alert: {}", e.getMessage(), e);
        }
    }
}
