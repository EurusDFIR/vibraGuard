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
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import java.time.Instant;
import java.time.LocalDateTime;
import java.time.ZoneId;

@Service
@RequiredArgsConstructor
@Slf4j
public class MqttSubscriberService {

    private final VibrationEventRepository eventRepository;
    private final DeviceRepository deviceRepository;
    private final ObjectMapper objectMapper;

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

            // Convert timestamp từ millis sang LocalDateTime
            if (sensorData.getTimestamp() != null) {
                LocalDateTime timestamp = LocalDateTime.ofInstant(
                        Instant.ofEpochMilli(sensorData.getTimestamp()),
                        ZoneId.systemDefault());
                event.setEventTimestamp(timestamp);
            } else {
                event.setEventTimestamp(LocalDateTime.now());
            }

            event.setSeverity("WARNING");
            event.setNotes("Vibration detected via MQTT");

            VibrationEvent savedEvent = eventRepository.save(event);
            log.info("✅ Vibration event saved: ID={}, Device={}, Time={}",
                    savedEvent.getId(),
                    device.getDeviceId(),
                    savedEvent.getEventTimestamp());

        } catch (Exception e) {
            log.error("❌ Error processing MQTT message: {}", e.getMessage(), e);
        }
    }
}
