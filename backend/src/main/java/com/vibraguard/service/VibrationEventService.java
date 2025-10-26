package com.vibraguard.service;

import com.vibraguard.dto.VibrationEventDTO;
import com.vibraguard.model.Device;
import com.vibraguard.model.VibrationEvent;
import com.vibraguard.repository.DeviceRepository;
import com.vibraguard.repository.VibrationEventRepository;
import lombok.RequiredArgsConstructor;
import lombok.extern.slf4j.Slf4j;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import java.util.List;
import java.util.stream.Collectors;

@Service
@RequiredArgsConstructor
@Slf4j
public class VibrationEventService {

    private final VibrationEventRepository eventRepository;
    private final DeviceRepository deviceRepository;

    @Transactional(readOnly = true)
    public List<VibrationEventDTO> getAllEvents() {
        log.info("Fetching all vibration events");
        List<VibrationEvent> events = eventRepository.findTop100ByOrderByEventTimestampDesc();
        return events.stream()
                .map(this::convertToDTO)
                .collect(Collectors.toList());
    }

    @Transactional(readOnly = true)
    public List<VibrationEventDTO> getEventsByDeviceId(Long deviceId) {
        log.info("Fetching events for device ID: {}", deviceId);
        List<VibrationEvent> events = eventRepository.findByDeviceIdOrderByEventTimestampDesc(deviceId);
        return events.stream()
                .map(this::convertToDTO)
                .collect(Collectors.toList());
    }

    @Transactional
    public VibrationEventDTO createEvent(String deviceId, Integer sensorValue) {
        log.info("Creating event for device: {}", deviceId);

        Device device = deviceRepository.findByDeviceId(deviceId)
                .orElseThrow(() -> new RuntimeException("Device not found: " + deviceId));

        VibrationEvent event = new VibrationEvent();
        event.setDevice(device);
        event.setSensorValue(sensorValue);
        event.setEventTimestamp(java.time.LocalDateTime.now());
        event.setSeverity("WARNING");

        VibrationEvent savedEvent = eventRepository.save(event);
        log.info("Event created with ID: {}", savedEvent.getId());

        return convertToDTO(savedEvent);
    }

    private VibrationEventDTO convertToDTO(VibrationEvent event) {
        VibrationEventDTO dto = new VibrationEventDTO();
        dto.setId(event.getId());
        dto.setDeviceId(event.getDevice().getDeviceId());
        dto.setDeviceName(event.getDevice().getDeviceName());
        dto.setLocation(event.getDevice().getLocation());
        dto.setEventTimestamp(event.getEventTimestamp());
        dto.setSensorValue(event.getSensorValue());
        dto.setSeverity(event.getSeverity());
        dto.setNotes(event.getNotes());
        return dto;
    }
}
